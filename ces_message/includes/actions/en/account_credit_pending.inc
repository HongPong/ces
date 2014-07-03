<?php
/**
 * @file
 * English credit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transaction waiting authoriztion';

$body = 'Dear [transaction:toaccount:user:fullname],
  
A <strong>new transaction</strong> has been created and is waiting to be <strong>manually authorized</strong> by <strong>[transaction:fromaccount:user:fullname]</strong>.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Ordered by: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

--
[exchange:shortname] administrator:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
