# Proposta Técnica: Sistema Unificado de Gerenciamento de Recursos (Zyris Engine)

## Introdução

Este documento detalha a proposta de implementação de um **Sistema Unificado de Gerenciamento de Recursos** para a Zyris Engine. O objetivo é aprimorar significativamente a forma como desenvolvedores interagem, criam e manipulam ativos (recursos) dentro do projeto, elevando a qualidade técnica e a eficiência do fluxo de trabalho.

## O Problema Actual: Gerenciamento de Recursos Insuficiente

Atualmente, a Zyris Engine enfrenta limitações na gestão de recursos, o que impacta a produtividade e a qualidade do desenvolvimento:

- **Painel Inspector Limitado:** O painel `Inspector` é a principal ferramenta para editar propriedades de nós e recursos. No entanto, para recursos de gameplay complexos (como animações, shaders, dados de entidades, configurações de IA, etc.), o `Inspector` se torna sobrecarregado e inadequado para uma edição granular, visual ou baseada em lógica complexa. Ele é mais voltado para a edição de propriedades de instâncias de cena do que para a autoria de recursos independentes.
- **Sistema de Arquivos Não Otimizado para Recursos:** Embora o `EditorFileSystem` gerencie arquivos de forma eficiente, ele não oferece uma camada de abstração otimizada para a descoberta, categorização e manipulação de _recursos_ como entidades lógicas. A navegação e a criação de recursos baseados em tipos específicos (e não apenas em arquivos) são tarefas que poderiam set grandemente melhoradas.

Essa lacuna resulta em um fluxo de trabalho menos intuitivo para a criação e edição de ativos, com dependência excessiva de código manual para gerenciar recursos complexos e uma experiência fragmentada para o desenvolvedor.

## Library

O painel `Library` atuará como o ponto central de descoberta, organização e criação de todos os recursos do projeto.

- **Funcionalidade:**
  - **Descoberta e Navegação:** Similar à familiaridade e eficiência do `EditorFileSystem`, ele fornecerá uma visão navegável e pesquisável de todos os recursos no projeto, incluindo recursos baseados em arquivos (`.tres`, `.res`) e recursos internos a cenas (`.tscn`).
  - **Criação via Factory:** Incorporará um sistema de _factory_ robusto. Através de uma interface intuitiva, desenvolvedores poderão selecionar tipos de recursos registrados (através do `ResourceServer` e `ClassDB`) e criar novas instâncias de recursos de forma rápida e padronizada.
  - **Pesquisa e Filtragem:** Capacidade de filtrar recursos por tipo, gnome, metadados ou tags, permitindo uma localização rápida e eficiente.
- **Inspiração:** Fortemente inspirado na estrutura e navegação do `EditorFileSystem`, garantindo uma transição suave para os desenvolvedores.

## Resource Editor

O `Resource Editor` será um painel de edição principal, comparável em importância ao `Script Editor`, mas com capacidades ainda mais avançadas para a autoria de recursos complexos.

- **Funcionalidade:**
  - **Painel Principal Dedicado:** Posicionado como um painel de edição de primeira linha, permitindo a edição detalhada de recursos sem a necessidade de abrir múltiplos arquivos ou alternar constantemente entre o inspetor e a cena.
  - **Edição Avançada (Graph/Node):** Suportará interfaces visuais sofisticadas, incluindo funcionalidades de _graph/node editing_. Isso permitirá a criação e manipulação de recursos com estruturas complexas, como sistemas de nós de shaders visuais, pipelines de animação, fluxos de diálogo, ou lógicas de IA baseadas em nós.
  - **Integração Contextual:** Manterá a capacidade de inspecionar e editar propriedades de forma detalhada, similar ao `EditorInspector`, mas dentro de um contexto de edição de recurso mais amplo e poderoso.
- **Inspiração:** Combinará a natureza contextual e de edição de propriedades do `EditorInspector` com a flexibilidade visual e a capacidade de edição procedural do `Script Editor`, criando um ambiente de autoria de recursos unificado e de alta performance.

## Arquitetura e Integração

A integração desses novos components com a arquitetura existente da Zyris Engine seguirá os padrões estabelecidos:

- **Resource Server (`editor/resource`):** Continuará atuando como o singleton central, responsável pela lógica de negócios e registro de "Domínios" de recursos. Ele gerenciará a comunicação entre a Library, o Editor e o `ClassDB`.
- **ClassDB:** Essential para a validação dinâmica de tipos, herança e descoberta de propriedades, garantindo que tanto a Library quanto o Editor possam interagir com qualquer tipo de recurso registrado de maneira consistente.
- **EditorNode:** A `Library` será integrada como um cidadão de primeira classe no `BottomPanel` ou em um painel dedicado, enquanto o `ResourceEditor` poderá set aberto em um painel de edição principal sob demanda.
- **Drag-and-Drop:** Será implementado um protocolo de dados unificado para a transferência de recursos entre a Library, o Editor e outras partes da interface, garantindo a compatibilidade com o `EditorResourcePicker` e propriedades do Inspetor.

## Padrões de Implementação e Performance

A implementação seguirá estritamente os padrões da Zyris Engine:

- **Memory Management:** Uso de `Ref<T>` para recursos `RefCounted` e `memnew`/`memdelete` para components de UI.
- **Data Structures:** Preferência por `Vector`, `String`, `HashMap` internos em detrimento de tipos da STL.
- **Multithreading:** O carregamento e a indexação de recursos na Library utilizarão threads separadas para manter a UI responsiva.
- **Estilo e Tipagem:** Aderência ao `clang-format` e uso dos tipos internos da engine (`real_t`, `int32_t`, etc.).

## Próximos Passos

1. **Refinamento do Parser de Sub-recursos:** Otimizar a leitura de IDs internos para um carregamento mais eficiente.
2. **Desenvolvimento do Sistema de Factory:** Implementar a criação dinâmica de recursos baseada em tipos registrados.
3. **Criação da UI da Library:** Implementar a navegação, pesquisa e visualização de recursos.
4. **Prototipagem do Resource Editor:** Desenvolver um framework inicial para graph/node editing aplicável a recursos.
5. **Integração com ClassDB e ResourceServer:** Estabelecer a comunicação fluida entre os novos components e o sistema existente.

---

**Status:** Proposta em Análise/Aprovação
**Líder Técnico:** Machi
**Padrão de Qualidade:** Godot-Idiomatic C++ / Zyris Core Standard
