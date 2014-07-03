<?php
/**
 * @file
 * English debit rejected template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transaction rejected';

$body = 'Estimat/da [transaction:toaccount:user:fullname],
  
Una transacció des el teu compte ha estat <strong>rebutjada</strong> i per tant el teu compte NO serà carregat.

Detalls de la transacció:

Venedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripció: [transaction:concept]
Quantitat: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada per: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

Si aquest rebuig és un error, entra al teu compte i accepta la transacció seguint aquest enllaç: <a href="[site:url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Autoritza transacció</a>.

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
