<?php
/**
 * @file
 * Forms definition, submit and validation functions for ces_bank module.
 */

/**
 * @defgroup ces_bank_forms Forms from Ces Bank
 * @ingroup ces_bank
 * @{
 * Forms definition.
 */

/**
 * Exchange form. Create, view and edit forms.
 *
 * @param array  $form
 *   Form.
 * @param array  $form_state
 *   State from.
 * @param string $op
 *   Options.
 * @param int    $exchange_id
 *   Id of exchange.
 */
function ces_bank_exchange_form($form, &$form_state, $op, $exchange_id = NULL) {
  global $user;
  $bank = new Bank();
  if (isset($form_state['values'])) {
    // Form rebuild.
    $exchange = $form_state['values'];
  }
  else {
    if ($op == 'new') {
      $exchange = $bank->getDefaultExchange();
    }
    elseif ($exchange_id) {
      $exchange = $bank->getExchange($exchange_id);
    }
    else {
      return array(
        'message' => array(
          '#markup' => '<p>' . t('Invalid exchange id.') . '</p>',
        ),
      );
    }
    // We change the exchange field name from "name" to "fullname" so it doesn't
    // conflict with user's form "name" field.
    $exchange['fullname'] = $exchange['name'];
    unset($exchange['name']);
    $form_state['values'] = $exchange;
  }

  $form['exchange'] = array(
    '#type' => 'vertical_tabs',
    '#description' => t('Settings for an individual exchange in Community Exchange System.'),
  );

  $form['exchange']['operation'] = array(
    '#type' => 'value',
    '#value' => $op,
  );

  $form['exchange']['state'] = array(
    '#type' => 'value',
    '#value' => isset($exchange['state']) ? $exchange['state'] : 0,
  );

  if ($op != 'new') {
    $form['exchange']['id'] = array(
      '#type' => 'value',
      '#value' => $exchange['id'],
    );
  }

  $form['exchange']['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
    '#description' => t('General settings for this exchange.'),
  );

  $form['exchange']['general']['code'] = array(
    '#type' => 'textfield',
    '#description' => t('Four numbers or uppercase letters identifying this exchange.'),
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

  $form['exchange']['general']['fullname'] = array(
    '#type' => 'textfield',
    '#description' => t('It can be the same as the short name. To be used as title.'),
    '#title' => t('Full name'),
    '#default_value' => $exchange['fullname'],
    '#required' => TRUE,
  );

  $form['exchange']['general']['website'] = array(
    '#type' => 'textfield',
    '#description' => t('Website or blog of this exchange. Include http:// or https://'),
    '#title' => t('Website'),
    '#default_value' => $exchange['website'],
    '#required' => FALSE,
  );

  // Create new exclusive user for administer this exchange. Later it will be
  // editable from the standard user form.
  if ($op == 'new') {
    $form['exchange']['admin'] = array(
      '#type' => 'fieldset',
      '#title' => t('Administrator'),
      '#description' => t('Administrator settings for this exchange. One Drupal user is the administrator. It is recommended not to use a real person name here.'),
    );
    $form['exchange']['admin'] = user_register_form($form['exchange']['admin'], $form_state);
    unset($form['exchange']['admin']['actions']);
  }
  else {
    $form['exchange']['admin'] = array(
      '#type' => 'value',
      '#value' => $exchange['admin'],
      '#description' => t('Administrator settings for this exchange. One Drupal user is the administrator.'),
    );
  }

  $form['exchange']['location'] = array(
    '#type' => 'fieldset',
    '#title' => t('Location'),
    '#description' => t('Location settings for this exchange.'),
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
    '#required' => FALSE,
    '#maxlength' => 200,
  );

  $form['exchange']['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency'),
    '#description' => t('Currency settings for this exchange.'),
  );

  $form['exchange']['currency']['currencysymbol'] = array(
    '#type' => 'textfield',
    '#description' => t('Symbol for the currency of this exchange. It can be any 3 unicode characters.'),
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
    '#description' => t('Value of this currency against the value of an average hour of labor. This is used to convert amounts between two currencies. If your currency is worth 10u/hour then put "0.1". If you want to use your national currency as standard of value, look for the average cost of an hour of labor or choose an approximation based on your community reality.'),
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
    '#options' => array(
      '0' => t('0'),
      '1' => t('1'),
      '2' => t('2'),
      '3' => t('3'),
      '4' => t('4')),
    '#required' => TRUE,
  );
  $form['exchange']['users'] = array(
    '#type' => 'fieldset',
    '#title' => t('Members'),
    '#description' => t('Settings for users registering with exchange. Sub-modules add settings here.'),
  );
  if (!empty($exchange['state'])) {
    // Exchange has been activated.
    $limitchains = $bank->getAllLimitChains($exchange['id']);
    $options = array();
    foreach ($limitchains as $id => $limit) {
      $options[$id] = $limit['name'];
    }
    $form['exchange']['users']['limitchain'] = array(
      '#type' => 'select',
      '#title' => t('Default limit chain'),
      '#description' => t('The limit chain for new accounts.'),
      '#options' => $options,
      '#default_value' => $exchange['limitchain'],
    );
  }
  switch ($op) {
    case 'activate':
      $value = t('Activate exchange');
      break;

    case 'new':
      $value = t('Create exchange');
      break;

    default:
      $value = t('Update exchange');
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $value,
  );

  return $form;
}
/**
 * Validates bank exchange form for new or existing exchanges.
 * @see ces_bank_exchange_form_submit()
 */
