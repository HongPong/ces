<?php

/**
 * @file
 * Functions from parse offers and category
 */

/**
 * Parse setting
 */

function parse_offers($data, $row) {

  $exchange_id = $GLOBALS['exchange_id'];

  $category = $data['Category'];

  if ( $category ) {

    $category_id = return_create_category($category, $row);

    $offer = array(
      'type'       => 'offer',
      'user'       => $data['UID'],
      'title'      => $data['Title'],
      'body'       => $data['Description'],
      'category'   => $category_id,
      'keywords'   => $data['Keys'],
      'state'      => ( ( $data['Hidden'] == 0 ) ? 1 : 0 ),
      'created'    => strtotime($data['DateAdded']),
      'modified'   => time(),
      'expire'     => strtotime($data['DateExpires']),
      'rate'       => $data['Rate'],
      'image'      => $data['Image'],
    );

    $extra_info = array(
      'ID'      => $data['ID'],
      'UID'     => $data['UID'],
      'Remote'  => $data['Remote'],
      'Subcat'  => $data['Subcat'],
      'ConRate' => $data['ConRate'],
    );
  }

  // Find uid from user
  $query = db_query('SELECT uid FROM {users} where name=:name',array(':name' => $data['UID']));
  $offer_user_id = $query->fetchColumn(0);

  if ( empty($offer_user_id) ) {
    error_i4c(t("Error: No user was found [".$data['UID']."]")." $fila");
    return FALSE;
    }

  $offer['user'] = $offer_user_id;

  $o = (object) $offer;
  $o->ces_offer_rate = array(LANGUAGE_NONE => array(array('value' => $offer['rate'])));
  unset($o->rate);
  $offer = ces_offerwant_save($o);

  if (!empty($offer->image) ) {

    $file = 'https://www.community-exchange.org/pics/'.$data['Image'];
    $parts = explode(".", $file); 
    $extension = end($parts);  
    $directory = file_default_scheme() . '://' . variable_get('ces_offerswants_picture_path', 'ces_offerswants_pictures');
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    $name_image = 'picture-' . $offer->id . '-' . REQUEST_TIME . '.' . $extension;
    $destination = file_stream_wrapper_uri_normalize($directory . '/'.$name_image );
    if ( getImageCES($file, $destination) ) {

    $file = new stdClass;
    $file->uid = $offer_user_id;
    $file->filename = $name_image;
    $file->uri = $destination;
    $file->status = 1;
    $file->filemime = image_type_to_mime_type(exif_imagetype($destination));

    file_save($file);


      file_usage_add($file, 'ces_offerswants', 'ces_offerwant', $offer->id);
      $offer->image = $file->fid;
      // Re-save the entity with the new image file id.
      $offer = ces_offerwant_save($offer);
    } else {
      return FALSE;
    } 
  }

  if ( $offer ) {
    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $GLOBALS['import_id'],
        'object' => 'offers',
        'object_id' => $offer->id,
        'row' => $row,
        'data' => serialize($extra_info)
      ))->execute();
    return $nid;
  }


  return FALSE ;

}

/**
 * Delete setting
 * 
 * Delete exchange import
 */

function delete_category($import_id) {

  echo 'Pendiente borrado de categorias '.$import_id;

}

?>
