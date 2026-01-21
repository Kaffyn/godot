extends CharacterBody3D
class_name BaseCharacter

@onready var asc: AbilitySystemComponent = $AbilitySystemComponent
@onready var inv: InventoryContainer = $InventoryContainer

@export var character_name: String = "Unnamed"
@export var character_class: String = "Generic"

func _ready() -> void:
	if not asc.attribute_set:
		push_error("AbilitySystemComponent must have an attribute_set.")

func get_health() -> float:
	return asc.attribute_set.health if asc.attribute_set else 0.0

func get_max_health() -> float:
	return asc.attribute_set.max_health if asc.attribute_set else 100.0

func get_stamina() -> float:
	return asc.attribute_set.stamina if asc.attribute_set else 0.0

func apply_damage(amount: float):
	if asc.attribute_set:
		asc.attribute_set.health -= amount
		if asc.attribute_set.health <= 0:
			_die()

func _die():
	print(character_name + " has died.")
	queue_free()
