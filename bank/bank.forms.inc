<?php

/**
Exchange form. Create, view and edit forms.
**/
function bank_exchange_form($form, &$form_state, $op, $exchangeId = NULL){
  $bank = new Bank();
  if(isset($form_state['values'])){//form rebuild
    $exchange = $form_state['values'];
  }else{
    if($op == 'new'){
      $exchange = $bank->getDefaultExchange();
    }else{
      $exchange = $bank->getExchange($exchangeId);      
    } 
  }
  
  $form['exchange'] = array(
    '#type' => 'vertical_tabs'
  );

  $form['exchange']['operation'] = array(
    '#type' => 'value',
    '#value' => $op
  );
  
  $form['exchange']['state'] = array(
    '#type' => 'value',
    '#value' => $exchange['state'],
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
  
  $form['exchange']['admin'] = array(
    '#type' => 'value',
    '#value' => $exchange['admin'],
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

  if($exchange['state'] != 0){ //exchange has been activated
    $form['exchange']['limits'] = array(
      '#type'=>'fieldset',
      '#title'=>t('Limits'),
    );
    
    $limitchains = $bank->getAllLimitChains($exchange['id']);
    $options = array();
    foreach($limitchains as $id=>$limit){
      $options[$id] = $limit['name'];
    }
    $form['exchange']['limits']['limitchain'] = array(
      '#type'=>'select',
      '#title'=>t('Default limit chain'),
      '#description'=>t('The limit chain for new accounts.'),
      '#options'=> $options,
    );
  }
  
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $op == 'activate'?t('Activate'):t('Submit'),
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
  
  
  //_bank_limitchain_fieldset_validate($form, $form_state['values']['limitchain']);
  
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
    if($op == 'activate'){
      $bank->activateExchange($form_state['values']);
      drupal_set_message(t('Exchange %code successfully activated. There has been sent an email to the new exchange administrator.', array('%code'=>$form_state['values']['code'])));
    }
  }catch(Exception $e){
    drupal_set_message(t('An error ocurred while saving the exchange record. Details:') ."\n". $e->getMessage(), 'error');
  }
}

function bank_limitchain_form($form, &$form_state, $op, $id){
  
  $bank = new Bank();
  if(!empty($form_state['values'])){//Form rebuild. Take values from last user input.
    $values = &$form_state['values'];
    $op = $values['operation'];
  }else{//First build. Take values from logic layer.
    if($op == 'new'){
      $values = $bank->getDefaultLimitChain($id);
    }else{
      $values = $bank->getLimitChain($id);
    }
  }

  $form = array(
    '#type' => 'fieldset',
    '#title' =>  t('Limit chain'),
    '#tree' => TRUE,
  );
  
  //save values: operation, exchange and id.
  $form['operation'] = array(
    '#type' => 'value',
    '#value' => $op
  );

  $form['exchange'] = array(
    '#type' => 'value',
    '#value' => $values['exchange'],
  );
  
  if($op != 'new'){
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $values['id'],
    );
  }
  
  $form['name'] = array(
    '#type' => 'textfield',
    '#description' => t('Name for this account limit'),
    '#title' => t('name'),
    '#default_value' => $values['name'],
    '#required' => TRUE
  );
  // Limits list
  $form['limits'] = array(
    '#type'=> 'fieldset',
    '#title' => t('Account limits'),
    '#description' => t('List of account limits currently belonging to this chain.'),
    
  );
  
  if(isset($values['limits'])){
    foreach($values['limits'] as $key => $value){
      $form['limits'][$key] = _bank_limitchain_limit_fieldset(array(),$values['limits'][$key]);
    }
  }
  //Add new limit to the chain
  $form['newlimit'] = array(
    '#type'=>'fieldset',
    '#title'=>t('Add account limit'),
    '#description'=>t('Add a new account limit to this limit chain.')
  );
  $form['newlimit']['classname'] = array(
    '#type'=>'select',
    '#title'=>t('Limit class'),
    '#options'=>$bank->getAccountLimitClasses(),
    '#description' => t('The type of limit to add. Each class limites a different aspect of the account balance or in a different way. Once added you will be able to configure its parameters.')
  );
  $form['newlimit']['value'] = array(
    '#type' => 'value',
    '#value' => '0',
  );
  $form['newlimit']['block'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );
  $form['newlimit']['operation'] = array(
    '#type' => 'value',
    '#value' => 'new',
  );
  
  $form['newlimit']['addlimit'] = array(
    '#type'=>'submit',
    '#value'=>t('Add limit'),
    '#submit'=>array('bank_limitchain_form_addlimit'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Helper function.
 */
function _bank_limitchain_limit_fieldset($form, &$form_values){
  if(!isset($form_values['operation']))
    $form_values['operation'] = 'edit';
  
  $bank = new Bank();
  $options = $bank->getAccountLimitClasses(); 
  $limit = array(
    '#type' => 'fieldset',
    '#title' => $options[$form_values['classname']],
    '#description' => $options[$form_values['classname']],
  );
  $limit['classname'] = array(
    '#type'=>'value',
    '#value'=>$form_values['classname'],
  );
  $limit['operation'] = array(
    '#type'=>'value',
    '#value'=>$form_values['operation'],
  );
  if($form_values['operation']!='new'){
    $limit['id'] = array(
      '#type'=>'value',
      '#value'=>$form_values['id'],
    );  
  }
  $limit['block'] = array(
    '#type'=>'checkbox',
    '#title'=>t('Block'),
    '#description'=>t('Block transactions when account reaches the limit vaule.'),
    '#default_value'=>$form_values['block'],
    
  );
  $limit['value'] = array(
    '#type'=>'textfield',
    '#title'=>t('Limit value'),
    '#description'=>t('Value for this limit class'),
    '#default_value'=>$form_values['value'], 
    '#required'=>TRUE,
  );
  $limit['deletelimit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit'=>array('bank_limitchain_form_deletelimit'),
  );
  
  return $limit;
}

function bank_limitchain_form_addlimit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
  
  $parents = $form_state['clicked_button']['#parents'];
  $value = &$form_state['values'];
  for($i=0;$i<count($parents)-2;$i++){
    $value = &$value[$parents[$i]];
  }
  
  $value['limits'][] = $value['newlimit'];
  
}

function bank_limitchain_form_deletelimit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
  $parents = $form_state['clicked_button']['#parents'];
  //go to the parent element of the clicked button and unset it!
  $value = &$form_state['values'];
  for($i=0;$i<count($parents)-2;$i++){
    $value = &$value[$parents[$i]];
    $key = $parents[$i+1];
  }
  unset($value[$key]);
}

function bank_limitchain_form_validate($form, &$form_state){
  $values = $form_state['values'];
  if(!empty($values['limits'])){
    foreach($values['limits'] as $key=>$limit){
      $amount = $limit['value'];
      if(!is_numeric($amount)){
        form_set_error('value', t('The limit amount must be numeric'));
      }
    }
  }
}

function bank_limitchain_fieldset_submit($form, &$form_state){
  $values = $form_state['values'];
  try{
    $bank = new Bank();
    if($form_values['operation'] == 'new'){//create new limit chain
      $bank->createLimitChain($values);
    }else if($form_values['operation'] == 'edit'){
      $bank->updateLimitChain($values);
    }
  }catch(Exception $e){
    drupal_set_message(t('Error occurred while updating the limit chain. Details: ')."\n".$e->getMessage(), 'error');
  }
}

function bank_limitchain_form_submit($form, &$form_state){
  $bank = new Bank();
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
  
  
  if($toAccountId != NULL){
    $form['toaccountname'] = array(
      '#type' => 'value',
      '#value' => $toaccount['name'],
    );
  }else{
    $form['toaccountname'] = array(
      '#title' => t('Seller account'),
      '#description' => t('The recipient of the transaction. The Exchange four letters code plus four numbers.'),
      '#type' => 'textfield',
      '#defalut_value' => $toaccount['name'],
    );
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
    $form['fromaccountname']['#default_value'] = !empty($form_state['values']['fromaccountname'])?$form_state['values']['fromaccountname']:'';
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
  $form['itemtoaccountname'] = array(
      '#type'=>'item',
      '#title'=>t('Seller account'),
      '#markup'=>$values['toaccountname'],
  );
  $form['itemfromaccountname'] = array(
      '#type'=>'item',
      '#title'=>t('Buyer account'),
      '#markup'=>$values['fromaccountname'],
  );
  $form['itemamount'] = array(
      '#type'=>'item',
      '#title'=>t('Amount'),
      '#markup'=>$values['amount'],
  );
  $form['itemconcept'] = array(
      '#type'=>'item',
      '#title'=>t('Description'),
      '#markup'=>$values['concept'],
  );
  $form['back'] = array(
      '#type'=>'submit',
      '#value'=>t('Back'),
      '#submit' => array('bank_transaction_confirm_form_back'),
      '#limit_validation_errors' => array(),
  );
  
  $form['submit'] = array(
      '#type'=>'submit',
      '#value'=>t('Submit'),
      '#submit'=> array('bank_transaction_confirm_form_submit'),
  );
  
  return $form;
}

function bank_transaction_confirm_form_back($form, &$form_state){
  $form_state['values'] = $form_state['page_values'][1];
  $form_state['page_num'] = 1;
  $form_state['rebuild'] = TRUE;
}

function bank_transaction_form_validate($form, &$form_state){
  if(!empty($form_state['page_num']) && $form_state['page_num'] == 2) return;
  
  $bank = new Bank();
  $record = $form_state['values'];
  
  $amount = $record['amount'];
  if(!is_numeric($amount)){
    form_set_error('amount', t('The amount entered is not a numeric value. Use the dot "." character for the decimal point and do not use any thousands separator.'));
    if(! $amount > 0.0 ){
      form_set_error('amount', t('The amount must be positive.'));
    }
  }
  $account1 = $record['toaccountname'];
  if($bank->getAccountByName($account1) === FALSE){
    form_set_error('toaccountname', t('Seller account %acc is incorrect.', array('%acc'=>$account1)));    
  }
  $account2 = $record['fromaccountname'];
  if($bank->getAccountByName($account2) === FALSE){
    form_set_error('fromaccountname', t('Buyer account %acc is incorrect.', array('%acc'=>$account2)));    
  }
  if($account1 == $account2){
    form_set_error('fromaccountname', t('Buyer account cannot be the same as seller account.'));
  }
  
}


function bank_transaction_confirm_form_submit($form, &$form_state){
  $bank = new Bank();
  $record = $form_state['page_values'][1];
  $transaction = array(
    'user' => $record['user'],
    'fromaccountname' => $record['fromaccountname'],
    'toaccountname' => $record['toaccountname'],
    'amount' => $record['amount'],
    'concept' => $record['concept'],
  );
  
  $bank->createTransaction($transaction);
  $messages = $bank->applyTransaction($transaction['id']);
  if($messages === TRUE) {
    drupal_set_message(t('Transaction successfully applied.'));
  }else{
    $text = t('The transaction has NOT been applied');
    $text .= '<br/>'.t('Details:').'<br/>';
    $text .= '<ul>';
    foreach($messages as $message){
      $text .= '<li>'.$message . '</li>';
    }
    $text .= '</ul>';;
    drupal_set_message($text, 'warning');
  }
  
}

/**
 * @return array page where to find the last transactions with this account.
 * @see bank_account_statement_form_submit()
 */
function bank_account_statement_form($form, &$form_state, $accountId){
  $bank = new Bank();
  $account = $bank->getAccount($accountId); //throws exception
  
  $form = array();
  //filter controls
  $form['controls'] = array(
    '#type' => 'fieldset',
    '#title' => t('Controls'),
  );
  if(isset($form_state['values']['from']) && isset($form_state['values']['to'])){
    $from = $form_state['values']['from'];
    $to = $form_state['values']['to'];
  }else{
    $lastmonth = mktime(0, 0, 0, date('m')-1, date('d'), date('Y'));
    $from = array(
      'year' => date('Y',$lastmonth),
      'month' => date('m', $lastmonth),
      'day' => date('d',$lastmonth),
    );
    $to = array(
      'year' => date('Y'),
      'month' => date('m'),
      'day' => date('d'),
    );
  }
  $form['controls']['from'] = array(
      '#type'=>'date',
      '#default_value' => $from,
      '#title'=>t('From'),
  );
  
  $form['controls']['to'] = array(
      '#type'=>'date',
      '#default_value' => $to,
      '#title'=>t('To'),
  );
  $form['controls']['submit'] = array(
      '#type'=>'submit',
      '#value'=>t('Submit'),
  );
  $timefrom = mktime(0,0,0,$from['month'], $from['day'], $from['year']);
  $timeto = mktime(0,0,0,$to['month'], $to['day'], $to['year']);
  
  //transaction table
  $form['transactions'] = array(
      '#type' => 'fieldset',
      '#title' => t('Transactions from %datefrom to %dateto', 
        array(
          '%datefrom'=>format_date($timefrom, $type = 'short'),
          '%dateto'=>format_date($timeto, $type = 'short'),
        )
      ),
  );
  
  $form['transactions']['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      array(
        'data'=>t('Date'),
        'field'=>'created',
        'sort'=>'asc'
      ),
      array(
        'data' => t('Account'),
      ),
      array(
        'data' => t('Description'),
      ),
      array(
        'data' => t('Credit'),
      ),
      array(
        'data' => t('Debit'),
      ),
      array(
        'data' => t('Balance'),
      ),
      array(
        'data' => t('Actions')
      ),
    ),
    '#rows' => array(),
  );
  
  $transactions = $bank->getTransactions(array('account'=>$account['id'], 'createdsince'=> $timefrom, 'createduntil'=> $timeto));
  
  foreach($transactions as $transaction){
    $row = array();
    $row[] = format_date($transaction['created']);
    
    if($transaction['fromaccount'] != $account['id']){
      $otherAccount = $bank->getAccount($transaction['fromaccount']);
      $type = 0;
    }else if($transaction['toaccount']!=$account['id']){
      $otherAccount = $bank->getAccount($transaction['toaccount']);
      $type = 1;
    }else{//This is impossible, but just in case.
      $otherAccount = $account;
      $type = 2;
    }
    $row[] = $otherAccount['name'];
    $row[] = check_plain($transaction['concept']);
    $row[] = $type==0?$transaction['amount']:'';
    $row[] = $type==1?$transaction['amount']:'';
    $row[] = ' - ';
    $row[] = l(t('Edit'), 'ces/bank/transaction/edit/'.$transaction['id']);
    
    $form['transactions']['table']['#rows'][] = $row;
  }
  
  return $form;
}

function bank_account_statement_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}