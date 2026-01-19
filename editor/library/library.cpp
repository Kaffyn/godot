/**************************************************************************/
/*  library.cpp                                                           */
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

#include "library.h"
#include "editor/library/assets/library_assets.h"
#include "editor/library/craft/library_craft.h"
#include "editor/library/workbench/library_workbench.h"
#include "editor/resource/resource_editor.h"

void Library::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_resource_created"), &Library::_on_resource_created);
}

void Library::_on_resource_created() {
	if (assets_panel) {
		assets_panel->scan_project();
	}
}

Library::Library() {
	tabs = memnew(TabContainer);
	tabs->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(tabs);

	// Tab 1: Assets (Flat List)
	assets_panel = memnew(LibraryAssets);
	tabs->add_child(assets_panel);

	// Tab 2: Workbench
	workbench_panel = memnew(LibraryWorkbench);
	tabs->add_child(workbench_panel);

	// Connect assets to workbench
	assets_panel->set_workbench_inspector(workbench_panel->get_inspector());

	// Tab 3: CraftTable
	craft_panel = memnew(LibraryCraft);
	craft_panel->set_on_resource_created_callback(callable_mp(this, &Library::_on_resource_created));
	tabs->add_child(craft_panel);

	// Connect ResourceEditor to Workbench
	if (ResourceEditor::get_singleton()) {
		ResourceEditor::get_singleton()->connect("graph_node_selected", callable_mp(workbench_panel, &LibraryWorkbench::edit_object));
		ResourceEditor::get_singleton()->connect("graph_node_deselected", callable_mp(workbench_panel, &LibraryWorkbench::edit_object).bind((Object *)nullptr));
	}
}

Library::~Library() {
}
