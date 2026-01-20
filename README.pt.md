# Zyris Engine

[![Godot Version](https://img.shields.io/badge/Godot-4.5.2--stable-blue.svg)](https://github.com/godotengine/godot)
[![Zyris Version](https://img.shields.io/badge/4.5.2-zyris.1-orange.svg)](https://github.com/Kaffyn/ZyrisEngine/tree/Zyris)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

**Zyris** é uma poderosa distribuição de engine de jogo baseada na [Godot Engine](https://godotengine.org), desenvolvida por **Kaffyn** e contribuidores. Nossa missão é expandir os limites do desenvolvimento de jogos de código aberto, implementando recursos avançados e otimizações que rivalizam com engines líderes da indústria como Unreal Engine e Unity.

## Visão

O Zyris não é um fork destinado a contribuições upstream para a Godot Engine. Em vez disso, somos uma **distribuição paralela** que:

- **Estende as capacidades da Godot** com recursos de ponta
- **Otimiza a performance** para o desenvolvimento de jogos de alto nível
- **Implementa inovações proprietárias** mantendo os princípios de código aberto
- **Foca em fluxos de trabalho e ferramentas de produção AAA**

Todo o crédito para a engine base vai para o [repositório official da Godot Engine](https://github.com/godotengine/godot) e sua incrível comunidade de contribuidores.

## Versionamento e Política de Sincronização

O Zyris mantém sincronização contínua com o repositório official da Godot Engine para garantir compatibilidade e acesso aos últimos recursos e correções de bugs.

### Formato de Versão

**Formato de Versão Zyris:** `Godot-X.Y.Z-zyris.N`

- **X.Y.Z** - Versão official da Godot Engine (ex: `4.5.0`)
- **N** - Incremento de subversão do Zyris (ex: `zyris.1`, `zyris.2`)

**Versão Atual:** `4.5.2-zyris.1`

### Estratégia de Sincronização

- **Rastreamento Upstream:** O Zyris sincroniza automaticamente com o último lançamento estável da Godot
- **Rebasing Automatizado:** Workflows do GitHub Actions realizam rebase contínuo das mudanças do Zyris sobre as atualizações oficiais da Godot
- **Compatibilidade Primeiro:** Todos os recursos do Zyris são projetados para serem compatíveis com as mudanças upstream
- **Cadência de Lançamento:** Novas subversões do Zyris são lançadas após cada atualização maior da Godot e quando recursos significativos são adicionados

Esta abordagem garante que os usuários do Zyris se beneficiem tanto da estabilidade dos lançamentos oficiais da Godot quanto dos recursos avançados exclusivos do Zyris.

## Roadmap

O Zyris está implementando um conjunto abrangente de sistemas. Abaixo está nosso roadmap de desenvolvimento:

### Implementado

- [x] **Virtual Input Devices** - Camada de abstração de entrada multi-plataforma

  Um sistema completo de entrada virtual integrado diretamente ao core da engine, fornecendo controles de toque fluidos para dispositivos móveis e híbridos.

  **Nós de Interface:**
  - `VirtualButton` - Botão de toque configurável com feedback visual e mapeamento de ações
  - `VirtualJoystick` - Controle de analógico com zonas mortas e sensibilidade customizáveis
  - `VirtualDPad` - Pad directional com suporte a 4 e 8 direções
  - `VirtualTouchPad` - Área de gestos multi-toque para controle de câmera e viewport

  **Recursos Principais:**
  - **Integração de Feedback Háptico** - Suporte a vibração para dispositivos móveis e gamepads
  - **Rastreamento de Dispositivo de Entrada** - API `LastInputType` detecta e rastreia automaticamente o método de entrada ativo (Toque, Teclado/Mouse, Gamepad)
  - **Adaptação de UI Dinâmica** - Controles virtuais aparecem/ocultam automaticamente com base no dispositivo detectado
  - **Modos de Visibilidade** - `ALWAYS`, `TOUCHSCREEN_ONLY`, `WHEN_PAUSED` para comportamento flexível de UI
  - **Mapeamento de Ações** - Integração direta com o sistema InputMap da Godot
  - **Sistema de Entrada Polimórfico** - A classe base `InputEvent` agora usa métodos virtuais para correspondência de ações e comparação de eventos, permitindo que eventos virtuais customizados se integrem perfeitamente ao core da engine como cidadãos de primeira classe.
  - **Integração com o Editor** - Suporte total no inspetor com configuração visual
  - **Otimizado para Performance** - Overhead mínimo com manipulação eficiente de eventos

### Em Desenvolvimento

- [ ] **Editor de Recursos & Biblioteca** - Ferramentas avançadas de composição de dados

  Um espaço de trabalho dedicado no Painel Principal para edição de Resources, tratando dados como cidadãos de primeira classe ao lado de Scripts e Cenas.

  **Resource:**
  - **Interface tipo IDE:** Layout em split-view com barra lateral para recursos recentes e área de edição central.
  - **Modos Duplos:** Alternância fluida entre **Modo Visual** (Baseado em Grafos/Nós) e **Modo Código** (Visualização de texto serializado).
  - **Filtragem Inteligente:** Exibe apenas propriedades "Editor" relevantes, filtrando metadados internos e ruídos de armazenamento, espelhando a visão limpa do Inspetor.
  - **Integração de Script:** Botão dedicado "Editar Script" para pular instantaneamente para a lógica do recurso no Editor de Scripts.

  **Library:**
  - **Navegador de Assets:** Um gerenciador de assets focado em visualização em grade e agrupamento avançado, substituindo a estrutura tradicional de árvore de pastas.
  - **Workbench:** Um "Inspetor Rápido" ciente do contexto para ajustes imediatos em assets selecionados sem trocar de contexto.
  - **CraftTable:** Uma bancada especializada que filtra e exibe apenas recursos compatíveis com o Zyris (Domínios registrados) para criação agilizada.
  - **Ações de Contexto:** Operações de Renomear e Deletar integradas.

- [ ] **Gameplay Ability System (GAS)** - Engine de gameplay e combate orientada a dados
  - Implementação nativa em C++ de `AbilitySystemComponent`, `GameplayAbility`, `GameplayEffect` e `AttributeSet`.
  - Execução consciente de contexto usando `GameplayTags` de alta performance.
  - Fluxo de execução determinístico otimizado para combate em tempo real.

- [ ] **Level Streaming System (LSS)** - Orquestração de mundo e gerenciamento de estado
  - **Arquitetura Yggdrasil:** `LSSServer` nativo gerenciando o ciclo de vida da engine via Máquina de Estados (GSM).
  - **Sistema de StreamingZone:** Carregamento em background com pré-cache espacial inteligente.
  - **Arquitetura Super Node:** `LSSRoot` isola mundos de jogo para efeitos de transição avançados e gestão de universo.

- [ ] **Save System** - API de persistência de alta performance
  - `SaveServer` nativo provendo gestão centralizada de estado e E/S assíncrona.
  - Serialização binária multi-thread com arquitetura baseada em slots.
  - Integrado profundamente ao LSS para snapshots automáticos de mundo e restauração de estado.

- [ ] **Camera System (vCam)** - Arbitragem cinematográfica e blending dinâmico
  - **Arquitetura de Câmera Virtual:** Blending baseado em prioridade integrado nativamente ao `Camera3D` e `Camera2D`.
  - **Arbitragem Nativa:** `CameraServer` atua como árbitro central para seleção de vCam ativa.
  - **Shake Procedural:** Sistema avançado de trauma baseado em ruído de Perlin para comportamento de câmera realista.

- [ ] **Inventory System** - Gestão escalável de itens e transações
  - `InventoryServer` centralizado para transações de itens e sistema de loot autoritativo.
  - Recursos de `ItemResource` orientados a dados e `InventoryContainer` nativo para armazenamento persistente.
  - Arquitetura baseada em componentes para integração fluida de UI (slots, grids, hotbars).

- [ ] **Editor de Recursos & Biblioteca** - Ferramentas avançadas de composição de dados

  Um espaço de trabalho dedicado no Painel Principal para edição de Resources, tratando dados como cidadãos de primeira classe ao lado de Scripts e Cenas.

  **Resource:**
  - **Interface tipo IDE:** Layout em split-view com barra lateral para recursos recentes e área de edição central.
  - **Modos Duplos:** Alternância fluida entre **Modo Visual** (Baseado em Grafos/Nós) e **Modo Código** (Visualização de texto serializado).
  - **Filtragem Inteligente:** Exibe apenas propriedades "Editor" relevantes, filtrando metadados internos e ruídos de armazenamento, espelhando a visão limpa do Inspetor.
  - **Integração de Script:** Botão dedicado "Editar Script" para pular instantaneamente para a lógica do recurso no Editor de Scripts.

  **Library:**
  - **Navegador de Assets:** Um gerenciador de assets focado em visualização em grade e agrupamento avançado, substituindo a estrutura tradicional de árvore de pastas.
  - **Workbench:** Um "Inspetor Rápido" ciente do contexto para ajustes imediatos em assets selecionados sem trocar de contexto.
  - **CraftTable:** Uma bancada especializada que filtra e exibe apenas recursos compatíveis com o Zyris (Domínios registrados) para criação agilizada.
  - **Ações de Contexto:** Operações de Renomear e Deletar integradas.

### Sistemas Core Planejados

- [ ] **Perception System** - Engine de percepção avançada
  - Integração com backend de física
  - Sistema de estímulos multi-sensoriais (Visual, Auditivo, Olfativo, Térmico)
  - Otimização espacial via BVH
  - Integração com memória Blackboard

- [ ] **Environmental System** - Simulação sistêmica de clima e ambiente
  - Simulação global de umidade e vento
  - Mecânicas de temperatura e sobrevivência
  - Sistemas de clima baseados em biomas
  - Ciclo dia/noite com simulação astronômica
  - Integração de shaders para efeitos atmosféricos

- [ ] **Behavior Tree System** - Sistema de IA híbrido
  - Editor visual (Painel Principal)
  - Pipeline de treinamento via Reinforcement Learning (backend em Python)
  - Integração de IA para dificuldade adaptativa
  - Execução multi-thread
  - Suporte a multiplayer determinístico

- [ ] **Camera System** - Gerenciamento avançado de câmera
  - Arquitetura de câmera virtual (inspirada em Cinemachine)
  - Tremor de câmera procedural (baseado em Perlin noise)
  - Blending de câmeras baseado em prioridade
  - Controles de zona morta e amortecimento
  - Integração com cutscenes

- [ ] **Audio Engine** - Sistema de áudio avançado
  - Sequenciador de música visual
  - Síntese de áudio procedural
  - Sistema de música adaptativa (transições verticais/horizontais)
  - Gerenciamento avançado de concorrência e vozes
  - Áudio espacial 3D com oclusão

- [ ] **Cutscene System** - Sequenciador cinematográfico
  - Sistema de cutscenes baseado em timeline
  - Trilhas semânticas (Câmera, Some, IA)
  - Restauração de estado
  - Edição não destrutiva

- [ ] **Quest System** - Sistema narrativo baseado em grafos
  - Editor visual de quests
  - Grafos de quest ramificados
  - Engine de sugestão consciente de contexto
  - Rastreamento de objetivos

- [ ] **Inventory System** - Gerenciamento unificado de items
  - Sistema de transações baseado em servidor
  - Tabelas de loot
  - Sistema de crafting
  - Components de UI (slots, hotbar)

- [ ] **AOT Export System (Zyris AOT Compiler)** - GDScript para código nativo

  **Zyris AOT Compiler (ZAC)** é um modo de exportação embutido que compila GDScript diretamente para código de máquina (binário nativo) no memento da exportação, garantindo performance e segurança máximas sem alterar o fluxo de desenvolvimento.

  **Objetivos Core:**
  - **Performance:** Eliminar o overhead da VM para igualar ou exceder a velocidade de execução interpretada.
  - **Segurança:** Fortalecimento contra engenharia reversa (removendo metadados, símbolos e código-fonte).
  - **Portabilidade:** Permitir execução em ambientes restritos (consoles) que proíbem JIT.

  **Arquitetura:**
  Integrado diretamente ao pipeline de exportação do Zyris, transformando GDScript -> C++ -> Binário Nativo de forma transparente.

  **Roadmap de Implementação:**
  - **Parte 1: Integração com a Engine**
    - [ ] Interface de Exportação e Flags (UI)
    - [ ] Geração de Pacote de Entrada (`.aot_input`)
    - [ ] Hooks de Pós-processamento
  - **Parte 2: Engine do Compilador (ZAC)**
    - [ ] Infraestrutura (C++, CMake)
    - [ ] Parser e AST (compatível com Godot)
    - [ ] Validação Semântica (subconjunto AOT-Safe)
    - [ ] IR Tipada Estaticamente e Otimizações
    - [ ] Geração de Backend C++ (`gen.cpp`)
    - [ ] Integração com Toolchain (MSVC, Clang, NDK)
    - [ ] Hardening e Stripping

## Instalação

### Construindo a partir do código-fonte

O Zyris usa o mesmo sistema de build da Godot Engine (SCons):

```bash
# Clone o repositório
git clone https://github.com/Kaffyn/ZyrisEngine.git
cd zyris

# Compile para sua plataforma
scons platform=windows target=editor
# ou
scons platform=linux target=editor
# ou
scons platform=macos target=editor
```

Para instruções detalhadas de compilação, veja a [documentação official da Godot](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html).

## Contribuição

O Zyris recebe contribuições de desenvolvedores que compartilham nossa visão de criar uma engine de jogo de classe mundial.

### Filosofia de Desenvolvimento

Seguimos o **"Godot Way"** para o desenvolvimento do core da engine:

- Adesão estrita às diretrizes de estilo C++ da Godot
- Uso de tipos da engine (`Vector<T>`, `String`, `Ref<T>`)
- Abordagem focada em performance com otimização de localidade de cache
- Documentação abrangente para todas as APIs públicas

Veja nosso [Manifesto de Desenvolvimento](.github/DEVELOPMENT.md) para diretrizes detalhadas.

### Fluxo de Trabalho de Contribuição

1. **Faça um Fork** do repositório
2. **Crie uma branch de recurso** (`git checkout -b feature/recurso-incrivel`)
3. **Commit suas mudanças** seguindo nosso formato de mensagem de commit
4. **Push para o seu fork** (`git push origin feature/recurso-incrivel`)
5. **Abra um Pull Request** com uma descrição clara

### Formato de Mensagem de Commit

```
Topic: Descrição curta no modo imperativo

Explicação detalhada do que mudou e por quê.
Referencie quaisquer issues relacionadas ou decisões de design.
```

**Exemplos:**

- `Core: Add virtual input device tracking API`
- `Rendering: Optimize shader compilation pipeline`
- `Editor: Implement advanced gesture recognition UI`

## Documentação

- **Documentação da Engine**: [Godot Docs](https://docs.godotengine.org) (referência da engine base)
- **Recursos Específicos do Zyris**: Veja `doc/classes/` para documentação XML
- **Guia de Desenvolvimento**: `.github/DEVELOPMENT.md`

## Estrutura do Projeto

```
zyris/
├── core/          # Tipos core da engine, matemática, gerenciamento de memória
├── scene/         # Sistema de cena, nós, GUI, 2D/3D
├── servers/       # Servidores de baixo nível de renderização, física e áudio
├── editor/        # Ferramentas do editor, plugins, inspetores
├── modules/       # Módulos opcionais e extensões
├── platform/      # Implementações específicas de plataforma
├── drivers/       # Drivers de hardware (renderização, áudio, entrada)
├── doc/           # Documentação XML para classes
└── tests/         # Testes unitários e de integração
```

## Licença

A Zyris Engine é licenciada sob a **Licença MIT**, mantendo compatibilidade com o licenciamento da Godot Engine.

```
Copyright (c) 2025-present Kaffyn and Zyris contributors
Copyright (c) 2014-present Godot Engine contributors
Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur

Veja LICENSE.txt e COPYRIGHT.txt para detalhes completos.
```

## Agradecimentos

O Zyris é construído sobre o trabalho incrível da comunidade da **Godot Engine**. Somos profundamente gratos a:

- **Juan Linietsky** e **Ariel Manzur** - Fundadores da Godot Engine
- Todos os **contribuidores da Godot Engine** - Por criarem uma base fantástica
- A **comunidade open-source** - Por tornar projetos como este possíveis

Para a lista completa de contribuidores da Godot, veja [AUTHORS.md](AUTHORS.md) e [DONORS.md](DONORS.md).

## Links

- **Godot Engine Official**: <https://godotengine.org>
- **GitHub da Godot**: <https://github.com/godotengine/godot>
- **Repositório do Zyris**: <https://github.com/Kaffyn/ZyrisEngine>
- **Rastreador de Issues**: <https://github.com/Kaffyn/ZyrisEngine/issues>

---

**Zyris Engine** - Expandindo os limites do desenvolvimento de jogos de código aberto
