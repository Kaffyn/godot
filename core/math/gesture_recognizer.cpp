/**************************************************************************/
/*  gesture_recognizer.cpp                                                */
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

#include "gesture_recognizer.h"

#include "core/os/time.h"

// --- Pure Mathematical API (Stateless) ---

Dictionary GestureRecognizer::recognize(const PackedVector2Array &p_points) {
	if (p_points.size() < 3) {
		Dictionary d;
		d["type"] = GESTURE_UNKNOWN;
		d["confidence"] = 0.0f;
		return d;
	}

	Vector<Vector2> points;
	for (int i = 0; i < p_points.size(); i++) {
		points.push_back(p_points[i]);
	}

	return _recognize_internal(points, 1);
}

Dictionary GestureRecognizer::recognize_multistroke(const TypedArray<PackedVector2Array> &p_strokes) {
	Vector<Vector<Vector2>> strokes;
	for (int i = 0; i < p_strokes.size(); i++) {
		PackedVector2Array pa = p_strokes[i];
		Vector<Vector2> s;
		for (int j = 0; j < pa.size(); j++) {
			s.push_back(pa[j]);
		}
		if (!s.is_empty()) {
			strokes.push_back(s);
		}
	}

	if (strokes.is_empty()) {
		Dictionary d;
		d["type"] = GESTURE_UNKNOWN;
		d["confidence"] = 0.0f;
		return d;
	}

	Vector<Vector2> combined = _combine_strokes(strokes);
	return _recognize_internal(combined, strokes.size());
}

Dictionary GestureRecognizer::_recognize_internal(const Vector<Vector2> &p_points, int p_stroke_count) {
	GestureResult result;
	result.start_position = p_points[0];
	result.end_position = p_points[p_points.size() - 1];

	// Preprocess candidate
	Vector<Vector2> resampled = _resample(p_points, resample_points);

	// Normal Transformation
	Vector<Vector2> processed;
	if (rotation_invariant) {
		processed = _rotate_to_zero(resampled);
	} else {
		processed = resampled;
	}
	processed = _translate_to_origin(_scale_to_square(processed, 250.0f));

	// Feature Extraction
	GestureFeatures candidate_feats = _extract_features(resampled, p_stroke_count);

	float best_b = Math::INF;
	GestureType best_t = GESTURE_UNKNOWN;

	for (int i = 0; i < templates.size(); i++) {
		// 1. FAST FILTERS (O(1))
		if (templates[i].features.stroke_count != p_stroke_count) continue;

		// 2. FEATURE MATCHING (O(N) or O(1))
		// Use direction histogram and curvature to prune expensive $Q
		float feat_score = _match_features(candidate_feats, templates[i].features);
		if (feat_score < 0.4f) continue; // Prune if features are too different

		// 3. CLOUD MATCH (O(N^2))
		float ar_penalty = 0.0f;
		if (use_aspect_ratio_check) {
			float r = (candidate_feats.aspect_ratio > templates[i].features.aspect_ratio) ?
						(candidate_feats.aspect_ratio / templates[i].features.aspect_ratio) :
						(templates[i].features.aspect_ratio / candidate_feats.aspect_ratio);

			// Non-linear Gaussian penalty: exp(-((r-1)^2) / sigma)
			// r=1.0 -> penalty=0 (scale=1.0), r=2.0 -> penalty high.
			// Here we use it to scale distance (or just subtract from confidence)
			// Let's stick to a distance penalty for the $Q part:
			ar_penalty = (r - 1.0f) * 10.0f * aspect_ratio_sensitivity;
		}

		float d = _cloud_distance(processed, templates[i].lut_points, best_b + ar_penalty);
		if (d + ar_penalty < best_b) {
			best_b = d + ar_penalty;
			best_t = templates[i].type;
		}
	}

	float confidence = 0.0f;
	if (best_t != GESTURE_UNKNOWN) {
		confidence = CLAMP(1.0f - (best_b / 50.0f), 0.0f, 1.0f);
	}

	Dictionary d;
	d["type"] = best_t;
	d["confidence"] = (confidence >= min_confidence) ? confidence : 0.0f;
	d["start_position"] = result.start_position;
	d["end_position"] = result.end_position;
	return d;
}

