<?php

/**
 * @file
 * Implements the ces_message functionality.
 */

/**
 * @defgroup ces_message_message Ces users messages
 * @ingroup ces_message
 * @{
 * creating a core Entity API entity.
 */

/**
 * Basic information for the page.
 */
function ces_message_info_page($args = FALSE) {
  $content['preface'] = array(
    '#type' => 'item',
    '#markup' => t('The ces message.'),
  );
  // @todo pagina para administrar todos los mensajes del exchange
  if (ces_bank_access('admin', 'exchange', ces_bank_get_current_exchange())) {
    $content['preface']['#markup'] = t('You can view all ces meessage !link.',
      array('!link' => l(t('here'), 'ces/bank/account/message/all'))
    );
  }
  $content['table'] = ces_message_list_entities($args);

  return $content;
}


/**
 * Fetch a message object.
 *
 * @param int $mid
 *   Integer specifying the ces message id.
 * @param bool $reset
 *   A boolean indicating that the internal cache should be reset.
 *
 * @return object
 *   A fully-loaded $message object or FALSE if it cannot be loaded.
 *
 * @see ces_message_load_multiple()
 * @see ces_message_menu()
 */
function ces_message_load($mid = NULL, $reset = FALSE) {
  $mids = (isset($mid) ? array($mid) : array());
  $message = ces_message_load_multiple($mids, array(), $reset);
  return $message ? reset($message) : FALSE;
}
/**
 * Loads multiple message entities.
 *
 * We only need to pass this request along to entity_load(), which
 * will in turn call the load() method of our entity controller class.
 */
function ces_message_load_multiple($mids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('ces_message', $mids, $conditions, $reset);
}

/**
 * Implements the uri callback.
 */
function ces_message_uri($message) {
  return array(
    'path' => 'ces/bank/account/message/' . $message->mid,
  );
}

/**
 * Returns a render array with all ces_message entities.
 *
 * In this message example we know that there won't be many entities,
 * so we'll just load them all for display. See pager_example.module
 * to implement a pager. Most implementations would probably do this
 * with the contrib Entity API module, or a view using views module,
 * but we avoid using non-core features in the Templates project.
 *
 * @see pager_example.module
 */
function ces_message_list_entities($args = FALSE) {
  global $user;
  $current_exchange = ces_bank_get_current_exchange();
  $current_exchange_id = $current_exchange['id'];

  $all = ($args == 'all' && ces_bank_access('admin', 'exchange', $current_exchange_id)) ? TRUE : FALSE;
  $content = array();
  if ($all) {
    // Load all message.
    // Limit current exchange.
    // $conditions = array('exchange' => $current_exchange_id);
    $conditions = FALSE;
  }
  else {
    // Load all of our messages.
    $conditions = array('user' => $user->uid);
  }
  $entities = ces_message_load_multiple(FALSE, $conditions);
  if (!empty($entities)) {
    foreach ($entities as $entity) {
      // Create tabular rows for our entities.
      $exchange = ces_bank_get_exchange($entity->exchange);
      $user_row = user_load($entity->user);
      if ($all) {
        $rows[] = array(
          'data' => array(
            'user' => $user->name,
            'exchange' => $exchange['shortname'],
            'created' => format_date($entity->created, 'custom', 'l d \d\e F \d\e\l Y | G:i'),
            'subject' => l($entity->subject, 'ces/bank/account/message/' . $entity->mid),
            'type_message' => $entity->type_message,
          ),
        );
      }
      else {
        $rows[] = array(
          'data' => array(
            'exchange' => $exchange['shortname'],
            'created' => format_date($entity->created, 'custom', 'l d \d\e F \d\e\l Y | G:i'),
            'subject' => l($entity->subject, 'ces/bank/account/message/' . $entity->mid),
            'type_message' => $entity->type_message,
          ),
        );
      }
    }
    if ($all) {
      $content['entity_table'] = array(
        '#theme' => 'table',
        '#rows' => $rows,
        '#header' => array(
          t('User'),
          t('Exchange'),
          t('Created'),
          t('Subject'),
          t('Type'),
        ),
      );
    }
    else {
      $content['entity_table'] = array(
        '#theme' => 'table',
        '#rows' => $rows,
        '#header' => array(
          t('Exchange'),
          t('Created'),
          t('Subject'),
          t('Type'),
        ),
      );
    }
  }
  else {
    // There were no entities. Tell the user.
    $content[] = array(
      '#type' => 'item',
      '#markup' => t('No ces_message entities currently exist.'),
    );
  }
  return $content;
}

/**
 * Callback for a page title when this entity is displayed.
 */
function ces_message_title($entity) {
  return t('Ces Messages');
}

