---

# EVTC Offline Combat Analysis

Development expanded beyond the live ArcDPS callback pipeline into direct parsing of ArcDPS EVTC combat-log files.

The purpose of this work is to give MetricHUD access to complete encounter data independently of the live HUD event stream.

This creates a second combat-analysis path:

```text
Live ArcDPS Events
        +
Saved EVTC Combat Logs
```

The EVTC path is intended to support:

- Encounter reconstruction
- Player damage calculation
- DPS calculation
- Skill usage analysis
- Rotation reconstruction
- Historical encounter analysis
- Training-system data

Status:

**ACTIVE DEVELOPMENT / MAJOR FOUNDATION WORKING**

---

# EVTCAnalyzer

A dedicated EVTC analysis system was added.

Primary files:

```text
EVTCAnalyzer.h
EVTCAnalyzer.cpp
```

The analyzer is responsible for loading and interpreting ArcDPS EVTC combat-log files.

The initial goal was not to implement the complete EVTC specification.

Instead, development proceeded incrementally:

```text
Load File
    ↓
Validate Header
    ↓
Locate EVTC Sections
    ↓
Parse Agents
    ↓
Parse Skills
    ↓
Parse Combat Events
    ↓
Identify Player
    ↓
Filter Player Damage
    ↓
Calculate Time Span
    ↓
Calculate DPS
```

This incremental approach allowed each assumption about the EVTC format to be verified against an actual combat log before building additional analysis on top of it.

---

# EVTC File Loading

MetricHUD successfully loads a real ArcDPS EVTC combat-log file.

Debug validation confirmed:

```text
EVTC Load: PASS
```

The loaded file was recognized as:

```text
EVTC20260811
```

The EVTC revision was also successfully extracted.

Example verified runtime data:

```text
EVTC Version: EVTC20260811
EVTC Revision: 1
```

Status:

**PASS**

---

# EVTC Encounter Identification

The parser successfully reads the encounter identifier from the EVTC header.

Verified example:

```text
EVTC Encounter ID: 16199
```

This establishes that MetricHUD can identify the encounter represented by the EVTC file.

Status:

**PASS**

---

# EVTC Agent Table Parsing

The EVTC agent table is now parsed.

Verified test file:

```text
EVTC Agent Count: 28
```

The parser extracts agent information including:

```text
Agent Address
Name
Profession
Elite Specialization
```

This is critical because combat events identify actors using agent addresses rather than character names.

Status:

**PASS**

---

# EVTC Skill Table Parsing

The EVTC skill table is now parsed.

Verified test file:

```text
EVTC Skill Count: 125
```

This provides the mapping required to convert EVTC SkillIDs into readable Guild Wars 2 skill names.

This is separate from the live ArcDPS callback skill-name mapping and allows saved combat logs to be analyzed independently.

Status:

**PASS**

---

# EVTC Combat Event Extraction

MetricHUD successfully located the combat-event section of the EVTC file.

Verified test values:

```text
EVTC Extracted Size: 102348
EVTC Combat Event Offset: 11212
EVTC Combat Event Count: 1424
Parsed Combat Events: 1424
```

The calculated combat-event count matched the number of events successfully parsed.

This was an important structural validation of the EVTC parser.

Status:

**PASS**

---

# EVTC Damage Event Identification

Initial EVTC debugging inspected the parsed combat events for damage-like records.

The initial filter used fields including:

```text
IsStatechange == 0
IsActivation == 0
Buff == 0
Value > 0
```

The test encounter produced:

```text
Damage Combat Events: 16
```

Sample parsed records showed values such as:

```text
Src
Dst
SkillID
Value
```

This demonstrated that MetricHUD was correctly reaching real damage records inside the EVTC event stream.

Status:

**PASS**

---

# EVTC Damage Source Resolution

The next step was determining which agent generated the observed damage events.

A source-agent lookup was added.

The damage events resolved to:

```text
Src 2000 = Sheirina Nightfall
Profession: 8
Elite: 34
```

This confirmed that the source address used by the damage events corresponded to the player's character.

