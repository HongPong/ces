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

$msg       = FALSE ;
$error     = FALSE ;
$user_id   = $GLOBALS['user']->uid;

$import_id   = ( isset($_POST['import_id']) ) ? $_POST['import_id'] : FALSE ;
$import_id   = ( isset($_GET['import_id']) )  ? $_GET['import_id']  : $import_id ;
$step        = ( isset($_POST['step']) )      ? $_POST['step']      : 0 ;
$row         = ( isset($_POST['row']) )       ? $_POST['row']       : 1 ;
$anonymous = TRUE ;
$send_mail_user = FALSE ; 

// debug
echo '<pre>REQUEST: ' ; print_r($_REQUEST) ; echo '</pre>'; // exit() ; // DEV  
echo '<pre>row: ' ; print_r($row) ; echo '</pre>'; // exit() ; // DEV  


if ( $import_id ) {

   $result = db_query('SELECT i.exchange_id, e.name, i.step, i.row, i.anonymous 
      FROM {ces_import4ces_exchange} i 
      LEFT JOIN {ces_exchange} e ON i.exchange_id = e.id 
      WHERE i.finished=0 AND i.uid = :uid
      ', array(':uid' => $user_id));

   foreach ($result as $record) {
      $exchange_name      = $record->name      ; 
      $step               = $record->step      ;
      $row                = $record->row       ;
      $anonymous          = $record->anonymous ;
   }

}


?>

<?php if ( $import_id ) { ?>

<h1>Importing <?php echo $exchange_name ?></h2>

<?php } ?>

<?php if ( $step !== 0 ) { ?>
<div id="step_info">Step <span id="step"><?php echo $step ?></span></div>
<?php } ?>

<?php

switch ($step) {
case '0':
?>
      <?php echo t('Put all the csv files in the sites /default/files/import folder.')?>

      <form action="" method="POST">
         <input type="hidden" name="step" value="1"/>
         <input type="submit" name="new" value="<?php echo t('New import')?>"/>
      </form>
<?php
   break;

case '1':  // Import setting.csv
?>
      <h3>Import Setting</h3>
<?php
   include('imports/setting.php');
   $file_csv = $path_csv.'settings.csv';
   $data = procesa_csv($file_csv, 'parse_setting', $row);

   // @todo Edit data if there is a error
   //  createfrom($setting); 

   if ( $data ) {
      $msg = "New Bank created";
      $step++ ; $row=1 ;
   } else {
      $error = "Error in parse setting";
   }
   break;

case '2':  // Import users.csv
?>
      <h3>Import Users</h3>
<?php
   include('imports/users.php');
   $file_csv = $path_csv.'users.csv';
   $data = procesa_csv($file_csv, 'parse_users', $row);
   if ( ! $data ) break;
   // createfrom($data); 
   $step++ ; $row=1 ;
   break;

default:
   $error = "Step not found";
   break;
}

?>

<?php if ( $error ) { ?>
<p class="error"><?php echo $error ?></p>
<?php } ?>

<?php if ( $msg ) { ?>
<div id="message"><?php echo $msg ?></div>
<?php } ?>



<?php

$result = db_query('SELECT i.id, i.exchange_id, e.code, e.name, i.created, i.step, o.row, i.uid
   FROM {ces_import4ces_exchange} i 
   LEFT JOIN {ces_exchange} e ON i.exchange_id = e.id 
   LEFT JOIN {ces_import4ces_objects} o ON i.id = o.import_id 
   WHERE i.finished=0 AND i.uid = :uid
   ORDER BY o.id DESC
   LIMIT 1
   ', array(':uid' => $user_id));

foreach ($result as $record) {
?>
   <form action="" method="post">
   <fieldset>
      <legend><?php echo $record->name ?></legend>
      <input type="hidden" name="step" value="<?php echo $record->step ?>"/>
      <input type="hidden" name="row" value="<?php echo $record->row ?>"/>
      <input type="hidden" name="import_id" value="<?php echo $record->id ?>">
      <input type="submit" name="continue" value="<?php echo t("Continue") ?>">
      <input type="submit" name="delete" value="<?php echo t("Delete") ?>">
   </fieldset>
   </form>
<?php
}



