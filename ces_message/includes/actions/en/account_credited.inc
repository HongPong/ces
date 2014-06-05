<?php

/**
 * @file
 * English account credited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Your account [transaction:toaccount:name] has been credited';

$body = '[transaction:toaccount:exchange:currencyname]:Your account [transaction:toaccount:name] has been credited.

Transaction details:

Seller account: [transaction:toaccount:name]
Seller name: [transaction:toaccount:user:fullname]

Buyer account: [transaction:fromaccount:name]
Buyer name: [transaction:fromaccount:user:fullname]

Description: [transaction:concept]
Amount: [transaction:amount][transaction:fromaccount:exchange:currencysymbol]

Ordered by: [transaction:fromaccount:user:fullname] < [transaction:fromaccount:user:mail] >

If this credit is incorrect contact the seller or the exchange administrator
within 10 days to have it corrected.
';