function ces_bank_exchange_form_validate($form, &$form_state) {
  // Code.
  $code = $form_state['values']['code'];
  if (drupal_strlen($code) != 4) {
    form_set_error('code', t('Code %code is invalid. Code must have exactly 4 letters.', array('%code' => $code)));
  }
  for ($i = 0; $i < drupal_strlen($code); $i++) {
    $ord = ord(drupal_substr($code, $i, 1));
    if (($ord < ord('A') || $ord > ord('Z')) && ($ord < ord('0') || $ord > ord('9'))) {
      form_set_error('code', t('Code %code is invalid. Code characters must be uppercase ASCII letters and numbers. No accents, no spaces, no special characters.', array('%code' => $code)));
    }
  }
  // Website.
  $website = $form_state['values']['website'];
  if ($website != '' && !valid_url($website, TRUE)) {
    form_set_error('website', t('The url %url is invalid. Please enter a fully qualified URL like http://www.example.org/something or leave the field blank.', array('%url' => $website)));
  }
  // Map.
  $map = $form_state['values']['map'];
  if (!valid_url($map, TRUE)) {
    form_set_error('website', t('The map url %url is invalid. Please enter a fully qualified URL like http://maps.google.com/....', array('%url' => $map)));
  }
  // Currency value.
  $currencyvalue = $form_state['values']['currencyvalue'];
  if (!is_numeric($currencyvalue)) {
    form_set_error('currencyvalue', t('Currency value %value is invalid. Enter a numeric value like 0.1, 0.0666666667 or 1', array('%value' => $currencyvalue)));
  }
  if ($form_state['values']['operation'] == 'new') {
    user_register_validate($form['exchange']['admin'], $form_state);
    user_account_form_validate($form['exchange']['admin'], $form_state);
  }
}
/**
 * Processes bank exchange form for new or existing exchanges.
 */
function ces_bank_exchange_form_submit($form, &$form_state) {
  $bank = new Bank();
  $op = $form_state['values']['operation'];
  try {
    if ($op == 'new') {
      // Create admin user.
      user_register_submit($form['exchange']['admin'], $form_state);
      // Set the new user as the exchange administrator.
      $form_state['values']['admin'] = $form_state['values']['uid'];
    }
    $form_state['values']['name'] = $form_state['values']['fullname'];
    if ($op == 'new') {
      $bank->createExchange($form_state['values']);
      drupal_set_message(t('Exchange %code successfully created. You must wait for the system administrator to set up the exchange before it is operative. You will receive an e-mail notification within the next few days.', array('%code' => $form_state['values']['code'])));
    }
    elseif ($op == 'edit') {
      $bank->updateExchange($form_state['values']);
      drupal_set_message(t('Exchange %code successfully updated.', array('%code' => $form_state['values']['code'])));
    }
    elseif ($op == 'activate') {
      $bank->activateExchange($form_state['values']);
      // Activate admin Drupal user.
      $admin = user_load($form_state['values']['admin']);
      $admin->status = 1;
      user_save($admin);
      // Clear cache so register block includes the new exchange.
      cache_clear_all('ces_bank_countries', 'cache');
      drupal_set_message(t('Exchange %code successfully activated. There has been sent an email to the new exchange administrator.', array('%code' => $form_state['values']['code'])));
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while saving the exchange record. Details: %msg', array('msg' => $e->getMessage())), 'error');
  }
}
/**
 * Form for controlling limit chains.
 */
function ces_bank_limitchain_form($form, &$form_state, $limitchain = NULL) {
  $bank = new Bank();
  if (!empty($form_state['values'])) {
    // Form rebuild. Take values from last user input.
    $limitchain = &$form_state['values'];
    $op = $form_state['values']['operation'];
  }
  else {
    // First build. Take values from logic layer.
    if ($limitchain == NULL) {
      $exchange = ces_bank_get_current_exchange();
      if ($exchange === FALSE) {
        return array(
          'message' => array(
            '#markup' => '<p>' . t('Invalid current exchange') . '</p>',
          ),
        );
      }
      $limitchain = $bank->getDefaultLimitChain($exchange['id']);
      $limitchain['name'] = '';
      $op = 'new';
    }
    else {
      $op = 'edit';
    }
  }

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Limit chain'),
    '#tree' => TRUE,
  );

  // Save values: operation, exchange and id.
  $form['operation'] = array(
    '#type' => 'value',
    '#value' => $op,
  );

  $form['exchange'] = array(
    '#type' => 'value',
    '#value' => $limitchain['exchange'],
  );

  if ($op != 'new') {
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $limitchain['id'],
    );
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#description' => t('Name for this account limit'),
    '#title' => t('name'),
    '#default_value' => $limitchain['name'],
    '#required' => TRUE,
  );
  // Limits list.
  $form['limits'] = array(
    '#type' => 'fieldset',
    '#title' => t('Account limits'),
    '#description' => t('List of account limits currently belonging to this chain.'),
  );

  if (isset($limitchain['limits'])) {
    foreach ($limitchain['limits'] as $key => $value) {
      $form['limits'][$key] = _ces_bank_limitchain_limit_fieldset(array(), $limitchain['limits'][$key]);
    }
  }
  // Add new limit to the chain.
  $form['newlimit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add account limit'),
    '#description' => t('Add a new account limit to this limit chain.'),
  );
  $form['newlimit']['classname'] = array(
    '#type' => 'select',
    '#title' => t('Limit class'),
    '#options' => module_invoke_all('account_limit_classes'),
    '#description' => t('The type of limit to add. Each class limits a different aspect of the account balance or in a different way. Once added, you will be able to configure its parameters.'),
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
    '#type' => 'submit',
    '#value' => t('Add limit'),
    '#submit' => array('ces_bank_limitchain_form_addlimit'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $op == 'new' ? t('Create limit chain') : t('Update limit chain'),
  );
  return $form;
}
/**
 * Helper function.
 */
