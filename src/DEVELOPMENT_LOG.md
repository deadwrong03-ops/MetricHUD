# MetricHUD Development Log

This document tracks major MetricHUD development milestones, technical discoveries, tests, and current work.

MetricHUD is under active development.

---

# Current Development Version

**0.3.0 Development Build**

Current primary focus:

> Build a reliable player combat-event and skill-activation pipeline before implementing advanced combat analysis and training systems.

---

# Project Foundation

## Core Architecture

Completed:

- Modular source architecture
- Nexus addon initialization
- Nexus addon shutdown
- Render callback
- Options callback
- ConfigManager
- Persistent JSON configuration
- Reset Settings
- MetricRegistry
- Generic metric rendering
- Metric formatting
- Metric ordering
- HUDWindow
- HUD visibility control
- Draggable HUD
- HUD position locking
- Persistent settings

Status:

**PASS**

---

# Initial Metrics

Implemented:

- FPS
- Ping
- Map Name
- Map ID
- Character Name
- Player Speed
- Combat Time
- DPS

Metric visibility can be controlled through the MetricHUD options panel.

Status:

**PASS**

---

# Map and Player Information

Implemented live game information through the available Nexus/Mumble data.

Verified:

- Map ID changes correctly when changing maps.
- Map Name resolves correctly.
- Character Name displays correctly.
- Player Speed updates.
- FPS updates.
- Ping updates.

Status:

**PASS**

---

# ArcDPS Combat Analyzer Foundation

A new `CombatAnalyzer` system was introduced to begin building MetricHUD's combat-analysis layer.

Primary files involved:

```text
CombatAnalyzer.h
CombatAnalyzer.cpp
ArcDPS.h
entry.cpp
```

The analyzer is currently exposed through a development/debug section in the Nexus options panel.

---

## ArcDPS Combat Event Subscription

MetricHUD subscribes to:

```text
EV_ARCDPS_COMBATEVENT_LOCAL_RAW
```

Combat events are received by:

```cpp
OnArcDPSCombat(void* eventArgs)
```

and forwarded into:

```cpp
CombatAnalyzer::ProcessEvent(...)
```

Status:

**PASS**

---

# CombatEvent Structure

MetricHUD currently exposes the ArcDPS combat-event fields required for analysis.

Fields investigated include:

```text
Time
SourceAgent
DestinationAgent
Value
BuffDamage
OverstackValue
SkillID
SourceInstanceID
DestinationInstanceID
SrcMasterInstanceID
DestinationMasterInstanceID
IFF
Buff
Result
IsActivation
IsBuffRemove
IsNinety
IsFifty
IsMoving
IsStatechange
IsFlanking
IsShields
IsOffcycle
```

This expanded structure allows MetricHUD to investigate differences between:

- Damage events
- Activation events
- Buff events
- Triggered effects
- Player actions
- State changes

Status:

**PASS**

---

# Skill Name Resolution

Early CombatAnalyzer testing initially displayed many skills as:

```text
Unknown (SkillID)
```

Investigation confirmed that ArcDPS supplies the skill name through the combat callback.

`ProcessEvent()` was expanded to receive:

```cpp
const char* skillName
```

The callback now forwards:

```cpp
combatAnalyzer.ProcessEvent(ev, combatData->skillname);
```

MetricHUD stores the relationship between:

```text
SkillID ? Skill Name
```

This eliminated the need to manually hardcode every Guild Wars 2 skill ID.

Live testing successfully resolved skill names.

Examples observed:

```text
Death Spiral
Gravedigger
Dusk Strike
Life Rend
Life Reap
Life Slash
Nightfall
Grasping Darkness
Fading Twilight
Chilling Nova
"Chilled to the Bone!"
"You Are All Weaklings!"
```

Status:

**PASS**

---

# Recent Skill Tracking

The Combat Analyzer now maintains a recent-skills list.

This provides a quick live view of skills appearing in the player's combat-event stream.

Verified against ArcDPS during live combat.

Status:

**PASS**

---

# Raw Skill Usage Counter

A skill usage/debug counter was introduced.

Initial logic tracks events by SkillID and applies basic timing suppression to avoid extremely rapid duplicate events.

Example internal concept:

```cpp
skillUseCounts[event->SkillID]++;
```

