# Supermarket Mayhem - Agent Instructions

## Purpose

This file defines the shared instructions for autonomous AI development agents working on Supermarket Mayhem.

Workflow:
- ChatGPT = planning, architecture, analysis, task definition and project direction
- Codex = autonomous implementation, testing and technical execution
- Unreal Engine = development and manual test environment
- GitHub = source of truth and version control

Claude Code is not part of the project workflow.

## Source of truth

When information conflicts, use this order:
1. Latest explicit user/product decision
2. Docs/DECISIONS.md
3. Docs/GAME_DESIGN.md
4. Docs/GAMEPLAY.md
5. Docs/MULTIPLAYER.md and Docs/TECHNICAL_ARCHITECTURE.md
6. Docs/ROADMAP.md for current implementation status
7. Existing implementation

AGENTS.md defines agent working rules but does not override product decisions.

## Core project constraints

- Unreal Engine 5.8
- C++ + Blueprints
- First-person
- PC / Steam
- Target match size 2-8 players
- Public matchmaking, friend parties and private matches are required
- Roles: Hider and Hunter
- Hiders disguise as supermarket props
- Hunters use weapons
- NPC customers, physics and environmental chaos are part of the intended experience
- Voice chat is planned later
- Ranked mode is not part of V1

## Autonomous workflow

For a clearly defined task, work autonomously from start to finish:

1. Inspect git status, branch and relevant recent history.
2. Read this file and all relevant project documentation.
3. Inspect the existing implementation.
4. Determine the smallest coherent implementation needed.
5. Implement all necessary related changes without asking for confirmation between ordinary technical steps.
6. Build and test the affected systems.
7. If a build/test fails, analyze the root cause, implement a reasonable fix and rebuild/retest. Repeat as appropriate.
8. Review the final diff for unintended changes.
9. Update relevant documentation when implementation state or architecture changes.
10. Run final validation and inspect git status.
11. Commit completed work with a clear message when validation passes.
12. Push completed work to the appropriate GitHub branch.
13. Report the result only when complete or when a genuine product/architecture decision is required.

Do not stop merely because a technical detail is uncertain. Resolve ordinary implementation details from the existing architecture, documentation and established Unreal conventions.

## Escalation rules

Escalate only when continuing requires a genuine product or architectural decision that cannot be derived from the source of truth.

Examples:
- changing a core gameplay rule;
- changing player count or role model;
- changing networking authority;
- adding a paid service/plugin;
- an irreversible architectural tradeoff not covered by documentation;
- conflicting requirements that cannot be reconciled safely.

Do not ask for confirmation for ordinary file operations, refactoring within the existing design, compiler fixes, test fixes, documentation corrections, build commands, git inspection, commits or pushes after successful validation.

## Unreal Engine

Treat these as high-impact areas:
- .uproject
- C++ Source
- Build configuration
- Plugins
- Config
- Blueprints
- Maps
- Assets
- Data Assets
- GameMode
- GameState
- Player systems
- Replication
- Multiplayer
- Server authority

Inspect existing usage before modifying them. Do not replace working systems without a concrete reason.

Gameplay-critical state remains server authoritative. Maintain compatibility with the planned Steam multiplayer architecture and 2-8 player target.

Use C++ for core/reusable/networking systems where appropriate and Blueprints for suitable composition, configuration and presentation.

For .uasset/.umap changes, never claim success unless the change was actually performed and verified through an appropriate Unreal/editor workflow.

## Multiplayer

Gameplay-critical state remains server authoritative:
- roles
- round state
- timers
- disguise state
- weapon state
- damage
- eliminations
- win conditions
- important interactions

Clients request actions; the server validates and determines authoritative outcomes.

The prototype may use a host/listen-server approach, but implementation must remain compatible with the planned production direction.

## Git and GitHub

GitHub is the source of truth.

Before work:
- inspect git status;
- inspect the current branch;
- inspect relevant recent history.

During work:
- keep changes scoped;
- respect .gitignore;
- do not commit generated Unreal directories;
- do not commit secrets or credentials;
- use branches for larger features when appropriate.

Before commit:
- inspect the diff;
- verify only intended files changed;
- build/test the affected functionality;
- update documentation when required.

After successful completion:
- create a meaningful commit;
- push to the appropriate branch;
- verify the repository state.

Never force-push or rewrite shared history unless explicitly instructed.

## Testing and reporting

Never claim a test passed unless it actually ran.

Distinguish between:
- source inspection;
- successful compilation/build;
- automated test;
- PIE verification;
- manual Unreal Editor verification;
- multiplayer verification;
- unverified assumptions.

If Unreal Editor interaction is required and cannot be automated, perform all available automated validation first and clearly identify the remaining manual check.

## Documentation

Keep documentation synchronized with implementation.

When a product or architecture decision changes:
- update Docs/DECISIONS.md;
- update affected documentation;
- update Docs/ROADMAP.md when status changes.

Do not create parallel status systems without a clear need.

## Scope discipline

Do not add features merely because they appear useful. Do not silently change gameplay, monetization, platform strategy, networking architecture or project scope.

Prefer small, coherent, testable changes over broad rewrites.
