<?php
/**
 * @file
 * English credit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Transacción esperando autoritzación';

$body = 'Estimado/a [transaction:toaccount:user:fullname],
  
Se ha creado una <strong>nueva transacción</strong> que está esperando ser <strong>autorizada manualmente</strong> por <strong>[transaction:fromaccount:user:fullname]</strong> pera hacer-se efectiva. Quando sea autorizada se hará el traspaso entre las cuentas.

Detalles de la transacción:

Vendedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripción: [transaction:concept]
Cantidad: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada por: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
