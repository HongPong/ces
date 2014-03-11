<?php

/**
 * @file
 * functions of ces_import4ces
 */

function procesa_csv($file_csv) {

   $fila = 1;
   if (($gestor = fopen($file_csv, "r")) !== FALSE) {
      while (($datos = fgetcsv($gestor, 1000, ",")) !== FALSE) {
         $numero = count($datos);
         // echo "<h1>Fila: $fila / Número de campos $numero</h1>\n";
         foreach( $datos as $key => $val ) {
            $datos[$key] = trim( $datos[$key] );
            $datos[$key] = iconv('ISO-8859-1', 'UTF-8', $datos[$key]) ;
            $datos[$key] = str_replace('""', '"', $datos[$key]);
            $datos[$key] = preg_replace("/^\"(.*)\"$/sim", "$1", $datos[$key]);
            if ( $fila == 1 ) {
               $heads[] = $datos[$key] ;
            } else {
               $cols[] = $datos[$key] ;
            }
            // echo "<br/>";
            // echo "<h2>$key</h2>";
            // echo "<h3 style='background: yellow;'>".$datos[$key]."</h3>";
         }
         if ( $fila !== 1 ) {
            // @todo Repair it here
            if ( FALSE && count($heads) !== count($cols) ) {
               $faltan = count($cols) - count($heads);
               for ($i=0; $i<$faltan;$i++) {
                  array_push($heads,"LACK!-0$i");
               }
               ?>
               <p class="error">
               <?php echo t('Mismatch fields, check the form to correct errors');?>
               </p>
               <?php
            }
            if ( count($heads) !== count($cols) ) {
               ?>
               <p class="error">
                  <?php echo t('Not match the number of fields, it may have been interpreted in a comma as a field separator.') ?>
                  <br />
                  <?php echo t('For example links to maps can generate this problem.')?>
                  <br />
                  <?php echo t('One possible solution is to enclose in double quotes the problematic field.')?>
                  <br />
                  <?php echo t('Please check the csv file to ensure proper export.')?>
               </p>
               <?php
               return FALSE;
            }
         $importar[] = array_combine($heads,$cols);
         }
         $fila++;
      }

      // if ( isset($importar) ) { createfrom($importar); }

      fclose($gestor);
   }

   return $importar;

}

/**
 * Create a form with data
 */

function createfrom($importar) {

   foreach ( $importar as $num_fila => $fila ) {

      ?>
      <fieldset class="form_i4c" id="num_fila_<?php echo $num_fila ?>">
         <legend>Row <?php echo $num_fila ?></legend>
         <form action="" method="post">
         <?php foreach ( $fila as $key => $value ) { ?>
         <label for="label_<?php echo $key ?>"><?php echo $key ?></label>
         <?php if ( strlen($value) > 100 ) { ?>
            <textarea name="<?php echo $key ?>"><?php echo $value ?> id="<?php echo $key ?>"</textarea>
         <?php } else { ?>
            <input type="text" name="<?php echo $key ?>" value="<?php echo $value ?>" id="<?php echo $key ?>">     
         <?php } ?>
         <?php } ?>
         </form>
      </fieldset>
      <?php

   }
}
?>
