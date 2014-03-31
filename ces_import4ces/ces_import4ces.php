<?php
/**
 * @file
 * Import data from CES
 */


ini_set('auto_detect_line_endings',TRUE);

include_once DRUPAL_ROOT . '/includes/install.inc';
require_once drupal_get_path('module', 'ces_import4ces') . '/ces_import4ces_functions.php';
require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.install';
require_once drupal_get_path('module', 'ces_offerswants') . '/ces_offerswants.module';

drupal_add_css(drupal_get_path('module', 'ces_import4ces') .'/style.css');

$path_csv = DRUPAL_ROOT.'/sites/default/files/import/';

$msg       = FALSE ;
$error     = FALSE ;
$user_id   = $GLOBALS['user']->uid;

$title = "Step not found";  //< Default title
/** Titles from steps */
$titles_steps = array(
  0 => "New exchange",
  1 => "Exchange",
  2 => "Users",
  3 => "Offers",
  4 => "Wants",
  5 => "Trades",
  6 => "The end",
);

$import_id   = ( isset($_POST['import_id']) ) ? $_POST['import_id'] : FALSE ;
$import_id   = ( isset($_GET['import_id']) )  ? $_GET['import_id']  : $import_id ;
$step        = ( isset($_POST['step']) )      ? $_POST['step']      : 0 ;
$row         = ( isset($_POST['row']) )       ? $_POST['row']       : 0 ;

$anonymous      = TRUE  ;   ///< Hide personal info
$send_mail_user = FALSE ;   ///< Send email from reset password

if ( $import_id ) {

  $result = db_query('SELECT i.exchange_id, e.code, e.name, i.step, i.row, i.anonymous 
    FROM {ces_import4ces_exchange} i 
    LEFT JOIN {ces_exchange} e ON i.exchange_id = e.id 
    WHERE i.finished=0 AND i.uid = :uid
    ', array(':uid' => $user_id));

  foreach ($result as $record) {
    $exchange_id        = $record->exchange_id ;   
    $exchange_name      = $record->name        ; 
    $exchange_code      = $record->code        ; 
    $step               = $record->step        ;
    $row                = $record->row         ;
    $anonymous          = $record->anonymous   ;
  }

  $GLOBALS['exchange_id'] = $exchange_id ;
  $GLOBALS['exchange_name'] = $exchange_name ;
  $GLOBALS['exchange_code'] = $exchange_code ;
}

$GLOBALS['anonymous'] = $anonymous ;
$GLOBALS['import_id'] = $import_id ;
$GLOBALS['send_mail_user'] = $send_mail_user ;
$GLOBALS['msg'] = $msg ;
$GLOBALS['error'] = $error ;
$GLOBALS['step'] = $step ;
$GLOBALS['row'] = $row ;

if ( isset($_POST['row_error']) ) {
  $row = $_POST['row'];
  $GLOBALS['row'] = $row; 
  update_row($row);
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

case '1':
  include('imports/setting.php');
  $file_csv = $path_csv.'settings.csv';
  $parse_function = 'parse_setting';
  break;

case '2':
  include('imports/users.php');
  $file_csv = $path_csv.'users.csv';
  $parse_function = 'parse_users';
  break;

case '3':
  include('imports/offers.php');
  $file_csv = $path_csv.'offers.csv';
  $parse_function = 'parse_offers';
  break;

case '4':
  include('imports/wants.php');
  $file_csv = $path_csv.'wants.csv';
  $parse_function = 'parse_wants';
  break;

case '5':
  include('imports/trades.php');
  $file_csv = $path_csv.'trades.csv';
  $parse_function = 'parse_trades';
  break;

default:
  $msg = "Process completed successfully";
  break;
}

if ( isset($parse_function) ) {
  $title = ( isset($titles_steps[$step]) ) ? $titles_steps[$step] : $title ;
  ?>
  <h3><?php echo t('Importing').' '.t($title) ?></h3>
  <?php
  update_step($step);
  $status['data_come_from'] = 0;
  $status = procesa_csv($file_csv, $parse_function, $row);
  if ( $status['finished'] ) {
    $step++; 
    $row=0 ;
    update_step($step) ;
    update_row($row) ;
    $msg = "Step completed successfully";
  }
}
?>

<?php if ( $error ) { ?>
<p class="error"><?php echo $error ?></p>
<?php } ?>

<?php if ( $msg ) { ?>
<div id="message"><?php echo $msg ?></div>
<?php } ?>



<?php

$result = db_query('SELECT i.id, i.exchange_id, e.code, e.name, i.created, i.step, i.row , i.uid, i.observations
  FROM {ces_import4ces_exchange} i 
  LEFT JOIN {ces_exchange} e ON i.exchange_id = e.id 
  WHERE i.finished=0 AND i.uid = :uid
  ORDER BY i.id DESC
  ', array(':uid' => $user_id));

foreach ($result as $record) {

  $name          = ( isset($record->name)          ) ? $record->name          : 'NULL' ;
  $id            = ( isset($record->id)            ) ? $record->id            : 'NULL' ;
  $step          = ( isset($record->step)          ) ? $record->step          : 'NULL' ;
  $row           = ( isset($record->row)           ) ? $record->row           : 0      ;
  $observations  = ( isset($record->observations)  ) ? $record->observations  : FALSE  ;

  // Comprobaciones
  // Si no hay un exchange asociado debe comunicarse
?>
   <form class="form_i4c" action="" method="post">
   <fieldset>
      <legend><?php echo $name ?></legend>
      <div class="info_import">
      Step: <?php echo $titles_steps[$step]." ($step / ".count($titles_steps)." )"; ?> / row: <?php echo $row ?>
      </div>
      <?php if ( $observations ) { ?>
      <textarea class="import_observation"><?php echo $observations ?></textarea>
      <?php } ?>
      <input type="hidden" name="step" value="<?php echo $step ?>"/>
      <input type="hidden" name="row" value="<?php echo $row?>"/>
      <input type="hidden" name="import_id" value="<?php echo $id ?>">
      <input type="submit" name="continue" value="<?php echo t("Continue") ?>">
      <input type="submit" name="delete" value="<?php echo t("Delete Importation") ?>">
   </fieldset>
   </form>
<?php
}

