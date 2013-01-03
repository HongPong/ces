<?php
/**
 * @ingroup ces.bank
 * @{
 */
/**
 * @file
 * CES banking logic layer.
 */
/**
 * The controller for all the banking logic. This is the only accessible class from outside this package.
 */
class Bank{
  public function access($permission, $object, $objectid, $account = NULL) {
    $perm = new UserPermission($account);
    return $perm->access($permission, $object, $objectid);
  }
  /**
   * Create exchange. This exchange is not functional yet. It must be activated.
   */
  public function createExchange(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      //Create exchange record.
      unset($record['id']);
      $record['state'] = Exchange::STATE_INACTIVE;
      $this->updateObject('Exchange', $record);
      $notifier = new Notifier();
      $notifier->notify('ces_bank', 'new exchange', 1, 1, array('exchange' => $record));
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates the exchange record.
   */
  public function updateExchange(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('Exchange', $record);
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates and activates an exchange
   */
  public function activateExchange(array &$record) {
    $t = Serializer::DBTransaction();
    global $user;
    try{
      $s = new Serializer('Exchange');
      //activate exchange
      $exchange = $s->loadFromRecord($record);
      $exchange->activate();
      $s->save($exchange);
      //Create permission records. By the moment they are static, but they could
      //potentially be configured and stored in $record.
      //Let exchange users see the exchange details.
      $perm = array(
        'permission' => Permission::PERMISSION_VIEW,
        'object' => 'exchange details',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_EXCHANGE,
        'scopeid' => $exchange->id,
      );
      $this->createPermission($perm);
      //Let everybody register accounts to this exchange.
      $perm = array(
        'permission' => Permission::PERMISSION_USE,
        'object' => 'exchange accountcreator',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_GLOBAL,
        'scopeid' => 0,
      );
      $this->createPermission($perm);
      //Let exchange users see other exchange accounts.
      $perm = array(
        'permission' => Permission::PERMISSION_VIEW,
        'object' => 'exchange accounts',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_EXCHANGE,
        'scopeid' => $exchange->id,
      );
      $this->createPermission($perm);
      //Let exchange creator administer the exchange.
      $perm = array(
        'permission' => Permission::PERMISSION_ADMIN,
        'object' => 'exchange',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_USER,
        'scopeid' => $exchange->admin,
      );
      $this->createPermission($perm);
      //Create a new zero account for the admin of this exchange
      $account = $this->getDefaultAccount($exchange->id);
      $account['users'][0]['user'] = $exchange->admin;
      $account['name'] = $exchange->code . '0000';
      $this->createAccount($account);
      
      $notifier = new Notifier();
      $notifier->notify('ces_bank', 'exchange activated', $exchange->admin, $user->uid, array('exchange' => $exchange->getRecord()));
      $this->activateAccount($account);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates and activates an account.
   */
  public function activateAccount(array &$record) {
    $t = Serializer::DBTransaction();
    global $user;
    try{
      $s = new Serializer('LocalAccount');
      $account = $s->loadFromRecord($record);
      $account->activate();
      $s->save($account);
      //Create permission entry for account owner(s).
      $accountusers = $account->getUsers();
      foreach ($accountusers as $accountuser) {
        $perm = array(
          'permission' => Permission::PERMISSION_EDIT,
          'object' => 'account',
          'objectid' => $account->id,
          'scope' => Permission::SCOPE_USER,
          'scopeid' => $accountuser->user,
        );
        $this->createPermission($perm);
      }
      $notifier = new Notifier();
      //build recipients array;
      $recipients = array();
      $accusers = $account->getUsers();
      foreach ($accusers as $accuser) {
        $recipients[] = $accuser->user;
      }
      $notifier->notify('ces_bank', 'account activated', $recipients, $user->uid, array('account' => $account->getRecord()));
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Gets the exchange from its id
   */
  public function getExchange($id) {
    return $this->getObjectRecord('Exchange', $id);
  }
  /**
   * Deletes an exchange
   */
  public function deleteExchange($id) {
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * @return array with all exchanges in this server that satisfy certain conditions.
   * @param $conditions array conditions filter on the fields of exchange records.
   *                          By default return the active ones.
   */
  public function getAllExchanges(array $conditions = array('state' => 1)) {
    $s = new Serializer('Exchange');
    $exchanges = $s->loadCollection($conditions);
    $records = array();
    $as = new Serializer('LocalAccount');
    foreach ($exchanges as $exchange) {
      $record = $exchange->getRecord();
      $record['accounts'] = $as->count('exchange', $exchange->id);
      $records[$exchange->id] = $record;
    }
    return $records;
  }
  /**
   * @return array with all accounts that meet certain conditions.
   * @param $conditions array filter the records. A common example is:
   * <code>
   * $conditions = array( 'exchange' => $id, 'state' => 1 )
   * </code>
   */
  public function getAllAccounts(array $conditions = array('state' => 1), $order = NULL) {
    $as = new Serializer('LocalAccount');
    $accounts = $as->loadCollection($conditions, $order);
    $records = array();
    foreach ($accounts as $account) {
      $records[$account->id] = $account->getRecord();
    }
    return $records;
  }
  /**
   * Creates transaction. The transaction is not made effective until it is
   * applied.
   */
  public function createTransaction(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      $transaction = $this->newTransaction($record);
      $s = new Serializer(get_class($transaction));
      $s->save($transaction);
      $record['id'] = $transaction->id;
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates the transaction record.
   */
  public function updateTransaction(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('BasicTransaction', $record);
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return a transaction record given its id.
   */
  public function getTransaction($id) {
    return $this->getObjectRecord('BasicTransaction', $id);
  }
  /**
   * @param $conditions array
   * Associative array with conditions to be met by the returned transaction
   * records. Possible values include:
   * - account: id. Get transactions from or to the specified account id. If this
   * parameter is set then the following two must not be set.
   * - fromaccount: id. Get transactions where specified account id is the buyer.
   * - toaccount: id. Get transactions where the specified account id is the seller.
   * - createdsince: time. Get transactions created after this timestamp in seconds.
   * - createduntil: time. Get transactions created before this timestamp in seconds.
   * - limit: int. Get only n transactions.
   *
   * @return array
   * transaction records meeting all conditions specified by $conditions parameter.
   * The amount is expressed in a currency depending on the given conditions: if
   * 'account' is set, then it is used the account's currency. If 'fromaccount'
   * is set but 'toaccount' is ommited, then the fromaccount's currency is used.
   * Otherwise the currency is the seller's account one for each exchenge.
   */
  public function getTransactions(array $conditions) {
    $s = new Serializer('BasicTransaction');
    //Choose currency.
    $currencyaccount = NULL;
    $currency = NULL;
    if (isset($conditions['account']))
      $currencyaccount = $conditions['account'];
    if (isset($conditions['fromaccount']) && !isset($conditions['toaccount']))
      $currencyaccount = $conditions['fromaccount'];
    if ($currencyaccount!=NULL) {
      $as = new Serializer('LocalAccount');
      $account = $as->load($currencyaccount);
      $currency = $account->getExchange()->getCurrency();
    }
    //Load collection.
    if (isset($conditions['account'])) {
      //Shall we improve Serializer so it can directly handle 'or' conditions?
      $account = $conditions['account'];
      unset($conditions['account']);
      $conditions['fromaccount'] = $account;
      $transactions1 = $s->loadCollection($conditions, 'created');
      unset($conditions['fromaccount']);
      $conditions['toaccount'] = $account;
      $transactions2 = $s->loadCollection($conditions, 'created');
      $transactions = array_merge($transactions1, $transactions2);
    }
    else {
      $transactions = $s->loadCollection($conditions, 'created');
    }
    $records = array();
    foreach ($transactions as $t) {
      $record = $t->getRecord();
      if ($currency != NULL)
      $record['amount'] = $t->getAmount($currency);
      $records[] = $record;
    }
    return $records;
  }
  /**
   * creates a banking account
   */
  public function createAccount(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      //Create account record.
      unset($record['id']);
      $record['balance'] = 0;
      $account = new LocalAccount($record);
      $s = new Serializer('LocalAccount');
      $s->save($account);
      //saveExtraData() after create account record because we need the new
      //account id in order to create the AccountUser record(s).
      $account->saveExtraData();
      $record = $account->getRecord();
      $exchange = $account->getExchange();
      $notifier = new Notifier();
      $notifier->notify('ces_bank', 'new account', $exchange->admin, 1, array('account' => $account->getRecord()));
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * updates a banking account
   */
  public function updateAccount(array &$record) {
    $t = Serializer::DBTransaction();
    unset($record['balance']);
    try{
      return $this->updateObject('LocalAccount', $record);
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array an account given its id
   */
  public function getAccount($id) {
    return $this->getObjectRecord('LocalAccount', $id);
  }
  /**
   * @return array an account given its name
   * **/
  public function getAccountByName($name) {
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromUniqueKey('name', $name);
    if ($account === FALSE) return FALSE;
    return $account->getRecord();
  }
  private function getAccountHistoricValue($id, $field, $from, $to) {
    $s = new Serializer('LocalAccount');
    $account = $s->load($id);
    $st = new Serializer('BasicTransaction');
    $result = $st->selectFunctionField('SUM', 'amount',
      array(
        array($field, $account->id),
        array('created', $from, '>='),
        array('created', $to, '<='),
        array('state', array(3)),
      ));
    return $result;
  }
  public function getAccountHistoricSales($id, $from, $to = null) {
    if($to == null) $to = time();
    return $this->getAccountHistoricValue($id, 'toaccount', $from, $to);
  }
  public function getAccountHistoricPurchases($id, $from, $to = null) {
    if($to == null) $to = time();
    return $this->getAccountHistoricValue($id, 'fromaccount', $from, $to);
  }
  public function getAccountHistoricBalance($id, $time) {
    $s = new Serializer('LocalAccount');
    $account = $s->load($id);
    $balance = $account->balance;
    $positive = $this->getAccountHistoricSales($id, $time);
    $negative = $this->getAccountHistoricPurchases($id, $time);
    $timebalance =  $balance - $positive + $negative;
    return $timebalance;
  }
  /**
   * @return array of this user accounts.
   */
  public function getUserAccounts($uid) {
    $s = new Serializer('AccountUser');
    $useraccounts = $s->loadCollection(array('user' => $uid));
    $accounts = array();
    foreach ($useraccounts as $useracc) {
      $accounts[] = $this->getAccount($useracc->account);
    }
    return $accounts;
  }
  /**
   * deletes an account.
   */
  public function deleteAccount($id) {
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * creates a limit chain.
   */
  public function createLimitChain(array &$record) {
    unset($record['id']);
    return $this->updateLimitChain($record);
  }
  /**
   * updates a limit chain.
   */
  public function updateLimitChain(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      if (isset($record['limits'])) {
        foreach ($record['limits'] as $key => $limit) {
          if (!isset($limit['data'])) {
            $record['limits'][$key]['data'] = array();
          }
        }
      }
      else {
        $record['limits'] = array();
      }
      return $this->updateObject('LimitChain', $record);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array a limit chain given its id.
   */
  public function getLimitChain($id) {
    return $this->getObjectRecord('LimitChain', $id);
  }
  /**
   * deletes a limit chain
   */
  public function deleteLimitChain($id) {
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * @return array the default exchange. To be used as a model for new exchanges.
   */
  public function getDefaultExchange() {
    //TODO: hardcode it!
    $exchange = Exchange::getDefaultExchange();
    $exchange->admin = NULL;
    $exchange->id = NULL;
    return $exchange->getRecord();
  }
  /**
   * @return array the exchange's default account. To be used as a model for new.
   * accounts for this exchange.
   */
  public function getDefaultAccount($exchangeId) {
    $exchange = $this->getObject('Exchange', $exchangeId);
    return $exchange->getDefaultAccount()->getRecord();
  }
  /**
   * @return array the exchange's default limit chain.
   */
  public function getDefaultLimitChain($exchangeId) {
    $exchange = $this->getObject('Exchange', $exchangeId);
    $account  = $exchange->getDefaultAccount();
    return $account->getLimitChain()->getRecord();
  }
  /**
   * @return array all defined limit chains for this exchange.
   */
  public function getAllLimitChains($exchangeId) {
    $s = new Serializer('LimitChain');
    $limits = $s->loadCollection(array('exchange' => $exchangeId));
    $records = array();
    foreach ($limits as $limit) {
      $records[$limit->id] = $limit->getRecord();
    }
    return $records;
  }
  /**
   * Applies a transaction given its id.
   */
  public function applyTransaction($transactionId) {
    $t = Serializer::DBTransaction();
    try{
      //Get transaction. May be any type of transaction.
      $transaction = $this->getObject('BasicTransaction', $transactionId);
      //Trigger transaction application.
      $transaction->trigger();
      $transaction->check();
      $ret = FALSE;
      if ($transaction->getState() == Transaction::STATE_ACCEPTED) {
        $transaction->apply();
        if ($transaction->getState() == Transaction::STATE_COMMITED) {
          $ret = TRUE;
        }
      }
      if (!$ret) {
        $ret = $transaction->getLog($transaction->getState());
      }
      $s = new Serializer('BasicTransaction');
      $s->save($transaction);
      return $ret;
    }catch(Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array the names of classes that implement the AccountLimit interface.
   */
  /*
  public function getAccountLimitClasses() {
    $classes = module_invoke_all('account_limit_classes');
    $limits = array();
    foreach ($classes as $className) {
      if (class_exists($className)) {
        $limits[$className] = $className::getName();
      }
    }
    return $limits;
  }*/
  /**
   * @return array the names of classes that implement the DecoratedTransaction interface.
   */
  public function getDecoratedTransactionClasses() {
    $classes = module_invoke_all('decorated_transaction_classes');
    return $classes;
  }
  public function getPermission($id) {
    return $this->getObjectRecord('Permission', $id);
  }
  public function createPermission(array &$record) {
    $t = Serializer::DBTransaction();
    try {
      unset($record['id']);
      return $this->updateObject('Permission', $record);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  public function deletePermission($id) {
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  public function updatePermission(array &$record) {
    $t = Serializer::DBTransaction();
    try {
      return $this->updateObject('Permission', $record);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  public function getAdministrablePermissions($userid = NULL) {
    $userperms = new UserPermission($userid);
    $permissions = $userperms->getPermissions();
    $result = array();
    foreach ($permissions as $perm) {
      $result[] = $perm->getRecord();
    }
    return $result;
  }
  /**
   * @return A string representation of $amount following $exchange preference
   * and $format option.
   */
  public function formatAmount($amount, $exchange, $symbol = FALSE, $sign = FALSE) {
    $decimals = $exchange['currencyscale'];
    $number = number_format($amount, $decimals, '.', ' ');
    if ($symbol) {
      $number .= $exchange['currencysymbol'];
    }
    if($sign && $amount > 0) {
      $number = '+' . $number;
    }
    return $number;
  }
  private function getObject($className, $id) {
    $s = new Serializer($className);
    return $s->load($id);
  }
  private function getObjectRecord($className, $id) {
    $object = $this->getObject($className, $id);
    if ($object === FALSE)
      throw new Exception(t('Object of type %classname with id %id does not exist.', array('%classname' => $className, '%id' => $id)));
    return $object->getRecord();
  }
  private function updateObject($className, array &$record) {
    $s = new Serializer($className);
    $object = $s->loadFromRecord($record);
    $s->save($object);
    $record['id'] = $object->id;
    return TRUE;
  }
  /**
   * Creates a new transacion object based on the record array.
   * @return Transaction
   */
  private function newTransaction(array &$record) {
    unset($record['id']);
    $record['state'] = Transaction::STATE_NEW;
    //load accounts
    if (isset($record['toaccount'])) {
      $toaccount = $this->getAccount($record['toaccount']);
    }
    elseif (isset($record['toaccountname'])) {
      $toaccount = $this->getAccountByName($record['toaccountname']);
    }
    else {
      throw new Exception(t('Seller account not set.'));
    }
    if (isset($record['fromaccount'])) {
      $fromaccount = $this->getAccount($record['fromaccount']);
    }
    elseif (isset($record['fromaccountname'])) {
      $fromaccount = $this->getAccountByName($record['fromaccountname']);
    }
    else {
      throw new Exception(t('Seller account not set.'));
    }
    if ($toaccount === FALSE)
      throw new Exception(t('Error loading seller account.'));
    if ($fromaccount === FALSE)
      throw new Exception(t('Error loading buyer account.'));
    //set up acocunt id's
    $record['toaccount'] = $toaccount['id'];
    $record['fromaccount'] = $fromaccount['id'];
    if (!isset($record['data'])) {
      $record['data'] = array();
    }
    //Basic transaction.
    $transaction = new BasicTransaction($record);
    if ($toaccount['exchange'] != $fromaccount['exchange']) {
      //Interexchange transaction.
      $transaction = new InterExchangeTransaction($transaction, array());
    }
    return $transaction;
  }
  private function notifyTransaction(Transaction $transaction) {
    $notifier = new Notifier();
    $accountFromName = $transaction->getFromAccountName();
    $accountToName = $transaction->getToAccountName();
    $transactionUser = $transaction->getUser();
    $params = array(
      'transaction' => array(
        'amount' => $transaction->getAmount(),
        'concept' => $transaction->getConcept(),
        'fromaccount' => $accountFromName,
        'toaccount' => $accountToName,
      )
    );
    $this->notifyTransactionAccount('account debited', $accountFromName, $transactionUser, $params);
    $this->notifyTransactionAccount('account creadited', $accountToName, $transactionUser, $params);
  }
  /**
   * @
   */
  private function notifyTransactionAccount($messageKey, $accountName, $transactionUser, $params) {
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromUniqueKey('name', $accountName);
    $accountUsers = $account->getUsers();
    $notifier = new Notifier();
    foreach ($accountUsers as $accountUser) {
      if ($transactionUser != $accountUser->user) { //don't notify the transaction user
        $notifier->notify('ces_bank', $messageKey, $accountUser->user, $transactionUser, $params);
      }
    }
  }
}
/**
 * An exchange community.
 */
class Exchange extends DBObject{
  public static $table = 'ces_exchange';
  /**
  @var int
  */
  public $id;
  /**
  @var string
  */
  public $code;
  /**
  @var int
  */
  public $state;
  /**
  @var Currency
  */
  protected $currency;
  /**
  @var int
  */
  public $limitchain;
  /**
  @var string
  */
  public $shortname;
  /**
  @var string
  */
  public $name;
  /**
  @var string
  */
  public $country;
  /**
  @var string
  */
  public $region;
  /**
  @var string
  */
  public $town;
  /**
  @var string
  */
  public $website;
  /**
  @var string
  */
  public $map;
  /**
  Administrator user uid.
  @var int
  */
  public $admin;
  /**
  @var int
  */
  public $created;
  /**
  @var int
  */
  public $modified;
  /**
  @var int
  */
  const DEFAULT_EXCHANGE_ID = 1;
  const STATE_INACTIVE = 0;
  const STATE_ACTIVE = 1;
  public function loadRecord(&$record) {
    parent::loadRecord($record);
    $this->currency = new Currency($record);
  }
  public function getRecord() {
    $record = parent::getRecord();
    $record = array_merge($record, $this->currency->getRecord());
    unset($record['currency']);
    return $record;
  }
  /**
   * @return Exchange The default exchange. To be used as a model for new
   * exchanges. It is the exchange stored with id Exchange::DEFAULT_EXCHANGE_ID.
   */
  public static function getDefaultExchange() {
    $s = new Serializer('Exchange');
    $default = $s->load(Exchange::DEFAULT_EXCHANGE_ID);
    if ($default === FALSE)
      throw new Exception(t('Default exchange ( id = %id ) not found', array('%id' => self::DEFAULT_EXCHANGE_ID)));
    return $default;
  }
  /**
   * @return LocalAccount to be used as a prototype for new accounts.
   */
  public function getDefaultAccount() {
    global $user;
    $record = array(
      'id' => NULL,
      'exchange' => $this->id,
      'name' => $this->getFreeAccountName(),
      'balance' => 0.0,
      'state' => LocalAccount::STATE_HIDDEN,
      'kind' => LocalAccount::TYPE_INDIVIDUAL,
      'limitchain' => $this->limitchain,
      'users' => array(
        array(
          'account' => NULL,
          'user' => $user->uid,
          'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR
        ),
      )
    );
    $account = new LocalAccount($record);
    return $account;
  }
  private function getFreeAccountName() {
    //NOTE: this function is highly inefficient. It can be changed with a
    //single specific DB query if causes performance problems.
    $s = new Serializer('LocalAccount');
    $accounts = $s->loadCollection(array('exchange' => $this->id), 'name');
    $start = floor((count($accounts) -1) / 2); //This is a bit tricky.
    $code = !empty($accounts)?$accounts[$start]->getCode():0;
    if (!is_numeric($code)) $code = 0;
    for ($i=$start+1; $i<count($accounts); $i++) {
      $acccode = $accounts[$i]->getCode();
      if (is_numeric($acccode)) {
        if ($acccode - $code > 1) {
          break;
        }
        else {
          $code = $acccode;
        }
      }
    }
    $code =  '' . ($code +1) ;
    while (drupal_strlen($code)<4)
      $code = '0' . $code;
    return $this->code . $code;
  }
  /**
   * Activates a newly created exchange. Creates all the additional structures an
   * exchange needs in order to be operative. This includes:
   *  - default limit chain
   * And sets up the active flag to this exchange.
   * Exchange needs to be saved after this call so the state flag persists.
   */
  public function activate() {
    $es = new Serializer('Exchange');
    //Setup default limit chain
    if (empty($this->limitchain)) {
      //create default limit chain copying from the default exchange
      $defExchange = Exchange::getDefaultExchange();
      $defLimitChain = $defExchange->getDefaultAccount()->getLimitChain();
      $defLimitChain->id = NULL;
      $defLimitChain->exchange = $this->id;
      $ls = new Serializer('LimitChain');
      $ls->save($defLimitChain);
      $this->limitchain = $defLimitChain->id;
      $es->updateField($this, 'limitchain');
    }
    /*
    $virtAccount = $this->getVirtualAccount();
    if (empty($virtAccount)) {
      //create virtual account
      $virtAccount = $this->getDefaultAccount();
      $virtAccount->kind = LocalAccount::TYPE_VIRTUAL;
      $virtAccount->state = LocalAccount::STATE_HIDDEN;
      $virtAccount->name = $this->code . 'VIRT';
      $as = new Serializer('LocalAccount');
      $as->save($virtAccount);
    }
    */
    $this->state = Exchange::STATE_ACTIVE;
  }
  /**
   * @return Account the virtual account for this exchange. It is created if it
   * doesn't exist.
   * @todo notify exchange admin in case of virtual user creation.
   */
  public function getVirtualAccount(Exchange $exchange) {
    $s = new Serializer('LocalAccount');
    $name = $this->code . $exchange->code;
    $virtAccount = $s->loadFromUniqueKey('name', $name);
    if ($virtAccount === FALSE) {
      $record = array(
        'id' => NULL,
        'exchange' => $this->id,
        'name' => $name,
        'balance' => 0.0,
        'state' => LocalAccount::STATE_HIDDEN,
        'kind' => LocalAccount::TYPE_VIRTUAL,
        'limitchain' => $this->limitchain,
        'users' => array(
          array(
            'account' => NULL,
            'user' => 1,
            'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR
          ),
        )
      );
      $bank = new Bank();
      $bank->createAccount($record);
      $virtAccount = new LocalAccount($record);
    }
    return $virtAccount;
  }
  /**
   * @return Currency
   */
  public function getCurrency() {
    return $this->currency;
  }
}
/**
 * A currency. A currency belongs to a unique Exchange.
 */
class Currency extends DBObject{
  /**
  @var string
  */
  public $currencysymbol;
  /**
  @var string
  */
  public $currencyname;
  /**
  @var string
  */
  public $currenciesname;
  /**
  @var double
  */
  public $currencyvalue;
  /**
   @var int
   */
  public $currencyscale;
  /**
   * @return float the amount in this currency of the value represented by the
   * two given parameters.
   * @param $amount float the amount to be converted.
   * @param $currency Currency the original currency.
   */
  public function convert($amount, Currency $currency) {
    $rate = ( $currency->currencyvalue / $this->currencyvalue );
    $newamount = $amount * $rate;
    return $newamount;
  }
}
interface Account {
  public function getName();
  public function applyTransaction(Transaction &$transaction);
  public function checkTransaction(Transaction &$transaction);
}
/**
 * A banking account.
 */
class LocalAccount extends DBObject implements Account {
  /**
   * @var int
   */
  public $id;
  /**
   * @var int
   */
  public $exchange;
  /**
   * @var string
   */
  public $name;
  /**
   * @var double
   */
  public $balance;
  /**
   * @var int
   */
  public $limitchain;
  /**
   * @var int
   */
  public $created;
  /**
   * @var int
   */
  public $modified;
  /**
   * @var int
   */
  public $kind;
  /**
   * @var int
   */
  public $state;
  /**
   * @var array of users related to this account. Tipically this array is a
   * singleton with the account's administrator user. Each entry is an
   * AccountUser object.
   */
  private $users;
  public static $table = 'ces_account';
  const STATE_HIDDEN = 0;
  const STATE_ACTIVE = 1;
  const STATE_LOCKED = 2;
  const STATE_CLOSED = 3;
  const TYPE_INDIVIDUAL = 0;
  const TYPE_SHARED = 1;
  const TYPE_ORGANIZATION = 2;
  const TYPE_COMPANY = 3;
  const TYPE_PUBLIC = 4;
  const TYPE_VIRTUAL = 5;

  public function loadRecord(&$record) {
    parent::loadRecord($record);
    if (isset($record['users']) && is_array($record['users'])) {
      $this->users = array();
      foreach ($record['users'] as $accuser) {
        $this->users[] = new AccountUser($accuser);
      }
    }
    else {
      $s = new Serializer('AccountUser');
      $this->users = $s->loadCollection(array('account' => $this->id));
    }
  }
  public function getRecord() {
    $record = parent::getRecord();
    $record['users'] = array();
    foreach ($this->users as $useraccount) {
      $uarecord = $useraccount->getRecord();
      $record['users'][$uarecord['user']] = $uarecord;
    }
    return $record;
  }
  public function saveExtraData() {
    if (!empty($this->id)) {
      foreach ($this->users as $accuser) {
        $accuser->account = $this->id;
        $s = new Serializer('AccountUser');
        $s->save($accuser);
      }
    }
  }
  /**
   * @return LimitChain The limit chain associated to this account.
   */
  public function getLimitChain() {
    $s = new Serializer('LimitChain');
    return $s->load($this->limitchain);
  }
  /**
   * This function updates the amount of this account with this transaction if
   * it is accepted. The updated account is safely saved to the DB.
   */
  public function applyTransaction(Transaction &$transaction) {
    $s = new LockSerializer('LocalAccount');
    //work with updated data and lock this account in database.
    $account = $s->load($this->id);
    $amount = $transaction->getAmount();
    if ($transaction->getFromAccountName() == $this->name) {
      $account->balance = $account->balance - $amount;
    }
    if ($transaction->getToAccountName() == $this->name) {
      $account->balance = $account->balance + $amount;
    }
    $this->balance = $account->balance; //update actual object amount because it could be reused.
    $s->updateField($account, 'balance');
  }
  public function checkTransaction(Transaction &$transaction) {
    if ($this->state == LocalAccount::STATE_ACTIVE) {
      return $this->getLimitChain()->checkTransaction($this, $transaction);
    }
    else {
      return array(t('User %user cannot accept transactions because is not active.', array('%user' => $this->name)));
    }
  }
  public function activate() {
    $this->state = LocalAccount::STATE_ACTIVE;
  }
  public function getName() {
    return $this->name;
  }
  public function getCode() {
    return drupal_substr($this->name, 4);
  }
  public function &getUsers() {
    return $this->users;
  }
  /**
   * @return Exchange the exchange object to which this account belongs.
   */
  public function getExchange() {
    $es = new Serializer('Exchange');
    return $es->load($this->exchange);
  }
}
class AccountUser extends DBObject{
  public static $table = 'ces_accountuser';
  public static $primary_key = array('user', 'account');
  public $id;
  public $user;
  public $account;
  public $role;
  const ROLE_ACCOUNT_ADMINISTRATOR = 0;
}
/**
 * A chain of account limits. It is used by account.
 */
class LimitChain extends DBObject {
  public static $table = 'ces_limitchain';
  /**
  @var int
  */
  public $id;
  /**
  @var int
  */
  public $exchange;
  /**
  @var String
  */
  public $name;
  const DEFAULT_LIMITCHAIN_ID = 1;
  /**
   * @var array of AccountLimit
   */
  protected $limits;
  /**
   * @override
   */
  public function loadRecord(&$record) {
    //load fields
    parent::loadRecord($record);
    //load limits
    $this->limits = array();
    $s = new Serializer('AccountLimit');
    if (isset($record['limits'])) {//record comes with limits. This is because it comes from UI
      foreach ($record['limits'] as $limitRecord) {
        $limitRecord['limitchain'] = $this->id;
        $this->limits[] = $s->loadFromRecord($limitRecord);
      }
    }
    else {
      $this->limits = $s->loadCollection(array('limitchain' => $this->id));
    }
  }
  public function getRecord() {
    $record = parent::getRecord();
    $record['limits'] = array();
    foreach ($this->limits as $limit) {
      $record['limits'][] = $limit->getRecord();
    }
    return $record;
  }
  /**
   * @override
   */
  public function saveExtraData() {
    $s = new Serializer('AccountLimit');
    $newids = array();
    //create/update limits
    foreach ($this->limits as $limit) {
      $s->save($limit);
      $newids[] = $limit->id;
    }
    //delete unused limits.
    //This is inefficient (it could be done in a single DB query),
    //but it doesn't matter since this is a very occasional operation.
    $limits = $s->loadCollection(array('limitchain' => $this->id));
    $allids = array();
    foreach ($limits as $limit) {
      $allids[] = $limit->id;
    }
    $oldids = array_diff($allids, $newids);
    foreach ($oldids as $id) {
      $s->delete($id);
    }
  }
  public function checkAccount(LocalAccount &$account) {
    $messages = array();
    foreach ($this->limits as $limit) {
      if ($limit->checkAccount($account)!==TRUE) {
        $messages[] = $limit->getFailMessage($account);
      }
    }
    if (empty($messages)) {
      return TRUE;
    }
    else {
      return $messages;
    }
  }
  public function checkTransaction(LocalAccount &$account, Transaction &$transaction) {
    $messages = array();
    foreach ($this->limits as $limit) {
      if (!$limit->checkTransaction($account, $transaction)) {
        $messages[] = $limit->getFailMessage($account);
      }
    }
    if (empty($messages)) {
      return TRUE;
    }
    else {
      return $messages;
    }
  }
  public function getAccountLimits() {
    return $this->limits;
  }
}
/**
 * Generic class for account limits. Each account limit limites one aspect of accounts.
 */
abstract class AccountLimit extends DBObject{
  public static $table = 'ces_limit';
  /**
   * @var int
   */
  public $id;
  /**
   * @var double
   */
  public $value;
  /**
   * @var boolean
   */
  public $block;
  /**
   * @var int
   */
  public $limitchain;
  /**
   * @var string
   */
  public $classname;
  /**
   * @var array
   */
  public $data;
  /**
   * @return bool. The state of this account against this AccountLimit. TRUE
   * for pass and FALSE for fail.
   */
  public function checkAccount(LocalAccount &$account) {
    return ($this->getAccountValue($account) > $this->getLimit());
  }
  /**
   * @return int. The state of this account when this transaction is performed.
   * The account can be the seller or the buyer.
   * **/
  public function checkTransaction(LocalAccount &$account, Transaction &$transaction) {
    if (!$this->block)
      return TRUE; //non-blocking limits always approve transactions.
    else {
      $value = $this->getAccountValue($account);
      $newvalue = $value;
      $transactionValue = $this->getTransactionValue($transaction);
      if ($account->getName() == $transaction->getFromAccountName()) {
        $newvalue -= $transactionValue;
      }
      elseif ($account->getName() == $transaction->getToAccountName()) {
        $newvalue += $transactionValue;
      }
      if ($newvalue >= $this->getLimit())//account is ok after transaction
        return TRUE;
      elseif ($newvalue >= $value) //account is not ok after transaction but improves its mark.
        return TRUE;
      else                        //block transaction
        return FALSE;
    }
  }
  public function getLimit() {
    return $this->value;
  }
  /**
   * @return double. The amount of this account.
   */
  public abstract function getAccountValue(LocalAccount &$account);
  /**
   * @return double. The amount of this transaction.
   */
  public abstract function getTransactionValue(Transaction &$transaction);
  public abstract function getFailMessage(LocalAccount &$account);
}
/**
 * Limites an account if its balance is less than a given value.
 */
class AbsoluteDebitLimit extends AccountLimit {
  public function getAccountValue(LocalAccount &$account) {
    return $account->balance;
  }
  public function getTransactionValue(Transaction &$transaction) {
    return $transaction->getAmount();
  }
  public function getFailMessage(LocalAccount &$account) {
    return t('Account %account exceeds the debit limit of %limit.', array('%limit' => $this->getLimit(), '%account' => $account->getName()));
  }
}
/**
 * Limites an account if its balance is more than a given value.
 */
class AbsoluteCreditLimit extends AccountLimit {
  public function getAccountValue(LocalAccount &$account) {
    return - $account->balance;
  }
  public function getTransactionValue(Transaction &$transaction) {
    return - $transaction->getAmount();
  }
  public function getLimit() {
    return - $this->value;
  }
  public function getFailMessage(LocalAccount &$account) {
    return t('Account %account exceeds the credit limit of %limit.', array('%limit' => $this->value, '%account' => $account->getName()));
  }
}
/**
 * Transaction interface
 *
 *
 * Transactions
 * ============
 * A transaction is an amount of money to be transferred from one account to
 * another account. The basic transaction moves certanin quantity of money through
 * two accounts within the same exchange. There are other king of transactions
 * moving currency between servers or third party applications. In each echange
 * there is an special account called 'virtual account'. This account acts as the
 * foreign account for all inter-exchange transactions. It is also used to
 * temporally save the money in delayed transactions.
 */
interface Transaction {
  /**
   * @return the currency amount of this transaction.
   */
  public function getAmount(Currency $currency = NULL);
  /**
   * @return int
   * The id of the user who has created this transaction.
   */
  public function getUser();
  /**
   * @return string
   * A short description of this transaction.
   */
  public function getConcept();
  /**
   * @return string
   * The buyer account public ID
   */
  public function getFromAccountName();
  /**
   * @return string
   * The seller account public ID
   */
  public function getToAccountName();
  /**
   * @return int
   * The current state of this transaction.
   */
  public function getState();
  /**
   * @return array
   * Array of messages with details about given $state.
   */
  public function getLog($state);
  /**
   * Trigger this transaction.
   */
  public function trigger();
  /**
   * Check this transaction.
   */
  public function check();
  /**
   * Apply this transaction.
   */
  public function apply();
  /**
   * Revoke this transaction.
   */
  public function revoke();
  /**
   * The transaction has been created but hasn't already been triggered to
   * apply. This is the state while the transaction can be edited. Waiting
   * for the user to apply this transaction.
   */
  const STATE_NEW = 0;
  /**
   * The transaction has been triggered to be applied. Waiting for acceptance
   * from all authorities.
   */
  const STATE_TRIGGERED = 1;
  /**
   * The transaction has been accepted by all authorities. It is being
   * effectively applied by the system. This state is short-lived in local
   * transactions.
   */
  const STATE_ACCEPTED = 2;
  /**
   * The transaction has been successfully applied. It can however be revoked.
   * If there aren't issues for this transaction after a prudencial time, it
   * will be archieved.
   */
  const STATE_COMMITED = 3;
  /**
   * The transaction is archieved and successfully applied. Nothing can be done
   * with it. It exists only as historical data.
   */
  const STATE_ARCHIVED = 4;
  /**
   * The transaction -triggered to apply- has been rejected by some of the
   * authorities. It will be discarted in some days if nobody changes their
   * opinion.
   */
  const STATE_REJECTED = 5;
  /**
   * The transaction is discarted and has not been applied. Nothing can be done
   * with it. It exists only as historical data.
   */
  const STATE_DISCARDED = 6;
  /**
   * The transaction has been triggered to be revoked. Waiting for acceptance of
   * this operation by all authorities.
   */
  const STATE_REVOKE_TRIGGERED = 7;
  /**
   * The revoke operation has been accepted by all authorities. The system is
   * effectively doing the revoke. Short-lived state for local transactions.
   */
  const STATE_REVOKE_ACCEPTED = 8;
  /**
   * Some authorities rejected the revoke operation, so the transaction will
   * remain applied.
   */
  const STATE_REVOKE_REJECTED = 9;
  /**
   * The transaction has been successfully revoked. Marked to be discarted after
   * a prudential time.
   */
  const STATE_REVOKED = 10;
  /**
   * There has been a system error with this transaction. It requires
   * administrator attention.
   */
  const STATE_ERROR = 11;

}
/**
 * Concrete class for the basic transaction. It can be enchanced using DecoratorTransactions.
 */
class BasicTransaction extends DBObject implements Transaction{
  /**
   * @var int
   * Transaction identifier.
   */
  public $id;
  /**
   * @var int
   * Buyer account identifier.
   */
  public $fromaccount;
  /**
   * @var int
   * Seller account identifier.
   */
  public $toaccount;
  /**
   * @var float
   */
  public $amount;
  /**
   * @var int
   */
  public $user;
  /**
   * @var string
   */
  public $concept;
  /**
   * @var int
   */
  public $state;
  /**
   * @var int
   */
  public $created;
  /**
   * @var int
   */
  public $modified;
  /**
   * @var array
   */
  public $data;
  public static $table = 'ces_transaction';
  /**
   * @return the amount of this transaction in the given currency.
   * @param $currency The currency to be used. If ommited it returns the value
   * in the seller's exchange currency.
   */
  public function getAmount(Currency $currency = NULL) {
    if ($currency != NULL) {
      //get current (seller's) currency.
      $as = new Serializer('LocalAccount');
      $toaccount = $as->load($this->toaccount);
      return $currency->convert($this->amount, $toaccount->getExchange()->getCurrency());
    }
    else {
      return $this->amount;
    }
  }
  public function getUser() {
    return $this->user;
  }
  public function getConcept() {
    return $this->concept;
  }
  public function getFromAccountName() {
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->fromaccount);
    return $acc->name;
  }
  public function getToAccountName() {
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->toaccount);
    return $acc->name;
  }
  public function getState() {
    return $this->state;
  }
  public function log(array $messages) {
    if (empty($messages)) return;
    if (!isset($this->data))
      $this->data = array();
    if (!isset($this->data['log']))
      $this->data['log'] = array();
    if (!isset($this->data['log'][$this->state]))
      $this->data['log'][$this->state] = array();
    $this->data['log'][$this->state] = array_merge($this->data['log'][$this->state], $messages);
  }
  public function getLog($state) {
    if (isset($this->data) && isset($this->data['log']) && isset($this->data['log'][$state])) {
      return $this->data['log'][$state];
    }
    else{
      return array();
    }
  }
  /**
   * Changes the transaction state.
   */
  public function setState($state) {
    $flow = array(
      Transaction::STATE_NEW => array(Transaction::STATE_TRIGGERED, Transaction::STATE_DISCARDED),
      Transaction::STATE_TRIGGERED => array(Transaction::STATE_ACCEPTED, Transaction::STATE_REJECTED),
      Transaction::STATE_ACCEPTED => array(Transaction::STATE_COMMITED, Transaction::STATE_ERROR),
      Transaction::STATE_COMMITED => array(Transaction::STATE_ARCHIVED, Transaction::STATE_REVOKE_TRIGGERED),
      Transaction::STATE_REJECTED => array(Transaction::STATE_TRIGGERED, Transaction::STATE_DISCARDED),
    );
    $candidates = $flow[$this->state];
    if (!in_array($state, $candidates)) {
      throw new Exception(t('Invalid state transition for transaction %trans.', array('%trans' => $this->id)));
    }
    $this->state = $state;
  }
  public function trigger() {
    $this->setState(Transaction::STATE_TRIGGERED);
  }
  public function check() {
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->load($this->fromaccount);
    $accountTo = $s->load($this->toaccount);
    $messagesFrom = $accountFrom->checkTransaction($this);
    $messagesTo   = $accountTo->checkTransaction($this);
    if ($messagesFrom === TRUE && $messagesTo === TRUE) {
      $this->setState(Transaction::STATE_ACCEPTED);
    }
    else {
      $this->setState(Transaction::STATE_REJECTED);
      if ($messagesFrom === TRUE) {
        $messages = $messagesTo;
      }
      elseif ($messagesTo === TRUE ) {
        $messages = $messagesFrom;
      }
      else {
        $messages = array_($messagesFrom, $messagesTo);
      }
      $this->log($messages);
    }
  }
  public function apply() {
    //load account objects
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->load($this->fromaccount);
    $accountTo = $s->load($this->toaccount);
    //apply transaction to both accounts
    try{
      $accountTo->applyTransaction($this);
      $accountFrom->applyTransaction($this);
      $this->setState(Transaction::STATE_COMMITED);
      return TRUE;
    }
    catch (Exception $e) {
      $this->setState(Transaction::STATE_ERROR);
      $this->log(array($e->getMessage()));
      throw $e;
    }
  }
  public function revoke() {
    throw new Exception('Not implemented yet');
  }
  private static function getStateString($state) {
    $string = array(
        0 => 'NEW',
        1 => 'TRIGGERED',
        2 => 'ACCEPTED',
        3 => 'COMMITED',
        4 => 'ARCHIVED',
        5 => 'REJECTED',
        6 => 'DISCARTED',
        7 => 'REVOKE TRIGGERED',
        8 => 'REVOKE ACCEPTED',
        9 => 'REVOKE REJECTED',
        10 => 'REVOKED',
        11 => 'ERROR',
    );
    return $string[$state];
  }
}
/**
 * Abstract class for transaction decorators.
 */
abstract class DecoratedTransaction extends DBObject implements Transaction{
    /**
     * @var Transaction
     */
    private $parent;
    public static $table = 'ces_transaction';
    
    public function getAmount(Currency $currency = NULL) {
      return $this->parent->getAmount($currency);
    }
    public function getUser() {
      return $this->parent->getUser();
    }
    public function getConcept() {
      return $this->parent->getConcept();
    }
    public function getFromAccountName() {
      return $this->parent->getFromAccountName();
    }
    public function getToAccountName() {
      return $this->parent->getToAccountName();
    }
    public function getParent() {
      return $this->parent;
    }
    public function getState() {
      return $this->parent->getState();
    }
    public function getLog($state) {
      return $this->parent->getLog($state);
    }
    public function log($messages) {
      return $this->parent->log($messages);
    }
    public function trigger() {
      return $this->parent->trigger();
    }
    public function check() {
      return $this->parent->check();
    }
    public function apply() {
      return $this->parent->apply();
    }
    public function revoke() {
      return $this->parent->revoke();
    }
    public function setState($state) {
      return $this->parent->setState($state);
    }
    public function __construct($parent, $record) {
      parent::__construct($record);
      $this->parent = $parent;
    }
}
/**
 * This class extends the basic transaction features to allow transactions
 * between distinct exchanges. The amount of this transaction has always to be
 * understood in the seller's exchange currency.
 */
class InterExchangeTransaction extends DecoratedTransaction{
  /**
   * @var BasicTransaction
   * Transaction buyer <-> virtual. This is not stored in the DB.
   */
  protected $virtualTransactionFrom;
  /**
   * @var BasicTransaction
   * Transaction virtual <-> seller. This is not stored in the DB.
   */
  protected $virtualTransactionTo;
  public function trigger() {
    $this->getVirtualTransactionFrom()->trigger();
    $this->getVirtualTransactionTo()->trigger();
    if ($this->getVirtualTransactionFrom()->getState() == Transaction::STATE_TRIGGERED
    && $this->getVirtualTransactionTo()->getState() == Transaction::STATE_TRIGGERED) {
      $this->setState(Transaction::STATE_TRIGGERED);
    }
  }
  public function check() {
    //Check two virtual transactions.
    $virtFrom = $this->getVirtualTransactionFrom();
    $virtFrom->check();
    $virtTo = $this->getVirtualTransactionTo();
    $virtTo->check();
    //Merge results.
    if ($virtFrom->getState() == Transaction::STATE_ACCEPTED
    && $virtTo->getState() == Transaction::STATE_ACCEPTED) {
      $this->setState(Transaction::STATE_ACCEPTED);
    }
    elseif ($virtFrom->getState() == Transaction::STATE_REJECTED
    || $virtTo->getState() == Transaction::STATE_REJECTED) { //one of the two must be rejected.
      $this->setState(Transaction::STATE_REJECTED);
    }
    elseif ($virtFrom->getState() == Transaction::STATE_TRIGGERED
    || $virtTo->getState() == Transaction::STATE_TRIGGERED) {
      $this->setState(Transaction::STATE_TRIGGERED);
    }
    //Add the virtual logs regarding the final state of the whole interexchange transaction.
    $this->log($virtFrom->getLog($this->getState()));
    $this->log($virtTo->getLog($this->getState()));
  }
  public function apply() {
    //Apply the two virtual transactions.
    $virtFrom = $this->getVirtualTransactionFrom();
    $virtFrom->apply();
    $virtTo = $this->getVirtualTransactionTo();
    $virtTo->apply();
    //Merge state.
    if ($virtFrom->getState() == Transaction::STATE_COMMITED
    && $virtTo->getState() == Transaction::STATE_COMMITED) {
      $this->setState(Transaction::STATE_COMMITED);
    }
    elseif ($virtFrom->getState() == Transaction::STATE_ERROR
    || $virtTo->getState() == Transaction::STATE_ERROR) {
      $this->setState(Transaction::STATE_ERROR);
    }
    $this->log($virtFrom->getLog($this->getState()));
    $this->log($virtTo->getLog($this->getState()));
  }
  protected function getVirtualTransactionFrom() {
    if ($this->virtualTransactionFrom == NULL) {
      $sa = new Serializer('LocalAccount');
      $toAccount = $sa->loadFromUniqueKey('name', $this->getToAccountName());
      $fromAccount = $sa->loadFromUniqueKey('name', $this->getFromAccountName());
      $toExchange = $toAccount->getExchange();
      $fromExchange = $fromAccount->getExchange();
      $fromVirtualAccount = $fromExchange->getVirtualAccount($toExchange);
      $amount = $this->getAmount($fromExchange->getCurrency());
      $this->virtualTransactionFrom = new BasicTransaction(array(
        'fromaccount' => $fromAccount->id,
        'toaccount' => $fromVirtualAccount->id,
        'amount' => $amount,
        'concept' => $this->getConcept(),
        'user' => $this->getUser(),
        'state' => $this->getState(),
      ));
    }
    return $this->virtualTransactionFrom;
  }
  protected function getVirtualTransactionTo() {
    if ($this->virtualTransactionTo == NULL) {
      $sa = new Serializer('LocalAccount');
      $toAccount = $sa->loadFromUniqueKey('name', $this->getToAccountName());
      $fromAccount = $sa->loadFromUniqueKey('name', $this->getFromAccountName());
      $toExchange = $toAccount->getExchange();
      $fromExchange = $fromAccount->getExchange();
      $toVirtualAccount = $toExchange->getVirtualAccount($fromExchange);

      $this->virtualTransactionTo = new BasicTransaction(array(
          'fromaccount' => $toVirtualAccount->id,
          'toaccount' => $toAccount->id,
          'amount' => $this->getAmount(), //This transaction is in the "to" currency
          'concept' => $this->getConcept(),
          'user' => $this->getUser(),
          'state' => $this->getState(),
      ));
    }
    return $this->virtualTransactionTo;
  }
}
/**
 *
 */
class LevyedTransaction extends DecoratedTransaction{
  /**
   * @var double percentage (over 100)
   */
  public $levy;
  public function apply() {
    $fromAccountName = $this->getFromAccountName();
    $s = new Serializer('LocalAccount');
    $fromAccount = $s->loadFromUniqueKey('name', $fromAccountName);
    $exchange = $fromAccount->getExchange();
    $levyAccount = $exchange->getLevyAccount();
    $amount = $this->getAmount()* $this->levy / 100.0;
    $levyTransaction = new BasicTransaction(array(
        'fromaccount' => $this->fromaccount,
        'toaccount' => $levyAccount->id,
        'amount' => $amount,
        'concept' => t('Levy. Transaction %trans', array('%trans', $this->getId())),
        'user' => $this->getUser(),
    ));
    //TODO: finish this!
  }
}
/**
 * The set of permissions of a particular user
 */
class UserPermission{
  /**
   * @var int
   */
  private $userid;
  /**
   * @var array of Permission
   */
  private static $permissions = array();
  /**
   * @param $user int
   */
  public function __construct($userid) {
    global $user;
    if ($userid === NULL) {
      $this->userid = $user->uid;
    }
    else{
      $this->userid = $userid;
    }
  }
  /**
   * @param $permission int or string
   * @param $object String
   * @param $objectid int
   * @return bool whether user of this Permissions object has access of type given by
   * the $permission parameter over the object ($object, $objectid).
   */
  public function access($permission, $object, $objectid) {
    if ($this->userid == 1)
      return TRUE;
    if (is_string($permission)) {
      $equiv = array(
        'view' => 10,
        'use' => 20,
        'edit' => 30,
        'admin' => 40,
      );
      $permission = $equiv[$permission];
    }
    //Treat here the recursive case of transaction
    if ($object == 'transaction') {
      $ts = new Serializer('BasicTransaction');
      $as = new Serializer('LocalAccount');
      $transaction = $ts->load($objectid);
      $fromaccount = $as->loadFromUniqueKey('name', $transaction->getFromAccountName());
      $toaccount = $as->loadFromUniqueKey('name', $transaction->getToAccountName());
      return $this->access($permission, 'account buyer', $fromaccount->id)
          || $this->access($permission, 'account seller', $toaccount->id);
    }
    //compute permissions
    $userperms = &$this->getPermissions();
    foreach ($userperms as $userperm) {
      if ($userperm->permission >= $permission
        && $this->belongs($object, $objectid, $userperm->object, $userperm->objectid)) {
        return TRUE;
      }
    }
    return FALSE;
  }
  /**
   * compute the user's permissions and populate the private internal variable
   * $permissions.
   * Note: this function easily needs 10 or 15 database queries. It is
   * recommendable to make cache of its result. Further, it could be easily
   * improved to do only 3 or 4 queries if it is necessary (changing the foreach
   * loops for IN(...) SQL statements).
   */
  public function &getPermissions() {
    if (!isset(self::$permissions[$this->userid])) {
      $sp = new Serializer('Permission');
      if ($this->userid == 1) {
        $permissions = $sp->loadCollection(array());
      }
      else {
        $permissions = array();
        //Get permissions directly granted to this user.
        $userperms = $sp->loadCollection(array('scope' => 'user', 'scopeid' => $this->userid));
        $permissions = &array_merge($permissions, $userperms);
        //Get permissions granted to user's accounts. Compute exchanges.
        $sau = new Serializer('AccountUser');
        $useraccounts = $sau->loadCollection(array('user' => $this->userid));
        foreach ($useraccounts as $useraccount) {
          $accountperms = $sp->loadCollection(array('scope' => 'account', 'scopeid' => $useraccount->account));
          $permissions = &array_merge($permissions, $accountperms);

        }
        //Compute exchanges.
        $exchanges = array();
        $sla = new Serializer('LocalAccount');
        foreach ($useraccounts as $useraccount) {
          $account = $sla->load($useraccount->account);
          if (!in_array($account->exchange, $exchanges)) {
            $exchanges[] = $account->exchange;
          }
        }
        //Get permissions granted to user's accounts exchanges.
        foreach ($exchanges as $exchange) {
          $exchangeperms = $sp->loadCollection(array('scope' => 'exchange', 'scopeid' => $exchange));
          $permissions = &array_merge($permissions, $exchangeperms);
        }
        //Get global permissions.
        $globalperms = $sp->loadCollection(array('scope' => 'global'));
        $permissions = &array_merge($permissions, $globalperms);
      }
      //Set the static variable.
      self::$permissions[$this->userid] = &$permissions;
    }
    return self::$permissions[$this->userid];
  }
  /**
   * @return boolean
   * Whether (object1,objectid1) belongs to (object2, objectid2).
   */
  private function belongs($object1, $objectid1, $object2, $objectid2) {
    //Case permission. Permission belongs to A iif the object of that permission
    //belongs to A.
    if ($objectid1 == 'permission') {
      $s = new Serializer('Permission');
      $perm = $s->load($objectid1);
      return belongs($perm->object, $perm->objectid, $object2, $objectid2);
    }
    //Case same scope. Compare identifiers.
    if (substr($object1, 0, strlen($object2)) == $object2) {
      return $objectid1 == $objectid2 || (substr($object1, 0, 6) == 'global');
    }
    elseif (strpos($object2, ' ') === FALSE) {
      //All belongs to global scope
      if ($object2 == Permission::SCOPE_GLOBAL) {
        return TRUE;
      }
      //Strip non-scope keywords from object1
      $object1 = substr($object1, 0, strpos($object1, ' '));
      //At this point we know that $object2 is one of 'account', 'exchange' and
      //$object1 one of 'account', 'exchange', 'global' and $object2 != $object1
      //so the only favorable possibily is the following.
      if ($object1 == Permission::SCOPE_ACCOUNT && $object2 == Permission::SCOPE_EXCHANGE) {
        $s = new Serializer('LocalAccount');
        $account = $s->load($objectid1);
        return ($account->exchange == $objectid2);
      }
      else {
        return FALSE;
      }
    }
    else {
      return FALSE;
    }
  }
}
class Permission extends DBObject{
  /**
   * @var int
   */
  public $id;
  /**
   * @var int
   */
  public $permission;
  /**
   * @var String
   * Determines, together with $objectid, the target data of this permission.
   * It begins with a scope string and optionally contain another
   * keyword separated by space. eg. 'account' or 'account balance'. This scope
   * cannot be user. It can be only 'global', 'exchange' or 'account'
   */
  public $object;
  /**
   * @var int
   */
  public $objectid;
  /**
   * @var String
   * Determines, together with scopeid, the group of users having this
   * permission.
   * Is a single keyword. Must be one of 'global', 'exchange', 'account', 'user'
   */
  public $scope;
  /**
   * @var int
   */
  public $scopeid;
  public static $table = 'ces_permission';
  const PERMISSION_VIEW = 10;
  const PERMISSION_USE = 20;
  const PERMISSION_EDIT = 30;
  const PERMISSION_ADMIN = 40;
  const SCOPE_GLOBAL = 'global';
  const SCOPE_EXCHANGE = 'exchange';
  const SCOPE_ACCOUNT = 'account';
  const SCOPE_USER = 'user';
}
/**
 * @} End of "ingroup ces.bank".
 */