 MetricHUD

> ⚠️ **Early Development Build**
>
> MetricHUD is under active development and is not yet feature complete.

A lightweight, customizable metrics HUD for **Guild Wars 2**.

The goal is simple:

> **Show the information the player wants—and nothing they don't.**

---

## Privacy & Security

MetricHUD is designed to operate without requiring access to your Guild Wars 2 account credentials.

- MetricHUD does not request or store Guild Wars 2 API keys.
- MetricHUD does not require account login information.
- Combat analysis is based on locally available game/addon data.
- Training profiles are selected or imported by the player.
- No account credentials are required to use MetricHUD.

If a future feature would require authenticated account access, it will be evaluated separately rather than being included as a requirement for the core addon.

---

# Progress

Current Version: **0.3.0**

## Completed

- [x] Modular project architecture
- [x] ConfigManager
- [x] MetricRegistry
- [x] Generic metric rendering
- [x] Metric formatting system
- [x] Metric ordering
- [x] HUDWindow framework
- [x] Live HUD visibility toggle
- [x] Draggable HUD
- [x] Lock HUD position
- [x] Persistent JSON configuration
- [x] Organized options panel
- [x] Reset Settings
- [x] Individual metric visibility toggles
- [x] Text metric support
- [x] Mumble Link integration
- [x] arcdps combat-event integration
- [x] Combat event collection
- [x] Combat-session tracking
- [x] Combat DPS calculation
- [x] Combat-session reset
- [x] Recent skill ID tracking
- [x] Combat Analyzer debug panel

## Current Metrics

- [x] FPS
- [x] Ping
- [x] Combat Timer
- [x] DPS
- [x] Map Name
- [x] Map ID
- [x] Character Name
- [x] Player Speed

      Known issue:
Mumble-derived metrics may occasionally fall back to Unknown / 0
(Map Name, Map ID, Character Name, Player Speed).

---

# Design Philosophy

MetricHUD is built around a few simple principles.

- **Precision over repetition** – Use direct input whenever practical.
- **Immediate feedback** – Changes should be reflected instantly.
- **Clarity** – A clean, self-explanatory interface.
- **Efficiency** – Minimize unnecessary clicks.
- **Stability before features** – Reliability always comes first.

---

# Planned Metrics

- Player Position
- Time in Combat
- Downed Count
- Death Count
- Boon Uptime
- Food Timer
- Utility Timer
- Jade Bot Buffs *(Experimental)*
- Squad Statistics
- Encounter Summary
- World Boss Statistics *(Experimental)*

## Rotation & Combat Training

- Skill Rotation Log
- Live Skill History
- Skill Usage Summary
- Rotation Timeline
- User-Defined Practice Rotations
- Expected vs. Actual Rotation Comparison
- Skill Timing Analysis
- Rotation Gap / Idle Time Detection
- Missed or Delayed Skill Detection
- Auto-Attack Filtering
- Rotation Practice Score
- Post-Combat Rotation Summary
- Training Feedback

> **Rotation Training Goal**
>
> MetricHUD is intended to help players move beyond random skill usage and understand structured Guild Wars 2 combat. Rotation tools will focus on showing what the player actually did, comparing it against a user-selected practice rotation, and providing clear feedback about skill order, timing, and downtime.
>
> MetricHUD will not define a single "correct" rotation. Practice rotations will be configurable so players can train different builds, strategies, and skill priorities.

> **Experimental**
>
> Experimental features rely on game events or APIs that are still being validated. Accuracy is not guaranteed until fully tested.

---

# Planned Features

### HUD

- Custom metric selection
- Metric reordering
- Saveable layouts
- Multiple HUD profiles

### Appearance

- Custom colors
- Font scaling
- Transparency controls

### Architecture

- Plugin-style metric system
- Metric categories
- Expanded metric data sources
- Skill event history
- Rotation analysis framework

---

# Roadmap

## Phase 1 — Foundation ✅

- [x] Core architecture
- [x] Configuration system
- [x] JSON persistence
- [x] Metric registry
- [x] Generic metric rendering
- [x] Metric formatting
- [x] Metric ordering

## Phase 2 — HUD

- [x] Draggable HUD
- [x] Lock HUD position
- [x] Metric visibility controls
- [ ] Window position persistence
- [ ] Layout customization
- [ ] Appearance options

## Phase 3 — Core Metrics

- [x] FPS
- [x] Ping
- [x] Combat Timer
- [x] DPS
- [x] Map Name
- [x] Map ID
- [x] Character Name
- [x] Player Speed
- [ ] Player Position
- [ ] Boon Uptime
- [ ] Squad Statistics

## Phase 4 — Combat Event System

- [x] arcdps combat-event integration
- [x] Player combat-event detection
- [x] Skill ID tracking
- [x] Recent skill history
- [x] Combat-session tracking
- [x] Combat DPS calculation
- [x] Combat-session reset
- [x] Last-fight statistics capture
- [ ] Skill-name resolution
- [ ] Full combat skill history
- [ ] Skill usage statistics
- [ ] Auto-attack filtering

## Phase 5 — Rotation Training

- [ ] Live Skill Rotation Log
- [ ] Rotation Timeline
- [ ] User-Defined Practice Rotations
- [ ] Expected vs. Actual Rotation
- [ ] Skill Timing Analysis
- [ ] Rotation Gap / Idle Time Detection
- [ ] Missed or Delayed Skill Detection
- [ ] Rotation Practice Score
- [ ] Post-Combat Rotation Summary
- [ ] Training Feedback

## Phase 6 — Release

- [ ] Optimization
- [ ] Documentation
- [ ] Version 1.0

---

# Long-Term Vision

MetricHUD is intended to be more than a collection of numbers.

Guild Wars 2 allows players to progress through much of the early game without requiring structured skill usage. As combat becomes more demanding, understanding skill priority, timing, cooldown management, weapon usage, and combat uptime becomes increasingly important.

MetricHUD aims to bridge that gap.

Rather than simply showing the player what happened during combat, MetricHUD's training tools are intended to help players understand **how they played** and identify areas where their execution can improve.

The goal is not to tell players there is only one correct way to play.

Instead, players will be able to define or select a practice rotation and use MetricHUD to compare that target against their actual combat performance.

**arcdps provides the combat data. MetricHUD aims to help players learn from it.**
