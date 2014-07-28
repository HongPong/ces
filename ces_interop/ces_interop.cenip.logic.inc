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
      return new CesInteropCENIPTransaction($record);
    }
    return parent::newTransaction($record);
  }
}

/**
 * Class for CENIP remote transactions.
 */
class CesInteropCENIPTransaction extends CesBankDecoratedTransaction {
  /**
   * @var string The remote account id.
   */
  public $remoteaccountname;
  /**
   * @var string The remote network id.
   */
  public $remotenetworkid;
  /**
   * @var string The unique transaction id.
   */
  public $remoteid;
}