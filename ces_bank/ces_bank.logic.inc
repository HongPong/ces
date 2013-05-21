<?php
/**
 * @file
 * CES banking logic layer.
 */
/**
 * @ingroup ces.bank
 * @{
 */
/**
 * The controller for all the banking logic. This is the only accessible class
 * from outside this package.
 */
class Bank {
  /**
   * TODO Documentation.
   */
  public function access($permission, $object, $objectid, $account = NULL) {
    $perm = new UserPermission($account);
    return $perm->access($permission, $object, $objectid);
  }
  /**
   * Create exchange. This exchange is not functional yet. It must be activated.
   */
  public function createExchange(array &$record) {
    $t = Serializer::DBTransaction();
    try {
      // Create exchange record.
      unset($record['id']);
      $record['state'] = Exchange::STATE_INACTIVE;
      $this->updateObject('Exchange', $record);
      $notifier = new Notifier();
      $notifier->notify('ces_bank', 'new exchange', 1, 1, array('exchange' => $record));
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates the exchange record.
   */
  public function updateExchange(array &$record) {
    $t = Serializer::DBTransaction();
    try {
      return $this->updateObject('Exchange', $record);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates and activates an exchange.
   */
  public function activateExchange(array &$record) {
    $t = Serializer::DBTransaction();
    global $user;
    try{
      $s = new Serializer('Exchange');
      // Activate exchange.
      $exchange = $s->loadFromRecord($record);
      $exchange->activate();
      $s->save($exchange);
      // Create permission records. By the moment they are static,but they could
      // potentially be configured and stored in $record.
      // Let exchange users see the exchange details.
      $perm = array(
        'permission' => Permission::PERMISSION_VIEW,
        'object' => 'exchange details',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_EXCHANGE,
        'scopeid' => $exchange->id,
      );
      $this->createPermission($perm);
      // Let everybody register accounts to this exchange.
      $perm = array(
        'permission' => Permission::PERMISSION_USE,
        'object' => 'exchange accountcreator',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_GLOBAL,
        'scopeid' => 0,
      );
      $this->createPermission($perm);
      // Let exchange users see other exchange accounts.
      $perm = array(
        'permission' => Permission::PERMISSION_VIEW,
        'object' => 'exchange accounts',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_EXCHANGE,
        'scopeid' => $exchange->id,
      );
      $this->createPermission($perm);
      // Let exchange creator administer the exchange.
      $perm = array(
        'permission' => Permission::PERMISSION_ADMIN,
        'object' => 'exchange',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_USER,
        'scopeid' => $exchange->admin,
      );
      $this->createPermission($perm);
      // Let exchange users see blog posts and comment them.
      $perm = array(
        'permission' => Permission::PERMISSION_USE,
        'object' => 'exchange blog',
        'objectid' => $exchange->id,
        'scope' => Permission::SCOPE_EXCHANGE,
        'scopeid' => $exchange->id,
      );
      $this->createPermission($perm);

      // Create a new zero account for the admin of this exchange.
      $account = $this->getDefaultAccount($exchange->id);
      $account['users'] = array(
        $exchange->admin => array(
          'user' => $exchange->admin,
          'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
        ),
      );
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
      // Create permission entry for account owner(s).
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
      // Build recipients array.
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
   * Gets the exchange from its id.
   */
  public function getExchange($id) {
    return $this->getObjectRecord('Exchange', $id);
  }
  /**
   * Gets the exchange object by name.
   *
   * @return array
   *   an exchange given its name, or FALSE if not available.
   */
  public function getExchangeByName($name) {
    $s = new Serializer('Exchange');
    $exchange = $s->loadFromUniqueKey('code', $name);
    if ($exchange === FALSE) {
      return FALSE;
    }
    return $exchange->getRecord();
  }
  /**
   * Deletes an exchange.
   */
  public function deleteExchange($id) {
    // @TODO: implement this.
    throw new Exception('Not implemented yet');
  }
  /**
   * Gets all exchanges under certain conditions.
   *
   * @return array
   *   with all exchanges in this server that satisfy certain conditions.
   *
   * @param array $conditions 
   *   conditions filter on the fields of exchange records. By default return the
   *   active ones.
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
   * Get all accounts that meet certain conditions.
   *
   * @return array 
   *   with all accounts that meet certain conditions.
   *
   * @param array $conditions 
   *   filter the records. A common example is:
   *   <code>
   *   $conditions = array( 'exchange' => $id, 'state' => 1 )
   *   </code>
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
   * Creates transaction when applied. 
   * 
   * The transaction is not made effective until it is applied.
   */
  public function createTransaction(array &$record) {
    $t = Serializer::DBTransaction();
    try{
      $transaction = $this->newTransaction($record);
      $s = new Serializer(get_class($transaction));
      $s->save($transaction);
      $record['id'] = $transaction->getId();
    }
    catch (Exception $e) {
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
      $this->deleteTransaction($record['id']);
      unset($record['id']);
      $this->createTransaction($record);
      return $this->applyTransaction($record['id']);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Get a transaction record.
   *
   * @return
   *   a transaction record given its id.
   */
  public function getTransaction($id) {
    return $this->getObjectRecord('BasicTransaction', $id);
  }
  /**
   * Get a transaction from an account.
   *
   * @return
   *   an account by name.
   */
  public function getTransactionFromAccount(array $record) {
    $s = new Serializer('BasicTransaction');
    $transaction = $s->loadFromRecord($record);
    $name = $transaction->getFromAccountName();
    return $this->getAccountByName($name);
  }
  /**
   * Get a transaction to an account.
   *
   * @return string
   *   an account by name.
   * @see ces_bank_transaction_form()
   */
  public function getTransactionToAccount(array $record) {
    $s = new Serializer('BasicTransaction');
    $transaction = $s->loadFromRecord($record);
    $name = $transaction->getToAccountName();
    return $this->getAccountByName($name);
  }
  /**
   * Get a transaction amount in an exchange currency.
   *
   * @return string
   *   an amount.
   * @see ces_bank_transaction_form()
   */
  public function getTransactionAmount(array $transaction, array $exchange) {
    $st = new Serializer('BasicTransaction');
    $trans = $st->loadFromRecord($transaction);
    $se = new Serializer('Exchange');
    $excha = $se->loadFromRecord($exchange);
    return $trans->getAmount($excha->getCurrency());
  }
  /**
   * Function getTransactions returns records based on conditions.
   *
   * @param array $conditions
   *   Associative array with conditions to be met by the returned transaction
   *   records. Possible values include:
   *   - account: id. Get transactions from or to the specified account id.
   *   If this parameter is set then the following two must not be set.
   *   - fromaccount: id. Get transactions where specified account id is buyer.
   *   - toaccount: id. Get transactions where the specified account id is seller.
   *   - createdsince: time. Get transactions created after this timestamp in seconds.
   *   - createduntil: time. Get transactions created before this timestamp in seconds.
   *   - limit: int. Get only n transactions.
   *
   * @return array
   *   transaction records meeting all conditions specified by $conditions parameter.
   *   The amount is expressed in a currency depending on the given conditions: if
   *   'account' is set, then it is used the account's currency. If 'fromaccount'
   *   is set but 'toaccount' is ommited, then the fromaccount's currency is used.
   *   Otherwise the currency is the seller's account one for each exchenge.
   */
  public function getTransactions(array $conditions) {
    $s = new Serializer('BasicTransaction');
    // Choose currency.
    $currencyaccount = NULL;
    $currency = NULL;
    if (isset($conditions['account'])) {
      $currencyaccount = $conditions['account'];
    }
    if (isset($conditions['fromaccount']) && !isset($conditions['toaccount'])) {
      $currencyaccount = $conditions['fromaccount'];
    }
    if ($currencyaccount != NULL) {
      $as = new Serializer('LocalAccount');
      $account = $as->load($currencyaccount);
      $currency = $account->getExchange()->getCurrency();
    }
    // Load collection.
    if (isset($conditions['account'])) {
      // Shall we improve Serializer so it can directly handle 'or' conditions?
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
      if ($currency != NULL) {
        $record['amount'] = $t->getAmount($currency);
      }
      $records[] = $record;
    }
    return $records;
  }
  /**
   * Delete transaction by ID.
   *
   * @return TRUE
   */
  public function deleteTransaction($id) {
    $t = Serializer::DBTransaction();
    try{
      $s = new Serializer('BasicTransaction');
      $transaction = $s->load($id);
      $transaction->revoke();
      if ($transaction->getState() != Transaction::STATE_REVOKED) {
        throw new Exception('Couldn\'t revoke transaction: ' . $transaction->getLog($transaction->getState()));
      }
      $s->delete($transaction);
      return TRUE;
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Creates a banking account.
   */
  public function createAccount(array &$record) {
    $t = Serializer::DBTransaction();
    try {
      // Create account record.
      unset($record['id']);
      $record['balance'] = 0;
      $account = new LocalAccount($record);
      $s = new Serializer('LocalAccount');
      $s->save($account);
      // saveExtraData() after create account record because we need the new
      // account id in order to create the AccountUser record(s).
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
   * Updates a banking account.
   */
  public function updateAccount(array &$record) {
    $t = Serializer::DBTransaction();
    unset($record['balance']);
    try{
      return $this->updateObject('LocalAccount', $record);
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Gets an account, returns object record.
   *
   * @return array
   *   an account given its id
   */
  public function getAccount($id) {
    return $this->getObjectRecord('LocalAccount', $id);
  }
  /**
   * Gets an account by its name.
   *
   * @return array 
   *   an account given its name
   */
  public function getAccountByName($name) {
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromUniqueKey('name', $name);
    if ($account === FALSE) {
      return FALSE;
    }
    return $account->getRecord();
  }
  /**
   * Gets an account's historic value.
   * @todo: Use protected function?
   */
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
  /**
   * Gets an account's historic sales.
   */
  public function getAccountHistoricSales($id, $from, $to = NULL) {
    if ($to == NULL) {
      $to = time();
    }
    return $this->getAccountHistoricValue($id, 'toaccount', $from, $to);
  }
  /**
   * Gets an account's historic purchases.
   */
  public function getAccountHistoricPurchases($id, $from, $to = NULL) {
    if ($to == NULL) {
      $to = time();
    }
    return $this->getAccountHistoricValue($id, 'fromaccount', $from, $to);
  }
  /**
   * Gets an account's historic balance at a certain time point.
   */
  public function getAccountHistoricBalance($id, $time) {
    $s = new Serializer('LocalAccount');
    $account = $s->load($id);
    $balance = $account->balance;
    $positive = $this->getAccountHistoricSales($id, $time);
    $negative = $this->getAccountHistoricPurchases($id, $time);
    $timebalance = $balance - $positive + $negative;
    return $timebalance;
  }
  /**
   * Get user accounts.
   *
   * @return array
   *   of this user accounts.
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
   * Deletes an account.
   */
  public function deleteAccount($id) {
    // @TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * Creates a limit chain.
   */
  public function createLimitChain(array &$record) {
    unset($record['id']);
    return $this->updateLimitChain($record);
  }
  /**
   * Updates a limit chain.
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
   * Return a limit chain given an id.
   * @return array
   *   a limit chain given its id.
   */
  public function getLimitChain($id) {
    return $this->getObjectRecord('LimitChain', $id);
  }
  /**
   * Deletes a limit chain.
   */
  public function deleteLimitChain($id) {
    // @TODO: implement this.
    throw new Exception('Not implemented yet');
  }
  /**
   * Gets default exchange.
   * @return array
   *   the default exchange. To be used as a model for new exchanges.
   */
  public function getDefaultExchange() {
    // @TODO: hardcode it!
    $exchange = Exchange::getDefaultExchange();
    $exchange->admin = NULL;
    $exchange->id = NULL;
    return $exchange->getRecord();
  }
  /**
   * Gets default account as template for new accounts.
   * @return array
   *   the exchange's default account. To be used as a model for new
   *   accounts for this exchange.
   */
  public function getDefaultAccount($exchangeId) {
    $exchange = $this->getObject('Exchange', $exchangeId);
    return $exchange->getDefaultAccount()->getRecord();
  }
  /**
   * Gets default limit chain as template for new limit chains.
   * @return array
   *   the exchange's default limit chain.
   */
  public function getDefaultLimitChain($exchangeId) {
    $exchange = $this->getObject('Exchange', $exchangeId);
    $account  = $exchange->getDefaultAccount();
    return $account->getLimitChain()->getRecord();
  }
  /**
   * Get all limit chains for this exchange.
   * @return array 
   *   all defined limit chains for this exchange.
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
    try {
      // Get transaction. May be any type of transaction.
      $transaction = $this->getObject('BasicTransaction', $transactionId);
      // Trigger transaction application.
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
    }
    catch (Exception $e) {
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Gets the names of classes that implement AccountLimit interface.
   * @return array 
   *   the names of classes that implement the AccountLimit interface.
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
   * Get the names of classes implementing DecoratedTransaction interface.
   * @return array
   *   the names of classes that implement the DecoratedTransaction interface.
   */
  public function getDecoratedTransactionClasses() {
    $classes = module_invoke_all('decorated_transaction_classes');
    return $classes;
  }
  /**
   * Get the permissions for this id.
   */
  public function getPermission($id) {
    return $this->getObjectRecord('Permission', $id);
  }
  /**
   * Create permission based on record.
   */
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
  /**
   * Delete permission based on ID (TODO).
   */
  public function deletePermission($id) {
    // @TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * Update permission based on record.
   */
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
  /**
   * Get the records with permissions for this user id.
   *
   * @return array
   *   The records to which the userid has permissions.
   */
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
   * Formats the amount for an exchange.
   * @return string 
   *   A string representation of $amount following $exchange preference
   *   and $format option.
   */
  public function formatAmount($amount, $exchange, $symbol = FALSE, $sign = FALSE) {
    $decimals = $exchange['currencyscale'];
    $number = number_format($amount, $decimals, '.', ' ');
    if ($symbol) {
      $number .= $exchange['currencysymbol'];
    }
    if ($sign && $amount > 0) {
      $number = '+' . $number;
    }
    return $number;
  }
  /**
   * TODO: document.
   */
  private function getObject($className, $id) {
    $s = new Serializer($className);
    return $s->load($id);
  }
  /**
   * Get record in object based on id with given classname. 
   */
  private function getObjectRecord($className, $id) {
    $object = $this->getObject($className, $id);
    if ($object === FALSE) {
      throw new Exception(t('Object of type %classname with id %id does not exist.', array('%classname' => $className, '%id' => $id)));
    }
    return $object->getRecord();
  }
  /**
   * Update object in classname with array of records.
   */
  private function updateObject($className, array &$record) {
    $s = new Serializer($className);
    $object = $s->loadFromRecord($record);
    $s->save($object);
    if (isset($object->id)) {
      $record['id'] = $object->id;
    }
    return TRUE;
  }
  /**
   * Auxiliar
   * @see newTransaction
   */
  private function transactionFormToRecord($record) {
    $record['state'] = Transaction::STATE_NEW;
    // Load accounts.
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
    if ($toaccount === FALSE) {
      throw new Exception(t('Error loading seller account.'));
    }
    if ($fromaccount === FALSE) {
      throw new Exception(t('Error loading buyer account.'));
    }
    // Set up account id's.
    $record['toaccount'] = $toaccount['id'];
    $record['fromaccount'] = $fromaccount['id'];
    if (!isset($record['data'])) {
      $record['data'] = array();
    }
    return $record;
  }
  /**
   * Creates a new transaction object based on the record array.
   * @return Transaction
   *   New transaction object based on the record array.
   */
  private function newTransaction(array &$record) {
    unset($record['id']);
    $record = $this->transactionFormToRecord($record);
    $fromaccount = $this->getObject('LocalAccount', $record['fromaccount']);
    $toaccount = $this->getObject('LocalAccount', $record['toaccount']);
    // Basic transaction. Within one exchange.
    if ($toaccount->exchange == $fromaccount->exchange) {
      $transaction = new BasicTransaction($record);
    }
    else {
    // Complex transaction interexchange.
      $transaction = InterExchangeTransaction::newInterExchangeTransaction($record);
    }
    return $transaction;
  }
  /**
   * Creates notifier from transaction.
   * @return Transaction
   * @see notifyTransactionAccount
   */
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
      ),
    );
    $this->notifyTransactionAccount('account debited', $accountFromName, $transactionUser, $params);
    $this->notifyTransactionAccount('account credited', $accountToName, $transactionUser, $params);
  }

  /**
   * Triggers notification for transaction account.
   * @see notify
   */
  private function notifyTransactionAccount($messageKey, $accountName, $transactionUser, $params) {
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromUniqueKey('name', $accountName);
    $accountUsers = $account->getUsers();
    $notifier = new Notifier();
    foreach ($accountUsers as $accountUser) {
      if ($transactionUser != $accountUser->user) { // Don't notify the transaction user.
        $notifier->notify('ces_bank', $messageKey, $accountUser->user, $transactionUser, $params);
      }
    }
  }
}

/**
 * An exchange community.
 */
class Exchange extends DBObject {
  public static $table = 'ces_exchange';
  /**
   * @var int
   */
  public $id;
  /**
   * @var string
   */
  public $code;
  /**
   * @var int
   */
  public $state;
  /**
   * @var Currency
   */
  protected $currency;
  /**
   * @var int
   */
  public $limitchain;
  /**
   * @var string
   */
  public $shortname;
  /**
   * @var string
   */
  public $name;
  /**
   * @var string
   */
  public $country;
  /**
   * @var string
   */
  public $region;
  /**
   * @var string
   */
  public $town;
  /**
   * @var string
   */
  public $website;
  /**
   * @var string
   */
  public $map;
  /**
   * Administrator user uid.
   * @var int
   */
  public $admin;
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

  const DEFAULT_EXCHANGE_ID = 1;
  const STATE_INACTIVE = 0;
  const STATE_ACTIVE = 1;
  /**
   * Loads record.
   */
  public function loadRecord(&$record) {
    parent::loadRecord($record);
    $this->currency = new Currency($record);
  }
  /**
   * Gets record.
   */
  public function getRecord() {
    $record = parent::getRecord();
    $record = array_merge($record, $this->currency->getRecord());
    unset($record['currency']);
    return $record;
  }
  /**
   * Returns a default exchange as model for new exchanges.
   *
   * @return Exchange
   *   The default exchange. To be used as a model for new
   *   exchanges. It is the exchange stored with id 
   *   Exchange::DEFAULT_EXCHANGE_ID.
   */
  public static function getDefaultExchange() {
    $record = array(
      'id' => 0,
      'code' => 'AAAA',
      'active' => 0,
      'shortname' => '',
      'name' => '',
      'country' => '',
      'region' => '',
      'town' => '',
      'website' => '',
      'map' => '',
      'admin' => 1,
      'limitchain' => 1,
      'currencysymbol' => '',
      'currencyname' => '',
      'currenciesname' => '',
      'currencyvalue' => '1',
      'currencyscale' => 2,
      'created' => REQUEST_TIME,
      'modified' => REQUEST_TIME,
      'data' => array(),
    );
    $s = new Serializer('Exchange');
    $exchange = $s->loadFromRecord($record);
    return $exchange;
  }

  /**
   * Returns a default, hidden LocalAccount as prototype for new accounts.
   *
   * @return LocalAccount
   *   Account to be used as a prototype for new accounts.
   */
  public function getDefaultAccount() {
    $record = array(
      'exchange' => $this->id,
      'name' => $this->getFreeAccountName(),
      'balance' => 0.0,
      'state' => LocalAccount::STATE_HIDDEN,
      'kind' => LocalAccount::TYPE_INDIVIDUAL,
      'limitchain' => $this->limitchain,
      'users' => array(
        array(
          'account' => NULL,
          'user' => 0,
          'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
        ),
      ),
    );
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromRecord($record);
    return $account;
  }
  /**
   * TODO: Document.
   * @see getDefaultAccount()
   */
  private function getFreeAccountName() {
    // NOTE: this function is highly inefficient. It can be changed with a
    // single specific DB query if causes performance problems.
    $s = new Serializer('LocalAccount');
    $accounts = $s->loadCollection(array('exchange' => $this->id), 'name');
    $start = floor((count($accounts) - 1) / 2); 
    // This is a bit tricky.
    $code = !empty($accounts) ? $accounts[$start]->getCode() : 0;
    if (!is_numeric($code)) {
      $code = 0;
    }
    for ($i = $start + 1; $i < count($accounts); $i++) {
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
    $code = '' . ($code + 1);
    while (drupal_strlen($code) < 4) {
      $code = '0' . $code;
    }
    return $this->code . $code;
  }
  /**
   * Activates a newly created exchange, with new structures required. 
   * Creates all the additional structures an exchange needs in order
   * to be operative. This includes:
   *  - default limit chain
   * And sets up the active flag to this exchange.
   * Exchange needs to be saved after this call so the state flag persists.
   */
  public function activate() {
    $es = new Serializer('Exchange');
    // Setup default limit chain.
    if (empty($this->limitchain)) {
      // Create default limit chain.
      $record = array(
        'exchange' => $this->id,
        'name' => 'default',
      );
      $ls = new Serializer('LimitChain');
      $defLimitChain = $ls->loadFromRecord($record);
      $ls->save($defLimitChain);
      // Update this exchange with the default limit chain.
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
   * Get virtual account for this exchange, or create if needed.
   *
   * @return Account
   *   the virtual account for this exchange. It is created if it
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
        ),
      );
      $bank = new Bank();
      $bank->createAccount($record);
      $virtAccount = new LocalAccount($record);
    }
    return $virtAccount;
  }
  /**
   * Get currency of this exchange.
   * @return Currency
   *   This exchange's currency.
   * @see Currency
   */
  public function getCurrency() {
    return $this->currency;
  }
}
/**
 * A currency. A currency belongs to a unique Exchange.
 */
class Currency extends DBObject {
  /**
   * @var string
   */
  public $currencysymbol;
  /**
   * @var string
   */
  public $currencyname;
  /**
   * @var string
   */
  public $currenciesname;
  /**
   * @var double
   */
  public $currencyvalue;
  /**
   * @var int
   */
  public $currencyscale;
  /**
   * Converts amount from a currency into a new amount. 
   * @see BasicTransaction
   * @return float
   *   the amount in this currency of the value represented by the
   *   two given parameters.
   *
   * @param $amount float the amount to be converted.
   * @param $currency Currency the original currency.
   */
  public function convert($amount, Currency $currency) {
    $rate = ($currency->currencyvalue / $this->currencyvalue);
    $newamount = $amount * $rate;
    return $newamount;
  }
}

interface Account {
  /**
   * In most implementations, gets name of account holder.
   */
  public function getName();
  /**
   * Apply transaction.
   * @see LocalAccount
   */
  public function applyTransaction(Transaction &$transaction);
  /**
   * Check transaction.
   * @see LocalAccount
   */
  public function checkTransaction(Transaction &$transaction);
}
/**
 * LocalAccount is a banking account.
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
  /**
   * LoadRecord loads loadRecord function from the parent DBObject.
   */
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
  /**
   * GetRecord loads getRecord function from the parent DBObject.
   */
  public function getRecord() {
    $record = parent::getRecord();
    $record['users'] = array();
    foreach ($this->users as $useraccount) {
      $uarecord = $useraccount->getRecord();
      $record['users'][$uarecord['user']] = $uarecord;
    }
    return $record;
  }
/**
 * Saves extra data.
 * @see createAccount
 */
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
   * Gets limit chain.
   * @return LimitChain 
   *   The limit chain associated to this account.
   */
  public function getLimitChain() {
    $s = new Serializer('LimitChain');
    return $s->load($this->limitchain);
  }
  /**
   * This function updates this account's amount with this transaction if accepted.
   *
   * If it is accepted, the updated account is safely saved to the DB.
   */
  public function applyTransaction(Transaction &$transaction) {
    $s = new LockSerializer('LocalAccount');
    // Work with updated data and lock this account in database.
    $account = $s->load($this->id);
    $amount = $transaction->getAmount();
    if ($transaction->getFromAccountName() == $this->name) {
      $account->balance = $account->balance - $amount;
    }
    if ($transaction->getToAccountName() == $this->name) {
      $account->balance = $account->balance + $amount;
    }
    $this->balance = $account->balance; 
    // Update actual object amount because it could be reused.
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
  /**
   * Changes local account state to active.
   */  
  public function activate() {
    $this->state = LocalAccount::STATE_ACTIVE;
  }
  /**
   * Gets name of account holder.
   */
  public function getName() {
    return $this->name;
  }
  /**
   * TODO: document.
   */  
  public function getCode() {
    return drupal_substr($this->name, 4);
  }
  /**
   * TODO: document.
   */
  public function &getUsers() {
    return $this->users;
  }
  /**
   * @return Exchange
   *   the exchange object to which this account belongs.
   */
  public function getExchange() {
    $es = new Serializer('Exchange');
    return $es->load($this->exchange);
  }
}
  /**
   * TODO: document. Class AccountUser.
   */
class AccountUser extends DBObject {
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
   * @var int
   */
  public $id;
  /**
   * @var int
   */
  public $exchange;
  /**
   * @var String
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
    // Load fields.
    parent::loadRecord($record);
    // Load limits.
    $this->limits = array();
    $s = new Serializer('AccountLimit');
    if (isset($record['limits'])) {
    // Record comes with limits. This is because it comes from UI.
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
    // Delete unused limits.
    // This is inefficient (it could be done in a single DB query),
    // but it doesn't matter since this is a very occasional operation.
    $limits = $s->loadCollection(array('limitchain' => $this->id));
    $allids = array();
    foreach ($limits as $limit) {
      $allids[] = $limit->id;
    }
    $oldids = array_diff($allids, $newids);
    foreach ($oldids as $id) {
      $s->delete($limits[$id]);
    }
  }
  /**
   * TODO: document.
   */
  public function checkAccount(LocalAccount &$account) {
    $messages = array();
    foreach ($this->limits as $limit) {
      if ($limit->checkAccount($account) !== TRUE) {
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
  /**
   * TODO: document.
   */
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
  /**
   * TODO: document.
   */
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
   * Todo: document.
   * @return bool
   *   The state of this account against this AccountLimit. TRUE
   *   for pass and FALSE for fail.
   */
  public function checkAccount(LocalAccount &$account) {
    return ($this->getAccountValue($account) > $this->getLimit());
  }
  /**
   * Todo: document.
   * @return int
   *   The state of this account when this transaction is performed.
   *   The account can be the seller or the buyer.
   */
  public function checkTransaction(LocalAccount &$account, Transaction &$transaction) {
    if (!$this->block) {
      return TRUE;
    }
      // Non-blocking limits always approve transactions.
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
      if ($newvalue >= $this->getLimit()) {
        // Account is ok after transaction.
        return TRUE;
      }
      elseif ($newvalue >= $value) {
        // Account is not ok after transaction but improves its mark.
        return TRUE;
      }
      else {
        // Block transaction.
        return FALSE;
      }
    }
  }
  /**
   * TODO: document.
   */
  public function getLimit() {
    return $this->value;
  }
  /**
   * TODO: document.
   * @return double
   *   The amount of this account.
   */
  public abstract function getAccountValue(LocalAccount &$account);
  /**
   * TODO: document.
   * @return double
   *   The amount of this transaction.
   */
  public abstract function getTransactionValue(Transaction &$transaction);
  /**
   * TODO: document.
   */
  public abstract function getFailMessage(LocalAccount &$account);
}
/**
 * Limits an account if its balance is less than a given value.
 */
class AbsoluteDebitLimit extends AccountLimit {
  /**
   * TODO: document.
   */
  public function getAccountValue(LocalAccount &$account) {
    return $account->balance;
  }
  /**
   * TODO: document.
   */
  public function getTransactionValue(Transaction &$transaction) {
    return $transaction->getAmount();
  }
  /**
   * TODO: document.
   */
  public function getFailMessage(LocalAccount &$account) {
    return t('Account %account exceeds the debit limit of %limit.', array('%limit' => $this->getLimit(), '%account' => $account->getName()));
  }
}
/**
 * Limits an account if its balance is more than a given value.
 */
class AbsoluteCreditLimit extends AccountLimit {
  /**
   * TODO: document.
   */
  public function getAccountValue(LocalAccount &$account) {
    return -$account->balance;
  }
  /**
   * TODO: document.
   */
  public function getTransactionValue(Transaction &$transaction) {
    return -$transaction->getAmount();
  }
  /**
   * TODO: document.
   */
  public function getLimit() {
    return -$this->value;
  }
  /**
   * TODO: document.
   */
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
   * TODO: document.
   */
  public function getId();
  /**
   * TODO: document.
   * @return the currency amount of this transaction.
   */
  public function getAmount(Currency $currency = NULL);
  /**
   * TODO: document.
   * @return int
   * The id of the user who has created this transaction.
   */
  public function getUser();
  /**
   * A short description of this transaction.
   * @return string
   *   A short description of this transaction.
   */
  public function getConcept();
  /**
   * TODO: document.
   * @return string
   *   The buyer account public ID
   */
  public function getFromAccountName();
  /**
   * TODO: document.
   * @return string
   *   The seller account public ID
   */
  public function getToAccountName();
  /**
   * TODO: document.
   * @return int
   *   The current state of this transaction.
   */
  public function getState();
  /**
   * TODO: document.
   * @return array
   *   Array of messages with details about given $state.
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
 * Concrete class for the basic transaction. It can be enhanced using DecoratorTransactions.
 */
class BasicTransaction extends DBObject implements Transaction {
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
   * TODO: document.
   */  
  public function getId(){
    return $this->id;
  }
  /**
   * TODO: document.
   *
   * @return the amount of this transaction in the given currency.
   *
   * @param $currency The currency to be used. If ommited it returns the value
   * in the seller's exchange currency.
   */
  public function getAmount(Currency $currency = NULL) {
    if ($currency != NULL) {
      // Get current (seller's) currency.
      $as = new Serializer('LocalAccount');
      $toaccount = $as->load($this->toaccount);
      return $currency->convert($this->amount, $toaccount->getExchange()->getCurrency());
    }
    else {
      return $this->amount;
    }
  }
  /**
   * TODO: document.
   */
  public function getUser() {
    return $this->user;
  }
  /**
   * TODO: document.
   */
  public function getConcept() {
    return $this->concept;
  }
  /**
   * TODO: document.
   */
  public function getFromAccountName() {
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->fromaccount);
    return $acc->name;
  }
  /**
   * TODO: document.
   */
  public function getToAccountName() {
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->toaccount);
    return $acc->name;
  }
  /**
   * TODO: document.
   */
  public function getState() {
    return $this->state;
  }
  /**
   * TODO: document.
   */
  public function log(array $messages) {
    if (empty($messages)) {
      return;
    }
    if (!isset($this->data)) {
      $this->data = array();
    }
    if (!isset($this->data['log'])) {
      $this->data['log'] = array();
    }
    if (!isset($this->data['log'][$this->state])) {
      $this->data['log'][$this->state] = array();
    }
    $this->data['log'][$this->state] = array_merge($this->data['log'][$this->state], $messages);
  }
  /**
   * TODO: document.
   */
  public function getLog($state) {
    if (isset($this->data) && isset($this->data['log']) && isset($this->data['log'][$state])) {
      return $this->data['log'][$state];
    }
    else {
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
      Transaction::STATE_REJECTED => array(Transaction::STATE_TRIGGERED, Transaction::STATE_DISCARDED),
      Transaction::STATE_COMMITED => array(Transaction::STATE_ARCHIVED, Transaction::STATE_REVOKE_TRIGGERED),
      Transaction::STATE_REVOKE_TRIGGERED => array(Transaction::STATE_REVOKE_ACCEPTED, Transaction::STATE_REVOKE_REJECTED),
      Transaction::STATE_REVOKE_ACCEPTED => array(Transaction::STATE_REVOKED, Transaction::STATE_ERROR),
      Transaction::STATE_REVOKE_REJECTED => array(Transaction::STATE_REVOKE_TRIGGERED),
      Transaction::STATE_REVOKED => array(Transaction::STATE_DISCARDED, Transaction::STATE_TRIGGERED),

    );
    $candidates = $flow[$this->state];
    if (!in_array($state, $candidates)) {
      throw new Exception(t('Invalid state transition for transaction %trans.', array('%trans' => $this->id)));
    }
    $this->state = $state;
  }
  /**
   * TODO: document.
   */
  public function trigger() {
    $this->setState(Transaction::STATE_TRIGGERED);
  }
  /**
   * TODO: document.
   */
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
  /**
   * TODO: document.
   */
  public function apply() {
    // Load account objects.
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->load($this->fromaccount);
    $accountTo = $s->load($this->toaccount);
    // Apply transaction to both accounts.
    try {
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
  /**
   * TODO: document.
   */
  public function revoke() {
    // Load account objects.
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->load($this->fromaccount);
    $accountTo = $s->load($this->toaccount);
    // Revoke transaction from both accounts
    try {
      $this->amount = -$this->amount;
      $accountTo->applyTransaction($this);
      $accountFrom->applyTransaction($this);
      $this->amount = -$this->amount;
      // Go through the three necessary workflow states.
      $this->setState(Transaction::STATE_REVOKE_TRIGGERED);
      $this->setState(Transaction::STATE_REVOKE_ACCEPTED);
      $this->setState(Transaction::STATE_REVOKED);
      return TRUE;
    }
    catch (Exception $e) {
      $this->setState(Transaction::STATE_ERROR);
      $this->log(array($e->getMessage()));
      throw $e;
    }
  }
  /**
   * TODO: document.
   */
  private static function getStateString($state) {
    $string = array(
      0 => 'NEW',
      1 => 'TRIGGERED',
      2 => 'ACCEPTED',
      3 => 'COMMITED', // Typo.
      4 => 'ARCHIVED',
      5 => 'REJECTED',
      6 => 'DISCARTED', // Typo.
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
abstract class DecoratedTransaction extends DBObject implements Transaction {
    /**
     * @var Transaction
     */
    private $parent;
    public static $table = 'ces_transaction';
  /**
   * TODO: document.
   */
    public function getId() {
      return $this->parent->getId();
    }
  /**
   * TODO: document.
   */
    public function getAmount(Currency $currency = NULL) {
      return $this->parent->getAmount($currency);
    }
  /**
   * TODO: document.
   */
    public function getUser() {
      return $this->parent->getUser();
    }
  /**
   * TODO: document.
   */
    public function getConcept() {
      return $this->parent->getConcept();
    }
  /**
   * TODO: document.
   */
    public function getFromAccountName() {
      return $this->parent->getFromAccountName();
    }
  /**
   * TODO: document.
   */
    public function getToAccountName() {
      return $this->parent->getToAccountName();
    }
  /**
   * TODO: document.
   */
    public function getParent() {
      return $this->parent;
    }
  /**
   * TODO: document.
   */
    public function getState() {
      return $this->parent->getState();
    }
  /**
   * TODO: document.
   */
    public function getLog($state) {
      return $this->parent->getLog($state);
    }
  /**
   * TODO: document.
   */
    public function log($messages) {
      return $this->parent->log($messages);
    }
  /**
   * TODO: document.
   */
    public function trigger() {
      return $this->parent->trigger();
    }
  /**
   * TODO: document.
   */
    public function check() {
      return $this->parent->check();
    }
  /**
   * TODO: document.
   */
    public function apply() {
      return $this->parent->apply();
    }
  /**
   * TODO: document.
   */
    public function revoke() {
      return $this->parent->revoke();
    }
  /**
   * TODO: document.
   */
    public function setState($state) {
      return $this->parent->setState($state);
    }
  /**
   * TODO: document.
   */
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
class InterExchangeTransaction extends DecoratedTransaction {
  /**
   * @var int The identifier of the other transaction
   */
  public $pairid;
  /**
   * @var int one of InterExchangeTransaction::$FROM or InterExchangeTransaction::$TO.
   */
  public $side;

  public static $FROM = 0;
  public static $TO = 1;
  /**
   * TODO: document.
   * @return InterExchangeTransaction
   */
  public static function newInterExchangeTransaction($record) {
    $sa = new Serializer('LocalAccount');
    $fromaccount = $sa->load($record['fromaccount']);
    $toaccount = $sa->load($record['toaccount']);
    $se = new Serializer('Exchange');
    $fromexchange = $se->load($fromaccount->exchange);
    $toexchange = $se->load($toaccount->exchange);

    $torecord = $record;
    $torecord['fromaccount'] = $toexchange->getVirtualAccount($fromexchange)->id;
    $totransaction = new InterExchangeTransaction(
            new BasicTransaction($torecord),
            array('side' => InterExchangeTransaction::$TO,
                  'pairid' => NULL,
                 )
            );
    $st = new Serializer('InterExchangeTransaction');
    $st->save($totransaction);

    $fromrecord = $record;
    $fromrecord['toaccount'] = $fromexchange->getVirtualAccount($toexchange)->id;
    $fromrecord['amount'] = $totransaction->getAmount($fromexchange->getCurrency());
    $fromtransaction = new InterExchangeTransaction(
            new BasicTransaction($fromrecord),
            array('side' => InterExchangeTransaction::$FROM,
                  'pairid' => $totransaction->getId(),
                 )
            );
    $st->save($fromtransaction);
    $totransaction->pairid = $fromtransaction->getId();
    return $totransaction;
  }
  /**
   * TODO: document.
   * @return InterExchangeTransaction
   */
  public function trigger() {
    $this->getParent()->trigger();
    $pair = $this->getPair();
    $pair->getParent()->trigger();
    $s = new Serializer('BasicTransaction');
    $s->save($pair);
    // @TODO: If the first transaction fails, don't try the second. If the second
    // fails, undo the first.
  }
  /**
   * TODO: document.
   */
  public function check() {
    // Check two virtual transactions.
    $this->getParent()->check();
    $pair = $this->getPair();
    $pair->getParent()->check();
    $s = new Serializer('BasicTransaction');
    $s->save($pair);
  }
  /**
   * TODO: document.
   */
  public function apply() {
    //Apply the two virtual transactions.
    $this->getParent()->apply();
    $pair = $this->getPair();
    $pair->getParent()->apply();
    $s = new Serializer('BasicTransaction');
    $s->save($pair);
  }
  /**
   * TODO: document. Revoke the two virtual transactions.
   */
  public function revoke() {
    // Revoke the two virtual transactions.
    $this->getParent()->revoke();
    $pair = $this->getPair();
    $pair->getParent()->revoke();
    $s = new Serializer('BasicTransaction');
    $s->save($pair);
  }
  /**
   * TODO: document.
   */
  public function deleteExtraData() {
    $pair = $this->getPair()->getParent();
    $s = new Serializer('BasicTransaction');
    $s->delete($pair);
  }
  /**
   * TODO: document.
   */
  public function getToAccountName() {
    return $this->getVirtualTransactionTo()->getToAccountName();
  }
  /**
   * TODO: document.
   */
  public function getFromAccountName() {
    return $this->getVirtualTransactionFrom()->getFromAccountName();
  }
  /**
   * TODO: document.
   */
  protected function getVirtualTransactionFrom() {
    if ($this->side == InterExchangeTransaction::$FROM) {
      $t = $this->getParent();
      return $t;
    }
    else {
      $t = $this->getPair()->getParent();
      return $t;
    }
  }
  /**
   * TODO: document.
   */
  protected function getVirtualTransactionTo() {
    if ($this->side == InterExchangeTransaction::$TO) {
      $t = $this->getParent();
      return $t;
    }
    else {
      $t = $this->getPair()->getParent();
      return $t;
    }
  }
  /**
   * TODO: document.
   */
  protected function getPair() {
    $s = new Serializer('BasicTransaction');
    $iet = $s->load($this->pairid);
    return $iet;
  }
}
/**
 * LevyedTransaction, a kind of decorated transaction.
 */
class LevyedTransaction extends DecoratedTransaction {
  /**
   * @var double percentage (over 100)
   */
  public $levy;
  /**
   * TODO: document.
   */
  public function apply() {
    $fromAccountName = $this->getFromAccountName();
    $s = new Serializer('LocalAccount');
    $fromAccount = $s->loadFromUniqueKey('name', $fromAccountName);
    $exchange = $fromAccount->getExchange();
    $levyAccount = $exchange->getLevyAccount();
    $amount = $this->getAmount() * $this->levy / 100.0;
    $levyTransaction = new BasicTransaction(array(
        'fromaccount' => $this->fromaccount,
        'toaccount' => $levyAccount->id,
        'amount' => $amount,
        'concept' => t('Levy. Transaction %trans', array('%trans', $this->getId())),
        'user' => $this->getUser(),
    ));
    // @TODO: finish this!
  }
}
/**
 * The set of permissions of a particular user.
 */
class UserPermission {
  /**
   * @var int
   */
  private $userid;
  /**
   * @var array of Permission
   */
  private static $permissions = array();
  /**
   * TODO: document.
   *
   * @param $user int
   */
  public function __construct($userid) {
    global $user;
    if ($userid === NULL) {
      $this->userid = $user->uid;
    }
    else {
      $this->userid = $userid;
    }
  }
  /**
   * Todo document this.
   *
   * @param $permission int or string
   * @param $object String
   * @param $objectid int
   *
   * @return bool
   *   whether user of this Permissions object has access of type given by
   *   the $permission parameter over the object ($object, $objectid).
   */
  public function access($permission, $object, $objectid) {
    if ($this->userid == 1) {
      return TRUE;
    }
    if (is_string($permission)) {
      $equiv = array(
        'view' => 10,
        'use' => 20,
        'edit' => 30,
        'admin' => 40,
      );
      $permission = $equiv[$permission];
    }
    // Treat here the recursive case of transaction.
    if ($object == 'transaction') {
      $ts = new Serializer('BasicTransaction');
      $as = new Serializer('LocalAccount');
      $transaction = $ts->load($objectid);
      $fromaccount = $as->loadFromUniqueKey('name', $transaction->getFromAccountName());
      $toaccount = $as->loadFromUniqueKey('name', $transaction->getToAccountName());
      return $this->access($permission, 'account buyer', $fromaccount->id)
          || $this->access($permission, 'account seller', $toaccount->id);
    }
    // Compute permissions.
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
   * Compute the user's permissions and set up internal $permissions.
   *
   * Compute the user's permissions and populate the private internal variable
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
        // Get permissions directly granted to this user.
        $userperms = $sp->loadCollection(array('scope' => 'user', 'scopeid' => $this->userid));
        $permissions = &array_merge($permissions, $userperms);
        // Get permissions granted to user's accounts. Compute exchanges.
        $sau = new Serializer('AccountUser');
        $useraccounts = $sau->loadCollection(array('user' => $this->userid));
        foreach ($useraccounts as $useraccount) {
          $accountperms = $sp->loadCollection(array('scope' => 'account', 'scopeid' => $useraccount->account));
          $permissions = &array_merge($permissions, $accountperms);

        }
        // Compute exchanges.
        $exchanges = array();
        $sla = new Serializer('LocalAccount');
        foreach ($useraccounts as $useraccount) {
          $account = $sla->load($useraccount->account);
          if (!in_array($account->exchange, $exchanges)) {
            $exchanges[] = $account->exchange;
          }
        }
        // Get permissions granted to user's accounts exchanges.
        foreach ($exchanges as $exchange) {
          $exchangeperms = $sp->loadCollection(array('scope' => 'exchange', 'scopeid' => $exchange));
          $permissions = &array_merge($permissions, $exchangeperms);
        }
        // Get global permissions.
        $globalperms = $sp->loadCollection(array('scope' => 'global'));
        $permissions = &array_merge($permissions, $globalperms);
      }
      // Set the static variable.
      self::$permissions[$this->userid] = &$permissions;
    }
    return self::$permissions[$this->userid];
  }
  /**
   * TODO document.
   * @return boolean
   *   Whether (object1,objectid1) belongs to (object2, objectid2).
   */
  private function belongs($object1, $objectid1, $object2, $objectid2) {
    // Case permission. Permission belongs to A if the object of that permission
    // belongs to A.
    if ($objectid1 == 'permission') {
      $s = new Serializer('Permission');
      $perm = $s->load($objectid1);
      return belongs($perm->object, $perm->objectid, $object2, $objectid2);
    }
    // Case same scope. Compare identifiers.
    if (substr($object1, 0, strlen($object2)) == $object2) {
      return $objectid1 == $objectid2 || (substr($object1, 0, 6) == 'global');
    }
    elseif (strpos($object2, ' ') === FALSE) {
      // All belongs to global scope.
      if ($object2 == Permission::SCOPE_GLOBAL) {
        return TRUE;
      }
      // Strip non-scope keywords from object1.
      $object1 = substr($object1, 0, strpos($object1, ' '));
      // At this point we know that $object2 is one of 'account', 'exchange' and
      // $object1 one of 'account', 'exchange', 'global'
      // and $object2 != $object1
      // so the only favorable possibility is the following.
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
  /**
   * TODO: document.
   */
class Permission extends DBObject {
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
