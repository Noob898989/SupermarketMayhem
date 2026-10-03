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

**Implementation status (2026-08-25, uncommitted, PIE-verified with 2 clients):**
- Steps 3-10 are implemented and verified end-to-end for exactly 2 players
  (see Docs/ROADMAP.md, Phase 5). Role assignment, the Preparation timer,
  the Hunt timer, server-authoritative Hunter elimination (or "Hider
  escaped!" on Hunt timeout), and an automatic restart back to step 3 are
  all confirmed working in PIE.
- Step 10 ("Rematch/queue") is currently an automatic restart with the
  same 2 players and unchanged roles (Hider stays Hider, Hunter stays
  Hunter) - not a queue/rematch UI or a new role assignment.
- Steps 1-2 ("Match found", "Load map") are not implemented yet - players
  currently join directly into Lvl_SupermarketBlockout via PIE; there is
  no matchmaking (Phase 7, Docs/ROADMAP.md, still "Not started").
- The GameMode now uses the connected PlayerState collection and an ordered,
  configurable role-slot list. Its default `[Hider, Hunter]` preserves the
  two-player loop. For a larger configured roster, all Hiders are reset
  together and the Hunt ends on elimination only when no Hider remains.
  The role distribution for 3-8 players remains a game-design question.

## Hider play
Hiders choose plausible props, hide, observe, reposition carefully and exploit visual/environmental clutter.

As of 2026-08-25, 7 Props with unique PropIds are placed in
Lvl_SupermarketBlockout.umap (checkout area, all 4 aisles, and the
backoffice side room), so the Hider genuinely chooses between multiple
distinct hiding spots rather than a single fixed one.

## Hunter play
Hunters inspect the environment, look for suspicious behavior, listen and use weapons.

**Weapon foundation status:** the existing Hunter fire input now requests a
shot from an equipped weapon actor. The weapon validates the Hunter role,
equipment and Hunt phase and resolves hits on the server before the existing
elimination flow runs. The weapon actor and equipped reference replicate.
The default magazine holds six rounds, and reload takes 1.5 seconds; both
values are weapon-configurable. Press R to request a reload. Firing spends a
round on a validated shot, including a miss; firing with no ammo or during
reload is rejected. Ammo and reload state are server-owned and replicated.
The weapon uses the existing pistol static mesh for the Hunter's owner-only
camera view and a third-person hand-mounted view. Both are cosmetic components
driven by the replicated equipped state; their visual attachments do not
control server gameplay. Existing pistol animations provide cosmetic fire,
reload, equip and dry-fire feedback when compatible with the character's
animation instances. Fire uses its montage; reload, equip and dry-fire
sequences play through local dynamic montages. Accepted fire sends a
server-originated cosmetic event for audio and a short muzzle point-light
pulse. Owner recoil affects only the
weapon mesh. An empty shot is rejected by the server and can return a
throttled owner-only cue. Ammo, reload, elimination, animation and presentation
still need runtime PIE verification; see Docs/ROADMAP.md, Phase 4.

## NPC customers
The first customer foundation spawns four server-owned NPCs when the first
round enters Preparation (GameMode defaults are configurable: 0-8, initial 4).
Customers use a replicated mannequin Character and move under a server-only
AIController. The timer-driven flow cycles through Idle, Walk and Shop, with
independent randomized idle and shopping durations. The controller prefers
reachable level actors tagged `CustomerDestination` and can filter those by
the profile's `PreferredDestinationTag`; when no suitable tagged destination
is available, it selects a random reachable NavMesh location. Result pauses
behavior, and Preparation resets the state and timers.

Customer behavior can be configured with an optional Data Asset profile for
movement speed, idle/shopping duration, preferred destination tag and reaction
sensitivity. Without an asset, the base `DefaultCustomer` profile is
used. Place level actors tagged `CustomerSpawn` and `CustomerDestination` to
author spawn and shopping locations; add a second actor tag matching
`PreferredDestinationTag` to specialize a profile. Tagged destinations are
checked for a complete NavMesh path. PlayerStarts are the spawn fallback.
The blockout has a NavMeshBoundsVolume and dynamic Recast generation. The
GameMode now performs a one-time authoritative `Build()` during Preparation
only when the placed Recast data has no active tiles. In the latest headless
two-client run this built six tiles, all four customers accepted navigation
requests, and three reached tagged shelf targets. Client-side presentation
and replicated movement still need PIE verification.

Server-confirmed Hunter shots also report noise (including a valid miss) to
the server customer manager. NPCs combine intensity, distance falloff and
their profile sensitivity; stronger nearby sounds can interrupt a walk and
send them toward a different reachable point, after which their normal
shopping cycle continues. Short per-customer cooldowns coalesce rapid reports.
The Manny Unarmed animation blueprint supplies movement and idle presentation;
Shop currently uses its stationary idle pose, with no dedicated shopping
animation. A fresh headless server/two-client smoke test reached Hunt with four
server-spawned NPCs; all four accepted navigation requests and three reached
shopping targets. Actual noise reactions, visible locomotion and client-side
NPC replication still need PIE verification.

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

## Interactive supermarket objects
The blockout contains four interactive shelf targets with two placeholder
product actors each. A shelf is both a tagged customer destination and player
interaction target, with references to multiple products. Product actors carry
minimal ID/type/name/mesh/optional-weight data; there is no product economy.

The existing Interact input first requests interaction with the actor under the
player camera. The character component exposes the focused target and prompt
text for a future HUD. If no interactive actor is hit, existing disguise
interaction continues. Living Hiders and Hunters may use objects in Preparation
and Hunt; Result/Waiting and eliminated players are rejected. Customers use the
same server interaction entry point on arrival and remain in the timed Shop
state; no product is consumed.

The server retraces aim and validates target, distance, role, elimination,
round and interaction-enabled state. Interaction count replicates and resets
at Preparation. Optional player interaction noise uses the existing server
customer-noise hook and is disabled by default. The actor also exposes a
server-only physics/movement-replication hook; no chaos behavior is enabled.
There is no HUD prompt widget. The latest headless server/two-client run
generated six active NavMesh tiles and three NPCs reached shelf targets through
the shared server interaction entry point. Player input interaction, visual
movement and client-side replication still require PIE verification.
