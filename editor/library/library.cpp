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

#include "scene/gui/label.h"

Library::Library() {
	tabs = memnew(TabContainer);
	tabs->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(tabs);

	// Tab 1: Assets
	VBoxContainer *assets_tab = memnew(VBoxContainer);
	assets_tab->set_name("Assets");
	Label *l1 = memnew(Label);
	l1->set_text("Assets Browser (TODO)");
	assets_tab->add_child(l1);
	tabs->add_child(assets_tab);

	// Tab 2: Workbench
	VBoxContainer *workbench_tab = memnew(VBoxContainer);
	workbench_tab->set_name("Workbench");
	Label *l2 = memnew(Label);
	l2->set_text("Workbench Inspector (TODO)");
	workbench_tab->add_child(l2);
	tabs->add_child(workbench_tab);

	// Tab 3: CraftTable
	VBoxContainer *craft_tab = memnew(VBoxContainer);
	craft_tab->set_name("CraftTable");
	Label *l3 = memnew(Label);
	l3->set_text("Creation Wizards (TODO)");
	craft_tab->add_child(l3);
	tabs->add_child(craft_tab);
}

Library::~Library() {
}