GestureRecognizer::GestureType GestureRecognizer::get_swipe_direction(const Vector2 &p_start, const Vector2 &p_end, float p_min_distance) {
	Vector2 delta = p_end - p_start;
	float dist = delta.length();
	if (dist < p_min_distance) return GESTURE_UNKNOWN;

	float angle = delta.angle(); // Radians
	if (angle < 0) angle += Math::TAU;

	float sector = Math::TAU / 8.0f;
	int direction = int((angle + sector / 2.0f) / sector) % 8;

	switch (direction) {
		case 0: return GESTURE_SWIPE_RIGHT;
		case 1: return GESTURE_SWIPE_DOWN_RIGHT;
		case 2: return GESTURE_SWIPE_DOWN;
		case 3: return GESTURE_SWIPE_DOWN_LEFT;
		case 4: return GESTURE_SWIPE_LEFT;
		case 5: return GESTURE_SWIPE_UP_LEFT;
		case 6: return GESTURE_SWIPE_UP;
		case 7: return GESTURE_SWIPE_UP_RIGHT;
		default: return GESTURE_UNKNOWN;
	}
}

// --- $Q / Cloud Match Helpers ---

Vector<Vector2> GestureRecognizer::_combine_strokes(const Vector<Vector<Vector2>> &p_strokes) const {
	Vector<Vector2> combined;
	for (int i = 0; i < p_strokes.size(); i++) {
		for (int j = 0; j < p_strokes[i].size(); j++) {
			combined.push_back(p_strokes[i][j]);
		}
	}
	return combined;
}

// Greedy Cloud Match (O(n^2) but n is small ~32)
// Matches two point clouds regardless of order or direction.
float GestureRecognizer::_cloud_distance(const Vector<Vector2> &p_candidate, const Vector<Vector2> &p_template, float p_min_so_far) {
	int n = p_candidate.size();
	float limit = p_min_so_far * n; // Convert average limit to sum limit

	// Step 1: Sum min distances from Candidate to Template
	float sum1 = 0.0f;
	for (int i = 0; i < n; i++) {
		float min_d = Math::INF;
		for (int j = 0; j < n; j++) {
			float d = p_candidate[i].distance_squared_to(p_template[j]); // Squared for speed
			if (d < min_d) min_d = d;
		}
		sum1 += Math::sqrt(min_d);
		if (sum1 > limit) return Math::INF; // Early exit
	}

	// Step 2: Sum min distances from Template to Candidate
	float sum2 = 0.0f;
	for (int i = 0; i < n; i++) {
		float min_d = Math::INF;
		for (int j = 0; j < n; j++) {
			float d = p_template[i].distance_squared_to(p_candidate[j]);
			if (d < min_d) min_d = d;
		}
		sum2 += Math::sqrt(min_d);
		if (sum2 > limit) return Math::INF; // Early exit
	}

	return MIN(sum1, sum2) / n;
}

// --- Standard Pipeline ---

Vector<Vector2> GestureRecognizer::_resample(const Vector<Vector2> &p_points, int p_n) {
	float interval = _path_length(p_points) / (p_n - 1);
	float D = 0.0f;
	Vector<Vector2> new_points;
	new_points.push_back(p_points[0]);

	Vector<Vector2> working_points = p_points;

	for (int i = 1; i < working_points.size(); i++) {
		float d = working_points[i - 1].distance_to(working_points[i]);
		if ((D + d) >= interval) {
			float qx = working_points[i - 1].x + ((interval - D) / d) * (working_points[i].x - working_points[i - 1].x);
			float qy = working_points[i - 1].y + ((interval - D) / d) * (working_points[i].y - working_points[i - 1].y);
			Vector2 q(qx, qy);
			new_points.push_back(q);
			working_points.insert(i, q);
			D = 0.0f;
		} else {
			D += d;
		}
	}

	if (new_points.size() == p_n - 1) {
		new_points.push_back(working_points[working_points.size() - 1]);
	}
	return new_points;
}

