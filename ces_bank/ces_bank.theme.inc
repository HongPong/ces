<?php
/**
 * @file
 * All theme default functions for bank module.
 */
/**
 * Theme function for page ces_bank_exchange_view().
 */
function theme_ces_bank_exchange_view($variables) {
  $elem = $variables['element']['exchange'];
  $out = '<h2>' . $elem['name'] . '</h2>';
  $out .= '<dl>';
  $out .= '<dt>' . t('Code') . '</dt>';
  $out .= '<dd>' . $elem['code'] . '</dd>';
  $out .= '<dt>' . t('Website') . '</dt>';
  $out .= '<dd>' . $elem['website'] . '</dd>';
  $out .= '<dt>' . t('Country') . '</dt>';
  $out .= '<dd>' . $elem['country'] . '</dd>';
  $out .= '<dt>' . t('Town') . '</dt>';
  $out .= '<dd>' . $elem['town'] . ' (' . $elem['region'] . ')</dd>';
  $out .= '<dt>' . t('Currency') . '</dt>';
  $out .= '<dd>' . $elem['currencyname'] . ', ' . $elem['currencysymbol'] . '</dd>';
  $out .= '<dt>' . t('Currency value') . '</dt>';
  $out .= '<dd>' . $elem['currencyvalue'] . '</dd>';
  $out .= '<dt>' . t('Since') . '</dt>';
  $out .= '<dd>' . format_date($elem['created']) . '</dd>';
  $out .= '</dl>';
  return $out;
}
/**
 * Theme function for page ces_bank_account_view().
 */
function theme_ces_bank_account_view($variables) {
  $elem = $variables['element']['account'];
  $out = '<h2>' . t('Account details') . '</h2>';
  $out .= '<dl>';
  $out .= '<dt>' . t('Exchange') . '</dt>';
  $out .= '<dd>' . $elem['exchange'] . '</dd>';
  $out .= '<dt>' . t('Account number') . '</dt>';
  $out .= '<dd>' . $elem['name'] . '</dd>';
  $out .= '<dt>' . t('Username') . '</dt>';
  $out .= '<dd>' . $elem['username'] . '</dd>';
  $out .= '<dt>' . t('Type') . '</dt>';
  $out .= '<dd>' . $elem['kind'] . '</dd>';
  $out .= '<dt>' . t('Limit') . '</dt>';
  $out .= '<dd>' . $elem['limitchain'] . '</dd>';
  $out .= '<dt>' . t('State') . '</dt>';
  $out .= '<dd>' . $elem['state'] . '</dd>';
  $out .= '</dl>';
  return $out;
}