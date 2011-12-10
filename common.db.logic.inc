<?php

class Serializer {
    
  protected $className;
  protected $table;
  
  public function __construct($className){
    $this->className = $className;
    $this->table = $className::$table;
  }
  
  public function load($id){
    $record = db_query('SELECT * FROM {'.$this->table.'} WHERE id = :id', array(':id'=>$id))->fetchAssoc();
    if($record === FALSE) return FALSE;
    return $this->loadFromRecord($record);
  }
  
  public function loadFromRecord($record){
    $obj = new $this->className($record);
    if(!empty($record['decoration']) && is_array($record['decoration'])){//load decorator pattern
      foreach($record['decoration'] as $value){
        if(isset($value['classname']) && class_exists($value['classname'])){
          $obj = new $className($obj, $value['data']);
        }
      }
    }
    return $obj;
  }

  public function save(DBObject $object){
    $parent = $object->getParent();
    $decoration = array();
    while($parent != NULL && ($parent instanceof DBObject)){
      $object->saveExtraData();
      $decoration[] = array(
                        'classname'=> get_class($object),
                        'data'     => $object->getRecord()
                      );
      array_reverse($decoration);
      $object = $parent;
      $parent = $object->getParent();
    }
    $object->saveExtraData();
    $record = $object->getRecord();
    $record['decoration'] = $decoration;
    
    $record['modified'] = time();
    if($this->id !== NULL){
      $record['created'] = time();
      drupal_write_record($this->table, $record);
    }else{
      drupal_write_record($this->table, $record, 'id');
    }
  }
  
  /**
   * @return array Objects with attribute $key = $value ordered by $order field.
   */
  public function loadCollection($key, $value, $order = NULL){
    $query = db_select($this->table, 't')->fields('t')->condition($key, $value);
    if($order != NULL){
      $query->orderBy($order);
    }
    $result = $query->execute();
    $loads = array();
    while(($record = $result->fetchAssoc()) !== FALSE ){
      $loads[] = $this->loadFromRecord($record);
    }
    return $loads;
  }
  
  public function loadFromUniqueKey($key, $value){
    $query = db_select($this->table, 't')->fields('t')->condition($key, $value)->range(0,1);
    $result = $query->execute();
    $record = $result->fetchAssoc();
    if($record === FALSE) return FALSE;
    return $this->loadFromRecord($record);
  }
    
}

class LockSerializer extends Serializer{
  public function loadCollection($key, $value){
    throw new Exception('Not implemented');
  }
  public function loadFromUniqueKey($key, $value){
    throw new Exception('Not implemented');
  }
  public function load($id){
    lock_acquire($this->table . ' ' . $id);
    return parent::load($id);
  }
  public function save(DBObject $object){
    $res = parent::save($id);
    lock_release($this->table . ' ' . $object->id);
    return $res;
  }
  
}

abstract class DBObject{
  public static $table;  
  
  public function getRecord(){
    $record = get_object_vars($this);
    return $record;
  }
  
  protected function loadRecord($record){
    $reflect = new ReflectionObject($this);
    foreach($record as $key=>$value){
      if($reflect->hasProperty($key) && $reflect->getProperty($key)->isPublic()){
        $this->{$key} = $value;  
      }
    }
  }
  /**
   * If the overrider has other DBObject objects as class members override
   * this function to save them.
   */
  public function saveExtraData(){}
  
  public function __construct($record){
    $this->loadRecord($record);
  }
  public function getParent(){
    return NULL;
  }
}

