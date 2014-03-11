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

$error_procesando = FALSE;

if ( ! isset($_POST['pass']) ) {

   ?>

   <?php echo t('Put all the csv files in the sites /default/files/import folder and click "Continue"');?>
   <form action="" method="POST">
   <input type="hidden" name="pass" value="0"/>
   <input type="submit" name="continue" value="<?php echo t("Continue") ?>"/>
   </form>
   <?php

} elseif ( $_POST['pass'] == 0 ) {

   ?>
   <p>Createing a new exchange...</p>
   <?php


   $file_csv = $path_csv.'settings.csv';

   if ( ! file_exists($file_csv) ) {

      ?>
      <p class="error">
      <?php echo t("Was not found import file");?> [<?php echo $file_csv?>]
      <?php echo t("Place the files in the folder sites/default/files/ and try again.");?>
      </p>
      <form action="" method="POST">
      <input type="hidden" name="pass" value="0"/>
      <input type="submit" name="continue" value="<?php echo t("Continue") ?>"/>
      </form>
      <?php
      }

   ?>
   <p>extracting information...</p>

   <?php

   $setting = procesa_csv($file_csv);

   if ( $setting ) {

      global $user;

      // Crear bank
      $bank = new Bank();

      $setting = $setting[0];

      // Create exchange.
      $exchange = array(
        'code'             => $setting['ExchangeID'],
        'shortname'        => $setting['ExchangeTitle'],
        'name'             => $setting['ExchangeName'],
        'website'          => $setting['WebAddress'],
        'country'          => $setting['CountryCode'],
        'region'           => $setting['Province'],
        'town'             => $setting['Town'],
        'map'              => $setting['MapAddress'],
        'currencysymbol'   => $setting['CurLet'],
        'currencyname'     => $setting['ConCurName'],
        'currenciesname'   => $setting['CurNamePlural'],

        'currencyvalue'    => 1, // @todo $setting['?'],
        'currencyscale'    => 1, // @todo $setting['?'],

        'admin'            => $user->uid,
        'data'             => array(
          'registration_offers' => 1,
          'registration_wants' => 0,
        ),
      );
      $bank->createExchange($exchange);
   }

}

?>
<style>
.form_i4c input, .form_i4c textarea {
    width: 100%;
}
.form_i4c textarea {
    height: 160px;
}
</style>

