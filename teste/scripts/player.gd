extends BaseCharacter
class_name Player

@export var speed: float = 5.0
@export var jump_velocity: float = 4.5

var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")
@onready var hud: CanvasLayer = get_node("../HUD")

func _ready():
	super._ready()
	character_name = "The Alchemist"
	character_class = "Alchemist"

func _process(_delta: float) -> void:
	if hud:
		var attr = asc.attribute_set
		hud.update_stats(attr.health, attr.max_health, attr.stamina, attr.max_stamina, attr.alchemic_power)

func _physics_process(delta: float) -> void:
	if not is_on_floor():
		velocity.y -= gravity * delta

	# Handle jump.
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = jump_velocity

	# Get input direction from Virtual Controls or Keyboard
	var input_dir := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	var direction := (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()

	if direction:
		velocity.x = direction.x * speed
		velocity.z = direction.z * speed
	else:
		velocity.x = move_toward(velocity.x, 0, speed)
		velocity.z = move_toward(velocity.z, 0, speed)

	move_and_slide()

func _input(event: InputEvent) -> void:
	# Example of GAS ability trigger
	if event.is_action_pressed("ability_1"):
		asc.try_activate_ability("dash")

	if event.is_action_pressed("ability_2"):
		asc.try_activate_ability("pulse")

	# Toggle Camera Focus (vCam Blending)
	if event.is_action_pressed("toggle_focus"):
		var follow = $vCam_Follow
		var focus = $vCam_Focus
		if follow.priority > focus.priority:
			follow.priority = 0
			focus.priority = 10
		else:
			follow.priority = 10
			focus.priority = 0

	# Persistence (Save System)
	if event.is_action_pressed("save_game"):
		var data = {
			"position": position,
			"health": asc.attribute_set.health,
			"inventory": inv.serialize_items()
		}
		SaveServer.save_game("showcase_slot", data)
		print("Game Saved!")

	if event.is_action_pressed("load_game"):
		var data = SaveServer.load_game("showcase_slot")
		if data:
			position = data.position
			asc.attribute_set.health = data.health
			inv.deserialize_items(data.inventory)
			print("Game Loaded!")
