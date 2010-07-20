// $Id$

/**
 * @file
 * Written by Henri MEDOT <henri.medot[AT]absyx[DOT]fr>
 * http://www.absyx.fr
 *
 * Portions of code:
 * Copyright (c) 2003-2010, CKSource - Frederico Knabben. All rights reserved.
 * For licensing, see LICENSE.html or http://ckeditor.com/license
 */

(function() {

  // Get a CKEDITOR.dialog.contentDefinition object by its ID.
  var getById = function(array, id, recurse) {
    for (var i = 0, item; (item = array[i]); i++) {
      if (item.id == id) return item;
      if (recurse && item[recurse]) {
        var retval = getById(item[recurse], id, recurse);
        if (retval) return retval;
      }
    }
    return null;
  };



  CKEDITOR.plugins.add('drupal_link', {

    init: function(editor, pluginPath) {
      CKEDITOR.on('dialogDefinition', function(e) {
        if ((e.editor != editor) || (e.data.name != 'link')) return;

        // Overrides definition.
        var definition = e.data.definition;
        var infoTab = definition.getContents('info');

        definition.onShow = CKEDITOR.tools.override(definition.onShow, function(original) {
          return function() {
            original.call(this);
          };
        });
/*
        definition.onOk = function() {
          
        };
*/
        // Overrides linkType definition.
        var content;
        content = getById(infoTab.elements, 'linkType');
        content.items.unshift(['Drupal', 'drupal']);
        content['default'] = 'drupal';
        infoTab.elements.push({
          type: 'html',
          id: 'drupalOptions',
          html: '<div>' + CKEDITOR.tools.htmlEncode(editor.lang.link.title)
            + '<form><div class="cke_dialog_ui_input_text"><input type="text" id="drupal_link" class="cke_dialog_ui_input_text form-autocomplete" />'
            + '<input type="hidden" id="drupal_link-autocomplete" class="autocomplete" value="'
            + CKEDITOR.tools.htmlEncode(Drupal.settings.ckeditor_link.autocomplete_path) + '" /></div></form></div>',
          onLoad: function() {
            Drupal.behaviors.autocomplete(this.getElement().$);
          }
        });
        content.onChange = CKEDITOR.tools.override(content.onChange, function(original) {
          return function() {
            original.call(this);
            var dialog = this.getDialog();
            var element = dialog.getContentElement('info', 'drupalOptions').getElement().getParent().getParent();
            if (this.getValue() == 'drupal') {
              element.show();
            }
            else {
              element.hide();
            }
          };
        });
        content.commit = function(data) {
          data.type = this.getValue();
          if (data.type == 'drupal') {
            data.type = 'url';
            var dialog = this.getDialog();
            dialog.getContentElement('info', 'protocol').setValue('');
            dialog.getContentElement('info', 'url').setValue('internal:node/4');
          }
        };
      });
    }
  });
})();