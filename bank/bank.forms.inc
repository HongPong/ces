<?php

/**
Exchange form. Create, view and edit forms.
**/
function bank_exchange_form($form, &$form_state, $op, $exchangeId = NULL){
  $bank = new Bank();

  if($op == 'edit' || $op == 'view'){
    try{
      $exchange = $bank->getExchange($exchangeId);
    }catch(Exception $e){
      drupal_set_message(t('Exchange id @id does not exist.', array('@id'=>$exchangeId)),'error');
      return $form;
    }
  }else if($op == 'new'){
    $exchange = $bank->getDefaultExchange();
  }
  
  $form['exchange'] = array(
    '#type' => 'vertical_tabs'
  );

  $form['exchange']['operation'] = array(
    '#type' => 'value',
    '#value' => $op
  );
  
  if($op != 'new'){
    $form['exchange']['id'] = array(
      '#type' => 'value',
      '#value' => $exchange['id'],
    );
  }
  
  $form['exchange']['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General')
  );

  $form['exchange']['general']['code'] = array(
    '#type' => 'textfield',
    '#description' => t('Four uppercase letters identifying this exchange.'),
    '#title' => t('Code'), 
    '#default_value' => $exchange['code'], 
    '#size' => 4,
    '#maxlength' => 4, 
    '#required' => TRUE,
  );

  $form['exchange']['general']['shortname'] = array(
    '#type' => 'textfield',
    '#description' => t('Acronym or short title. To be used in small-sized places like menus.'),
    '#title' => t('Short name'), 
    '#default_value' => $exchange['shortname'], 
    '#size' => 16,
    '#maxlength' => 32,
    '#required' => TRUE,
  );

  $form['exchange']['general']['name'] = array(
    '#type' => 'textfield',
    '#description' => t('It can be the same as the short name. To be used as title.'),
    '#title' => t('Full name'), 
    '#default_value' => $exchange['name'], 
    '#required' => TRUE,
  );

  $form['exchange']['general']['website'] = array(
    '#type' => 'textfield',
    '#description' => t('Website or blog of this exchange.'),
    '#title' => t('Website'), 
    '#default_value' => $exchange['website'], 
    '#required' => FALSE,
  );

  $form['exchange']['location'] = array(
    '#type' => 'fieldset',
    '#title' => t('Location')
  );
  
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  
  $form['exchange']['location']['country'] = array(
    '#type' => 'select',
    '#description' => t('The legal country of this exchange.'),
    '#title' => t('Country'), 
    '#default_value' => $exchange['country'], 
    '#options' => $countries,
    '#required' => TRUE,
  );
  
  $form['exchange']['location']['region'] = array(
    '#type' => 'textfield',
    '#description' => t('Region within the country.'),
    '#title' => t('Region'), 
    '#default_value' => $exchange['region'], 
    '#required' => TRUE,
  );
  
  $form['exchange']['location']['town'] = array(
    '#type' => 'textfield',
    '#description' => t('Main city or town.'),
    '#title' => t('Town'), 
    '#default_value' => $exchange['town'], 
    '#required' => TRUE,
  );

  $form['exchange']['location']['map'] = array(
    '#type' => 'textfield',
    '#description' => t('Paste here a Google Map URL pointing to your location. Something like http://maps.google.com/maps?...'),
    '#title' => t('Map'), 
    '#default_value' => $exchange['map'], 
    '#required' => TRUE,
  );

  if($op!='new'){
    $form['exchange']['limitchain'] = array(
      '#type' => 'fieldset',
      '#title' =>  t('Default limit chain'),
      '#tree' => TRUE,
    );

    $form['exchange']['limitchain']['values'] = bank_limitchain_form($form, $form_state, $op, $exchange['id'], $exchange['limitchain']);  
  }
  
  $form['exchange']['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency')
  );

  $form['exchange']['currency']['currencysymbol'] = array(
    '#type' => 'textfield',
    '#description' => t('Symbol for the currency of this exchange. It can be any unicode character.'),
    '#title' => t('Symbol'),
    '#default_value' => $exchange['currencysymbol'],
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 3, 
  );

  $form['exchange']['currency']['currencyname'] = array(
    '#type' => 'textfield',
    '#description' => t('Name for a single unit of this currency.'),
    '#title' => t('Name'),
    '#default_value' => $exchange['currencyname'],
    '#required' => TRUE,
    '#size' => 32,
    '#maxlength' => 60, 
  );

  $form['exchange']['currency']['currenciesname'] = array(
    '#type' => 'textfield',
    '#description' => t('Plural currency name.'),
    '#title' => t('Plural name'),
    '#default_value' => $exchange['currenciesname'],
    '#required' => TRUE,
    '#size' => 32,
    '#maxlength' => 60, 
  );

  $form['exchange']['currency']['currencyvalue'] = array(
    '#type' => 'textfield',
    '#description' => t('Value of this currency against the value of an average hour of labour. This is used to convert amounts between two currecies. If your currency worths 10u/hour then put "0.1". If you want to use your national currency as standard of value, look for the average cost of an our of labour or choose an approximation based on your community reality.'),
    '#title' => t('Value'),
    '#default_value' => $exchange['currencyvalue'],
    '#required' => TRUE,
    '#size' => 16,
    '#maxlength' => 60, 
  );

  $form['exchange']['currency']['currencyscale'] = array(
    '#type' => 'select',
    '#description' => t('Number of decimal digits to be shown.'),
    '#title' => t('Scale'),
    '#default_value' => $exchange['currencyscale'],
    '#options' => array('0'=>'0','1'=>'1','2'=>'2','3'=>'3','4'=>'4'),
    '#required' => TRUE
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function bank_exchange_form_validate($form, &$form_state){
  //code
  $code = $form_state['values']['code'];
  if(strlen($code)!=4)
    form_set_error('code', t('Code %code is invalid. Code must have exactly 4 letters.', array('%code'=>$code)));
  for($i=0;$i<strlen($code);$i++){
    $ord = ord(substr($code,$i,1));
    if($ord < ord('A') || $ord > ord('Z'))
      form_set_error('code', t('Code %code is invalid. Code characters must be uppercase ASCII letters. No accents, no spaces, no special characters.', array('%code'=> $code)));
  }
  //website
  $website = $form_state['values']['website'];
  if($website != '' && !valid_url($website, TRUE))
    form_set_error('website', t('The url %url is invalid. Please enter a fully qualified URL like http://www.example.org/something or leave the field blank.', array('%url'=>$website)));
  //map
  $map = $form_state['values']['map'];
  if(!valid_url($map, TRUE))
    form_set_error('website', t('The map url %url is invalid. Please enter a fully qualified URL like http://maps.google.com/....', array('%url'=>$map)));
  //currency value
  $currencyvalue = $form_state['values']['currencyvalue'];
  if(!is_numeric($currencyvalue))
    form_set_error('currencyvalue', t('Currency value %value is invalid. Enter a numeric value like 0.1, 0.0666666667 or 1', array('%value'=>$currencyvalue)));
  
  bank_limitchain_form_validate($form, $form_state);
  
}

function bank_exchange_form_submit($form, &$form_state){
  $bank = new Bank();
  $op = $form_state['values']['operation'];
  try{
    if( $op == 'new'){
      $bank->createExchange($form_state['values']);
      drupal_set_message(t('Exchange %code successfully created. You must wait for the system administrator to set up the exchange before it is operative. You will recive an e-mail notification within the next few days.', array('%code'=>$form_state['values']['code'])));
    }else if( $op == 'edit'){
      $bank->updateExchange($form_state['values']);
      drupal_set_message(t('Exchange %code successfully updated.',array('%code'=>$form_state['values']['code'])));
    }
  }catch(Exception $e){
    drupal_set_message(t('An error ocurred while saving the exchange record. Details:') ."\n". $e->getMessage(), 'error');
  }
}

function bank_limitchain_form($form, &$form_state, $op, $exchangeId, $id = NULL){
  $bank = new Bank();

  if($op == 'edit' || $op == 'view'){
    try{
      $limitchain = $bank->getLimitChain($id);
    }catch(Exception $e){
      drupal_set_message(t('Limit chain id @id does not exist.', array('@id'=>$id)),'error');
      return $form;
    }
  }else if($op == 'new'){
    $limitchain = $bank->getDefaultLimitChain($exchangeId);
  }
  
  $form['name'] = array(
    '#type' => 'textfield',
    '#description' => t('Name for this account limit'),
    '#title' => t('name'),
    '#default_value' => $limitchain['name'],
    '#required' => TRUE
  );

  $form['operation'] = array(
    '#type'=>'value',
    '#value'=>$op
  );

  $form['exchange'] = array(
    '#type' => 'value',
    '#value' => $limitchain['exchange']
  );
  $form['limits'] = array(
    '#type'=> 'fieldset',
    '#title' => t('Account limits'),
    '#description' => t('List of account limits currently belonging to this chain.')
  );
  foreach($limitchain['limits'] as $index=>$limit){
    $form['limits'][$index] = array(
      '#type'=>'fieldset',
      '#title'=> $limit['limitclass'],
      '#tree' => TRUE,
    );
    $form['limits'][$index]['limitclass'] = array(
      '#type'=>'value',
      '#value'=>$limit['limitclass']
    );
    $form['limits'][$index]['limit'] = array(
      '#type'=>'text',
      '#title'=>t('Limit value'),
      '#description'=>t('Value for this limit class'),
      '#default_value'=>$limit['limit'],
      
    );
    $form['limits'][$index]['block'] = array(
      '#type'=>'checkbox',
      '#title'=>t('Block'),
      '#description'=>t('Block transactions when account reaches the limit vaule.'),
      '#default_value'=>$limit['block'],
    );
    $form['limits'][$index]['up'] = array(
      '#type'=>'submit',
      '#title'=>t('Up'),
      '#submit'=>array('bank_limitchain_form_submit_uplimit'),
    );
    $form['limits'][$index]['down'] = array(
      '#type'=>'submit',
      '#title'=>t('Down'),
      '#submit'=>array('bank_limitchain_form_submit_downlimit')
    );
    $form['limits'][$index]['delete'] = array(
      '#type'=>'submit',
      '#title'=>t('Delete'),
      '#submit'=>array('bank_limitchain_form_submit_deletelimit')
    );
  }
  $form['newlimit'] = array(
    '#type'=>'fieldset',
    '#title'=>t('Add account limit'),
    '#description'=>t('Add a new account limit to this limit chain.')
  );
  $form['newlimit']['newlimitclass'] = array(
    '#type'=>'select',
    '#title'=>t('Limit class'),
    '#options'=>$bank->getAccountLimitClasses(),
    '#description' => t('The type of limit to add. Each class limites a different aspect of the account balance or in a different way. Once added you will be able to configure its parameters.')
  );
  $form['newlimit']['addlimit'] = array(
    '#type'=>'submit',
    '#value'=>t('Add limit'),
    '#submit'=>array('bank_limitchain_form_submit_addlimit')
  );

  return $form;
}

function bank_limitchain_form_validate($form, &$form_state){
  //TODO
}
function bank_limitchain_form_submit_addlimit($form, &$form_state){
  //TODO
}
function bank_limitchain_form_submit_deletelimit($form, &$form_state){
  //TODO
}
function bank_limitchain_form_submit_uplimit($form, &$form_state){
  //TODO
}
function bank_limitchain_form_submit_downlimit($form, &$form_state){
  //TODO
}
function bank_limitchain_form_submit($form, &$form_state){
  $op = $form_state['values']['operation'];
  try{
    if( $op == 'new'){
      $bank->createLimitChain($form_state['values']);
      drupal_set_message(t('Limit chain %name successfully created.', array('%name'=>$form_state['values']['name'])));
    }else if( $op == 'edit'){
      $bank->updateLimitChain($form_state['values']);
      drupal_set_message(t('Limit chain %name successfully updated.', array('%name'=>$form_state['values']['name'])));
    }
  }catch(Exception $e){
    drupal_set_message(t('An error ocurred while saving the record. Details:') ."\n". $e->getMessage(), 'error');
  }
}
/**
 * @param id either the id of an exchange (if $op == new) or the id of an account
 * otherwise.
 */
function bank_account_form($form, &$form_state, $op, $id){
  // TODO: Acess control to some fields.
  $bank = new Bank();
  
  
  if($op == 'new') {
    $exchange = $bank->getExchange($id);
    $account = $bank->getDefaultAccount($id);
  }else if($op == 'edit' || $op == 'view'){
    $account = $bank->getAccount($id);
    $exchange = $bank->getExchange($account['exchange']);
  }
  
  
  $form['account']['id'] = array(
    '#type' => 'value',
    '#value' => $account['id']
  );
  $form['account']['operation'] = array(
    '#type' => 'value',
    '#value' => $op
  );
  $form['account']['exchange'] = array(
    '#type' => 'value',
    '#value' => $account['exchange'],
  );
  $form['account']['balance'] = array(
    '#type' => 'value',
    '#value' => $account['balance'],
  );
  $form['account']['exchangename'] = array(
    '#type' => 'item',
    '#title' => t('Exchange'),
    '#markup' => $exchange['code']. ' - ' .$exchange['name'],
  );
  
  $form['account']['name'] = array(
    '#type' => 'textfield',
    '#description' => t('Four digits code.'),
    '#title' => t('name'),
    '#default_value' => $account['name'],
    '#required' => TRUE,
  );
  
  //TODO: multiple users
  $accuser = reset($account['users']);
  $accdrupaluser = user_load($accuser['user']);
  
  $form['account']['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Account administrator'),
    '#autocomplete' => 'user/autocomplete',
    '#default_value' => $accdrupaluser->name
  );
  
  $form['account']['kind'] = array(
    '#type'=>'select',
    '#title'=>t('Type'),
    '#description'=>t('Select "Individual" for personal use, "Shared" if several people share this account, "Organization" for a non-profit organization, "Company" for a profit-pursuing company and "Public" for a public account.'),
    '#options'=>array(
      0 => t('Individual'), 
      1 => t('Shared'), 
      2 => t('Organization'), 
      3 => t('Company'), 
      4 => t('Public')),
    '#required'=>TRUE,
    '#default_value' => $account['kind'],
  );
  
  $form['account']['state'] = array(
    '#type'=>'select',
    '#title'=>t('State'),
    '#description'=>t('Active is the common state.'),
    '#options'=>array(
      0 => t('Hidden'),
      1 => t('Active'),
      2 => t('Closed'),
      3 => t('Locked'),
    ),
    '#default_value' => $account['state'],
  );
  
  $limitchains = $bank->getAllLimitChains($exchange['id']);
  $options = array();
  foreach($limitchains as $id=>$limit){
    $options[$id] = $limit['name'];
  }
  
  $form['account']['limitchain'] = array(
    '#type'=>'select',
    '#title'=> t('Limit chain'),
    '#description'=>t('The limit chain to apply to this account'),
    '#options'=> $options,
    '#default_value' => $account['limitchain'],
  );
  
  $form['account']['submit'] = array(
    '#type'=>'submit',
    '#value' => t('Submit'),
  );
  
  return $form;
  
}
function bank_account_form_validate($form, &$form_state){
  //TODO
  $record = &$form_state['values'];
  $username = $record['username'];
  $op = $record['op'];
  $accuser = user_load_by_name($username);
  if($accuser === FALSE){
    form_set_error('username', t('Username %name does not exist.', array('%name'=>$username)));
  }
  $name = $record['name'];
  $bank = new Bank();
  $account = $bank->getAccountByName($name);
  if($op == 'new'){
    if($account != FALSE){
      form_set_error('name', t('Account number %name already exists.', array('%name'=>$name)));
    }
  }
}

