/**************************************************************************/
/*  lss_root.cpp                                                          */
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

#include "lss_root.h"
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#include "scene/resources/universe_state.h"

#include "servers/lss_server.h"

void LSSRoot::set_universe(const Ref<UniverseState> &p_universe) {
	current_universe = p_universe;
}

Ref<UniverseState> LSSRoot::get_universe() const {
	return current_universe;
}

void LSSRoot::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			set_process_internal(true);
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			LSSServer::get_singleton()->update();
		} break;
	}
}

void LSSRoot::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_universe", "universe"), &LSSRoot::set_universe);
	ClassDB::bind_method(D_METHOD("get_universe"), &LSSRoot::get_universe);

	ClassDB::bind_method(D_METHOD("get_viewport"), &LSSRoot::get_viewport);
	ClassDB::bind_method(D_METHOD("get_container"), &LSSRoot::get_container);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "universe", PROPERTY_HINT_RESOURCE_TYPE, "UniverseState"), "set_universe", "get_universe");
}

LSSRoot::LSSRoot() {
	container = memnew(SubViewportContainer);
	container->set_name("LSSContainer");
	container->set_stretch(true);
	container->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	// In the future, we might want to set anchors/size if LSSRoot is added to a Control
	add_child(container, false, INTERNAL_MODE_BACK);

	viewport = memnew(SubViewport);
	viewport->set_name("LSSViewport");
	viewport->set_handle_input_locally(false);
	container->add_child(viewport, false, INTERNAL_MODE_BACK);
}
