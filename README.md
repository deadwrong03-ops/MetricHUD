 MetricHUD

> ⚠️ **Early Development Build**
>
> MetricHUD is under active development and is not yet feature complete.

A lightweight, customizable metric HUD for **Guild Wars 2**.

The goal is simple:

> **Show the information the player wants—and nothing they don't.**

---

# Progress

Current Version: **0.1.1**

## Completed

- [x] Modular project architecture
- [x] ConfigManager
- [x] MetricRegistry foundation
- [x] Generic MetricDefinition framework
- [x] HUDWindow framework
- [x] Live HUD visibility toggle
- [x] Lock HUD position
- [x] Live FPS metric
- [x] Individual metric enable/disable
- [x] Organized options panel

## In Progress

- [ ] Generic metric collection
- [ ] Metric ordering
- [ ] Window position persistence
- [ ] Ping metric

---

# Design Philosophy

MetricHUD is built around a few simple principles.

- **Player choice first** – Show only the metrics the player wants.
- **Immediate feedback** – Changes should be reflected instantly.
- **Clarity** – A clean, self-explanatory interface.
- **Efficiency** – Minimize unnecessary clicks.
- **Modularity** – Every metric should be easy to add, remove, and customize.
- **Stability before features** – Reliability always comes first.

---

# Planned Metrics

- FPS ✅
- Ping
- Combat Timer
- Time in Combat
- DPS
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
- Draggable HUD windows
- Drag-and-drop metric ordering
- Saveable layouts
- Multiple HUD profiles

### Appearance

- Custom colors
- Font scaling
- Transparency controls

### Architecture

- Generic metric registry
- Plugin-style metric system
- Metric categories
- Persistent metric configuration

---

# Roadmap

## Phase 1 — Foundation ✅

- [x] Core architecture
- [x] Configuration system
- [x] Metric registry
- [x] Generic metric definitions
- [x] First configurable metric (FPS)

## Phase 2 — HUD

- [ ] Window position persistence
- [ ] Layout customization
- [ ] Appearance options
- [ ] Metric ordering

## Phase 3 — Metrics

- [ ] Ping
- [ ] Combat Timer
- [ ] DPS
- [ ] Boon Uptime
- [ ] Squad Statistics

## Phase 4 — Release

- [ ] Optimization
- [ ] Documentation
- [ ] Version 1.0