function _ces_bank_limitchain_limit_fieldset($form, &$form_values) {
  if (!isset($form_values['operation'])) {
    $form_values['operation'] = 'edit';
  }
  $options = module_invoke_all('account_limit_classes');
  $limit = array(
    '#type' => 'fieldset',
    '#title' => $options[$form_values['classname']],
    '#description' => $options[$form_values['classname']],
  );
  $limit['classname'] = array(
    '#type' => 'value',
    '#value' => $form_values['classname'],
  );
  $limit['operation'] = array(
    '#type' => 'value',
    '#value' => $form_values['operation'],
  );
  if ($form_values['operation'] != 'new') {
    $limit['id'] = array(
      '#type' => 'value',
      '#value' => $form_values['id'],
    );
  }
  $limit['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Block'),
    '#description' => t('Block transactions when account reaches the limit vaule.'),
    '#default_value' => $form_values['block'],
  );
  $limit['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Limit value'),
    '#description' => t('Value for this limit class'),
    '#default_value' => $form_values['value'],
    '#required' => TRUE,
  );
  $limit['deletelimit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('ces_bank_limitchain_form_deletelimit'),
  );
  return $limit;
}
/**
 * TODO: Document.
 */
function ces_bank_limitchain_form_addlimit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $parents = $form_state['clicked_button']['#parents'];
  $value = &$form_state['values'];
  for ($i = 0; $i < count($parents) - 2; $i++) {
    $value = &$value[$parents[$i]];
  }
  $value['limits'][] = $value['newlimit'];
}
/**
 * Delete a limit in the form and set flag to rebuild form.
 */
function ces_bank_limitchain_form_deletelimit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $parents = $form_state['clicked_button']['#parents'];
  // Go to the parent element of the clicked button and unset it!
  $value = &$form_state['values'];
  for ($i = 0; $i < count($parents) - 2; $i++) {
    $value = &$value[$parents[$i]];
    $key = $parents[$i + 1];
  }
  unset($value[$key]);
}
/**
 * Validate the limit chain form.
 */
function ces_bank_limitchain_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  if (!empty($values['limits'])) {
    foreach ($values['limits'] as $key => $limit) {
      $amount = $limit['value'];
      if (!is_numeric($amount)) {
        form_set_error('value', t('The limit amount must be numeric.'));
      }
      if ($limit['classname'] == 'AbsoluteCreditLimit') {
        if ($amount < 0) {
          form_set_error('value', t('The credit limit must not be negative.'));
        }
      }
      elseif ($limit['classname'] == 'AbsoluteDebitLimit') {
        if ($amount > 0) {
          form_set_error('value', t('The debit limit must not be positive.'));
        }
      }
    }
  }
}
/**
 * Submit the limit chain form, for new or current limit chains.
 */
