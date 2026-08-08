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

## D013 - Engine version
Unreal Engine 5.8 is the current and binding engine version for this project.

Verified: the project's EngineAssociation GUID
({84FF32D3-4EFC-8FE0-85C6-1982B4A08650}) resolves to the UE 5.8 install on
this machine (D:\Program Files\Epic Games\UE_5.8). SupermarketMayhemEditor has
been built successfully against this engine.

Earlier documentation referenced UE 5.5; this was outdated and has been
corrected throughout Docs/ and CLAUDE.md. No gameplay, assets, map content, or
the .uproject EngineAssociation were changed as part of this correction.

## Change protocol
When a core decision changes:
1. add a new decision entry
2. mark the old decision as superseded if needed
3. update affected docs
4. update CLAUDE.md if relevant
5. update ROADMAP.md if relevant
