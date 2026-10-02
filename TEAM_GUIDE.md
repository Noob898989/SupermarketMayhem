# Supermarket Mayhem - Team Guide

## Working model

### ChatGPT - Project Lead
Responsible for project status, planning, architecture, task definition, product-decision identification, result review and overall consistency.

### Codex - Autonomous Development Agent
Responsible for repository inspection, implementation, builds, tests, debugging, documentation updates, diff review, commits and pushes.

Codex should operate autonomously for ordinary technical decisions and escalate only genuine product or architectural decisions.

### Unreal Engine - Development/Test Environment
Authoritative environment for editor validation, Blueprint/asset/map validation, PIE, multiplayer PIE and visual/manual gameplay verification.

### GitHub - Source of Truth
Authoritative versioned project state and documentation.

## Standard workflow

ChatGPT
-> defines a clear task
-> Codex inspects project and docs
-> Codex implements
-> Codex builds/tests
-> Codex fixes failures
-> Codex reviews diff
-> Unreal/manual verification when required
-> Codex commits and pushes
-> ChatGPT reviews result
-> next task

## Decision boundary

Codex may make ordinary technical implementation decisions when they fit the existing architecture.

Product decisions and undocumented core architectural changes remain with the user.

## Claude Code

Claude Code is not part of the active development workflow.
