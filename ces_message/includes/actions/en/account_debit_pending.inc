<?php
/**
 * @file
 * English debit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Accept transaction';

$body = 'A new transaction to [transaction:toaccount:name]([transaction:toaccount:user:fullname]) has been created and is awaiting 
<strong>your authorization</strong> to be applied. 

Please accept (or reject) this transaction following this link:

      <h3><strong><a href="[base_url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Authorize transaction</a></strong></h3>

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])

Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])

Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>

Ordered by: [transaction:fromaccount:user:fullname] < [transaction:fromaccount:user:mail] >

';
