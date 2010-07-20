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
            var element = this.getElement();
            Drupal.behaviors.autocomplete(element.$);
            var link = element.getFirst().getFirst().getFirst();
            link.
          }
        });
        content.onChange = CKEDITOR.tools.override(content.onChange, function(original) {
          return function() {
            original.call(this);
            var dialog = this.getDialog();
            var element = dialog.getContentElement('info', 'drupalOptions').getElement().getParent().getParent();
            if (this.getValue() == 'drupal') {
              element.show();
              if (editor.config.linkShowTargetTab) {
                dialog.showPage('target');
              }
              var uploadTab = dialog.definition.getContents('upload');
              if (uploadTab && !uploadTab.hidden) {
                dialog.hidePage('upload');
              }
            }
            else {
              element.hide();
            }
          };
        });
        content.setup = function(data) {
          if (!data.url) {
            data.type = 'drupal';
          }
          if (data.type) {
            this.setValue(data.type);
          }
        }
        content.validate = function() {
          var dialog = this.getDialog();
          if (this.getValue() != 'drupal') {
            return true;
          }
          var func = CKEDITOR.dialog.validate.notEmpty(editor.lang.link.noUrl);
          alert(func.apply(this.getElement().getDocument().getById('drupal_link')));
        },
        content.commit = function(data) {
          data.type = this.getValue();
          if (data.type == 'drupal') {
            data.type = 'url';
            var dialog = this.getDialog();
            dialog.getContentElement('info', 'protocol').setValue('');
            var match = /\((.*?)\)\s*$/i.exec(this.getElement().getDocument().getById('drupal_link').getValue());
            dialog.getContentElement('info', 'url').setValue(match && match[1]);
          }
        };
      });
    }
  });
})();