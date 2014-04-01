<?php

/**
 * @file
 * Functions from parse announces
 */

/**
 * Parse setting
 */

function parse_announce($data, $row) {

  $exchange_id = $GLOBALS['exchange_id'];

  $extra_info = array(
    'ID'         => $data['ID'],
    'DateAdded'  => $data['DateAdded'],
    'DateEvent'  => $data['DateEvent'],
    'DateExpiry' => $data['DateExpiry'],
    'Keep'       => $data['Keep'],
  );

  if ( substr($data['Owner'],-4) == '0000' ) {
    $announce_user_id = $GLOBALS['user']->uid;
  } else {
    // Find uid from user
    $query = db_query('SELECT uid FROM {users} where name=:name',array(':name' => $data['Owner']));
    $announce_user_id = $query->fetchColumn(0);
  }

  if ( empty($announce_user_id) ) {
    $m  =  t("No user was found")." [".$data['Owner']."] in row [$row]";
    $e  = "$m\n\n".t("It may be a user of other banks have not yet imported, incidence is saved.");
    $m .= "\n".implode($data,',');  
    add_observation($m);
    error_i4c($e);
    return ;
    }


  // Create a blog post
  $node = new stdClass();
  $node->title = $data['Title'];
  $node->type = 'ces_blog';
  node_object_prepare($node);
  $node->language = LANGUAGE_NONE;
  $node->uid = $announce_user_id;
  $node->status = 1;
  $node->promote = 0;
  $node->comment = 2;
  $node->ces_blog_exchange[LANGUAGE_NONE][0]['value'] = $exchange_id;
  $node->body[LANGUAGE_NONE][0] = array(
    'summary' => '',
    'value' => $data['Description'],
    'format' => 'filtered_html',
  );
  $node = node_submit($node);

  node_save($node);

  $nid = db_insert('ces_import4ces_objects')
    ->fields(array(
      'import_id' => $GLOBALS['import_id'],
      'object' => 'announces',
      'object_id' => $node->nid,
      'row' => $row,
      'data' => serialize($extra_info)
    ))->execute();
  return $nid;

}

?>
