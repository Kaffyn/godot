extends Node2D

@onready var character = $Character
@onready var label_score = $CanvasLayer/LabelScore
@onready var fire_btn = $CanvasLayer/ButtonFire
@onready var cooldown_overlay = $CanvasLayer/ButtonFire/CooldownOverlay

const SPEED = 500.0
const FIRE_RATE = 0.15

var score = 0
var enemies = []
var bullets = []
var fire_cooldown = 0.0
var aim_direction = Vector2.RIGHT

func _ready():
	fire_btn.pivot_offset = fire_btn.size / 2

func _physics_process(delta):
	# 1. MOVEMENT (CharacterBody2D way)
	var move_dir = Input.get_vector("move_left", "move_right", "move_up", "move_down")
	character.velocity = move_dir * SPEED
	character.move_and_slide()
	
	# 2. AIMING & AUTO-FIRE
	var aim_input = Input.get_vector("aim_left", "aim_right", "aim_up", "aim_down")
	if aim_input.length() > 0.1:
		aim_direction = aim_input.normalized()
		character.rotation = aim_direction.angle()
		
		if aim_input.length() > 0.5:
			_check_and_shoot()

	# 3. MANUAL SHOOTING
	if Input.is_action_pressed("fire"):
		_check_and_shoot()

	# Cooldown Processing
	if fire_cooldown > 0:
		fire_cooldown -= delta
	_update_cooldown_ui()

	# 4. GAME ELEMENTS
	_update_game_elements(delta)

func _check_and_shoot():
	if fire_cooldown <= 0:
		_shoot()
		fire_cooldown = FIRE_RATE
		_animate_fire_button()

func _shoot():
	var bullet = ColorRect.new()
	bullet.size = Vector2(16, 6)
	bullet.color = Color.YELLOW
	bullet.position = character.position + (aim_direction * 40.0)
	bullet.rotation = aim_direction.angle()
	add_child(bullet)
	bullets.append({"node": bullet, "velocity": aim_direction * 800.0, "life": 1.5})

func _animate_fire_button():
	var tween = create_tween()
	tween.tween_property(fire_btn, "scale", Vector2(0.8, 0.8), 0.05)
	tween.tween_property(fire_btn, "scale", Vector2(1.0, 1.0), 0.05)

func _update_cooldown_ui():
	if cooldown_overlay:
		var fill = clamp(fire_cooldown / FIRE_RATE, 0.0, 1.0)
		cooldown_overlay.scale.y = fill
		cooldown_overlay.visible = fill > 0

func _update_game_elements(delta):
	var bullets_to_remove = []
	for b in bullets:
		b.node.position += b.velocity * delta
		b.life -= delta
		if b.life <= 0: bullets_to_remove.append(b)
	
	for b in bullets_to_remove:
		b.node.queue_free()
		bullets.erase(b)

	if randf() < 0.04 and enemies.size() < 15:
		_spawn_enemy()

	var enemies_to_remove = []
	for e in enemies:
		var dir = (character.position - e.node.position).normalized()
		e.node.position += dir * 160.0 * delta
		
		var e_rect = Rect2(e.node.position - Vector2(20,20), Vector2(40, 40))
		for b in bullets:
			if e_rect.has_point(b.node.position):
				enemies_to_remove.append(e)
				b.life = 0
				score += 10
				label_score.text = "Score: %d" % score
				break
	
	for e in enemies_to_remove:
		if e in enemies:
			e.node.queue_free()
			enemies.erase(e)

func _spawn_enemy():
	var e_node = ColorRect.new()
	e_node.size = Vector2(40, 40)
	e_node.color = Color.INDIAN_RED
	e_node.pivot_offset = Vector2(20, 20)
	var side = randi() % 4
	var screen = get_viewport_rect().size
	match side:
		0: e_node.position = Vector2(randf()*screen.x, -50)
		1: e_node.position = Vector2(randf()*screen.x, screen.y+50)
		2: e_node.position = Vector2(-50, randf()*screen.y)
		3: e_node.position = Vector2(screen.x+50, randf()*screen.y)
	add_child(e_node)
	enemies.append({"node": e_node})
