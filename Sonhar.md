# O Sonhar

> **Conceito:** O Laboratório de Criação & IDE Visual
> **Status:** Core System

O **Resource Editor** (anteriormente codinome "Sonhar") não é apenas uma pasta de assets. É o **Main Panel central do Zyris**, uma IDE completa dentro do Godot para criação, inspeção e composição visual de dados e sistemas.

Se o **Yggdrasil** é o corpo do mundo, o **Resource Editor** é a mente onde ele é concebido.

---

## Filosofia & Identidade

O Resource Editor resolve o problema da "Invisible Data" em engines de jogos. Em vez de editar Resources complexos (como Árvores de Habilidade, Quests ou Loot Tables) em listas verticais infinitas no Inspector padrão, ele oferece interfaces visuais ricas e contextuais.

### O Editor do Editor

1. **Hub Central:** Ele não implementa lógica de gameplay, mas fornece a bancada onde essa lógica é configurada.
2. **Infraestrutura de Injeção:** Os outros módulos (Behavior Tree, Ability System, Gaia) "injetam" seus editores dentro do Resource Editor.
3. **Independência:** Se um módulo for desabilitado, o editor continua existindo, mostrando os dados daquele módulo como "Dormant" (Leitura apenas).

---

## Arquitetura (Infrastructure)

Assim como todos os sistemas Zyris, o Resource Editor é construído sobre a rocha do C++ (GDExtension), seguindo a trindade Server-Resource-Node.

### 1. ResourceServer (C++ Singleton)

O backend invisível. Não é um Node, não está na SceneTree.

- **Função:** Gerencia o registro de **Domínios**.
- **Responsabilidade:**
  - Mantém o catálogo de todos os `Blueprints` e `Resources` do projeto.
  - Fornece API para que outros módulos registrem suas ferramentas (`register_domain("Behavior Tree", behavior_tree_editor_scene)`).
  - Indexação de busca rápida para a Biblioteca.

### 2. ResourceEditor (Interface Dual)

A manifestação visual (Frontend) dividida em duas áreas estratégicas, com um layout similar ao Script Editor:

- **Main Panel (Resource Editor):**
  - **Função:** Área de trabalho principal com suporte a **Abas**.
  - **Modos:** Alternância entre **Visual** (Grafo) e **Código** (Texto Serializado).
  - **Sidebar:** Lista de recursos recentes e filtro de busca.

- **Bottom Panel (A Biblioteca):**
  - **Função:** O Browser de assets unificado e "File System" de dados.
  - **Layout:** Split-View com Árvore de Pastas à esquerda e Grade de Assets à direita.

### 3. Components Base (A Meta-Engine)

O Resource Editor não é apenas um container UI. Ele fornece a infraestrutura técnica para que os módulos funcionem.

#### 3.1. Visual Components (Interface / Nodes)

Unidades visuais C++ (`GDExtension`) que são instanciadas como **Abas** no painel principal.

- **`SonharGraph`:** Editor de gráficos node-based. Base para BTs, Quests e Skills.
- **`SonharSequencer`:** Editor de timeline linear. Base para Áudio e Cutscenes.
- **`SonharBoard`:** Editores espaciais 2D/3D (Mapas, Grids).
- **`SonharTable`:** Planilhas de alta performance para dados massivos.

#### 3.2. Data Components (Contratos / Resources)

Classes C++ que os módulos devem estender para garantir compatibilidade com a interface.

- **`SonharNodeResource`:** Contém metadados de grafo (posição `Vector2`, título, slots). Estendido por `BTTask`.
- **`SonharTrackResource`:** Contém dados de sequenciamento (keyframes, tracks). Estendido por `MusicTrack`.
- **`SonharCellResource`:** Unidade de dado espacial ou tabular.

---

## Estrutura Interna

### A Oficina (Bottom Panel - 3 Tabs)

O "Cinto de Utilidades", sempre disponível para dar suporte à criação.

1. **Assets (Tab 1):** O buscador universal.
   - **Esquerda:** Árvore de diretórios do projeto.
   - **Direita:** Grade visual de assets (ícones) com suporte a Drag & Drop.
2. **Workbench (Tab 2):** Inspector rápido para ajustes finos sem abrir a aba completa.
3. **CraftTable (Tab 3):** Wizards para criação e composição rápida de novos resources.
   - **Filtro:** Exibe apenas resources registrados no Zyris (Domínios), facilitando a criação de dados específicos do jogo.

### Os Domínios (Main Panel)

As implementações concretas dos módulos usando os components do Resource Editor.

| Módulo            | Componente Base   | Recurso Base (`Data`)                |
| :---------------- | :---------------- | :----------------------------------- |
| **Behavior Tree** | `SonharGraph`     | `BTTask` (`SonharNodeResource`)      |
| **Gaia**          | `SonharBoard`     | `BiomeMap` (`SonharCellResource`)    |
| **Sounds**        | `SonharSequencer` | `MusicTrack` (`SonharTrackResource`) |
| **Quest**         | `SonharGraph`     | `QuestNode` (`SonharNodeResource`)   |

---

## Mecânica de Injeção (IoC)

O Resource Editor é agnóstico. O Behavior Tree diz "Eu existo e uso estes components".

```cpp
// Example: BehaviorTree registering itself to ResourceServer
void BehaviorTreeModule::initialize() {
    ResourceServer::get_singleton()->register_domain(
        "Behavior Tree",       // Visual Name
        "BehaviorTree",        // Resource Type
        "SonharGraph",         // Base Visual Class (Factory)
        icon_texture           // Icon
    );
}
```

### Ciclo de Vida Editorial

1. **Draft (Rascunho):** Um resource criado começa como rascunho.
2. **Compiled (Compilado):** Ao salvar, o editor valida os dados e "compila" o Resource final otimizado.
3. **Baked:** Para dados pesados (navegação, luz), dispara bakers em background.

---

## Resumo para o Arquiteto

- **Gnome:** Library (Painel Inferior).
- **Tipo:** Ferramenta / Editor (Only-Editor na maior parte).
- **Singleton:** `ResourceServer` (Gerenciador de Plugins/Domínios).
- **Meta:** Tornar o Godot Inspector obsoleto para Game Design de alto nível.
