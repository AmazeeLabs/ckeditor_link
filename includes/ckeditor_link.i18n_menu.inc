<?php
/**
 * @file
 * Written by Henri MEDOT <henri.medot[AT]absyx[DOT]fr>
 * http://www.absyx.fr
 */

/**
 * Implementation of hook_ckeditor_link_TYPE_autocomplete().
 */
function ckeditor_link_ckeditor_link_i18n_menu_autocomplete($string) {
  $matches = array();

  $menus = array_keys(array_filter(variable_get('ckeditor_link_autocomplete_menus', array())));
  if (count($menus)) {
    $query = db_select('menu_links', 'ml');
    $query->innerJoin('locales_source', 'ls', 'ls.context = CONCAT(:prefix, ml.mlid, :suffix)', array(':prefix' => 'item:', ':suffix' => ':title'));
    $query->innerJoin('locales_target', 'lt', 'lt.lid = ls.lid');
    $query->fields('ml', array('link_path'));
    $query->addExpression('CONVERT(lt.translation USING utf8)', 'link_title');
    $query->fields('lt', array('language'));
    $query->where('CONVERT(lt.translation USING utf8) LIKE :pattern', array(':pattern' => '%'. db_like($string) .'%'));
    $query->condition('ml.hidden', 0);
    $query->condition('ml.external', 0);
    $query->orderBy('link_title');
    $query->range(0, 10);
    if (!in_array('- any -', $menus)) {
      $query->condition('ml.menu_name', $menus, 'IN');
    }
    $result = $query->execute();
    foreach ($result as $item) {
      $router_item = menu_get_item($item->link_path);
      if ($router_item && $router_item['access']) {
        $path = ckeditor_link_path_prefix_language($item->link_path, $item->language);
        $matches[$path] = $item->link_title;
      }
    }
  }

  return $matches;
}
