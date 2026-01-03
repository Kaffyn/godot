# Plano de Engenharia - Virtual Device

Este plano define a integração de um novo tipo de dispositivo à Godot Engine: o **Dispositivo Virtual**. Assim como a engine suporta Teclados, Mouses e Gamepads, passaremos a ter dispositivos virtuais nativos, integrados profundamente ao pipeline de entrada e ao `InputMap`.

## 1. Modelo Mental (Funcionamento)

Para entender a profundidade desta mudança, vamos comparar como os Joysticks funcionam hoje e como funcionarão nativamente.

### Forma Atual (Addon/Script)

O funcionamento é baseado em **tradução reativa**. Imagine o script como um intérprete: ele observa os dedos na tela e, cada vez que um movimento ocorre, ele corre até o Singleton `Input` e diz: "Ei, o usuário quer que você execute a ação 'andar_frente'!". O script precisa monitorar IDs de dedos manualmente e calcular vetores em GDScript, o que gera overhead e distancia o input do "fluxo de hardware".

### Nova Forma (Engine Core/Virtual Device)

O funcionamento é baseado na criação de um **novo tipo de dispositivo nativo**. O `VirtualDevice` não é um simulador que finge ser um Gamepad ou Mouse; ele é um cidadão de primeira classe na Godot, categorizado como um dispositivo próprio. Isso significa que a engine reconhece eventos vindo especificamente de "Dispositivos Virtuais", permitindo total separação e controle no `InputMap` e no pipeline de processamento, sem latência.

## 2. Arquitetura do Dispositivo Virtual (Scene/GUI)

A base do sistema será um novo grupo de nós do tipo `Control`, permitindo que designers criem interfaces de entrada personalizadas que se comportam como hardware.

### [IMPLEMENTED] VirtualDevice (Abstract Class)

- **Base:** `Control`
- **Função:** Classe base abstrata para todos os controles virtuais.
- **Responsabilidades:**
  - Gerenciamento de `device` (índice do dispositivo virtual).
  - Lógica comum de multitoque e captura de foco.
  - Interface para despacho de eventos para o `Input`.
  - Propriedades: `device`, `visibility_mode`, `disabled`, `action_mask`.

### [IMPLEMENTED] VirtualButton

- **Herança:** `VirtualDevice`
- **Função:** Botão virtual que emite `InputEventVirtualButton`.

### [IMPLEMENTED] VirtualJoystick

- **Herança:** `VirtualDevice`
- **Função:** Joystick analógico **fixo** que emite `InputEventVirtualMotion`.

### [IMPLEMENTED] VirtualTouchPad (Pure Motion)

- **Herança:** `VirtualDevice`
- **Funcionalidade:** Área para movimentação relativa (Estilo laptop), ideal para controle de câmera sem um joystick fixo.
- **Arquitetura:** Este nó é estritamente para **Motion** (Movimento Contínuo). Ele emite `InputEventVirtualMotion` baseado em deltas de frame. Ele **não** realiza reconhecimento de gestos.
- **Propriedades:** `sensitivity`, `x_axis`, `y_axis`.

### [IMPLEMENTED] VirtualGestureArea (Pure Gesture)

- **Herança:** `VirtualDevice`
- **Funcionalidade:** Detector de gestos (Swipe, Pinch, Rotate, Symbols) que emite eventos discretos.
- **Arquitetura:** Este nó é estritamente para **Gestures** (Comandos Discretos). Ele acumula traços e emite `InputEventVirtualGesture` apenas quando um padrão é reconhecido.
- **Algoritmo:** Utiliza o `GestureRecognizer` ($Q Multistroke) interno.
- **Propriedades:** `min_confidence`, `resample_points`.

## 4. Expansão do Sistema de Entrada (Core)

Para que esses nós funcionem como dispositivos reais, expandimos o núcleo da engine.

### [IMPLEMENTED] InputEvent (core/input/)

- Introdução de novos tipos de eventos nativos:
  - `InputEventVirtualButton`
  - `InputEventVirtualMotion`
  - `InputEventVirtualGesture`: Possui `gesture_type` (Enum) e `confidence`.

### [IMPLEMENTED] GestureRecognizer (core/math/)

- **Base:** `RefCounted`
- **Função:** Classe utilitária para reconhecimento algorítmico de gestos.
- **Algoritmo ($Q Multistroke):** Implementa o algoritmo **$Q Super-Quick Recognizer**, uma evolução do $1 e $P.
  - **Multistroke:** Suporta gestos compostos por múltiplos traços (ex: desenhar um 'X' com dois riscos separados).
  - **Invariância:** Reconhece formas independentemente da ordem dos traços, direção do desenho ou rotação.
  - **Pipeline:** Resample -> Scale -> Translate -> Cloud Match (LUT).
- **Detecção:**
  - **Direcional:** 8 direções de swipe.
  - **Formas Geométricas:** Círculo, Triângulo, Quadrado, Check, X, ZigZag.
  - **Alfabeto Latino:** A-Z (Unistroke optimized).
- **Extensibilidade:** Possui API (`create_template`) para adicionar novos gestos via GDScript em tempo de execução.

---

## Exemplos de Integração (InputEvent API)

Abaixo estão exemplos de como implementar movimentação de Player e Câmera em 3D utilizando a API de eventos nativa da Godot (`_input` / `_unhandled_input`), sem acoplamento direto com nós de UI.

### 1. CharacterController3D (Player)

Este script controla um personagem usando `InputEventVirtualMotion` (Joystick Virtual) e teclado/gamepad, unificados pelo `InputMap`.