Vector<Vector2> GestureRecognizer::_rotate_to_zero(const Vector<Vector2> &p_points) {
	Vector2 c = _centroid(p_points);
	float theta = Math::atan2(c.y - p_points[0].y, c.x - p_points[0].x);
	return _rotate_by(p_points, -theta);
}

Vector<Vector2> GestureRecognizer::_rotate_by(const Vector<Vector2> &p_points, float p_theta) {
	Vector2 c = _centroid(p_points);
	float cos_theta = Math::cos(p_theta);
	float sin_theta = Math::sin(p_theta);
	Vector<Vector2> new_points;

	for (int i = 0; i < p_points.size(); i++) {
		float qx = (p_points[i].x - c.x) * cos_theta - (p_points[i].y - c.y) * sin_theta + c.x;
		float qy = (p_points[i].x - c.x) * sin_theta + (p_points[i].y - c.y) * cos_theta + c.y;
		new_points.push_back(Vector2(qx, qy));
	}

	return new_points;
}

Vector<Vector2> GestureRecognizer::_scale_to_square(const Vector<Vector2> &p_points, float p_size) {
	if (p_points.is_empty()) return p_points;
	Rect2 bounds(p_points[0], Size2());
	for (int i = 1; i < p_points.size(); i++) {
		bounds.expand_to(p_points[i]);
	}

	Vector<Vector2> new_points;
	float w = MAX(0.001f, bounds.size.width);
	float h = MAX(0.001f, bounds.size.height);
	for (int i = 0; i < p_points.size(); i++) {
		float qx = p_points[i].x * (p_size / w);
		float qy = p_points[i].y * (p_size / h);
		new_points.push_back(Vector2(qx, qy));
	}
	return new_points;
}

Vector<Vector2> GestureRecognizer::_translate_to_origin(const Vector<Vector2> &p_points) {
	Vector2 c = _centroid(p_points);
	Vector<Vector2> new_points;
	for (int i = 0; i < p_points.size(); i++) {
		new_points.push_back(p_points[i] - c);
	}
	return new_points;
}

// --- Feature Extraction ---

GestureRecognizer::GestureFeatures GestureRecognizer::_extract_features(const Vector<Vector2> &p_points, int p_stroke_count) {
	GestureFeatures f;
	f.aspect_ratio = _calculate_aspect_ratio(p_points);
	f.path_length = _path_length(p_points);
	f.stroke_count = p_stroke_count;
	_calculate_direction_histogram(p_points, f.direction_hist);
	f.curvature = _calculate_curvature(p_points);
	return f;
}

void GestureRecognizer::_calculate_direction_histogram(const Vector<Vector2> &p_points, float r_hist[8]) {
	for (int i = 0; i < 8; i++) r_hist[i] = 0.0f;
	if (p_points.size() < 2) return;

	for (int i = 1; i < p_points.size(); i++) {
		Vector2 d = p_points[i] - p_points[i - 1];
		if (d.length_squared() < 0.0001f) continue;
		float angle = d.angle();
		if (angle < 0) angle += Math::TAU;

		float sector = Math::TAU / 8.0f;
		int bin = int((angle + sector / 2.0f) / sector) % 8;
		r_hist[bin] += d.length();
	}

	// Normalize
	float total = 0.0f;
	for (int i = 0; i < 8; i++) total += r_hist[i];
	if (total > 0) {
		for (int i = 0; i < 8; i++) r_hist[i] /= total;
	}
}

float GestureRecognizer::_calculate_curvature(const Vector<Vector2> &p_points) {
	if (p_points.size() < 3) return 0.0f;
	float total_curvature = 0.0f;
	for (int i = 1; i < p_points.size() - 1; i++) {
		Vector2 v1 = (p_points[i] - p_points[i - 1]).normalized();
		Vector2 v2 = (p_points[i + 1] - p_points[i]).normalized();
		total_curvature += Math::abs(v1.angle_to(v2));
	}
	return total_curvature / p_points.size();
}