/**
 * Menu callback to display an entity.
 *
 * As we load the entity for display, we're responsible for invoking a number
 * of hooks in their proper order.
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 */
function ces_message_view($entity, $view_mode = 'tweaky') {
  // Our entity type, for convenience.
  $entity_type = 'ces_message';
  // Start setting up the content.

  $entity->content = array(
    '#view_mode' => $view_mode,
  );
  // Build fields content - this is where the Field API really comes in to play.
  // The task has very little code here because it all gets taken care of by
  // field module.
  // field_attach_prepare_view() lets the fields load any data they need
  // before viewing.
  field_attach_prepare_view($entity_type, array($entity->mid => $entity),
    $view_mode);
  // We call entity_prepare_view() so it can invoke hook_entity_prepare_view()
  // for us.
  entity_prepare_view($entity_type, array($entity->mid => $entity));
  // Now field_attach_view() generates the content for the fields.
  $entity->content += field_attach_view($entity_type, $entity, $view_mode);

  // OK, Field API done, now we can set up some of our own data.
  $entity->content['created'] = array(
    '#type' => 'item',
    '#title' => t('Created date'),
    '#markup' => format_date($entity->created),
  );
  $entity->content['type_message'] = array(
    '#type' => 'item',
    '#title' => t('Type message'),
    '#markup' => t($entity->type_message),
  );
  $entity->content['subject'] = array(
    '#type' => 'item',
    '#title' => t('Subject'),
    '#markup' => $entity->subject,
  );
  $entity->content['body'] = array(
    '#type' => 'item',
    '#prefix' => '<pre>',
    '#suffix' => '</pre>',
    '#title' => t('Body'),
    '#markup' => $entity->body,
  );

  // Now to invoke some hooks. We need the language code for
  // hook_entity_view(), so let's get that.
  global $language;
  $langcode = $language->language;
  // And now invoke hook_entity_view().
  module_invoke_all('entity_view', $entity, $entity_type, $view_mode,
    $langcode);
  // Now invoke hook_entity_view_alter().
  drupal_alter(array('ces_message_view', 'entity_view'),
    $entity->content, $entity_type);

  // And finally return the content.
  return $entity->content;
}

/**
 * We save the entity by calling the controller.
 */
function ces_message_save(&$entity) {
  return entity_get_controller('ces_message')->save($entity);
}


/**
 * Use the controller to delete the entity.
 */
function ces_message_delete($entity) {
  entity_get_controller('ces_message')->delete($entity);
}

/**
 * CesMessageEntityControllerInterface definition.
 *
 * We create an interface here because anyone could come along and
 * use hook_entity_info_alter() to change our controller class.
 * We want to let them know what methods our class needs in order
 * to function with the rest of the module, so here's a handy list.
 *
 * @see hook_entity_info_alter()
 */
interface CesMessageEntityControllerInterface
  extends DrupalEntityControllerInterface {

  /**
   * Create an entity.
   */
  public function create();

  /**
   * Save an entity.
   *
   * @param object $entity
   *   The entity to save.
   */
  public function save($entity);

  /**
   * Delete an entity.
   *
   * @param object $entity
   *   The entity to delete.
   */
  public function delete($entity);

}

/**
 * CesMessageEntityController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class CesMessageEntityController
  extends DrupalDefaultEntityController
  implements CesMessageEntityControllerInterface {

  /**
   * Create and return a new ces_message entity.
   */
  public function create() {
    global $language;
    $entity = new stdClass();
    $entity->type = 'ces_message';
    $entity->mid = 0;
    $entity->type_message = '';
    $entity->bundle_type = 'default';
    $entity->subject = '';
    $entity->body = '';
    $entity->user = 0;
    $entity->exchange = 0;
    $entity->state = 0;
    return $entity;
  }

  /**
   * Saves the custom fields using drupal_write_record().
   */
  public function save($entity) {
    // If our entity has no id, then we need to give it a
    // time of creation.
    if (empty($entity->mid)) {
      $entity->created = time();
    }
    // Invoke hook_entity_presave().
    module_invoke_all('entity_presave', $entity, 'ces_message');
    // The 'primary_keys' argument determines whether this will be an insert
    // or an update. So if the entity already has an ID, we'll specify
    // id as the key.
    $primary_keys = $entity->mid ? 'id' : array();
    // Write out the entity record.
    drupal_write_record('ces_message', $entity, $primary_keys);
    // We're going to invoke either hook_entity_update() or
    // hook_entity_insert(), depending on whether or not this is a
    // new entity. We'll just store the name of hook_entity_insert()
    // and change it if we need to.
    $invocation = 'entity_insert';
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if (empty($primary_keys)) {
      field_attach_insert('ces_message', $entity);
      $message = t('Inserted message %action', array('%action' => $entity->type_message));
      drupal_set_message($message);
    }
    else {
      field_attach_update('ces_message', $entity);
      $invocation = 'entity_update';
    }
    // Invoke either hook_entity_update() or hook_entity_insert().
    module_invoke_all($invocation, $entity, 'ces_message');
    return $entity;
  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for deleteMultiple().
   */
  public function delete($entity) {
    $this->deleteMultiple(array($entity));
  }

  /**
   * Delete one or more ces_message entities.
   *
   * Deletion is unfortunately not supported in the base
   * DrupalDefaultEntityController class.
   *
   * @param array $entities
   *   An array of entity IDs or a single numeric ID.
   */
  public function deleteMultiple($entities) {
    $mids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {
          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, 'ces_message');
          field_attach_delete('ces_message', $entity);
          $mids[] = $entity->mid;
        }
        db_delete('ces_message')
          ->condition('id', $mids, 'IN')
          ->execute();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('ces_message', $e);
        throw $e;
      }
    }
  }
}

/**
 * @} End of "defgroup ces_message".
 */
