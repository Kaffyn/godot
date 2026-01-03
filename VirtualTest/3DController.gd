extends CharacterBody3D

@export var speed = 5.0
@export var sensitivity_touch = 0.005
@export var sensitivity_mouse = 0.002

@onready var gimbal_h = $"../GimbalH"
@onready var gimbal_v = $"../GimbalH/GimbalV"
@onready var label_debug = $"../CanvasLayer/LabelDebug"

var move_input = Vector2.ZERO
var rotation_target = Vector2.ZERO

func _physics_process(delta):
	# Using the Action System for movement
	var input_dir = Input.get_vector("move_left", "move_right", "move_up", "move_down")
	
	if input_dir != Vector2.ZERO:
		var direction = (gimbal_h.global_transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
		velocity.x = direction.x * speed
		velocity.z = direction.z * speed
	else:
		velocity.x = move_toward(velocity.x, 0, speed)
		velocity.z = move_toward(velocity.z, 0, speed)

	move_and_slide()
	
	# Smoothly follow the character with the gimbal
	gimbal_h.position = lerp(gimbal_h.position, position, 0.1)

func _input(event):
	# 1. Right TouchPad (Camera Rotation) - Devices usually handle rotation as raw delta
	if event is InputEventVirtualMotion and event.device == 1:
		_rotate_camera(-event.axis_value * sensitivity_touch, event.axis)
		_update_debug("Camera Rotation Axis %d: %.2f" % [event.axis, event.axis_value])

	# 2. Mouse Support (Integration Test)
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		_rotate_camera(-event.relative.x * sensitivity_mouse, 0)
		_rotate_camera(-event.relative.y * sensitivity_mouse, 1)

func _rotate_camera(delta, axis):
	if axis == 0: # Horizontal
		gimbal_h.rotate_y(delta)
	elif axis == 1: # Vertical
		gimbal_v.rotate_x(delta)
		gimbal_v.rotation.x = clamp(gimbal_v.rotation.x, deg_to_rad(-80), deg_to_rad(80))

func _update_debug(txt):
	if label_debug:
		label_debug.text = "[3D TEST] " + txt
