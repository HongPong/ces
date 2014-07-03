<?php

/**
 * @file
 * English account credited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Your account [transaction:toaccount:name] has been credited';

$body = 'Your account [transaction:toaccount:name] has been credited.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Ordered by: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

View your current <a href="[site:url]/ces/bank/account/transaction">statement of account</a>. If this credit is incorrect click on the sale description and edit the transaction to update it.

--
[exchange:shortname] administrator:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
