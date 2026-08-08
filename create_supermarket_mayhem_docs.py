from pathlib import Path
from textwrap import dedent

# Put this script directly into your Unreal project root and run it.
# Example:
# C:\GameDevelopment\SupermarketMayhem\create_project_docs.py

ROOT = Path(__file__).resolve().parent
DOCS = ROOT / "Docs"
DOCS.mkdir(exist_ok=True)

FILES = {
"CLAUDE.md": r"""
# Supermarket Mayhem - Claude Code Instructions

## Project
Supermarket Mayhem is a first-person multiplayer Prop-Hunt/social action game set in a living supermarket.

Core fantasy:
"Hide as a product while the supermarket keeps running."

## Non-negotiable core decisions
- Platform: PC / Steam
- Engine: Unreal Engine 5.5
- Programming: C++ + Blueprints
- Camera: First Person
- Target match size: 2-8 players
- Public matchmaking: required
- Friends + strangers in the same public match: required
- Private matches: required
- Language: matchmaking preference, not a hard filter
- Roles: Hider and Hunter
- Hiders disguise themselves as supermarket products/props
- Hunters use weapons
- NPC customers keep the supermarket alive
- Physics and environmental chaos are important
- Voice chat is planned, not required for the first prototype
- Ranked mode is not part of V1

## How Claude must work
1. Read this file and relevant files in Docs/ before major implementation.
2. Inspect the existing Unreal project before changing code.
3. Never rewrite working systems without a reason.
4. Never silently change a core design decision.
5. For a major architectural change, explain the impact first.
6. Keep multiplayer readiness in mind from the beginning.
7. Prefer server-authoritative gameplay-critical state.
8. Use C++ for core/networking systems where appropriate and Blueprints for suitable iteration/presentation.
9. Do not add paid services or unnecessary plugins without approval.
10. Do not claim something works unless it was tested or the limitation is clearly stated.

## Source of truth
If documentation conflicts:
1. Latest explicit user decision
2. Docs/DECISIONS.md
3. GAME_DESIGN.md / GAMEPLAY.md
4. MULTIPLAYER.md / TECHNICAL_ARCHITECTURE.md
5. Other docs
6. Existing implementation

## Development order
1. First-person player
2. Supermarket blockout
3. Prop/disguise system
4. Multiplayer replication
5. Hunter weapon
6. Round system
7. NPC supermarket activity
8. Steam party/lobby/matchmaking
9. Content/polish
10. Playtesting/release

## Change protocol
If the user changes a core decision:
- update DECISIONS.md
- update affected documentation
- update ROADMAP.md if necessary
- explain affected systems
- then implement the approved change

## Current state
Milestone 0 is working:
- UE5.5 project exists
- First Person template works
- WASD works
- mouse look works
- Git is installed

Next target:
Create a small supermarket blockout and establish the first clean playable milestone.
""",

"Docs/GAME_DESIGN.md": r"""
# Supermarket Mayhem - Game Design

## High concept
A first-person multiplayer game where players hide as ordinary supermarket products while other players hunt them with weapons.

Core fantasy:
> Hide as a product while the supermarket keeps running.

## Match fantasy
Players enter a living supermarket, receive roles, Hiders prepare their hiding positions, Hunters are released, and the hunt begins while NPC customers and the supermarket continue operating.

## Roles

### Hider
- becomes a supermarket product/prop
- hides in plausible locations
- observes Hunter movement
- moves when necessary
- survives until the round ends or future objectives are completed

### Hunter
- searches for suspicious objects
- observes the environment
- uses sound and movement clues
- uses weapons
- eliminates Hiders

## Perspective
First person is the core perspective. The game is not designed as a top-down or bird's-eye game.

## Players
Initial target: 2-8 players.

## Supermarket
Initial environment should include:
- entrance
- aisles
- shelves
- checkout area
- refrigerators/freezers
- product displays
- carts and baskets
- promotional displays
- backroom/storage
- NPC customers

## Props
Possible examples:
- cans
- bottles
- cartons
- cereal boxes
- cleaning products
- household goods
- food packages

The final product library is content work and must remain data-driven.

## Weapons
Weapons are core Hunter gameplay. The first prototype only needs one simple weapon to prove:
- aiming
- firing
- hit detection
- damage/elimination
- multiplayer authority

## NPCs
Customers create movement, noise and camouflage. They should make the supermarket feel alive rather than like an empty arena.

## Physics
Relevant objects can be knocked over, moved or disturbed. Physics must remain multiplayer- and performance-safe.

## Tone
- chaotic
- funny
- tense
- accessible
- readable
- believable enough to make hiding among products work

## V1 priorities
1. fun core loop
2. readable hiding/hunting
3. reliable multiplayer
4. satisfying weapon
5. living supermarket
6. performance
7. content variety
8. progression/cosmetics
""",

"Docs/GAMEPLAY.md": r"""
# Supermarket Mayhem - Gameplay

## Core loop
Quick Play -> matchmaking -> lobby -> role assignment -> Hider preparation -> hunt -> result -> play again

## Public matchmaking
Quick Play finds strangers. Matchmaking should prefer:
1. acceptable ping/region
2. preferred language
3. filling the match

Language must be relaxed before queue times become unreasonable.

## Friends
Players can form a party and enter matchmaking together. A party must not be split.

Example:
3 friends + 5 other players = 8-player match.

## Private match
Friends can create a private session. Fewer than 8 players may be allowed in the prototype.

## Round
1. Match found
2. Load map
3. Assign roles
4. Hiders get preparation time
5. Hunters are released
6. Hunt runs on a timer
7. Eliminations/interactions occur
8. Win condition resolves
9. Results screen
10. Rematch/queue

## Hider play
Hiders choose plausible props, hide, observe, reposition carefully and exploit visual/environmental clutter.

## Hunter play
Hunters inspect the environment, look for suspicious behavior, listen and use weapons.

## Future ideas
Possible later systems:
- prop abilities
- decoys
- special events
- objectives
- overtime
- environmental events
- more weapons

Do not build these before the basic loop is fun.
""",

"Docs/MULTIPLAYER.md": r"""
# Supermarket Mayhem - Multiplayer

## Requirements
- public matchmaking
- strangers
- friend parties
- private matches
- 2-8 players initially
- Steam-first
- language preference
- region/ping awareness

## Server authority
Gameplay-critical state should be server authoritative:
- roles
- round state
- timers
- disguise state
- weapon state
- damage
- eliminations
- win conditions
- important interactions

Clients request actions; they should not authoritatively declare outcomes.

## Prototype
A host/listen-server approach is acceptable for early development to reduce complexity and cost.

## Production direction
Dedicated servers are the preferred final direction if the game reaches production scale.

## Steam
Steam is the first platform and should cover identity, friends, invites, lobbies and the selected networking/matchmaking approach.

Do not lock in a specific plugin/API implementation until its UE5.5 compatibility is verified.

## Party
A party remains together when entering public matchmaking.

## Voice
Planned later:
- party/lobby voice
- team voice
- proximity voice

Not required for the first playable prototype.

## Scalability
If the player count later increases, review:
- replication
- NPC count
- physics
- bandwidth
- server tick
- visibility
- matchmaking capacity
""",

"Docs/TECHNICAL_ARCHITECTURE.md": r"""
# Supermarket Mayhem - Technical Architecture

## Engine
Unreal Engine 5.5

## Code
C++ + Blueprints.

Use C++ for core systems, networking and reusable foundations where appropriate. Use Blueprints for suitable gameplay composition, configuration and rapid iteration.

## Main systems
- Player
- Input
- Interaction
- Prop/Disguise
- Weapon
- Damage
- Round/GameState
- Lobby/Party
- Matchmaking
- NPC
- Supermarket interaction
- Physics
- UI
- Audio

## Networking
Design core gameplay for replication and server authority from the beginning.

Avoid client-trusting shortcuts that would require a rewrite later.

## Content
Products and weapons should be data-driven where useful so new content does not require rewriting core systems.

## Performance
Development PC:
- Ryzen 5 5500
- RTX 2060
- 16 GB RAM
- Windows 11 Pro

32 GB RAM is a useful future upgrade, but not a prerequisite.

Profile before optimizing blindly.

## Source control
Git from the beginning. Do not commit Unreal-generated directories such as:
- Binaries
- Intermediate
- Saved
- DerivedDataCache

## Plugins
Only add plugins with a documented purpose, UE5.5 compatibility and acceptable licensing/cost.

## Testing
Each milestone needs a reproducible test. Multiplayer tests should progress from local clients to real Steam sessions.
""",

"Docs/ART_DIRECTION.md": r"""
# Supermarket Mayhem - Art Direction

## Goal
The supermarket must be believable enough for players to disappear among products while remaining readable and visually appealing.

## Visual style
- recognizable supermarket
- colorful products
- clear silhouettes
- grounded environment with playful presentation
- readable player characters
- enough visual clutter for hiding

Avoid an empty warehouse look and avoid excessive visual noise.

## Environment
Build a small high-quality supermarket first:
- entrance
- aisles
- checkout
- refrigeration
- produce
- drinks
- household
- promotions
- storage/backroom

## Asset strategy
Start with blockout and placeholders. Later use licensed marketplace assets, Blender edits and original assets where worthwhile.

Never assume an external asset can be redistributed without checking its license.
""",

"Docs/AUDIO.md": r"""
# Supermarket Mayhem - Audio

## Goals
Audio should create:
- supermarket atmosphere
- tension
- humor
- spatial awareness
- weapon feedback
- prop interaction feedback

## Ambience
Potential sounds:
- refrigeration
- carts
- footsteps
- announcements
- checkout beeps
- NPC chatter
- doors
- product/shelf interaction

## Gameplay
Important sounds:
- weapons
- hits
- movement
- prop interactions
- round state
- suspicious interactions

## Voice
Voice chat is planned later. Possible modes are party, team and proximity voice.
""",

"Docs/UI_UX.md": r"""
# Supermarket Mayhem - UI/UX

## Main menu
- Quick Play
- Play With Friends
- Private Match
- Settings
- Quit

## Quick Play
Choose language preference and start matchmaking.

## Party
Show party members and allow invitations before matchmaking.

## Match found
Show player count and loading/ready state.

## In-round HUD
Keep it minimal.
Possible information:
- remaining time
- role-specific information
- weapon/ammunition information for Hunters

## Results
Show:
- winning side
- individual result
- basic statistics
- rematch/queue

The supermarket should remain the main visual interface during the round.
""",

"Docs/MONETIZATION.md": r"""
# Supermarket Mayhem - Monetization

## Current status
No monetization in the prototype.

The core game must be fun before monetization is designed.

## Possible future
Cosmetic-only options may include:
- character cosmetics
- prop skins
- weapon cosmetics
- themed bundles

Avoid pay-to-win gameplay advantages.
""",

"Docs/ROADMAP.md": r"""
# Supermarket Mayhem - Roadmap

## Phase 0 - Foundation
Status: IN PROGRESS
- UE5.5 installed
- First Person C++ template working
- WASD verified
- mouse look verified
- Git installed
- documentation package

## Phase 1 - First playable
Goal: walk through a basic supermarket.
- first-person player
- supermarket blockout
- basic interaction framework
- placeholder products

Done when: the player can enter and move through a small supermarket.

## Phase 2 - Prop system
Goal: become a product.
- prop selection
- transformation
- collision
- replicated-ready state

## Phase 3 - Multiplayer prototype
Goal: two or more clients see replicated players/props.

## Phase 4 - Hunter weapon
Goal: one weapon can aim, fire, hit and eliminate a Hider with server authority.

## Phase 5 - Round system
Goal: complete Hider vs Hunter round with roles, timers, win condition and results.

## Phase 6 - Living supermarket
NPC customers, ambience and environmental interactions.

## Phase 7 - Steam multiplayer
Friends, party, lobby, Quick Play, private match and language preference.

## Phase 8 - Content/polish
More props, weapons, animation, VFX, audio, UI and optimization.

## Phase 9 - Playtesting
External testers, balance, bug fixing and onboarding.

## Phase 10 - Release preparation
Steam page, legal/licensing checks, builds, performance and release candidate.

Do not skip the core loop validation.
""",

"Docs/DECISIONS.md": r"""
# Supermarket Mayhem - Decision Log

## D001 - First person
Core gameplay uses first person.

## D002 - Public multiplayer
Public matchmaking with strangers is a core requirement.

## D003 - Match size
Initial target is 2-8 players.

## D004 - Friends
Friend parties can enter public matchmaking together.

## D005 - Language
Language is a preference, not a hard matchmaking filter.

## D006 - Weapons
Weapons are a core Hunter feature.

## D007 - NPC supermarket
NPC customers are part of the intended core experience.

## D008 - Steam
Steam is the first target platform.

## D009 - Server authority
Gameplay-critical state should be server authoritative.

## D010 - Prototype hosting
Host/listen-server is acceptable for early prototypes; production should remain compatible with dedicated servers.

## D011 - Ranked
No ranked ladder for V1.

## D012 - Claude Code
Claude Code is an implementation assistant. The user remains the product/design decision maker.

## Change protocol
When a core decision changes:
1. add a new decision entry
2. mark the old decision as superseded if needed
3. update affected docs
4. update CLAUDE.md if relevant
5. update ROADMAP.md if relevant
"""
}

for relative_path, content in FILES.items():
    path = ROOT / relative_path
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(dedent(content).strip() + "\n", encoding="utf-8")

print("Supermarket Mayhem documentation package created.")
print(f"Project root: {ROOT}")
for relative_path in FILES:
    print(f"  created: {relative_path}")
