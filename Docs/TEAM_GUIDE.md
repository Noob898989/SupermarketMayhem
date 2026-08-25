# Supermarket Mayhem - Team Guide

## 1. Projektübersicht

**Projekt:** Supermarket Mayhem  
**Genre:** First-Person Multiplayer Prop-Hunt / Social Action  
**Plattform:** PC / Steam  
**Engine:** Unreal Engine 5.8  
**Programmierung:** C++ + Blueprints  
**Zielgröße:** 2–8 Spieler  
**Repository:** https://github.com/Noob898989/SupermarketMayhem  
**Projektleitung:** Noob898989  
**Team:** 2 Personen

### Kurzbeschreibung

Supermarket Mayhem ist ein First-Person-Multiplayer-Prop-Hunt-Spiel in einem lebendigen Supermarkt.

Die Hider verwandeln sich in gewöhnliche Supermarktprodukte und versuchen, sich möglichst glaubwürdig zu verstecken. Die Hunter suchen nach verdächtigen Objekten, beobachten Bewegungen und nutzen Waffen, um die Hider auszuschalten.

Die zentrale Spielidee lautet:

> Hide as a product while the supermarket keeps running.

Der Supermarkt soll sich während der Runde lebendig anfühlen. NPC-Kunden, Geräusche, bewegliche Objekte, Physik und alltäglicher Supermarkt-Betrieb sorgen für Ablenkung und Chaos.

---

## 2. Vision

1. **Verstecken soll glaubwürdig und lustig sein:** Ein Hider soll wirklich wie ein normales Produkt im Supermarkt wirken können.
2. **Jede Runde soll Spannung und Chaos erzeugen:** Hunter beobachten die Umgebung, Hider müssen sich bewegen und Risiken eingehen.
3. **Der Supermarkt ist ein aktiver Teil des Spiels:** NPC-Kunden, Props, Physik, Geräusche und Umweltinteraktionen sollen die Jagd beeinflussen.

---

## 3. Aktueller Stand

### Aktueller Meilenstein

**Phase 0 – Foundation / Vorbereitung des ersten Playable**

Der aktuelle Projektstand:

- Unreal Engine 5.8 Projekt vorhanden
- First-Person-C++-Template funktioniert
- WASD-Bewegung funktioniert
- Mouse Look funktioniert
- Git/GitHub eingerichtet
- Projektdokumentation vorhanden
- Claude-Code-Grundstruktur vorhanden
- Game-Design und Gameplay-Grundlagen dokumentiert
- Multiplayer-Grundrichtung festgelegt
- Roadmap vorhanden

### Als Nächstes

**Phase 1 – First Playable**

Ziel:

> Der Spieler kann einen kleinen, begehbaren Supermarkt betreten und sich darin bewegen.

Prioritäten:

1. Supermarkt-Blockout
2. grundlegende Interaktion
3. Platzhalter-Produkte
4. saubere technische Grundlage für das spätere Prop-System

### Danach

1. Prop-/Disguise-System
2. Multiplayer-Replikation
3. erste Hunter-Waffe
4. Round-System
5. lebendiger Supermarkt mit NPCs
6. Steam-Party/Lobby/Matchmaking
7. Content und Polish
8. Playtesting
9. Release-Vorbereitung

---

## 4. Verantwortlichkeiten

Die Verantwortlichkeiten sind bewusst getrennt, damit möglichst wenig gleichzeitig an denselben Unreal-Assets gearbeitet wird.

### Projektleitung / Core Systems – Noob898989

Hauptverantwortung:

- Gameplay-Logik
- Core-Gameplay-Systeme
- Player-System
- Interaktionssystem
- Prop-/Disguise-System
- Waffen
- Damage/Elimination
- Round-System
- GameMode/GameState
- Multiplayer und Replikation
- Server Authority
- Steam-Integration
- Lobby / Party / Matchmaking
- technische Architektur
- GitHub-Projektstruktur
- technische Entscheidungen
- Integration der Systeme

### World / Content / Presentation – Teammitglied

Hauptverantwortung:

- Supermarkt-Map
- Level Design
- Blockout
- Umgebung
- Regale
- Kassenbereich
- Kühlschränke/Gefrierbereiche
- Backroom/Storage
- Props und Produktdarstellung
- Beleuchtung
- Environment-Art
- geeignete UI-/Presentation-Aufgaben
- Audio-/VFX-Unterstützung
- visuelle Atmosphäre
- Vorbereitung von Content für das Prop-System

