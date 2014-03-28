<?php

/**
 * @file
 * Functions from parse trades
 */

/**
 * Parse trades
 */

function parse_trades($data, $row) {

  $exchange_id = $GLOBALS['exchange_id'];

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

  $account_seller    = $bank->getAccountByName($data['Seller']);
  $account_buyer     = $bank->getAccountByName($data['Buyer']);

  if ( substr($data['EnteredBy'],-4) == '0000' ) {
    $trade_user_id = $GLOBALS['user']->uid;
  } else {
    // Find uid from user
    $query = db_query('SELECT uid FROM {users} where name=:name',array(':name' => $data['EnteredBy']));
    $trade_user_id = $query->fetchColumn(0);
  }

  $trade = array(
    'fromaccount'     => $account_seller['id'],
    'toaccount'       => $account_buyer['id'],
    'fromaccountname' => $data['Seller'],
    'toaccountname'   => $data['Buyer'],
    'amount'          => $data['Amount'],
    'concept'         => $data['Description'],
    'user'            => $trade_user_id,
    'state'           => 3, 
    'created'    => strtotime($data['DateEntered']),
    'modified'   => time(),
  );

  $extra_info = array(
    'ID'                 => $data['ID'],
    'RemoteExchange'     => $data['RemoteExchange'],
    'RemoteBuyer'        => $data['RemoteBuyer'],
    'RecordID'           => $data['RecordID'],
    'Levy'               => $data['Levy'],
    'LevyRate'           => $data['LevyRate'],
  );

  $trans = array(
    'fromaccountname' => $trade['fromaccountname'],
    'toaccountname' => $trade['toaccountname'],
    'amount' => $trade['amount'],
    'concept' => $trade['concept'],
    'user' => $trade['user'],
  );

  $bank->createTransaction($trans);
  $bank->applyTransaction($trans['id']);

  if ( $trade ) {
    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $GLOBALS['import_id'],
        'object' => 'trades',
        'object_id' => $trans['id'],
        'row' => $row,
        'data' => serialize($extra_info)
      ))->execute();
    return $nid;
  }


  return FALSE ;

}

?>
