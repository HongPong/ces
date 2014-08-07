<?php
/**
 * @file
 * Classes for Community Exchange Network Intertrading Protocol.
 */
class CesInteropCENIPBank extends CesBank{
  /**
   * Creates a transaction object.
   * 
   * If the record specifies this is a CENIP transaction, it returns a new
   * CesInteropCENIPTransaction object, otherwise it calls the parent and
   * returns a regular transaction.
   * 
   * @return CesBankTransactionInterface
   */
  protected function newTransaction(array &$record) {
    if (!empty($record['cenip'])) {
      if (isset($record['toaccount'])) {
        $toaccount = $this->getObject('CesBankLocalAccount', $record['toaccount']);
      }
      else {
        $s = new IcesSerializer('CesBankLocalAccount');
        $toaccount = $s->loadFromUniqueKey('name', $record['toaccountname']);
      }
      $toexchange = $toaccount->getExchange();
      $remoteexchange = new CesInteropCENIPRemoteExchange($record['nid']);
      $basic = array(
        'fromaccount' => $toexchange->getVirtualAccount($remoteexchange)->id,
        'toaccount' => $toaccount->id,
        'amount' => $record['amount'],
        'concept' => $record['concept'],
        'data' => array(),
      );
      $cenip = array(
        'nid' => $record['nid'],
        'buyer_id' => $record['buyer_id'],
      );
      return new CesInteropCENIPTransaction(new CesBankBasicTransaction($basic), $cenip);
    }
    else {
      return parent::newTransaction($record);
    }
  }
}
/**
 * Represents a CENIP remote exchange. It is a stub in fact.
 */
class CesInteropCENIPRemoteExchange extends CesBankExchange {
  
  /**
   * Build new instance from the name.
   */
  public function __construct($code) {
    $this->code = $code;
  }
}
/**
 * Class for CENIP remote transactions.
 */
class CesInteropCENIPTransaction extends CesBankDecoratedTransaction {
  /**
   * @var string The remote network id.
   */
  public $nid;
  /**
   * @var string The remote account id.
   */
  public $buyer_id;
  /**
   * @var string The remote account user name.
   */
  public $buyer_name;
  /**
   * @var string The remote account user mail.
   */
  public $buyer_email;
  /**
   * @var string The name of the remote exchange.
   */
  public $buyer_xname;
  /**
   * @var string The country code of the remote exchange.
   */
  public $buyer_country;
  /**
   * @var string The unique transaction id.
   */
  public $txid;
  
  /**
   * Implements apply().
   */
  public function apply() {
    $as = new IcesSerializer('CesBankLocalAccount');
    $toaccount = $as->loadFromUniqueKey('name', $this->getToAccountName());
    $es = new IcesSerializer('CesBankExchange');
    $toexchange = $es->load($toaccount->exchange);
    $toaccusers = $toaccount->getUsers();
    $toaccuser = $toaccusers[0];
    $touser = user_load($toaccuser->user);
    
    $data = array(
      'password' => $toexchange->data['cenip_pass'],
      'buyer_nid' => $this->nid,
      'buyer_id' => $this->buyer_id,
      'seller_nid' => $toexchange->data['cenip_user'],
      'seller_id' => $toaccount->name,
      'seller_xname' => $toexchange->name,
      'seller_name' => ces_user_get_name($touser),
      'seller_email' => $touser->mail,
      'seller_country' => $toexchange->country,
      'description' => $this->getConcept(),
      'amount' => $this->getAmount(),
    );
    $url = variable_get(CES_INTEROP_CEN_CLEARING_CENTRAL_URL, CES_INTEROP_CEN_CLEARING_CENTRAL_URL_DEFAULT);
    $url .= '/' . CES_INTEROP_CEN_CLEARING_CENTRAL_TXINPUT;
    $response = drupal_http_request($url, array(
      'data' => drupal_http_build_query($data),
      'method' => 'POST',
    ));
    if (!empty($response->data)) {
      // The response is url-encoded.
      $response = drupal_get_query_array($response->data);
      // In the response attribute is the transaction status or error code.
      switch ($response['response']) {
        // Tansaction successfully applied.
        case 1:
          // Save return values for better review.
          if (isset($response['txid'])) {
            $this->txid = $response['txid'];
          }
          if (isset($response['buyer_name'])) {
            $this->buyer_name = $response['buyer_name'];
          }
          if (isset($response['buyer_email'])) {
            $this->buyer_email = $response['buyer_email'];
          }
          if (isset($response['buyer_xname'])) {
            $this->buyer_xname = $response['buyer_xname'];
          }
          if (isset($response['buyer_country'])) {
            $this->buyer_country = $response['buyer_country'];
          }
          // Update amount with the remote answer just in case there is any
          // change (in rounding for example).
          if (isset($response['amount'])) {
            $this->getParent()->amount = $response['amount'];
          }
          // Apply the local transaction between the virtual account and the 
          // local account.
          parent::apply();
          break;
        
        // Error cases:
        case 2:
          $message = t('Buyer account does not exist.');
          break;
        
        case 3:
          $message = t('Network ID does not exist.');
          break;
        
        case 4:
          $message = t('Transaction denied by remote exchange.');
          break;
        
        case 5:
          $message = t('Faulty data.');
          break;
        
        case 6:
          $message = t('The transaction has repeated ID.');
          break;
        
        case 7:
          $message = t('URL error.');
          break;
        
        case 8:
          $message = t('Remote conversion rate not set.');
          break;
        
        case 9:
          $message = t('Remote server error.');
          break;
        
        case 10:
          $message = t('Password incorrect.');
          break;
        
        case 11:
          $message = t('Invalid IP of incomming server.');
          break;
        
        case 12:
          $message = t('No Transaction ID provided.');
          break;
        
        case 13:
          $message = t('Transaction ID does not exist.');
          break;
        
        case 14:
          $message = t('Unable to connect to remote server.');
          break;
        
        default:
          $message = t('Unknown error in remote transaction.'); 
      }
      if ($response['response'] != 1) {
        $this->setState(CesBankTransactionInterface::STATE_ERROR);
        $this->log(array($message));
      }
    }
    else {
      $this->setState(CesBankTransactionInterface::STATE_ERROR);
      $this->log(array(t('Network error in remote transaction.')));
    }
  }
  /**
   * Implements revoke().
   */
  public function revoke() {
    //TODO: remotely revoke.
    parent::revoke();
  }
}