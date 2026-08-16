# MetricHUD

> ⚠️ **Early Development Build**
>
> MetricHUD is under active development and is not yet feature complete.

A lightweight, customizable metrics HUD for **Guild Wars 2**.

The goal is simple:

> **Show the information the player wants—and nothing they don't.**

MetricHUD is being developed as a Nexus addon with the long-term goal of providing useful combat metrics, encounter information, and optional player-training tools without overwhelming the screen.

> **ArcDPS gives you the data. MetricHUD helps you learn from it.**

---

## Current Version

**0.3.0 Development Build**

MetricHUD is currently an experimental development project.

Features, interfaces, configuration formats, and internal systems may change as development continues.

---

## Current Status

MetricHUD currently has a working HUD framework, persistent configuration system, metric registry, game/map information, live ArcDPS combat integration, and an experimental EVTC combat-log analysis pipeline.

The live Combat Analyzer can currently identify player and player-owned agent combat events, process direct and condition/buff damage, track skill usage, attribute damage by skill, measure combat timing, and calculate live DPS.

The EVTC analyzer can independently parse saved ArcDPS combat logs, identify the player, attribute direct and condition damage, identify player-owned agents, and calculate combined player + owned-agent damage and DPS.

The current development focus is validating the remaining differences between MetricHUD's live calculations and ArcDPS before expanding the combat-analysis foundation into advanced metrics, encounter analysis, reliable rotation analysis, and training features.

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

MetricHUD has a working live ArcDPS combat-event pipeline.

Completed and validated work includes:

- ArcDPS LOCAL_RAW combat-event subscription
- Combat event forwarding into `CombatAnalyzer`
- Player instance identification
- Player/self event filtering
- Player-owned agent attribution
- Pet/minion event acceptance through source-master ownership
- Direct damage tracking
- Condition / buff damage tracking
- Combined live damage calculation
- Combat-event counting
- Combat timer foundation
- Live DPS calculation
- First/last damage timestamp tracking
- Last-fight data foundation
- Skill ID capture
- ArcDPS skill-name capture
- Skill ID → skill-name mapping
- Skill usage tracking
- Damage-by-skill tracking
- Recent combat-record tracking
- Raw skill-event counting
- Activation-event inspection
- State-change event filtering
- Combat-event debug information
- Live verification against ArcDPS
- Controlled pet-only combat validation

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

Owned-agent testing has also successfully identified pet-generated skills including:

- Smoke Assault
- Bite
- Takedown
- Bleeding

Triggered effects such as **Chilling Nova** can also be observed separately from manually activated skills.

---

## Combat Analyzer Status

The Combat Analyzer is currently a **development/debug system**.

It intentionally exposes detailed ArcDPS data while the event model, damage attribution, timing model, and skill-activation behavior are being understood and validated.

Current testing has confirmed that MetricHUD can:

1. Receive ArcDPS LOCAL_RAW combat events.
2. Resolve ArcDPS-provided skill names.
3. Associate names with skill IDs.
4. Identify the player's current ArcDPS instance.
5. Accept combat events generated directly by the player.
6. Attribute qualifying player-owned agent events back to the player.
7. Ignore ArcDPS state-change events before damage analysis.
8. Track direct damage.
9. Track condition / buff damage.
10. Attribute damage by skill.
11. Track recent combat records.
12. Track first and last qualifying damage timestamps.
13. Calculate live combat time and DPS.
14. Preserve last-fight information when a new combat session begins.
15. Continue operating without crashes during controlled live combat testing.

Controlled pet-only testing against a Standard Kitty Golem has successfully processed direct damage, Bleeding damage, skill attribution, combat timing, and DPS through the complete live pipeline.

MetricHUD's live calculations are now substantially closer to ArcDPS during controlled testing.

A small remaining DPS difference is still under investigation.

No arbitrary correction factor will be used simply to force the values to match.

---

## EVTC Combat Log Analysis

MetricHUD includes a working experimental **EVTC combat-log analysis pipeline**.

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
- Dynamic player instance identification
- Player direct-damage attribution
- Player condition / buff-damage attribution
- Player-owned agent attribution
- Owned-agent direct-damage attribution
- Direct-damage event counting
- Condition/buff-damage event counting
- First/last damage timestamp tracking
- Combined player + owned-agent damage timing
- Damage-duration calculation
- Experimental combined EVTC-derived DPS calculation

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
Identify Player-Owned Agents
      ↓
Identify Direct + Condition Damage
      ↓
Combine Player + Owned-Agent Damage
      ↓
Calculate Damage Duration
      ↓