function ces_bank_limitchain_form_submit($form, &$form_state) {
  $bank = new Bank();
  $op = $form_state['values']['operation'];
  try {
    if ($op == 'new') {
      $bank->createLimitChain($form_state['values']);
      drupal_set_message(t('Limit chain %name successfully created.', array('%name' => $form_state['values']['name'])));
      $form_state['redirect'] = 'ces/admin/limit';
    }
    elseif ($op == 'edit') {
      $bank->updateLimitChain($form_state['values']);
      drupal_set_message(t('Limit chain %name successfully updated.', array('%name' => $form_state['values']['name'])));
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while saving the record. Details: %msg', array('%msg' => $e->getMessage())), 'error');
  }
}
/**
 * Main bank account form.
 *
 * @todo other operations.
 */
function ces_bank_account_form($form, &$form_state, $account = NULL) {
  $bank = new Bank();
  if ($account != NULL) {
    if (is_int($account)) {
      $account = $bank->getAccount($account);
    }
    $exchange = $bank->getExchange($account['exchange']);
  }
  else {
    $exchange = ces_bank_get_current_exchange();
    if ($exchange === FALSE) {
      return array(
        'message' => array(
          '#markup' => '<p>' . t('Invalid current exchange') . '</p>',
        ),
      );
    }
    $account = $bank->getDefaultAccount($exchange['id']);
  }
  $admin = ces_bank_access('admin', 'exchange', $exchange['id']);

  if (!empty($form_state['values'])) {
    $account = array_merge($account, $form_state['values']);
  }

  $form['bankaccount'] = array();
  if (isset($account['id'])) {
    $form['bankaccount']['id'] = array(
      '#type' => 'value',
      '#value' => $account['id'],
    );
  }
  $form['bankaccount']['exchangename'] = array(
    '#type' => 'item',
    '#title' => t('Exchange'),
    '#markup' => $exchange['code'] . ' - ' . $exchange['name'],
  );
  $form['bankaccount']['exchange'] = array(
    '#type' => 'value',
    '#value' => $exchange['id'],
  );
  $form['bankaccount']['name'] = array(
    '#type' => 'textfield',
    '#description' => t('Four digits code.'),
    '#title' => t('Code'),
    '#default_value' => $account['name'],
    '#required' => TRUE,
    '#access' => $admin,
  );
  // Account users.
  $form['bankaccount']['users'] = array(
    '#tree' => TRUE,
    '#access' => $admin,
  );
  foreach ($account['users'] as $accuser) {
    $accuserform = array();
    if (isset($accuser['id'])) {
      $accuserform['id'] = array(
        '#type' => 'value',
        '#value' => $accuser['id'],
      );
    }
    if (!empty($accuser['user'])) {
      $accuserform['user'] = array(
        '#type' => 'value',
        '#value' => $accuser['user'],
      );
    }
    else {
      $accuserform['username'] = array(
        '#title' => t('Username'),
        '#description' => t('The username of the owner of this account.'),
        '#type' => 'textfield',
        '#autocomplete_path' => 'ces/bank/user/autocomplete',
        '#default_value' => isset($accuser['username']) ? $accuser['username'] : '',
      );
    }
    $accuserform['privilege'] = array(
      '#type' => 'value',
      '#value' => 0,
    );
    $form['bankaccount']['users'][] = $accuserform;
  }

  $options = array(
    0 => t('Individual'),
    1 => t('Shared'),
    2 => t('Organization'),
    3 => t('Company'),
    4 => t('Public'),
  );
  if ($account['kind'] == 5) {
    $options[5] = t('Virtual');
  }
  $form['bankaccount']['kind'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#description' => t('Select "Individual" for personal use, "Shared" if several people share this account, "Organization" for a non-profit organization, "Company" for a profit-pursuing company and "Public" for a public account.'),
    '#options' => $options,
    '#required' => TRUE,
    '#default_value' => $account['kind'],
  );
  if ($account['kind'] == 5) {
    $form['bankaccount']['kind']['#disabled'] = TRUE;
  }
  $limitchains = $bank->getAllLimitChains($exchange['id']);
  $options = array();
  foreach ($limitchains as $id => $limit) {
    $options[$id] = $limit['name'];
  }
  $form['bankaccount']['limitchain'] = array(
    '#type' => 'select',
    '#title' => t('Limit chain'),
    '#description' => t('The limit chain to apply to this account'),
    '#options' => $options,
    '#default_value' => $account['limitchain'],
    '#access' => $admin,
    '#required' => TRUE,
  );
  $form['bankaccount']['state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#description' => t('Active is the common state.'),
    '#options' => array(
      0 => t('Hidden'),
      1 => t('Active'),
      2 => t('Closed'),
      3 => t('Locked'),
    ),
    '#default_value' => $account['state'],
    '#access' => $admin,
    '#required' => TRUE,
  );

  $form['bankaccount']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}
/**
 * Validate main CES bank account form.
 */
function ces_bank_account_form_validate($form, &$form_state) {
  $record = &$form_state['values'];
  // Validate username.
  $users = $record['users'];
  foreach ($users as $key => $accuser) {
    if (isset($accuser['username'])) {
      $user = user_load_by_name($accuser['username']);
      if ($user === FALSE) {
        form_set_error('username', t('Username %name does not exist.', array('%name' => $accuser['username'])));
      }
      else {
        $record['users'][$key]['user'] = $user->uid;
      }
    }
  }
  // Validate account number.
  $name = $record['name'];
  $bank = new Bank();
  $account = $bank->getAccountByName($name);
  if ($account != FALSE && (!isset($record['id']) || $account['id'] != $record['id'])) {
    form_set_error('name', t('Account number %name already exists.', array('%name' => $name)));
  }
}
/**
 * Submit bank account form.
 */
function ces_bank_account_form_submit($form, &$form_state) {
  $record = &$form_state['values'];
  foreach ($record['users'] as $key => $accuser) {
    if (isset($record['id'])) {
      $record['users'][$key]['account'] = $record['id'];
    }
  }
  $bank = new Bank();
  try {
    $activate = FALSE;
    if (!isset($record['id'])) {
      // New account.
      $bank->createAccount($record);
      drupal_set_message(t('Account successfully created.'));
      if ($record['state'] == LocalAccount::STATE_HIDDEN) {
        drupal_set_message(t("The account will not be available until it is being activated by this exchange's administrator. You will receive an email when this is done."));
      }
      elseif ($record['state'] == LocalAccount::STATE_ACTIVE) {
        $activate = TRUE;
      }
    }
    else {
      $previous = $bank->getAccount($record['id']);
      $bank->updateAccount($record);
      if ($previous['state'] == LocalAccount::STATE_HIDDEN && $record['state'] == LocalAccount::STATE_ACTIVE) {
        $activate = TRUE;
      }
    }
    if ($activate) {
      // Activate Drupal user(s).
      foreach ($record['users'] as $key => $accuser) {
        $user = user_load($accuser['user']);
        if ($user->status == 0) {
          $user->status = 1;
          user_save($user);
        }
      }
      // Activate banking account.
      $bank->activateAccount($record);
      drupal_set_message(t('Account successfully activated.'));
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while saving the account record. Details: %msg', array('%msg' => $e->getMessage())), 'error');
  }
}
/**
 * Form for deleting an account in an exchange.
 *
 * @todo Implement delete account
 */
function ces_bank_account_delete_form($form, &$form_state, $account = NULL) {
  $bank = new Bank();
  if ($account != NULL) {
    if (!is_array($account)) {
      $account = $bank->getAccount($account);
    }
    $exchange = $bank->getExchange($account['exchange']);
  }
  $admin = ces_bank_access('admin', 'exchange', $exchange['id']);
  $form['bankaccount'] = array();
  $form['bankaccount']['title'] = array(
    '#type' => 'item',
    '#title' => t('Delete Exchange Account'),
    '#markup' => t('Deleting accounts is not implemented yet.'),
  );
  return $form;
}

/**
 * Single transaction form.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The form input values.
 * @param string $op
 *   The operation to perform on the transaction:
 *     - 'new': Create new transaction.
 *     - 'edit': Edit transaction.
 * @param array|int $arg
 *   Argument to this form depending on the operation. If the operation is 'new'
 *   $arg is the id of the exchange where the transaction is going to take
 *   place. If the operation is 'edit', $arg is the transaction array.
 * @param int $to_account_id
 *   Optional. The seller account id.
 * @param int $from_account_id
 *   Optional. The buyer account.
 *
 * @todo Set the currency according the toaccount exchange currency.
 * @todo Make an easy system to find account names by exchange/username.
 */
function ces_bank_transaction_form($form, &$form_state, $op, $arg, $to_account_id = NULL, $from_account_id = NULL) {
  if (!empty($form_state['values']) && !empty($form_state['values']['page'])
      && $form_state['values']['page'] == 'confirm') {
    return ces_bank_transaction_confirm_form($form, $form_state, $form_state['values']['operation']);
  }
  global  $user;
  $bank = new Bank();
  // Exchange options for selects.
  $exchanges = $bank->getAllExchanges();
  if (empty($exchanges)) {
    return array(
      'message' => array(
        '#markup' => '<p>' . t('There are no exchanges yet.') . '</p>',
      ),
    );
  }
  $exoptions = array();
  foreach ($exchanges as $exchange) {
    $exoptions[$exchange['id']] = $exchange['code'];
  }
  if ($op == 'new') {
    $exchangeid = $arg;
    if (!isset($exchanges[$exchangeid])) {
      return array(
        'message' => array(
          '#markup' => '<p>' . t('Invalid exchange id.') . '</p>',
        ),
      );
    }
  }
  elseif ($op == 'edit') {
    $from = $bank->getTransactionFromAccount($arg);
    $to = $bank->getTransactionToAccount($arg);
    $toexchange = $exchanges[$to['exchange']];
    $form_state['values']['toexchange'] = $to['exchange'];
    $form_state['values']['toaccountname'] = $to['name'];
    $form_state['values']['fromexchange'] = $from['exchange'];
    $form_state['values']['fromaccountname'] = $from['name'];
    $form_state['values']['concept'] = $arg['concept'];
    $form_state['values']['amount'] = $bank->formatAmount($bank->getTransactionAmount($arg, $toexchange), $toexchange, FALSE, FALSE);
    $form_state['values']['id'] = $arg['id'];
    $form_state['values']['user'] = $arg['user'];
  }
  $form['operation'] = array(
    '#type' => 'value',
    '#value' => $op,
  );
  if (isset($form_state['values']['id'])) {
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $form_state['values']['id'],
    );
  }
  // Select seller's exchange.
  $toexchangeid = isset($form_state['values']['toexchange']) ? $form_state['values']['toexchange'] : $exchangeid;
  $toexchange = $exchanges[$toexchangeid];
  if ($to_account_id == NULL) {
    $form['toexchange'] = array(
      '#title' => t('Seller exchange'),
      '#description' => t("The seller's account exchange."),
      '#type' => 'select',
      '#options' => $exoptions,
      '#default_value' => $toexchangeid,
      '#ajax' => array(
        'callback' => 'ces_bank_toaccountname_dropdown_callback',
        'wrapper' => 'dropdown-toaccountname-replace',
      ),
      '#weight' => 30,
    );
    // Select seller's account.
    $accountconditions = array(
      'exchange' => $toexchangeid,
      'state' => 1,
      'kind' => array(
        LocalAccount::TYPE_INDIVIDUAL,
        LocalAccount::TYPE_SHARED,
        LocalAccount::TYPE_ORGANIZATION,
        LocalAccount::TYPE_COMPANY,
        LocalAccount::TYPE_PUBLIC,
      ));
    $accounts = $bank->getAllAccounts($accountconditions);
    $tooptions = array();
    foreach ($accounts as $account) {
      $useracc = reset($account['users']);
      $tooptions[$account['name']] = $account['name'] . ' - ' . ces_user_get_name(user_load($useracc['user']));
    }
    $form['toaccountname'] = array(
      '#title' => t('Seller account'),
      '#description' => t('The recipient of the transaction. The Exchange four letters code plus four numbers.'),
      '#type' => 'select',
      '#options' => $tooptions,
      '#prefix' => '<div id="dropdown-toaccountname-replace">',
      '#suffix' => '</div>',
      '#weight' => 40,
    );
    if (isset($form_state['values']['toaccountname'])) {
      $form['toaccountname']['#default_value'] = $form_state['values']['toaccountname'];
    }
  }
  else {
    $toaccount = $bank->getAccount($to_account_id);
    if (empty($toaccount)) {
      drupal_set_message(t('Account id %id does not exist, is not active yet, or does not belong to exchange %ex', array('%id' => $to_account_id, '%ex' => $toexchange['code'])), 'error');
      return FALSE;
    };
    $form['toexchange'] = array(
      '#type' => 'value',
      '#value' => $toexchangeid,
    );
    $form['toaccountname'] = array(
      '#type' => 'value',
      '#value' => $toaccount['name'],
    );
  }
  $fromexchangeid = isset($form_state['values']['fromexchange']) ? $form_state['values']['fromexchange'] : $exchangeid;
  $fromexchange = $exchanges[$fromexchangeid];
  if ($from_account_id == NULL) {
    // Select buyer's exchange.
    $form['fromexchange'] = array(
      '#title' => t('Buyer exchange'),
      '#description' => t("The buyer's account exchange."),
      '#type' => 'select',
      '#options' => $exoptions,
      '#default_value' => $fromexchangeid,
      '#ajax' => array(
        'callback' => 'ces_bank_fromaccountname_dropdown_callback',
        'wrapper' => 'dropdown-fromaccountname-replace',
      ),
      '#weight' => 10,
    );
    // Select buyer's account.
    if ($fromexchangeid == $toexchangeid && !empty($tooptions)) {
      $fromoptions = &$tooptions;
    }
    else {
      $fromoptions = array();
      $accountconditions['exchange'] = $fromexchangeid;
      $accounts = $bank->getAllAccounts($accountconditions);
      foreach ($accounts as $account) {
        $useracc = reset($account['users']);
        $fromoptions[$account['name']] = $account['name'] . ' - ' . ces_user_get_name(user_load($useracc['user']));
      }
    }
    $form['fromaccountname'] = array(
      '#title' => t('Buyer account'),
      '#description' => t('The payer of the transaction. The Exchange four letters code plus four numbers.'),
      '#type' => 'select',
      '#options' => $fromoptions,
      '#prefix' => '<div id="dropdown-fromaccountname-replace">',
      '#suffix' => '</div>',
      '#weight' => 20,
    );
    if (isset($form_state['values']['fromaccountname'])) {
      $form['fromaccountname']['#default_value'] = $form_state['values']['fromaccountname'];
    }
  }
  else {
    $fromaccount = $bank->getAccount($from_account_id);
    if (empty($fromaccount)) {
      drupal_set_message(t('Account id %id does not exist, is not active yet, or does not belong to exchange %ex', array('%id' => $from_account_id, '%ex' => $fromexchange['code'])), 'error');
      return FALSE;
    }

    $form['fromexchange'] = array(
      '#type' => 'value',
      '#value' => $fromexchangeid,
    );
    $form['fromaccountname'] = array(
      '#type' => 'value',
      '#value' => $fromaccount['name'],
    );
  }
  $form['concept'] = array(
    '#type'  => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Enter the concept of the transference.'),
    '#default_value' => !empty($form_state['values']['concept']) ? $form_state['values']['concept'] : '',
    '#weight' => 50,
  );
  $form['amount'] = array(
    '#type' => 'textfield',
    '#size' => '5',
    '#title' => t('Amount in @currenciesname', array('@currenciesname' => $toexchange['currenciesname'])),
    '#description' => t('The amount to be transferred.'),
    '#default_value' => !empty($form_state['values']['amount']) ? $form_state['values']['amount'] : '',
    '#weight' => 60,
    '#field_suffix' => $toexchange['currencysymbol'],
  );
  $form['user'] = array(
    '#type' => 'value',
    '#value' => isset($form_state['values']['user']) ? $form_state['values']['user'] : $user->uid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $op == 'new' ? t('Create transaction') : t('Update transaction'),
    '#weight' => 100,
  );
  return $form;
}
/**
 * AJAX callback for transaction form.
 */
function ces_bank_toaccountname_dropdown_callback($form, $form_state) {
  return $form['toaccountname'];
}
/**
 * AJAX callback for transaction form.
 */
function ces_bank_fromaccountname_dropdown_callback($form, $form_state) {
  return $form['fromaccountname'];
}
/**
 * Submit transaction form.
 */
function ces_bank_transaction_form_submit($form, &$form_state) {
  $form_state['values']['page'] = 'confirm';
  $form_state['rebuild'] = TRUE;
}
/**
 * Submit confirm transaction form.
 *
 * @param string $op
 *   One of:
 *   - new: create and trigger transaction.
 *   - edit: edit a new transaction.
 *   - trigger: trigger an already created transaction.
 *   - discard: discard a rejected transaction (TODO).
 *   - revoke: revoke a committed transaction (TODO).
 * @param int $id
 *   the id of the transaction. If $op='new', then it is ignored
 *   in favor of $form_state values coming from ces_bank_transaction_form.
 */
function ces_bank_transaction_confirm_form($form, &$form_state, $op, $id = NULL) {
  if ($op == 'new' || $op == 'edit') {
    $values = $form_state['values'];
    // Copy values from previous form in a safe place.
    $form_state['edit_page'] = $values;
  }
  else {
    $bank = new Bank();
    try {
      $transaction = $bank->getTransaction($id);
      $fromaccount = $bank->getAccount($transaction['fromaccount']);
      $toaccount = $bank->getAccount($transaction['toaccount']);
      $values = array(
        'toaccountname' => $toaccount['name'],
        'fromaccountname' => $fromaccount['name'],
        'amount' => $transaction['amount'],
        'concept' => $transaction['concept'],
        'id' => $transaction['id'],
        'user' => $transaction['user'],
      );
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
      return array();
    }
  }
  if ($op != 'new' && $op != 'trigger' && $op != 'edit') {
    drupal_set_message(t('Operation not permitted or not implemented yet.'), 'error');
    return array();
  }
  if (isset($values['id'])) {
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $values['id'],
    );
  }
  $form['operation'] = array(
    '#type' => 'value',
    '#value' => $op,
  );
  // This value is used in ces_bank_transaction_form().
  $form['page'] = array(
    '#type' => 'value',
    '#value' => 'confirm',
  );
  $form['itemtoaccountname'] = array(
    '#type' => 'item',
    '#title' => t('Seller account'),
    '#markup' => $values['toaccountname'],
  );
  $form['itemfromaccountname'] = array(
    '#type' => 'item',
    '#title' => t('Buyer account'),
    '#markup' => $values['fromaccountname'],
  );
  $form['itemamount'] = array(
    '#type' => 'item',
    '#title' => t('Amount'),
    '#markup' => $values['amount'],
  );
  $form['itemconcept'] = array(
    '#type' => 'item',
    '#title' => t('Description'),
    '#markup' => $values['concept'],
  );
  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
    '#submit' => array('ces_bank_transaction_confirm_form_back'),
    '#limit_validation_errors' => array(),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('ces_bank_transaction_confirm_form_submit'),
  );
  switch ($op) {
    case 'new':
      $form['submit']['#value'] = t('Confirm transaction');
      break;

    case 'trigger':
      $form['submit']['#value'] = t('Trigger transaction');
      break;

    case 'edit':
      $form['submit']['#value'] = t('Update transaction');
      break;

  }
  return $form;
}
/**
 * TODO: Document.
 */
