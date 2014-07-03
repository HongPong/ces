<?php
/**
 * @file
 * English debit pending template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] Autoritzar transacció';

$body = 'Estimat/da [transaction:toaccount:user:fullname],

S\'ha creat una transacció cap a [transaction:toaccount:name] ([transaction:toaccount:user:fullname]) que espera <strong>la teva autorització</strong> per a poder ser aplicada. 

Si us plau accepta (o rebutja) aquesta transacció seguint l\'enllaç:

      <h3><strong><a href="[site:url]/user/login?destination=ces/bank/transaction/[transaction:id]/accept">Autoritza transacció</a></strong></h3>

És molt important que autoritzis els teus pagaments pendents immediatament pel bon funcionament del sistema de moneda social. També pots afegir aquest compte a la llista blanca de comptes de confiança que no requereixen autorització.

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
