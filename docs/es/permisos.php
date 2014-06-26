<?php
/**
 * @file
 * Documentación del funcionamiento de los permisos
 */

/**
 *
 * @defgroup ces_permissions Permisos en Integral CES
 * @{
 * 
 * Permisos
 *
 * El sistema de permisos es más complejo que el de drupal, ya que por ejemplo
 * un usuario puede ver el balance de los accounts que están en la exchange de
 * sus accounts, pero no al revés. Para esto tendríamos que crear permisos 
 * para cada nueva exchange e írselas asignando a cada usuario. Con el sistema
 * que hay ahora esto se calcula dinámicamente.
 * 
 * Hay tipos de recursos o scopes:
 * 
 * . 
 * - user
 * - account 
 * - exchange
 * - global
 * 
 * Y propiedades dentro de los recursos
 * 
 * . 
 * - account seller
 * - account buyer 
 * - account transactions 
 * - exchange details 
 * - exchange accounts 
 * - ...
 * 
 * Y también hay tipos de permisos
 * 
 * .
 * - view
 * - use
 * - edit
 * - admin
 * 
 * 
 * Entonces una entrada de la tabla ces_permissions que sea:
 * 
 * ['view', 'exchange accounts', 2, 'exchange', 2]
 * 
 * significa que damos permiso 'view' en el objeto 'exchange accounts' referido
 * a la exchange id=2 a todos los usuarios que tengan accounts en la exchange 
 * id=2.
 * 
 * 
 * Luego la página de administración de permisos, si se hace, se puede 
 * plantear de dos formas: Una interfície que esconda el funcionamiento 
 * interno y que pida casos de uso general tipo:
 * .
 * - Los usuarios pueden pagar
 * - Los usuarios pueden vender 
 * 
 * Esto tiene la dificultad de identficar los casos de uso apropiados y que hay
 * que programar la lógica que relaciona los dos mundos, pero resulta en ua 
 * interfície comprensible.
 * 
 * Una interfície que sea tal cual la base de datos. Esto es más potente y
 * flexible y lleva menos trabajo, pero probablemente sea más difícil de 
 * usar.
 * 
 * Yo ahora mismo me decantría por no hacerlo o en todo caso hacer la segunda
 * opción por ser más sencilla. Y siempre podemos ayudar un poco al 
 * administrador con campos desplegables y tal en vez de campos de texto llano.
 * 
 * 
 * @}
 **/