function ces_bank_transaction_confirm_form_back($form, &$form_state) {
  if (isset($form_state['edit_page'])) {
    $form_state['values'] = $form_state['edit_page'];
    $form_state['values']['page'] = 'edit';
    $form_state['rebuild'] = TRUE;
  }
  else {
    drupal_goto('ces/bank');
  }
}
/**
 * Validate transaction form.
 */
function ces_bank_transaction_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['page']) && $form_state['values']['page'] == 'confirm') {
    return;
  }

  $bank = new Bank();
  $record = $form_state['values'];

  $amount = $record['amount'];
  if (!is_numeric($amount)) {
    form_set_error('amount', t('The amount entered is not a numeric value. Use the dot "." character for the decimal point and do not use any thousands separator.'));
    if (!($amount > 0.0)) {
      form_set_error('amount', t('The amount must be positive.'));
    }
  }
  $account1 = $record['toaccountname'];
  if ($bank->getAccountByName($account1) === FALSE) {
    form_set_error('toaccountname', t('Seller account %acc is incorrect.', array('%acc' => $account1)));
  }
  $account2 = $record['fromaccountname'];
  if ($bank->getAccountByName($account2) === FALSE) {
    form_set_error('fromaccountname', t('Buyer account %acc is incorrect.', array('%acc' => $account2)));
  }
  if ($account1 == $account2) {
    form_set_error('fromaccountname', t('Buyer account cannot be the same as seller account.'));
  }
}
/**
 * Submit confirmation form for transaction.
 */
