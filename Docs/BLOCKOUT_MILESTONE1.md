# Supermarket Mayhem - Blockout Milestone 1

**Status: Umgesetzt.** Der Blockout wurde im Editor gebaut und ist als
`Content/Supermarket/Maps/Lvl_SupermarketBlockout.umap` committed (siehe
Commit `c6d7746`, "Complete AP2 supermarket blockout and AP3 round
system"). Siehe Docs/ROADMAP.md, Phase 1, fuer den aktuellen
Gesamtstatus.

Bauplan fuer den ersten spielbaren Supermarkt-Blockout. Dieses Dokument ist die
Referenz fuer die manuellen Editor-Schritte, da Maps und Blueprints im Unreal
Editor erstellt und validiert werden.

Alle Masse in Unreal-Einheiten = cm. Referenz: Spielercapsule Radius 34cm,
Halbhoehe 96cm (Gesamthoehe ~192cm), siehe `SupermarketMayhemCharacter.cpp`.

## Ziel-Map

`Content/Supermarket/Maps/Lvl_SupermarketBlockout.umap`

## Grundraster

- Koordinatensystem: X = Tiefe (0 = Eingang, wachsend Richtung Laden-Rueckwand),
  Y = Breite (0 = Mittelachse, negativ = links, positiv = rechts), Z = Hoehe.
- Deckenhoehe: 400cm
- Wandstaerke: 20cm
- Empfehlung: Editor-Grid-Snapping auf 50cm stellen, waehrend des Blockouts.

## Zonen (entlang X, Ladentiefe ca. 21m gesamt)

| Zone | X-Bereich | Y-Bereich | Inhalt |
|---|---|---|---|
| Eingang | 0 - 300 | -800 bis 800 (Wand), Tueroeffnung -150 bis 150 | BP_DoorFrame (vorhanden, aus `LevelPrototyping/Interactable/Door`) |
| Kassenbereich | 300 - 700 | -800 bis 800 | 4x Checkout-Modul bei X=500, Y = -600 / -200 / 200 / 600 |
| Backoffice-Nische ("bei den Kassen") | 300 - 800 | 800 - 1400 (seitlicher Anbau, Y ueber 800 hinaus) | kleiner Raum ca. 5x5m, Tuer bei X=500/Y=800 Richtung Kassenbereich |
| Regal-/Gangbereich | 700 - 2100 | -800 bis 800 | 4 Regalreihen parallel zu X, siehe unten |
| Kreuzung (Querweg) | 1300 - 1600 | -800 bis 800 | 300cm breiter Durchgang quer durch alle 4 Regalreihen |
| Rueckwand | 2100 | -800 bis 800 | schliesst den Laden ab |

## Regalreihen (im Regal-/Gangbereich)

4 Reihen bei Y = -600, -200, 200, 600 (Abstand 400cm = Gangbreite zwischen den Reihen).
Abstand aeusserste Reihe zur Seitenwand (Y=-800/800): 200cm Randgang.

Jede Reihe ist durch die Kreuzung (X 1300-1600) unterbrochen:
- vorderes Segment: X 700 - 1300 (6m lang)
- hinteres Segment: X 1600 - 2100 (5m lang)

Ergebnis: 3 begehbare Innengaenge (je 400cm breit) + 2 Randgaenge (je 200cm breit),
insgesamt mindestens 3 Kreuzungspunkte (jeder Innengang x Querweg).

## Wiederverwendbare Blueprint-Module

Alle Module: neuer Blueprint (Actor-Basisklasse) mit einer StaticMeshComponent,
Mesh aus `Content/LevelPrototyping/Meshes/`. Anlegen unter
`Content/Supermarket/Blueprints/Modules/`.

Segment-Ansatz (1 Meter Bausteine, per Alt+Drag entlang des Grids dupliziert) statt
fixer Grosslaengen - dadurch bleibt die Anordnung leicht anpassbar.

| Blueprint | Basis-Mesh | Groesse (L x T x H, cm) | Material | Verwendung |
|---|---|---|---|---|
| `BP_WallSegment_1m` | SM_Cube | 100 x 20 x 400 | MI_PrototypeGrid_Gray | Aussenwaende, entlang duplizieren |
| `BP_ShelfSegment_1m` | SM_Cube | 100 x 60 x 200 | MI_PrototypeGrid_TopDark (o.ae., optisch abgesetzt von Waenden) | Regalreihen, entlang duplizieren; je 2 Reihen Rueckenan-Ruecken fuer beidseitige Regale |
| `BP_CheckoutCounter` | SM_ChamferCube | 150 x 60 x 100 | MI_DefaultColorway | einzeln platzieren, 4x im Kassenbereich |

Fuer den Boden: `SM_Plane` einmal auf die Gesamtflaeche skalieren, Material
`MI_PrototypeGrid_Gray` (Grid hilft beim Einschaetzen von Massstab/Sichtlinien).

Tueren/Rahmen: vorhandenes `BP_DoorFrame` + `SM_Door` aus
`LevelPrototyping/Interactable/Door` wiederverwenden (Eingang, Backoffice-Nische) -
keine neuen Tuer-Assets noetig.

## Editor-Schritte (manuell, siehe Chat-Antwort fuer den Grund)

1. **Map anlegen:** File > New Level > Empty Level. Save As ->
   `Content/Supermarket/Maps/Lvl_SupermarketBlockout`.
2. **Module anlegen:** Im Content Browser nach `Content/Supermarket/Blueprints/Modules`
   navigieren, Rechtsklick > Blueprint Class > Actor. Je Modul: StaticMeshComponent
   hinzufuegen, Static Mesh + Material gemaess Tabelle setzen, Component-Scale/Groesse
   gemaess Tabelle einstellen, Compile + Save.
3. **Grid-Snapping aktivieren** (Editor-Symbolleiste, Movement-Snap auf 50cm).
4. **Boden platzieren:** SM_Plane einmal einfuegen, auf Gesamtflaeche skalieren.
5. **Waende, Regale, Kassen platzieren:** erstes Modul exakt positionieren (Details-Panel,
   Transform gemaess obiger Tabelle), dann mit Alt+Drag entlang der Achse duplizieren,
   bis die jeweilige Zone gefuellt ist. Beim Regalbereich die Kreuzungs-Luecke
   (X 1300-1600) frei lassen.
6. **Tueren setzen:** BP_DoorFrame am Eingang (X=0, Y=0) und an der
   Backoffice-Nische platzieren.
7. **Player Start setzen:** eine `PlayerStart`-Actor in den Eingangsbereich
   (z.B. X=150, Y=0, Z=100) platzieren - ohne PlayerStart ist die Map nicht spielbar.
8. **GameMode pruefen:** World Settings > GameMode Override entweder leer lassen
   (nutzt globalen Default `BP_FirstPersonGameMode` aus DefaultEngine.ini) oder
   explizit `BP_FirstPersonGameMode` setzen.
9. **Speichern, dann Play testen** (siehe Testschritte in der Chat-Antwort).

## Bekannte offene Punkte

- Die exakten Zahlen sind ein Startpunkt, kein Endergebnis - laut Auftrag soll die
  Anordnung leicht veraenderbar bleiben.
- Beleuchtung ist in diesem Milestone nicht spezifiziert (kein Lighting-Setup
  vorgegeben); Default-Lighting aus "Empty Level" reicht fuer einen Walkthrough-Test.
- Kollision: Standard-Cube/Plane-Meshes haben bereits Box-Collision; keine
  zusaetzliche Konfiguration noetig.
