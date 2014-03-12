<?php

/**
 * @defgroup ces_dev_install Entorno de desarrollo
 * @{
 * A continuación se explica detalladamente como establecer el entorno adecuado
 * para desarrollar el proyecto IntegralCES. Siguiendo estas instrucciones 
 * instalaremos la utilidad drush con la que a su vez instalaremos el Drupal con
 * el módulo IntegralCES y su tema con el control de versiones git.
 * 
 * Contenido
 *  - @ref install_drush
 *  - @ref install_drupal
 *  - @ref install_ces
 *  - @ref install_greences
 *  - @ref install_activate_modules
 *
 * @section install_drush Instalando drush
 * Seguimos la guia de DevelClub
 * http://www.develclub.es/drupal/tutorial-de-instalacion-de-drupal-7-en-espanol-usando-drush.html
 * Instalamos drush:
 * @code
 * sudo pear install drush/drush 
 * @endcode
 * Chequear estado:
 * @code
 * drush status
 * @endcode
 * Error con librería:
 * @code 
 * Drush needs to download a library from                               [error]
 * http://download.pear.php.net/package/Console_Table-1.1.3.tgz in order
 * to function, and the attempt to download this file automatically
 * failed because you do not have permission to write to the library
 * directory /usr/share/php/drush/lib. To continue you will need to
 * manually download the package from
 * http://download.pear.php.net/package/Console_Table-1.1.3.tgz, extract
 * it, and copy the directory into your /usr/share/php/drush/lib
 * directory.
 * @endcode
 * Como nos indica lo resolvemos
 * Bajamos librería:
 * @code
 * wget http://download.pear.php.net/package/Console_Table-1.1.3.tgz
 * @endcode
 * Descomprimimos:
 * @code
 * file-roller Console_Table-1.1.3.tgz 
 * @endcode
 * movemos a directorio de librerías:
 * @code
 * sudo cp -R Console_Table-1.1.3 /usr/share/php/drush/lib/
 * @endcode
 * Limpiamos:
 * @code
 * rm -fr Console_Table-1.1.3*
 * rm package.xml
 * @endcode
 * Chequeamos estado:
 * @code
 * $drush status
 * 
 * PHP executable        :  /usr/bin/php          
 * PHP configuration     :  /etc/php5/cli/php.ini 
 * PHP OS                :  Linux                 
 * Drush version         :  6.2.0                 
 * Drush configuration   :                        
 * Drush alias files     :                
 * @endcode
 * @section install_drupal Instalando drupal con drush
 * Nos colocamos en la carpeta del servidor y ejecutamos:
 * @code
 * $drush dl drupal --drupal-project-rename=drupaltest
 * Project drupal (7.26) downloaded to /home/eduardo/SERVIDOR/drupaldev/drupaldev.                 [success]
 * Project drupal contains:                                                                        [success]
 *  - 3 profiles: minimal, standard, testing
 *  - 4 themes: stark, garland, bartik, seven
 *  - 47 modules: tracker, simpletest, update, book, image, contextual, openid, forum, shortcut, aggregator, system, text, list, field_sql_storage, number,
 * options, field, color, filter, toolbar, rdf, node, comment, contact, menu, overlay, locale, blog, syslog, file, block, dashboard, path, statistics, taxonomy,
 * dblog, php, translation, user, profile, help, poll, search, trigger, field_ui, drupal_system_listing_compatible_test, drupal_system_listing_incompatible_test
 * @endcode
 * En 10 segundos tenemos la instalación realizada.
 * 
 * Bajamos traducciones:
 * @code
 * $wget http://ftp.drupal.org/files/translations/7.x/drupal/drupal-7.0-alpha1.ca.po
 * $wget http://ftp.drupal.org/files/translations/7.x/drupal/drupal-7.0-alpha1.es.po
 * @endcode
 * Movemos las traducciones a su directorio:
 * @code
 * $mv  *.po profiles/standard/translations/
 * @endcode
 * Hacemos la instalación:
 * @code
 * drush site-install standard --account-name=integraledu --account-pass=TU_PASSWORD --db-url=mysql://root:TU_PASSWORD_MYSQL\@localhost/drupaldev --locale=ca
 * @endcode
 * El solo crea la base de datos y todo lo necesario.
 * Solo nos faltará dar permisos de escritura en sites/default/files/
 * 
 * @section install_ces Instalando módulo ces para desarrollo
 * Seguimos instrucciones de drupal https://drupal.org/project/1367140/git-instructions
 * Nos bajamos módulo desde git:
 * @code
 * $git clone --branch 7.x-1.x TU_USUARIO_DRUPAL\@git.drupal.org:sandbox/esteve/1367140.git ces
 * @endcode
 * @code
 * Cloning into 'ces'...
 * TU_USUARIO_DRUPAL\@git.drupal.org's password: 
 * remote: Counting objects: 643, done.
 * remote: Compressing objects: 100% (639/639), done.
 * remote: Total 643 (delta 455), reused 0 (delta 0)
 * Receiving objects: 100% (643/643), 211.59 KiB | 169 KiB/s, done.
 * Resolving deltas: 100% (455/455), done.
 * @endcode
 * Añadimos nuestro email a la configuración de git:
 * @code
 * $git config user.email "TU_EMAIL"
 * $git config user.name "TU_USUARIO_DRUPAL"
 * @endcode
 * Activamos módulo desde drush::
 * @code
 * drush en $(for m in `find sites/all/modules/ -name '*.module'` ; do basename $m .module ; done)
 * ces is already enabled.                                                                                                                                       [ok]
 * The following extensions will be enabled: ces_user, ces_bank, ces_develop, ces_blog, ces_summaryblock, ces_notify, ces_offerswants
 * Do you really want to continue? (y/n): y   
 * ces_bank was enabled successfully.                                                                                                                            [ok]
 * ces_blog was enabled successfully.                                                                                                                            [ok]
 * ces_develop was enabled successfully.                                                                                                                         [ok]  
 * ces_notify was enabled successfully.                                                                                                                          [ok]  
 * ces_offerswants was enabled successfully.                                                                                                                     [ok]  
 * ces_summaryblock was enabled successfully.                                                                                                                    [ok]  
 * ces_user was enabled successfully.                                                                                                                            [ok]
 * Initial records created.
 * @endcode
 * 
 * @section install_greences Instalando theme greences
 * Bajamos tema desde la rama dev:
 * @code
 * $git clone --branch 7.x-1.x TU_USUARIO_DRUPAL\@git.drupal.org:sandbox/esteve/1866046.git greences
 * @endcode
 * Activar theme:
 * @code
 * $drush en greences
 * @endcode
 * Faltara ponerlo como predeterminado.
 * Nota: Es necesario tener instalado el tema ZEN (versión 7.x-5.x) que es el 
 * padre de greences https://drupal.org/project/zen
 * @code
 * $ drush dl zen
 * Project zen (7.x-5.4) downloaded to sites/all/themes/zen.                  [success]
 * $ drush en zen
 * The following extensions will be enabled: zen
 * Do you really want to continue? (y/n): y
 * zen was enabled successfully.                                                   [ok]
 * $ drush cc all
 * 'all' cache was cleared in self
 * @endcode
 * @section install_activate_modules Activamos módulos
 * Activamos módulos del sistema
 * - Syslog: Para tener registros del sistema
 * - Testing: Para testear aplicación
 * @}
 */