function ces_bank_transaction_confirm_form_submit($form, &$form_state) {
  $bank = new Bank();
  $result = TRUE;
  $op = $form_state['values']['operation'];
  $id = '';
  try {
    switch ($op) {
      case 'new':
        $record = $form_state['edit_page'];
        $bank->createTransaction($record);
        $id = $record['id'];
        $result = $bank->applyTransaction($id);
        break;

      case 'trigger':
        $id = $form_state['values']['id'];
        $result = $bank->applyTransaction($id);
        break;

      case 'edit':
        $record = $form_state['edit_page'];
        $result = $bank->updateTransaction($record);
        $id = $record['id'];
        break;

      default:
        throw new Exception(t('Operation not permitted or not implemented yet.'));
    }
    if ($result !== TRUE) {
      $text = t('The transaction has NOT been applied.');
      if (is_array($result)) {
        $text .= '<br/>' . t('Details:') . '<br/>';
        $text .= '<ul>';
        foreach ($result as $message) {
          $text .= '<li>' . filter_xss($message) . '</li>';
        }
        $text .= '</ul>';
      }
      throw new Exception($text);
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    return;
  }
  switch ($op) {
    case 'new':
    case 'trigger':
      $msg = t('Transaction successfully applied.');
      break;

    case 'edit':
      $msg = t('Transaction successfully updated.');
      break;

  }
  $form_state['redirect'] = 'ces/bank/account/transaction/' . $id . '/view';
  drupal_set_message(filter_xss($msg));
}
/**
 * Form to delete a transaction permanently.
 */
function ces_bank_transaction_delete_form($form, &$form_state, $transaction) {
  $form = array();
  $form['transaction'] = array('#type' => 'value', '#value' => $transaction);

  $message = '<p>' . t('Are you sure you want to delete the transaction?') . '</p>';
  $caption = '<p>' . t('Transaction details:') . '</p>';
  require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.pages.inc';
  $view = ces_bank_transaction_view($transaction);
  $caption .= '<p>' . drupal_render($view) . '</p>';
  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, filter_xss($message), 'ces/bank/account/transaction', filter_xss($caption), t('Delete'));
}
/**
 * Submit to delete a transaction permanently.
 */
