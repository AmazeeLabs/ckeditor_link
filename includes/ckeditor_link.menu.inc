<?php
/**
 * @file
 * Written by Henri MEDOT <henri.medot[AT]absyx[DOT]fr>
 * http://www.absyx.fr
 */

/**
 * Implementation of hook_ckeditor_link_TYPE_autocomplete().
 */
function ckeditor_link_ckeditor_link_menu_autocomplete($string) {
  $matches = array();

  $menus = array_keys(array_filter(variable_get('ckeditor_link_autocomplete_menus', array())));
  if (count($menus)) {
    $query = db_select('menu_links', 'ml')
      ->fields('ml', array('link_path', 'link_title'))
      ->condition('ml.link_title', '%'. db_like($string) .'%', 'LIKE')
      ->condition('ml.hidden', 0)
      ->condition('ml.external', 0)
      ->orderBy('ml.link_title')
      ->range(0, 10);
    if (function_exists('ckeditor_link_ckeditor_link_i18n_menu_autocomplete')) {
      $query->fields('ml', array('language'));
    }
    if (!in_array('- any -', $menus)) {
      $query->condition('ml.menu_name', $menus, 'IN');
    }
    $result = $query->execute();
    foreach ($result as $item) {
      $router_item = menu_get_item($item->link_path);
      if ($router_item && $router_item['access']) {
        $langcode = (isset($item->language)) ? $item->language : LANGUAGE_NONE;
        $path = ckeditor_link_path_prefix_language($item->link_path, $langcode);
        $matches[$path] = $item->link_title;
      }
    }
  }

  return $matches;
}

/**
 * Implementation of hook_ckeditor_link_TYPE_settings().
 */
function ckeditor_link_ckeditor_link_menu_settings() {
  $form['menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu items'),
  );
  $form['menu']['ckeditor_link_autocomplete_menus'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Menus'),
    '#options' => array('- any -' => t('<em>Any menu</em>')) + array_map('check_plain', menu_get_menus()),
    '#default_value' => variable_get('ckeditor_link_autocomplete_menus', array()),
    '#description' => t('Select the menus to be available as autocomplete suggestions.'),
  );

  return $form;
}
