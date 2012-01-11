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
  /**
   * Create exchange. This exchange is not functional yet. It must be activated.
   */
  public function createExchange(array &$record){
    $t = Serializer::DBTransaction();
    try{
      //create exchange record
      unset($record['id']);
      $record['state'] = Exchange::STATE_INACTIVE;
      $this->updateObject('Exchange', $record);
      
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates the exchange record.
   */
  public function updateExchange(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('Exchange', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Gets the exchange from its id
   */
  public function getExchange($id){
    return $this->getObjectRecord('Exchange', $id);
  }
  /**
   * Deletes an exchange
   */
  public function deleteExchange($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * @return array with all exchanges in this server.
   */
  public function getAllExchanges(){
    $s = new Serializer('Exchange');
    $exchanges = $s->loadCollection(array('active'=> 1));
    $records = array();
    $as = new Serializer('LocalAccount');
    foreach($exchanges as $exchange){
      $record = $exchange->getRecord();
      $record['accounts'] = $as->count('exchange', $exchange->id);
      $records[$exchange->id] = $record;
    }
    return $records;
  }
  /**
   * Creates transaction. The transaction is not made effective until it is 
   * applied.
   */
  public function createTransaction(array &$record){
    $t = Serializer::DBTransaction();
    try{
      unset($record['id']);
      $transaction = $this->newTransaction($record);
      $s = new Serializer(get_class($transaction));
      $s->save($transaction);
      $record['id'] = $transaction->id;
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * Updates the transaction record.
   */
  public function updateTransaction(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('BasicTransaction', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return a transaction record given its id.
   */
  public function getTransaction($id){
    return $this->getObjectRecord('BasicTransaction', $id);
  }
  
  /**
   * creates a banking account
   */
  public function createAccount(array &$record){
    $t = Serializer::DBTransaction();
    try{
      unset($record['id']);
      $account = new LocalAccount($record);
      $s = new Serializer('LocalAccount');
      $s->save($account);
      
      //saveExtraData() after create account record because we need the new 
      //account id in order to create the UserAccount record(s).
      $account->saveExtraData(); 
      
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * updates a banking account
   */
  public function updateAccount(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('LocalAccount', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array an account given its id
   */
  public function getAccount($id){
    return $this->getObjectRecord('LocalAccount', $id);
  }
  /**
   * @return array an account given its name
   * **/
  public function getAccountByName($name){
    $s = new Serializer('LocalAccount');
    $account = $s->loadFromUniqueKey('name', $name);
    if($account === FALSE) return FALSE;
    return $account->getRecord();
  }
  /**
   * @return array of this user accounts.
   */
  public function getUserAccounts($uid){
    $s = new Serializer('AccountUser');
    $useraccounts = $s->loadCollection(array('user'=> $uid));
    $accounts = array();
    foreach($useraccounts as $useracc){
      $accounts[] = $this->getAccount($useracc->account);
    }
    return $accounts;
  }
  /**
   * deletes an account.
   */
  public function deleteAccount($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * creates a limit chain.
   */
  public function createLimitChain(array &$record){
    $t = Serializer::DBTransaction();
    try{
      unset($record['id']);
      return $this->updateObject('LimitChain', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * updates a limit chain.
   */
  public function updateLimitChain(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('LimitChain', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array a limit chain given its id.
   */
  public function getLimitChain($id){
    return $this->getObjectRecord('LimitChain', $id);
  }
  /**
   * deletes a limit chain
   */
  public function deleteLimitChain($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * @return array the default exchange. To be used as a model for new exchanges.
   */
  public function getDefaultExchange(){
    $exchange = Exchange::getDefaultExchange();
    return $exchange->getRecord();
  }
  /**
   * @return array the exchange's default account. To be used as a model for new.
   * accounts for this exchange.
   */
  public function getDefaultAccount($exchangeId){
    $exchange = $this->getObject('Exchange',$exchangeId);
    return $exchange->getDefaultAccount()->getRecord();
  }
  /**
   * @return array the exchange's default limit chain.
   */
  public function getDefaultLimitChain($exchangeId){
    $exchange = $this->getObject('Exchange',$exchangeId);
    $account  = $exchange->getDefaultAccount();
    return $account->getLimitChain()->getRecord();
  }
  /**
   * @return array all defined limit chains for this exchange.
   */
  public function getAllLimitChains($exchangeId){
    $s = new Serializer('LimitChain');
    $limits = $s->loadCollection(array('exchange'=> $exchangeId));
    $records = array();
    foreach($limits as $limit){
      $records[$limit->id] = $limit->getRecord();
    }
    return $records;
  }
  /**
   * applies a transaction given its id.
   */
  public function applyTransaction($transactionId){
    $t = Serializer::DBTransaction();
    try{
      $transaction = $this->getObject('BasicTransaction', $transactionId);
      return $transaction->apply();
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  /**
   * @return array the names of classes that implement the AccountLimit interface.
   */
  public function getAccountLimitClasses(){
    $classes = module_invoke_all('account_limit_classes');
    $limits = array();
    foreach($classes as $className){
      if(class_exists($className)){
        $limits[$className] = $className::getName();
      }
    }
    return $limits;
  }
  /**
   * @return array the names of classes that implement the DecoratedTransaction interface.
   */
  public function getDecoratedTransactionClasses(){
    $classes = module_invoke_all('decorated_transaction_classes');
    return $classes;
  }
  
  
  private function getObject($className, $id){
    $s = new Serializer($className);
    return $s->load($id);
  }
  private function getObjectRecord($className, $id){
    $object = $this->getObject($className, $id);
    if($object === FALSE) 
      throw new Exception(t('Object of type %classname with id %id does not exist.', array('%classname'=>$className, '%id'=>$id)));
    return $object->getRecord();
  }
  private function updateObject($className, array &$record){
    $s = new Serializer($className);
    $object = $s->loadFromRecord($record);
    $s->save($object);
    $record['id'] = $object->id;
  }
  
  /**
   * Creates a new transacion object based on the record array.
   * @return Transaction
   */
  private function newTransaction(array &$record){
    unset($record['id']);
    //load accounts
    if(isset($record['toaccount'])){
      $toaccount = $this->getAccount($record['toaccount']);
    }else if(isset($record['toaccountname'])){
      $toaccount = $this->getAccountByName($record['toaccountname']);
    }else{
      throw new Exception(t('Seller account not set.'));
    }
    
    if(isset($record['fromaccount'])){
      $fromaccount = $this->getAccount($record['fromaccount']);
    }else if(isset($record['fromaccountname'])){
      $fromaccount = $this->getAccountByName($record['fromaccountname']);
    }else{
      throw new Exception(t('Seller account not set.'));
    }
    
    if($toaccount === FALSE) 
      throw new Exception(t('Error loading seller account.'));
    if($fromaccount === FALSE)
      throw new Exception(t('Error loading buyer account.'));
      
    
    //set up acocunt id's 
    $record['toaccount'] = $toaccount['id'];
    $record['fromaccount'] = $fromaccount['id'];
    $transaction = new BasicTransaction($record);
    
    if($toaccount['exchange'] != $fromaccount['exchange']) {
      //basic transaction
      $transaction = new InterExchangeTransaction($transaction, array());
    }
    
    return $transaction;
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
  
  public function loadRecord(&$record){
    parent::loadRecord($record);
    $this->currency = new Currency($record);
  }
  public function getRecord(){
    $record = parent::getRecord();
    $record = array_merge($record, $this->currency->getRecord());
    unset($record['currency']);
    return $record;
  }
  /**
   * @return Exchange The default exchange. To be used as a model for new 
   * exchanges. It is the exchange stored with id Exchange::DEFAULT_EXCHANGE_ID.
   */
  public static function getDefaultExchange(){
    $s = new Serializer('Exchange');
    $default = $s->load(Exchange::DEFAULT_EXCHANGE_ID);
    if($default === FALSE) 
      throw new Exception(t('Default exchange ( id = %id ) not found', array('%id'=>self::DEFAULT_EXCHANGE_ID)));
    return $default;
  }
  /**
   * @return LocalAccount to be used as a prototype for new accounts.
   */
  public function getDefaultAccount(){
    global $user;
    $record = array();
    $record['id']=NULL;
    $record['exchange'] = $this->id;
    $record['name'] = $this->getFreeAccountName();
    $record['balance'] = 0.0;
    $record['state'] = LocalAccount::STATE_HIDDEN;
    $record['kind'] = LocalAccount::TYPE_INDIVIDUAL;
    $record['limitchain'] = $this->limitchain;
    $record['create'] = time();
    $record['modified'] = time();
    $record['users'] = array(
      array(
        'account' => NULL,
        'user' => $user->uid, 
        'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR),
    );
    $account = new LocalAccount($record);
    return $account;
  }
  
  private function getFreeAccountName(){
    //NOTE: this function is highly inefficient. It can be changed with a
    //single specific DB query if causes performance problems.
    $s = new Serializer('LocalAccount');
    
    $accounts = $s->loadCollection(array($this->id => $this->id), 'name');
    
    $start = floor(count($accounts) / 2); //This is a bit tricky.
    $code = $accounts[$start]->getCode();
    
    if(!is_numeric($code)) $code = 0;
      
    for($i=$start+1; $i<count($accounts); $i++){
      $acccode = $accounts[$i]->getCode();
      if(is_numeric($acccode)){
        if($acccode - $code > 1){
          break;
        }else{
          $code = $acccode;
        }
      }
    }
    $code =  ''. ($code +1) ;
    while(strlen($code)<4) $code = '0'.$code;
    return $this->code . $code;
  }
  
  /**
   * Activates a newly created exchange. Creates all the additional structures an
   * exchange needs in order to be operative. This includes:
   *  - default limit chain
   *  - virtual account
   * And sets up the active flag to this exchange.
   */
  public function activate(){
    $es = new Serializer('Exchange');
    if(empty($this->limitchain)){
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
    $virtAccount = $this->getVirtualAccount();
    if(empty($virtAccount)){
      //create virtual account
      $virtAccount = $this->getDefaultAccount();
      $virtAccount->kind = LocalAccount::TYPE_VIRTUAL;
      $virtAccount->state = LocalAccount::STATE_HIDDEN;
      $virtAccount->name = $this->code . 'VIRT';
      $as = new Serializer('LocalAccount');
      $as->save($virtAccount);
    }
    $this->state = Exchange::STATE_ACTIVE;
    $s->updateField($this, 'state');
  }
  /**
   * @return Account the virtual account for this exchange.
   */
  public function getVirtualAccount(){
    $s = new Serializer('LocalAccount');
    return $s->loadFromUniqueKey('name', $this->code . 'VIRT');
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
   * singleton with the account's administrator user. Each entry is again an
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
  
  public function loadRecord(&$record){
    parent::loadRecord($record);
    if(isset($record['users']) && is_array($record['users'])){
      $this->users = array();
      foreach($record['users'] as $accuser){
        $this->users[] = new AccountUser($accuser);
      }
    }else{
      $s = new Serializer('AccountUser');
      $this->users = $s->loadCollection(array('account'=> $this->id));
    }
  }
  
  public function getRecord(){
    $record = parent::getRecord();
    $record['users'] = array();
    foreach($this->users as $useraccount){
      $uarecord = $useraccount->getRecord();
      $record['users'][$uarecord['user']] = $uarecord;
    }
    return $record;
  }
  
  public function saveExtraData(){
    if(!empty($this->id)){
      foreach($this->users as $accuser){
        $accuser->account = $this->id;
        $s = new Serializer('AccountUser');
        $s->save($accuser);
      }
    }
  }
  /**
   * @return LimitChain The limit chain associated to this account.
   */
  public function getLimitChain(){
    $s = new Serializer('LimitChain');
    return $s->load($this->limitchain);      
  }
  /**
   * This function updates the amount of this account with this transaction if
   * it is accepted.
   */
  public function applyTransaction(Transaction &$transaction){
    $s = new LockSerializer('LocalAccount');
    //work with updated data and lock this account in database.
    $account = $s->load($this->id);
    $amount = $transaction->getAmount();
    if($transaction->getFromAccountName() == $this->name){
      $account->balance = $account->balance - $amount;
      $this->setMessage(t('Your account has been debited by %amount.', array('%amount'=> $amount)));
    }
    if($transaction->getToAccountName() == $this->name){
      $account->balance = $account->balance + $amount;
      $this->setMessage(t('Your account has been credited by %amount.', array('%amount'=> $amount)));
    }
    $this->balance = $account->balance; //update actual object amount because it could be reused.
    
    $s->updateField($account, 'balance');
  }
   
  public function checkTransaction(Transaction &$transaction){
    return $this->getLimitChain()->checkTransaction($this, $transaction);
  }
  
  public function setMessage($message){
    //TODO
    drupal_set_message(t('Debug message for account ') . $this->name . ': ' . $message);
  }
  public function getMessages($message){
    //TODO
  }
  public function getName(){
    return $this->name;
  }
  public function getCode(){
    return substr($this->name, 4);
  }
}

class AccountUser extends DBObject{
  public static $table = 'ces_accountuser';
  
  public $user;
  public $account;
  public $role;
  
  const ROLE_ACCOUNT_ADMINISTRATOR = 0;
}
/**
 * A chain of account limits. It is used by account.
 */
class LimitChain extends DBObject{
  
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
    if(isset($record['limits'])){//record comes with limits. This is because it comes from
      foreach($record['limits'] as $limitRecord){
        $limitRecord['limitchain'] = $this->id;
        $this->limits[] = $s->loadFromRecord($limitRecord);
      }
    }else{
      $this->limits = $s->loadCollection(array('limitchain'=> $this->id));
    }
  }
  public function getRecord(){
    $record = parent::getRecord();
    $record['limits'] = array();
    foreach($this->limits as $limit){
      $record['limits'][] = $limits->getRecord();
    }
    return $record;
  }
  
  /**
   * @override
   */
  public function saveExtraData(){
    $s = new Serializer('AccountLimit');
    foreach($this->limits as $limit){
      $s->save($limit);
    }
  }
  
  public function checkAccount(LocalAccount &$account){
    $messages = array();
    foreach($this->limits as $limit){
      if($limit->checkAccount($account)!==TRUE){
        $messages[] = $limit->getFailMessage($account);
      }
    }
    if(empty($messages)){
      return TRUE;
    }else{
      return $messages;
    }
  }
  
  public function checkTransaction(LocalAccount &$account, Transaction &$transaction){
    $messages = array();
    foreach($this->limits as $limit){
      if(!$limit->checkTransaction($account, $transaction)){
        $messages[] = $limit->getFailMessage($account); 
      }
    }
    
    if(empty($messages)){
      return TRUE;
    }else{
      return $messages;
    }
  }
  
  public function getAccountLimits(){
    return $this->limits;
  }
}

/**
 * Generic class for account limits. Each account limit limites one aspect of accounts.
 */
abstract class AccountLimit extends DBObject{
  
  public static $table = 'ces_limit';
  /**
   * @var double
   */
  public $limit;
  
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
  public $limitclass;
  
  /**
   * @var array
   */
  public $data;
  
  /**
   * @return bool. The state of this account against this AccountLimit. TRUE 
   * for pass and FALSE for fail.
   */
  
  public function checkAccount(LocalAccount &$account){
    return ($this->getAccountValue($account) > $this->getLimit());
  }
  /**
   * @return int. The state of this account when this transaction is performed. 
   * The account can be the seller or the buyer.
   * **/
  public function checkTransaction(LocalAccount &$account, Transaction &$transaction){
    if(!$this->block) 
       return TRUE; //non-blocking limits always approve transactions
    else{
      $value = $this->getAccountValue($account);
      $newvalue = $value;
      $transactionValue = $this->getTransactionValue($transaction);
      if($account->getName() == $transaction->getFromAccountName()){
        $newvalue += $transactionValue;
      }else if($account->getName() == $transaction->getToAccountName()){
        $newvalue -= $transactionValue;
      }
      
      if($newvalue > $this->getLimit())//account is ok after transaction
        return TRUE;
      else if($newvalue > $value) //account is not ok after transaction but improves its mark
        return TRUE;
      else                        //block transaction
        return FALSE;
      
    }
  }
  public function getLimit(){
    return $this->limit;
  }
  /**
   * @return double. The amount of this account.
   */
  public abstract function getAccountValue(Account &$account);
  /**
   * @return double. The amount of this transaction.
   */
  public abstract function getTransactionValue(Transaction &$transaction);
  
  public abstract function getFailMessage(Account &$account);
  
  public static abstract function getName();
  
}

/**
 * Limites an account if its balance is less than a given value.
 */
class AbsoluteDebitLimit extends AccountLimit {
    
  public function getAccountValue(Account &$account){
    return $account->getAmount();
  }
  public function getTransactionValue(Transaction &$transaction){
    return $transaction->getAmount();  
  }
  public function getFailMessage(Account &$account){
    return t('Account %account exceeds the debit limit of %limit.', 
            array('%limit'=> $this->getLimit(),'%account'=> $account->getName()));
  }
  public static function getName(){
    return t('Absolute debit limit');
  }
}

/**
 * Limites an account if its balance is more than a given value.
 */
class AbsoluteCreditLimit extends AccountLimit {
    
  public function getAccountValue(Account &$account){
    return - $account->getAmount();
  }
  public function getTransactionValue(Transaction &$transaction){
    return - $transaction->getAmount();  
  }
  public function getLimit(){
    return - $this->limit;
  }
  public function getFailMessage(Account &$account){
    return t('Account %account exceeds the credit limit of %limit.', 
            array('%limit'=> $this->limit,'%account'=> $account->getName()));
  }
  public static function getName(){
    return t('Absolute credit limit');
  }
}


/**
 * Transaction interface
 * 
 * 
 * Transactions
 * ============
 * A transaction is an amount of currency to be transferred from one account to 
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
  public function getAmount();
  /**
   * @return the id of the user who has created this transaction.
   */
  public function getUser();
  /**
   * @return a short description of this transaction.
   */
  public function getConcept();
  
  /**
   * @return return the 'from' account public ID
   */
  public function getFromAccountName();
  
  /**
   * @return return the 'to' account public ID
   */
  public function getToAccountName();
  /**
   * Tries to apply this transaction. Returns the transaction state after the operation.
   */
  public function apply();
  /**
   * Tries to revoke this transaction. Returns the transaction state after the operation.
   */
  public function revoke();
  
  
}

/**
 * Concrete class for the basic transaction. It can be enchanced using DecoratorTransactions.
 */
class BasicTransaction extends DBObject implements Transaction{
    
  public $id;
  public $fromaccount;
  public $toaccount;
  public $amount;
  public $user;
  public $concept;
  public $state;
  public $created;
  public $modified;
  
  public static $table = 'ces_transaction';
    
  public function getAmount(){
    return $this->amount;
  }
  public function getUser(){
    return $this->user;
  }
  public function getConcept(){
    return $this->concept;
  }
  public function getFromAccountName(){
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->fromaccount);
    return $acc->name;
  }
  public function getToAccountName(){
    $s = new Serializer('LocalAccount');
    $acc = $s->load($this->toaccount);
    return $acc->name;
  }
  public function getState(){
    return $this->state;
  }
  public function apply(){
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->load($this->fromaccount);
    $accountTo = $s->load($this->toaccount);
    
    $messagesFrom = $accountFrom->checkTransaction($this);
    $messagesTo   = $accountTo->checkTransaction($this);
    
    if($messagesFrom === TRUE && $messagesTo === TRUE){
      $accountTo->applyTransaction($this);
      $accountFrom->applyTransaction($this);
      return TRUE;
    }else{
      $messages = array_merge($messagesFrom, $messagesTo);
      return $messages;
    }
  }
  
  public function revoke(){
    throw new Exception('Not implemented yet');
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
    
    public function getAmount(){
      return $this->parent->getAmount();
    }
    public function getUser(){
      return $this->parent->getUser();
    }
    public function getConcept(){
      return $this->parent->getConcept();
    }
    public function apply(){
      return $this->parent->apply();
    }
    public function revoke(){
      return $this->parent->revoke();
    }
    public function getFromAccountName() {
      return $this->parent->getFromAccountName();
    }
    public function getToAccountName() {
      return $this->parent->getToAccountName();
    }
    public function getParent(){
      return $this->parent;
    }
    public function getState(){
      return $this->parent->getState();
    }
    public function __construct($parent, $record){
      parent::__construct($record);
      $this->parent = $parent;
    }
    
}

class InterExchangeTransaction extends DecoratedTransaction{
  
  public function apply(){
    $fromAccountName = $parent->getFromAccountName();
    $toAccountName = $parent->toAccountName();
    
    $sa = new Serializer('LocalAccount');
    $fromAccount = $sa->loadFromUniqueKey('name', $fromAccountName);
    $toAccount = $sa->loadFromUniqueKey('name', $toAccountName);
    
    $se = new Serializer('Exchange');
    $fromExchange = $se->load($fromAccount->exchange);
    $toExchange = $se->load($toAccount->exchange);
    
    $fromVirtualAccount = $fromExchange->getVirtualAccount();
    $toVirtualAccount = $toExchange->getVirtualAccount();
    
    $virtualTransaction = new BasicTransaction(array(
        'fromaccount' => $toVirtualAccount->id,
        'toaccount'=> $fromVirtualAccount->id,
        'amount'=> $this->getAmount(),
        'concept' => t('Virtual currency exchange. Transaction %trans.', array('%trans', $this->id)),
        'user' => $this->user,
    ));
    
    //apply virtual transaction
    $fromVirtualAccount->applyTransaction($virtualTransaction);
    $toVirtualAccount->applyTransaction($virtualTransaction);
    
    if(  $virtualTransaction->getApplyState(Transaction::ACCOUNT_FLAG_FROM) == Transaction::APPLY_STATE_APPLIED 
      && $virtualTransaction->getApplyState(Transaction::ACCOUNT_FLAG_TO) == Transaction::APPLY_STATE_APPLIED){ 
      //apply transaction
      $parent->apply();
    }else{
      //TODO: report error
    }
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
  
  public function apply(){
    $fromAccountName = $this->getFromAccountName();
    $s = new Serializer('LocalAccount');
    $fromAccount = $s->loadFromUniqueKey('name',$fromAccountName);
    $s = new Serializer('Exchange');
    $exchange = $s->load($fromAccount->exchange);
    $levyAccount = $exchange->getLevyAccount();
    
    $amount = $this->getAmount()* $this->levy / 100.0;
    
    $levyTransaction = new BasicTransaction(array(
        'fromaccount' => $this->fromaccount,
        'toaccount'   => $levyAccount->id,
        'amount'      => $amount,
        'concept'     => t('Levy. Transaction %trans', array('%trans', $this->getId())),
        'user'        => $this->getUser(),
    ));
    
    //TODO: finish this!
  }
}

/**
 * @} End of "ingroup ces.bank".
 */