<?php

/**
 * @file
 * Functions from parse users
 */

/**
 * Parse setting
 */

function parse_users($data, $row) {

  echo '<pre>parse_users: ' ; print_r($data) ; echo '</pre>'; // exit() ; // DEV  
  if ( $data ) {

    // create user
    //
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
    $account = user_save('', $fields);

    // If you want to send the welcome email, use the following code

    // Manually set the password so it appears in the e-mail.
    $account->password = $fields['pass'];

    // Send the e-mail through the user module.
    // $email = "eduardo@mamedu.com";
    if ( $GLOBALS['send_mail_user'] ) {
      drupal_mail('user', 'register_no_approval_required', $email, NULL, array('account' => $account), variable_get('site_mail', 'noreply@example..com'));
    }


    $nid = db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $_SESSION['import']['id'],
        'object' => 'user',
        'object_id' => $account->uid,
        'row' => $account->uid,
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
