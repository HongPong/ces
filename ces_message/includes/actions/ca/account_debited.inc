<?php

/**
 * @file
 * English account debited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Your account [transaction:fromaccount:name] has been debited';

$body = '[transaction:fromaccount:exchange:shortname]: Your account [transaction:fromaccount:name]
has been debited.

Transaction details:

Seller account: [transaction:toaccount:name]
Seller name: [transaction:toaccount:user:fullname]

Buyer account: [transaction:fromaccount:name]
Buyer name: [transaction:fromaccount:user:fullname]

Description: [transaction:concept]
Amount: [transaction:amount][transaction:fromaccount:exchange:currencysymbol]

Ordered by: [current-user:fullname] <[current-user:mail]>

View your current <a href="[site:url]/ces/bank/account/transaction">statement of account</a>.

If this debit is incorrect contact the seller or the exchange administrator
within 10 days to have it corrected.

Seller contact: [transaction:toaccount:user:fullname]
mail: [transaction:toaccount:user:mail]
phone: [transaction:toaccount:user:mainphone]
';
