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
   * Exchange object accessors
   */
  public function createExchange(array &$record){
    $t = Serializer::DBTransaction();
    try{
      //create exchange record
      unset($record['id']);
      $this->updateObject('Exchange', $record);
      //create default limit chain for this new exchange.
      $deflimit = $this->getDefaultLimitChain(Exchange::DEFAULT_EXCHANGE_ID);
      unset($deflimit['id']);
      $deflimit['exchange'] = $record['id'];
      $this->createLimitChain($deflimit);
      //update exchange record with the new limitchain
      $record['limitchain'] = $deflimit['id'];
      $this->updateObject('Exchange', $record); 
      
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  public function updateExchange(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('Exchange', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  public function getExchange($id){
    return $this->getObjectRecord('Exchange', $id);
  }
  public function deleteExchange($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  public function getAllExchanges(){
    $s = new Serializer('Exchange');
    $exchanges = $s->loadCollection('active', 1);
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
   * Transaction object accessors
   */
  public function createTransaction(array &$record){
    $t = Serializer::DBTransaction();
    try{
      unset($record['id']);
      return $this->updateObject('BasicTransaction', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
    
  }
  public function updateTransaction(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('BasicTransaction', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  public function getTransaction($id){
    return $this->getObjectRecord('BasicTransaction', $id);
  }
  
  /**
   * Account object accessors
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
  public function updateAccount(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('LocalAccount', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  
  public function getAccount($id){
    return $this->getObjectRecord('LocalAccount', $id);
  }
  /**
   * @return array the account record or FALSE if account does not exist.
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
    $useraccounts = $s->loadCollection('user', $uid);
    $accounts = array();
    foreach($useraccounts as $useracc){
      $accounts[] = $this->getAccount($useracc->account);
    }
    return $accounts;
  }
  
  public function deleteAccount($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  /**
   * LimitChain object accessors
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
  public function updateLimitChain(array &$record){
    $t = Serializer::DBTransaction();
    try{
      return $this->updateObject('LimitChain', $record);
    }catch(Exception $e){
      $t->rollback();
      throw $e;
    }
  }
  public function getLimitChain($id){
    return $this->getObjectRecord('LimitChain', $id);
  }
  public function deleteLimitChain($id){
    //TODO: implement this
    throw new Exception('Not implemented yet');
  }
  
  
  public function getDefaultExchange(){
    $exchange = Exchange::getDefaultExchange();
    return $exchange->getRecord();
  }
  
  public function getDefaultAccount($exchangeId){
    $exchange = $this->getObject('Exchange',$exchangeId);
    return $exchange->getDefaultAccount()->getRecord();
  }
  
  public function getDefaultLimitChain($exchangeId){
    $exchange = $this->getObject('Exchange',$exchangeId);
    $account  = $exchange->getDefaultAccount();
    return $account->getLimitChain()->getRecord();
  }
  
  public function getAllLimitChains($exchangeId){
    $s = new Serializer('LimitChain');
    $limits = $s->loadCollection('exchange', $exchangeId);
    $records = array();
    foreach($limits as $limit){
      $records[$limit->id] = $limit->getRecord();
    }
    return $records;
  }
  
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
  private function updateObject($className, &$record){
    $s = new Serializer($className);
    $object = $s->loadFromRecord($record);
    $s->save($object);
    $record['id'] = $object->id;
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
  
  /**
  * Builds a new Exchange from an object following the structure of the table 'ces_exchange'.
  * This is used both from Exchange::load() function with actual data comming from the DB 
  * and from the visual layer, getting the data from a form.
  * All fields must be set. If the exchange in not stored yet in the DB the field id must be 
  * set to NULL.
  */
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
    //specific DB query if causes performance problems.
    $s = new Serializer('LocalAccount');
    
    $accounts = $s->loadCollection($this->id, $this->id, 'name');
    
    if(empty($accounts)){
      return $this->code . '0000';
    }
    $start = floor(count($accounts) / 2); //This is a bit tricky.
    $code = $accounts[$start]->getCode();
    
    for($i=$start+1; $i<count($accounts); $i++){
      $acccode = $accounts[$i]->getCode();
      if($acccode - $code > 1){
        break;
      }else{
        $code = $acccode;
      }
    }
    return $this->code . ($code +1) ;
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
  
  public function loadRecord(&$record){
    parent::loadRecord($record);
    if(isset($record['users']) && is_array($record['users'])){
      $this->users = array();
      foreach($record['users'] as $accuser){
        $this->users[] = new AccountUser($accuser);
      }
    }else{
      $s = new Serializer('AccountUser');
      $this->users = $s->loadCollection('account', $this->id);
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
    
    if($transaction->getFromAccountName() == $this->name){
      $account->amount = $account->amount - $transaction->getAmount();
      $this->setMessage(t('Your account has been debited by %amount.', array('%amount', $transaction->getAmount())));
    }
    if($transaction->getToAccountName() == $this->name){
      $account->amount = $account->amount + $transaction->getAmount();
      $this->setMessage(t('Your account has been credited by %amount.', array('%amount', $transaction->getAmount())));
    }
    $this->amount = $account->amount; //update actual object amount because it can be reused.
    
    $s->save($this->id);
  }
   
  public function checkTransaction(Transaction &$transaction){
    return $this->getLimitChain()->checkTransaction();
  }
  
  public function setMessage($message){
    //TODO
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
      $this->limits = $s->loadCollection('limitchain', $this->id);
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
    return $this->fromaccount;
  }
  public function getToAccountName(){
    return $this->toaccount;
  }
  public function apply(){
    $s = new Serializer('LocalAccount');
    $accountFrom = $s->loadFromUniqueKey('name',$this->fromaccount);
    $accountTo   = $s->loadFromUniqueKey('name',$this->toaccount);
    
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
    
    
  }
}

/**
 * @} End of "ingroup ces.bank".
 */