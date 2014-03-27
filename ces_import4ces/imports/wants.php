<?php

/**
 * @file
 * Functions from parse wants
 */

/**
 * Parse setting
 */

function parse_wants($data, $row) {

  $exchange_id = $GLOBALS['exchange_id'];

  // echo '<pre>data: ' ; print_r($data) ; echo '</pre>'; exit() ; // DEV  

  /*

    [ID] => 2
    [UID] => HORA0024
    [Keep] => 0
    [DateAdded] => 2011/05/14 23:47:45
    [Title] => Classes d'harmonia (musical, de moment)
    [Description] => Busquem qui ens pugui donar classes d'harmonia.

  */

  $want = array(
    'type'       => 'want',
    'user'       => $data['UID'],
    'title'      => $data['Title'],
    'body'       => $data['Description'],
    // 'category'   => $category_id,
    'keywords'   => '',
    'state'      => 1,
    'created'    => strtotime($data['DateAdded']),
    'modified'   => time(),
    'expire'     => ( strtotime($data['DateAdded']) + ( 60 * 60 * 24 * 365 ) ), // Add 1 year more of DateAdded (need something)
    // 'rate'       => $data['Rate'],
    // 'image'      => $data['Image'],
  );

  $extra_info = array(
    'ID'      => $data['ID'],
    'UID'     => $data['UID'],
  );

  if ( substr($data['UID'],-4) == '0000' ) {
    $want_user_id = $GLOBALS['user']->uid;
  } else {
    // Find uid from user
    $query = db_query('SELECT uid FROM {users} where name=:name',array(':name' => $data['UID']));
    $want_user_id = $query->fetchColumn(0);
  }

  if ( empty($want_user_id) ) {
    error_i4c(t("Error: No user was found [".$data['UID']."]")." $row");
    return FALSE;
    }

  $want['user'] = $want_user_id;

  $o = (object) $want;
  $want = ces_offerwant_save($o);

  if ( $want ) {
    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $GLOBALS['import_id'],
        'object' => 'wants',
        'object_id' => $want->id,
        'row' => $row,
        'data' => serialize($extra_info)
      ))->execute();
    return $nid;
  }


  return FALSE ;

}

?>
