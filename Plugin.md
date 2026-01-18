# Zyris Framework (desatualizado, isso relete o plugin zyris não a engine zyris, ou seja, o Zyris engine possui tudo que o Zyris Framework Plugin possui e muito mais, porem usa gnomes diferentes para os sistemas)

Menu gnome é Machi, sou professor de Godot e estou desenvolvendo o **Zyris**, um framework modular em C++ (GDExtension) para Godot 4.5+.

**Servers / Services:** Funcionam como "sistemas nativos". Não estão na SceneTree, não estendem `Node` (geralmente `Object` ou gerenciados internamente em C++) e são acessados globalmente como Singletons da Engine (ex: `PhysicsServer2D`). Não possuem `_process` no sentido de Node, mas rodam em seus próprios loops internos.

- [Zyris Framework](#zyris-framework)
  - [Arquitetura \& Design Patterns](#arquitetura--design-patterns)
    - [1. Servers (C++ Singletons)](#1-servers-c-singletons)
    - [2. Resources (Data-Driven)](#2-resources-data-driven)
    - [3. Nodes (Visualizers)](#3-nodes-visualizers)
    - [4. Scenes (Prefabs)](#4-scenes-prefabs)
  - [Ability System](#ability-system)
    - [Components do Ability System](#componentes-do-ability-system)
    - [Systems Internos](#systems-internos)
    - [Multiplayer](#multiplayer)
    - [Debug \& Tooling](#debug--tooling)
    - [Integração com o Sonhar](#integração-com-o-sonhar)
    - [Princípio Central](#princípio-central)
  - [Behavior Tree](#behavior-tree)
    - [O Cérebro Híbrido](#o-cérebro-híbrido)
    - [Integração com Ability System](#integração-com-ability-system)
    - [Multiplayer \& Determinismo](#multiplayer--determinismo)
    - [Perfis Dinâmicos](#perfis-dinâmicos)
  - [Director](#director)
  - [Gaia](#gaia)
    - [Arquitetura: Clima Sistêmico](#arquitetura-clima-sistêmico)
    - [Integração de Gameplay (Ability System)](#integração-de-gameplay-ability-system)
    - [Integração com outros Sistemas](#integração-com-outros-sistemas)
  - [Inventory](#inventory)
  - [Kinesis](#kinesis)
  - [Sonhar](#sonhar)
  - [Mimir](#mimir)
  - [Osmo](#osmo)
    - [Arquitetura: Virtual Cameras](#arquitetura-virtual-cameras)
    - [Conceitos Fundamentais](#conceitos-fundamentais)
    - [Integração com Director](#integração-com-director)
  - [Quests](#quests)
  - [Sounds](#sounds)
    - [O Compositor (Visual Music Sequencer)](#o-compositor-visual-music-sequencer)
    - [Arquitetura: Containers \& Events](#arquitetura-containers--events)
    - [Conceitos Fundamentais](#conceitos-fundamentais-1)
    - [Workflow](#workflow)
  - [Synapse](#synapse)
    - [Arquitetura: Native Physics Backend](#arquitetura-native-physics-backend)
    - [Conceitos Fundamentais](#conceitos-fundamentais-2)
  - [Yggdrasil](#yggdrasil)
    - [Game State \& Lifecycle](#game-state--lifecycle)
    - [Level Streaming (Portals \& Chunks)](#level-streaming-portals--chunks)
    - [Emissores (Nodes)](#emissores-nodes)
  - [Yggdrasil](#yggdrasil-1)

---

## Arquitetura & Design Patterns

O Zyris se diferencia por sua **rigidez estrutural** em prol da escalabilidade e performance. Abaixo definimos os três pilares de implementação para qualquer módulo do framework.

### 1. Servers (C++ Singletons)

> **Extends:** `Object`

Funcionam como subsistemas nativos da engine. Else **NÃO** são Nodes, não estão na SceneTree e não possuem `_process` conventional.

- **Ciclo de Vida:** Gerenciados internamente pela Engine e acessíveis globalmente via Singletons (como `PhysicsServer2D` ou `DisplayServer`).
- **Função:** Processamento pesado, gerenciamento de estado global e queries espaciais.
- **Exemplo:** `InventoryServer`, `SynapseServer`, `GaiaServer`, `SoundServer`, `QuestServer`, `DirectorServer`, `SonharServer`, `OsmoServer`, `MimirServer`, `Kinesis`, `Yggdrasil`.

**Por que usar Servers?**

1. **Persistência Real:** Nodes morrem quando a cena muda. Servers vivem desde o boot até o shutdown (ideal para Inventário, Quests, SaveData).
2. **Árbitro Global:** Quando 10 Câmeras querem set ativas, quem decide? O `OsmoServer`. Quando 50 sons tocam, quem corta o menos importante? O `SoundServer`.
3. **Performance:** Servers rodam lógica pesada (Spatial Hashing, AI Ticks) em C++ puro, sem o overhead da SceneTree.
4. **Clean Architecture:** Separa a _Lógica_ (Server) da _Visualização_ (Node). O Node apenas "pede", o Server "executa".

### 2. Resources (Data-Driven)

> **Extends:** `Resource`

Unidade básica de dados e lógica de comportamento. No Zyris, a lógica de gameplay reside nos Resources, não nos Nodes.

- **Conceito:** "Scriptable Objects" com superpoderes.
- **Função:** Definir **O QUE** acontece (Stats, Skills, Quests).
- **Vantagem:** Serializável, compartilhável entre instâncias e editável no Inspector.

### 3. Nodes (Visualizers)

> **Extends:** `Node`, `Node2D`, `Node3D`

Apenas a manifestação visual ou auditiva do sistema na cena.

- **Função:** Visualizar o estado actual ou capturar input.
- **Regra:** Nodes não devem conter lógica de persistência ou regras de negócio críticas. Else apenas consultam os Servers ou leem Resources.

### 4. Scenes (Prefabs)

> **Composition:** Nodes + Resources (`.tscn`)

O Zyris fornece implementações de referência prontas para uso, combinando Nodes e Resources para resolver problemas comuns de Gameplay e UI.

- **Conceito:** "Lego Blocks" pré-montados.
- **Função:** Acelerar o desenvolvimento entregando components complexos já configurados.
- **Exemplos:**
  - `ItemPickup.tscn`: (Area3D/2D + Sprite/Mesh + `Item` Resource).
  - `InventorySlot.tscn`: (TextureRect + Label + Lógica de Drag & Drop).
  - `VirtualJoystick.tscn`: (Control + TouchScreenButton do módulo **Kinesis**).

## Ability System

> Motor de Gameplay Orientado a Dados, Contexto e Composição
>
> **Interface Jogador -> Jogo:** Transforma intenção (Input) em Realidade (Gameplay). O sistema valida e executa o que o jogador deseja fazer.
>
> **Arquitetura (Hash Map Invertido):**
> Diferente de uma FSM comum, aqui **os Resources definem quando querem rodar**.
> O sistema indexa essas intenções e o _Ability System_ seleciona o melhor candidato para o contexto actual.

O Ability System é um package central do Framework responsável por decisão, seleção e aplicação de gameplay.

Ele não executa ações diretamente, não depende de FSM tradicional e não utilize singletons globais.
Seu papel é avaliar o contexto actual, consultar possibilidades válidas e decidir o melhor candidato com base em dados.

O sistema é orientado a Resources, determinístico, escalável e compatível com multiplayer.

**Servers / Services:** Nenhum.

> O sistema é orientado a Resources.
> O sistema é instanciado conscientemente e pode existir múltiplas vezes (player, NPC, boss, summon, companion).

**Nodes:**

- **AbilitySystem (extends Node):** Núcleo do sistema. Responsável por:
  - Construção do Contexto
  - Consulta indexada de candidatos
  - Validação de requisitos
  - Scoring
  - Seleção
  - Execução por fases
  - Gerenciamento de ciclo de vida

**Resources Fundamentais:**

- Character
- State
- Skill
- Effect

Todos os Resources são orientados à composição, formados por `AbilityComponents`.

### Components do Ability System

**Character (Resource)**

O `Character` representa a Character Sheet viva.
Ele é o modelo completo do personagem, independente de Node, cena ou representação visual.

> Pense nele como “o que o personagem é e tem”, não o que ele está fazendo agora.

O `Character` é a fonte de verdade do gameplay. Ele não executa lógica.

**Responsabilidades do Character:**

- Definir atributos base e máximos
- Armazenar progressão
- Conter referências para States, Skills, Effects e Items
- Servir como alvo principal de aplicação de gameplay

**Components de Character:**

- Atributos base e atuais
- Pools de States, Skills e Effects
- Inventário
- Tags e Flags
- Progressão
- Dados de versão

**AbilityComponent**

Classe base abstrata usada por:

- Character
- State
- Skill
- Effect

**AbilityComponents:**

- Descrevem regras
- Descrevem consequências
- Não executam lógica

**State (Resource)**

States representam modos de existência do personagem.
Um State pode set híbrido (ex: ataque + dash).

**Components de State:**

- Filter Components
- Score Components
- Movement Modifiers
- Physics Modifiers
- Combat Modifiers
- Combo Rules
- Reaction Rules
- Cost Definition
- Duration & Cooldown
- Lifecycle Rules

> Um State só é válido se o Character e o Contexto permitirem.

**Effect (Resource)**

Effects representam modificações temporárias ou condicionais aplicadas ao Character.

**Components de Effect:**

- Entry
- Exit
- Elemental
- Buffs / Debuffs
- Impact
- Costs
- Duration & Cooldown

**Skill (Resource)**

Skills representam desbloqueios e permissões de gameplay, não ações diretas.

**Components de Skill:**

- Requirements Skills
- Unlock
- Effect
- State
- Impact
- Buffs
- Elemental
- Costs
- Duration & Cooldown

### Systems Internos

**Context System:**

Constrói um snapshot imutável considerando:

- Character, Alvo(s), Ambiente, Superfície
- Estados físicos, Tags e Flags
- **Environmental Tags:** `Env.Weather.Rain`, `Env.Biome.Desert` (Injetadas pelo Gaia)
- States e Effects ativos
- Seed determinístico de RNG, Timestamp lógico

**Candidate Query System (Hash Map Invertido):**

A descoberta de candidatos e a filtragem fazem parte de um único sistema.
O Ability System utilize Hash Maps Invertidos indexados por Enums de filtro, permitindo queries rápidas e previsíveis.

**Pipeline:** Construção do Context -> Consulta aos Hash Maps -> União -> Validação -> Scoring -> Seleção -> Execução.

**Scoring System:**

Após validação, cada candidato é pontuado. Scoring é explícito e previsível.
Composto por: Base Score, Modificadores, Pesos, Penalidades, Fator aleatório.

**Selection System:**

O sistema seleciona o vencedor baseado em: Maior score, Top N ou Escolha ponderada.

**Execution System:**

Execução ocorre em fases bem definidas:

1. PreCheck
2. Cost Commit
3. Execution
4. PostExecution
5. Cleanup

**Apply System:**

Aplica consequências no Character: Entrada/saída de States, Aplicação de Effects, Modificações de atributos.

**Lifecycle System:**

Gerencia Duração, Manutenção, Refresh, Stacking e Expiração.

**Reaction & Interruption:**

Nada reage automaticamente. O Ability System decide baseado em Prioridades, Níveis de interrupção e Imunidades.

### Multiplayer

- Autoridade no servidor
- Execução determinística
- Replicação de Context
- Rollback e prediction

### Debug & Tooling

O Ability System expõe dados (Context Snapshot, Candidates, Scores, Timeline) que o Sonhar visualiza.

### Integração com o Sonhar

O Ability System registra resources e fornece Enums. O Sonhar renderiza o Editor e cria assets.
Nenhuma dependência direta de UI existe no Ability System.

### Princípio Central

> Character existe
> Contexto acontece
> Ability System decide
> Sonhar torna tudo editável

Este package não define ações.
Ele define possibilidades, restrições e decisões.

---

## Behavior Tree

> Cérebro Estratégico & Lógica Viva
>
> **Interface Jogador -> Jogo:** Define a autonomia do avatar, permitindo que commandos do jogador sejam interpretados inteligentemente.
>
> **Stack Híbrida:**
>
> - **Runtime (C++):** GDExtension de alta performance para o jogo final (No-Python dependency).
> - **Training (Python):** Backend de desenvolvimento para treinar redes neurais e conectar com a Gemini API.

Este package implementa um motor de IA de última geração, com um **Editor Visual (Main Panel)** inspirado na Unreal/LimboAI e um pipeline de treinamento nativo.

**Servers / Services:** Nenhum.

**Nodes:**

- **BehaviorTreePlayer (extends Node):** Executor de runtime da árvore (C++).
  - Multithreaded tick execution.
  - Gerenciamento de memória via Pools.

**Resources:**

- **BehaviorTree:** O grafo lógico.
- **RLConfig:** Configuração de treinamento (Sensores/Objetivos).
- **MachiBrain:** Arquivo otimizado (`.machi_brain`) contendo a Policy treinada.

### O Cérebro Híbrido

A arquitetura integra duas inteligências:

1. **Reflexos (Reinforcement Learning):** Para combate e física (Bosses). Treinado nativamente via Python backend e executado em C++.
2. **Cognição (Gemini Coach):** O Gemini analisa logs de treino para ajustar a dificuldade e gera assets estáticos (diálogos) em tempo de desenvolvimento.

### Integração com Ability System

| Sistema            | Papel          | Responsabilidade                                              |
| :----------------- | :------------- | :------------------------------------------------------------ |
| **Behavior Tree**  | **Estratégia** | Decide _o que_ fazer (ex: "Inimigo perto -> Atacar").         |
| **Ability System** | **Execução**   | Decide _como_ fazer (ex: "Qual ataque usar? Tenho stamina?"). |

### Multiplayer & Determinismo

- **Server Authority:** A árvore executa e decide exclusivamente no Servidor.
- **Client Prediction:** O Cliente prevê **Intenções**.
- **Snapshot Interpolation:** Blackboard replicado para killcams perfeitas.

### Perfis Dinâmicos

| Perfil      | Cérebro              | Exemplo   |
| :---------- | :------------------- | :-------- |
| **Critter** | BT Simples           | Pássaros. |
| **NPC**     | BT + Gemini Assets   | Aldeões.  |
| **Boss**    | BT + RL (Neural Net) | Chefes.   |

---

## Director

> O Maestro da Narrativa
>
> **Sequencer & Cutscenes:** Uma ferramenta de timeline unificada para orquestrar mementos cinematográficos.
>
> **Filosofia (Sequencer):** Inspirado no Unreal Sequencer. Manipula estado de jogo (Câmera, Some, TimeScale) em tempo real, sem "baking".

O `Director` orquestra os outros packages via **Semantic Tracks**.

**Server:**

- **DirectorServer:** Singleton gestor de estado (Bloqueia inputs em cutscene). Não é um Node.

**Nodes:**

- **SequencePlayer:** O executor da timeline na cena.
- **CinemaZone:** Trigger de início de cutscene.

**Features:**

- **Osmo Track:** Controla cortes de câmera e vCams.
- **Sound Track:** Dispara SoundCues com prioridade.
- **Synapse Track:** Emite estímulos de IA sincronizados.
- **State Restore:** Restaura o estado anterior do jogo ao fim da cutscene.

**Resources:**

- **Cutscene:** O asset da timeline.
- **ActionClip:** Bloco de ação para NPCs.

---

## Gaia

> A Atmosfera & o Tempo
>
> **Interface Jogo -> Jogador:** Gaia não apenas simula o clima, mas dita o **"Mood"** do jogo. É a camada final de imersão que conecta o jogador emocionalmente ao mundo virtual.

Simulação de Ambiente Sistêmico. Mais do que chuva visual, Gaia gerencia estados globais que afetam gameplay, física e shaders.

### Arquitetura: Clima Sistêmico

Gaia opera em **Estados Globais** que são propagados para todos os materiais e entidades do jogo.

**1. Global Wetness & Wind**

O clima não é local. É uma variável global.

- **Wetness (0.0 - 1.0):** Controla o quanto o mundo está molhado.
  - **Shaders:** Aumenta roughness, escurece albedo (via `global_shader_parameter`).
  - **Gameplay:** Aumenta chances de escorregar (Physics Materials), apaga fogo.
- **Wind Vector:** Vetor 3D global de vento.
  - Afeta partículas (chuva, folhas), balística de projéteis e movimentação de vegetação.

**2. Temperatura & Sobrevivência**

Gaia rastreia a temperatura ambiente baseada na Hora, Estação e Clima.

- **Gameplay Tag:** O jogador recebe Tags como `Status.Cold` ou `Status.Hot`.
- **Animação:** Procedural blending para animações de frio (tremer) ou calor (limpar suor).

**3. Biomas & Zonas**

O mundo é dividido em Biomas (Deserto, Tundra, Floresta).

- **Weather Weights:** Um deserto tem 99% de chance de Sol e 1% de Chuva. Uma Tundra tem 50% de Neve.
- **Temperature Offset:** O bioma define a base térmica (ex: Deserto +20ºC durante o dia, -10ºC à noite).
- **Audio Ambience:** O bioma dita o some de fundo (vento, fauna) em integração com o `Sounds`.

**4. Ciclo Dia/Noite**

Simulação astronômica simplificada.

- **Sun & Moon:** Controle orbital de luzes direcionais.
- **Curve-Driven:** Cor da luz, Neblina e Densidade de Nuvens são controlados por curvas (Resources), permitindo dias de "Inverno Pálido" ou "Verão Dourado".

**Servers:**

- `GaiaServer` (C++): Gerencia o loop de simulação e update de parâmetros globais de shader.

**Nodes:**

- `DayNightCycle` / `DayNightCycle2D`: Controlador de tempo (Suporte a 2D e 3D).
- `WeatherController`: Gerencia transições de clima (ex: Ensolarado -> Tempestade leva 30 segundos).
- `GaiaZone`: Área override (ex: Entrar numa caverna para a chuva/vento locais).

**Resources:**

- **BiomeDef:** Definição de tabelas de clima e offsets de temperatura.
- `WeatherDef`: Define um tipo de clima (Chuva, Neve, Tempestade de Areia).
- `SeasonDef`: Define durações de dias e curvas de temperatura.

### Integração de Gameplay (Ability System)

Gaia não é apenas visual. Ele conversa diretamente com o `AbilitySystem` via **Tags**.

1. **Tag Emitter:**

- Se chover, Gaia adiciona a tag `Env.Weather.Rain` ao Contexto Global.
- Se estiver frio, adiciona `Env.Temperature.Cold`.
- Se estiver no deserto, adiciona `Env.Biome.Desert`.

2. **Consumo de Recursos:**

- O Player pode ter um `Effect` passivo: "Se `Env.Temperature.Cold` estiver ativo, consuma +2 Stamina por segundo".

3. **Requirements:**

- Uma Skill `SummonSandworm` pode ter como requisito `Env.Biome.Desert`.
- Uma Skill `LightningStrike` pode ter dano ampliado se `Env.Weather.Rain` estiver ativo (Combo Elemental).

4. **Sinergia de Classe (Build Dependency):**
   - **Ice Mage:** Ganha regeneração de Mana na Tundra (`Env.Biome.Tundra`), mas sofre em Desertos.
   - **Fire Mage:** Ganha Power no Deserto, mas seus custos aumentam na Chuva.
   - **Warrior:** Neutro. Não ganha buffs nem debuffs, garantindo consistência em qualquer cenário.

### Integração com outros Sistemas

Gaia afeta o mundo inteiro.

- **Synapse:** Chuva forte ou Tempestade de Areia reduzem o alcance auditivo e visual dos inimigos (aumenta o _Noise Floor_ e reduz visibilidade).
- **Osmo:** Ventos fortes causam _Camera Shake_ passivo.
- **Sound:** O bioma dita o banco de sons ambiente que será carregado.

---

## Inventory

> O sistema de items unificado.
>
> **Interface de Recursos:** Gerencia a posse. O elo entre o esforço do jogador (loot) e o poder do personagem (equipamento).
>
> **Modularidade:**

**Servers:**

- **InventoryServer:** Singleton C++ agnóstico à cena. Gerencia transações, drops e queries globais.

**Nodes:**

- **InventoryContainer:** Lógica de armazenamento de items (Inventário local, Baú).
- **Slot (UI):** Componente visual para exibição e interação com um slot de inventário.
- **HotbarUI:** Interface para atalhos de items.
- **CraftingStation:** Bancada de trabalho para receitas.

**Resources:**

- **Item:** Definição base do item.
- **ItemCategory:** Classificação.
- **Inventory:** O storage de dados (pode set salvo no Mimir).
- **LootTable:** Tabelas de drop.
- **Recipe:** Regras de crafting.

---

## Kinesis

> Controle Total & Multiplataforma

> **Interface Jogador -> Jogo:** Kinesis é a camada de abstração de Input. Ele normaliza commandos de Teclado, Gamepad e Toque em um único "Vetor de Intenção".

**Servers:**

- **Kinesis (Singleton):** Centraliza o estado dos inputs (`movement_vector`, `jump_pressed`, etc.).

**Nodes:**

- **InputButton:** Botão virtual com suporte a mapeamento de ação (`action_name`) e multitouch.

**Conceito:**

O Kinesis permite que o `Character` apenas pergunte: `Kinesis.get_movement_vector()`. Ele não precisa saber se isso veio de um WASD, de um Controle de Xbox ou de um Joystick na tela do celular.

---

## Sonhar

> O Sonhar (Antigo Library)
>
> **O Laboratório de Criação & IDE Visual:** O Sonhar não é apenas uma pasta de assets. É o **Main Panel central do Zyris**, uma IDE completa dentro do Godot para criação e composição visual de dados.
>
> **Design Blueprint-like:** Edite Resources complexos como grafos visuais de components, não listas verticais.

Infraestrutura híbrida (C++/GDScript) focada em **Inversion of Control**. Outros packages (AbilitySystem, Sounds) injetam suas ferramentas no Sonhar.

**Servers:**

- **SonharServer:** Hub de registro onde packages externos se conectam. Não é um Autoload.

**Nodes / Tools:**

- **SonharEditor (Main Panel):**
  - **Graph Editor:** Editor visual de composição de recursos (ex: Arraste um `HealthComponent` para dentro de um `Character`).
  - **The Library (Resource Browser):** Sistema de arquivos filtrado apenas para dados de jogo.
  - **Factory:** Wizard de criação com templates pré-configurados.

**Resources:**

- **Blueprint:** Grafo visual que define a composição de dados complexos e relacionamentos entre Resources.

---

## Mimir

> A Memória Eterna (Server-Based)
>
> **Infraestrutura de Persistência:** API de baixo nível para serialização de dados.
>
> **Filosofia (Server & API):**
> Mimir não é um Node. Mimir é um **Server** (Singleton C++) que dorme 99% do tempo e só acorda quando invocado (ex: pelo `Yggdrasil`).

Mimir não utilize Autoloads. O acesso é direto via Singleton C++.

**Server:**

- **MimirServer (C++):** Motor de IO, Threads e Criptografia (AES-256). API global via `create_save()` e `load_save()`.

**Integração:**

- **Trigger:** Acionado pelo `Yggdrasil` em transições de cena.

**Resources:**

- **SaveSlot:** O arquivo no disco.
- **MigrationScript:** Lógica de atualização de versão (`v1` -> `v2`).
- **MimirSaveFile:** Estrutura serializável.

---

## Osmo

> Os Olhos do Jogador
>
> **Interface Jogo -> Jogador:** Enquanto o `Synapse` são os olhos do _Personagem_, o `Osmo` são os olhos do _Humano_. É através da câmera que o jogo direciona a atenção e recompensa o jogador com "game feel" (shakes, zoom, fluidez).

Sistema de Câmera Dinâmico. Inspirado na arquitetura do **Cinemachine** e na estabilidade do gimbal **DJI Osmo**.

### Arquitetura: Virtual Cameras

O Osmo abandona a ideia de "controlar a Camera2D/3D diretamente". Em vez disso, usamos **Virtual Cameras (vCams)**.

1. **O Cérebro (`OsmoBrain`):** Existe apenas uma Câmera real na cena. Ela não possui lógica própria.
2. **As Intenções (`VirtualCamera`):** Espalhadas pela cena, várias vCams definem enquadramentos ideais ("Exploration", "Dialog", "Combat").
3. **O Blend:** O `OsmoBrain` calcula a interpolação entre a vCam ativa anterior e a nova, criando transições cinematográficas automáticas.

### Conceitos Fundamentais

**1. Compositor (`Composer`)**

Regras de enquadramento.

- **LookAt:** Define para onde a câmera aponta.
- **Dead Zone:** Área central onde o alvo pode se mover sem mover a câmera (evita enjoo).
- **Damping:** Atraso suave no movimento para dar "peso".

**2. Ruído (`Noise Shake`)**

Shakes não são animações "baked". Usamos **Perlin Noise** em múltiplas camadas.

- **Trauma:** Valor de 0.0 a 1.0. Uma explosão adiciona 0.5 de trauma.
- **Decay:** O trauma cai linearmente com o tempo.
- **Seed:** Shakes são determinísticos e procedurais.

**3. Prioridade (`Priority`)**

Múltiplas vCams podem estar ativas (ex: Player entra numa `CameraZone` de combate).
O Osmo sempre escolhe a vCam ativa com a **Maior Prioridade**.

**Servers:**

- `OsmoServer`: **O Árbitro.** Mantém a lista global de vCams registradas e decide a cada frame qual é a "Vencedora" baseada em prioridade. Sem ele, as câmeras não saberiam quem deve estar ativa.

**Nodes:**

- `OsmoBrain`: A câmera real.
- `OsmoCamera` / `OsmoCamera3D`: Define parâmetros de lente, alvo e comportamento.
- `OsmoZone` / `OsmoZone3D`: Área que ativa uma vCam específica ao entrar.

**Resources:**

- `ShakeProfile`: Definição de amplitude e frequência do ruído.

### Integração com Director

O **Director** controla o Osmo em cutscenes.
Ele pode "tomar o controle" da câmera principal, sobrescrevendo a lógica de vCams para um plano sequential roteirizado, e depois devolver o controle suavemente para a câmera de gameplay.

---

## Quests

> Narrativa Sistemática & Graph-Based
>
> **Interface de Propósito:** Dá sentido à ação. Transforma o gameplay momento-a-momento em uma jornada de longo prazo com início, meio e fim.
>
> **Design Graph-First:** Quests não são listas lineares. São **Grafos de Estados** com ramificações.

O package opera em duas camadas: o **Quest Graph** (Estrutura) e o **Suggestion Engine** (Contexto), guiando o jogador dinamicamente.

**Servers:**

- **QuestServer:** Gerenciador global de runtime dos grafos.

**Servers / Services:** Nenhum.

**Nodes:**

- **QuestTrigger:** Volume 3D que avança ou inicia uma quest.
- **ObjectiveMarker:** Componente visual (seta/ícone).

**Resources:**

- **QuestGraph:** O blueprint da missão (Editor Visual).
- **QuestObjective:** Definição atômica de tarefa.
- **QuestReward:** Loot, XP, Reputação.

---

## Sounds

> A Voz do Mundo
>
> **Interface Jogo -> Jogador:** Enquanto o `AbilitySystem` define a ação técnica, o `Sounds` define o **Feedback Auditivo**. É a recompensa imediata e a ambientação que moldam a percepção humana.

Gerenciamento de Áudio Avançado & Criação Musical. Sistema inspirado em middlewares como **Wwise** e **FMOD**, mas expandido para set uma **DAW (Digital Audio Workstation) Interna** com suporte a áudio procedural e sequenciamento visual.

### O Compositor (Visual Music Sequencer)

O `Sounds` não apenas toca arquivos, ele **cria música**.

- **Visual Music Sequencer:** Uma timeline estilo DAW (Logic Pro, FL Studio) dentro do Sonhar.
- **Procedural Audio:** Sintetizadores em tempo real (Oscillators, Filters, Envelopes) para criar SFX dinâmicos sem arquivos `.wav`.
- **Adaptive Music:** Transições verticais (Layering) e horizontais (Sequencing) controladas pelo Gameplay.

### Arquitetura: Containers & Events

Sons não são apenas arquivos `.wav`. São estruturas lógicas.

**1. SoundCue (O Evento):** O jogo pede para tocar um "Cue" (ex: `Footstep_Dirt`).
**2. Containers (A Lógica):** O Cue contém uma árvore de decisão.

- **RandomContainer:** Escolhe um clip aleatório de uma lista (evita repetição robótica).
- **SequenceContainer:** Toca passos em ordem (ex: Recarga de arma).
- **SwitchContainer:** Escolhe baseado em uma variável global (ex: `SurfaceType = Wood` -> Toca madeira).

**3. Playback:** O `SoundManager` aloca um player de um Pool otimizado.

### Conceitos Fundamentais

**1. Concurrency (Gerenciamento de Vozes)**

Explosões são legais, mas 50 explosões simultâneas destroem os ouvidos e a CPU.

- **Max Instances:** Cada SoundCue define quantas cópias dele podem tocar ao mesmo tempo.
- **Resolution:** Se o limit for atingido, o sistema decide quem matar:
  - _Kill Oldest_: Para sons rápidos (tiros).
  - _Kill Furthest_: Para sons ambientes (monstros).
  - _Dont Play_: Simplesmente ignore o novo pedido.

**2. Priority & Ducking**

Nem todo some é igual.

- **Voice Lines > Gameplay > Ambience**.
- Quando um some de alta prioridade toca (Diálogo), o sistema aplica **Ducking** (redução de volume) automático nas categorias inferiores.

**3. Integração 3D**

- **Spatial Audio:** Suporte nativo a posicionamento 3D.
- **Occlusion:** Raycast simples para abafar sons atrás de paredes (Low Pass Filter).

**Servers:**

- `SoundServer` (C++): Gerencia o Mixer, Buses e Concurrency Pools. Acessado via `SoundServer.play_cue()`.

**Nodes:**

- `AudioListener2D/3D`: Ouvido do jogador.
- `AmbientArea`: Define zonas de áudio (Reverb, Ambience Loop).

**Resources:**

- `SoundCue`: A definição do evento sonoro.
- `SoundCategory`: Definição de Buses e Prioridade.

### Workflow

1. **Sound Designer** cria os `SoundCues` no editor, configurando randomização e envelopes.
2. **Programador** chama `SoundManager.play_cue("Explosion", position)`.
3. **Synapse** escuta esses eventos automaticamente para alertar AI.

---

## Synapse

> The Perception Engine
>
> **Interface Jogador -> Jogo:** A percepção do Avatar. Permite que o personagem "sinta" o mundo para reagir a ele, fechando o ciclo de interação.
>
> **Filosofia:**
> Synapse não é apenas um sistema de triggers. É uma **Engine de Simulação Sensorial**.
> Ele gerencia milhares de estímulos (luz, some, cheiro, vibração) e determina quem percebe o quê, utilizando otimização espacial avançada.

### Arquitetura: Native Physics Backend

O Synapse não reinventa a roda. Em vez de criar um grid espacial customizado, ele atua como uma **Camada Sistêmica** sobre o `PhysicsServer2D` da Godot.

1. **Physics Layers:** Utilizamos layers dedicadas (ex: `Perception Layer`) para isolar a consulta sensorial da física de colisão.
2. **Otimização Nativa:** Aproveitamos o BVH (Bounding Volume Hierarchy) da engine para queries espaciais rápidas.
3. **Abstração:** O Synapse fornece a lógica rica (Tags, Intensidade, Oclusão Lógica) que o sistema de física cru não possui.

### Conceitos Fundamentais

**1. Estímulo (`Stimulus`)**

Um evento perceptível no mundo. Não é apenas "Some", mas um pacote de dados rico.

- **Tipo:** Visual, Auditivo, Olfativo, Vibracional, Térmico.
- **Intensidade:** Valores normalizados (0.0 a 1.0). Decai com distância e obstrução.
- **Tags:** Informação semântica ("Danger", "Food", "Ally").

**2. Sensor (`Sensor`)**

O órgão receptor do agente. Cada sensor é especializado em um tipo de estímulo e possui configurações de acuidade.

- **Threshold:** Intensidade mínima para notar algo (ex: um some muito baixo é ignorado).
- **Attention Buffer:** Tempo necessário de exposição para confirmar a detecção (evita "olhos nas costas").

**3. MemóriaSensorial (`Blackboard`)**

O Synapse não toma decisões. Ele apenas _informa_.
Quando um Sensor confirma um estímulo, ele escreve diretamente na memória de curto prazo do Agente (Blackboard), atualizando `TargetLocation`, `LastKnownPosition`, etc.

**SynapseServer (Singleton C++)**

Despachante de eventos e coordenador de queries.

- Centraliza queries complexas.

---

## Yggdrasil

> O Tecido do Espaço-Tempo & Game State
>
> **Ciclo de Vida:** Gerencia não apenas cenas, mas o estado vital do jogo (Pause, Loading, Persistência Volátil).

**Singleton:** `Yggdrasil` (Server/View).

### Game State & Lifecycle

O Yggdrasil atua como o **Game Manager** central.

- **State Management:** Mantém dados voláteis críticos como `Health`, `Ammo`, `Level`, `Kills/Deaths` que precisam persistir entre transições de cena rápidas.
- **Global Pause:** Centraliza a lógica de `toggle_pause()`.

### Level Streaming (Portals & Chunks)

Sistema avançado de carregamento de mundos para jogos grandes.

**Nodes:**

- **Portal2D / Portal3D:** Transição física entre áreas. Teleporta o jogador e gerencia o loading da nova área.
- **ChunkLoaderZone2D / ChunkLoaderZone3D:** Carrega/Descarrega pedaços do mapa (Chunks) baseado na posição do jogador.
- **LevelAnchor2D / LevelAnchor3D:** Ponto de referência para spawn e conexão de portais.

**Resources:**

- **UniverseState:** Mantém o registro de quais portais/chunks estão ativos.
- **TransitionResource:** Define o visual da transição (Fade, Shader).
- Gerencia o "Time Slicing" dos sensores para distribuir a carga de processamento entre frames.
- **Não** armazena posições (o PhysicsServer já faz isso).

**Sensores (Nodes)**

**`VisualSensor2D / 3D`**

- Wrapper para **ShapeCast2D** (Cone de Visão) e **RayCast2D** (Linha de Visão).
- Valida oclusão física.

**`AuditorySensor2D / 3D`**

- Wrapper para **Area2D** (Alcance da audição).
- Detecta apenas `StimulusEmitters` na layer de Percepção.

**`ProximitySensor2D / 3D`**

- Wrapper para **ShapeCast2D** (Círculo imediato).

### Emissores (Nodes)

**`StimulusEmitter`**

Wrapper leve sobre um **Area2D**.

- Carrega os metadados (`Tags`, `Intensidade`) que o Sensor lê ao colidir.

- **Propriedades:**
  - `type`: Tipo de estímulo (Visual, Auditivo, etc).
  - `faction`: Quem emitiu.
  - `tags`: Etiquetas semânticas ("Danger", "Food").
  - `intensity`: Força do sinal.

**Integração com Behavior Tree**

O fluxo de dados é unidirecional e reativo:

1. **Mundo:** O Player dispara uma arma (`SoundCue` com Tag `LOUD`).
2. **Synapse:** O `SoundManager` registra automaticamente um `Stimulus` Auditivo no local.
3. **Sensor:** O `AuditorySensor` do Inimigo (na cell adjacente) detecta o estímulo.
4. **Memória:** O Sensor atualiza o Blackboard: `HasHeardNoise = true`, `NoiseLocation = Vector3(...)`.
5. **Cérebro:** A Behavior Tree checa o Blackboard, interrompe a patrulha e inicia a árvore `Investigate`.

**Reflexos (Fast Path)**

Para situações críticas (ex: Granada caindo aos pés), o Synapse pode enviar um sinal de **Interrupção de Reflexo** direto, forçando a Behavior Tree a abortar qualquer lógica complexa e executar uma esquiva imediata.

---

## Yggdrasil

> O Tecido do Espaço-Tempo
>
> **Infraestrutura de Universo:** Muito além de um "Scene Manager". Yggdrasil é a Máquina de Estados do Jogo Inteiro (Intro -> Menu -> Loading -> Gameplay).
>
> **Filosofia (Game State Machine):** O mundo não para no loading. Yggdrasil gerencia streaming, transições e persistência volátil.

**Arquitetura:**

- **Yggdrasil (SubViewportContainer):** O nó raiz persistente.
  - Gerencia o `SubViewport` principal.
  - Carrega/Descarrega cenas filhas (o "Mundo" actual).
  - Gerencia a UI global (Loading Screens, Pause Menu) acima do mundo.

**Integração com Mimir:**

Como Yggdrasil nunca morre, ele coordena o salvamento:

1. Pausa o jogo.
2. Pede ao `MimirServer` para serializar o estado dos atores na cena actual.
3. Troca a cena filha.
4. Pede ao `MimirServer` para restaurar o estado na nova cena.

**Features:**

- **Smart Streaming:** Pre-caching de cenas adjacentes e loading em threads de background.
- **Resource Transitions:** Transições visuais (Fades, shaders) são Resources (`.tres`) configuráveis.
- **Game State Flow:** Gerencia o ciclo `Boot -> Menu -> Gameplay -> Pause`.

**Nodes:**

- **LevelAnchor:** Marker3D para spawn points.
- **Portal:** Gatilho de mudança de estado.
- **StreamingVolume:** Área que inicia pre-loading de chunks.

**Resources:**

- **UniverseState:** Definição de um estado de jogo.
- **TransitionResource:** Configuração da transição.
