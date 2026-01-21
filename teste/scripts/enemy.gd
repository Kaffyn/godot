extends BaseCharacter
class_name Enemy

@export var speed: float = 2.0
@export var detection_range: float = 10.0

var target: BaseCharacter = null
var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")

func _physics_process(delta: float) -> void:
	if not is_on_floor():
		velocity.y -= gravity * delta

	if target:
		var dir = (target.global_position - global_position).normalized()
		velocity.x = dir.x * speed
		velocity.z = dir.z * speed
		move_and_slide()

		var look_dir = global_position + velocity
		if global_position.distance_to(look_dir) > 0.1:
			look_at(look_dir, Vector3.UP)
	else:
		velocity.x = move_toward(velocity.x, 0, speed)
		velocity.z = move_toward(velocity.z, 0, speed)
		move_and_slide()

func _on_detection_area_body_entered(body: Node3D) -> void:
	if body is Player:
		target = body

func _on_detection_area_body_exited(body: Node3D) -> void:
	if body == target:
		target = null
