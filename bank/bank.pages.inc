<?php
/**
 * Page with the list of all exchanges organized by country.
 */
function bank_exchange_list_page(){
  $bank = new Bank();
  $exchanges = $bank->getAllExchanges();
  
  $page = array();
  
  if(empty($exchanges)){
    $page['text'] = array(
      '#type' => 'markup',
      '#value' => t('There are no active exchanges'),
    );
  }else{
    
    include_once DRUPAL_ROOT . '/includes/locale.inc';
    $countries = country_get_list();
    
    foreach($exchanges as $exchange){
      $country = $countries[$exchange['country']];
      if(!isset($page[$country])){
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
              'data'=>t('Code'),
              'field'=>'code',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Name'),
              'field'=>'name',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Region'),
              'field'=>'region',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Town'),
              'field'=>'town',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Accounts')  ,
              'field'=>'accounts',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Currency'),
              'field'=>'currencysymbol',
              'sort'=>'asc'
            ),
            array(
              'data'=>t('Actions')
            ),
          ),
          '#rows' => array(),
        );
      }

      $row = array(
        $exchange['code'], 
        l($exchange['name'], 'ces/bank/account/new/'.$exchange['id'], array('title'=>t('Click to register to this exchange'))), 
        $exchange['region'], 
        $exchange['town'], 
        $exchange['accounts'], 
        $exchange['currencysymbol'],
        ''
      );
      
      $page[$country]['table']['#rows'][] = $row;
    }
    
    foreach($page as $country=>$fieldset){
      $page[$country]['#title'] .= ' ('.count($fieldset['table']['#rows']).')';
    }
    
    ksort($page);
  }
  
  return $page;
}

function bank_account_list_page(){
  global $user;
  
  $bank = new Bank();
  $page = array();
  
  $accounts = $bank->getUserAccounts($user->uid);
  foreach($accounts as $account){
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
      '#markup' => l(t('Statement'), 'ces/bank/account/statement/'.$account['id']),
    );
    
    $accform['edit'] = array(
      '#type' => 'item',
      '#markup' => l(t('Edit'), 'ces/bank/account/edit/'.$account['id']),
    );
    
    $accform['transaction'] = array(
      '#type' => 'item',
      '#markup' => l(t('Transaction'), 'ces/bank/transaction/'.$account['id']),
    );
    
    $page[$account['name']] = $accform;
    
    
  }

  return $page;
}

function bank_admin_page(){
  $bank = new Bank();
  
  $page = array();
  $page['new_exchanges'] = array(
      '#type' => 'fieldset',
      '#title' => t('Exchange requests'),
  );
  
  $exchanges = $bank->getAllExchanges(array('state'=>0));
  if(!empty($exchanges)){
    $list = array(
      '#theme' => 'item_list',
      '#items' => array(),
      
    );
    foreach($exchanges as $exchange){
      $list['#items'][] = l($exchange['code'].' - '.$exchange['name'], 'ces/bank/exchange/activate/'.$exchange['id']);
    }
    $page['new_exchanges']['new_exchanges_list'] = $list;
  }else{
    $page['new_exchanges']['text'] = array(
      '#type'=>'item',
      '#title'=>t('No requests for new exchanges.'),
    );
  }
  return $page;
  
}
