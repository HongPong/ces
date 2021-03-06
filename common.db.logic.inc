<?php
/**
 * @ingroup ces.common.db
 * @{
 */

/**
 * @file
 * This file contains the "active record" (approximately) pattern for loading
 * and saving objects.
 */

class Serializer {

  protected $className;
  protected $table;
  /**
   * cache array. Srtucture:
   *  class name
   *    unique key
   *      value => record
   */
  static protected $cache = array();

  public function __construct($className) {
    $this->className = $className;
    $class = new ReflectionClass($this->className);
    $this->table = $class->getStaticPropertyValue('table');
  }

  public function load($id) {
    if (!isset(Serializer::$cache[$this->className])) $cache[$this->className] = array();
    if (!isset(Serializer::$cache[$this->className]['id'])) $cache[$this->className]['id'] = array();
    if (!isset(Serializer::$cache[$this->className]['id'][$id])) {
      $record = db_query('SELECT * FROM {' . $this->table . '} WHERE id = :id', array(':id' => $id))->fetchAssoc();
      if ($record === FALSE)
        return FALSE;
      Serializer::$cache[$this->className]['id'][$id] = $this->loadFromRecord($record);
    }
    return Serializer::$cache[$this->className]['id'][$id];
  }
  /**
   * @return int
   */
  public function count($key, $value) {
    $query = db_select($this->table, 't')->condition($key, $value)->countQuery();
    $result = $query->execute()->fetchField();
    return $result;
  }
  /**
   * @return DBObject
   */
  public function loadFromRecord($record) {
    $this->handleSerialized($record);
    if (isset($record['classname']) && class_exists($record['classname']) && is_subclass_of($record['classname'], $this->className)) {
      $obj = new $record['classname']($record);
    }
    else {
      $obj = new $this->className($record);
    }
    if (!empty($record['decoration']) && is_array($record['decoration'])) {//load decorator pattern
      foreach ($record['decoration'] as $value) {
        if (isset($value['classname']) && class_exists($value['classname'])) {
          $obj = new $value['classname']($obj, $value['data']);
        }
      }
    }
    return $obj;
  }

  public function save(DBObject &$object) {
    $record = $object->getRecord();
    //Call save extra data to all implied objects.
    $auxobject = $object; //Don't loose the main reference!
    $inner = NULL;
    do{
      $auxobject->saveExtraData();
      $inner = $auxobject;
      $auxobject = $auxobject->getParent();
    } while ($auxobject != NULL && ($auxobject instanceof DBObject));
    //Decide between insert and update, and automatically populate temporal fields.
    $record['modified'] = REQUEST_TIME;
    if (empty($record['id'])) {
      $record['created'] = REQUEST_TIME;
      drupal_write_record($this->table, $record);
      //Update the inserted object with the id parameter.
      if (isset($record['id'])) {
        $inner->id = $record['id'];
      }
    }
    else {
      drupal_write_record($this->table, $record, 'id');
    }
    $this->deleteRecordFromCache($record);
  }
  /**
   * Updates a single database field from a single row.
   */
  public function updateField(DBObject &$object, $key) {
    $id = $object->id;
    $value = $object->{$key};
    $this->deleteCache();
    return db_update($this->table)->condition('id', $id)->fields(array($key => $value))->execute();
  }
  /**
   * Performs a SQL function such as COUNT(*), SUM(*), ... on a table field for
   * those rows meeting conditions.
   * @param $key must be SQL safe.
   * @param $conditions array Each entry is a condition. A condition is an array
   * of three therms
   */
  public function selectFunctionField($function, $key, array $conditions) {
    $functions = array('COUNT', 'SUM');
    if (!in_array($function, $functions))
      throw new Exception('Invalid database function.');
    $query = db_select($this->table, 't');
    $query->addExpression($function . '(' . $key . ')');
    foreach ($conditions as $condition) {
      if (count($condition) == 3)
        $query->condition($condition[0], $condition[1], $condition[2]);
      elseif (count($condition) == 2)
        $query->condition($condition[0], $condition[1]);
    }
    return $query->execute()->fetchField();
  }
  /**
   * @param $conditions array
   * Associative array with field names as keys. There are two special conditions
   * for those objects having the created [time] field:
   * - createdsince: the smaller created time to return.
   * - createduntil: the largest created time to return.
   * @TODO: remove these two exceptions and change the conditions to be an array
   * of three elements: (key, value, $cmp='=').
   * @param $order string
   * A field to order by.
   * @return array Objects with attribute $key = $value ordered by $order field.
   */
  public function loadCollection(array $conditions, $order = NULL) {
    if (!db_table_exists($this->table)) {
      // This prevents a fatal error in case CES table is gone.
      drupal_set_message(t('WARNING: CES table @table is missing. You probably need to reinstall CES.'), array('@table' => $this->table));
      return FALSE;
    }
    $query = db_select($this->table, 't')->fields('t');
    if (isset($conditions['createdsince'])) {
      $query = $query->condition('created', $conditions['createdsince'], '>=');
      unset($conditions['createdsince']);
    }
    if (isset($conditions['createduntil'])) {
      $query = $query->condition('created', $conditions['createduntil'], '<=');
      unset($conditions['createduntil']);
    }
    if (isset($conditions['limit'])) {
      $query->range(0, $conditions['limit']);
      unset($conditions['limit']);
    }
    foreach ($conditions as $key => $value) {
      $query = $query->condition($key, $value);
    }
    if ($order != NULL) {
      $query->orderBy($order);
    }
    
    $result = $query->execute();
    $loads = array();
    while (($record = $result->fetchAssoc()) !== FALSE) {
      $loads[] = $this->loadFromRecord($record);
    }
    return $loads;
  }
  /**
   * @return DBObject
   */
  public function loadFromUniqueKey($key, $value) {
    if (!isset(Serializer::$cache[$this->className])) $cache[$this->className] = array();
    if (!isset(Serializer::$cache[$this->className][$key])) $cache[$this->className][$key] = array();
    if (!isset(Serializer::$cache[$this->className][$key][$value])) {
      $query = db_select($this->table, 't')->fields('t')->condition($key, $value)->range(0, 1);
      $result = $query->execute();
      $record = $result->fetchAssoc();
      if ($record === FALSE)
        return FALSE;
      Serializer::$cache[$this->className][$key][$value] = $this->loadFromRecord($record);
    }
    return Serializer::$cache[$this->className][$key][$value];
  }
  /**
   * Deletes a database record
   * @param id int the record id.
   */
  public function delete(DBObject $object) {
    $object->deleteExtraData();
    $this->deleteCache();
    $record = $object->getRecord();
    $query = db_delete($this->table)->condition('id', $record['id']);
    return $query->execute();
  }
  /**
   * @return object
   */
  public static function DBTransaction() {
    return db_transaction();
  }
  private function handleSerialized(array &$record) {
    //It would be nice if it was done automatically by Drupal, but...
    $serialized = array('data', 'decoration');
    foreach ($serialized as $field) {
      if (isset($record[$field]) && is_string($record[$field])) {
        $record[$field] = unserialize($record[$field]);
      }
    }
  }
  private function deleteRecordFromCache(array $record) {
    if (isset(Serializer::$cache[$this->className])) {
      foreach (Serializer::$cache[$this->className] as $key => $value) {
        if (isset($record[$key]) && isset($value[$record[$key]])) {
          unset(Serializer::$cache[$this->className][$key][$record[$key]]);
        }
      }
    }
  }
  private function deleteCache() {
    Serializer::$cache[$this->className] = array();
  }
}