Live testing showed that this system is useful for inspecting combat behavior but does NOT yet represent true player button presses.

Status:

**PARTIAL / INVESTIGATION**

---

# Critical Finding: Combat Events Are Not Skill Activations

Live testing demonstrated that:

> One player skill activation can generate multiple ArcDPS combat events.

Example:

A player may activate a skill once while MetricHUD receives several events associated with that SkillID.

During testing, **Death's Charge** was a clear example of this behavior.

Therefore:

```text
Raw Skill Events != Player Skill Uses
```

This means raw event counting cannot be used directly for rotation analysis.

This finding changes the architecture of future skill tracking.

MetricHUD must derive a separate **skill activation stream**.

---

# Triggered Skills / Proc Investigation

Testing also identified skills/effects that appear in the combat stream without being directly pressed by the player.

Example:

```text
Chilling Nova
```

Chilling Nova appeared alongside manually activated skills during Necromancer testing.

This confirms that future rotation analysis must distinguish:

```text
Manual Skill Activation
```

from:

```text
Triggered / Proc Effect
```

Hardcoding individual triggered skills is not considered a desirable long-term solution.

The preferred solution is classification using ArcDPS event metadata.

Status:

**UNDER INVESTIGATION**

---

# Activation Event Investigation

The debug panel was expanded to expose activation-related ArcDPS data.

Fields currently being inspected include:

```text
SkillID
Value
BuffDamage
IsActivation
SourceInstanceID
SrcMasterInstanceID
IsStatechange
IsMoving
Time
```

This allows controlled comparisons between:

- manually pressed skills
- damage events
- triggered effects
- multi-hit skills
- activation events

Status:

**ACTIVE DEVELOPMENT**

---

# Player / Self Filtering

Combat events were initially forwarded into CombatAnalyzer before confirming whether they belonged to the player.

This risked NPC/enemy events entering MetricHUD's player skill stream.

`OnArcDPSCombat()` was changed so the analyzer is called only after validating the source.

Current processing order:

```cpp
if (combatData->src == nullptr)
{
    return;
}

if (combatData->src->IsSelf == 0)
{
    return;
}

if (ev->IsStatechange != 0)
{
    return;
}

combatAnalyzer.ProcessEvent(ev, combatData->skillname);
```

This means CombatAnalyzer receives:

```text
Valid source
?
Player/self source
?
Non-statechange event
?
ProcessEvent
```

Live testing confirmed that obvious NPC/enemy abilities no longer contaminate the player's skill list.

Status:

**PASS**

---

# Self-Filter Regression Test

A live regression test was performed after introducing player-source filtering.

Verified:

- Combat Analyzer continued receiving events.
- DPS continued updating.
- Recent Skills continued working.
- Skill names remained correct.
- Skill Usage continued working.
- Activation Events continued being captured.
- No obvious enemy/NPC skills appeared in the player skill list.
- No crash occurred.

Status:

**PASS**

Checkpoint committed and pushed.

---

# DPS Foundation

CombatAnalyzer currently tracks:

```text
Combat Events
Damage Events
Direct Damage
Combat Time
DPS
Last Fight Damage
Last Fight Time
Last Fight DPS
```

The system is still experimental and will require additional refinement as encounter boundaries and combat-state detection improve.

Status:

**WORKING DEVELOPMENT FOUNDATION**

---

# Combat Analyzer Debug Panel

The current debug interface exposes development information including:

```text
Combat Events
Damage Events
Direct Damage
Analyzer Combat Time
Analyzer DPS
Last Fight Damage
Last Fight Time
Last Fight DPS
Last Skill ID
Last Activation
Recent Skills
Skill Usage (Raw Events)
Recent Records
Activation Events
```

This interface is intentionally verbose.

It exists to understand the ArcDPS event stream and is not intended to represent the final MetricHUD user interface.

---

# Current Technical Model

MetricHUD's combat pipeline currently resembles:

```text
ArcDPS
   ?
EV_ARCDPS_COMBATEVENT_LOCAL_RAW
   ?
OnArcDPSCombat()
   ?
Validate event
   ?
Validate source
   ?
Require IsSelf
   ?
Ignore state-change events
   ?
CombatAnalyzer::ProcessEvent()
   ?
Skill-name mapping
   ?
Combat records
   ?
Raw skill events
   ?
Activation-event investigation
   ?
Future reliable player-action stream
```

