<?php

/**
 * @file
 * Functions from parse setting
 */

/**
 * Parse exchange settings.
 */
function parse_setting($import_id, $setting, $row, &$context) {
  if (isset($context['results']['error']))
    return;
  $tx = db_transaction();
  try {
    global $user;

    // Crear bank
    $bank = new Bank();
    
    // Create exchange administrator drupal user. It will be completed in users
    // step.
    $record = array(
      'name' => $setting['ExchangeID'] . '0000',
      'mail' => (CES_IMPORT4CES_ANONYMOUS) ? 'test-' . $setting['ExchangeID'] . 
        '0000@test.com' : $setting['Email'],
      'pass' => $setting['Password'],
      'status' => 1,
      'roles' => array(DRUPAL_AUTHENTICATED_RID => 'authenticated user'),
    );
    $admin_user = user_save('', $record);

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
      'currencyscale' => 2,
      'admin' => $admin_user->uid,
      'data' => array(
        'registration_offers' => 1,
        'registration_wants' => 0,
      ),
    );

    $extra_data = array(
      'ExchangeType' => $setting['ExchangeType'],
      'ExchangeDescr' => $setting['ExchangeDescr'],
      'Password' => $setting['Password'],
      'Logo' => $setting['Logo'],
      'Administrator' => $setting['Administrator'],
      'Addr1' => $setting['Addr1'],
      'Addr2' => $setting['Addr2'],
      'Addr3' => $setting['Addr3'],
      'Postcode' => $setting['Postcode'],
      'CountryName' => $setting['CountryName'],
      'Tel1' => $setting['Tel1'],
      'Tel2' => $setting['Tel2'],
      'Fax' => $setting['Fax'],
      'TelCode' => $setting['TelCode'],
      'Email' => $setting['Email'],
      'InternetMessaging' => $setting['InternetMessaging'],
      'AdminTel' => $setting['AdminTel'],
      'AdminEmail' => $setting['AdminEmail'],
      'MemSec' => $setting['MemSec'],
      'MemSecEmail' => $setting['MemSecEmail'],
      'MemSecEmailAlt' => $setting['MemSecEmailAlt'],
      'MemSecPsw' => $setting['MemSecPsw'],
      'MemSecTel' => $setting['MemSecTel'],
      'LevyRate' => $setting['LevyRate'],
      'CurName' => $setting['CurName'],
      'ConCurLet' => $setting['ConCurLet'],
      'ReDir' => $setting['ReDir'],
      'Hidden' => $setting['Hidden'],
      'Active' => $setting['Active'],
      'TimeBased' => $setting['TimeBased'],
      'TimeUnit' => $setting['TimeUnit'],
      'DateAdded' => $setting['DateAdded'],
      'DateModified' => $setting['DateModified'],
      'TimeDiff' => $setting['TimeDiff'],
      'DaylightSavingOn' => $setting['DaylightSavingOn'],
      'DaylightSavingOff' => $setting['DaylightSavingOff'],
      'Language' => $setting['Language'],
      'DefaultExchanges' => $setting['DefaultExchanges'],
      'Cell' => $setting['Cell'],
      'SubscriptionExchange' => $setting['SubscriptionExchange'],
      'WelcomeLetter' => $setting['WelcomeLetter'],
      'InviteLetter' => $setting['InviteLetter'],
      'InviteLetterHead' => $setting['InviteLetterHead'],
      'DoMoney' => $setting['DoMoney'],
      'ConRedeemRate' => $setting['ConRedeemRate'],
      'HidePsw' => $setting['HidePsw'],
      'NoDetails' => $setting['NoDetails'],
      'BudRate' => $setting['BudRate'],
    );

    $bank->createExchange($exchange);
    $bank->activateExchange($exchange);
    // Update default credit/debit limit.
    $default_limit = $bank->getLimitChain($exchange['limitchain']);
    $default_credit = $setting['CredLim'];
    $default_debit = $setting['DebLim'];
    if ($default_credit != 0) {
      $default_limit['limits'][] = array(
        'classname' => 'AbsoluteCreditLimit',
        'value' => $default_credit,
        'block' => FALSE,
      );
    }
    if ($default_debit != 0) {
      $default_limit['limits'][] = array(
        'classname' => 'AbsluteDebitLimit',
        'value' => - $default_debit,
        'block' => FALSE,
      );
    }
    $bank->updateLimitChain($default_limit);
    db_update('ces_import4ces_exchange')
    ->condition('id', $import_id)->fields(array(
      'exchange_id' => $exchange['id'],
      'data' => serialize($extra_data),
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
