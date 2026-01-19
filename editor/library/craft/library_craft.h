/**************************************************************************/
/*  library_craft.h                                                       */
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

#pragma once

#include "editor/gui/editor_file_dialog.h"
#include "scene/gui/box_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/tree.h"

class LibraryCraft : public VBoxContainer {
	GDCLASS(LibraryCraft, VBoxContainer);

	LineEdit *craft_search;
	Tree *craft_tree;
	EditorFileDialog *creation_dialog;
	String current_creation_domain;

	// Callback to refresh assets list
	Callable on_resource_created_callback;

	void _update_craft_tree();
	void _on_craft_search_text_changed(const String &p_text);
	void _on_craft_item_activated();
	void _on_creation_file_selected(const String &p_path);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	static Ref<Resource> create_resource_from_domain(const StringName &p_domain_name);
	static Ref<Resource> create_resource_by_class(const StringName &p_class_name);

	void set_on_resource_created_callback(const Callable &p_callback);
	LibraryCraft();
	~LibraryCraft();
};
