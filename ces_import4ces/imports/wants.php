<?php
/**
 * @file
 * Functions from parse wants
 */

/**
 * @defgroup ces_import4ces_wants Parse wants from CES
 * @ingroup ces_import4ces
 * @{
 * Functions from parse wants
 */

/**
 * Parse wants.
 */
function ces_import4ces_parse_wants($import_id, $data, $row, &$context) {
  if (isset($context['results']['error'])) {
    return;
  }
  $tx = db_transaction();
  try {
    $context['results']['import_id'] = $import_id;
    $import = ces_import4ces_import_load($import_id);
    $import->row = $row;

    $category_id = ces_import4ces_get_category('unclassified', $import);

    /*
    [ID] => 2
    [UID] => HORA0024
    [Keep] => 0
    [DateAdded] => 2011/05/14 23:47:45
    [Title] => Classes d'harmonia (musical, de moment)
    [Description] => Busquem qui ens pugui donar classes d'harmonia.
    */

    $want = array(
      'type' => 'want',
      'user' => $data['UID'],
      'title' => $data['Title'],
      'body' => $data['Description'],
      'category' => $category_id,
      'keywords' => '',
      'state' => 1,
      'created' => strtotime($data['DateAdded']),
      'modified' => strtotime($data['DateAdded']),
      // Add 150 days more of DateAdded (need something).
      'expire' => (strtotime($data['DateAdded']) + (60 * 60 * 24 * 150)),
      // 'rate'       => $data['Rate'],
      // 'image'      => $data['Image'],
    );

    $extra_info = array(
      'ID' => $data['ID'],
      'UID' => $data['UID'],
    );

    // Find uid from user.
    $query = db_query('SELECT uid FROM {users} where name=:name', array(':name' => $data['UID']));
    $want_user_id = $query->fetchColumn(0);

    if (empty($want_user_id)) {
      $m = t('The user @user was not found in want import row @row. It may be a
      user from another exchange not yet imported.', array('@user' => $data['UID'], '@row' => $row));

      throw new Exception($m);
    }

    $want['user'] = $want_user_id;

    $o = (object) $want;
    $want = ces_offerwant_save($o);

    if ($want) {
      $nid = db_insert('ces_import4ces_objects')
          ->fields(array(
            'import_id' => $import_id,
            'object' => 'wants',
            'object_id' => $want->id,
            'row' => $row,
            'data' => serialize($extra_info),
          ))->execute();
      return $nid;
    }
  }
  catch (Exception $e) {
    $tx->rollback();
    ces_import4ces_batch_fail_row($import_id, array_keys($data), array_values($data), $row, $context);
    $context['results']['error'] = check_plain($e->getMessage());
  }
}
/** @} */
