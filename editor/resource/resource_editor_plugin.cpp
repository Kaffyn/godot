/**************************************************************************/
/*  resource_editor_plugin.cpp                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "resource_editor_plugin.h"

#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/themes/editor_scale.h"

void ResourceEditorPlugin::edit(Object *p_object) {
	Resource *res = Object::cast_to<Resource>(p_object);
	if (resource_editor && res) {
		resource_editor->edit(Ref<Resource>(res));
	}
}

bool ResourceEditorPlugin::handles(Object *p_object) const {
	Resource *res = Object::cast_to<Resource>(p_object);
	if (!res) {
		return false;
	}

	// Exclude types handled by other editors
	if (res->is_class("Script")) {
		return false;
	}
	if (res->is_class("PackedScene")) {
		return false;
	}
	// Shader editor handles Shader
	if (res->is_class("Shader")) {
		return false;
	}

	return true;
}

void ResourceEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		resource_editor->show();
		if (library_button) {
			library_button->show();
		}
	} else {
		resource_editor->hide();
		if (library_button) {
			if (resource_library->is_visible_in_tree()) {
				hide_bottom_panel();
			}
			library_button->hide();
		}
	}
}

ResourceEditorPlugin::ResourceEditorPlugin() {
	resource_editor = memnew(ResourceEditor);
	resource_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	EditorInterface::get_singleton()->get_editor_main_screen()->add_child(resource_editor);
	resource_editor->hide();

	resource_library = memnew(ResourceLibrary);
	resource_library->set_custom_minimum_size(Size2(0, 250 * EDSCALE));
	library_button = add_control_to_bottom_panel(resource_library, "Library");
	library_button->hide();
}

ResourceEditorPlugin::~ResourceEditorPlugin() {
}
