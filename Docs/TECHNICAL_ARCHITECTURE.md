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

### Hunter weapon presentation
`ASupermarketMayhemWeapon` remains the authority for equipment, fire requests,
ammo and reload timing. Fire requests are validated on the server against the
owner's role, elimination state, Hunt phase, reload state, ammo and the
Character's equipped-weapon reference before ammo is spent and the existing
server trace/elimination path runs. Fire during reload/empty ammo, reload
outside Hunt and requests from an unequipped/stale weapon are rejected.
Unequip and Result cancel the server reload timer; Preparation resets ammo
and reload state.

The weapon actor and its gameplay fields replicate. Its first-person and
third-person mesh components are cosmetic local subobjects and are not
replicated: each instance attaches them to the owner's camera and character
hand respectively, with owner-only visibility rules. Equip and reload
presentation follows the existing replicated flags. Accepted fire uses an
unreliable server multicast for transient montage, sound and muzzle-light
feedback; small recoil animates only the owner's weapon mesh and does not
modify camera aim or server hit traces. Dry-fire feedback is an owner-only
cosmetic response to a server-rejected empty shot. Presentation asset
compatibility and multiplayer behavior still require PIE verification.

### Living supermarket customer foundation
`ASupermarketMayhemGameMode` creates the non-replicated
`ASupermarketMayhemCustomerSpawnManager` on the server at the first
Preparation transition. Its editable settings default to four customers
(minimum 0, maximum 8), a generic customer class, optional customer Data
Asset, and actor tags for level-authored spawn/destination points. Tagged
spawn actors are preferred; level PlayerStarts are the fallback.

Each `ASupermarketMayhemCustomer` is a replicated Character using standard
CharacterMovement replication. Customer type and the small Idle/Walk/Shop/
Paused state are replicated. Its `ASupermarketMayhemCustomerAIController`
exists and runs behavior only on the server. Timer callbacks transition
between states; destinations use tagged actors or random reachable NavMesh
points. No AI tick or per-movement RPC is added. The optional
`USupermarketMayhemCustomerData` asset configures speed, idle/shop duration
ranges, preferred destination tag and reaction sensitivity; base defaults
support a single `DefaultCustomer` without requiring an authored Data Asset.
Independent randomized timers produce different customer start/visit phases.
Tagged destinations are projected to NavMesh and accepted only when a full
path exists; the controller falls back to a random reachable NavMesh point.

Round hooks enable/reset customers during Preparation, keep them moving
during Hunt and stop movement/behavior during Result. After an accepted Hunter
fire request passes server checks and the server camera trace runs, the weapon
reports a noise event through GameMode to the existing customer manager. The
manager forwards it to its server-owned NPC list. Each NPC applies distance
falloff, configurable reaction sensitivity and a short cooldown; the server
AIController may interrupt walking for strong events and select another
destination. The controller clears behavior timers on pause/reset, and no
additional AI Tick or client RPC is used. The existing Manny Unarmed
animation blueprint supplies locomotion; Shop uses stationary idle. When the
placed Recast actor has no active tiles, the authoritative GameMode builds the
small blockout NavMesh once at Preparation. A headless server/two-client run
confirmed six active tiles, accepted navigation requests for all four
customers, several completed shelf arrivals, role assignment, Preparation and
Hunt. Client replication/presentation, Hunter fire/noise reaction and PIE
remain unverified.

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

### Interactive supermarket foundation
`ASupermarketMayhemInteractiveActor` is a replicated server-authoritative base
for player- and customer-facing objects. It owns a static mesh and a separate
query-only visibility volume, prompt/interaction metadata, an authority-only
interaction count/event, and round reset behavior. It has no per-frame tick.
Its optional physics hook enables server physics and movement replication for
future movable objects without adding chaos rules.

`ASupermarketMayhemShelf` carries a replicated product actor array and is tagged
`CustomerDestination` and `ShoppingTarget`. `ASupermarketMayhemProduct` adds
replicated product ID/type/weight fields. The map uses simple cube meshes.
`USupermarketMayhemInteractionComponent` supplies local target and prompt
queries, then requests a server action; the server repeats the trace and
validates target identity, range, role, elimination and round. Existing disguise
interaction remains the fallback. NPC target completion calls the same actor
entry point from authority-side AI. Optional player interaction noise routes
through the existing GameMode/customer-manager hook and is disabled by default.

No inventory, pickup/throw, checkout, product economy, HUD widget or chaos
system is implemented. The two-client headless run initially found zero active
NavMesh tiles because the map's placed Recast actor had no baked data and the
runtime did not automatically request a full build for that existing actor.
The GameMode now checks active tiles at Preparation and performs a server
`Build()` only when none exist. The validated run generated six active tiles,
all four NPC navigation requests were accepted, three shelf arrivals were
logged, and Hunt began. PIE and client-side replication remain open.
