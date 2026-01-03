extends Control

var score = 0
var target_gesture = -1
var target_name = ""

@onready var label_instruction = $VBox/LabelInstruction
@onready var label_score = $VBox/LabelScore
@onready var label_feedback = $LabelFeedback
@onready var timer = $Timer

var gesture_pool = []

func _ready():
	randomize()
	# Populate pool with all supported gestures
	for i in range(GestureRecognizer.GESTURE_SWIPE_LEFT, GestureRecognizer.GESTURE_DIGIT_9 + 1):
		gesture_pool.append(i)
	pick_new_target()

func pick_new_target():
	target_gesture = gesture_pool[randi() % gesture_pool.size()]
	target_name = _get_name(target_gesture)
	label_instruction.text = "Task: " + target_name
	timer.start(8.0) # 8 seconds per task

func _process(_delta):
	# Using Action System for Gestures
	if target_gesture != -1:
		if Input.is_action_just_pressed("cast_spell"):
			score += 1
			_show_feedback("CORRECT! +1", Color.GREEN)
			pick_new_target()

func _input(event):
	if event is InputEventVirtualGesture:
		var gesture_name = _get_name(event.gesture_type)
		print("[GESTURE] Received: %s (ID: %d, Confidence: %.2f)" % [gesture_name, event.gesture_type, event.value])
		
		# Log Raw Traço (for AI analysis)
		if event.points.size() > 0:
			print("[TELEMETRY] Stroke Points: ", event.points)
		
		if event.gesture_type == target_gesture:
			score += 1
			_show_feedback("CORRECT! +1", Color.GREEN)
			pick_new_target()
		elif event.gesture_type == GestureRecognizer.GESTURE_UNKNOWN:
			# Not recognized by core
			_show_feedback("NOT RECOGNIZED (Check Console)", Color.YELLOW)
		else:
			# Recognized as wrong pattern
			score -= 5
			_show_feedback("WRONG! -5 (Got %s)" % gesture_name, Color.RED)

func _on_timer_timeout():
	score -= 10
	_show_feedback("TIMEOUT! -10", Color.ORANGE)
	pick_new_target()

func _show_feedback(msg, color):
	label_feedback.text = msg
	label_feedback.modulate = color
	label_score.text = "Score: %d" % score
	print("[GESTURE TEST] %s | Final Score: %d" % [msg, score])

func _get_name(type):
	match type:
		GestureRecognizer.GESTURE_SWIPE_LEFT: return "Swipe Left"
		GestureRecognizer.GESTURE_SWIPE_RIGHT: return "Swipe Right"
		GestureRecognizer.GESTURE_SWIPE_UP: return "Swipe Up"
		GestureRecognizer.GESTURE_SWIPE_DOWN: return "Swipe Down"
		GestureRecognizer.GESTURE_SWIPE_UP_LEFT: return "Swipe Up-Left"
		GestureRecognizer.GESTURE_SWIPE_UP_RIGHT: return "Swipe Up-Right"
		GestureRecognizer.GESTURE_SWIPE_DOWN_LEFT: return "Swipe Down-Left"
		GestureRecognizer.GESTURE_SWIPE_DOWN_RIGHT: return "Swipe Down-Right"
		GestureRecognizer.GESTURE_CIRCLE_CLOCKWISE: return "Circle (CW)"
		GestureRecognizer.GESTURE_CIRCLE_COUNTER_CLOCKWISE: return "Circle (CCW)"
		GestureRecognizer.GESTURE_TRIANGLE: return "Triangle"
		GestureRecognizer.GESTURE_SQUARE: return "Square"
		GestureRecognizer.GESTURE_CHECK_MARK: return "Check Mark"
		GestureRecognizer.GESTURE_X_MARK: return "X Mark"
		GestureRecognizer.GESTURE_ZIGZAG: return "Zigzag"
		GestureRecognizer.GESTURE_RECTANGLE: return "Rectangle"
		GestureRecognizer.GESTURE_ELLIPSE: return "Ellipse"
		GestureRecognizer.GESTURE_HEXAGON: return "Hexagon"
		GestureRecognizer.GESTURE_PENTAGON: return "Pentagon"
		GestureRecognizer.GESTURE_STAR: return "Star"
		GestureRecognizer.GESTURE_DIAMOND: return "Diamond"
		_:
			if type >= GestureRecognizer.GESTURE_LETTER_A and type <= GestureRecognizer.GESTURE_LETTER_Z:
				return "Letter " + char(65 + (type - GestureRecognizer.GESTURE_LETTER_A))
			if type >= GestureRecognizer.GESTURE_DIGIT_0 and type <= GestureRecognizer.GESTURE_DIGIT_9:
				return "Digit " + str(type - GestureRecognizer.GESTURE_DIGIT_0)
			return "Unknown Pattern (%d)" % type
