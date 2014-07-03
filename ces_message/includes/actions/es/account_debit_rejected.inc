<?php
/**
 * @file
 * English debit rejected template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transacción rechazada';

$body = 'Estimatdo/a [transaction:toaccount:user:fullname],
  
Una transacción desde tu cuenta ha estado <strong>rechazada</strong> y por tanto tu cuenta NO será cargada.

Detalles de la transacción:

Vendedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripción: [transaction:concept]
Cantidad: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada por: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

Si este rechazo es un error, entra a tu cuenta y acepta la transacción siguiendo este enlace: <a href="[site:url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Autorizar transacción</a>.

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
