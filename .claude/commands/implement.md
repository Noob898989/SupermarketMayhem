---
description: Implementiert einen zuvor vom Nutzer freigegebenen Plan.
---

Setze den zuvor freigegebenen Plan um für: $ARGUMENTS

Vorgehen:
1. Wenn kein zuvor mit /plan erstellter und vom Nutzer freigegebener Plan
   vorliegt: stoppe und erstelle zuerst einen Plan (siehe /plan), bevor
   irgendetwas geändert wird.
2. Setze nur um, was im freigegebenen Plan steht. Erweiterungen oder
   Abweichungen erfordern erneute Rückfrage.
3. Halte dich an die Projektregeln aus CLAUDE.md und die Grenzen des jeweils
   zuständigen Agenten (.claude/agents/).
4. Arbeite in kleinen, nachvollziehbaren Schritten; zeige Änderungen, bevor
   sie übernommen werden, wenn Ausmaß oder Art der Änderung es nahelegen.
5. Dokumentiere Architekturentscheidungen in Docs/DECISIONS.md, falls
   während der Umsetzung nötig.
6. Führe nach Abschluss, sofern sinnvoll, einen Build/Test durch bzw. rufe
   den QA-Agenten auf (siehe /test).
7. Fasse am Ende zusammen: was wurde geändert, was wurde getestet, was ist
   offen.
