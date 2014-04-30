<?php

/**
 * @file
 * Functions from parse trades
 */

/**
 * Parse trades
 */
function parse_trades($import_id, $data, $row, &$context) {
  global $user;
  if (isset($context['results']['error']))
    return;
  $tx = db_transaction();
  try {
    $context['results']['import_id'] = $import_id;
    $bank = new Bank();
    /*
      [ID] => 2
      [Seller] => HORA0014
      [Buyer] => HORA0001
      [RemoteExchange] =>
      [RemoteBuyer] =>
      [RecordID] => 0001201105142134451
      [DateEntered] => 2011/05/14
      [EnteredBy] => HORA0001
      [Amount] => 1
      [Levy] => 0
      [LevyRate] => 0
      [Description] => 10€

      array('NET10001', 'NET10002', 1.2, '3kg of potatoes.', $users['Euclides']->uid),

      int(10)
      fromaccount 	varchar(31)
      toaccount 	varchar(31)
      amount 	decimal(31,4)
      user 	int(10)
      concept 	longtext
      state 	int(10)
      created 	int(10)
      modified 	int(10)
      data 	blob 		BINARY
      decoration



     */
    $account_seller = _ces_import4ces_trades_get_account($import_id, $data['Seller'], $data);
    if ($account_seller === FALSE) {
      throw new Exception('Acount @account not found.', array('@account' => $data['Seller']));
    }
    $account_buyer = _ces_import4ces_trades_get_account($import_id, $data['Buyer'], $data);
    if ($account_buyer === FALSE) {
      throw new Exception('Acount @account not found.', array('@account' => $data['Buyer']));
    }
    if (substr($data['EnteredBy'], -4) == '0000') {
      $trade_user_id = $user->uid;
    }
    else {
      // Find uid from user
      $query = db_query('SELECT uid FROM {users} where name=:name', array(':name' => $data['EnteredBy']));
      $trade_user_id = $query->fetchColumn(0);
      if (!$trade_user_id) {
        $trade_user_id = $user->uid;
      }
    }

    $extra_info = array(
      'ID' => $data['ID'],
      'RemoteExchange' => $data['RemoteExchange'],
      'RemoteBuyer' => $data['RemoteBuyer'],
      'RecordID' => $data['RecordID'],
      'Levy' => $data['Levy'],
      'LevyRate' => $data['LevyRate'],
    );

    $trans = array(
      'fromaccountname' => $account_buyer['name'],
      'toaccountname' => $account_seller['name'],
      'amount' => $data['Amount'],
      'concept' => $data['Description'],
      'user' => $trade_user_id,
      'created' => strtotime($data['DateEntered']),
      'modified' => strtotime($data['DateEntered']),
    );

    $bank->createTransaction($trans);
    $bank->applyTransaction($trans['id']);

    db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $import_id,
        'object' => 'trades',
        'object_id' => $trans['id'],
        'row' => $row,
        'data' => serialize($extra_info)
      ))->execute();
    ces_import4ces_update_row($import_id, $row);
  }
  catch (Exception $e) {
    $tx->rollback();
    ces_import4ces_batch_fail_row($import_id, array_keys($data), array_values($data), $row, $context);
    $context['results']['error'] = check_plain($e->getMessage());
  }
}

/**
 * Get an account record for this trade. If the trade is inter-exchange, it
 * returns the proper interexchange virtual account, creating it if necessary.
 * 
 * @param $import_id int
 *   The id of this import.
 * @param $name string
 *   The name of the account to be retrieved.
 * @param $data array
 *   The full import line.
 * 
 * @return The accout record or FALSE if it donesn't exist.
 */
function _ces_import4ces_trades_get_account($import_id, $name, $data) {
  $bank = new Bank();
  if (substr($name, 4) == 'VIRT') {
      // This is an inter-exchange transaction. Use the corresponding virtual
      // account.
      $name = substr($name, 0, 4) . $data['RemoteExchange'];
      $account_seller = $bank->getAccountByName($name);
      if ($account_seller === FALSE) {
        // The virtual account does not exist yet, so let's create it.
        $import = ces_import4ces_import_load($import_id);
        $exchange = $bank->getExchange($import->exchange_id);
        $account_seller = array(
          'id' => NULL,
          'exchange' => $exchange['id'],
          'name' => $name,
          'balance' => 0.0,
          'state' => LocalAccount::STATE_HIDDEN,
          'kind' => LocalAccount::TYPE_VIRTUAL,
          'limitchain' => $exchange['limitchain'],
          'users' => array(
            array(
              'account' => NULL,
              'user' => 1,
              'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
            ),
          ),
        );
        $bank->createAccount($account_seller);
        $bank->activateAccount($account_seller);
      }
    }
    else{
      $account_seller = $bank->getAccountByName($name);
    }
    return $account_seller;
}