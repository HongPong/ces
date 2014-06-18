<?php
/**
 * @file
 * English credit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transaction waiting authoriztion';

$body = 'Dear [transaction:toaccount:user:fullname],
  
A <strong>new transaction</strong> has been created ans is waiting to be <strong>manually
authorized</strong> by <strong>[transaction:fromaccount:user:fullname]</strong>.

Transaction details:

Seller: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])

Buyer: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])

Description: [transaction:concept]
Amount: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>

Ordered by: [transaction:fromaccount:user:fullname] < [transaction:fromaccount:user:mail] >
';