class LockSerializer extends Serializer{
  public function loadCollection(array $conditions, $order = NULL) {
    throw new Exception('Not implemented');
  }
  public function loadFromUniqueKey($key, $value) {
    throw new Exception('Not implemented');
  }
  /**
   * Load object and acquires lock
   */
  public function load($id) {
    $lockname = $this->table . ' ' . $id;
    $times = 0;
    while (!lock_acquire($lockname)) {
      $times++;
      if ($times > 5)
        throw new Exception(t('Impossible to get the database lock %lock', array('%lock' => $lockname)));
      lock_wait($lockname);
    }
    return parent::load($id);
  }
  /**
   * Saves object and releases lock
   */
  public function save(DBObject $object) {
    $res = parent::save($object);
    lock_release($this->table . ' ' . $object->id);
    return $res;
  }
  /**
   * updates field and releases lock
   */
  public function updateField(DBObject $object, $key) {
    $res = parent::updateField($object, $key);
    lock_release($this->table . ' ' . $object->id);
    return $res;
  }
}

abstract class DBObject{
  public static $table;
  public function getRecord() {
    //Handle decorator pattern.
    $object = $this;
    $parent = $object->getParent();
    $decoration = array();
    if ($parent != NULL && ($parent instanceof DBObject)) {
      do {
        $decoration[] = array(
                          'classname' => get_class($object),
                          'data' => $object->getSimpleRecord(),
                        );
        $object = $parent;
        $parent = $object->getParent();
      } while ($parent != NULL && ($parent instanceof DBObject));
      array_reverse($decoration);
    }
    //Get record.
    $record = $object->getSimpleRecord();
    $record['decoration'] = $decoration;
    return $record;
  }
  /**
   * @returns The fields directly hanging from this object.
   */
  private function getSimpleRecord() {
    $record = get_object_vars($this);
    foreach ($record as $key => $value) {
      if ($value === NULL) {
        unset($record[$key]);
      }
    }
    return $record;
  }
  protected function loadRecord($record) {
    $reflect = new ReflectionObject($this);
    foreach ($record as $key => $value) {
      if ($reflect->hasProperty($key) && $reflect->getProperty($key)->isPublic()) {
        $this->{$key} = $value;
      }
    }
  }
  /**
   * If the overrider has other DBObject objects as class members override
   * this function to save them.
   */
  public function saveExtraData() {
  }
  public function deleteExtraData() {
  }
  public function __construct($record) {
    $this->loadRecord($record);
  }
  public function getParent() {
    return NULL;
  }
}

interface DBTransaction {
  public function rollback();
}

/**
 * @} End of "ingroup ces.common.db".
 */