This was the first direct link between:

```text
EVTC Combat Event
        ↓
Source Agent Address
        ↓
Player Character
```

Status:

**PASS**

---

# Dynamic Player Identification

The first player-damage test temporarily used the observed source address:

```text
2000
```

This was useful only as a controlled diagnostic.

It was NOT acceptable as permanent logic because EVTC agent addresses can differ between logs.

The hard-coded source was therefore replaced with dynamic player-agent identification using the parsed EVTC agent table.

The player character was successfully identified as:

```text
Sheirina Nightfall
```

The damage filter now uses the resolved player agent address rather than assuming a fixed address.

Architecture:

```text
EVTC Agent Table
        ↓
Identify Player Agent
        ↓
Retrieve Agent Address
        ↓
Filter Combat Events
        ↓
Player Damage Events
```

Status:

**PASS**

---

# EVTC Player Direct Damage

After resolving the player dynamically, MetricHUD filtered damage events to the player's outgoing direct damage.

Verified test result:

```text
Player Direct Damage: 27551
Player Direct Damage Events: 15
```

The values remained identical after replacing the temporary hard-coded player address with dynamic player identification.

This confirmed that dynamic player resolution was functioning correctly.

Status:

**PASS**

---

# Player Damage Timestamp Tracking

The EVTC player-damage loop was expanded to record the timestamps of the first and last qualifying player damage events.

New tracked values:

```text
First Player Damage Time
Last Player Damage Time
Player Damage Duration
```

Verified runtime values:

```text
First Player Damage Time: 46410217
Last Player Damage Time: 46419174
Player Damage Duration: 8957
```

The duration is therefore:

```text
8957 ms
```

or:

```text
8.957 seconds
```

This provided the time component required for the first independent EVTC DPS calculation.

Status:

**PASS**

---

# First EVTC-Derived Player DPS

Using:

```text
Player Direct Damage: 27551
Player Damage Duration: 8957 ms
```

MetricHUD calculated:

```text
Player EVTC DPS: 3075.9
```

The calculation is:

```text
DPS = Damage / (Duration / 1000)
```

For the verified test:

```text
27551 / 8.957 ≈ 3075.9 DPS
```

This represents the first successfully calculated DPS value produced directly from parsed EVTC player combat data.

The value is not yet intended to represent MetricHUD's final encounter-DPS model.

It currently measures the player's qualifying direct damage across the observed first-to-last player-damage interval.

Status:

**PASS — FIRST EVTC DPS CALCULATION VERIFIED**

---

# Important EVTC DPS Limitation

The current EVTC DPS calculation uses:

```text
Direct player damage
```

and:

```text
First qualifying player damage event
        ↓
Last qualifying player damage event
```

This is an intentionally simple validation model.

It does NOT yet account for every component required for final production DPS reporting.

Future refinement may need to consider:

- Condition damage
- Buff damage
- Player-owned minions/pets
- Encounter start/end boundaries
- Invulnerability phases
- Target filtering
- Multiple targets
- Downed-state behavior
- Encounter-specific mechanics
- Damage generated through player-owned agents

Therefore:

```text
Current EVTC DPS
    =
Verified parsing/calculation foundation

NOT

Final production DPS model
```

---

# EVTC Debug Interface

The Combat Analyzer Debug section currently exposes EVTC diagnostic information including:

```text
EVTC Load Status
EVTC Version
EVTC Revision
EVTC Encounter ID
EVTC Agent Count
EVTC Skill Count
EVTC Extracted Size
EVTC Combat Event Offset
EVTC Combat Event Count
Parsed Combat Events
Damage Combat Events
Sample Damage Events
Damage Source Lookup
Player Direct Damage
Player Direct Damage Events
First Player Damage Time
Last Player Damage Time
Player Damage Duration
Player EVTC DPS
First Event Information
First Skill Information
Parsed Agent Information
```

This interface remains intentionally verbose.

Its purpose is to validate EVTC parsing and combat-analysis assumptions before those systems are exposed through the normal MetricHUD interface.

---

