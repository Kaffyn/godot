# Editor de Resource (Sistema do Painel Principal)

## 1. Resumo Executivo

O **Resource Editor** é um sistema central da Zyris Engine, integrado diretamente no **Painel Principal** (junto com 2D, 3D e Script). Ele substitui o fluxo limitado de apenas Inspetor, permitindo que qualquer `.tres` ou resource customizado seja aberto em um espaço de trabalho dedicado com suporte a edição dupla: **Visual** e **Código**.

Quando um Resource é selecionado no FileSystem, a Zyris Engine o abre simultaneamente no Inspetor (para ajustes rápidos) e no Painel Principal (para arquitetura profunda e edição).

**Objetivos Chave:**

1. **Edição Dupla:** Alternância perfeita entre um editor baseado em grafos (Visual) e um editor baseado em texto (Código).
2. **Cidadão de Primeira Classe:** Posicionado no Painel Principal, reconhecendo que os dados são tão críticos quanto os scripts e as cenas.
3. **Arquitetura de Dados:** Focado na composição de components e na gestão de estados de recursos complexos.

---

## 2. Interface e Modos de Fluxo de Trabalho

O Resource Editor ocupa o espaço central do editor e oferece dois modos de visualização:

### 2.1 Modo Visual (Modo Grafo)

Uma bancada baseada em grafos para montar a hierarquia do Resource.

* **Composição:** Arraste e solte components no recurso raiz.
* **Conexões:** Vincule propriedades e eventos entre diferentes sub-recursos.
* **Organização:** Gerenciamento visual de metadados de layout.

### 2.2 Modo Código (Modo Texto)

Um editor de texto integrado, similar ao editor de Script, mas focado no formato serializado do Resource.

* **Edição Direta:** Modifique valores brutos rapidamente.
* **Refatoração:** Copie, cole e substitua blocos de dados serializados.
* **Depuração:** Veja exatamente como o Resource está sendo salvo no disco.
* **Filtragem:** Filtra automaticamente metadados internos (`metadata/_`) e propriedades que não são do editor para fornecer uma visão limpa e semântica dos dados.

---

## 3. Arquitetura Técnica

O sistema reside em `editor/resource` e é orquestrado pelo singleton **ResourceServer**.

* **ResourceServer:** Gerencia domínios registrados (subsistemas Zyris). Outros módulos se registram via `register_domain(name, type, visual_class, icon, rules)`, direcionando resources para seus editores visuais apropriados.
* **ResourceEditor:** O controle do painel principal.
  * **Sidebar:** Gerencia a lista e filtragem de resources abertos.
  * **Toolbar:** Controles para alternância de modo **Visual/Código** e o botão **Edit Script** (ativo quando o resource tem um script anexado).
  * **Editor Area:** Hospeda o `custom_editor` (do domínio) ou a bancada padrão `ResourceGraphNode`.

---

## 4. A Biblioteca (The Library)

A **Library** é o painel inferior especializado para gerenciamento de assets e criação rápida.

> [!NOTE]
> Para documentação detalhada sobre os subsistemas da Library (Assets, Workbench, CraftTable), consulte o arquivo [LIBRARY.md](file:///C:/Users/bruno/Desktop/Godot/LIBRARY.md).

---

## 5. Conclusão

O **Resource Editor** remove a barreira entre o design de dados e a implementação técnica. Ao oferecer os modos Visual e Código no painel principal, a Zyris Engine permite que desenvolvedores e designers trabalhem no mesmo asset usando a ferramenta mais eficiente para cada tarefa, mantendo a integridade e a clareza da arquitetura de dados do projeto.
