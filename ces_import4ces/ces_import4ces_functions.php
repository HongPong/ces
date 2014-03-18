<?php

/**
 * @file
 * functions of ces_import4ces
 */

function procesa_csv($file_csv, $parse, $row=1) {

   $importar = FALSE ;

   if ( ! file_exists($file_csv) ) {

      error_i4c(t("Was not found import file")." [$file_csv] <br />". t("Place the files in the folder sites/default/files/ and try again."));
      return FALSE;
   }

   $fila = 0;
   if (($gestor = fopen($file_csv, "r")) !== FALSE) {
      while (($datos = fgetcsv($gestor, 1000, ",")) !== FALSE) {
         $numero = count($datos);
         if ( $fila != 0 && $fila < $row ) continue;
         // echo "<h1>Fila: $fila / Número de campos $numero</h1>\n";
         foreach( $datos as $key => $val ) {
            $datos[$key] = trim( $datos[$key] );
            $datos[$key] = iconv('ISO-8859-1', 'UTF-8', $datos[$key]) ;
            $datos[$key] = str_replace('""', '"', $datos[$key]);
            $datos[$key] = preg_replace("/^\"(.*)\"$/sim", "$1", $datos[$key]);
            if ( $fila == 0 ) {
               $heads[] = $datos[$key] ;
            } else {
               $cols[] = $datos[$key] ;
            }
         }
         if ( $fila !== 0 ) {
            // @todo Repair it here
            if ( count($heads) > count($cols) ) {
               $faltan = count($cols) - count($heads);
               for ($i=0; $i<$faltan;$i++) {
                  array_push($heads,"LACK!-0$i");
               }
               error_i4c(t('Mismatch fields, check the form to correct errors'));
               $importar = array_combine($heads,$cols);
               $cols = array();
               return $importar;
            }
            if ( count($heads) < count($cols) ) {
               $faltan = count($heads) - count($cols) ;
               for ($i=0; $i<$faltan;$i++) {
                  array_push($cols,"");
               }
            }
            if ( count($heads) !== count($cols) ) {
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
            if ( $parse($importar, $fila) === FALSE ) {
               error_i4c(t("Error on parse row")." $fila");
               return FALSE;
            }

         }
         $fila++;
      }

      // if ( isset($importar) ) { createfrom($importar); }

      fclose($gestor);
   }

   return ( $importar ) ? $importar : FALSE ;

}

/**
 * Create a form with data
 */

function createfrom($importar) {

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
         </form>
      </fieldset>
<?php
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
?>
