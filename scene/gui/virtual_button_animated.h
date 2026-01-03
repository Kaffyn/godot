/**************************************************************************/
/*  virtual_button_animated.h                                             */
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

#ifndef VIRTUAL_BUTTON_ANIMATED_H
#define VIRTUAL_BUTTON_ANIMATED_H

#include "scene/gui/virtual_button.h"

class VirtualButtonAnimated : public VirtualButton {
	GDCLASS(VirtualButtonAnimated, VirtualButton);

public:
	enum FillMode {
		FILL_LEFT_TO_RIGHT,
		FILL_RIGHT_TO_LEFT,
		FILL_TOP_TO_BOTTOM,
		FILL_BOTTOM_TO_TOP,
	};

private:
	float value = 1.0f;
	FillMode fill_mode = FILL_LEFT_TO_RIGHT;
	bool show_progress = true;

	struct ThemeCache {
		Ref<StyleBox> progress_style;
		Color progress_color = Color(1, 1, 1, 0.3);
	} theme_cache;

protected:
	virtual void _update_theme_item_cache() override;
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_value(float p_value);
	float get_value() const;

	void set_fill_mode(FillMode p_mode);
	FillMode get_fill_mode() const;

	void set_show_progress(bool p_show);
	bool is_showing_progress() const;

	VirtualButtonAnimated();
};

VARIANT_ENUM_CAST(VirtualButtonAnimated::FillMode);

#endif // VIRTUAL_BUTTON_ANIMATED_H
