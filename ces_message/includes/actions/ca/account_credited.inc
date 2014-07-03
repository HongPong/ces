<?php

/**
 * @file
 * English account credited template.
 *
 * @ingroup ces_message_template
 */

$subject = '[[exchange:shortname]] El teu compte [transaction:toaccount:name] ha estat abonat';

$body = 'Estimat/da [transaction:toaccount:user:fullname],

El teu compte [transaction:toaccount:name] ha estat abonat.

Detalls de la transacció:

Venedor/a: <strong>[transaction:toaccount:name]</strong> ([transaction:toaccount:user:fullname])
Comprador/a: <strong>[transaction:fromaccount:name]</strong> ([transaction:fromaccount:user:fullname])
Descripció: [transaction:concept]
Quantitat: <strong>[transaction:amount][transaction:fromaccount:exchange:currencysymbol]</strong>
Entrada per: [transaction:orderedby:fullname] < [transaction:orderedby:mail] >

Mira el teu <a href="[site:url]/ces/bank/account/transaction">Estat del compte</a>. Si aquest abonament és incorrecte, clica a la descripció de la venda i edita la transacció per solucionar-ho.

--
Administrador/a de [exchange:shortname]:
[exchange:admin:fullname]
[exchange:admin:mail]
[exchange:admin:mainphone]
';
