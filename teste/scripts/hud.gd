extends CanvasLayer

@onready var health_bar: ProgressBar = $PassiveHUD/StatusBars/HealthBar
@onready var stamina_bar: ProgressBar = $PassiveHUD/StatusBars/StaminaBar
@onready var power_label: Label = $InteractiveHUD/CharacterSheet/VBox/PowerLabel

func update_stats(health: float, max_health: float, stamina: float, max_stamina: float, power: float):
	health_bar.max_value = max_health
	health_bar.value = health
	stamina_bar.max_value = max_stamina
	stamina_bar.value = stamina
	power_label.text = "Alchemic Power: %d" % power
