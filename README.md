# MetricHUD

> ⚠️ **Early Development Build**
>
> MetricHUD is under active development and is not yet feature complete.

A lightweight, customizable metrics HUD for **Guild Wars 2**.

The goal is simple:

> **Show the information the player wants—and nothing they don't.**

MetricHUD is being developed as a Nexus addon with the long-term goal of providing useful combat metrics, encounter information, and optional player-training tools without overwhelming the screen.

---

## Current Version

**0.3.0 Development Build**

MetricHUD is currently an experimental development project.

Features, interfaces, configuration formats, and internal systems may change as development continues.

---

## Current Status

MetricHUD currently has a working HUD framework, persistent configuration system, metric registry, game/map information, live ArcDPS combat integration, and an experimental EVTC combat-log analysis pipeline.

The current development focus is building a reliable combat-data foundation from both live ArcDPS events and saved EVTC combat logs before expanding into advanced metrics, encounter analysis, rotation analysis, and training features.

---

## Completed

### Core Addon Framework

- Modular project architecture
- Nexus addon loading/unloading
- Stable render callback
- Options panel
- ConfigManager
- JSON settings persistence
- Reset Settings support
- MetricRegistry foundation
- Generic metric rendering
- Metric formatting system
- Metric ordering
- HUDWindow framework
- Live HUD visibility toggle
- Draggable HUD
- Lock HUD Position
- Window/settings persistence

### Current HUD Metrics

MetricHUD currently supports:

- FPS
- Ping
- Map Name
- Map ID
- Character Name
- Player Speed
- Combat Time
- DPS

Metrics can be individually enabled or disabled through the MetricHUD options panel.

### Map / Character Data

- Live map identification
- Map ID tracking
- Map name resolution
- Character name display
- Player movement speed
- MumbleLink integration

### ArcDPS Combat Integration

MetricHUD has a working early ArcDPS combat-event pipeline.

Completed work includes:

- ArcDPS local combat-event subscription
- Combat event forwarding into `CombatAnalyzer`
- Direct damage tracking
- Combat-event counting
- Combat timer foundation
- DPS calculation
- Last-fight data foundation
- Skill ID capture
- ArcDPS skill-name capture
- Skill ID → skill-name mapping
- Recent skill tracking
- Raw skill-event counting
- Activation-event inspection
- Combat-event debug information
- Player/self event filtering
- State-change event filtering
- Live verification against ArcDPS

The analyzer has successfully identified Guild Wars 2 skills including examples such as:

- Death Spiral
- Gravedigger
- Dusk Strike
- Life Rend
- Life Reap
- Life Slash
- Nightfall
- Grasping Darkness
- Fading Twilight
- Chilling Nova
- "Chilled to the Bone!"
- "You Are All Weaklings!"

Triggered effects such as **Chilling Nova** can also be observed separately from manually activated skills.

---

## Combat Analyzer Status

The Combat Analyzer is currently a **development/debug system**.

It is intentionally exposing raw ArcDPS data while the event model is being understood and validated.

Current testing has confirmed that MetricHUD can:

1. Receive ArcDPS combat events.
2. Resolve ArcDPS-provided skill names.
3. Associate names with skill IDs.
4. Filter combat events to the player's own source.
5. Ignore ArcDPS state-change events before analysis.
6. Track recent player skills.
7. Count raw skill-related events.
8. Inspect activation events.
9. Calculate live DPS from the combat stream.
10. Continue operating without crashes during live combat testing.

---

## EVTC Combat Log Analysis

MetricHUD now includes an early working **EVTC combat-log analysis pipeline**.

This allows MetricHUD to analyze locally saved ArcDPS `.evtc` / `.zevtc` combat logs independently of the live ArcDPS event stream.

Current EVTC functionality includes:

- EVTC file loading
- `.zevtc` extraction
- EVTC header parsing
- Encounter ID detection
- Agent-table parsing
- Skill-table parsing
- Combat-event parsing
- Damage-event identification
- Dynamic player-agent identification
- Player direct-damage filtering
- Player damage-event counting
- First/last player damage timestamp tracking
- Damage-duration calculation
- Experimental EVTC-derived DPS calculation

The current EVTC analysis pipeline is:

```text
EVTC Combat Log
      ↓
Parse Header
      ↓
Parse Agents
      ↓
Parse Skills
      ↓
Parse Combat Events
      ↓
Identify Player
      ↓
Identify Player Damage
      ↓
Calculate Damage Duration
      ↓
Calculate DPS
```

### Current EVTC Status

The complete pipeline has been successfully validated against a real ArcDPS combat log.

MetricHUD can currently load the log, reconstruct its core EVTC data structures, identify the player's character dynamically, isolate qualifying outgoing direct-damage events, determine the associated damage interval, and calculate an independent DPS value from those events.