function ces_bank_transaction_delete_form_submit($form, &$form_state) {
  $bank = new Bank();
  $transaction = $form_state['values']['transaction'];
  $bank->deleteTransaction($transaction['id']);
  drupal_set_message(t('Transaction %concept has been deleted.', array('%concept' => $transaction['concept'])));
  $form_state['redirect'] = 'ces/bank/account/transaction';
}

/**
 * Bank account statement form.
 *
 * @return array
 *   page where to find the last transactions with this account.
 * @see ces_bank_account_statement_form_submit()
 * @see ces_bank_account_statement_page()
 */
function ces_bank_account_statement_form($form, &$form_state) {
  $form = array(
    '#method' => 'get',
    '#token' => FALSE,
  );
  // Filter controls.
  $form['controls'] = array(
    '#type' => 'container',
    '#attributes' => array(),
  );
  if (isset($form_state['values']['from']) && isset($form_state['values']['to'])) {
    $from = $form_state['values']['from'];
    $to = $form_state['values']['to'];
  }
  else {
    $lastmonth = mktime(0, 0, 0, date('m') - 1, date('d'), date('Y'));
    $from = array(
      'year' => date('Y', $lastmonth),
      'month' => date('n', $lastmonth),
      'day' => date('d', $lastmonth),
    );
    $to = array(
      'year' => date('Y'),
      'month' => date('n'),
      'day' => date('d'),
    );
  }
  $form['controls']['from'] = array(
    '#type' => 'date',
    '#default_value' => $from,
    '#title' => t('From'),
  );
  $form['controls']['to'] = array(
    '#type' => 'date',
    '#default_value' => $to,
    '#title' => t('To'),
  );
  $form['controls']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  return $form;
}
/**
 * Bank account statement form submit.
 */
