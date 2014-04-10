<?php
/**
 * @file
 * All page callback functions for bank module.
 */

/**
 * @todo: redo all this page!
 */

 /**
  * TODO: document.
  */
function ces_bank_exchange_admin() {
  $bank = new Bank();
  $exchange = ces_bank_get_current_exchange();
  $page = array();
  // New accounts request.
  $accounts = $bank->getAllAccounts(array('state' => 0, 'exchange' => $exchange['id']));
  if (!empty($accounts)) {
    $page['new_accounts'] = array(
      'type' => 'fieldset',
      'title' => t('New accounts'),
      'description' => t('List of new account requests for this exchange. Activate or delete all them.'),
    );
    $list = array(
      '#theme' => 'item_list',
      '#items' => array(),
    );
    foreach ($accounts as $account) {
      $list['#items'][] = l($account['name'], 'ces/bank/exchange/' . $exchange['id'] . '/account/' . $account['id'] . '/activate');
    }
    $page['new_accounts']['new_accounts_list'] = $list;
  }
  else {
    $page['no_new_accounts'] = array(
      '#type' => 'item',
      '#title' => t('No requests for new accounts.'),
    );
  }
  $page['limitchains'] = array(
    '#markup' => l(t('Administer limit chains'), 'ces/bank/exchange/' . $exchange['id'] . '/limitchain'),
  );
  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_accounts_admin() {
  $bank = new Bank();
  $exchange = ces_bank_get_current_exchange();
  if ($exchange === FALSE) {
    return array(
      'message' => array(
        '#markup' => '<p>' . t('Invalid current exchange') . '</p>',
      ),
    );
  }
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('ces-table'),
    ),
    '#header' => array(
      array(
        'data' => t('Account'),
        'sort' => 'asc',
        'field' => 'name',
      ),
      array(
        'data' => t('Type'),
        'sort' => 'asc',
        'field' => 'kind',
      ),
      array(
        'data' => t('State'),
        'sort' => 'asc',
        'field' => 'state',
      ),
      array(
        'data' => t('Limit'),
        'sort' => 'asc',
        'field' => 'limitchain',
      ),
      array(
        'data' => t('Balance'),
        'sort' => 'asc',
        'field' => 'balance',
      ),
      array(
        'data' => t('Actions'),
      ),
    ),
    '#rows' => array(),
  );
  $accounts = $bank->getAllAccounts(array('exchange' => $exchange['id']));
  $kinds = array(
    0 => t('Individual'),
    1 => t('Shared'),
    2 => t('Organization'),
    3 => t('Company'),
    4 => t('Public'),
    5 => t('Virtual'),
  );
  $states = array(
    0 => t('Hidden'),
    1 => t('Active'),
    2 => t('Closed'),
    3 => t('Locked'),
  );
  $limitchains = $bank->getAllLimitChains($exchange['id']);
  $limits = array();
  foreach ($limitchains as $id => $limit) {
    $limits[$id] = $limit['name'];
  }
  $rows = array();
  foreach ($accounts as $account) {
    $row = array(
      $account['name'],
      $kinds[$account['kind']],
      $states[$account['state']],
      $limits[$account['limitchain']],
      $bank->formatAmount($account['balance'], $exchange, TRUE),
      l(t('View'), 'ces/admin/account/' . $account['id'] . '/view') . ' ' .
      l(t('Edit'), 'ces/admin/account/' . $account['id'] . '/edit') . ' ' .
      l(t('Delete'), 'ces/admin/account/' . $account['id'] . '/delete'),
    );
    $rows[] = $row;
  }
  _ces_bank_sort_table($page['table']['#header'], $rows);
  $page['table']['#rows'] = $rows;
  return $page;
}
/**
 * Page with the list of all exchanges organized by country.
 */
