<?php
/**
 * @file
 * Import data from CES
 */

include_once DRUPAL_ROOT . '/includes/install.inc';
require_once drupal_get_path('module', 'ces_import4ces') . '/ces_import4ces_functions.php';
require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.install';
require_once drupal_get_path('module', 'ces_offerswants') . '/ces_offerswants.module';

$path_csv = DRUPAL_ROOT.'/sites/default/files/import/';
$step     = ( isset($_POST['step']) ) ? $_POST['step'] : 0 ;
$msg      = FALSE ;
$error    = FALSE ;

?>

<div id="step_info">Step <span id="step"><?php echo $step ?></span></div>

<?php

switch ($step) {
   case '0':
      $msg = t('Put all the csv files in the sites /default/files/import folder and click "Continue"');
      $step++;
      break;

   case '1':

      $file_csv = $path_csv.'settings.csv';

      if ( ! file_exists($file_csv) ) {

         ?>
         <p class="error">
         <?php echo t("Was not found import file");?> [<?php echo $file_csv?>]
         <?php echo t("Place the files in the folder sites/default/files/ and try again.");?>
         </p>
         <?php
         }

      include('imports/setting.php');

      $step++;
      break;

}

?>

<?php if ( $error ) { ?>
<div id="message"><?php echo $error ?></div>
<?php } ?>

<?php if ( $msg ) { ?>
<div id="message"><?php echo $msg ?></div>
<?php } ?>



<form action="" method="POST">
   <input type="hidden" name="step" value="<?php echo $step ?>"/>
   <input type="submit" name="continue" value="<?php echo t("Continue") ?>"/>
</form>

<style>
.form_i4c input, .form_i4c textarea {
    width: 100%;
}
.form_i4c textarea {
    height: 160px;
}
#step_info{
    background: none repeat scroll 0 0 #F5F5F5;
    width: 100%;
}
#actions > a {
    background: none repeat scroll 0 0 #9ACD32;
    border-radius: 6%;
    color: #FFFFFF;
    margin: 6px 6px 6px 0;
    padding: 4px;
}
</style>

