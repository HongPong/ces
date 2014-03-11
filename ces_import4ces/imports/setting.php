<?php

/**
 * @file
 */

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

?>
