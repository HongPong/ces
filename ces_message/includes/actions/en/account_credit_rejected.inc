<?php
/**
 * @file
 * English credit rejected template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transaction rejected';

$body = 'Dear [transaction:toaccount:user:fullname],
  
A transaction to your account has been <strong>rejected</strong> so your account will NOT be credited.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Ordered by: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

If this transaction should be accepted, contact the buyer [transaction:toaccount:user:fullname] <[transaction:fromaccount:user:mail]> so she/he accept the transaction or contact the exchange admnistrator if necessary.

--
[exchange:shortname] administrator:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
