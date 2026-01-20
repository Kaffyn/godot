/**************************************************************************/
/*  resource_graph_node.cpp                                               */
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

#include "resource_graph_node.h"
#include "resource_node_preview.h"
#include "resource_node_property.h"
#include "scene/gui/separator.h"

void ResourceGraphNode::set_resource(const Ref<Resource> &p_resource) {
	resource = p_resource;

	// Clear existing children
	for (int i = get_child_count() - 1; i >= 0; i--) {
		Node *child = get_child(i);
		remove_child(child);
		memdelete(child);
	}
	clear_all_slots();

	if (resource.is_null()) {
		set_title("Null");
		return;
	}

	set_title(resource->get_class());

	// Add Preview
	ResourceNodePreview *preview = memnew(ResourceNodePreview);
	preview->set_resource(resource);
	add_child(preview);
	// Slot 0 for preview? No, let's keep it clean for now.

	// Separator
	add_child(memnew(HSeparator));

	List<PropertyInfo> props;
	resource->get_property_list(&props);

	// Adjust slot index because of preview and separator children (GraphNode slots are mapped to children indices usually, but set_slot takes index)
	// Actually set_slot index corresponds to child index.
	// Child 0: Preview
	// Child 1: Separator
	// Properties start at Child 2.
	int child_idx = 2;

	for (const PropertyInfo &E : props) {
		// Filter exactly like EditorInspector
		if (!(E.usage & PROPERTY_USAGE_EDITOR)) {
			continue;
		}
		if (E.name == "script" || E.name == "resource_name" || E.name == "resource_path" || E.name == "resource_local_to_scene" || E.name.begins_with("metadata/_")) {
			continue;
		}

		ResourceNodeProperty *prop_row = memnew(ResourceNodeProperty);
		prop_row->set_property(resource, E.name, resource->get(E.name));
		add_child(prop_row);
		// Enable right slot for Resources (output)
		if (E.type == Variant::OBJECT) {
			set_slot(child_idx, false, 0, Color(1, 1, 1), true, 0, Color(0, 1, 0));
		}
		child_idx++;
	}
}
Ref<Resource> ResourceGraphNode::get_resource() const {
	return resource;
}

ResourceGraphNode::ResourceGraphNode() {
}
