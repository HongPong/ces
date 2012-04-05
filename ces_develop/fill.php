<?php
/**
 * This file is a script for filling the database with initial data for 
 * developing purposes. 
 * 
 * It deletes all ces/bank data.
 * 
 * It creates a pair of exchanges and several accounts as detailed:
 * 
 * Exchange: HORA 
 *  Account: HORA0001
 *  Account: HORA0002
 * Exchange: BCNA
 *  Account: BCNA0001
 *  Account: BCNA0002
 * 
 * And the following users if they don't exist
 * 
 * adminhora (HORA administrator)
 * userhora (HORA0001 owner)
 *
 * adminbcna (BCNA administrator)
 * userbcna (BCNA0001 owner)
 * 
 * userhorabcna (HORA0002 & BCNA0002 owner)
 * 
 * 
 */

//clean
db_delete('ces_account')->execute();
db_delete('ces_accountuser')->execute();
db_delete('ces_exchange')->execute();
db_delete('ces_limit')->execute();
db_delete('ces_limitchain')->execute();
db_delete('ces_messages')->execute();
db_delete('ces_permission')->execute();
db_delete('ces_transaction')->execute();
include_once DRUPAL_ROOT . '/includes/install.inc';
require_once(drupal_get_path('module', 'bank').'/bank.install');
_bank_create_default_exchange();
_bank_create_default_permissions();

//create users
registerUser('adminhora');
registerUser('userhora');
registerUser('adminbcna');
registerUser('userbcna');
registerUser('userhorabcna');
$adminhora = user_load_by_name('adminhora');
$userhora = user_load_by_name('userhora');
$adminbcna = user_load_by_name('adminbcna');
$userbcna = user_load_by_name('userbcna');
$userhorabcna = user_load_by_name('userhorabcna');

//create exchanges
$bank = new Bank();
$hora = array(
  'code' => 'HORA',
  'shortname' => 'EX Bages',
  'name' => 'A bona hora - Ecoxarxa del Bages',
  'website' => 'http://abonahora.wordpress.com',
  'country' => 'ES',
  'region' => 'Bages',
  'town' => 'Manresa',
  'map' => 'http://maps.google.com/?ll=41.723796,1.832142&spn=0.083663,0.145912&hnear=Manresa,+Province+of+Barcelona,+Catalonia,+Spain&t=m&z',
  'currencysymbol' => 'ℏ',
  'currencyname' => 'hora',
  'currenciesname' => 'hores',
  'currencyvalue' => '1.0',
  'currencyscale' => '2',
);
$bank->createExchange($hora);
$bank->activateExchange($hora);

$bcna = array(
  'code' => 'BCNA',
  'shortname' => 'EX Barcelona',
  'name' => 'Ecoxarxa de Barcelona',
  'website' => 'http://barcelona.ecoxarxes.cat',
  'country' => 'ES',
  'region' => 'Barcelonès',
  'town' => 'Barcelona',
  'map' => 'http://maps.google.com/barcelona',
  'currencysymbol' => 'ECO',
  'currencyname' => 'eco',
  'currenciesname' => 'ecos',
  'currencyvalue' => '0.1',
  'currencyscale' => '2',
);
$bank->createExchange($bcna);
$bank->activateExchange($bcna);
//create accounts
$limitchainhora = $bank->getDefaultLimitChain($hora['id']);
$hora0001 = array(
  'exchange'=>$hora['id'],
  'name'=>'HORA0001',
  'limitchain'=>$limitchainhora['id'],
  'kind'=>LocalAccount::TYPE_INDIVIDUAL,
  'state'=>LocalAccount::STATE_ACTIVE,
  'users'=>array(
    array(
      'user'=>$userhora->uid,
      'role'=>AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
      'account'=>0
    )
  )
);
$bank->createAccount($hora0001);
$hora0002 = array(
  'exchange'=>$hora['id'],
  'name'=>'HORA0002',
  'limitchain'=>$limitchainhora['id'],
  'kind'=>LocalAccount::TYPE_INDIVIDUAL,
  'state'=>LocalAccount::STATE_ACTIVE,
  'users'=>array(
    array(
      'user'=>$userhorabcna->uid,
      'role'=>AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
      'account'=>0
    )
  )
);
$bank->createAccount($hora0002);
$limitchainbcna = $bank->getDefaultLimitChain($bcna['id']);
$bcna0001 = array(
  'exchange'=>$bcna['id'],
  'name'=>'BCNA0001',
  'limitchain'=>$limitchainbcna['id'],
  'kind'=>LocalAccount::TYPE_INDIVIDUAL,
  'state'=>LocalAccount::STATE_ACTIVE,
  'users'=>array(
    array(
      'user'=>$userbcna->uid,
      'role'=>AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
      'account'=>0
    )
  )
);
$bank->createAccount($bcna0001);
$bcna0002 = array(
  'exchange'=>$bcna['id'],
  'name'=>'BCNA0002',
  'limitchain'=>$limitchainbcna['id'],
  'kind'=>LocalAccount::TYPE_INDIVIDUAL,
  'state'=>LocalAccount::STATE_ACTIVE,
  'users'=>array(
    array(
      'user'=>$userhorabcna->uid,
      'role'=>AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
      'account'=>0
    )
  )
);
$bank->createAccount($bcna0002);

//set administrative permissions
$perm1 = array(
  'permission' => Permission::PERMISSION_ADMIN,
  'object' => 'exchange',
  'objectid' => $hora['id'],
  'scope' => Permission::SCOPE_USER,
  'scopeid' => $adminhora->uid
);
$bank->createPermission($perm1);
$perm2 = array(
  'permission' => Permission::PERMISSION_ADMIN,
  'object' => 'exchange',
  'objectid' => $bcna['id'],
  'scope' => Permission::SCOPE_USER,
  'scopeid' => $adminbcna->uid
);
$bank->createPermission($perm2);

function registerUser($name){
  // register a new user
  if(!user_load_by_name($name)){
    $form_state = array();
    $form_state['values']['name'] = $name;
    $form_state['values']['mail'] = $name.'@test.com';
    $form_state['values']['pass']['pass1'] = 'password';
    $form_state['values']['pass']['pass2'] = 'password';
    $form_state['values']['op'] = t('Create new account');
    drupal_form_submit('user_register_form', $form_state);
  }
}