function ces_bank_exchange_list_page($country) {
  $bank = new Bank();
  $conditions = array(
    'state' => 1,
    'country' => $country,
  );
  $exchanges = $bank->getAllExchanges($conditions);
  $page = array();
  if (empty($exchanges)) {
    require_once DRUPAL_ROOT . '/includes/locale.inc';
    $countries = country_get_list();
    $page['text'] = array(
      '#type' => 'markup',
      '#value' => t('There are no active exchanges in %country.', array('%country' => $countries[$country])),
    );
  }
  else {
    $page['table'] = array(
      '#theme' => 'table',
      '#attributes' => array(
        'class' => array('ces-table'),
      ),
      '#header' => array(
        array(
          'data' => t('Code'),
          'field' => 'code',
          'sort' => 'asc',
        ),
        array(
          'data' => t('Name'),
          'field' => 'name',
          'sort' => 'asc',
        ),
        array(
          'data' => t('Region'),
          'field' => 'region',
          'sort' => 'asc',
        ),
        array(
          'data' => t('Town'),
          'field' => 'town',
          'sort' => 'asc',
        ),
        array(
          'data' => t('Accounts')  ,
          'field' => 'accounts',
          'sort' => 'asc',
        ),
        array(
          'data' => t('Currency'),
          'field' => 'currencysymbol',
          'sort' => 'asc',
        ),
      ),
      '#rows' => array(),
    );
    foreach ($exchanges as $exchange) {
      $row = array(
        $exchange['code'],
        l($exchange['name'], 'user/register/' . $exchange['code'], array('title' => t('Click to open an account in this exchange'))),
        $exchange['region'],
        $exchange['town'],
        $exchange['accounts'],
        $exchange['currencysymbol'],
      );
      $page['table']['#rows'][] = $row;
    }
  }
  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_exchange_accounts_page() {
  $bank = new Bank();
  $exchange = ces_bank_get_current_exchange();
  $accounts = $bank->getAllAccounts(array('exchange' => $exchange['id']));
  // Accounts per user.
  $useraccounts = array();
  foreach ($accounts as $account) {
    foreach ($account['users'] as $user) {
      $uid = $user['user'];
      if (!isset($useraccounts[$uid])) {
        $useraccounts[$uid] = array();
      }
      $useraccounts[$uid][] = $account;
    }
  }
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('ces-table'),
    ),
    '#header' => array(
      array(
        'data' => t('User'),
        'field' => 'user',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Account'),
        'field' => 'account',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Balance'),
        'field' => 'balance',
        'sort' => 'asc',
      ),
    ),
  );
  
  $rows = array();
  foreach ($useraccounts as $uid => $accounts) {
    $user = user_load($uid);

    $accountsperuser = "";
    $balancesperuser = "";
    foreach ($accounts as $account) {
      $accountsperuser .= $account['name'] . '<br/>';
      $balancesperuser .= $bank->formatAmount($account['balance'], $exchange, TRUE) . '<br/>';
    }
    // first cell begins with 8 chars of the name user in capital letters
    // only for sort case insensitive proposals.
    $row = array(
      strtoupper(sprintf("%-8s",ces_user_get_name($user))) .
      '<div class="ces-user-column">' .
      '<div class="user-picture">' . theme('user_picture', array('account' => $user)) . '</div>' .
      l(ces_user_get_name($user), 'user/' . $uid) .
      '</div>',
      $accountsperuser,
      $balancesperuser,
    );
    $rows[] = $row;
  }
  _ces_bank_sort_table($page['table']['#header'], $rows);
  $lenghtrows = count($rows);
  for ($i=0;$i<$lenghtrows;$i++) {
  	 $rows[$i][0] = substr($rows[$i][0], 8);
  }
  $page['table']['#rows'] = $rows;
  return $page;
}

/**
 * TODO: document.
 */
