<?php
/**
 * @file
 * This file is a script for filling the database with initial data for
 * demo pruposes. It empties the users and all ces tables and populates
 * it with 10 users with their respective accounts in two exchanges and
 * applies random transactions between them.
 */
include_once DRUPAL_ROOT . '/includes/install.inc';
require_once(drupal_get_path('module', 'ces_bank') . '/ces_bank.install');
//RESET USERS
$query = db_query('SELECT uid FROM {users}');
$users = $query->fetchAllAssoc('uid');
foreach( $users as $user) {
  if (((int)($user->uid)) > 1) {
    user_delete($user->uid);
  }
}
//RESET CES
db_delete('ces_account')->execute();
db_delete('ces_accountuser')->execute();
db_delete('ces_exchange')->execute();
db_delete('ces_limit')->execute();
db_delete('ces_limitchain')->execute();
db_delete('ces_messages')->execute();
db_delete('ces_permission')->execute();
db_delete('ces_transaction')->execute();
_ces_bank_create_default_exchange();
_ces_bank_create_default_permissions();
//CREATE STUFF
$bank = new Bank();

//CREATE USERS
$usernames = array('adminnet1', 'adminnet2', 'Gauss', 'Euler', 'Riemann', 'Euclides', 'Noether', 'Galois', 'Hamilton', 'Newton');
$users = array();
foreach ($usernames as $name) {
  $users[$name] = register_user($name);
}
//CREATE EXCHANGE
$net1 = array(
  'code' => 'NET1',
  'shortname' => 'Net 1',
  'name' => 'Exchange network 1 - Time bank',
  'website' => 'http://www.integralces.net',
  'country' => 'ES',
  'region' => 'Bages',
  'town' => 'Manresa',
  'map' => 'http://maps.google.com/?ll=41.723796,1.832142&spn=0.083663,0.145912&hnear=Manresa,+Province+of+Barcelona,+Catalonia,+Spain&t=m&z',
  'currencysymbol' => 'ℏ',
  'currencyname' => 'hour',
  'currenciesname' => 'hours',
  'currencyvalue' => '1.0',
  'currencyscale' => '2',
  'admin' => $users['adminnet1']->uid,
);
$bank->createExchange($net1);
$bank->activateExchange($net1);
$net2 = array(
  'code' => 'NET2',
  'shortname' => 'Net 2',
  'name' => 'Exchange network 2 - Euro based',
  'website' => 'http://www.integralces.net',
  'country' => 'ES',
  'region' => 'Barcelonès',
  'town' => 'Barcelona',
  'map' => 'http://maps.google.com/barcelona',
  'currencysymbol' => 'ECO',
  'currencyname' => 'eco',
  'currenciesname' => 'ecos',
  'currencyvalue' => '0.1',
  'currencyscale' => '2',
  'admin' => $users['adminnet2']->uid,
);
$bank->createExchange($net2);
$bank->activateExchange($net2);
//CREATE ACCOUNTS
$accounts = array();
for ($i=0; $i<4; $i++) {
  $name = $usernames[$i+2];
  $accounts[$name] = register_account($users[$name], $net1, $i+1);
}
for ($i=0; $i<4; $i++) {
  $name = $usernames[$i+6];
  $accounts[$name] = register_account($users[$name], $net2, $i+1);
}
//CREATE LOCAL TRANSACTIONS

for ($e=1; $e<=2; $e++) {
  for ($i=0; $i<10; $i++) {
    $from = rand(1, 4);
    do{
      $to = rand(1, 4);
    }while($from == $to);
    $trans = array(
        'fromaccountname' => 'NET' . $e . '000' . $from,
        'toaccountname' => 'NET' . $e . '000' . $to,
        'amount' => rand(1, 100)/10,
        'concept' => 'NET' . $e . ' demo transaction #' . $i
      );
    $bank->createTransaction($trans);
    $bank->applyTransaction($trans['id']);
  }
}

//CREATE ONE INTER-TRANSACTION
$trans = array(
  'fromaccountname' => 'NET10001',
  'toaccountname' => 'NET20001',
  'amount' => '10',
  'concept' => 'Inter-exchange demo transaction #1',
);
$bank->createTransaction($trans);
$bank->applyTransaction($trans['id']);
//ACTIVATE VIRTUAL ACCOUNTS
$account = $bank->getAccountByName('NET1NET2');
$bank->activateAccount($account);
$account = $bank->getAccountByName('NET2NET1');
$bank->activateAccount($account);
//RE-TRIGGER INTEREXCHANGE TRANSACTION
$bank->applyTransaction($trans['id']);

function register_user($name) {
  // register a new user
  if (!user_load_by_name($name)) {
    $form_state = array();
    $form_state['values']['name'] = $name;
    $form_state['values']['mail'] = strtolower($name) . '@integralces.net';
    $form_state['values']['pass']['pass1'] = 'integralces';
    $form_state['values']['pass']['pass2'] = 'integralces';
    $form_state['values']['op'] = t('Create new account');
    drupal_form_submit('user_register_form', $form_state);
  }
  return user_load_by_name($name);
}

function register_account($user, $exchange, $i) {
  $bank = new Bank();
  $limit = $bank->getDefaultLimitChain($exchange['id']);
  $account = array(
    'exchange' => $exchange['id'],
    'name' => $exchange['code'] . '000' . $i,
    'limitchain' => $limit['id'],
    'kind' => LocalAccount::TYPE_INDIVIDUAL,
    'state' => LocalAccount::STATE_HIDDEN,
    'users' => array(
      array(
        'user' => $user->uid,
        'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
      )
    )
  );
  $bank->createAccount($account);
  $bank->activateAccount($account);
}