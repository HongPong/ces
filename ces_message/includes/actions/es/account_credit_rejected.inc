<?php
/**
 * @file
 * English credit rejected template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transacció rebutjada';

$body = 'Estimat/da [transaction:toaccount:user:fullname],
  
Una transacción hacia tu cuenta ha sido <strong>rechazada</strong> y NO recibirás el importe a tu cuenta.

Detalles de la transacción:

Vendedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripción: [transaction:concept]
Cantidad: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada por: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

Si esta transacción tendría que haber sido aceptada, contacta con el/la comprador/a [transaction:toaccount:user:fullname] <[transaction:fromaccount:user:mail]> para que acepte la transacción, o contacta el/la administrador/a en caso de cualquier problema.

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
