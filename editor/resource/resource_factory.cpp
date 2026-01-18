/**************************************************************************/
/*  resource_factory.cpp                                                  */
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

#include "resource_factory.h"
#include "core/object/class_db.h"
#include "editor/editor_node.h"
#include "resource_server.h"

Ref<Resource> ResourceFactory::create_resource_from_domain(const StringName &p_domain_name) {
	if (!ResourceServer::get_singleton()) {
		ERR_PRINT("ResourceServer singleton is not available.");
		return Ref<Resource>();
	}

	Dictionary info = ResourceServer::get_singleton()->get_domain_info(p_domain_name);
	String resource_class_name = info.get("resource_type", "");

	if (resource_class_name.is_empty()) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Resource domain '%s' not found or not registered correctly."), p_domain_name));
		return Ref<Resource>();
	}

	return create_resource_by_class(resource_class_name);
}

Ref<Resource> ResourceFactory::create_resource_by_class(const StringName &p_class_name) {
	if (!ClassDB::class_exists(p_class_name)) {
		ERR_PRINT(vformat("Class '%s' does not exist in ClassDB.", p_class_name));
		return Ref<Resource>();
	}
	if (!ClassDB::is_parent_class(p_class_name, "Resource")) {
		ERR_PRINT(vformat("Class '%s' does not inherit from Resource.", p_class_name));
		return Ref<Resource>();
	}
	if (!ClassDB::can_instantiate(p_class_name)) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Class '%s' cannot be instantiated. Check if it's abstract or if it requires specific setup."), p_class_name));
		return Ref<Resource>();
	}

	Object *obj = ClassDB::instantiate(p_class_name);
	if (!obj) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Failed to instantiate class '%s'."), p_class_name));
		return Ref<Resource>();
	}

	Resource *res = Object::cast_to<Resource>(obj);
	if (!res) {
		memdelete(obj); // Clean up if cast fails
		EditorNode::get_singleton()->show_warning(vformat(TTR("Instantiated object for class '%s' is not a Resource."), p_class_name));
		return Ref<Resource>();
	}

	Ref<Resource> resource = res;
	return resource;
}
