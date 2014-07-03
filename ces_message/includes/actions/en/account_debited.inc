<?php

/**
 * @file
 * English account debited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Your account [transaction:fromaccount:name] has been debited';

$body = 'Your account [transaction:fromaccount:name] has been debited.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Ordered by: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

View your current <a href="[site:url]/ces/bank/account/transaction">statement of account</a>. If this debit is incorrect contact the seller or the exchange administrator immediately to have it corrected.

Seller contact: [transaction:toaccount:user:fullname]
mail: [transaction:toaccount:user:mail]
phone: [transaction:toaccount:user:mainphone]

--
[exchange:shortname] administrator:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
