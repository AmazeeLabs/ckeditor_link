<?php
/**
 * @file
 * Written by Henri MEDOT <henri.medot[AT]absyx[DOT]fr>
 * http://www.absyx.fr
 */

/**
 * Implementation of hook_ckeditor_link_TYPE_autocomplete().
 */
function ckeditor_link_ckeditor_link_node_autocomplete($string, $limit) {
  $matches = array();

  $node_types = array_keys(array_filter(variable_get('ckeditor_link_autocomplete_node_types', array('- any -' => '- any -'))));
  if (count($node_types)) {
    $or = db_or();
    $or->condition('n.title', '%'. db_like($string) .'%', 'LIKE');

    $query = db_select('node', 'n')
      ->fields('n', array('nid', 'title', 'language'))
      ->orderBy('n.title')
      ->orderBy('n.type')
      ->range(0, $limit)
      ->addTag('node_access');
    if (!in_array('- any -', $node_types)) {
      $query->condition('n.type', $node_types, 'IN');
    }
    if (module_exists('title')) {
      $query->join('field_data_title_field', 't', 't.entity_id = n.nid');
      $or->condition('t.title_field_value', '%'. db_like($string) .'%', 'LIKE');
      $query->addField('t', 'entity_id', 'entity_id');
      $query->addField('t', 'title_field_value', 'title_field');
      $query->addField('t', 'language', 'title_field_language');
    }
    $query->condition($or);
    $result = $query->execute();
    foreach ($result as $node) {
        $langcode = (isset($node->language)) ? $node->language : LANGUAGE_NONE;
        $label = $node->title;
        $path = ckeditor_link_path_prefix_language('node/'. $node->nid, $langcode);
        $entity = node_load($node->nid);
        if (module_exists('entity_translation') && entity_translation_enabled('node', $entity)) {
          $langcode = $node->title_field_language;
          $label = $node->title_field . ' (' . $langcode . ')';
          $path = ckeditor_link_path_prefix_language('node/'. $node->entity_id, $langcode);
        }

        $matches[$path] = $label;
    }
  }

  return $matches;
}

/**
 * Implementation of hook_ckeditor_link_TYPE_revert().
 */
function ckeditor_link_ckeditor_link_node_revert($path, &$langcode) {
  if (!preg_match('`^node/(\d+)$`', $path, $matches)) {
    return;
  }

  $nid = $matches[1];
  $query = db_select('node', 'n')
    ->fields('n', array('title', 'language'))
    ->condition('n.nid', $nid)
    ->addTag('node_access');
  if (module_exists('title')) {
    $query->join('field_data_title_field', 't', 't.entity_id = n.nid');
    $query->addField('t', 'title_field_value', 'title_field');
    $query->addField('t', 'language', 'title_field_language');
    $query->condition('t.language', $langcode, '=');
  }
  if ($node = $query->execute()->fetchObject()) {
    if ($node->language != $langcode) {
      $langcode = LANGUAGE_NONE;
    }
    $label = $node->title;
    $entity = node_load($nid);
    if (module_exists('title') && module_exists('entity_translation') && entity_translation_enabled('node', $entity)) {
      $label = $node->title_field;
      $langcode = $node->title_field_language;
    }
    return $label;
  }

  return FALSE;
}

/**
 * Implementation of hook_ckeditor_link_TYPE_url().
 */
function ckeditor_link_ckeditor_link_node_url($path, $langcode) {
  if (!preg_match('`^node/(\d+)$`', $path, $matches)) {
    return;
  }

  $nid = $matches[1];

  $languages = ckeditor_link_get_languages();
  if ($languages) {
    // Core language support for content types has 2 as maximum value. Over this
    // contrib modules such as Entity Translation might introduce different
    // behaviors, hence we just skip any language alteration.
    $node = db_query('SELECT nid, type, language FROM {node} WHERE nid = :nid', array(':nid' => $nid))->fetchObject();
    if ($node) {
      $language_support = variable_get('language_content_type_' . $node->type, 0) <= 2;
      if ($language_support && $node->language && ($node->language != LANGUAGE_NONE) && isset($languages[$node->language])) {
        $langcode = $node->language;
      }
    }
  }

  return ckeditor_link_url("node/$nid", $langcode);
}

/**
 * Implementation of hook_ckeditor_link_TYPE_settings().
 */
function ckeditor_link_ckeditor_link_node_settings() {
  $form['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Nodes'),
  );
  $form['node']['ckeditor_link_autocomplete_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#options' => array('- any -' => t('<em>Any content type</em>')) + array_map('check_plain', node_type_get_names()),
    '#default_value' => variable_get('ckeditor_link_autocomplete_node_types', array('- any -' => '- any -')),
    '#description' => t('Select the content types to be available as autocomplete suggestions.'),
  );

  return $form;
}
