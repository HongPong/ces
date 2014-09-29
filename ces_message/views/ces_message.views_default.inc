<?php
/**
 * @file
 * @brief Views to ces message
 */
 
/**
 * Implements hook_views_default_views().
 */
function ces_message_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'ces_message';
  $view->description = 'List of message';
  $view->tag = 'default';
  $view->base_table = 'users';
  $view->human_name = 'Messages';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Ces Message View';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Reinicialitza';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['pager']['options']['tags']['first'] = '« primer';
  $handler->display->display_options['pager']['options']['tags']['previous'] = '‹ anterior';
  $handler->display->display_options['pager']['options']['tags']['next'] = 'següent ›';
  $handler->display->display_options['pager']['options']['tags']['last'] = 'últim »';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'mid' => 'mid',
    'name' => 'name',
    'state' => 'state',
    'subject' => 'subject',
    'created' => 'created',
  );
  $handler->display->display_options['style_options']['default'] = 'created';
  $handler->display->display_options['style_options']['info'] = array(
    'mid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'state' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'subject' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Camp: Ces Message: Ces Message Id */
  $handler->display->display_options['fields']['mid']['id'] = 'mid';
  $handler->display->display_options['fields']['mid']['table'] = 'ces_message';
  $handler->display->display_options['fields']['mid']['field'] = 'mid';
  $handler->display->display_options['fields']['mid']['label'] = 'Id';
  $handler->display->display_options['fields']['mid']['exclude'] = TRUE;
  /* Camp: Ces Message: Date message */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'ces_message';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'long';
  $handler->display->display_options['fields']['created']['second_date_format'] = 'long';
  /* Camp: Ces Message: Assumpte */
  $handler->display->display_options['fields']['subject']['id'] = 'subject';
  $handler->display->display_options['fields']['subject']['table'] = 'ces_message';
  $handler->display->display_options['fields']['subject']['field'] = 'subject';
  $handler->display->display_options['fields']['subject']['label'] = 'Subject';
  $handler->display->display_options['fields']['subject']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['subject']['alter']['path'] = 'ces/bank/account/message/[mid]/view';
  /* Sort criterion: Ces Message: Date message */
  $handler->display->display_options['sorts']['created_1']['id'] = 'created_1';
  $handler->display->display_options['sorts']['created_1']['table'] = 'ces_message';
  $handler->display->display_options['sorts']['created_1']['field'] = 'created';
  $handler->display->display_options['sorts']['created_1']['order'] = 'DESC';
  /* Contextual filter: Usuari: Uid */
  $handler->display->display_options['arguments']['uid']['id'] = 'uid';
  $handler->display->display_options['arguments']['uid']['table'] = 'users';
  $handler->display->display_options['arguments']['uid']['field'] = 'uid';
  $handler->display->display_options['arguments']['uid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['uid']['exception']['title'] = 'Tot';
  $handler->display->display_options['arguments']['uid']['default_argument_type'] = 'current_user';
  $handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Ces Message: Assumpte */
  $handler->display->display_options['filters']['subject']['id'] = 'subject';
  $handler->display->display_options['filters']['subject']['table'] = 'ces_message';
  $handler->display->display_options['filters']['subject']['field'] = 'subject';
  $handler->display->display_options['filters']['subject']['operator'] = 'longerthan';
  $handler->display->display_options['filters']['subject']['value'] = '1';

  /* Display: List message */
  $handler = $view->new_display('page', 'List message', 'page');
  $handler->display->display_options['path'] = 'ces/bank/account/message/listm';
  $handler->display->display_options['menu']['title'] = 'Messages view';
  $handler->display->display_options['menu']['description'] = 'Ces messages List2';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'user-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['ces_message'] = array(
    t('Master'),
    t('Ces Message View'),
    t('more'),
    t('Apply'),
    t('Reinicialitza'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« primer'),
    t('‹ anterior'),
    t('següent ›'),
    t('últim »'),
    t('Id'),
    t('.'),
    t(','),
    t('Created'),
    t('Subject'),
    t('Tot'),
    t('List message'),
  );

  $export['messages'] = $view;
 
  $view = new view();
  $view->name = 'ces_message_list_all';
  $view->description = 'List of all user\'s message';
  $view->tag = 'default';
  $view->base_table = 'users';
  $view->human_name = 'All Ces Message';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Ces Message View All';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access user profiles';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Reinicialitza';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['pager']['options']['tags']['first'] = '« primer';
  $handler->display->display_options['pager']['options']['tags']['previous'] = '‹ anterior';
  $handler->display->display_options['pager']['options']['tags']['next'] = 'següent ›';
  $handler->display->display_options['pager']['options']['tags']['last'] = 'últim »';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'mid' => 'mid',
    'name' => 'name',
    'subject' => 'subject',
    'created' => 'created',
    'type_message' => 'type_message',
    'shortname' => 'shortname',
  );
  $handler->display->display_options['style_options']['default'] = 'created';
  $handler->display->display_options['style_options']['info'] = array(
    'mid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'subject' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'type_message' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'shortname' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Ces Exchange: Ces exchange message relationship */
  $handler->display->display_options['relationships']['id']['id'] = 'id';
  $handler->display->display_options['relationships']['id']['table'] = 'ces_exchange';
  $handler->display->display_options['relationships']['id']['field'] = 'id';
  $handler->display->display_options['relationships']['id']['required'] = TRUE;
  /* Camp: Ces Message: Date message */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'ces_message';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'long';
  $handler->display->display_options['fields']['created']['second_date_format'] = 'long';
  /* Camp: Ces Message: Ces Message Id */
  $handler->display->display_options['fields']['mid']['id'] = 'mid';
  $handler->display->display_options['fields']['mid']['table'] = 'ces_message';
  $handler->display->display_options['fields']['mid']['field'] = 'mid';
  $handler->display->display_options['fields']['mid']['label'] = 'Id';
  $handler->display->display_options['fields']['mid']['exclude'] = TRUE;
  /* Camp: Usuari: Nom */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'User';
  $handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
  /* Camp: Ces Message: Assumpte */
  $handler->display->display_options['fields']['subject']['id'] = 'subject';
  $handler->display->display_options['fields']['subject']['table'] = 'ces_message';
  $handler->display->display_options['fields']['subject']['field'] = 'subject';
  $handler->display->display_options['fields']['subject']['label'] = 'Subject';
  $handler->display->display_options['fields']['subject']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['subject']['alter']['path'] = 'ces/message/[mid]';
  /* Camp: Ces Message: Tipus */
  $handler->display->display_options['fields']['type_message']['id'] = 'type_message';
  $handler->display->display_options['fields']['type_message']['table'] = 'ces_message';
  $handler->display->display_options['fields']['type_message']['field'] = 'type_message';
  /* Camp: Ces Exchange: Exchange Shortname */
  $handler->display->display_options['fields']['shortname']['id'] = 'shortname';
  $handler->display->display_options['fields']['shortname']['table'] = 'ces_exchange';
  $handler->display->display_options['fields']['shortname']['field'] = 'shortname';
  $handler->display->display_options['fields']['shortname']['label'] = 'Exchange';
  $handler->display->display_options['fields']['shortname']['element_label_colon'] = FALSE;
  /* Sort criterion: Ces Message: Date message */
  $handler->display->display_options['sorts']['created_1']['id'] = 'created_1';
  $handler->display->display_options['sorts']['created_1']['table'] = 'ces_message';
  $handler->display->display_options['sorts']['created_1']['field'] = 'created';
  $handler->display->display_options['sorts']['created_1']['order'] = 'DESC';
  /* Contextual filter: Usuari: Nom */
  $handler->display->display_options['arguments']['name']['id'] = 'name';
  $handler->display->display_options['arguments']['name']['table'] = 'users';
  $handler->display->display_options['arguments']['name']['field'] = 'name';
  $handler->display->display_options['arguments']['name']['exception']['title'] = 'Tot';
  $handler->display->display_options['arguments']['name']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['name']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['name']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['name']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['name']['limit'] = '0';
  /* Filter criterion: Ces Message: Assumpte */
  $handler->display->display_options['filters']['subject']['id'] = 'subject';
  $handler->display->display_options['filters']['subject']['table'] = 'ces_message';
  $handler->display->display_options['filters']['subject']['field'] = 'subject';
  $handler->display->display_options['filters']['subject']['operator'] = 'longerthan';
  $handler->display->display_options['filters']['subject']['value'] = '1';
  /* Filter criterion: Usuari: Nom */
  $handler->display->display_options['filters']['uid']['id'] = 'uid';
  $handler->display->display_options['filters']['uid']['table'] = 'users';
  $handler->display->display_options['filters']['uid']['field'] = 'uid';
  $handler->display->display_options['filters']['uid']['value'] = '';
  $handler->display->display_options['filters']['uid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
  $handler->display->display_options['filters']['uid']['expose']['label'] = 'User';
  $handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
  $handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
  $handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Ces Exchange: Exchange Shortname */
  $handler->display->display_options['filters']['shortname']['id'] = 'shortname';
  $handler->display->display_options['filters']['shortname']['table'] = 'ces_exchange';
  $handler->display->display_options['filters']['shortname']['field'] = 'shortname';
  $handler->display->display_options['filters']['shortname']['operator'] = 'contains';
  $handler->display->display_options['filters']['shortname']['exposed'] = TRUE;
  $handler->display->display_options['filters']['shortname']['expose']['operator_id'] = 'shortname_op';
  $handler->display->display_options['filters']['shortname']['expose']['label'] = 'Exchange';
  $handler->display->display_options['filters']['shortname']['expose']['operator'] = 'shortname_op';
  $handler->display->display_options['filters']['shortname']['expose']['identifier'] = 'shortname';
  $handler->display->display_options['filters']['shortname']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Ces Message: Assumpte */
  $handler->display->display_options['filters']['subject_1']['id'] = 'subject_1';
  $handler->display->display_options['filters']['subject_1']['table'] = 'ces_message';
  $handler->display->display_options['filters']['subject_1']['field'] = 'subject';
  $handler->display->display_options['filters']['subject_1']['operator'] = 'contains';
  $handler->display->display_options['filters']['subject_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['subject_1']['expose']['operator_id'] = 'subject_1_op';
  $handler->display->display_options['filters']['subject_1']['expose']['label'] = 'Assumpte';
  $handler->display->display_options['filters']['subject_1']['expose']['operator'] = 'subject_1_op';
  $handler->display->display_options['filters']['subject_1']['expose']['identifier'] = 'subject_1';
  $handler->display->display_options['filters']['subject_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Ces Message: Tipus */
  $handler->display->display_options['filters']['type_message']['id'] = 'type_message';
  $handler->display->display_options['filters']['type_message']['table'] = 'ces_message';
  $handler->display->display_options['filters']['type_message']['field'] = 'type_message';
  $handler->display->display_options['filters']['type_message']['operator'] = 'contains';
  $handler->display->display_options['filters']['type_message']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_message']['expose']['operator_id'] = 'type_message_op';
  $handler->display->display_options['filters']['type_message']['expose']['label'] = 'Tipus';
  $handler->display->display_options['filters']['type_message']['expose']['operator'] = 'type_message_op';
  $handler->display->display_options['filters']['type_message']['expose']['identifier'] = 'type_message';
  $handler->display->display_options['filters']['type_message']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['type_message']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  $handler->display->display_options['filters']['type_message']['group_info']['label'] = 'Tipus';
  $handler->display->display_options['filters']['type_message']['group_info']['identifier'] = 'type_message';
  $handler->display->display_options['filters']['type_message']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['type_message']['group_info']['group_items'] = array(
    1 => array(),
    2 => array(),
    3 => array(),
  );

  /* Display: List all user's message */
  $handler = $view->new_display('page', 'List all user\'s message', 'page');
  $handler->display->display_options['path'] = 'ces/messages/admin';
  $handler->display->display_options['menu']['title'] = 'Messages view';
  $handler->display->display_options['menu']['description'] = 'Ces messages';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'user-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['ces_message_list_all'] = array(
    t('Master'),
    t('Ces Message View All'),
    t('more'),
    t('Apply'),
    t('Reinicialitza'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« primer'),
    t('‹ anterior'),
    t('següent ›'),
    t('últim »'),
    t('Ces exchange message relationship'),
    t('Created'),
    t('Id'),
    t('.'),
    t(','),
    t('User'),
    t('Subject'),
    t('Tipus'),
    t('Exchange'),
    t('Tot'),
    t('Assumpte'),
    t('List all user\'s message'),
  );
  
  $export['messages_all'] = $view;

  return $export;
}

/**
 * Implements hook_views_pre_render().
 */
// function ces_message_views_pre_render($view) {
//   if ($view->name == 'ces_message' ) {
//     foreach ($view->result as &$result) {
//       if($result->ces_message_state == 0) {
//         $result->ces_message_subject = "<b>" . $result->ces_message_subject . "</b>";
//       }
//     }
//   } 
// }
