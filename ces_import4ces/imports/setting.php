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
         'shortname'        => $setting['ExchangeName'],
         'name'             => $setting['ExchangeTitle'],
         'website'          => $setting['WebAddress'],
         'country'          => $setting['CountryCode'],
         'region'           => $setting['Province'],
         'town'             => $setting['Town'],
         'map'              => $setting['MapAddress'],
         'currencysymbol'   => $setting['CurLet'],
         'currencyname'     => $setting['ConCurName'],
         'currenciesname'   => $setting['CurNamePlural'],

         'currencyvalue'    => 1,
         'currencyscale'    => 1,

         'admin'            => $user->uid,
         'data'             => array(
            'registration_offers' => 1,
            'registration_wants' => 0,
         ),
      );

      $extra_data = array(
        'ExchangeType'         => 'ExchangeType',
        'ExchangeDescr'        => 'ExchangeDescr', 
        'Password'             => 'Password',
        'Logo'                 => 'Logo',
        'Administrator'        => 'Administrator', 
        'Addr1'                => 'Addr1',
        'Addr2'                => 'Addr2',
        'Addr3'                => 'Addr3',
        'Postcode'             => 'Postcode',
        'CountryName'          => 'CountryName',
        'Tel1'                 => 'Tel1',
        'Tel2'                 => 'Tel2',
        'Fax'                  => 'Fax',
        'TelCode'              => 'TelCode',
        'Email'                => 'Email',
        'InternetMessaging'    => 'InternetMessaging',     
        'AdminTel'             => 'AdminTel',     
        'AdminEmail'           => 'AdminEmail',       
        'MemSec'               => 'MemSec',   
        'MemSecEmail'          => 'MemSecEmail',        
        'MemSecEmailAlt'       => 'MemSecEmailAlt',
        'MemSecPsw'            => 'MemSecPsw',      
        'MemSecTel'            => 'MemSecTel',      
        'LevyRate'             => 'LevyRate',     
        'CurName'              => 'CurName',    
        'ConCurLet'            => 'ConCurLet',      
        'ReDir'                => 'ReDir',  
        'Hidden'               => 'Hidden',   
        'Active'               => 'Active',   
        'TimeBased'            => 'TimeBased',      
        'TimeUnit'             => 'TimeUnit',     
        'DateAdded'            => 'DateAdded',      
        'DateModified'         => 'DateModified',         
        'CredLim'              => 'CredLim',    
        'DebLim'               => 'DebLim',   
        'TimeDiff'             => 'TimeDiff',     
        'DaylightSavingOn'     => 'DaylightSavingOn',    
        'DaylightSavingOff'    => 'DaylightSavingOff',    
        'Language'             => 'Language',    
        'DefaultExchanges'     => 'DefaultExchanges',    
        'Cell'                 => 'Cell',    
        'SubscriptionExchange' => 'SubscriptionExchange',   
        'WelcomeLetter'        => 'WelcomeLetter',    
        'InviteLetter'         => 'InviteLetter',    
        'InviteLetterHead'     => 'InviteLetterHead',    
        'DoMoney'              => 'DoMoney',    
        'ConRedeemRate'        => 'ConRedeemRate',    
        'HidePsw'              => 'HidePsw',    
        'NoDetails'            => 'NoDetails',    
        'BudRate'              => 'BudRate',    
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
            'data' => serialize($extra_data)
         ))->execute();

      $GLOBALS['import_id'] = $nid ;
      return $nid;

   }

   return FALSE ;

}

?>