### Gemeinsam

Beide sind verantwortlich für:

- Game Design
- Ideen und Verbesserungsvorschläge
- Playtesting
- Bugs melden
- Dokumentation
- wichtige Designentscheidungen
- Review des jeweils anderen Bereichs
- lokale Tests vor dem Push
- saubere Git-Commits

---

## 5. Grundregel zur Zusammenarbeit

Die beiden Arbeitsbereiche sollen möglichst unabhängig voneinander bleiben.

### Grundprinzip

**Core Systems** und **World/Content** werden getrennt entwickelt.

Beispiele:

- Der Gameplay-Entwickler baut das Prop-System.
- Der World-Entwickler erstellt die Supermarkt-Umgebung und liefert passende Produkt-Props.
- Das Prop-System soll so gebaut werden, dass neue Produkte möglichst ohne Änderung der Core-Logik hinzugefügt werden können.

### Wichtig

Nicht gleichzeitig dieselben wichtigen Unreal-Dateien bearbeiten, wenn es sich vermeiden lässt.

Besonders vorsichtig sein bei:

- `.umap` / Level-Dateien
- gemeinsam genutzten Blueprints
- zentralen Data Assets
- Core-C++-Dateien
- Projektkonfiguration
- Plugins

---

## 6. Git-Arbeitsweise

GitHub ist die gemeinsame Quelle für den Projektstand.

Jeder Entwickler arbeitet auf seinem eigenen Computer.

### Vor der Arbeit

Immer zuerst den aktuellen Stand holen:

```text
git pull
```

### Während der Arbeit

Lokal entwickeln und testen.

### Nach einer abgeschlossenen Aufgabe

```text
git status
git add .
git commit -m "Beschreibung der Änderung"
git push
```

Nur getestete und nachvollziehbare Änderungen pushen.

### Wichtig

Ein Push bedeutet:

> Dieser Stand ist jetzt Teil des gemeinsamen Projektverlaufs.

GitHub ist gleichzeitig Versionsverwaltung und Backup für alles, was erfolgreich gepusht wurde.

---

## 7. Branch-Regel

Für größere Änderungen möglichst eigene Feature-Branches verwenden.

Beispiele:

```text
feature/prop-system
feature/weapon-system
feature/round-system
feature/supermarket-blockout
feature/supermarket-environment
feature/lighting
```

Der `main`-Branch soll möglichst einen funktionierenden Projektstand enthalten.

Kleine, ungefährliche Dokumentationsänderungen können direkt auf `main` erfolgen.

---

## 8. Test-Regel

Jeder Entwickler testet seine Änderungen lokal, bevor sie in den gemeinsamen Projektstand gelangen.

### Core-System-Test

Der Gameplay-Entwickler prüft insbesondere:

- funktioniert das System?
- funktioniert es nach erneutem Start?
- gibt es Fehler/Warnings?
- ist Multiplayer-Replikation berücksichtigt?
- wurde nichts außerhalb des eigenen Bereichs unnötig verändert?

### World-/Content-Test

Der World-Entwickler prüft insbesondere:

- lässt sich die Map öffnen?
- kann der Spieler sie betreten?
- sind Kollisionen korrekt?
- funktionieren relevante Interaktionen?
- gibt es offensichtliche Performance-Probleme?
- wurden keine Core-Systeme versehentlich verändert?

---

## 9. KI-Arbeitsweise

### ChatGPT

ChatGPT dient als:

- Projektplanung
- Game-Design-Unterstützung
- technische Architektur
- Problemlösung
- Priorisierung
- Erklärung komplexer Systeme
- Vorbereitung konkreter Aufgaben für Claude Code
- Review von Entscheidungen

Die Projektleitung entscheidet letztendlich, welche Änderung umgesetzt wird.

### Claude Code

Claude Code dient als:

- lokale Entwicklungsunterstützung
- Codeanalyse
- Implementierung
- Änderung bestehender Dateien
- technische Untersuchung
- Ausführung klar definierter Entwicklungsaufgaben

Claude Code arbeitet immer innerhalb des lokalen Projektordners.

### Wichtige Regel

KI darf keine grundlegende Designentscheidung stillschweigend ändern.

Bei größeren Architekturänderungen zuerst erklären:

1. Was soll geändert werden?
2. Warum ist die Änderung sinnvoll?
3. Welche Dateien/Systeme sind betroffen?
4. Welche Auswirkungen hat sie auf Multiplayer und bestehende Systeme?
5. Erst danach implementieren.