**This EVTC DPS calculation is still experimental.**

The current implementation is a validated foundation rather than the final production DPS model.

Future refinement will investigate additional damage attribution including:

- Condition / buff damage
- Player-owned minion and pet damage
- Triggered damage
- Encounter boundaries
- Target filtering
- Multiple-target encounters
- Other EVTC damage-event classifications

The existing direct-damage calculation will remain as a known-good baseline while the EVTC damage model is expanded.

---

## Important Development Finding

ArcDPS combat events do **not** map one-to-one with player button presses.

A single skill activation may generate multiple combat events.

For example, during testing, skills such as **Death's Charge** generated multiple events from a single player action.

Therefore:

> **Raw combat-event count must not be treated as skill-use count.**

MetricHUD currently preserves the raw event information while development continues on reliable skill-activation detection.

This distinction is important for future rotation analysis and training features.

---

## In Development

### EVTC Damage Model

The EVTC parser has reached a working end-to-end milestone.

Current development is expanding the validated direct-damage model to determine which EVTC events should contribute to accurate player and encounter DPS.

Areas under investigation include:

- Direct damage
- Condition / buff damage
- Player-owned agents
- Pets and minions
- Triggered damage
- Target identification
- Encounter boundaries
- Multi-target encounters

The current direct-damage DPS calculation is being preserved as a validated baseline while additional EVTC event classifications are added.

### Reliable Skill Activation Tracking

Another major CombatAnalyzer milestone is determining when a player actually activated a skill rather than simply counting every ArcDPS event generated by that skill.

Current investigation includes:

- Activation event classification
- Source instance information
- Source master information
- Event timing
- Skill IDs
- Skill names
- Multi-event skills
- Triggered/proc skills
- Manual activation vs triggered effect separation
- Duplicate-event suppression

The goal is to produce a reliable stream resembling:

```text
Player activated Skill A
Player activated Skill B
Player activated Skill C
```

rather than:

```text
Skill A event
Skill A event
Skill A event
Skill B event
Skill B event
```

That reliable activation stream will become the foundation for rotation analysis.

---

## Planned Metrics

Future MetricHUD metrics and systems may include:

### Combat

- Improved DPS
- Combat timer
- Time in combat
- Last-fight statistics
- Downed count
- Death count
- Skill usage
- Skill activation history

### Buffs

- Boon uptime
- Food timer
- Utility timer
- Jade Bot buffs *(experimental)*

### Encounter / Squad

- Squad statistics
- Encounter summaries
- Personal encounter history
- World boss logging *(experimental)*

---

## Training Modules

A major long-term goal for MetricHUD is optional player training and performance analysis.

Planned concepts include:

- Rotation / priority coaching
- Player skill rotation log
- Phase coaching
- DPS coaching
- Goal-based optimization
- Boon uptime goals
- Skill downtime analysis
- Burst-window analysis
- Execute-phase guidance
- Consistency scoring
- Survivability / downed context
- Personal self-comparison
- Player-selected goals and targets

### Training Modes

Potential training modes include:

**Guided**

Provides active guidance while learning a build or rotation.

**Independent**

Tracks performance without constant instruction and provides analysis afterward.

**Benchmark**

Provides structured performance comparison for controlled testing.

The goal is not to force one universal way to play.

MetricHUD should allow players to select the information and goals that matter to them.

---

## HUD Design Philosophy

MetricHUD follows several core principles:

### Show Only Useful Information

The HUD should remain small and readable rather than becoming another large combat-information panel.

### Customizable

Players choose which metrics are visible.

The long-term target is approximately **five visible metrics at once** to prevent HUD clutter.

### Immediate Feedback

Useful information should be available during or immediately after combat.

### Precision Over Repetition

A metric should represent what actually happened rather than simply counting every event available from the combat stream.

### Stability Before Features

New systems are tested incrementally before larger features are built on top of them.

---

## Privacy & Security

MetricHUD is designed to operate without requiring access to your Guild Wars 2 account credentials.

MetricHUD does not require your ArenaNet password.

Combat analysis currently uses locally available game/addon data provided through Nexus, MumbleLink, ArcDPS integration, and locally saved ArcDPS EVTC combat logs.

---

## Development

MetricHUD is currently being developed and tested incrementally.

The development process generally follows:

```text
Implement
↓
Build
↓
Live Test
↓
Verify
↓
Commit
↓
Push
```

Major systems are intentionally developed in small checkpoints so regressions can be identified quickly.

Detailed development checkpoints, technical discoveries, test results, and recovery information are maintained separately in:

```text
DEVELOPMENT_LOG.md
```

---

## Disclaimer

MetricHUD is an unofficial Guild Wars 2 addon.

It is not affiliated with or endorsed by ArenaNet.

Guild Wars 2 and all associated trademarks are property of their respective owners.
