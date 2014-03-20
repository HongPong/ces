<?php

/**
 * @file
 * Functions from parse users
 */

/**
 * Parse setting
 */

function parse_users($data, $row) {

  if ( $data ) {

    // create drupal user

    // The admin user account is created with the exchange 
    $query = db_query('SELECT ca.name FROM {ces_account} ca where ca.name=:name',
      array(':name' => $data['UID']));
    $result = $query->fetchAllAssoc('name');

    if ( !empty($result) ) return ;

    // @todo Al crear un usuario se utiliza el mail como identificador y se 
    // genera un password aleatorio, enviano un email al usuario que podra
    // resetear el password.
    //
    // Comprobar que es el comportamiento que deseamos.

    //This will generate a random password, you could set your own here
    $password = user_password(8);

    //set up the user fields
    $fields = array(
      'name' => $data['UID'],
      'mail' => ( $GLOBALS['anonymous'] ) ? 'test-'.$data['UID'].'@test.com' : $data['Email'],
      'pass' => $password,
      'status' => 1,
      'init' => 'email address',
      'roles' => array(
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
      ),
    );

    //the first parameter is left blank so a new user is created
    $user_drupal = user_save('', $fields);

    // If you want to send the welcome email, use the following code

    // Manually set the password so it appears in the e-mail.
    $user_drupal->password = $fields['pass'];

    // Send the e-mail through the user module.
    // $email = "eduardo@mamedu.com";
    // if ( $GLOBALS['send_mail_user'] ) {
    //   drupal_mail('user', 'register_no_approval_required', $email, NULL, array('account' => $user_drupal), variable_get('site_mail', 'noreply@example..com'));
    // }


    // User in CES

    $bank = new Bank();
    $limit = $bank->getDefaultLimitChain($GLOBALS['exchange_id']);
    $account = array(
      'exchange' => $GLOBALS['exchange_id'],
      'name' => $data['UID'],
      'limitchain' => $limit['id'],
      'kind' => LocalAccount::TYPE_INDIVIDUAL,
      'state' => LocalAccount::STATE_HIDDEN,
      'users' => array(
        array(
          'user' => $data['UID'],
          'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
        ),
      ),
    );
    $bank->createAccount($account);
    $bank->activateAccount($account);

    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $GLOBALS['import_id'],
        'object' => 'user',
        'object_id' => $user_drupal->uid,
        'row' => $user_drupal->uid,
        'data' => serialize($data)
      ))->execute();

    return $nid;

  }

  return FALSE ;

}

/**
 * Delete setting
 * 
 * Delete exchange import
 */

function delete_setting($import_id) {

  echo 'Pendiente borrado de importación '.$import_id;

}

?>
