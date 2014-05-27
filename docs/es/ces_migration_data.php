<?php
/**
 * @file
 * Documentation from import4ces
 */

/**
 * @defgroup ces_migration_data Datos de importación del CES
 * @ingroup ces_import4ces_doc
 * @{
 * @section content Contenido
 *  - @ref spec
 *    - @ref users_file
 *    - @ref settings_file
 *    - @ref trades_file
 *    - @ref balances_file
 *    - @ref offers_file
 *    - @ref wants_file
 *    - @ref users_migrate
 *    - @ref duplicity
 *    - @ref subarea
 *    - @ref noemail
 *    - @ref coord
 *    - @ref limit
 * 
 * @section spec Especificació dels arxius d'entrada
 * Els arxius següents són informació que tenim disponible del CES i que és
 * bastant exhaustiva. De fet, hi ha més aviat moltes més coses de les que
 * actualment tenen lloc a l'IntegralCES.
 * 
 * Compte: La codificació dels arxius és: Windows1252/WinLatin 1.
 * 
 * @subsection users_file Arxiu users.csv
 *  - UID: El número de compte corrent.
 *  - Password: El password de l'usuari.
 *  - UserType: El tipus de compte. adm per administrador, org per organització
 *     sense ànim de lucre, ind per individual, fam per compartit, com per
 *      empreses,
 *     pub per a comptes públics, vir per virtual. L'administrador té més 
 *     permisos i només n'hi ha un (que jo sàpiga), org, ind, fam i com són 
 *     iguals a la pràctica (crec). pub és més accessible en el sentit que 
 *     les transaccions d'aquest compte les pot veure tothom. vir són 
 *     comptes per comptabilitzar els intercanvis  amb altres xarxes i no 
 *     pertanyen a ningú.
 *  - FirstName: Nom
 *  - Surname: Cognom
 *  - OrgName: Per a comptes de tipus com, org, pub, el nom de l'organització
 *  - Address1: Adreça
 *  - Address2: Zona/Barri
 *  - Address3: Ciutat/Poble
 *  - Postcode: Codi postal
 *  - Subarea: El CES té el concepte de subarea per dividir la xarxa en vàries
 *     regions. Llavors es pot assignar un coordinador per regió que té 
 *     permisos sobre els comptes d'aquella regió. També es poden buscar 
 *     ofertes només referents a comptes associats a aquesta regió. Aquest 
 *     concepte (de moment) no existeix a l'integralCES.
 *
 *     DefaultSub. 0=fals, -1=cert. Si cert, aquest usuari veurà per defecte 
 *     només la informació relacionada amb la seva subàrea. Els usuaris 
 *     poden veure informació de totes les subàrees o d'una concreta
 *     utilitzant el filtre de subàrea en la pàgina principal del seu compte.
 *
 *  - PhoneH: Telèfon de casa.
 *  - PhoneW: Telèfon de la feina.
 *  - PhoneF: Fax.
 *  - PhoneM: Telèfon mòbil.
 *  - Email: mail.
 *  - IM: Instant messaging id.
 *  - WebSite: web de l'usuari
 *  - DOB: Date of birth? És blanc en les dades que tinc.
 *  - NoEmail1: No rebre llistes per mail.
 *  - NoEmail2: No rebre newsletters per mail.
 *  - NoEmail3: No rebre actualitzacions per mail.
 *  - NoEmail4: No rebre extractes per mail.
 *  - Hidden: Compte invisible.
 *  - Created: Data de creació del compte
 *  - LastAccess: Últim accés al compte
 *  - LastEdited: Última modificació al compte (no al balanç!)
 *  - EditedBy: UID del compte que l'ha editat per última vegada
 *  - InvNo: ?
 *  - OrdNo: ?
 *  - Coord: Fals si 0, cert si -1. Si cert, aquest usuari és coordinador 
 *    local, cosa que significa que té permisos per administrar els comptes i
 *    transaccions dels usuaris de la seva Subarea.
 *  - CredLimit: Límit de crèdit. La màxima quantitat de moneda que aquest 
 *    compte pot tenir. 0 per sense limit.
 *  - DebLimit: Límit de dèbit. El màxim deute que aquest compte pot tenir.
 *    0 per sense límit.
 *  - LocalOnly. 0 per fals, -1 per cert. Si cert, aquest usuari només pot 
 *    fer transaccions amb comptes de la mateixa xarxa (no pot fe canvi de 
 *    moneda).
 *  - Notes: Notes per a l'administració. Per defecte conté les dades 
 *    originals del registre, en fomat text,
 *  - Lang: idioma en tres lletres. No segueix l'estàndard ISO. eng per 
 *    anglès, cat per català, spa per castellà.
 *  - Photo: Nom de l'arxiu de la foto de perfil. Però l'arxiu en sí no és
 *    accessible.
 *  - HideAddr1: No mostrar el camp Address1 a la resta d'usuaris.
 *  - HideAddr2: No mostrar el camp Address2 a la resta d'usuaris.
 *  - HideAddr3: No mostrar el camp Address3 a la resta d'usuaris.
 *  - HideArea: ? No estic segur de si significa amagar la SubArea, però la 
 *    interfície no ho deixa escollir.
 *  - HideCode: No mostrar el camp Postcode a la resta d'usuaris
 *  - HidePhoneH: No mostrar el camp PhoneH a la resta d'usuaris.
 *  - HidePhoneW: No mostrar el camp PhoneW a la resta d'usuaris.
 *  - HidePhoneF: No mostrar el camp PhoneF a la resta d'usuaris.
 *  - HidePhoneM: No mostrar el camp PhoneM a la resta d'usuaris.
 *  - HideEmail: No mostrar el camp Email a la resta d'usuaris.
 *  - IdNo: Número de DNI.
 *  - LoginCount: Nombre de vegades que l'usuari ha fet login.
 *  - SubsDue: Data del proper pagament de subscripció, només per xarxes 
 *    que ho tinguin activat.
 *  - Closed: Si aquest compte està tancat (no s'esborra per mantenir 
 *    l'enllaç de la taula de transaccions).
 *  - DateClosed: Si el compte és ancat, data en que que es va tancar el 
 *    compte.
 *  - Translate. Si el compte té permisos per a traduir la interfície al 
 *    seu idioma.
 *  - Locked. si el compte està bloquejat i per tant no pot efectuar 
 *    transaccions.
 *  - Buddy: Número de compte de l'amic. L'amic és el compte que va 
 *    portar aquest nou compte a la xarxa de moneda social. Hi ha un mecanisme 
 *    opcional que fa que l'amic rebi un petit % de cada cobrament d'aquest 
 *    compte.
 * 
 * @subsection settings_file Arxiu settings.csv
 *  - ExchangeID: Identificadr de 4 lletres de la xarxa.
 *  - ExchangeTitle: Nom llarg de la xarxa.
 *  - ExchangeName: Nom curt de la xarxa.
 *  - ExchangeType: tb per bancs del temps, no sé com deu ser per a les xarxes
 *    basades en moneda de curs legal.
 *  - ExchangeDescr: Descripció llarga de la xarxa.
 *  - Password: Password d'administració de la xarxa.
 *  - Town: Regió de la xarxa.
 *  - Logo: Nom de l'arxiu del logo de la xarxa. L'arxiu en sí és 
 *    inaccessible.
 *  - Administrator: Nom de l'administrador.
 *  - Addr1: Línia 1 de l'adreça de l'administrador
 *  - Addr2: Línia 2 de l'adreça de l'administrador
 *  - Addr3: Línia 3 de l'adreça de l'administrador
 *  - Postcode: Codi postal de l'administrador.
 *  - Province: Comunitat autònoma o província.
 *  - CountryCode: ES per Espanya.
 *  - CountryName: Spain per Espanya.
 *  - Tel1: Telèfon de casa de l'administrador.
 *  - Tel2: Telèfon de la feina de l'administrador.
 *  - Fax: Fax de l'administrador.
 *  - TelCode: Codi del telèfon segons el país.
 *  - Email: email d'administració.
 *  - InternetMessaging: Id de missatgeria instantània.
 *  - AdminTel: Telèfon de l'administrador.
 *  - AdminEmail: Mail genèric d'administració de la xarxa que proporciona 
 *    el CES i que està redireccionat al mail del camp Email, i que és el
 *    remitent dels mails automàtics enviats pel sistema.
 *  - MemSec: Nom del coordinador de membres. El coordinador de membres és 
 *    una interfície d'administració amb menys privilegis (no pot veure 
 *    transaccions) però pot crear usuaris, actualitzar-los i també 
 *    adminstrar les ofertes i demandes.
 *  - MemSecEmail: Mail del coordinador de membres.
 *  - MemSecEmailAlt: 2n Mail del coordinador de membres.
 *  - MemSecPsw: Password del coordinador de membres.
 *  - MemSecTel: Telèfon del coordinador de membres.
 *  - LevyRate: Percentatge de impost sobre les transaccions que s'aplica en
 *    aquesta xarxa.
 *  - CurName: Nom singular de la moneda.
 *  - CurNamePlural: Nom plural de la moneda.
 *  - CurLet: Símbol de la moneda, HTML-escpat.
 *  - ConCurName: Nom de la moneda de referència.
 *  - ConCurLet: Símbol de la moneda de referència.
 *  - MapAddress: Adreça de google maps.
 *  - WebAddress: Adreça web de la xarxa (blog o el que sigui).
 *  - ReDir: Pàgina on es redireccionen els usuaris en fer logout.
 *  - Hidden:Si la xarxa és amagada. 0=fals, -1=cert.
 *  - Active: Si la xarxa és activa.
 *  - TimeBased: Si la moneda de la xarxa està basada en el temps.
 *  - TimeUnit: La unitat de temps equivalent a la moneda.
 *  - DateAdded: Data de creació de la xarxa.
 *  - DateModified: Data de modificació dels paràmetres de la xarxa.
 *  - CredLim: CreditLimit per defecte.
 *  - DebLim: DeitLimit per defecte.
 *  - TimeDiff: Diferència en hores respecte la hora GMT0.
 *  - DaylightSavingOn: Data en la que s'entra en l'horari d'estiu.
 *  - DaylightSavingOff: Data en la que s'entra en l'horari d'hivern.
 *  - Language: Idioma per defecte dels usuaris.
 *  - DefaultExchanges: Separades per salts de línia, els codis de 4 lletres
 *    de les xarxes “amigues” que apareixen per defecte al buscar ofertes 
 *    (no sé si altres coses) després de les ofertes de la xarxa local.
 *  - Cell: Telèfon mobil de l'administrador.
 *  - SubscriptionExchange: 0=fals, -1=cert. Cert per aquelles xarxes que 
 *    requereixen que els seus membres paguin subscripcions. Afegeix un 
 *    camp (SubsDue) en el registre de l'usuari que diu la data del proper
 *    pagament de subscripció.
 *  - WelcomeLetter: Missatge de benvinguda. Conté els camps dinàmics 
 *    {New member's name}, {Account #}, {Password}.
 *  - InviteLetter: Missatge d'invitació.
 *  - InviteLetterHead: Fragment del títol del missatge d'invitació que va 
 *    entremig de: [Nom d'usuari] {fragment} [Nom de la xarxa]
 *  - DoMoney: ? Crec que és per activar la funcionalitat de euros virtuals.
 *    Amb aquesta funcionalitat cada compte té un balanç en moneda social i 
 *    un balanç en euros virtuals. Els euros virtuals són euros dipositats 
 *    en una caixa comuna i que es transfereixen virtualment d'un usuari a 
 *    l'altre.
 *  - ConRedeemRate: ?
 *  - HidePsw: ? No sé quin password se suposa que amaga.
 *  - NoDetails: 0=fals, -1=cert. En cert, amaga es balanços i el nombre i
 *    quntitat de les transaccions dels usuaris.
 *  - BudRate: Percentatge del total de la transacció que va cap al compte
 *    “amic”. 
 *
 * @subsection trades_file Arxiu trades.csv
 *
 *  - ID: Número creixent que comena per 1.
 *  - Seller: UID del compte que cobra.
 *  - Buyer: UID del compte que paga.
 *  - RemoteExchange: Si la transacció és entre xarxes, codi de 4 lletres 
 *    de la xarxa externa.
 *  - RemoteBuyer: Si la transacció es entre xarxes, UID del compte de la 
 *    xarxa externa. En aquest cas, Seller o Buyer és un compte virtual 
 *    XXXXVIRT.
 *  - RecordID: Codi que identifica la transacció. Sembla el compte que 
 *    ha efectuat la transacció concatenat amb la data de la transacció.
 *  - DateEntered: Data que s'ha entrat la transacció.
 *  - EnteredBy: UID del compte que ha efectuat la transacció. Normalment
 *    el venedor, però també pot ser un administrador.
 *  - Amount: Quantitat.
 *  - Levy: Quantitat d'impost
 *  - LevyRate: Percentatge de l'impost.
 *  - Description: Descripció de la transacció.
 *
 * @subsection balances_file Arxiu balances.csv
 *
 * Aquest arxiu sembla d'una taula només per a cache, ja que les dades es
 * poden calcular des de la taula de les transaccions.
 *
 *  - UID: UID del compte.
 *  - Sales: Nombre de ventes
 *  - Income: Quantitat cobrada.
 *  - Purchases: Nombre de compres.
 *  - Expenditure: Quantitat pagada.
 *  - Levy: Impost pagat.
 *  - Balance: Balanç.
 * 
 * @subsection offers_file Arxiu offers.csv
 *  - ID: Número incremental.
 *  - UID: UID de l'usuari propietari de l'oferta.
 *  - Remote: Per a ofertes posades per usuaris que no pertanyen a aquesta 
 *    xarxa, un codi que conté el UID del compte que l'ha posat i (crec) 
 *    que la data d'expiració i algo més. En aquest cas UID és l'usuari 
 *    virtual.
 *  - Category: Nom de la categoria de l'oferta.
 *  - Subcat: ? Nom de la subcategoria de l'oferta.
 *  - Title: Títol de l'oferta.
 *  - Description: Descripció de l'oferta en HTML.
 *  - Image: Nom de la imatge de la oferta. La imatge és inaccessible.
 *  - Keys: paraules clau.
 *  - Rate: Text, preu de la oferta (en la moneda de la xarxa).
 *  - ConRate: Text, preu de a oferta (en euros  moneda legal)
 *  - DateAdded: Data de creació.
 *  - DateExpires: Data d'expiració de l'oferta.
 *  - Hidden: Si l'oferta està amagada.
 * @subsection wants_file Arxiu wants.csv
 *  - ID: Enter incremental.
 *  - UID: UID del compte propietari de la demanda
 *  - Keep: Quant de temps ha de durar la oferta.
 *  - DateAdded: Data de creació de la demanda.
 *  - Title: Títol de la demanda.
 *  - Description: Descripció de la demanda en HTML.
 * 
 * @section users_migrate Migració d'usuaris
 * @subsection duplicity Duplicitat
 * Els usuaris i els comptes corrents a CES són el mateix concepte, però 
 * son conceptes separats a IntegralCES. Per aquest mateix motiu, hi ha 
 * diferents comptes al CES amb idèntic email, cosa que no és admesa en
 * Drupal estàndard.
 *
 * @todo Decidir què fem.
 *
 * @subsection subarea Subarea
 *
 * Aquest és un concepte que no està implementat en el CES.
 *
 * @todo Decidir què fem.
 *
 * @subsection noemail NoEmail
 *
 * Aquest és un concepte que no està implementat en el CES, perquè en 
 * particular no està implementada la funcionalitat de enviar mails
 * massius des de l'administració de la xarxa.
 *
 * Observació: Aquesta funcionalitat però ha demostrat ser interessant
 * i potser podem valorar d'afegir-la. En qualsevol cas jo no em 
 * preocuparia massa d'aquest camp.
 *
 * @subsection coord Coord
 *
 * Tampoc aquest concepte existeix al CES.
 *
 * Observació: La naturalesa del sistema de permisos fa que sigui fàcil
 * d'implementar. Tot i això jo no ho faria i ho deixaria en tot cas per 
 * més endavant sempre que puguem tenir varis comptes administradors.
 *
 * @subsection limit Limit
 *
 * En el CES els límits s'escriuen directament al compte de l'usuari. En
 * l'IntegralCES el sistema és més sofisticat: per a cada xarxa, hi ha 
 * diferents tipologies de límits (ex: “individuals”, “companyies”, 
 * “associació X”,...), i un compte està associat a un tipus de límit.
 * Aquests tius de límits són els que internament anomenem limitchains,
 * doncs a nivell d'implementació estan formats per una cadena de límits
 * atòmics del tipus “No estar per sot de X” o 
 * “No estar per sobre de X”.
 *
 * El que cal fer és crear una instància de limitchain per a cada límit
 * diferent que hi hagi, però no una per a cada compte! És d'esperar 
 * que tots els comptes excepte uns quants tinguin els mateixos valors.
 * @}
 */
