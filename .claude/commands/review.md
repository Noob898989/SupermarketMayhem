---
description: Prüft bestehende Änderungen auf Architektur-, Gameplay- und Regressionsprobleme.
---

Prüfe die aktuellen Änderungen (git diff / letzte Commits, sofern nicht
anders angegeben: $ARGUMENTS) auf:

1. Architekturprobleme (Verstoß gegen Docs/TECHNICAL_ARCHITECTURE.md,
   Docs/MULTIPLAYER.md, insbesondere Server-Autorität).
2. Gameplay-Konsistenz (Verstoß gegen Docs/GAME_DESIGN.md,
   Docs/GAMEPLAY.md, oder gegen bereits getroffene Entscheidungen in
   Docs/DECISIONS.md).
3. Regressionen (bestehende, bisher funktionierende Systeme beschädigt?).
4. Verstöße gegen die Projektregeln aus CLAUDE.md und .claude/agents/ (z.B.
   eigenmächtige Design-Entscheidungen, verfrühte Steam-/Multiplayer-
   Implementierung, gelöschte Dateien ohne Freigabe, kostenpflichtige
   Plugins, Zugangsdaten im Repository).

Melde Befunde priorisiert (kritisch/mittel/gering), mit Datei und Zeile,
ohne sie automatisch zu beheben, sofern nicht ausdrücklich mit "und behebe"
angefordert.