function ces_bank_account_list_page() {
  global $user;
  $bank = new Bank();
  $page = array();
  $accounts = $bank->getUserAccounts($user->uid);
  foreach ($accounts as $account) {
    $exchange = $bank->getExchange($account['exchange']);
    $accform = array(
      '#type' => 'fieldset',
      '#title' => t('Account') . ' ' . $account['name'],
    );
    $accform['exchange'] = array(
      '#type' => 'item',
      '#title' => t('Exchange'),
      '#markup' => $exchange['name'],
    );
    $accform['account'] = array(
      '#type' => 'item',
      '#title' => t('Account'),
      '#markup' => $account['name'],
    );
    $accform['balance'] = array(
      '#type' => 'item',
      '#title' => t('Balance'),
      '#markup' => $account['balance'],
    );
    $accform['statement'] = array(
      '#type' => 'item',
      '#markup' => l(t('Statement'), 'ces/bank/exchange/' . $exchange['id'] . '/account/' . $account['id'] . '/transaction'),
    );
    $accform['edit'] = array(
      '#type' => 'item',
      '#markup' => l(t('Edit'), 'ces/bank/exchange/' . $exchange['id'] . '/account/' . $account['id'] . '/edit'),
    );
    $accform['transaction'] = array(
      '#type' => 'item',
      '#markup' => l(t('Charge a sale'), 'ces/bank/exchange/' . $exchange['id'] . '/account/' . $account['id'] . '/transaction/sell'),
    );
    $page[$account['name']] = $accform;
  }
  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_admin_page() {
  $bank = new Bank();
  $page = array();
  $page['new_exchanges'] = array(
    '#type' => 'fieldset',
    '#title' => t('Exchange requests'),
  );
  $exchanges = $bank->getAllExchanges(array('state' => 0));
  if (!empty($exchanges)) {
    $list = array(
      '#theme' => 'item_list',
      '#items' => array(),
    );
    foreach ($exchanges as $exchange) {
      $list['#items'][] = l($exchange['code'] . ' - ' . $exchange['name'], 'ces/admin/exchange/' . $exchange['id'] . '/activate');
    }
    $page['new_exchanges']['new_exchanges_list'] = $list;
  }
  else {
    $page['new_exchanges']['text'] = array(
      '#type' => 'item',
      '#title' => t('No requests for new exchanges.'),
    );
  }
  return $page;
}
/**
 * TODO: Document.
 */
function ces_bank_permissions_page() {
  $bank = new Bank();
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('ces-table'),
    ),
    '#header' => array(
      array(
        'data' => t('Permission type'),
        'field' => 'permission',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Object type'),
        'field' => 'object',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Object identifier'),
        'field' => 'objectid',
      ),
      array(
        'data' => t('Scope type'),
        'field' => 'scope',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Scope identifier'),
        'field' => 'scopeid',
      ),
      array(
        'data' => t('Actions'),
      ),
    ),
    '#rows' => array(),
  );
  global $user;
  $perms = $bank->getAdministrablePermissions($user->uid);
  foreach ($perms as $perm) {
    $row = array(
      $perm['permission'],
      $perm['object'],
      $perm['objectid'],
      $perm['scope'],
      $perm['scopeid'],
    );
    $actions = array();
    if ($bank->access('admin', 'permission', $perm['id'])) {
      $actions[] = l(t('Edit'), 'ces/bank/permission/' . $perm['id'] . '/edit');
      $actions[] = l(t('Delete'), 'ces/bank/permission/' . $perm['id'] . '/delete');
    }
    $row[] = implode(' ', $actions);
    $page['table']['#rows'][] = $row;
  }
  return $page;
}
/**
 * List the limitchains for the given exchange.
 */
