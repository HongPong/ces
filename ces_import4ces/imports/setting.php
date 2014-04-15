<?php

/**
 * @file
 * Functions from parse setting
 */

/**
 * Parse exchange settings.
 */
function parse_setting($setting, $row, &$context) {
  if (isset($context['results']['error']))
    return;
  $tx = db_transaction();
  try {

    global $user;

    // Crear bank
    $bank = new Bank();

    // Create exchange.
    $exchange = array(
      'code' => $setting['ExchangeID'],
      'shortname' => $setting['ExchangeName'],
      'name' => $setting['ExchangeTitle'],
      'website' => $setting['WebAddress'],
      'country' => $setting['CountryCode'],
      'region' => $setting['Province'],
      'town' => $setting['Town'],
      'map' => $setting['MapAddress'],
      'currencysymbol' => html_entity_decode($setting['CurLet'], ENT_QUOTES, 'UTF-8') ,
      'currencyname' => $setting['ConCurName'],
      'currenciesname' => $setting['CurNamePlural'],
      'currencyvalue' => 1,
      'currencyscale' => 1,
      'admin' => $user->uid,
      'data' => array(
        'registration_offers' => 1,
        'registration_wants' => 0,
      ),
    );

    $extra_data = array(
      'ExchangeType' => 'ExchangeType',
      'ExchangeDescr' => 'ExchangeDescr',
      'Password' => 'Password',
      'Logo' => 'Logo',
      'Administrator' => 'Administrator',
      'Addr1' => 'Addr1',
      'Addr2' => 'Addr2',
      'Addr3' => 'Addr3',
      'Postcode' => 'Postcode',
      'CountryName' => 'CountryName',
      'Tel1' => 'Tel1',
      'Tel2' => 'Tel2',
      'Fax' => 'Fax',
      'TelCode' => 'TelCode',
      'Email' => 'Email',
      'InternetMessaging' => 'InternetMessaging',
      'AdminTel' => 'AdminTel',
      'AdminEmail' => 'AdminEmail',
      'MemSec' => 'MemSec',
      'MemSecEmail' => 'MemSecEmail',
      'MemSecEmailAlt' => 'MemSecEmailAlt',
      'MemSecPsw' => 'MemSecPsw',
      'MemSecTel' => 'MemSecTel',
      'LevyRate' => 'LevyRate',
      'CurName' => 'CurName',
      'ConCurLet' => 'ConCurLet',
      'ReDir' => 'ReDir',
      'Hidden' => 'Hidden',
      'Active' => 'Active',
      'TimeBased' => 'TimeBased',
      'TimeUnit' => 'TimeUnit',
      'DateAdded' => 'DateAdded',
      'DateModified' => 'DateModified',
      'CredLim' => 'CredLim',
      'DebLim' => 'DebLim',
      'TimeDiff' => 'TimeDiff',
      'DaylightSavingOn' => 'DaylightSavingOn',
      'DaylightSavingOff' => 'DaylightSavingOff',
      'Language' => 'Language',
      'DefaultExchanges' => 'DefaultExchanges',
      'Cell' => 'Cell',
      'SubscriptionExchange' => 'SubscriptionExchange',
      'WelcomeLetter' => 'WelcomeLetter',
      'InviteLetter' => 'InviteLetter',
      'InviteLetterHead' => 'InviteLetterHead',
      'DoMoney' => 'DoMoney',
      'ConRedeemRate' => 'ConRedeemRate',
      'HidePsw' => 'HidePsw',
      'NoDetails' => 'NoDetails',
      'BudRate' => 'BudRate',
    );

    $bank->createExchange($exchange);
    $bank->activateExchange($exchange);

    $import_id = db_insert('ces_import4ces_exchange')
        ->fields(array(
          'exchange_id' => $exchange['id'],
          'created' => REQUEST_TIME,
          'step' => 1,
          'anonymous' => CES_IMPORT4CES_ANONYMOUS,
          'uid' => $user->uid,
          'data' => serialize($extra_data)
        ))->execute();

    ces_import4ces_update_row($import_id, $row);
    $context['message'] = check_plain($exchange['name']);
    $context['results']['import_id'] = $import_id;
  }
  catch (Exception $e) {
    $tx->rollback();
    ces_import4ces_batch_fail_row(NULL, array_keys($setting), array_values($setting), $row, $context);
    $context['results']['error'] = check_plain($e->getMessage());
  }
}
