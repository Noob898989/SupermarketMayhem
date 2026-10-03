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
This foundation has no weapon presentation and still needs runtime PIE
verification; see Docs/ROADMAP.md, Phase 4.

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
