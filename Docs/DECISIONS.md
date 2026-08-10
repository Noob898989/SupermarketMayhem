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

## D014 - AP4.1 scope: local, non-replicated prop/disguise

The initial prop/disguise implementation (AP4.1: SupermarketMayhemProp,
SupermarketMayhemDisguiseComponent, and the related
SupermarketMayhemCharacter changes) is scoped to local, non-replicated
behavior only, consistent with D010 (host/listen-server acceptable for
early prototypes). No networking, replication, or server-authority
validation is implemented for disguise/prop state in this pass. Full
server-authoritative replication of this state is deferred to the
Multiplayer prototype phase (Docs/ROADMAP.md, Phase 3), per
Docs/MULTIPLAYER.md ("Server authority": disguise state is expected to
become server-authoritative and replicated).

## Change protocol
When a core decision changes:
1. add a new decision entry
2. mark the old decision as superseded if needed
3. update affected docs
4. update CLAUDE.md if relevant
5. update ROADMAP.md if relevant

## Open (not yet decided)

The following are open questions identified during a project status
analysis. They are explicitly NOT decisions - they require Game Director /
user input before being implemented or resolved one way or the other. Do
not treat entries in this section as binding; once the user decides one,
move it into the numbered decision log above as a new D0xx entry.

### O001 - GameMode Blueprint naming
The template Blueprint `BP_FirstPersonGameMode`
(Content/FirstPerson/Blueprints/) appears to have been reparented in the
editor to the C++ class ASupermarketMayhemGameMode - it is the GameMode
actually used in the verified Phase 5 PIE test (see
Docs/ROADMAP.md, Phase 5) - but it was not renamed. No
`BP_SupermarketMayhemGameMode` asset exists. Open question: keep the
existing template name, or rename/recreate it to reflect its current role?

### O002 - Variant_Horror / Variant_Shooter template content
`Source/SupermarketMayhem/Variant_Horror/` and `Variant_Shooter/` (and the
matching Content/ folders) are unmodified UE5 template demo content, not
part of the active game flow, but still included in the module (see
`SupermarketMayhem.Build.cs`, PublicIncludePaths). Open question: keep as
technical reference (e.g. for the Phase 4 Hunter weapon work, as already
noted for the weapons/ai agents in .claude/agents/), or remove once no
longer needed?

### O003 - Round system scaling to 2-8 players
D003 requires a 2-8 player match size, but the current Phase 5 round system
implementation (ASupermarketMayhemGameMode) is hardcoded to exactly 2
players (HiderController/HunterController as single fields, not a list).
Open question: when should this be scaled up - before or after Phase 3
(Multiplayer prototype)?
