<?php

/**
 * @file
 * Functions from parse users
 */

/**
 * Parse users
 */
function parse_users($import_id, $data, $row, &$context) {
  if (isset($context['results']['error']))
    return;
  $tx = db_transaction();
  try {
    $context['results']['import_id'] = $import_id;
    $import = ces_import4ces_import_load($import_id);
    
    // @todo Al crear un usuario se utiliza el mail como identificador y se 
    // genera un password aleatorio, enviano un email al usuario que podra
    // resetear el password.
    //
    // Comprobar que es el comportamiento que deseamos.
    
    if (CES_IMPORT4CES_RESET_PASSWORD) {
      $password = user_password(8);
    }
    else {
      $password = $data['Password'];
    }

    /*
      UserType: El tipus de compte. adm per administrador, org per organització
      sense ànim de lucre, ind per individual, fam per compartit, com per
      empreses, pub per a comptes públics, vir per virtual. L'administrador té
      més permisos i només n'hi ha un (que jo sàpiga), org, ind, fam i com són
      iguals a la pràctica (crec). pub és més accessible en el sentit que les
      transaccions d'aquest compte les pot veure tothom. vir són comptes per
      comptabilitzar els intercanvis  amb altres xarxes i no pertanyen a ningú.

      kind One of INDIVIDUAL (0), SHARED (1), ORGANIZATION (2), COMPANY (3), PUBLIC (4)

      'kind' => LocalAccount::TYPE_INDIVIDUAL,

      const TYPE_INDIVIDUAL = 0;
      const TYPE_SHARED = 1;
      const TYPE_ORGANIZATION = 2;
      const TYPE_COMPANY = 3;
      const TYPE_PUBLIC = 4;
      const TYPE_VIRTUAL = 5;
     */

    $type_user = array(
      'adm' => LocalAccount::TYPE_INDIVIDUAL,
      'org' => LocalAccount::TYPE_ORGANIZATION,
      'ind' => LocalAccount::TYPE_INDIVIDUAL,
      'fam' => LocalAccount::TYPE_SHARED,
      'com' => LocalAccount::TYPE_COMPANY,
      'pub' => LocalAccount::TYPE_PUBLIC,
      'vir' => LocalAccount::TYPE_VIRTUAL
    );

    /*
      Lang: idioma en tres lletres. No segueix l'estàndard ISO. eng per anglès,
      cat per català, spa per castellà.
     */

    $langs = array(
      'eng' => 'en',
      'cat' => 'ca',
      'spa' => 'es',
      'default' => language_default()->language,
    );

    //set up the user fields
    $fields = array(
      'name' => $data['UID'],
      'mail' => ($import->anonymous) ? 'test-' . $data['UID'] . '@test.com' : $data['Email'],
      'pass' => $password,
      'status' => ( $data['Locked'] == 0 ) ? 1 : 0,
      // 'init' => ( $GLOBALS['anonymous'] ) ? 'test-'.$data['UID'].'@test.com' : $data['Email'],
      'language' => ( isset($langs[$data['Lang']]) ) ? $langs[$data['Lang']] : $langs['default'],
      'roles' => array(
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
      ),
      // User custom fields
      'ces_firstname' => array(LANGUAGE_NONE => array(array('value' => $data['Firstname']))),
      'ces_surname' => array(LANGUAGE_NONE => array(array('value' => $data['Surname']))),
      'ces_address' => array(LANGUAGE_NONE => array(array('value' => $data['Address1'] . "\n" . $data['Address2']))),
      'ces_town' => array(LANGUAGE_NONE => array(array('value' => $data['Address3']))),
      'ces_postcode' => array(LANGUAGE_NONE => array(array('value' => $data['Postcode']))),
      'ces_phonemobile' => array(LANGUAGE_NONE => array(array('value' => $data['PhoneM']))),
      'ces_phonework' => array(LANGUAGE_NONE => array(array('value' => $data['PhoneW']))),
      'ces_phonehome' => array(LANGUAGE_NONE => array(array('value' => $data['PhoneH']))),
      'ces_website' => array(LANGUAGE_NONE => array(array('value' => $data['WebSite']))),
    );

    $extra_data = array(
      'OrgName' => $data['OrgName'],
      'SubArea' => $data['SubArea'],
      'DefaultSub' => $data['DefaultSub'],
      'PhoneF' => $data['PhoneF'],
      'IM' => $data['IM'],
      'DOB' => $data['DOB'],
      'NoEmail1' => $data['NoEmail1'],
      'NoEmail2' => $data['NoEmail2'],
      'NoEmail3' => $data['NoEmail3'],
      'NoEmail4' => $data['NoEmail4'],
      'Hidden' => $data['Hidden'],
      'Created' => $data['Created'],
      'LastAccess' => $data['LastAccess'],
      'LastEdited' => $data['LastEdited'],
      'EditedBy' => $data['EditedBy'],
      'InvNo' => $data['InvNo'],
      'OrdNo' => $data['OrdNo'],
      'Coord' => $data['Coord'],
      'CredLimit' => $data['CredLimit'],
      'DebLimit' => $data['DebLimit'],
      'LocalOnly' => $data['LocalOnly'],
      'Notes' => $data['Notes'],
      'Photo' => $data['Photo'],
      'HideAddr1' => $data['HideAddr1'],
      'HideAddr2' => $data['HideAddr2'],
      'HideAddr3' => $data['HideAddr3'],
      'HideArea' => $data['HideArea'],
      'HideCode' => $data['HideCode'],
      'HidePhoneH' => $data['HidePhoneH'],
      'HidePhoneW' => $data['HidePhoneW'],
      'HidePhoneF' => $data['HidePhoneF'],
      'HidePhoneM' => $data['HidePhoneM'],
      'HideEmail' => $data['HideEmail'],
      'IdNo' => $data['IdNo'],
      'LoginCount' => $data['LoginCount'],
      'SubsDue' => $data['SubsDue'],
      'Closed' => $data['Closed'],
      'DateClosed' => $data['DateClosed'],
      'Translate' => $data['Translate'],
      'Buddy' => $data['Buddy'],
    );

    // Administrative user hs already been created in the first step, but we now
    // are completing the record with the user info.
    if (substr($data['UID'], -4) == '0000') {
      $user_drupal = user_load_by_name($data['UID']);
    }
    else {
      $user_drupal = FALSE;
    }
    $user_drupal = user_save($user_drupal, $fields);
    if ($user_drupal === FALSE) {
      throw new Exception(t('Error creating Drupal user.'));
    }

    // If you want to send the welcome email, use the following code
    // Manually set the password so it appears in the e-mail.
    $user_drupal->password = $fields['pass'];

    // Account in CES. The administrative account has already been created in
    // exchange import.
    if (substr($user_drupal->name, -4) != '0000') {
      $bank = new Bank();
      $limit = $bank->getDefaultLimitChain($import->exchange_id);
      $account = array(
        'exchange' => $import->exchange_id,
        'name' => $data['UID'],
        'limitchain' => $limit['id'],
        'kind' => $type_user[$data['UserType']],
        'state' => LocalAccount::STATE_HIDDEN,
        'users' => array(
          array(
            'user' => $user_drupal->uid,
            'role' => AccountUser::ROLE_ACCOUNT_ADMINISTRATOR,
          ),
        ),
      );
      $bank->createAccount($account, FALSE);
      $bank->activateAccount($account);
    }
    db_insert('ces_import4ces_objects')
      ->fields(array(
        'import_id' => $import_id,
        'object' => 'user',
        'object_id' => $user_drupal->uid,
        'row' => $row,
        'data' => serialize($extra_data),
      ))->execute();
    ces_import4ces_update_row($import_id, $row);
  }
  catch (Exception $e) {
    $tx->rollback();
    ces_import4ces_batch_fail_row($import_id, array_keys($data),
      array_values($data), $row, $context);
    $context['results']['error'] = check_plain($e->getMessage());
  }
}