# Current Dual Combat-Analysis Architecture

MetricHUD now has two developing sources of combat information.

```text
                    MetricHUD
                       │
          ┌────────────┴────────────┐
          │                         │
   LIVE ARCDPS EVENTS          EVTC LOG FILE
          │                         │
OnArcDPSCombat()                EVTCAnalyzer
          │                         │
CombatAnalyzer              Parse Header
          │                         │
Live Metrics               Parse Agents
          │                         │
Live Skills                Parse Skills
          │                         │
Activation Research        Parse Events
                                    │
                              Identify Player
                                    │
                              Player Damage
                                    │
                              Damage Duration
                                    │
                                  DPS
```

The two systems are complementary.

The live ArcDPS pipeline is intended for immediate HUD feedback.

The EVTC pipeline is intended for deeper encounter reconstruction, historical analysis, rotation analysis, and training data.

---

# Major Technical Milestone

MetricHUD can now independently perform the following operations on an ArcDPS EVTC combat log:

```text
Load EVTC file
    ↓
Validate EVTC header
    ↓
Read encounter information
    ↓
Parse agent table
    ↓
Parse skill table
    ↓
Locate combat-event section
    ↓
Parse combat events
    ↓
Identify damage events
    ↓
Resolve damage source
    ↓
Identify player dynamically
    ↓
Filter player's direct damage
    ↓
Measure player damage duration
    ↓
Calculate player DPS
```

Every stage above has been validated against the current test EVTC file.

This establishes the first working end-to-end EVTC combat-analysis pipeline in MetricHUD.

---

# Current Stable Checkpoint — EVTC Analyzer

At the end of this development checkpoint:

```text
Build: PASS

EVTC File Load: PASS
EVTC Header Parsing: PASS
EVTC Version: PASS
EVTC Revision: PASS
Encounter ID: PASS

Agent Table Parsing: PASS
Skill Table Parsing: PASS
Combat Event Extraction: PASS
Combat Event Parsing: PASS

Damage Event Detection: PASS
Damage Source Resolution: PASS
Dynamic Player Identification: PASS

Player Direct Damage: PASS
Player Damage Event Count: PASS
First Damage Timestamp: PASS
Last Damage Timestamp: PASS
Damage Duration: PASS

First EVTC Player DPS Calculation: PASS

Runtime Stability: PASS
```

Verified test values:

```text
Character: Sheirina Nightfall

EVTC Version: EVTC20260811
EVTC Revision: 1
Encounter ID: 16199

Agents: 28
Skills: 125
Parsed Combat Events: 1424
Damage Combat Events: 16

Player Direct Damage: 27551
Player Direct Damage Events: 15

First Player Damage Time: 46410217
Last Player Damage Time: 46419174
Player Damage Duration: 8957 ms

Player EVTC DPS: 3075.9
```

---

# Resume Here — EVTC DPS Refinement

**NEXT DEVELOPMENT STARTING POINT**

Do not rebuild the EVTC parser from scratch.

The current parser has successfully demonstrated:

```text
EVTC file
    ↓
Agents
    ↓
Skills
    ↓
Combat Events
    ↓
Player
    ↓
Player Damage
    ↓
Time Span
    ↓
DPS
```

The next objective is to refine the damage model beyond the current direct-damage-only calculation.

Primary question:

> Which EVTC events must be attributed to the player to produce an accurate Guild Wars 2 DPS value?

Investigate the distinction between:

```text
Direct Damage
Condition / Buff Damage
Player-Owned Agent Damage
Triggered Damage
Other Combat Events
```

Do this incrementally.

Preserve the current verified direct-damage calculation as a baseline while additional damage categories are investigated.

Do not remove the EVTC debug output until the expanded damage model has been validated.

---

# Latest Verified State

MetricHUD now has a working end-to-end EVTC parsing foundation.

A real ArcDPS EVTC log has been successfully loaded, its agent/skill/combat-event structures parsed, the player's character dynamically identified, outgoing direct damage isolated, the first/last player-damage timestamps measured, and an independent player DPS value calculated from those records.