function ces_bank_account_statement_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
/**
 * Bank permission form.
 */
function ces_bank_permission_form($form, &$form_state, $op, $id = NULL) {
  $form = array();
  if ($op != 'new') {
    $bank = new Bank();
    $values = $bank->getPermission($id);
  }
  else {
    $values = array(
      'permission' => '',
      'object' => '',
      'objectid' => 0,
      'scope' => '',
      'scopeid' => 0,
    );
  }
  $form['operation'] = array(
    '#type' => 'value',
    '#value' => $op,
  );
  if ($op != 'new') {
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $id,
    );
  }
  $form['permission'] = array(
    '#title' => t('Permission type'),
    '#type' => 'select',
    '#options' => array(
      10 => t('View'),
      20 => t('Use'),
      30 => t('Edit'),
      40 => t('Admin'),
    ),
    '#default_value' => $values['permission'],
    '#required' => TRUE,
  );
  $form['object'] = array(
    '#type' => 'select',
    '#title' => t('Object type'),
    '#description' => t('The type of the piece of data on which the permission is set.'),
    // @TODO: move this array to the bank core file.
    '#options' => array(
      'global' => t('Global'),
      'global statistics' => t('Global statistics'),
      'global exchangecreator' => t('Global exchange creation'),
      'exchange' => t('Exchange'),
      'exchange details' => t('Exchange configuration details'),
      'exchange accountcreator' => t('Exchange account creation'),
      'account' => t('Account'),
      'account details' => t('Account details'),
      'account transactions' => t('Account transactions'),
      'account seller' => t('Account seller'),
      'account buyer' => t('Account buyer'),
    ),
    '#default_value' => $values['object'],
    '#required' => TRUE,
  );
  // @TODO: use ajax to provide an usable interface instead of the following
  // element.
  $form['objectid'] = array(
    '#type' => 'textfield',
    '#title' => t('Object identifier'),
    '#default_value' => $values['objectid'],
    '#required' => TRUE,
  );
  $form['scope'] = array(
    '#type' => 'select',
    '#title' => t('Scope type'),
    '#description' => t('The group of users who have this permission.'),
    '#options' => array(
      'user' => t('User'),
      'account' => t('Account'),
      'exchange' => t('Exchange'),
      'global' => t('Global'),
    ),
    '#default_value' => $values['scope'],
    '#required' => TRUE,
  );
  // @TODO: use ajax to provide an usable interface instead of the following
  // element.
  $form['scopeid'] = array(
    '#type' => 'textfield',
    '#title' => t('Scope identifier'),
    '#default_value' => $values['scopeid'],
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $op == 'new' ? t('Create permission') : t('Update permission'),
  );
  return $form;
}
/**
 * Check if object exists to host a permission.
 */
function _ces_bank_permission_check_object_exists($object, $objectid) {
  $space = strpos($object, ' ');
  if ($space > 0) {
    $object = substr($object, 0, $space);
  }
  switch ($object) {
    case 'global':
      return TRUE;

    case 'user':
      return user_load($objectid) !== FALSE;

    case 'exchange':
      $bank = new Bank();
      return $bank->getExchange($objectid) !== FALSE;

    case 'account':
      $bank = new Bank();
      return $bank->getAccount($objectid) !== FALSE;

  }
  return FALSE;
}
/**
 * Validation for bank permission form.
 */
function ces_bank_permission_form_validate($form, &$form_state) {
  $bank = new Bank();
  $values = $form_state['values'];
  if (!$bank->access('admin', $values['object'], $values['objectid'])) {
    form_set_error('object', t('You are not authorized to modify permissions on this object.'));
  }
  if (!_ces_bank_permission_check_object_exists($values['object'], $values['objectid'])) {
    form_set_error('object', t('Object with given id does not exist.'));
  }
  if (!_ces_bank_permission_check_object_exists($values['scope'], $values['scopeid'])) {
    form_set_error('scope', t('Scope with given id does not exist.'));
  }
}
/**
 * Submit bank permission form.
 */
function ces_bank_permission_form_submit($form, &$form_state) {
  $bank = new Bank();
  $op = $form_state['values']['operation'];
  try {
    if ($op == 'new') {
      $bank->createPermission($form_state['values']);
      drupal_set_message(t('Permission successfully created.'));
    }
    elseif ($op == 'edit') {
      $bank->updatePermission($form_state['values']);
      drupal_set_message(t('Permission successfully updated.'));
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('An error ocurred while saving the record. Details: %msg', array('msg' => $e->getMessage())), 'error');
  }
}
/** @} */
