# MetricHUD

> ⚠️ **Early Development Build**
>
> MetricHUD is under active development and is not yet feature complete.

A lightweight, customizable metrics HUD for **Guild Wars 2**.

The goal is simple:

> **Show the information the player wants—and nothing they don't.**

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

## Current Metrics

- [x] FPS
- [x] Ping
- [x] Combat Timer
- [x] Map Name
- [x] Map ID
- [x] Character Name

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

- Player Speed
- Player Position
- DPS
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

## Phase 3 — Metrics

- [x] FPS
- [x] Ping
- [x] Combat Timer
- [x] Map Name
- [x] Map ID
- [x] Character Name
- [ ] Player Speed
- [ ] Player Position
- [ ] DPS
- [ ] Boon Uptime
- [ ] Squad Statistics

## Phase 4 — Release

- [ ] Optimization
- [ ] Documentation
- [ ] Version 1.0