function bank_account_form_submit($form, &$form_state){
  $record = $form_state['values'];
  $op = $record['operation'];
  $bank = new Bank();
  $username = $record['username'];
  $accuser = user_load_by_name($username);
  $record['users'] = array(
    array(
      'user' => $accuser->uid,
      'role' => 0,
      'account' => $record['id'],
    ),
  );
  
  try{
    if($op == 'new'){
      $bank->createAccount($record);
      drupal_set_message(t('Account successfully created. The account will not be available until it is being activated by this exchange\'s administrator. You will recieve an email when this is done.'));
    }else if($op == 'edit'){
      $bank->updateAccount($record);
      drupal_set_message(t('Account sucessfully updated.'));
    }else if($op == 'delete'){
      $bank->deleteAccount($record['id']);
    }
  }catch(Exception $e){
    drupal_set_message(t('An error occurred while saving the account record. Details: '."\n".$e->getMessage()), 'error');
  }
}

/**
 * Single transaction form
 */
function bank_transaction_form($form, &$form_state, $toAccountId = NULL, $fromAccountId = NULL){
  
  if (!empty($form_state['page_num']) && $form_state['page_num'] == 2) {
    return bank_transaction_confirm_form($form, $form_state);
  }

  //TODO
  // 1: Set the currency according the toaccount exchage currency.
  // 2: Make an easy system to find account names by exchange/username.
  global $user;
  
  $bank = new Bank();
  if($toAccountId == NULL){
    $accounts = $bank->getUserAccounts($user->uid);
    $toaccount = reset($accounts);
  }else{
    $toaccount = $bank->getAccount($toAccountId);
  }
  
  $exchange = $bank->getExchange($toaccount['exchange']);
  
  $form['toaccountname'] = array(
      '#title' => t('Seller account'),
      '#description' => t('The recipient of the transaction. The Exchange four letters code plus four numbers.'),
  );
  if($toAccountId != NULL){
    $form['toaccountname']['#type'] = 'item';
    $form['toaccountname']['#markup'] = $toaccount['name'];
  }else{
    $form['toaccountname']['#type'] = 'textfield';
    $form['toaccountname']['#defalut_value'] = $toaccount['name'];
  }
  
  $form['fromaccountname'] = array(
      '#title' => t('Buyer account'),
      '#description' => t('The payer of the transaction. The Exchange four letters code plus four numbers.'),
  );
  if($fromAccountId != NULL){
    $fromaccount = $bank->getAccount($fromAccountId);
    $form['fromaccountname']['#type'] = 'item';
    $form['fromaccountname']['#markup'] = $fromaccount['name'];
  }else{
    $form['fromaccountname']['#type'] = 'textfield';
    $form['fromaccountname']['#default_value'] = '';
  }
  $form['concept']= array(
    '#type'  => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Enter the concept of the transference.'),
    '#default_value' => !empty($form_state['values']['concept'])?$form_state['values']['concept']:'',
  );
  $form['amount'] = array(
      '#type'=>'textfield',
      '#size'=>'5',
      '#title'=>t('Amount in ').$exchange['currenciesname'],
      '#description'=>t('The amount to be transferred.'),
      '#default_value' => !empty($form_state['values']['amount'])?$form_state['values']['amount']:'',
  );
  
  $form['user'] = array(
      '#type'=>'value',
      '#value'=>$user->uid,
  );
  $form['submit'] = array(
      '#type'=>'submit',
      '#value'=>t('Submit'),
  );
  return $form;
}

function bank_transaction_form_submit($form, &$form_state){
  $form_state['page_values'][1] = $form_state['values'];

  if (!empty($form_state['page_values'][2])) {
    $form_state['values'] = $form_state['page_values'][2];
  }

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

function bank_transaction_confirm_form($form, &$form_state){
  $values = $form_state['page_values'][1];
  $form['accountname'] = array(
      '#type'=>'item',
      '#title'=>t('Seller account'),
      '#markup'=>$values['toaccountname'],
  );
  $form['fromaccountname'] = array(
      '#type'=>'item',
      '#title'=>t('Buyer account'),
      '#markup'=>$values['fromaccountname'],
  );
}

function bank_transaction_form_validate($form, &$form_state){
  //TODO
  $record = $form_state['values'];
  $amount = $record['amount'];
  if(!is_numeric($amount)){
    form_set_error('amount', t('The amount entered is not a numeric value. Use the dot "." character for the decimal point and do not use any thousands separator.'));
  }
  
  
  
}


function bank_transaction_form_submit($form, &$form_state){
  $bank = new Bank();
  $transaction = array();
  
  $bank->createTransaction($transaction);
}