float GestureRecognizer::_match_features(const GestureFeatures &p_candidate, const GestureFeatures &p_template) {
	// Histogram similarity (Dot product for normalized histograms)
	float hist_sim = 0.0f;
	for (int i = 0; i < 8; i++) {
		hist_sim += p_candidate.direction_hist[i] * p_template.direction_hist[i];
	}

	// Curvature similarity (Inverse difference)
	float curv_sim = 1.0f - MIN(1.0f, Math::abs(p_candidate.curvature - p_template.curvature) * 2.0f);

	return (hist_sim * 0.7f) + (curv_sim * 0.3f);
}

float GestureRecognizer::_path_distance(const Vector<Vector2> &p_a, const Vector<Vector2> &p_b) {
	float d = 0.0f;
	int count = MIN(p_a.size(), p_b.size());
	if (count == 0) return Math::INF;
	for (int i = 0; i < count; i++) {
		d += p_a[i].distance_to(p_b[i]);
	}
	return d / count;
}

Vector2 GestureRecognizer::_centroid(const Vector<Vector2> &p_points) {
	Vector2 c;
	if (p_points.is_empty()) return c;
	for (int i = 0; i < p_points.size(); i++) {
		c += p_points[i];
	}
	return c / p_points.size();
}

float GestureRecognizer::_path_length(const Vector<Vector2> &p_points) {
	float d = 0.0f;
	for (int i = 1; i < p_points.size(); i++) {
		d += p_points[i - 1].distance_to(p_points[i]);
	}
	return d;
}

// --- Dynamic API ---

// --- Aspect Ratio Logic ---

float GestureRecognizer::_calculate_aspect_ratio(const Vector<Vector2> &p_points) {
	if (p_points.is_empty()) return 1.0f;
	Rect2 bounds(p_points[0], Size2());
	for (int i = 1; i < p_points.size(); i++) {
		bounds.expand_to(p_points[i]);
	}
	float w = MAX(0.001f, bounds.size.width);
	float h = MAX(0.001f, bounds.size.height);
	return w / h;
}

// ...

void GestureRecognizer::_add_template(GestureType p_type, const Vector<Vector<Vector2>> &p_strokes) {
	Vector<Vector2> combined = _combine_strokes(p_strokes);
	if (combined.is_empty()) return;

	// Pre-process template exactly like candidate (Resample -> Scale -> Translate)
	Vector<Vector2> resampled = _resample(combined, resample_points);

	Template t;
	t.type = p_type;
	t.strokes = p_strokes;
	t.features = _extract_features(resampled, p_strokes.size());

	// For the LUT points (used in $Q), we also need normal transformation
	Vector<Vector2> processed = resampled;
	// When creating templates, we assume they are "base" shapes (unrotated if needed)
	// But if rotation_invariant is true, we must store the normalized (rotated to zero) version.
	if (rotation_invariant) {
		processed = _rotate_to_zero(processed);
	}
	t.lut_points = _translate_to_origin(_scale_to_square(processed, 250.0f));

	templates.push_back(t);
}

// ...



// ...

void GestureRecognizer::set_rotation_invariant(bool p_enable) {
	rotation_invariant = p_enable;
}

bool GestureRecognizer::is_rotation_invariant() const {
	return rotation_invariant;
}

void GestureRecognizer::set_use_aspect_ratio_check(bool p_enable) {
	use_aspect_ratio_check = p_enable;
}

bool GestureRecognizer::is_using_aspect_ratio_check() const {
	return use_aspect_ratio_check;
}

void GestureRecognizer::set_aspect_ratio_sensitivity(float p_sensitivity) {
	aspect_ratio_sensitivity = p_sensitivity;
}

float GestureRecognizer::get_aspect_ratio_sensitivity() const {
	return aspect_ratio_sensitivity;
}

// ... BINDINGS ...

void GestureRecognizer::create_template(int p_type, const PackedVector2Array &p_points) {
	Vector<Vector<Vector2>> strokes;
	Vector<Vector2> s;
	for(int i=0; i<p_points.size(); i++) s.push_back(p_points[i]);
	strokes.push_back(s);
	_add_template((GestureRecognizer::GestureType)p_type, strokes);
}