---

# Next Development Target

## Reliable Skill Activation Counting

The next major task is to determine when a player actually activates a skill.

The desired result is:

```text
Death Spiral
Gravedigger
Nightfall
Grasping Darkness
```

representing actual player actions.

The system must avoid interpreting this:

```text
Death's Charge
Death's Charge
Death's Charge
Death's Charge
Death's Charge
```

as five player presses when those events were generated by one activation.

---

## Data To Investigate

Continue comparing:

```text
SkillID
IsActivation
SourceInstanceID
SrcMasterInstanceID
Time
Value
BuffDamage
IsStatechange
IsMoving
```

between controlled skill activations.

Particular attention should be given to:

- normal weapon skills
- multi-hit skills
- movement skills
- channeled skills
- utility skills
- elite skills
- auto attacks
- triggered/proc effects

---

# Planned CombatAnalyzer Architecture

Long-term separation should likely become:

```text
RAW COMBAT EVENTS
        ?
EVENT CLASSIFICATION
        ?
PLAYER ACTION / ACTIVATION STREAM
        ?
COMBAT METRICS
        ?
ROTATION ANALYSIS
        ?
TRAINING SYSTEM
```

Raw events should remain available for debugging and advanced analysis.

Player actions should be maintained separately.

---

# Future Combat Features

Once reliable activation tracking exists, development can expand toward:

- Skill rotation history
- Rotation timing
- Skill downtime
- Burst-window detection
- Rotation consistency
- DPS phase analysis
- Execute-phase analysis
- Personal benchmark comparison
- Player-selected performance goals

---

# Training System Concepts

Planned training concepts currently include:

- Rotation / priority coaching
- Player skill rotation log
- Phase coaching
- DPS coaching
- Build/gear goal-based optimization
- Boon uptime goals
- Skill downtime analysis
- Burst windows
- Execute phase
- Consistency scoring
- Survivability/downed context
- Personal self-comparison
- Player-selected goals

Potential modes:

```text
Guided
Independent
Benchmark
```

These remain future-development concepts and are not currently implemented.

---

# Other Planned MetricHUD Features

Planned non-training features include:

- Customizable HUD
- Approximately five visible metrics at once
- Improved DPS
- Combat Timer
- Time in Combat
- Downed Count
- Death Count
- Boon Uptime
- Food Timer
- Utility Timer
- Squad Statistics
- Encounter Summary
- World Boss Logging (experimental)
- Jade Bot Buff Tracking (experimental)
- Personal encounter history
- Player-selected metric goals

---

# Development Principles

MetricHUD development follows several core rules.

## Precision Over Repetition

Do not count something simply because ArcDPS emitted an event.

Determine what that event actually represents.

## Stability Before Features

New foundations are tested before additional systems are built on top of them.

## Controlled Testing

When event behavior is unclear, use controlled tests with known skill activations.

## Small Checkpoints

Development should proceed through:

```text
Code
?
Save
?
Build
?
Live Test
?
Verify
?
Commit
?
Push
```

Avoid stacking multiple unverified changes.

---

# Current Stable Checkpoint

At the end of the current development session:

```text
Build: PASS
ArcDPS callback: PASS
Skill names: PASS
Recent skills: PASS
Self-only filtering: PASS
State-change filtering: PASS
DPS regression test: PASS
Activation-event capture: PASS
Live combat stability: PASS
Git commit: COMPLETE
Git push: COMPLETE
```

No unfinished code change is intentionally being left between checkpoints.

---

# Resume Here

**NEXT SESSION STARTING POINT**

Do not begin by changing the HUD or adding another metric.

Resume inside the CombatAnalyzer work.

Primary objective:

> Convert the currently observed ArcDPS event stream into a reliable representation of actual player skill activations.

Start with controlled testing of individual skill presses and compare activation metadata.

Once a reliable rule can distinguish actual player activations from repeated damage/proc events, implement that logic separately from the existing raw-event counter.

The raw counter should remain available for debugging until the activation system is verified.

---

# Last Verified State

MetricHUD builds successfully and runs in Guild Wars 2 without a known crash from the current CombatAnalyzer changes.

The latest verified CombatAnalyzer pipeline filters events to the local player before processing and preserves skill names supplied by ArcDPS.

**Safe stopping point reached.**