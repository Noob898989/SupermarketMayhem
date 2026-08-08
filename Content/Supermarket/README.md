# Content/Supermarket

Container fuer allen Supermarket-spezifischen Content (nicht Template-Content).

## Struktur

- `Maps/` - spielbare Level, z.B. `Lvl_SupermarketBlockout.umap`
- `Blueprints/Modules/` - wiederverwendbare Blockout-Bausteine (Wand-, Regal-, Kassen-Module)

Weitere Unterordner (`Meshes/`, `Materials/`, `Blueprints/Gameplay/`, ...) werden erst
angelegt, wenn tatsaechlich Content dafuer existiert.

Bis eigene Meshes/Materials fuer den Supermarkt existieren, wird bewusst auf
`Content/LevelPrototyping/` zurueckgegriffen (Blockout-Kit: SM_Cube, SM_ChamferCube,
SM_Plane, MI_PrototypeGrid_*).

Der konkrete Bauplan fuer Milestone 1 (Masse, Layout, Editor-Schritte) steht in
`Docs/BLOCKOUT_MILESTONE1.md`.
