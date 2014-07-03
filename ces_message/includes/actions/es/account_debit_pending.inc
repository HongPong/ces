<?php
/**
 * @file
 * English debit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Autorizar transacción';

$body = 'Estimado/a [transaction:toaccount:user:fullname],

S\'ha creat una transacció cap a [transaction:toaccount:name] ([transaction:toaccount:user:fullname]) que espera <strong>la teva autorització</strong> per a poder ser aplicada. 

Por favor acepta (o rechaza) esta transacción siguiendo el enlace:

      <h3><strong><a href="[site:url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Autorizar transacción</a></strong></h3>

Es muy importante que autorizes tus pagos pendientes inmediatamente por el buen funcionamiento del sistema de moneda social. También puedes añadir esta cuenta a la lista blanca de cuentas de confianza que no requieren autorización.

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
