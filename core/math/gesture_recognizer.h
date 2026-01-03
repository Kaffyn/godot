/**************************************************************************/
/*  gesture_recognizer.h                                                  */
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

#ifndef GESTURE_RECOGNIZER_H
#define GESTURE_RECOGNIZER_H

#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"

class GestureRecognizer : public RefCounted {
	GDCLASS(GestureRecognizer, RefCounted);

public:
	enum GestureType {
		GESTURE_UNKNOWN,
		// Directional swipes (8 directions)
		GESTURE_SWIPE_LEFT,
		GESTURE_SWIPE_RIGHT,
		GESTURE_SWIPE_UP,
		GESTURE_SWIPE_DOWN,
		GESTURE_SWIPE_UP_LEFT,
		GESTURE_SWIPE_UP_RIGHT,
		GESTURE_SWIPE_DOWN_LEFT,
		GESTURE_SWIPE_DOWN_RIGHT,
		// Geometric shapes
		GESTURE_CIRCLE_CLOCKWISE,
		GESTURE_CIRCLE_COUNTER_CLOCKWISE,
		GESTURE_TRIANGLE,
		GESTURE_SQUARE,
		GESTURE_CHECK_MARK,
		GESTURE_X_MARK,
		GESTURE_ZIGZAG,
		GESTURE_RECTANGLE,
		GESTURE_ELLIPSE,
		GESTURE_HEXAGON,
		GESTURE_PENTAGON,
		GESTURE_STAR,
		GESTURE_DIAMOND,
		// Alphabet (A-Z)
		GESTURE_LETTER_A, GESTURE_LETTER_B, GESTURE_LETTER_C, GESTURE_LETTER_D, GESTURE_LETTER_E,
		GESTURE_LETTER_F, GESTURE_LETTER_G, GESTURE_LETTER_H, GESTURE_LETTER_I, GESTURE_LETTER_J,
		GESTURE_LETTER_K, GESTURE_LETTER_L, GESTURE_LETTER_M, GESTURE_LETTER_N, GESTURE_LETTER_O,
		GESTURE_LETTER_P, GESTURE_LETTER_Q, GESTURE_LETTER_R, GESTURE_LETTER_S, GESTURE_LETTER_T,
		GESTURE_LETTER_U, GESTURE_LETTER_V, GESTURE_LETTER_W, GESTURE_LETTER_X, GESTURE_LETTER_Y,
		GESTURE_LETTER_Z,
		// Numbers (0-9)
		GESTURE_DIGIT_0, GESTURE_DIGIT_1, GESTURE_DIGIT_2, GESTURE_DIGIT_3, GESTURE_DIGIT_4,
		GESTURE_DIGIT_5, GESTURE_DIGIT_6, GESTURE_DIGIT_7, GESTURE_DIGIT_8, GESTURE_DIGIT_9,
	};

	struct GestureResult {
		GestureType type = GESTURE_UNKNOWN;
		float confidence = 0.0f;
		Vector2 start_position;
		Vector2 end_position;
		float duration = 0.0f;
	};

	struct GestureFeatures {
		float aspect_ratio = 1.0f;
		float path_length = 0.0f;
		float curvature = 0.0f;
		float direction_hist[8] = { 0 };
		int stroke_count = 0;
	};

private:
	struct Template {
		GestureType type;
		Vector<Vector<Vector2>> strokes; // Multistroke support
		Vector<Vector2> lut_points;
		GestureFeatures features;
	};

	Vector<Template> templates;
	float min_confidence = 0.5f;
	int resample_points = 32;

	// Settings
	bool rotation_invariant = true; // If true, treats rotated shapes as the same
	bool use_aspect_ratio_check = true; // If true, penalizes matches with different aspect ratios
	float aspect_ratio_sensitivity = 1.5f; // Sensitivity of AR penalty

	// $Q Implementation Methods
	void _add_template(GestureType p_type, const Vector<Vector<Vector2>> &p_strokes);

	// Pipeline Helpers
	Dictionary _recognize_internal(const Vector<Vector2> &p_points, int p_stroke_count);
	Vector<Vector2> _combine_strokes(const Vector<Vector<Vector2>> &p_strokes) const;
	Vector<Vector2> _resample(const Vector<Vector2> &p_points, int p_n);
	Vector<Vector2> _rotate_to_zero(const Vector<Vector2> &p_points);
	Vector<Vector2> _rotate_by(const Vector<Vector2> &p_points, float p_theta);
	Vector<Vector2> _scale_to_square(const Vector<Vector2> &p_points, float p_size);
	Vector<Vector2> _translate_to_origin(const Vector<Vector2> &p_points);
	float _calculate_aspect_ratio(const Vector<Vector2> &p_points);

	// Feature Extraction
	GestureFeatures _extract_features(const Vector<Vector2> &p_points, int p_stroke_count);
	void _calculate_direction_histogram(const Vector<Vector2> &p_points, float r_hist[8]);
	float _calculate_curvature(const Vector<Vector2> &p_points);
	float _match_features(const GestureFeatures &p_candidate, const GestureFeatures &p_template);

	float _path_distance(const Vector<Vector2> &p_a, const Vector<Vector2> &p_b);
	float _path_length(const Vector<Vector2> &p_points);
	Vector2 _centroid(const Vector<Vector2> &p_points);

	// Cloud Match (Order Independent)
	float _cloud_distance(const Vector<Vector2> &p_candidate, const Vector<Vector2> &p_template, float p_min_so_far);

protected:
	static void _bind_methods();

public:
	// Pure Mathematical API (Stateless)
	Dictionary recognize(const PackedVector2Array &p_points);
	Dictionary recognize_multistroke(const TypedArray<PackedVector2Array> &p_strokes);

	// Utility static helpers
	static GestureType get_swipe_direction(const Vector2 &p_start, const Vector2 &p_end, float p_min_distance);

	void set_min_confidence(float p_confidence);
	float get_min_confidence() const;

	void set_resample_points(int p_points);
	int get_resample_points() const;

	// Dynamic Template API
	void create_template(int p_type, const PackedVector2Array &p_points);
	void create_multistroke_template(int p_type, const TypedArray<PackedVector2Array> &p_strokes);
	void remove_template(int p_type);

	void set_rotation_invariant(bool p_enable);
	bool is_rotation_invariant() const;

	void set_use_aspect_ratio_check(bool p_enable);
	bool is_using_aspect_ratio_check() const;

	void set_aspect_ratio_sensitivity(float p_sensitivity);
	float get_aspect_ratio_sensitivity() const;

	GestureRecognizer();
};

VARIANT_ENUM_CAST(GestureRecognizer::GestureType);

#endif // GESTURE_RECOGNIZER_H
