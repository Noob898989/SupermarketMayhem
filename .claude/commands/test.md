---
description: Führt geeignete Tests und Build-Prüfungen für Supermarket Mayhem durch und berichtet die Ergebnisse.
---

Führe für den aktuellen Projektstand (bzw. für: $ARGUMENTS) geeignete
Prüfungen durch:

1. Wenn C++ geändert wurde: Editor-Build ausführen (UnrealBuildTool, Target
   SupermarketMayhemEditor, Win64, Development) und Ergebnis
   (Erfolg/Fehler/Warnungen) berichten.
2. Wenn keine build-relevanten Änderungen vorliegen: das explizit
   feststellen, statt einen Build zu behaupten.
3. Prüfen, ob ein reproduzierbarer manueller Testschritt für die Änderung
   beschrieben werden kann (siehe Docs/TECHNICAL_ARCHITECTURE.md: "Each
   milestone needs a reproducible test"), und diesen auflisten.
4. Bei Unreal-Editor-Inhalten (Maps, Blueprints), die nicht automatisiert
   testbar sind: klar benennen, dass ein manueller Test im Editor nötig ist,
   und genau beschreiben, was zu prüfen ist.
5. Ergebnisse ehrlich berichten — nichts als getestet ausgeben, was nicht
   tatsächlich geprüft wurde (siehe CLAUDE.md Regel 10).
