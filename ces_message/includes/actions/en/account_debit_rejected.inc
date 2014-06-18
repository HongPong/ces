<?php
/**
 * @file
 * English debit rejected template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transaction rejected';

$body = 'Dear [transaction:toaccount:user:fullname],
  
A transaction from your account has been <strong>rejected</strong> so your account
will NOT be debited.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])

Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])

Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>

Ordered by: [transaction:fromaccount:user:fullname] <[transaction:fromaccount:user:mail]>

If this rejection is an error please log in to your account and accept the transaction
following this link: <a href="[base_url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Authorize transaction</a>.
';