void GestureRecognizer::create_multistroke_template(int p_type, const TypedArray<PackedVector2Array> &p_strokes) {
	Vector<Vector<Vector2>> strokes;
	for(int i=0; i<p_strokes.size(); i++) {
		PackedVector2Array pa = p_strokes[i];
		Vector<Vector2> s;
		for(int j=0; j<pa.size(); j++) s.push_back(pa[j]);
		strokes.push_back(s);
	}
	_add_template((GestureType)p_type, strokes);
}

void GestureRecognizer::remove_template(int p_type) {
	for(int i=0; i<templates.size(); i++) {
		if(templates[i].type == (GestureRecognizer::GestureType)p_type) {
			templates.remove_at(i);
			i--;
		}
	}
}

void GestureRecognizer::set_min_confidence(float p_confidence) {
	min_confidence = CLAMP(p_confidence, 0.0f, 1.0f);
}

float GestureRecognizer::get_min_confidence() const {
	return min_confidence;
}

void GestureRecognizer::set_resample_points(int p_points) {
	resample_points = MAX(8, p_points);
}

int GestureRecognizer::get_resample_points() const {
	return resample_points;
}

// --- Bindings ---

void GestureRecognizer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("recognize", "points"), &GestureRecognizer::recognize);
	ClassDB::bind_method(D_METHOD("recognize_multistroke", "strokes"), &GestureRecognizer::recognize_multistroke);

	ClassDB::bind_static_method("GestureRecognizer", D_METHOD("get_swipe_direction", "start", "end", "min_distance"), &GestureRecognizer::get_swipe_direction);

	ClassDB::bind_method(D_METHOD("create_template", "type", "points"), &GestureRecognizer::create_template);
	ClassDB::bind_method(D_METHOD("create_multistroke_template", "type", "strokes"), &GestureRecognizer::create_multistroke_template);
	ClassDB::bind_method(D_METHOD("remove_template", "type"), &GestureRecognizer::remove_template);

	ClassDB::bind_method(D_METHOD("set_rotation_invariant", "enable"), &GestureRecognizer::set_rotation_invariant);
	ClassDB::bind_method(D_METHOD("is_rotation_invariant"), &GestureRecognizer::is_rotation_invariant);

	ClassDB::bind_method(D_METHOD("set_use_aspect_ratio_check", "enable"), &GestureRecognizer::set_use_aspect_ratio_check);
	ClassDB::bind_method(D_METHOD("is_using_aspect_ratio_check"), &GestureRecognizer::is_using_aspect_ratio_check);

	ClassDB::bind_method(D_METHOD("set_aspect_ratio_sensitivity", "sensitivity"), &GestureRecognizer::set_aspect_ratio_sensitivity);
	ClassDB::bind_method(D_METHOD("get_aspect_ratio_sensitivity"), &GestureRecognizer::get_aspect_ratio_sensitivity);

	ClassDB::bind_method(D_METHOD("set_min_confidence", "confidence"), &GestureRecognizer::set_min_confidence);
	ClassDB::bind_method(D_METHOD("get_min_confidence"), &GestureRecognizer::get_min_confidence);

	ClassDB::bind_method(D_METHOD("set_resample_points", "points"), &GestureRecognizer::set_resample_points);
	ClassDB::bind_method(D_METHOD("get_resample_points"), &GestureRecognizer::get_resample_points);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rotation_invariant"), "set_rotation_invariant", "is_rotation_invariant");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_aspect_ratio_check"), "set_use_aspect_ratio_check", "is_using_aspect_ratio_check");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aspect_ratio_sensitivity"), "set_aspect_ratio_sensitivity", "get_aspect_ratio_sensitivity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_confidence", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_min_confidence", "get_min_confidence");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "resample_points", PROPERTY_HINT_RANGE, "8,128,1"), "set_resample_points", "get_resample_points");

	// Enums
	BIND_ENUM_CONSTANT(GESTURE_UNKNOWN);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_LEFT);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_RIGHT);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_UP);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_DOWN);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_UP_LEFT);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_UP_RIGHT);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_DOWN_LEFT);
	BIND_ENUM_CONSTANT(GESTURE_SWIPE_DOWN_RIGHT);
	BIND_ENUM_CONSTANT(GESTURE_CIRCLE_CLOCKWISE);
	BIND_ENUM_CONSTANT(GESTURE_CIRCLE_COUNTER_CLOCKWISE);
	BIND_ENUM_CONSTANT(GESTURE_TRIANGLE);
	BIND_ENUM_CONSTANT(GESTURE_SQUARE);
	BIND_ENUM_CONSTANT(GESTURE_CHECK_MARK);
	BIND_ENUM_CONSTANT(GESTURE_X_MARK);
	BIND_ENUM_CONSTANT(GESTURE_ZIGZAG);
	BIND_ENUM_CONSTANT(GESTURE_RECTANGLE);
	BIND_ENUM_CONSTANT(GESTURE_ELLIPSE);
	BIND_ENUM_CONSTANT(GESTURE_HEXAGON);
	BIND_ENUM_CONSTANT(GESTURE_PENTAGON);
	BIND_ENUM_CONSTANT(GESTURE_STAR);
	BIND_ENUM_CONSTANT(GESTURE_DIAMOND);

	BIND_ENUM_CONSTANT(GESTURE_LETTER_A);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_B);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_C);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_D);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_E);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_F);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_G);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_H);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_I);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_J);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_K);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_L);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_M);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_N);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_O);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_P);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_Q);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_R);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_S);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_T);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_U);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_V);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_W);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_X);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_Y);
	BIND_ENUM_CONSTANT(GESTURE_LETTER_Z);

	BIND_ENUM_CONSTANT(GESTURE_DIGIT_0);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_1);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_2);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_3);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_4);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_5);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_6);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_7);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_8);
	BIND_ENUM_CONSTANT(GESTURE_DIGIT_9);
}