Calculate DPS
```

### Current EVTC Status

The complete pipeline has been successfully validated against real ArcDPS combat logs.

MetricHUD can currently load an EVTC log, reconstruct its core data structures, identify the player's character dynamically, identify player-owned agents through instance/master-instance relationships, attribute qualifying direct and condition damage, determine the combined damage interval, and calculate an independent player + owned-agent DPS value.

**EVTC-derived DPS is still experimental.**

The current implementation is a validated analysis foundation rather than the final production encounter-DPS model.

Future refinement will continue investigating:

- Triggered damage
- Encounter boundaries
- Target filtering
- Multiple-target encounters
- Invulnerability phases
- Other EVTC damage-event classifications
- Encounter-specific behavior

The validated direct, condition, and owned-agent calculations will remain known-good baselines while the EVTC damage model is expanded.

---

## Important Development Finding

ArcDPS combat events do **not** map one-to-one with player button presses.

A single skill activation may generate multiple combat events.

For example, during testing, skills such as **Death's Charge** generated multiple events from a single player action.

Therefore:

> **Raw combat-event count must not be treated as skill-use count.**

MetricHUD currently preserves the raw event information while development continues on reliable skill-activation detection.

This distinction is especially important for future rotation analysis and training features.

---

## In Development

### Live DPS Refinement

The live Combat Analyzer now has a validated foundation for:

- Player damage
- Player-owned agent damage
- Direct damage
- Condition / buff damage
- Damage-by-skill attribution
- Combat timing
- Live DPS

Controlled testing has shown MetricHUD operating close to ArcDPS, but a small remaining difference is still being investigated.

Areas under investigation include:

- ArcDPS combat-window timing
- Damage inclusion/exclusion rules
- Triggered damage
- Other condition-damage behavior
- Multi-target behavior
- Differences between first-to-last-damage timing and ArcDPS's internal DPS timing model

The working damage model will not be modified simply to force MetricHUD to display the same number as ArcDPS.

Changes will be based on controlled runtime evidence.

### EVTC Damage Model

The EVTC parser has reached a working end-to-end milestone.

The current model supports verified direct damage, condition/buff damage, player-owned agent attribution, combined player + owned-agent damage, combined timing, and experimental DPS calculation.

Further development will investigate:

- Triggered damage
- Target identification
- Encounter boundaries
- Invulnerability phases
- Multi-target encounters
- Encounter-specific damage behavior

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

That reliable activation stream will become one of the foundations for rotation and priority analysis.

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

Guild Wars 2 can allow players to progress through much of the early game by simply using skills as they become available. That can work perfectly well while leveling and exploring, but it does not necessarily teach the priorities that become increasingly important later in the game.

As combat becomes more demanding, performance can depend on understanding things such as:

- Skill priority
- Rotation structure
- Burst windows
- Boon uptime
- Positioning
- Encounter phases
- Execute phases
- Resource management
- Build and gear interactions

The transition can be difficult because the game does not always clearly explain **why** one approach performs differently from another.

MetricHUD's training systems are intended to help bridge that gap.

The goal is not to tell a player that they played **wrong**.

Instead, MetricHUD should help answer questions such as:

- What happened during the fight?
- Which skills contributed most?
- Where was damage gained or lost?
- Were important skills sitting unused?
- Did priorities change during a particular phase?
- How consistent was the player between attempts?
- Is the player's build or gear aligned with the goal they selected?
- What is the most useful thing to work on next?

> **ArcDPS gives you the data. MetricHUD helps you learn from it.**

Planned concepts include:

- Rotation / priority coaching
- Player skill rotation log
- Phase coaching
- DPS coaching
- Goal-based build and gear optimization
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

Provides active guidance while learning a build, priority system, or rotation.

**Independent**

Tracks performance without constant instruction and provides analysis afterward.

**Benchmark**

Provides structured performance comparison for controlled testing.

Training should favor useful improvement opportunities over punishment or rigid judgments.

Where appropriate, MetricHUD should recognize **priority and phase-based gameplay** rather than assuming that every build must follow one unchanging sequence.

Personal improvement should also be emphasized over simply comparing every player against elite benchmark performance.

The goal is not to force one universal way to play.

MetricHUD should allow players to select the information and goals that matter to them.

---

## How the Pieces Fit Together

MetricHUD's major systems are being developed as parts of the same long-term analysis pipeline:

```text
ArcDPS Live Combat Data
        +
Saved EVTC Combat Logs
        ↓
Reliable Combat Data
        ↓
Damage / Timing / Skills / Buffs
        ↓
Encounter Reconstruction
        ↓
Rotation & Priority Analysis
        ↓
Performance Analysis
        ↓
Optional Training & Coaching
```

The current Combat Analyzer and EVTC work are therefore not separate from the planned training system.

They are the data foundation required to make future coaching accurate and useful.

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

### Constructive Analysis

MetricHUD should identify useful opportunities for improvement without framing player performance as simply right or wrong.

Training and analysis should explain **what happened, why it matters, and what the player could work on next**.

### Personal Improvement

Where possible, progress should be measured against the player's own previous performance and selected goals rather than treating elite benchmark performance as the only definition of success.

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