```gdscript
extends CharacterBody3D

const SPEED = 5.0
const JUMP_VELOCITY = 4.5

# Variáveis para armazenar estado do input virtual/físico
var move_input := Vector2.ZERO

func _ready() -> void:
 # Opcional: Garantir que o mouse não interfira se for um jogo mobile
 Input.mouse_mode = Input.MOUSE_MODE_CAPTURED if OS.get_name() != "Android" else Input.MOUSE_MODE_VISIBLE

func _unhandled_input(event: InputEvent) -> void:
 # Tratamento de Joystick Virtual (Analog)
 if event is InputEventVirtualMotion:
  # Device 0 geralmente é o stick esquerdo
  if event.device == 0:
   if event.axis == 0: move_input.x = event.axis_value
   if event.axis == 1: move_input.y = event.axis_value

 # Fallback para Teclado/Gamepad via InputMap (se configurado)
 if event.is_action("move_left"): move_input.x = -event.get_action_strength("move_left")
 if event.is_action("move_right"): move_input.x = event.get_action_strength("move_right")
 if event.is_action("move_forward"): move_input.y = -event.get_action_strength("move_forward")
 if event.is_action("move_back"): move_input.y = event.get_action_strength("move_back")

func _physics_process(delta: float) -> void:
 # Adicione a gravidade
 if not is_on_floor():
  velocity += get_gravity() * delta

 # Pulo
 if Input.is_action_just_pressed("ui_accept") and is_on_floor():
  velocity.y = JUMP_VELOCITY

 # Movimentação baseada na direção da câmera
 var camera = get_viewport().get_camera_3d()
 var dir = Vector3.ZERO

 if camera:
  var cam_basis = camera.global_transform.basis
  dir = (cam_basis.x * move_input.x + cam_basis.z * move_input.y)
  dir.y = 0 # Manter no chão
  dir = dir.normalized()

 if dir:
  velocity.x = dir.x * SPEED
  velocity.z = dir.z * SPEED
 else:
  velocity.x = move_toward(velocity.x, 0, SPEED)
  velocity.z = move_toward(velocity.z, 0, SPEED)

 move_and_slide()
```

### 2. CameraController3D (Gimbal/Free Look)

Este script controla a rotação da câmera usando `InputEventVirtualMotion` (TouchPad Virtual) ou Mouse. O `VirtualTouchPad` é ideal aqui pois emite deltas relativos.

```gdscript
extends Node3D

@export var sensitivity_touch := 0.2
@export var sensitivity_mouse := 0.1
@export var target : Node3D # O Player a seguir

var rotation_x := 0.0
var rotation_y := 0.0

func _unhandled_input(event: InputEvent) -> void:
 # 1. Suporte a Virtual TouchPad
 # O VirtualTouchPad emite InputEventVirtualMotion mas com comportamento relativo
 if event is InputEventVirtualMotion:
  # Device 1 configurado como TouchPad de câmera
  if event.device == 1:
   if event.axis == 0: _rotate_camera(-event.axis_value * sensitivity_touch)
   if event.axis == 1: _rotate_camera_pitch(-event.axis_value * sensitivity_touch)

 # 2. Suporte a Mouse Nativo
 if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
  _rotate_camera(-event.relative.x * sensitivity_mouse)
  _rotate_camera_pitch(-event.relative.y * sensitivity_mouse)

 # 3. Suporte a Gestos (Swipe para girar 180, por exemplo)
 if event is InputEventVirtualGesture:
  if event.gesture_type == GestureRecognizer.GESTURE_SWIPE_LEFT:
   _tween_rotation(90)
  elif event.gesture_type == GestureRecognizer.GESTURE_SWIPE_RIGHT:
   _tween_rotation(-90)

func _rotate_camera(yaw: float) -> void:
 rotation_y += yaw
 rotation.y = rotation_y

func _rotate_camera_pitch(pitch: float) -> void:
 rotation_x += pitch
 rotation_x = clamp(rotation_x, deg_to_rad(-90), deg_to_rad(90))
 rotation.x = rotation_x

func _tween_rotation(degrees: float) -> void:
 var tween = create_tween()
 tween.tween_property(self, "rotation_degrees:y", rotation_degrees.y + degrees, 0.3)

func _process(delta: float) -> void:
 if target:
  global_position = target.global_position
```

### 3. GestureRecognizer API (Uso Direto - Core Math)

O `GestureRecognizer` agora é um utilitário matemático puro (stateless). Você fornece os pontos e ele retorna o resultado. Swipes simples podem ser detectados via helper estático.

```gdscript
extends Node2D

var recognizer = GestureRecognizer.new()
var points : PackedVector2Array = []
var drawing = false

func _ready():
 # Configurações globais do matcher
 recognizer.min_confidence = 0.6
 recognizer.use_aspect_ratio_check = true
 recognizer.rotation_invariant = true

func _input(event):
 if event is InputEventMouseButton:
  if event.button_index == MOUSE_BUTTON_LEFT:
   if event.pressed:
    drawing = true
    points.clear()
    points.append(event.position)
   else:
    drawing = false
    # 1. Checagem rápida de Swipe (Helper Estático)
    var swipe = GestureRecognizer.get_swipe_direction(points[0], points[-1], 50.0)
    if swipe != GestureRecognizer.GESTURE_UNKNOWN:
     print("Swipe detectado: ", swipe)
    else:
     # 2. Reconhecimento de Forma Matemática
     var result = recognizer.recognize(points)
     process_gesture(result)

 elif event is InputEventMouseMotion and drawing:
  points.append(event.position)
  queue_redraw()

func process_gesture(result: Dictionary):
 if result["confidence"] < recognizer.min_confidence:
  print("Gesto desconhecido.")
  return

 print("Gesto: %s (Confiança: %f)" % [result["type"], result["confidence"]])
```