GestureRecognizer::GestureRecognizer() {
	// Initialize templates using create_template (wraps to internal logic)
	// Geometric Shapes
	// Circle CW
	Vector<Vector2> circle_cw;
	for (int i = 0; i < 32; i++) {
		float theta = (Math::TAU * i) / 32.0f;
		circle_cw.push_back(Vector2(Math::cos(theta), Math::sin(theta)) * 100.0f);
	}
	Vector<Vector<Vector2>> s_cw; s_cw.push_back(circle_cw);
	_add_template(GESTURE_CIRCLE_CLOCKWISE, s_cw);

	// Square
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(100,0)); v.push_back(Vector2(100,100)); v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_SQUARE, strokes);
	}

	// Triangle
	{
		Vector<Vector2> v;
		v.push_back(Vector2(50,0)); v.push_back(Vector2(100,100)); v.push_back(Vector2(0,100)); v.push_back(Vector2(50,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_TRIANGLE, strokes);
	}

	// Check
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,50)); v.push_back(Vector2(40,100)); v.push_back(Vector2(100,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_CHECK_MARK, strokes);
	}

	// X Mark (Multistroke Example!)
	{
		Vector<Vector<Vector2>> x_multi;
		Vector<Vector2> x1; x1.push_back(Vector2(0,0)); x1.push_back(Vector2(100,100));
		Vector<Vector2> x2; x2.push_back(Vector2(100,0)); x2.push_back(Vector2(0,100));
		x_multi.push_back(x1);
		x_multi.push_back(x2);
		_add_template(GESTURE_X_MARK, x_multi);
	}

	// Zigzag
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(0,100)); v.push_back(Vector2(50,150));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_ZIGZAG, strokes);
	}

	// Rectangle (Distinguished from Square by Aspect Ratio)
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(200,0)); v.push_back(Vector2(200,100)); v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_RECTANGLE, strokes);
	}

	// Ellipse (Distinguished from Circle by Aspect Ratio)
	{
		Vector<Vector2> v;
		for (int i = 0; i <= 32; i++) {
			float theta = (Math::TAU * i) / 32.0f;
			v.push_back(Vector2(100 + 100*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_ELLIPSE, strokes);
	}

	// Hexagon
	{
		Vector<Vector2> v;
		for (int i = 0; i <= 6; i++) {
			float theta = (Math::TAU * i) / 6.0f;
			v.push_back(Vector2(50 + 50*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_HEXAGON, strokes);
	}

	// Pentagon
	{
		Vector<Vector2> v;
		for (int i = 0; i <= 5; i++) {
			float theta = (Math::TAU * i) / 5.0f - Math::TAU/4.0f;
			v.push_back(Vector2(50 + 50*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_PENTAGON, strokes);
	}

	// Star
	{
		Vector<Vector2> v;
		for (int i = 0; i <= 10; i++) {
			float r = (i % 2 == 0) ? 50.0f : 20.0f;
			float theta = (Math::TAU * i) / 10.0f - Math::TAU/4.0f;
			v.push_back(Vector2(50 + r*Math::cos(theta), 50 + r*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_STAR, strokes);
	}

	// Diamond
	{
		Vector<Vector2> v;
		v.push_back(Vector2(50,0)); v.push_back(Vector2(100,50)); v.push_back(Vector2(50,100)); v.push_back(Vector2(0,50)); v.push_back(Vector2(50,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIAMOND, strokes);
	}

	// Alphabet Samples
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,100)); v.push_back(Vector2(50,0)); v.push_back(Vector2(100,100)); v.push_back(Vector2(50,50)); v.push_back(Vector2(0,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_A, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(50,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(0,50)); v.push_back(Vector2(50,100)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_B, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_C, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(50,25)); v.push_back(Vector2(50,75)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_D, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100)); v.push_back(Vector2(0,50)); v.push_back(Vector2(100,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_E, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(0,50)); v.push_back(Vector2(100,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_F, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,50)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100)); v.push_back(Vector2(100,50)); v.push_back(Vector2(50,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_G, strokes);
	}
	{
		Vector<Vector<Vector2>> strokes;
		Vector<Vector2> s1; s1.push_back(Vector2(0,0)); s1.push_back(Vector2(0,100));
		Vector<Vector2> s2; s2.push_back(Vector2(100,0)); s2.push_back(Vector2(100,100));
		Vector<Vector2> s3; s3.push_back(Vector2(0,50)); s3.push_back(Vector2(100,50));
		strokes.push_back(s1); strokes.push_back(s2); strokes.push_back(s3);
		_add_template(GESTURE_LETTER_H, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(50,0)); v.push_back(Vector2(50,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(100,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_I, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(50,100)); v.push_back(Vector2(0,75));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_J, strokes);
	}
	{
		Vector<Vector<Vector2>> strokes;
		Vector<Vector2> s1; s1.push_back(Vector2(0,0)); s1.push_back(Vector2(0,100));
		Vector<Vector2> s2; s2.push_back(Vector2(100,0)); s2.push_back(Vector2(0,50)); s2.push_back(Vector2(100,100));
		strokes.push_back(s1); strokes.push_back(s2);
		_add_template(GESTURE_LETTER_K, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_L, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(50,100)); v.push_back(Vector2(100,0)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_M, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(100,100)); v.push_back(Vector2(100,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_N, strokes);
	}
	{
		Vector<Vector2> v; // Same as circle but mapped to O
		for (int i = 0; i <= 32; i++) {
			float theta = (Math::TAU * i) / 32.0f;
			v.push_back(Vector2(50 + 50*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_O, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(50,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(0,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_P, strokes);
	}
	{
		Vector<Vector2> v; // Circle with tail
		for (int i = 0; i <= 32; i++) {
			float theta = (Math::TAU * i) / 32.0f;
			v.push_back(Vector2(50 + 50*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		v.push_back(Vector2(50,50)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_Q, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,100)); v.push_back(Vector2(0,0)); v.push_back(Vector2(50,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(0,50)); v.push_back(Vector2(50,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_R, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,25)); v.push_back(Vector2(100,75)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_S, strokes);
	}
	{
		Vector<Vector<Vector2>> strokes;
		Vector<Vector2> s1; s1.push_back(Vector2(0,0)); s1.push_back(Vector2(100,0));
		Vector<Vector2> s2; s2.push_back(Vector2(50,0)); s2.push_back(Vector2(50,100));
		strokes.push_back(s1); strokes.push_back(s2);
		_add_template(GESTURE_LETTER_T, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100)); v.push_back(Vector2(100,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_U, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(50,100)); v.push_back(Vector2(100,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_V, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(25,100)); v.push_back(Vector2(50,0)); v.push_back(Vector2(75,100)); v.push_back(Vector2(100,0));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_W, strokes);
	}
	{
		Vector<Vector<Vector2>> strokes;
		Vector<Vector2> s1; s1.push_back(Vector2(0,0)); s1.push_back(Vector2(100,100));
		Vector<Vector2> s2; s2.push_back(Vector2(100,0)); s2.push_back(Vector2(0,100));
		strokes.push_back(s1); strokes.push_back(s2);
		_add_template(GESTURE_LETTER_X, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(100,0)); v.push_back(Vector2(50,50)); v.push_back(Vector2(50,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_Y, strokes);
	}
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(100,0)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_LETTER_Z, strokes);
	}

	// Digits 0-9
	// 0 - Ellipse/Circle
	{
		Vector<Vector2> v;
		for (int i = 0; i <= 32; i++) {
			float theta = (Math::TAU * i) / 32.0f;
			v.push_back(Vector2(50 + 50*Math::cos(theta), 50 + 50*Math::sin(theta)));
		}
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_0, strokes);
	}
	// 1 - Line down
	{
		Vector<Vector2> v;
		v.push_back(Vector2(50,0)); v.push_back(Vector2(50,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_1, strokes);
	}
	// 2 - Top curve, diagonal down, bottom right
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,25)); v.push_back(Vector2(50,0)); v.push_back(Vector2(100,25)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_2, strokes);
	}
	// 3 - Two curves
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(100,25)); v.push_back(Vector2(50,50)); v.push_back(Vector2(100,75)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_3, strokes);
	}
	// 4 - Down, Right, Lift, Down (or single stroke style)
	// Multistroke 4 makes more sense
	{
		Vector<Vector<Vector2>> strokes;
		Vector<Vector2> s1; s1.push_back(Vector2(100,0)); s1.push_back(Vector2(0,75)); s1.push_back(Vector2(100,75));
		Vector<Vector2> s2; s2.push_back(Vector2(75,0)); s2.push_back(Vector2(75,100));
		strokes.push_back(s1); strokes.push_back(s2);
		_add_template(GESTURE_DIGIT_4, strokes);
	}
	// 5 - Horizontal, Down, Curve
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,0)); v.push_back(Vector2(0,50)); v.push_back(Vector2(100,50)); v.push_back(Vector2(100,100)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_5, strokes);
	}
	// 6 - C curve to loop
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,0)); v.push_back(Vector2(0,50)); v.push_back(Vector2(0,100)); v.push_back(Vector2(100,100)); v.push_back(Vector2(100,50)); v.push_back(Vector2(0,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_6, strokes);
	}
	// 7 - Right, diagonal down
	{
		Vector<Vector2> v;
		v.push_back(Vector2(0,0)); v.push_back(Vector2(100,0)); v.push_back(Vector2(0,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_7, strokes);
	}
	// 8 - Infinity / Snowman
	{
		Vector<Vector2> v;
		v.push_back(Vector2(50,50)); v.push_back(Vector2(100,0)); v.push_back(Vector2(50,0)); v.push_back(Vector2(0,50)); v.push_back(Vector2(50,100)); v.push_back(Vector2(100,50)); v.push_back(Vector2(50,50));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_8, strokes);
	}
	// 9 - Loop top, down
	{
		Vector<Vector2> v;
		v.push_back(Vector2(100,50)); v.push_back(Vector2(0,50)); v.push_back(Vector2(0,0)); v.push_back(Vector2(100,0)); v.push_back(Vector2(100,100));
		Vector<Vector<Vector2>> strokes; strokes.push_back(v);
		_add_template(GESTURE_DIGIT_9, strokes);
	}
}
