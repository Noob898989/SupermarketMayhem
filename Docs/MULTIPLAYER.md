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