---

## 10. Gemeinsame Wissensbasis

Die Chat-Historie von ChatGPT oder Claude ist **nicht** die zentrale Projektquelle.

Die zentrale Projektquelle ist dieses Repository und insbesondere der `Docs/`-Ordner.

Wichtige Dokumente:

- `Docs/GAME_DESIGN.md` – Was ist das Spiel?
- `Docs/GAMEPLAY.md` – Wie funktioniert das Spiel?
- `Docs/ROADMAP.md` – Was wird als Nächstes gebaut?
- `Docs/MULTIPLAYER.md` – Wie soll Multiplayer funktionieren?
- `Docs/TECHNICAL_ARCHITECTURE.md` – Wie wird das technisch umgesetzt?
- `Docs/DECISIONS.md` – Welche wichtigen Entscheidungen wurden getroffen?
- `CLAUDE.md` – Regeln für Claude Code
- `TEAM_GUIDE.md` – Zusammenarbeit und aktueller gemeinsamer Stand

Wenn eine wichtige Entscheidung getroffen wird, muss sie in der passenden Dokumentation festgehalten werden.

---

## 11. Onboarding des zweiten Entwicklers

Wenn ein neuer Entwickler dem Projekt beitritt:

1. GitHub-Zugriff erhalten
2. Repository klonen
3. gleiche Unreal-Engine-Version installieren
4. benötigte Entwicklungssoftware installieren
5. Projekt öffnen
6. `TEAM_GUIDE.md` lesen
7. `CLAUDE.md` lesen
8. `GAME_DESIGN.md` lesen
9. `GAMEPLAY.md` lesen
10. `ROADMAP.md` lesen
11. bei Bedarf technische Dokumente lesen
12. Projekt lokal starten
13. ersten kleinen Test durchführen
14. erst danach an einer größeren Aufgabe arbeiten

---

## 12. Übergabe zwischen den Teammitgliedern

Wenn eine Aufgabe fertig ist, sollte die Übergabe kurz dokumentiert werden.

Beispiel:

```text
Aufgabe:
Supermarkt-Blockout

Status:
Fertig

Geändert:
- Main Supermarket Level
- Regale
- Kassenbereich
- Eingangsbereich

Getestet:
- Level öffnet
- Spieler kann laufen
- Kollision funktioniert

Nächster Schritt:
Gameplay kann Interaktionspunkte vorbereiten.
```

Dadurch kann das jeweils andere Teammitglied schnell verstehen, was vorhanden ist und was als Nächstes gebraucht wird.

---

## 13. Definition of Done

Eine Aufgabe gilt nicht nur deshalb als fertig, weil die Änderung lokal funktioniert.

Eine Aufgabe ist fertig, wenn:

- die Änderung lokal getestet wurde
- keine offensichtlichen Fehler vorhanden sind
- Dokumentation bei Bedarf aktualisiert wurde
- der Git-Commit verständlich beschrieben ist
- die Änderung gepusht wurde
- das andere Teammitglied weiß, was sich geändert hat

---

## 14. Aktueller Fokus

**Nicht alles gleichzeitig bauen.**

Der aktuelle Fokus ist:

> Einen kleinen, sauberen First-Playable-Prototypen erstellen.

Zuerst muss der Kern funktionieren:

**Spieler → Supermarkt → Bewegung → Interaktion → Prop/Disguise → Multiplayer → Hunter → Runde**

Erst danach werden umfangreiche Inhalte, zusätzliche Waffen, viele Props, Voice Chat, Cosmetics und andere spätere Systeme priorisiert.

---

## 15. Entscheidungsprinzip

Bei jeder neuen Idee fragen wir:

1. Verbessert sie den Kern der Spielidee?
2. Ist sie für den aktuellen Meilenstein notwendig?
3. Erzeugt sie unnötige technische Komplexität?
4. Beeinflusst sie Multiplayer?
5. Können wir sie später hinzufügen?

Wenn etwas nicht für den aktuellen Meilenstein notwendig ist, kommt es auf die Roadmap und nicht sofort in die Implementierung.

---

## 16. Wichtigste Regel

**Das Spiel soll Schritt für Schritt spielbar werden.**

Nicht möglichst viel Code produzieren.

Nicht möglichst viele Features gleichzeitig bauen.

Sondern:

> Kleine funktionierende Systeme bauen, testen, dokumentieren und erst dann erweitern.
