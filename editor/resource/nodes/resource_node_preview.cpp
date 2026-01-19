/**************************************************************************/
/*  resource_node_preview.cpp                                             */
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

#include "resource_node_preview.h"
#include "editor/editor_node.h"
#include "editor/inspector/editor_resource_preview.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/color_rect.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/resources/texture.h"

void ResourceNodePreview::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_preview_ready", "path", "preview", "small_preview", "userdata"), &ResourceNodePreview::_preview_ready);
}

void ResourceNodePreview::set_resource(const Ref<Resource> &p_resource) {
	resource = p_resource;

	if (resource.is_null()) {
		preview_rect->set_texture(Ref<Texture2D>());
		hide();
		return;
	}
	show();

	String path = resource->get_path();
	if (!path.is_empty()) {
		// Request async preview
		EditorResourcePreview::get_singleton()->queue_resource_preview(path, this, "_preview_ready", path);
	} else {
		// Fallback for unsaved resources or internal logic
		Ref<Texture2D> tex = resource;
		if (tex.is_valid()) {
			preview_rect->set_texture(tex);
		} else {
			preview_rect->set_texture(EditorNode::get_singleton()->get_object_icon(resource.ptr(), "Object"));
		}
	}
}

void ResourceNodePreview::_preview_ready(const String &p_path, const Ref<Texture2D> &p_preview, const Ref<Texture2D> &p_small_preview, const Variant &p_userdata) {
	if (resource.is_null() || resource->get_path() != p_path) {
		return; // Stale request
	}

	if (p_preview.is_valid()) {
		preview_rect->set_texture(p_preview);
	} else {
		// Failed to generate preview, use icon
		preview_rect->set_texture(EditorNode::get_singleton()->get_object_icon(resource.ptr(), "Object"));
	}
}

ResourceNodePreview::ResourceNodePreview() {
	set_custom_minimum_size(Size2(0, 96) * EDSCALE); // Slightly larger for better visibility
	set_alignment(ALIGNMENT_CENTER);

	// Add a background for transparency checking
	ColorRect *bg = memnew(ColorRect);
	bg->set_color(Color(0.1, 0.1, 0.1, 0.5)); // Dark bg
	bg->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(bg);
	// Move bg to back? VBoxContainer stacks children. We need a container if we want layering.
	// Let's switch to PanelContainer or use a MarginContainer with a stylebox later.
	// For now, simpler: Put TextureRect inside a CenterContainer inside a Panel.

	remove_child(bg);
	memdelete(bg); // Clean up trying to hack VBox.

	// Better structure:
	// VBox
	//   PanelContainer (with checkerboard style)
	//     TextureRect

	PanelContainer *panel = memnew(PanelContainer);
	panel->set_h_size_flags(SIZE_EXPAND_FILL);
	panel->set_v_size_flags(SIZE_EXPAND_FILL);
	panel->set_custom_minimum_size(Size2(0, 96) * EDSCALE);
	add_child(panel);

	preview_rect = memnew(TextureRect);
	preview_rect->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	preview_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	panel->add_child(preview_rect);
}
