# Resource Editor (Main Panel System)

**Technical Design Document v1.1**

| Metadata | Details |
| :--- | :--- |
| **Author** | Assistant (Lead Engine Dev) |
| **Reviewer** | Machi (Technical Director) |
| **Status** | **Implemented** |

---

## 1. Executive Summary

The **Resource Editor** is a core system of the Zyris Engine, integrated directly into the **Main Panel** (alongside 2D, 3D, and Script). It replaces the limited Inspector-only workflow, allowing any `.tres` or custom resource to be opened in a dedicated workspace with dual editing support: **Visual** and **Code**.

When a Resource is selected in the FileSystem, the Zyris Engine opens it simultaneously in the Inspector (for quick adjustments) and in the Main Panel (for deep architecture and editing).

**Key Goals:**

1. **Dual Editing:** Seamless switching between a graph-based editor (Visual) and a text-based editor (Code).
2. **First-Class Citizen:** Positioned in the Main Panel, acknowledging that data is as critical as scripts and scenes.
3. **Data Architecture:** Focused on component composition and managing the states of complex resources.

---

## 2. Interface and Workflow Modes

The Resource Editor occupies the central editor space and offers two viewing modes:

### 2.1 Visual Mode (Graph Mode)

A graph-based workbench for assembling the Resource hierarchy.

* **Composition:** Drag and drop components into the root resource.
* **Connections:** Link properties and events between different sub-resources.
* **Organization:** Visual management of layout metadata.

### 2.2 Code Mode (Text Mode)

An integrated text editor, similar to the Script editor, but focused on the Resource's serialized format.

* **Direct Editing:** Quickly modify raw values.
* **Refactoring:** Copy, paste, and replace serialized data blocks.
* **Debug:** See exactly how the Resource is being saved to disk.
* **Filtering:** Automatically filters out internal metadata (`metadata/_`) and non-editor properties to provide a clean, semantic view of the data.

---

## 3. Technical Architecture

The system resides in `editor/resource` and is orchestrated by the **ResourceServer** singleton.

*   **ResourceServer:** Manages registered domains (Zyris subsystems) and directs resources to their appropriate visual editors.
*   **ResourceEditor:** The main panel control, managing the split view between the Sidebar (recent files) and the Editor Area.

---

## 4. The Library

Integrated as the bottom panel, **The Library** serves as the primary asset browser for the Zyris ecosystem.

*   **Assets Tab:** A split-view file manager featuring a Folder Tree on the left and a visual Asset Grid on the right, enabling quick navigation and preview of project resources.
*   **CraftTable:** A specialized workbench that filters specifically for Zyris-registered domains (like Behavior Trees or Ability Blueprints), providing a focused environment for creating game data.

---

## 5. Conclusion

The **Resource Editor** removes the barrier between data design and technical implementation. By offering Visual and Code modes in the main panel, the Zyris Engine allows developers and designers to work on the same asset using the most efficient tool for each task, while maintaining the integrity and clarity of the project's data architecture.
