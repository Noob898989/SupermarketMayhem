# Supermarket Mayhem - Technical Architecture

## Engine
Unreal Engine 5.8

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
Only add plugins with a documented purpose, UE5.8 compatibility and acceptable licensing/cost.

## Testing
Each milestone needs a reproducible test. Multiplayer tests should progress from local clients to real Steam sessions.
