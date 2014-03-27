<?php

/**
 * @file
 * Functions from parse setting
 */

/**
 * Parse setting
 */

function parse_setting($setting) {

   if ( $setting ) {

      global $user;

      // Crear bank
      $bank = new Bank();

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
      $bank->activateExchange($exchange);

      $nid = db_insert('ces_import4ces_exchange')
         ->fields(array(
            'exchange_id' => $exchange['id'],
            'created' => REQUEST_TIME,
            'step' => 1,
            'anonymous' => ( ($GLOBALS['anonymous']) ? 1 : 0 ),
            'uid' => $user->uid,
            'data' => serialize($setting)
         ))->execute();

      $GLOBALS['import_id'] = $nid ;
      return $nid;

   }

   return FALSE ;

}

/**
 * Delete setting
 * 
 * Delete exchange import
 */

function delete_setting($import_id) {

   echo 'Pendiente borrado de importación '.$import_id;

}

?>
