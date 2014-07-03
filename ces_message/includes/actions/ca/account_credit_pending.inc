<?php
/**
 * @file
 * English credit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transacció esperant autorització';

$body = 'Estimat/da [transaction:toaccount:user:fullname],
  
S\'ha creat una <strong>nova transacció</strong> que està esperant ser <strong>autoritzada manualment</strong> per <strong>[transaction:fromaccount:user:fullname]</strong> per a fer-se efectiva. Quan sigui autoritzada es farà el traspàs entre els comptes.

Detalls de la transacció:

Venedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripció: [transaction:concept]
Quantitat: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada per: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
