<?php

/**
 * @file
 * English account credited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Tu cuenta [transaction:toaccount:name] ha sido abonada';

$body = 'Estimado/a [transaction:toaccount:user:fullname],

Tu cuenta [transaction:toaccount:name] ha sido abonada.

Detalles de la transacción:

Vendedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripción: [transaction:concept]
Cantidad: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada por: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

Mira tu <a href="[site:url]/ces/bank/account/transaction">Estado de la cuenta</a>. Si este abono es incorrecto, clica en la descripción de la venta y edita la transacción para solucionarlo.

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