function ces_bank_limitchain_list_page() {
  $exchange = ces_bank_get_current_exchange();
  if ($exchange === FALSE) {
    return array(
      'message' => array(
        '#markup' => '<p>' . t('Invalid current exchange') . '</p>',
      ),
    );
  }
  $bank = new Bank();
  $limitchains = $bank->getAllLimitChains($exchange['id']);
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('ces-table'),
    ),
    '#header' => array(
      array(
        'data' => t('Name'),
        'field' => 'name',
        'sort' => 'asc',
      ),
      array(
        'data' => t('Limits'),
      ),
      array(
        'data' => t('Accounts'),
      ),
      array(
        'data' => t('Actions'),
      ),
    ),
  );
  $limitnames = module_invoke_all('account_limit_classes');
  $rows = array();
  foreach ($limitchains as $limitchain) {
    $row = array();
    $row[] = $limitchain['name'];
    $limits = array();
    foreach ($limitchain['limits'] as $limit) {
      $limits[] = $limitnames[$limit['classname']] . ' ' . $bank->formatAmount($limit['value'], $exchange, TRUE) . ($limit['block'] ? ' ' . t('block') : '');
    }
    $row[] = implode('<br/>', $limits);
    $actions = array();
    if (ces_bank_access(Permission::PERMISSION_VIEW, 'exchange limitchains', CES_CURRENT_EXCHANGE)) {
      $actions[] = l(t('View'), 'ces/admin/limit/' . $limitchain['id'] . '/view');
    }
    if (ces_bank_access(Permission::PERMISSION_EDIT, 'exchange', CES_CURRENT_EXCHANGE)) {
      $actions[] = l(t('Edit'), 'ces/admin/limit/' . $limitchain['id'] . '/edit');
    }
    if (ces_bank_access(Permission::PERMISSION_ADMIN, 'exchange', CES_CURRENT_EXCHANGE)) {
      $actions[] = l(t('Delete'), 'ces/admin/limit/' . $limitchain['id'] . '/delete');
    }
    $accounts = $bank->getAllAccounts(array('exchange' => $exchange['id'], 'limitchain' => $limitchain['id']));
    $row[] = count($accounts);
    $row[] = implode(' ', $actions);
    $rows[] = $row;
  }
  _ces_bank_sort_table($page['table']['#header'], $rows);
  $page['table']['#rows'] = $rows;
  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_limitchain_view($limitchain) {
  $bank = new Bank();
  foreach ($limitchain['limits'] as $key => $value) {
    $limitchain['limits'][$key]['value'] = $bank->formatAmount($value['value'], ces_bank_get_current_exchange(), TRUE);
  }
  $page = array(
    '#theme' => 'ces_bank_limitchain_view',
    'limitchain' => $limitchain,
    'limitclasses' => module_invoke_all('account_limit_classes'),
  );
  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_exchange_view() {
  $exchange = ces_bank_get_current_exchange();
  if ($exchange !== FALSE) {
    $page = array(
      'exchange' => $exchange,
      '#theme' => 'ces_bank_exchange_view',
    );
  }
  else {
    $page = array(
      'message' => array(
        '#markup' => '<p>' . t("You don't belong to any exchange yet.") .
        '</p>',
      ),
    );
  }

  return $page;
}
/**
 * TODO: document.
 */
function ces_bank_account_view() {
  $account = ces_bank_get_current_account();
  $page = array(
    'account' => $account,
    '#theme' => 'ces_bank_account_view',
  );
  $bank = new Bank();
  // Add and override some items.
  $exchange = $bank->getExchange($account['exchange']);
  $page['account']['exchange'] = $exchange['name'];
  $users = reset($account['users']);
  $accuser = user_load($users['user']);
  $page['account']['username'] = $accuser->name;
  // TODO: these arrays are repeated in ces_bank_account_form(). They should be
  // shared.
  $kinds = array(
    0 => t('Individual'),
    1 => t('Shared'),
    2 => t('Organization'),
    3 => t('Company'),
    4 => t('Public'),
  );
  $states = array(
    0 => t('Hidden'),
    1 => t('Active'),
    2 => t('Closed'),
    3 => t('Locked'),
  );
  $page['account']['kind'] = $kinds[$account['kind']];
  $page['account']['state'] = $states[$account['state']];
  $limitchain = $bank->getLimitChain($account['limitchain']);
  $page['account']['limitchain'] = $limitchain['name'];
  return $page;
}
/**
 * TODO: Document.
 *
 * @param int $transaction
 *   transaction id.
 */
function ces_bank_transaction_view($transaction) {
  $bank = new Bank();

  $fromaccount = $bank->getTransactionFromAccount($transaction);
  $toaccount = $bank->getTransactionToAccount($transaction);
  $page = array(
    '#theme' => 'ces_bank_transaction_view',
  );
  $page['transaction'] = $transaction;
  $page['transaction']['amount'] = $bank->formatAmount($transaction['amount'], ces_bank_get_current_exchange(), TRUE);
  $page['fromaccount'] = $fromaccount;
  $page['toaccount'] = $toaccount;
  $page['toexchange'] = $bank->getExchange($toaccount['exchange']);
  if ($fromaccount['exchange'] != $toaccount['exchange']) {
    $page['fromexchange'] = $bank->getExchange($fromaccount['exchange']);
  }
  else {
    $page['fromexchange'] = &$page['toexchange'];
  }
  return $page;
}

/**
 * Sort the rows of a table.
 * 
 * The second parameter $rows gets sorted depending on the $header definition
 * and the parameters passed by URL.
 * 
 * @param array $headers
 *   Table heeaders as in theme_table.
 * @param array $rows
 *   Table rows as in theme_table, by reference. 
 */
function _ces_bank_sort_table(array $headers, array &$rows) {
  global $_ces_bank_sort_table_column;
  $order = tablesort_get_order($headers);
  $sort = tablesort_get_sort($headers);
  // Find the column number to sort.
  $i = 0;
  foreach ($headers as $header) {
    if ($header['field'] == $order['sql']) {
      $_ces_bank_sort_table_column = $i;
      break;
    }
    $i++;
  }
  if ($sort == 'asc') {
    usort($rows, '_ces_bank_sort_table_cmp');
  }
  else {
    usort($rows, '_ces_bank_sort_table_cmpr');
  }
}

/**
 * Table ows comparator callback.
 */
function _ces_bank_sort_table_cmp($a, $b) {
  global $_ces_bank_sort_table_column;
  $ca = $a[$_ces_bank_sort_table_column];
  $cb = $b[$_ces_bank_sort_table_column];
  return strcmp($ca, $cb);
}

/**
 * Reverse table rows comparator callback.
 */
function _ces_bank_sort_table_cmpr($a, $b) {
  global $_ces_bank_sort_table_column;
  $ca = $a[$_ces_bank_sort_table_column];
  $cb = $b[$_ces_bank_sort_table_column];
  return strcmp($cb, $ca);
}
