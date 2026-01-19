# Library (Bottom Panel)

## 1. Resumo Executivo

**Library** é o sistema fundamental de gerenciamento de assets da Zyris Engine, residindo no **Painel Inferior**. Ela serve como um hub unificado para navegar pelos assets do projeto, realizar edições rápidas em recursos selecionados e criar novos dados de gameplay através de assistentes especializados.

Ela complementa o [Editor de Resource](file:///C:/Users/bruno/Desktop/Godot/RESOURCE.md) fornecendo o contexto do sistema de arquivos e o pipeline de criação.

---

## 2. Módulos Core (Abas)

A interface da Biblioteca é dividida em três abas estratégicas:

### 2.1 Painel Assets

Um navegador de assets robusto projetado para projetos de alta densidade.

- **Grade de Miniaturas:** Visualização focada em ícones e gnomes para identificação rápida.
- **Organização Avançada:** Suporte a agrupamento (por pasta, tipo) e ordenação dinâmica.
- **Filtragem:** Busca avançada por gnome e tipo de recurso.
- **Integração:** Clicar duas vezes abre o recurso no Painel Principal e o seleciona no **Workbench**.

### 2.2 Painel Workbench

Um "Inspetor Rápido" ciente do contexto para ajustes imediatos.

- **Iteração Rápida:** Edite propriedades do asset selecionado no memento sem perder o foco na viewport principal ou no script.
- **UI Direcionada:** Mostra um subconjunto das propriedades mais relevantes para o tipo de recurso selecionado.

### 2.3 Painel CraftTable

O principal ponto de entrada para criar recursos específicos da Zyris.

- **Filtragem de Domínios:** Em vez da lista genérica "Novo Recurso" da Godot, ele mostra os domínios registrados no `ResourceServer` (ex: Árvores de Comportamento, Quests, Items).
- **Assistente de Criação:** Fluxo de trabalho simplificado para instanciar hierarquias de recursos complexas.

---

## 3. Arquitetura Técnica

O sistema está localizado em `editor/library` e interage intimamente com o core da engine.

- **Library (VBoxContainer):** O container UI principal que gerencia o `TabContainer`.
- **LibraryFactory (C++ Estático):** A sala de máquinas para a criação de recursos. Fornece métodos como `create_resource_from_domain()` e `create_resource_by_class()`, garantindo que qualquer novo asset seja inicializado corretamente com os padrões Zyris.
- **Integração com ResourceServer:** A Biblioteca consulta o `ResourceServer` para popular a CraftTable com os domínios disponíveis e seus ícones associados.

---

## 4. Exemplo de Fluxo de Trabalho: "O Caminho Zyris"

1. **Criação:** O desenvolvedor abre **A Biblioteca -> CraftTable** e seleciona "Behavior Tree".
2. **Nomenclatura:** Uma janela solicita um caminho para salvar; a `LibraryFactory` cria o arquivo `.tres`.
3. **Edição:** O novo recurso abre automaticamente no Modo Visual do **Editor de Resource**.
4. **Ajuste Rápido:** Mais tarde, o desenvolvedor seleciona o asset em **A Biblioteca -> Assets** e usa o **Workbench** para ajustar uma única variável de "Velocidade" sem trocar de painel.

---

## 5. Conclusão

A Biblioteca fornece a camada organizacional necessária para a Zyris Engine. Ao separar descoberta (Assets), pequenos ajustes (Workbench) e criação (CraftTable), ela permite um fluxo de trabalho de desenvolvimento de jogos mais limpo e eficiente.
