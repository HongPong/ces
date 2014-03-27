<?php

/**
 * @file
 * functions of ces_import4ces
 */

function procesa_csv($file_csv, $parse, $row=0) {

  $importar = FALSE ;   //< Data from file
  $data_come_from = FALSE ;

  // The data come form
  if ( isset($_POST['row_error']) ) {
    $data_come_from = TRUE ;
    $row = $_POST['row'];
  }

  // Array with data os step
  $status = array(
    'finished' => FALSE
    ,'row' => 0
  );

  if ( ! file_exists($file_csv) ) {

    error_i4c(t("Was not found import file")." [$file_csv] <br />". t("Place the files in the folder sites/default/files/ and try again."));
    return FALSE;
  }

  $fila = -1;
  if (($gestor = fopen($file_csv, "r")) !== FALSE) {
    while (($datos = fgetcsv($gestor, 1000, ",")) !== FALSE) {
      $fila++;
      $numero = count($datos);
      if ( $fila !== 0 && $fila <= $row ) continue;
      foreach( $datos as $key => $val ) {
        $datos[$key] = trim( $datos[$key] );
        //$datos[$key] = iconv('ISO-8859-1', 'UTF-8', $datos[$key]) ;
        $datos[$key] = iconv('Windows-1252', 'UTF-8', $datos[$key]) ;
        if ( $fila == 0 ) {
          $heads[] = $datos[$key] ;
        } else {
          $cols[] = $datos[$key] ;
        }
      }
      if ( $fila !== 0 ) {

        if ( $data_come_from ) {
          $cols = $_POST;
          unset($cols['row']);
          unset($cols['step']);
          unset($cols['import_id']);
          unset($cols['row_error']);
          $status['data_come_from'] = 1;
          $data_come_from = FALSE ;
        } elseif ( count($heads) > count($cols) ) {
          $faltan = count($heads) - count($cols);
          for ($i=0; $i<$faltan;$i++) {
            array_push($cols,"LACK!-0$i");
          }
          error_i4c(t('Mismatch fields, check the form to correct errors'));
          $importar = array_combine($heads,$cols);
          $cols = array();
          createfrom($importar, $fila, 3 , $GLOBALS['import_id']);
          return FALSE;
        } elseif ( count($heads) < count($cols) ) {
          $faltan = count($heads) - count($cols) ;
          for ($i=0; $i<$faltan;$i++) {
            array_push($cols,"");
          }
        } elseif ( count($heads) !== count($cols) ) {
          error_i4c(t('Not match the number of fields, it may have been interpreted in a comma as a field separator.').
            "<br/>".
            t('For example links to maps can generate this problem.').
            "<br/>".
            t('One possible solution is to enclose in double quotes the problematic field.').
            "<br/>".
            t('Please check the csv file to ensure proper export.')
          );
          return FALSE;
        }

        $importar = array_combine($heads,$cols);
        $cols = array();
        $status['row'] = $fila ;
        if ( $parse($importar, $fila) === FALSE ) {
          error_i4c(t("Error on parse row")." $fila");
          return $status;
        }
        update_row($fila);
      }
    }

    // if ( isset($importar) ) { createfrom($importar); }

    fclose($gestor);
  } else {
    return FALSE;
    }

  $status['finished'] = TRUE ;
  return $status ;

}

/**
 * Create a form with data
 */

function createfrom($importar, $row, $step, $import_id) {

?>
      <fieldset class="form_i4c">
         <form action="" method="post">
         <?php foreach ( $importar as $key => $value ) { ?>
         <label for="label_<?php echo $key ?>"><?php echo $key ?></label>
         <?php if ( isset($value) && !empty($value) && strlen($value) > 100 ) { ?>
            <textarea name="<?php echo $key ?>"><?php echo $value ?> id="<?php echo $key ?>"</textarea>
         <?php } else { ?>
            <input type="text" name="<?php echo $key ?>" value="<?php echo $value ?>" id="<?php echo $key ?>">     
         <?php } ?>
         <?php } ?>
         <div style="clear: both;"></div>
         <input type="hidden" name="row"       value="<?php echo $row  ?>"/>
         <input type="hidden" name="step"      value="<?php echo $step ?>"/>
         <input type="hidden" name="import_id" value="<?php echo $import_id ?>"/>
         <input type="submit" name="row_error" value="<?php echo t("Continue") ?>"/>
         <input type="submit" name="row_skip" value="<?php echo t("or skip this record").' (pending)' ?>"/>
         </form>

      </fieldset>
<?php
}

/**
 * Count step
 */
function update_step($step) {
  return db_update('ces_import4ces_exchange')->condition('id', $GLOBALS['import_id'])->fields(array('step' => $step))->execute();
}

/**
 * Count row
 */
function update_row($row) {
  return db_update('ces_import4ces_exchange')->condition('id', $GLOBALS['import_id'])->fields(array('row' => $row))->execute();
}

/**
 * Display error
 */

function error_i4c($msg) {

?>
   <p class="error">
   <?php echo $msg?>
   </p>
<?php
}

/**
 * Create o return category
 */
function return_create_category($title, $row) {

  $query = new EntityFieldQuery();

  $query->entityCondition('entity_type', 'ces_category')
        ->propertyCondition('exchange', $GLOBALS['exchange_id'])
        ->propertyCondition('title', $title);

  $result = $query->execute();

  if (isset($result['ces_category'])) {
    return key($result['ces_category']);
  } else {

    $cat = array(
      'parent' => 0,
      'title' => $title,
      'description' => $title,
      'exchange' => $GLOBALS['exchange_id'],
      'context' => 1, // offers
    );

    $category_object = ces_category_save($cat);
    $category_id = $category_object['id'];

    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $GLOBALS['import_id'],
        'object' => 'category',
        'object_id' => $category_id,
        'row' => $row,
        'data' => ''
      ))->execute();

    return $category_id;

  }
}

/**
 * Get image from CES
 */
function getImageCES($url_origen,$archivo_destino){
  $mi_curl = curl_init ($url_origen);
  $fs_archivo = fopen ($archivo_destino, "w");
  curl_setopt ($mi_curl, CURLOPT_FILE, $fs_archivo);
  curl_setopt ($mi_curl, CURLOPT_HEADER, 0);
  curl_exec ($mi_curl);
  curl_close ($mi_curl);
  fclose ($fs_archivo);
  return TRUE;
} 
?>
