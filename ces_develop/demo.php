<?php
/**
 * @file
 * This file is a script for filling the database with initial data for
 * demo purposes. It empties the users and all ces tables and populates
 * it with 10 users with their respective accounts in two exchanges and
 * applies random transactions between them.
 */
include_once DRUPAL_ROOT . '/includes/install.inc';
require_once(drupal_get_path('module', 'ces_bank') . '/ces_bank.install');
require_once(drupal_get_path('module', 'ces_offerswants') . '/ces_offerswants.module');
//RESET USERS
$query = db_query('SELECT uid FROM {users}');
$users = $query->fetchAllAssoc('uid');
foreach( $users as $user) {
  if (((int)($user->uid)) > 1) {
    user_delete($user->uid);
  }
}
$query = db_query('SELECT nid FROM {node} WHERE type = \'ces_blog\'');
$posts = $query->fetchAllAssoc('nid');
foreach($posts as $post) {
  node_delete($post->nid);
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
if (db_table_exists('ces_offerwant')) {
  db_delete('ces_offerwant')->execute();
}
if (db_table_exists('ces_category')) {
  db_delete('ces_category')->execute();
}
ces_bank_install();
//CREATE STUFF
$bank = new Bank();

//CREATE USERS
$usernames = array('Riemann', 'Euclides', 'Gauss' , 'Noether', 'Fermat');
$users = array();
foreach ($usernames as $name) {
  $users[$name] = register_user($name);
}
//CREATE EXCHANGE
$net1 = array(
  'code' => 'NET1',
  'shortname' => 'Net 1',
  'name' => 'Network 1 - Time bank',
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
  'admin' => $users['Riemann']->uid,
  'data' => array(
    'registration_offers' => 1,
    'registration_wants' => 0,
  ),
);
$bank->createExchange($net1);
$bank->activateExchange($net1);
$net2 = array(
  'code' => 'NET2',
  'shortname' => 'Net 2',
  'name' => 'Network 2 - Euro based',
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
  'admin' => $users['Fermat']->uid,
  'data' => array(
    'registration_offers' => 0,
    'registration_wants' => 0,
  ),
);
$bank->createExchange($net2);
$bank->activateExchange($net2);
//CREATE ACCOUNTS
$accounts = array();
for ($i=0; $i<3; $i++) {
  $name = $usernames[$i];
  $accounts[$name] = register_account($users[$name], $net1, $i+1);
}
for ($i=0; $i<3; $i++) {
  $name = $usernames[$i+2];
  $accounts[$name] = register_account($users[$name], $net2, $i+1);
}
//CREATE LOCAL TRANSACTIONS
$transactions = array(
    array('NET10001', 'NET10002', 1.2, '3kg of potatoes.', $users['Euclides']->uid),
    array('NET10002', 'NET10003', 0.8, 'Standard haircut.', $users['Gauss']->uid),
    array('NET10003', 'NET10001', 2.1, 'Yearly website mantainment', $users['Riemann']->uid),
    array('NET20001', 'NET20002', 25, 'Bike revision.', $users['Noether']->uid),
    array('NET20002', 'NET20003', 6, 'Natural soap.', $users['Fermat']->uid),
    array('NET20003', 'NET20001', 5.5, 'Ecologic carrots', $users['Gauss']->uid),
);
foreach($transactions as $t) {
  $trans = array(
    'fromaccountname' => $t[0],
    'toaccountname' => $t[1],
    'amount' => $t[2],
    'concept' => $t[3],
    'user' => $t[4],
  );
  $bank->createTransaction($trans);
  $bank->applyTransaction($trans['id']);
}
//CREATE ONE INTER-TRANSACTION
$trans = array(
  'fromaccountname' => 'NET10001',
  'toaccountname' => 'NET20001',
  'amount' => '10',
  'concept' => 'Some old math books from Germany.',
  'user' => $users['Gauss']->uid,
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

//
// OFFERWANTS
//

//Add categories

$names = array('Food', 'Hygiene', 'Professional services', 'Reparation', 'Education');
$exchanges = array($net1, $net2);
$categories = array();
foreach($exchanges as $e){
  $categories[$e['id']] = array();
  foreach($names as $c){
    $cat = array(
      'parent' => 0,
      'title' => $c,
      'description' => $c,
      'exchange'=> $e['id'],
      'context' => 1,
    );
    $categories[$e['id']][$c] = ces_category_save((object)$cat);
  }
}

//Add some offers 

$offers = array(array(
  'type'=>'offer',
  'user'=>$users['Riemann']->uid, 
  'title'=>'Cow\'s milk', 
  'body'=>'Natural cow\'s milk. Probably the best you\'ve ever tasted.',
  'category'=>$categories[$net1['id']]['Food']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => '0.2',
),array(
  'type'=>'offer',
  'user'=>$users['Euclides']->uid, 
  'title'=>'Bicycle mechanic', 
  'body'=>'I fix or setup your bike in less than an hour.',
  'category'=>$categories[$net1['id']]['Reparation']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => '1h/hour',
),array(
  'type'=>'offer',
  'user'=>$users['Gauss']->uid, 
  'title'=>'Natural soap', 
  'body'=>'Natural soap with smell of Alpine flowers. Very good for your skin.',
  'category'=>$categories[$net1['id']]['Hygiene']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => '0.40',
),array(
  'type'=>'offer',
  'user'=>$users['Gauss']->uid, 
  'title'=>'Cow\'s milk', 
  'body'=>'Natural sheep\'s milk. Probably the best you\'ve ever tasted.',
  'category'=>$categories[$net2['id']]['Food']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => '2.5',
),array(
  'type'=>'offer',
  'user'=>$users['Noether']->uid, 
  'title'=>'Car mechanic', 
  'body'=>'I fix or setup your car in less than an hour.',
  'category'=>$categories[$net2['id']]['Reparation']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => 'it depends'
),array(
  'type'=>'offer',
  'user'=>$users['Fermat']->uid, 
  'title'=>'Natural shampoo', 
  'body'=>'Natural shampoo with smell of Pyrinee flowers. Very good for your hair.',
  'category'=>$categories[$net2['id']]['Hygiene']->id,
  'keywords'=>'',
  'state'=>1,
  'created' => time(),
  'modified' => time(),
  'expire' => time()+3600*24*365,
  'rate' => '6ECO each'
));
foreach($offers as $offer) {
  $o = (object)$offer;
  $o->ces_offer_rate = array('und' => array(array('value' => $offer['rate'])));
  unset($o->rate);
  ces_offerwant_save($o);
}

//Blog posts
post_blog('Demo post','This is a demonstration blog post. 

This space is intended for the administrator or a group of editors to publish relevant information about the trading community, such as markets, events and news.

Happy testing!', $net1, $users['Riemann']);

post_blog('Lorem ipsum', 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.', $net1, $users['Riemann']);
post_blog('May exchange newsletter', 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.', $net2, $users['Fermat']);
post_blog('Another post', 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.', $net2, $users['Fermat']);



function post_blog($subject, $body, $exchange, $user) {
  $node = new stdClass();
  $node->title = $subject;
  $node->type = 'ces_blog';
  node_object_prepare($node);
  $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
  $node->uid = $user->uid; 
  $node->status = 1; //(1 or 0): published or not
  $node->promote = 0; //(1 or 0): promoted to front page
  $node->comment = 2; //2 = comments on, 1 = comments off
  $node->ces_blog_exchange['und'][0]['value'] = $exchange['id'];
  $node->body['und'][0] = array(
    'summary'=> '',
    'value' => $body,
    'format' => 'filtered_html',
  );
  $node = node_submit($node); // Prepare node for saving
  node_save($node);
}

function register_user($name) {
  // register a new user
  if (!user_load_by_name($name)) {
    $form_state = array();
    $form_state['values']['name'] = $name;
    $form_state['values']['mail'] = strtolower($name) . '@integralces.net';
    $form_state['values']['pass']['pass1'] = 'integralces';
    $form_state['values']['pass']['pass2'] = 'integralces';
    $form_state['values']['ces_firstname']['und'][0]['value'] = $name;
    $form_state['values']['ces_town']['und'][0]['value'] = 'Somewhere';
    $form_state['values']['ces_postcode']['und'][0]['value'] = '12345';
    $form_state['values']['ces_phonemobile']['und']['0']['value'] = '123456789';
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