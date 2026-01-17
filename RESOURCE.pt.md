# Resource Editor (Main Panel System)

**Documento de Design Técnico v1.1**

| Metadados | Detalhes |
| :--- | :--- |
| **Autor** | Assistente (Lead Engine Dev) |
| **Revisor** | Machi (Diretor Técnico) |
| **Status** | **Implementado** |

---

## 1. Resumo Executivo

O **Resource Editor** é um sistema central da Zyris Engine, integrado diretamente ao **Main Panel** (ao lado de 2D, 3D e Script). Ele substitui o fluxo limitado de edição exclusiva pelo Inspetor, permitindo que qualquer `.tres` ou resource customizado seja aberto em uma área de trabalho dedicada com suporte a edição dual: **Visual** e **Code**.

Ao selecionar um Resource no FileSystem, a Zyris Engine o abre simultaneamente no Inspetor (para ajustes rápidos) e no Main Panel (para arquitetura e edição profunda).

**Objetivos Chave:**

1. **Edição Dual:** Alternância fluida entre um editor de grafos (Visual) e um editor de texto (Code).
2. **Cidadão de Primeira Classe:** Posicionamento no Main Panel, reconhecendo que dados são tão críticos quanto scripts e cenas.
3. **Arquitetura de Dados:** Foco na composição de components e gerenciamento de estados de resources complexos.

---

## 2. Interface e Modos de Trabalho

O Resource Editor ocupa o espaço central do editor e oferece dois modos de visualização:

### 2.1 Modo Visual (Graph Mode)

Uma bancada de trabalho baseada em grafos para montar a hierarquia do Resource.

* **Composição:** Arrastar components para dentro do resource raiz.
* **Conexões:** Ligar propriedades e eventos entre diferentes sub-resources.
* **Organização:** Gerenciamento visual de metadados de layout.

### 2.2 Modo Code (Text Mode)

Um editor de texto integrado, similar ao editor de Scripts, mas focado no formato serializado do Resource.

* **Edição Direta:** Modificar valores brutos rapidamente.
* **Refatoração:** Copiar, colar e substituir blocos de dados serializados.
* **Debug:** Visualizar exatamente como o Resource está sendo salvo no disco.
* **Filtragem:** Filtra automaticamente metadados internos (`metadata/_`) e propriedades que não são de editor para fornecer uma visão limpa e semântica dos dados.

---

## 3. Arquitetura Técnica

O sistema reside em `editor/resource` e é orquestrado pelo singleton **ResourceServer**.

*   **ResourceServer:** Gerencia domínios registrados (subsistemas Zyris) e direciona resources para seus editores visuais apropriados.
*   **ResourceEditor:** O controle do painel principal, gerenciando a visualização dividida entre a Barra Lateral (arquivos recentes) e a Área de Edição.

---

## 4. The Library (A Biblioteca)

Integrada como o painel inferior, **The Library** serve como o principal navegador de assets para o ecossistema Zyris.

*   **Aba Assets:** Um gerenciador de arquivos com visualização dividida (Split-View), apresentando uma Árvore de Pastas à esquerda e uma Grade de Assets visual à direita, permitindo navegação rápida e pré-visualização dos recursos do projeto.
*   **CraftTable:** Uma bancada de trabalho especializada que filtra especificamente por domínios registrados no Zyris (como Behavior Trees ou Blueprints de Habilidade), proporcionando um ambiente focado para a criação de dados de jogo.

---

## 5. Conclusão

O **Resource Editor** remove a barreira entre o design de dados e a implementação técnica. Ao oferecer modos Visual e Code no painel principal, a Zyris Engine permite que desenvolvedores e designers trabalhem no mesmo asset usando a ferramenta mais eficiente para cada tarefa, mantendo a integridade e a clareza da arquitetura de dados do projeto.
