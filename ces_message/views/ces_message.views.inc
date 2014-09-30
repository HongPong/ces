<?php
/**
 * @file
 * Implements the ces_message view message.
 *
 * @defgroup ces_message_view_data Ces Message View
 * @ingroup ces_message
 * @{
 * View from message.
 */

/**
 * Implements hook_views_data().
 */
function ces_message_views_data() {
  $data['ces_message']['table']['group'] = t('Ces Message');
  $data['ces_message']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Ces Message table'),
    'help' => t('Ces Message view.'),
    'weight' => -10,
  );

  $data['ces_message']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'user',
    ),
    'ces_exchange' => array(
      'left_field' => 'id',
      'field' => 'exchange',
    ),
  );
  $data['ces_message']['mid'] = array(
    'title' => t('Ces Message Id'),
    'help' => t('Ces Message id.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'relationship' => array(
      'base' => 'users',
      'base field' => 'uid',
      // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
      'handler' => 'views_handler_relationship',
      'label' => t('Ces message users relationship'),
      'title' => t('Ces message users relationship'),
      'help' => t('More information on this relationship'),
    ),
  );
  $data['ces_message']['subject'] = array(
    'title' => t('Subject'),
    'help' => t('Subject of message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['ces_message']['body'] = array(
    'title' => t('Body'),
    'help' => t('Body of message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['ces_message']['type_message'] = array(
    'title' => t('Type'),
    'help' => t('Type of message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['ces_message']['state'] = array(
    'title' => t('Read'),
    'help' => t('Read on/off.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('State'),
      'type' => 'yes-no',
      'use equal' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ces_message']['created'] = array(
    'title' => t('Date message'),
    'help' => t('Date message.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['ces_message']['exchange'] = array(
    'title' => t('Exchange'),
    'help' => t('Exchange.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  return $data;
}
/**
 * @} End of "defgroup ces_message_view".
 */