The latest verified test produced:

```text
27551 player direct damage
15 player direct-damage events
8957 ms damage interval
3075.9 EVTC-derived DPS
```

This is the current recovery point if development context is lost.

**Safe EVTC checkpoint reached.**
---

# EVTC Damage Model Investigation

## Condition / Buff Damage Path Verified

Development continued beyond the original direct-damage EVTC checkpoint to determine how condition damage is represented in ArcDPS EVTC combat records.

A fresh EVTC log was generated using the character **Blueglove** in the Special Forces Training Area.

Dynamic player identification successfully resolved the player from the EVTC agent table:

```text
Src 2000 = Blueglove | Prof:4 Elite:55
```

This confirms that player identification is no longer dependent on a hard-coded agent address.

The fresh combat log produced:

```text
Player Direct Damage: 149521
Player Direct Damage Events: 29

Player Buff Damage: 26881
Player Buff Damage Events: 38

First Player Damage Time: 127270206
Last Player Damage Time: 127295768
Player Damage Duration: 25562

Player EVTC DPS: 5849.3
```

## Verified EVTC Damage Fields

Testing has now independently confirmed two player-damage paths in EVTC combat records.

### Direct / Strike Damage

Direct player damage is represented by:

```text
event.value
```

and is identified using combat events where:

```text
isStateChange == 0
isActivation == 0
buff == 0
value > 0
srcAgent == playerAgentAddress
```

### Condition / Buff Damage

Condition damage is represented by:

```text
event.buffDmg
```

Diagnostic output from the fresh Blueglove log showed repeated records such as:

```text
BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:543
BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:776
BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:620
```

These events were successfully attributed to the dynamically identified player source.

The EVTC parser therefore successfully detects condition/buff damage independently from direct damage.

## Important Representation Difference

The parsed EVTC log currently exposes condition damage through `buffDmg` as positive values.

Earlier live ArcDPS callback inspection showed `CombatRecord::buffDamage` values using a negative representation.

These two data paths must not automatically be assumed to use identical sign conventions.

MetricHUD will preserve this distinction until both representations have been fully validated.

## Current Damage Model

The existing EVTC DPS calculation is still intentionally based on direct damage only.

The DPS formula has **not yet been changed** to include condition damage.

This was deliberate so that the condition-damage path could be verified independently before modifying the previously validated direct-damage baseline.

The next development step is:

```text
Total Player Damage
    =
Direct / Strike Damage
    +
Condition / Buff Damage
```

After total player damage is validated, EVTC-derived DPS can be recalculated using the combined damage model.


## Latest Recovery Point

MetricHUD currently has verified support for:

- EVTC file loading
- EVTC header parsing
- Agent-table parsing
- Skill-table parsing
- Combat-event parsing
- Dynamic player identification from the EVTC agent table
- Player direct-damage attribution
- Player condition/buff-damage attribution
- Direct-damage event counting
- Condition/buff-damage event counting
- Player damage timestamp tracking
- Player damage-duration calculation
- Direct-damage-only EVTC DPS calculation
- EVTC diagnostic output for individual damage records

The latest verified Blueglove test produced:

```text
Direct Damage: 149521
Direct Damage Events: 29

Condition / Buff Damage: 26881
Condition / Buff Damage Events: 38

Damage Duration: 25562 ms
Direct-Damage EVTC DPS: 5849.3
```

**Safe EVTC direct + condition damage detection checkpoint reached.**

Next objective:

> Combine verified direct and condition damage into total player damage and validate the resulting EVTC DPS before modifying the production CombatAnalyzer damage model.





---

# Combined Direct + Condition Damage Checkpoint

MetricHUD has successfully expanded its EVTC damage model beyond direct damage.

Testing against the Blueglove EVTC log verified:

```text
Player Direct Damage: 149521
Player Direct Damage Events: 29

Player Buff Damage: 26881
Player Buff Damage Events: 38

Player Total Damage: 176402

Player Damage Duration: 25562 ms

Direct-only EVTC DPS: 5849.3
Combined EVTC DPS: 6900.9