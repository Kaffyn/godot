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
#include "editor/themes/editor_scale.h"
#include "scene/resources/texture.h"

void ResourceNodePreview::set_resource(const Ref<Resource> &p_resource) {
	resource = p_resource;

	if (resource.is_null()) {
		preview_rect->set_texture(Ref<Texture2D>());
		return;
	}

	// Quick check if it's a texture
	Ref<Texture2D> tex = resource;
	if (tex.is_valid()) {
		preview_rect->set_texture(tex);
	} else {
		// Fallback to class icon
		preview_rect->set_texture(EditorNode::get_singleton()->get_object_icon(resource.ptr(), "Object"));
	}
}

ResourceNodePreview::ResourceNodePreview() {
	set_custom_minimum_size(Size2(0, 64) * EDSCALE);
	set_alignment(ALIGNMENT_CENTER);

	preview_rect = memnew(TextureRect);
	preview_rect->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	preview_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	preview_rect->set_custom_minimum_size(Size2(64, 64) * EDSCALE);
	add_child(preview_rect);
}
