<?php
/**
 * @file
 * All page callback functions for bank module.
 */
function ces_bank_exchange_admin($exchangeid) {
  $bank = new Bank();
  $exchange = $bank->getExchange($exchangeid);
  $page = array();
  //New accounts request
  $page['new_accounts'] = array(
    'type' => 'fieldset',
    'title' => t('New accounts'),
    'description' => t('List of new account requests for this exchange. Activate or delete all them.')
  );
  $accounts = $bank->getAllAccounts(array('state' => 0, 'exchange' => $exchange['id']));
  if (!empty($accounts)) {
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
    $page['new_accounts']['text'] = array(
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
 * Page with the list of all exchanges organized by country.
 */
function ces_bank_exchange_list_page() {
  $bank = new Bank();
  $exchanges = $bank->getAllExchanges();
  $page = array();
  if (empty($exchanges)) {
    $page['text'] = array(
      '#type' => 'markup',
      '#value' => t('There are no active exchanges'),
    );
  }
  else {
    include_once DRUPAL_ROOT . '/includes/locale.inc';
    $countries = country_get_list();
    foreach ($exchanges as $exchange) {
      $country = $countries[$exchange['country']];
      if (!isset($page[$country])) {
        $page[$country] = array(
          '#type' => 'fieldset',
          '#title' => $country,
          '#attached' => array(
            'library' => array(
              array('system', 'drupal.form'),
              array('system', 'drupal.collapse')
            )
          ),
          '#attributes' => array(
            'class' => array('collapsible', 'collapsed'),
          ),
        );
        $page[$country]['table'] = array(
          '#theme' => 'table',
          '#header' => array(
            array(
              'data' => t('Code'),
              'field' => 'code',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Name'),
              'field' => 'name',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Region'),
              'field' => 'region',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Town'),
              'field' => 'town',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Accounts')  ,
              'field' => 'accounts',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Currency'),
              'field' => 'currencysymbol',
              'sort' => 'asc'
            ),
            array(
              'data' => t('Actions')
            ),
          ),
          '#rows' => array(),
        );
      }
      $row = array(
        $exchange['code'],
        $exchange['name'],
        $exchange['region'],
        $exchange['town'],
        $exchange['accounts'],
        $exchange['currencysymbol'],
        l(t('Register'), 'ces/bank/exchange/' . $exchange['id'] . '/account/new', array('title' => t('Click to open an account in this exchange'))),
      );
      $page[$country]['table']['#rows'][] = $row;
    }

    foreach ($page as $country => $fieldset) {
      $page[$country]['#title'] .= ' (' . count($fieldset['table']['#rows']) . ')';
    }

    ksort($page);
  }

  return $page;
}
function ces_bank_exchange_accounts_page($exchangeid) {
  $bank = new Bank();
  $accounts = $bank->getAllAccounts(array('exchange' => $exchangeid));
  $page = array();
  $table = array(
    '#theme' => 'table',
    '#header' => array(
      '#', 'Account', 'User', 'Balance', 'Actions'
    ),
    '#rows' => array(),
  );
  $counter = 1;
  foreach ($accounts as $account) {
    if ($account['state'] == 0) continue;//LocalAccount::STATE_HIDDEN)
    $acchref = 'ces/bank/exchange/' . $exchangeid . '/account/' . $account['id'];
    $actions = l(t('details'), $acchref . '/view');
    $row = array(
      $counter++,
      $account['name'],
      '-',
      $account['balance'],
      $actions,
    );
    $table['#rows'][] = $row;
  }
  $page['table'] = $table;
  return $page;
}

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
      $list['#items'][] = l($exchange['code'] . ' - ' . $exchange['name'], 'ces/bank/exchange/' . $exchange['id'] . '/activate');
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
 *
 */
function ces_bank_permissions_page() {
  $bank = new Bank();
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
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
        'sort' => 'asc'
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
      l(t('Edit'), 'ces/bank/permission/' . $perm['id'] . '/edit') . ' ' . l(t('Delete'), 'ces/bank/permission/' . $perm['id'] . '/delete'),
    );
    $page['table']['#rows'][] = $row;
  }
  return $page;
}
/**
 * List the limitchains for the given exchange.
 */
function ces_bank_limitchain_list_page($exchange) {
  $bank = new Bank();
  $limitchains = $bank->getAllLimitChains($exchange);
  $page = array();
  $page['table'] = array(
    '#theme' => 'table',
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
        'data' => t('Actions'),
      ),
    ),
    '#rows' => array(),
  );
  foreach ($limitchains as $limitchain) {
    $row = array();
    $row[] = $limitchain['name'];
    $limits = array();
    foreach ($limitchain['limits'] as $limit) {
      $limits[] = $limit['classname']::getName() . ' ' . $limit['value'] . ($limit['block'] ? ' ' . t('block') : '');
    }
    $row[] = implode('<br/>', $limits);
    $actions = array();
    if (ces_bank_access(Permission::PERMISSION_VIEW, 'exchange limitchains', $exchange)) {
      $actions[] = l(t('View'), 'ces/bank/exchange/' . $exchange . '/limitchain/' . $limitchain['id'] . '/view');
    }
    if (ces_bank_access(Permission::PERMISSION_EDIT, 'exchange', $exchange)) {
      $actions[] = l(t('Edit'), 'ces/bank/exchange/' . $exchange . '/limitchain/' . $limitchain['id'] . '/edit');
    }
    if (ces_bank_access(Permission::PERMISSION_ADMIN, 'exchange', $exchange)) {
      $actions[] = l(t('Delete'), 'ces/bank/exchange/' . $exchange . '/limitchain/' . $limitchain['id'] . '/delete');
    }
    $row[] = implode(' ', $actions);
    $page['table']['#rows'][] = $row;
  }
  return $page;
}
function ces_bank_exchange_view($id) {
  $bank = new Bank();
  $exchange = $bank->getExchange($id);
  $page = array(
    'exchange' => $exchange,
    '#theme' => 'ces_bank_exchange_view',
  );
  return $page;
}
function ces_bank_account_view($id) {
  $bank = new Bank();
  $account = $bank->getAccount($id);
  $page = array(
    'account' => $account,
    '#theme' => 'ces_bank_account_view',
  );
  //Add and override some items
  $exchange = $bank->getExchange($account['exchange']);
  $page['account']['exchange'] = $exchange['name'];
  $users = reset($account['users']);
  $accuser = user_load($users['user']);
  $page['account']['username'] = $accuser->name;
  //TODO: these arrays are repeated in ces_bank_account_form(). They should be
  //shared.
  $kinds = array(
    0 => t('Individual'),
    1 => t('Shared'),
    2 => t('Organization'),
    3 => t('Company'),
    4 => t('Public')
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