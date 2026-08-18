# MetricHUD Development Log

This file preserves MetricHUD development, diagnostics, runtime tests, recovery points, and verified checkpoints.

## Checkpoint Index

1. **EVTC OFFLINE COMBAT ANALYSIS**
2. **EVTC DAMAGE MODEL — DIRECT + CONDITION**
3. **OWNED-AGENT DAMAGE AND LIVE COMBAT ANALYZER**
4. **LIVE ARCDPS DAMAGE-STREAM INVESTIGATION AND ANALYZER VALIDATION**
5. **ARCDPS COMBAT-STATE AND DPS TIMING**
6. **PER-SKILL DAMAGE — BLEEDING SOURCE VERIFICATION**
7. **DAMAGE DISCREPANCY — DUAL-VALUE EVENT TEST**
8. **FINAL LIVE DAMAGE ACCOUNTING BASELINE**
9. **LAST FIGHT DPS HUD METRIC**

---

# ============================================================
# CHECKPOINT 1: EVTC OFFLINE COMBAT ANALYSIS
# ============================================================

Development expanded beyond the live ArcDPS callback pipeline into direct parsing of ArcDPS EVTC combat-log files.

The purpose of this work is to give MetricHUD access to complete encounter data independently of the live HUD event stream.

This creates a second combat-analysis path:

Live ArcDPS Events
        +
Saved EVTC Combat Logs

The EVTC path is intended to support:

Encounter reconstruction

Player damage calculation

DPS calculation

Skill usage analysis

Rotation reconstruction

Historical encounter analysis

Training-system data

Status:

ACTIVE DEVELOPMENT / MAJOR FOUNDATION WORKING

## 1. EVTCAnalyzer


A dedicated EVTC analysis system was added.

Primary files:

EVTCAnalyzer.h
EVTCAnalyzer.cpp

The analyzer is responsible for loading and interpreting ArcDPS EVTC combat-log files.

The initial goal was not to implement the complete EVTC specification.

Instead, development proceeded incrementally:

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

This incremental approach allowed each assumption about the EVTC format to be verified against an actual combat log before building additional analysis on top of it.

## 2. EVTC File Loading


MetricHUD successfully loads a real ArcDPS EVTC combat-log file.

Debug validation confirmed:

EVTC Load: PASS

The loaded file was recognized as:

EVTC20260811

The EVTC revision was also successfully extracted.

Example verified runtime data:

EVTC Version: EVTC20260811
EVTC Revision: 1

Status:

PASS

## 3. EVTC Encounter Identification


The parser successfully reads the encounter identifier from the EVTC header.

Verified example:

EVTC Encounter ID: 16199

This establishes that MetricHUD can identify the encounter represented by the EVTC file.

Status:

PASS

## 4. EVTC Agent Table Parsing


The EVTC agent table is now parsed.

Verified test file:

EVTC Agent Count: 28

The parser extracts agent information including:

Agent Address
Name
Profession
Elite Specialization

This is critical because combat events identify actors using agent addresses rather than character names.

Status:

PASS

## 5. EVTC Skill Table Parsing


The EVTC skill table is now parsed.

Verified test file:

EVTC Skill Count: 125

This provides the mapping required to convert EVTC SkillIDs into readable Guild Wars 2 skill names.

This is separate from the live ArcDPS callback skill-name mapping and allows saved combat logs to be analyzed independently.

Status:

PASS

## 6. EVTC Combat Event Extraction


MetricHUD successfully located the combat-event section of the EVTC file.

Verified test values:

EVTC Extracted Size: 102348
EVTC Combat Event Offset: 11212
EVTC Combat Event Count: 1424
Parsed Combat Events: 1424

The calculated combat-event count matched the number of events successfully parsed.

This was an important structural validation of the EVTC parser.

Status:

PASS

## 7. EVTC Damage Event Identification


Initial EVTC debugging inspected the parsed combat events for damage-like records.

The initial filter used fields including:

IsStatechange == 0
IsActivation == 0
Buff == 0
Value > 0

The test encounter produced:

Damage Combat Events: 16

Sample parsed records showed values such as:

Src
Dst
SkillID
Value

This demonstrated that MetricHUD was correctly reaching real damage records inside the EVTC event stream.

Status:

PASS

## 8. EVTC Damage Source Resolution


The next step was determining which agent generated the observed damage events.

A source-agent lookup was added.

The damage events resolved to:

Src 2000 = Sheirina Nightfall
Profession: 8
Elite: 34

This confirmed that the source address used by the damage events corresponded to the player's character.

This was the first direct link between:

EVTC Combat Event
        ↓
Source Agent Address
        ↓
Player Character

Status:

PASS

## 9. Dynamic Player Identification


The first player-damage test temporarily used the observed source address:

2000

This was useful only as a controlled diagnostic.

It was NOT acceptable as permanent logic because EVTC agent addresses can differ between logs.

The hard-coded source was therefore replaced with dynamic player-agent identification using the parsed EVTC agent table.

The player character was successfully identified as:

Sheirina Nightfall

The damage filter now uses the resolved player agent address rather than assuming a fixed address.

Architecture:

EVTC Agent Table
        ↓
Identify Player Agent
        ↓
Retrieve Agent Address
        ↓
Filter Combat Events
        ↓
Player Damage Events

Status:

PASS

## 10. EVTC Player Direct Damage


After resolving the player dynamically, MetricHUD filtered damage events to the player's outgoing direct damage.

Verified test result:

Player Direct Damage: 27551
Player Direct Damage Events: 15

The values remained identical after replacing the temporary hard-coded player address with dynamic player identification.

This confirmed that dynamic player resolution was functioning correctly.

Status:

PASS

## 11. Player Damage Timestamp Tracking


The EVTC player-damage loop was expanded to record the timestamps of the first and last qualifying player damage events.

New tracked values:

First Player Damage Time
Last Player Damage Time
Player Damage Duration

Verified runtime values:

First Player Damage Time: 46410217
Last Player Damage Time: 46419174
Player Damage Duration: 8957

The duration is therefore:

8957 ms

or:

8.957 seconds

This provided the time component required for the first independent EVTC DPS calculation.

Status:

PASS

## 12. First EVTC-Derived Player DPS


Using:

Player Direct Damage: 27551
Player Damage Duration: 8957 ms

MetricHUD calculated:

Player EVTC DPS: 3075.9

The calculation is:

DPS = Damage / (Duration / 1000)

For the verified test:

27551 / 8.957 ≈ 3075.9 DPS

This represents the first successfully calculated DPS value produced directly from parsed EVTC player combat data.

The value is not yet intended to represent MetricHUD's final encounter-DPS model.

It currently measures the player's qualifying direct damage across the observed first-to-last player-damage interval.

Status:

PASS — FIRST EVTC DPS CALCULATION VERIFIED

## 13. Important EVTC DPS Limitation


The current EVTC DPS calculation uses:

Direct player damage

and:

First qualifying player damage event
        ↓
Last qualifying player damage event

This is an intentionally simple validation model.

It does NOT yet account for every component required for final production DPS reporting.

Future refinement may need to consider:

Condition damage

Buff damage

Player-owned minions/pets

Encounter start/end boundaries

Invulnerability phases

Target filtering

Multiple targets

Downed-state behavior

Encounter-specific mechanics

Damage generated through player-owned agents

Therefore:

Current EVTC DPS
    =
Verified parsing/calculation foundation

NOT

Final production DPS model

## 14. EVTC Debug Interface


The Combat Analyzer Debug section currently exposes EVTC diagnostic information including:

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

This interface remains intentionally verbose.

Its purpose is to validate EVTC parsing and combat-analysis assumptions before those systems are exposed through the normal MetricHUD interface.

## 15. Current Dual Combat-Analysis Architecture


MetricHUD now has two developing sources of combat information.

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

The two systems are complementary.

The live ArcDPS pipeline is intended for immediate HUD feedback.

The EVTC pipeline is intended for deeper encounter reconstruction, historical analysis, rotation analysis, and training data.

## 16. Major Technical Milestone


MetricHUD can now independently perform the following operations on an ArcDPS EVTC combat log:

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

Every stage above has been validated against the current test EVTC file.

This establishes the first working end-to-end EVTC combat-analysis pipeline in MetricHUD.

## 17. Current Stable Checkpoint — EVTC Analyzer


At the end of this development checkpoint:

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

Verified test values:

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

## 18. Resume Here — EVTC DPS Refinement


NEXT DEVELOPMENT STARTING POINT

Do not rebuild the EVTC parser from scratch.

The current parser has successfully demonstrated:

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

The next objective is to refine the damage model beyond the current direct-damage-only calculation.

Primary question:

Which EVTC events must be attributed to the player to produce an accurate Guild Wars 2 DPS value?

Investigate the distinction between:

Direct Damage
Condition / Buff Damage
Player-Owned Agent Damage
Triggered Damage
Other Combat Events

Do this incrementally.

Preserve the current verified direct-damage calculation as a baseline while additional damage categories are investigated.

Do not remove the EVTC debug output until the expanded damage model has been validated.

## 19. Latest Verified State


MetricHUD now has a working end-to-end EVTC parsing foundation.

A real ArcDPS EVTC log has been successfully loaded, its agent/skill/combat-event structures parsed, the player's character dynamically identified, outgoing direct damage isolated, the first/last player-damage timestamps measured, and an independent player DPS value calculated from those records.

The latest verified test produced:

27551 player direct damage
15 player direct-damage events
8957 ms damage interval
3075.9 EVTC-derived DPS

This is the current recovery point if development context is lost.

Safe EVTC checkpoint reached.

# ============================================================
# END CHECKPOINT 1: EVTC OFFLINE COMBAT ANALYSIS
# ============================================================


---


# ============================================================
# CHECKPOINT 2: EVTC DAMAGE MODEL — DIRECT + CONDITION
# ============================================================

## 1. Condition / Buff Damage Path Verified


Development continued beyond the original direct-damage EVTC checkpoint to determine how condition damage is represented in ArcDPS EVTC combat records.

A fresh EVTC log was generated using the character Blueglove in the Special Forces Training Area.

Dynamic player identification successfully resolved the player from the EVTC agent table:

Src 2000 = Blueglove | Prof:4 Elite:55

This confirms that player identification is no longer dependent on a hard-coded agent address.

The fresh combat log produced:

Player Direct Damage: 149521
Player Direct Damage Events: 29

Player Buff Damage: 26881
Player Buff Damage Events: 38

First Player Damage Time: 127270206
Last Player Damage Time: 127295768
Player Damage Duration: 25562

Player EVTC DPS: 5849.3

## 2. Verified EVTC Damage Fields


Testing has now independently confirmed two player-damage paths in EVTC combat records.

Direct / Strike Damage

Direct player damage is represented by:

event.value

and is identified using combat events where:

isStateChange == 0
isActivation == 0
buff == 0
value > 0
srcAgent == playerAgentAddress

Condition / Buff Damage

Condition damage is represented by:

event.buffDmg

Diagnostic output from the fresh Blueglove log showed repeated records such as:

BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:543
BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:776
BUFF EVT: Src:2000 Dst:2155 Skill:736 BuffFlag:1 BuffDmg:620

These events were successfully attributed to the dynamically identified player source.

The EVTC parser therefore successfully detects condition/buff damage independently from direct damage.

## 3. Important Representation Difference


The parsed EVTC log currently exposes condition damage through buffDmg as positive values.

Earlier live ArcDPS callback inspection showed CombatRecord::buffDamage values using a negative representation.

These two data paths must not automatically be assumed to use identical sign conventions.

MetricHUD will preserve this distinction until both representations have been fully validated.

## 4. Current Damage Model


The existing EVTC DPS calculation is still intentionally based on direct damage only.

The DPS formula has not yet been changed to include condition damage.

This was deliberate so that the condition-damage path could be verified independently before modifying the previously validated direct-damage baseline.

The next development step is:

Total Player Damage
    =
Direct / Strike Damage
    +
Condition / Buff Damage

After total player damage is validated, EVTC-derived DPS can be recalculated using the combined damage model.

## 5. Latest Recovery Point


MetricHUD currently has verified support for:

EVTC file loading

EVTC header parsing

Agent-table parsing

Skill-table parsing

Combat-event parsing

Dynamic player identification from the EVTC agent table

Player direct-damage attribution

Player condition/buff-damage attribution

Direct-damage event counting

Condition/buff-damage event counting

Player damage timestamp tracking

Player damage-duration calculation

Direct-damage-only EVTC DPS calculation

EVTC diagnostic output for individual damage records

The latest verified Blueglove test produced:

Direct Damage: 149521
Direct Damage Events: 29

Condition / Buff Damage: 26881
Condition / Buff Damage Events: 38

Damage Duration: 25562 ms
Direct-Damage EVTC DPS: 5849.3

Safe EVTC direct + condition damage detection checkpoint reached.

Next objective:

Combine verified direct and condition damage into total player damage and validate the resulting EVTC DPS before modifying the production CombatAnalyzer damage model.

## 6. Combined Direct + Condition Damage Checkpoint


MetricHUD has successfully expanded its EVTC damage model beyond direct damage.

Testing against the Blueglove EVTC log verified:

Player Direct Damage: 149521
Player Direct Damage Events: 29

Player Buff Damage: 26881
Player Buff Damage Events: 38

Player Total Damage: 176402

Player Damage Duration: 25562 ms

Direct-only EVTC DPS: 5849.3
Combined EVTC DPS: 6900.9

# ============================================================
# END CHECKPOINT 2: EVTC DAMAGE MODEL — DIRECT + CONDITION
# ============================================================


---


# ============================================================
# CHECKPOINT 3: OWNED-AGENT DAMAGE AND LIVE COMBAT ANALYZER
# ============================================================

Development continued from the verified EVTC direct + condition damage model into player-owned agent attribution and the live CombatAnalyzer pipeline.

## 1. EVTC Owned-Agent Attribution


Testing with Blueglove confirmed that ArcDPS EVTC combat events expose the relationship between the player and player-owned agents through instance IDs.

Verified player information:

```text
Player Agent: 2000
Player Instance ID: 23

Owned-agent combat records repeatedly showed:

SrcMasterInstid: 23

This established the ownership rule:

event.srcAgent == playerAgentAddress
    OR
event.srcMasterInstid == playerInstanceID
```

The second condition allows damage generated by pets, minions, and other player-owned agents to be attributed back to the player.

Status:

PASS

## 2. EVTC Owned-Agent Damage


Owned-agent damage was initially calculated separately from player damage so the existing verified player-damage calculation would remain untouched during validation.

Verified pet-log results:

Player Total Damage: 87908

Owned Direct Damage: 116136
Owned Direct Damage Events: 68

Owned Buff Damage: 0
Owned Buff Damage Events: 0

Owned Total Damage: 116136

Combined damage:

Player Damage: 87908
Owned Damage: 116136

Combined Player + Owned Damage: 204044

Status:

PASS

## 3. Combined Player + Owned EVTC DPS


The EVTC timing model was expanded to track the earliest and latest qualifying damage events across both the player and owned agents.

The combined damage window uses:

Combined First Damage Time
    =
earliest(Player First Damage, Owned First Damage)

Combined Last Damage Time
    =
latest(Player Last Damage, Owned Last Damage)

Verified test values:

Combined Player + Owned Damage: 204044

Combined First Damage Time: 170608198
Combined Last Damage Time: 170692752

Combined Player + Owned DPS: 2413.2

A duplicate/incorrectly positioned combined-DPS calculation block was discovered during testing when the debug value remained at 0.0.

The duplicate block was removed and the calculation was placed after combined damage and timestamp calculation.

Runtime verification then produced:

Combined Player + Owned DPS: 2413.2
Player + Owned EVTC DPS: 2413.2

The two independently displayed values matched exactly.

Status:

PASS

## 4. Live CombatAnalyzer Owned-Agent Integration


After validating owned-agent attribution using saved EVTC data, development moved into the live ArcDPS CombatAnalyzer pipeline.

The existing live HUD path was traced to:

ArcDPS Combat Callback
        ↓
CombatAnalyzer::ProcessEvent()
        ↓
CombatAnalyzer::GetDPS()
        ↓
MetricRegistry::SetDPS()
        ↓
MetricHUD DPS

The production HUD was confirmed to receive its DPS value from:

metricRegistry.SetDPS(combatAnalyzer.GetDPS());

This confirmed that owned-agent support needed to be integrated into CombatAnalyzer, rather than attempting to pass EVTC debug variables directly into the HUD.

## 5. Live Player Instance Identification


CombatAnalyzer was expanded to store the player's current ArcDPS instance ID.

Added functionality includes:

playerInstanceID
SetPlayerInstanceID()
GetPlayerInstanceID()

The live ArcDPS callback now captures the player's instance ID from self-generated combat events.

Owned-agent events are allowed into the analyzer when:

SrcMasterInstanceID == playerInstanceID

This replaced the previous behavior where all events not marked IsSelf were rejected before reaching CombatAnalyzer.

Status:

PASS

## 6. Live Pet-Only Validation


Testing was performed in the Special Forces Training Area using a Standard Kitty Golem.

The player performed no attacks.

Only the Ranger pet was allowed to attack.

MetricHUD successfully received and processed pet-generated combat events without crashing.

Observed pet skills included:

Smoke Assault — Skill ID 34090
Bite — Skill ID 31312
Takedown — Skill ID 31430
Bleeding — Skill ID 736

ArcDPS live combat details confirmed the same pet skills and condition damage were present in the encounter.

This confirms that player-owned agent events now successfully reach the live CombatAnalyzer.

Status:

PASS

## 7. Live Buff / Condition Damage


The live CombatAnalyzer previously tracked only direct damage.

A separate live damage accumulator was added:

totalBuffDamage

Live condition/buff damage is detected using ArcDPS callback events where:

BuffDamage < 0

This is intentionally different from the EVTC parser, where verified buffDmg values use a positive representation.

The live DPS calculation now uses:

totalDirectDamage + totalBuffDamage

divided by the measured first-to-last damage interval.

A reset bug was discovered during testing:

totalDirectDamage

was cleared by ResetSession(), but the newly added:

totalBuffDamage

was not.

This caused buff damage from previous tests to remain accumulated.

ResetSession() was corrected to clear both damage accumulators.

Status:

PASS

## 8. Live DPS Comparison Testing


Multiple pet-only tests were performed against ArcDPS.

After owned-agent and buff-damage support was added, MetricHUD's live DPS moved substantially closer to ArcDPS.

One longer controlled pet-only test produced approximately:

MetricHUD

Direct Damage: 45111
Buff Damage: 3663
Combined Damage: 48774
Analyzer Combat Time: 32.60 sec
Analyzer DPS: 1496

ArcDPS displayed approximately:

Damage: 49.7k
DPS: 1523

Difference:

MetricHUD DPS: 1496
ArcDPS DPS:    1523

Difference: 27 DPS
Approximately 1.8%

A separate longer pet-only test produced:

MetricHUD

Direct Damage: 33293
Buff Damage: 2059
Combined Damage: 35352
Analyzer Combat Time: 21.16 sec
Analyzer DPS: 1671

ArcDPS displayed approximately:

Damage: 34.9k
DPS: 1647

The remaining small difference is still under investigation and should not be corrected by arbitrary adjustment.

ArcDPS and MetricHUD may use slightly different damage inclusion or timing rules.

The current objective is to identify any remaining event categories before considering the live DPS model final.

## 9. Live Combat Timing Diagnostics


Additional debug accessors were added for:

First Damage Time
Last Damage Time

These allow the live CombatAnalyzer's first-to-last damage interval to be inspected directly.

The existing combat-session lifecycle was also verified:

Enter Combat
    ↓
ResetSession()

Leave Combat
    ↓
CaptureLastFight()

A completed fight was successfully preserved in the Last Fight values while the next fight began with a fresh analyzer session.

Status:

PASS

## 10. Current Stable Checkpoint — Owned Agents and Live DPS


At the end of this development session:

Build: PASS
Runtime Stability: PASS

EVTC Player Direct Damage: PASS
EVTC Player Buff / Condition Damage: PASS
EVTC Owned-Agent Attribution: PASS
EVTC Owned-Agent Direct Damage: PASS
EVTC Combined Player + Owned Damage: PASS
EVTC Combined Player + Owned Timing: PASS
EVTC Combined Player + Owned DPS: PASS

Live Player Instance Identification: PASS
Live Owned-Agent Event Acceptance: PASS
Live Pet-Only Damage Processing: PASS
Live Buff / Condition Damage: PASS
Live Buff-Damage Session Reset: PASS
Live DPS Pipeline to MetricRegistry: PASS
Live HUD DPS Display: PASS
Last-Fight Capture / New-Fight Reset: PASS

The live analyzer has been tested with pet-only combat against a Standard Kitty Golem and remains stable.

MetricHUD's live DPS is now substantially closer to ArcDPS during owned-agent testing, with longer controlled tests producing differences in roughly the 1–2% range.

The remaining discrepancy has not been artificially corrected.

Further work should determine whether it comes from:

Additional ArcDPS damage-event categories

Damage inclusion/exclusion rules

Combat-window timing differences

Triggered damage

Other owned-agent behavior

## 11. Resume Here — Live DPS Refinement


NEXT DEVELOPMENT STARTING POINT

Do not rebuild the owned-agent system from scratch.

The current verified live path is:

ArcDPS Event
    ↓
Identify Player Instance
    ↓
Accept Player Event
        OR
Accept Owned-Agent Event
    ↓
CombatAnalyzer
    ↓
Direct Damage + Buff Damage
    ↓
First / Last Damage Time
    ↓
GetDPS()
    ↓
MetricRegistry
    ↓
MetricHUD DPS

Pet-only combat has been successfully processed through this complete path.

The next objective is to investigate the small remaining difference between MetricHUD and ArcDPS without disturbing the verified owned-agent and condition-damage foundation.

Preserve the current debug output until the remaining damage/timing behavior has been identified.

Safe owned-agent + live buff-damage + live DPS checkpoint reached.

# ============================================================
# END CHECKPOINT 3: OWNED-AGENT DAMAGE AND LIVE COMBAT ANALYZER
# ============================================================


---


# ============================================================
# CHECKPOINT 4: LIVE ARCDPS DAMAGE-STREAM INVESTIGATION AND ANALYZER VALIDATION
# ============================================================

Development continued from the stable owned-agent + live buff-damage checkpoint to investigate the remaining small difference between MetricHUD's live DPS calculation and ArcDPS.

The purpose of this investigation was to determine whether MetricHUD was losing damage before events reached CombatAnalyzer, particularly condition damage generated by player-owned agents.

## 1. Live Bleeding Damage Investigation


Pet-only testing against a Standard Kitty Golem continued using the Ranger pet.

Bleeding was selected as the primary diagnostic condition because:

- it was clearly visible in ArcDPS
- it was generated by the pet
- it used a known Skill ID
- it appeared consistently during controlled tests

Verified Bleeding Skill ID:

736

Temporary diagnostic counters were added to compare different points in the ArcDPS event pipeline.

These diagnostics included:

Raw Bleeding Damage

Rejected Bleeding Damage

SQUAD_RAW Bleeding Damage

The purpose was to determine whether Bleeding damage was:

received by the LOCAL_RAW callback,

rejected by MetricHUD's ownership filter,

or available through a separate ArcDPS combat-event stream.

## 2. LOCAL_RAW Bleeding Verification


Controlled pet-only tests demonstrated that the existing:

EV_ARCDPS_COMBATEVENT_LOCAL_RAW

subscription already receives the pet-generated Bleeding damage required by MetricHUD.

One verified test produced matching values:

Buff Damage: 2733

Raw Bleeding Damage: 2733

SQUAD Raw Bleeding Damage: 0

A later test produced:

Buff Damage: 5162

Raw Bleeding Damage: 5162

ArcDPS Bleeding: 5162

These results demonstrate that the condition damage was already present in the LOCAL_RAW event stream.

MetricHUD was therefore not missing Bleeding because it required a different ArcDPS stream.

Status:

PASS

## 3. Ownership Filter Investigation


A temporary Rejected Bleeding Damage counter was added before the ownership-filter return.

The purpose was to determine whether valid pet Bleeding events were reaching the callback but being discarded because the source was not recognized as the player or a player-owned agent.

Controlled testing did not identify the missing damage as rejected Bleeding through this filter.

This eliminated the ownership-filter rejection theory as the primary explanation for the remaining DPS difference.

The previously implemented player-instance / SrcMasterInstanceID ownership logic remains the verified live ownership model.

Status:

PASS

## 4. SQUAD_RAW Diagnostic Stream


A temporary second ArcDPS callback was added using:

EV_ARCDPS_COMBATEVENT_SQUAD_RAW

The second callback was intentionally kept separate from CombatAnalyzer.

Its only purpose was to count Bleeding damage so the LOCAL_RAW and SQUAD_RAW streams could be compared without disturbing the working production analyzer.

A temporary callback was added:

OnArcDPSSquadCombat()

Testing showed:

LOCAL_RAW Bleeding damage was populated.

SQUAD_RAW Bleeding damage remained 0 during the controlled pet-only test.

This demonstrated that SQUAD_RAW did not provide the missing pet-condition damage being investigated.

The production analyzer therefore remains subscribed to:

EV_ARCDPS_COMBATEVENT_LOCAL_RAW

Status:

PASS — SQUAD_RAW NOT REQUIRED

## 5. ArcDPS API Verification


During the investigation, Nexus.h was inspected directly to verify the current Nexus event API.

The AddonAPI structure exposes:

Events_Subscribe

Events_Unsubscribe

The correct API usage remains:

APIDefs->Events_Subscribe(...)

and:

APIDefs->Events_Unsubscribe(...)

There is no nested:

APIDefs->Events.Subscribe

interface in the current Nexus API used by MetricHUD.

This was verified directly against the project's Nexus.h definitions.

Status:

PASS

## 6. Diagnostic Cleanup


After the LOCAL_RAW versus SQUAD_RAW comparison was complete, all temporary Bleeding stream diagnostics were removed.

Removed temporary development state included:

rawBleedingDamage

rejectedBleedingDamage

squadRawBleedingDamage

Removed temporary callback:

OnArcDPSSquadCombat()

Removed temporary subscription:

EV_ARCDPS_COMBATEVENT_SQUAD_RAW

Removed temporary debug display lines for:

Raw Bleeding Damage

Rejected Bleeding Damage

SQUAD Raw Bleeding Damage

The verified production CombatAnalyzer logic was preserved.

The project was rebuilt after cleanup.

Build:

PASS

## 7. Post-Cleanup Runtime Sanity Test


A final controlled pet-only runtime test was performed after all temporary stream diagnostics were removed.

Test environment:

Special Forces Training Area

Target:

Standard Kitty Golem

Player attacks:

None

Damage source:

Ranger pet only

The analyzer successfully processed the fight.

Verified MetricHUD debug values:

Combat Events: 32

Damage Events: 20

Zero-Skill Damage Events: 0

Direct Damage: 21067

Buff Damage: 1846

First Damage Time: 263152112

Last Damage Time: 263170669

Analyzer Combat Time: 18.56

Analyzer DPS: 1186.2

Skill Usage:

Smoke Assault (34090): 5

Bite (31312): 8

Bleeding (736): 18

Takedown (31430): 1

Damage By Skill:

Smoke Assault (34090): 4863

Bite (31312): 14076

Bleeding (736): 1846

Takedown (31430): 1228

The Damage By Skill totals correctly account for:

Direct Damage: 21067

Buff Damage: 1846

Total recorded damage:

22913

The analyzer remained stable and Recent Records continued to populate.

No crash occurred.

Status:

PASS

## 8. Important Timing Observation


The final sanity test displayed:

First Damage Time: 263152112

Last Damage Time: 263170669

Analyzer Combat Time: 18.56

Analyzer DPS: 1186.2

The damage counters and per-skill attribution are internally consistent, but the remaining ArcDPS-versus-MetricHUD DPS discrepancy should still be treated as unresolved.

Do not introduce an arbitrary correction factor.

Future work should continue to verify exactly which timing window ArcDPS uses for its displayed DPS and whether additional combat-event inclusion rules affect the result.

## 9. Current Live Damage Pipeline


The verified production path remains:

ArcDPS LOCAL_RAW Event
    ↓
Identify Player Instance
    ↓
Accept Player Event
        OR
Accept Player-Owned Agent Event
    ↓
Ignore State-Change Records
    ↓
CombatAnalyzer::ProcessEvent()
    ↓
Direct Damage
        +
Buff / Condition Damage
    ↓
Damage By Skill
    ↓
First / Last Damage Timestamp
    ↓
CombatAnalyzer::GetDPS()
    ↓
MetricRegistry
    ↓
MetricHUD HUD

This path successfully handles pet-generated:

Strike damage

Condition damage

Skill attribution

Combat timing

DPS calculation

Recent combat records

## 10. Current Stable Checkpoint — Live Analyzer Stream Investigation


At the end of this checkpoint:

Build: PASS

Runtime Stability: PASS

LOCAL_RAW Event Reception: PASS

Player Event Acceptance: PASS

Owned-Agent Event Acceptance: PASS

Pet Direct Damage: PASS

Pet Buff / Condition Damage: PASS

Bleeding Damage Reception: PASS

Bleeding Skill Attribution: PASS

Damage By Skill: PASS

First / Last Damage Timestamp Tracking: PASS

Combat Time: PASS

Live DPS Calculation: PASS

Recent Records: PASS

SQUAD_RAW Comparison: COMPLETE

SQUAD_RAW Requirement: NOT REQUIRED

Temporary Bleeding Diagnostics: REMOVED

Temporary SQUAD_RAW Callback: REMOVED

Production Analyzer Preserved: PASS

Final Post-Cleanup Runtime Test: PASS

## 11. Resume Here — Live DPS Timing Refinement


NEXT DEVELOPMENT STARTING POINT

Do not rebuild the live CombatAnalyzer or owned-agent system from scratch.

The LOCAL_RAW stream has now been verified to contain the pet-generated condition damage being investigated.

The remaining DPS difference is not explained by missing Bleeding damage from the LOCAL_RAW callback.

The current verified foundation is:

LOCAL_RAW
    ↓
Player / Owned-Agent Filtering
    ↓
Direct Damage + Buff Damage
    ↓
Per-Skill Attribution
    ↓
Damage Timing
    ↓
Live DPS

The next objective is to investigate the remaining DPS discrepancy primarily from the perspective of:

ArcDPS combat-window timing

Damage-event inclusion/exclusion rules

Triggered damage behavior

Other condition-damage behavior

Multi-target behavior

Potential differences between first-to-last-damage timing and ArcDPS's internal DPS timing model

Do not modify the working damage totals merely to force MetricHUD to match ArcDPS.

Any future change to the DPS model should be supported by controlled runtime evidence.

Safe LOCAL_RAW + owned-agent + condition-damage + diagnostic-cleanup checkpoint reached.

# ============================================================
# END CHECKPOINT 4: LIVE ARCDPS DAMAGE-STREAM INVESTIGATION AND ANALYZER VALIDATION
# ============================================================


---


# ============================================================
# CHECKPOINT 5: ARCDPS COMBAT-STATE AND DPS TIMING
# ============================================================

## 1. ArcDPS State-Change Verification


The LOCAL_RAW callback was tested for ArcDPS state-change records.

Controlled runtime testing verified:

- `IsStatechange == 1` = player ENTERCOMBAT
- `IsStatechange == 2` = player EXITCOMBAT

MetricHUD now uses these ArcDPS state changes for CombatAnalyzer session boundaries.

On player ENTERCOMBAT:

- `CombatAnalyzer::ResetSession()` is called.
- The ArcDPS event timestamp is stored as the combat start time.

On player EXITCOMBAT:

- `CombatAnalyzer::CaptureLastFight()` is called.

The previous Mumble-based CombatAnalyzer reset/capture behavior was removed.

Mumble `IsInCombat` remains available for the HUD Combat Time metric, but it is no longer responsible for CombatAnalyzer session boundaries.

Status: PASS

## 2. Last Fight Damage Correction


`CaptureLastFight()` previously stored only direct damage:

`lastFightDamage = totalDirectDamage`

This was corrected to:

`lastFightDamage = totalDirectDamage + totalBuffDamage`

Controlled runtime testing verified that Last Fight Damage now exactly equals MetricHUD's Direct Damage + Buff Damage totals.

Status: PASS

## 3. Raw Damage Diagnostic


A temporary `arcTotalDamage` counter was added directly to the LOCAL_RAW callback to compare raw negative ArcDPS damage values against CombatAnalyzer totals.

The diagnostic counted:

- Negative `Value` as direct damage.
- Negative `BuffDamage` as buff / condition damage.

Controlled testing showed the raw callback total matched:

`CombatAnalyzer Direct Damage + CombatAnalyzer Buff Damage`

This confirmed that the analyzer's internally accepted damage records were being totaled consistently.

The temporary `arcTotalDamage` diagnostic was removed after verification.

Status: PASS

## 4. DPS Timing Model Investigation


The previous CombatAnalyzer DPS duration was:

`first damage timestamp -> last damage timestamp`

Controlled testing showed that this duration was consistently slightly shorter than the timing represented by ArcDPS's Damage window.

Several candidate timing windows were measured without initially changing the production DPS formula:

1. First Damage -> Last Damage
2. ArcDPS ENTERCOMBAT -> Last Damage
3. ArcDPS ENTERCOMBAT -> EXITCOMBAT
4. Mumble combat-state duration

ENTERCOMBAT -> EXITCOMBAT was clearly too long because ArcDPS can keep the player in combat well after the final damaging event.

Mumble combat time was also unsuitable for the DPS denominator.

The strongest match was consistently:

`ArcDPS ENTERCOMBAT -> Last Damage`

Controlled examples included:

- MetricHUD Arc Start -> Last Damage: 50.48 seconds
- ArcDPS displayed fight time: approximately 50 seconds

and:

- MetricHUD Arc Start -> Last Damage: 45.80 seconds
- ArcDPS damage/time values implied approximately 45.8 seconds

After changing the DPS denominator to ArcDPS ENTERCOMBAT -> Last Damage, another runtime comparison produced:

- MetricHUD Damage: 208352
- MetricHUD DPS: 5096.7
- ArcDPS Damage: approximately 209.8k
- ArcDPS Damage-window DPS: approximately 5109/s

The remaining difference was small and separate from the timing-model issue.

Based on repeated controlled runtime evidence, MetricHUD now calculates DPS using:

`ArcDPS ENTERCOMBAT timestamp -> last accepted damage timestamp`

Status: PASS

## 5. Last Fight Timing Alignment


After the DPS timing change, Last Fight Time still used the old first-damage -> last-damage duration while Last Fight DPS used the new Arc-start timing.

`CaptureLastFight()` was updated so Last Fight Time and Last Fight DPS now use the same verified timing model.

Final runtime verification showed:
- Arc Start -> Last Damage: 38.60 seconds
- Last Fight Time: 38.60 seconds
- Analyzer DPS: 4182.6
- Last Fight DPS: 4182.6

Status: PASS

## 6. Timing Diagnostic Cleanup


Temporary investigation-only code was removed after verification:

- `arcTotalDamage`
- ENTERCOMBAT -> EXITCOMBAT duration storage
- `combatEndTime`
- `SetCombatEndTime()`
- `GetArcCombatDurationSeconds()`
- Arc Combat Time debug display
- Temporary `STATE TEST` version text

The production timing pieces retained are:

- ArcDPS ENTERCOMBAT session reset
- `combatStartTime`
- `SetCombatStartTime()`
- `GetArcStartToLastDamageSeconds()`
- ArcDPS EXITCOMBAT Last Fight capture

## 7. Final Post-Cleanup Runtime Verification


Final runtime sanity testing after diagnostic cleanup verified:

- No crash
- ArcDPS state-change handling operational
- `Last Arc StateChange: 2` after combat exit
- Direct damage operational
- Buff / condition damage operational
- Arc Start -> Last Damage operational
- Live DPS operational
- Last Fight Damage operational
- Last Fight Time operational
- Last Fight DPS operational
- Recent Skills / Skill Usage / Damage By Skill remained operational

Final observed test:

- Direct Damage: 322824
- Buff Damage: 19591
- Total / Last Fight Damage: 342415
- Analyzer Combat Time: 57.15 seconds
- Arc Start -> Last Damage: 57.16 seconds
- Analyzer DPS: 5990.3
- Last Fight Time: 57.16 seconds
- Last Fight DPS: 5990.3
- Last Arc StateChange: 2

Build: PASS  
Runtime: PASS  
DPS timing refinement: PASS  
Diagnostic cleanup: PASS

## 8. Current Stable DPS Pipeline


LOCAL_RAW
↓
Player / Owned-Agent Filtering
↓
ArcDPS ENTERCOMBAT Session Start
↓
Direct Damage + Buff / Condition Damage
↓
Per-Skill Attribution
↓
Last Damage Timestamp
↓
Arc Start -> Last Damage Duration
↓
Live DPS
↓
ArcDPS EXITCOMBAT Last Fight Capture

The previous Live DPS Timing Refinement objective is now complete.

Do not replace the verified ArcDPS ENTERCOMBAT -> Last Damage timing model without controlled runtime evidence demonstrating a more accurate model.

Stable ArcDPS state-change + damage-accounting + DPS-timing checkpoint reached.

# ============================================================
# END CHECKPOINT 5: ARCDPS COMBAT-STATE AND DPS TIMING
# ============================================================


---


# ============================================================
# CHECKPOINT 6: PER-SKILL DAMAGE — BLEEDING SOURCE VERIFICATION
# ============================================================

A simplified golem test was used to compare ArcDPS and MetricHUD damage on a per-skill basis.

Direct-damage skills compared closely between the two tools, including:

- Ricochet
- Bite
- Smoke Assault
- Takedown

An apparent discrepancy was initially observed in Bleeding totals, particularly ArcDPS's `Bleeding (m)` minion-owned category.

## 1. Accepted Bleeding Source Diagnostic


Temporary diagnostics separated accepted Bleeding damage into:

- Self Bleeding
- Owned-agent Bleeding

One controlled test produced:

ArcDPS:
- Bleeding (m): 5943
- Bleeding: 659

MetricHUD:
- Owned Bleeding: 4145
- Self Bleeding: 657

This initially suggested a shortage on the owned-agent side.

## 2. Null-Source Bleeding Test


A temporary diagnostic was added before the existing:

`combatData->src == nullptr`

return to determine whether Bleeding events were being discarded because ArcDPS supplied no source-agent object.

Controlled test:

ArcDPS:
- Bleeding (m): 4962
- Bleeding: 641

MetricHUD:
- Owned Bleeding: 3436
- Self Bleeding: 641
- Null-Source Bleeding: 0

Result:

Null-source Bleeding did not explain the apparent discrepancy.

## 3. Ownership-Rejection Bleeding Test


A temporary Bleeding-only counter was then added immediately before the existing ownership-filter return.

The ownership rule itself was not changed.

Final controlled test:

ArcDPS:
- Bleeding (m): 2900
- Bleeding: 764

MetricHUD:
- Owned Bleeding: 2900
- Self Bleeding: 764
- Null-Source Bleeding: 0
- Rejected Bleeding: 0

The final controlled test matched exactly:

- ArcDPS player Bleeding = MetricHUD Self Bleeding
- ArcDPS minion Bleeding = MetricHUD Owned Bleeding

Result:

The earlier apparent Bleeding shortage was not reproducible.

The tests provide additional confirmation that:

- LOCAL_RAW receives player Bleeding.
- LOCAL_RAW receives owned-agent / pet Bleeding.
- Self Bleeding attribution is working.
- Owned-agent Bleeding attribution is working.
- Bleeding is not being lost through the null-source check.
- Bleeding is not being lost through the verified ownership filter.

No production ownership or damage-accounting changes were made.

All temporary Bleeding diagnostics were removed after testing.

Production callback restored exactly to the committed version.

Clean Release x64 rebuild: PASS

Git working tree after cleanup: CLEAN

Status: BLEEDING PATH VERIFIED / NO PRODUCTION CHANGE REQUIRED

## 4. Next Investigation Direction


Do not continue modifying Bleeding handling simply to force a match with an isolated ArcDPS display discrepancy.

Any remaining ArcDPS-versus-MetricHUD damage difference should be reproduced first in a controlled test and then isolated to a specific skill or event category before production damage logic is changed.

# ============================================================
# END CHECKPOINT 6: PER-SKILL DAMAGE — BLEEDING SOURCE VERIFICATION
# ============================================================


---


# ============================================================
# CHECKPOINT 7: DAMAGE DISCREPANCY — DUAL-VALUE EVENT TEST
# ============================================================

A temporary diagnostic was added to test whether ArcDPS LOCAL_RAW produces damage events containing both:

- `Value < 0`
- `BuffDamage < 0`

This was investigated because MetricHUD's direct-damage rule only counts negative `Value` when `BuffDamage == 0`.

Controlled runtime testing produced:

- Zero-Skill Damage Events: 0
- Dual-Value Damage Events: 0
- Dual-Value Direct Damage: 0

Result:

Dual-value damage events do not explain the remaining small ArcDPS-versus-MetricHUD damage difference observed in this test.

The production direct-damage and buff/condition-damage rules were left unchanged.

The temporary dual-value diagnostic was completely removed after testing.

Build after cleanup: PASS

Git working tree after cleanup: CLEAN

Status: THEORY ELIMINATED

## 1. Next Investigation Target


Do not repeat the already completed investigations for:

- LOCAL_RAW pet condition reception
- SQUAD_RAW
- owned-agent filtering
- rejected Bleeding
- SkillID == 0 damage
- dual-value damage events
- DPS timing

The next controlled investigation should compare ArcDPS and MetricHUD damage on a per-skill basis to identify which specific direct-damage skill or category accounts for any remaining difference.

# ============================================================
# END CHECKPOINT 7: DAMAGE DISCREPANCY — DUAL-VALUE EVENT TEST
# ============================================================


---


# ============================================================
# CHECKPOINT 8: FINAL LIVE DAMAGE ACCOUNTING BASELINE
# ============================================================

Additional controlled tests were performed after the Bleeding-source investigation to determine whether the remaining ArcDPS-versus-MetricHUD damage discrepancy could be reproduced reliably.

## 1. Player-Only Direct-Damage Baseline


A Necromancer/Reaper was used with no minions and Greatsword autoattack only.

No shroud, wells, utilities, minions, or other damage sources were used.

Observed MetricHUD total:

- Direct Damage: 141665
- Buff Damage: 0
- Total Damage: 141665

MetricHUD per-skill totals:

- Chilling Scythe: 59859
- Fading Twilight: 43610
- Dusk Strike: 30699
- Chilling Nova: 7497

These skill totals sum exactly to:

`141665`

ArcDPS displayed approximately:

- Total Damage: 141.7k
- DPS: 5640

MetricHUD displayed:

- DPS: 5642.5

Per-skill values closely matched ArcDPS.

Result:

Player-only direct damage accounting is verified.

Status: PASS

## 2. Pet-Only Baseline


A Ranger/Smokescale test was then performed with the player dealing no damage.

Initial comparisons were complicated by ArcDPS's completed-encounter display switching back to a previous Reaper fight after the golem was removed.

For subsequent testing, ArcDPS Chat Skills and MetricHUD Damage By Skill were compared while the same pet-only encounter was still active.

To prevent asynchronous display updates from producing false differences, the pet was placed into passive / Avoid Combat mode while the golem remained spawned, and the displays were allowed to become stationary before comparison.

Final frozen pet-only comparison:

ArcDPS Chat Skills:

- Bite (m): approximately 25.2k
- Smoke Assault (m): 7632
- Takedown (m): 3769
- Bleeding (m): 2897

MetricHUD Damage By Skill:

- Bite: 25226
- Smoke Assault: 8942
- Takedown: 3769
- Bleeding: 2897

MetricHUD totals:

- Direct Damage: 37973
- Buff Damage: 2897
- Total Damage: 40870

ArcDPS overall Damage window displayed approximately:

- Total Damage: 40.8k

Bite, Takedown, Bleeding, and the overall encounter total matched or closely matched MetricHUD.

The only remaining per-skill difference appeared in ArcDPS Chat Skills for Smoke Assault.

However, the ArcDPS Chat Skills rows themselves summed to less than ArcDPS's own overall Damage total by approximately the same amount as the Smoke Assault difference.

Therefore the Smoke Assault discrepancy appears to be an ArcDPS skill-attribution / display difference rather than missing or extra MetricHUD damage.

No production damage-accounting change was justified.

## 3. Final Damage Investigation Result


Controlled runtime testing now verifies:

- Player direct damage accounting: PASS
- Player condition damage accounting: PASS
- Owned-agent direct damage accounting: PASS
- Owned-agent condition damage accounting: PASS
- Bite attribution: PASS
- Takedown attribution: PASS
- Bleeding attribution: PASS
- Player-only total damage: PASS
- Pet-only total damage: PASS
- ArcDPS ENTERCOMBAT -> Last Damage DPS timing: PASS
- Last Fight Damage / Time / DPS capture: PASS

Previously investigated and eliminated theories include:

- Missing LOCAL_RAW pet Bleeding
- Need for SQUAD_RAW
- Ownership-filter rejection
- Null-source Bleeding
- SkillID == 0 damage
- Dual-value damage events
- Incorrect first-to-last-damage DPS timing

The live CombatAnalyzer damage foundation should now be treated as stable.

Do not modify working damage totals or ownership rules merely to force individual ArcDPS UI subviews to match when ArcDPS's own displayed breakdowns are internally inconsistent.

Any future damage-accounting change must first reproduce a discrepancy in a controlled encounter where:

1. Both tools are confirmed to be displaying the same encounter.
2. Damage sources are isolated.
3. Display values are stationary before comparison.
4. The overall encounter totals disagree, not only a secondary skill-attribution view.

Status: LIVE DAMAGE ACCOUNTING BASELINE COMPLETE

Stable LOCAL_RAW + owned-agent + direct-damage + condition-damage + DPS-timing checkpoint reached.

# ============================================================
# END CHECKPOINT 8: FINAL LIVE DAMAGE ACCOUNTING BASELINE
# ============================================================


---


# ============================================================
# CHECKPOINT 9: LAST FIGHT DPS HUD METRIC
# ============================================================

The verified CombatAnalyzer `Last Fight DPS` value was promoted from the debug panel into the normal MetricHUD metric system.

## 1. Implementation


Added:

- `MetricID::LastFightDPS`
- `MetricRegistry` storage for Last Fight DPS
- `SetLastFightDPS()`
- `GetLastFightDPS()`
- `Last Fight DPS` metric registration
- `Show Last Fight DPS` Options checkbox
- Continuous registry update from `CombatAnalyzer::GetLastFightDPS()`

The metric is disabled by default so existing HUD layouts are not changed automatically.

The normal HUD uses `MetricFormat::Integer`.

## 2. Runtime Verification


Initial state:

- Last Fight DPS displayed `0` before a completed encounter.

Completed fight test:

Combat Analyzer:

- Last Fight Time: 28.23 seconds
- Last Fight DPS: 3744.6
- Last Arc StateChange: 2

HUD:

- DPS: 0
- Last Fight DPS: 3745

The HUD integer value correctly rounded the stored analyzer value.

## 3. Previous-Fight Retention Test


During the next encounter:

- Live DPS updated normally.
- Last Fight DPS remained at 3745 from the previous completed encounter.
- Last Arc StateChange changed to 1.

This confirmed that starting a new combat session does not erase the previous completed-fight result.

## 4. Rollover Test


After completing the second encounter:

Combat Analyzer:

- Last Fight Time: 23.78 seconds
- Last Fight DPS: 2755.9
- Last Arc StateChange: 2

HUD:

- DPS: 0
- Last Fight DPS: 2756

The previous 3745 result was correctly replaced by the newest completed fight.

## 5. Result


- Options checkbox: PASS
- Generic HUD rendering: PASS
- Initial zero state: PASS
- Completed-fight capture: PASS
- Previous-fight retention during new combat: PASS
- New-fight rollover: PASS
- Live DPS independence: PASS
- Integer formatting / rounding: PASS
- Runtime stability: PASS

Status: LAST FIGHT DPS HUD METRIC COMPLETE

# ============================================================
# END CHECKPOINT 9: LAST FIGHT DPS HUD METRIC
# ============================================================



---

# ============================================================
# CHECKPOINT 10: LAST FIGHT TIME HUD METRIC
# ============================================================

**Status:** COMPLETE  
**Build:** PASS  
**Runtime:** PASS

---

## 1. Purpose

The verified CombatAnalyzer `Last Fight Time` value was promoted from the debug panel into the normal MetricHUD metric system.

The goal was to preserve and display the duration of the most recently completed combat encounter alongside Last Fight DPS.

---

## 2. Implementation

Added:

- `MetricID::LastFightTime`
- `MetricRegistry` storage for Last Fight Time
- `SetLastFightTime()`
- `GetLastFightTime()`
- `Last Fight Time` metric registration
- `Show Last Fight Time` Options checkbox
- Continuous registry update from `CombatAnalyzer::GetLastFightDurationSeconds()`

The metric is disabled by default so existing HUD layouts are not changed automatically.

The normal HUD uses:

`MetricFormat::Time`

---

## 3. Registration Issue Found During Testing

The first runtime build did not display the `Show Last Fight Time` checkbox.

Source inspection showed that `MetricRegistry::Initialize()` accidentally contained two identical `LastFightDPS` registration blocks.

As a result:

`metricRegistry.GetMetric(MetricID::LastFightTime)`

returned `nullptr`, preventing the Last Fight Time checkbox from rendering.

The duplicate `LastFightDPS` registration was replaced with the intended:

`MetricID::LastFightTime`

registration.

The project was rebuilt successfully after the correction.

---

## 4. Options and HUD Verification

After the registration fix:

- `Show Last Fight Time` appeared in MetricHUD Options.
- The metric rendered successfully through the generic HUD system.
- The initial value displayed as `00:00`.

Status: PASS

---

## 5. Completed-Fight Runtime Test

A normal Standard Kitty Golem fight was completed and the golem was removed to trigger ArcDPS EXITCOMBAT.

### Combat Analyzer

- Last Arc StateChange: 2
- Last Fight Time: 36.88 seconds
- Last Fight DPS: 3848.3

### Normal HUD

- DPS: 0
- Last Fight DPS: 3848
- Last Fight Time: 00:36

The normal HUD correctly formatted the stored `36.88` second encounter duration using `MetricFormat::Time`.

Live DPS independently returned to `0` after combat.

---

## 6. Final Verification

- Metric ID: PASS
- Registry storage: PASS
- Setter / getter: PASS
- Metric registration: PASS
- Options checkbox: PASS
- Generic HUD rendering: PASS
- Initial zero state: PASS
- Completed-fight capture: PASS
- Time formatting: PASS
- Last Fight DPS compatibility: PASS
- Live DPS independence: PASS
- Runtime stability: PASS

---

## 7. Final Status

**LAST FIGHT TIME HUD METRIC: COMPLETE**

The normal MetricHUD HUD can now retain both the DPS and duration of the most recently completed fight.

# ============================================================
# END CHECKPOINT 10: LAST FIGHT TIME HUD METRIC
# ============================================================


---

# ============================================================
# CHECKPOINT 11: LAST FIGHT DAMAGE HUD METRIC
# ============================================================

**Status:** COMPLETE  
**Build:** PASS  
**Runtime:** PASS

---

## 1. Purpose

The verified CombatAnalyzer `Last Fight Damage` value was promoted from the debug panel into the normal MetricHUD metric system.

The goal was to retain and display the total damage dealt during the most recently completed combat encounter.

The stored value includes both:

- Direct / strike damage
- Buff / condition damage

---

## 2. Implementation

Added:

- `MetricID::LastFightDamage`
- `MetricRegistry` storage for Last Fight Damage
- `SetLastFightDamage()`
- `GetLastFightDamage()`
- `Last Fight Damage` metric registration
- `Show Last Fight Damage` Options checkbox
- Continuous registry update from `CombatAnalyzer::GetLastFightDamage()`

The metric is disabled by default so existing HUD layouts are not changed automatically.

The normal HUD uses:

`MetricFormat::Integer`

---

## 3. Options and HUD Verification

Runtime testing confirmed:

- `Show Last Fight Damage` appeared in MetricHUD Options.
- Enabling the option caused the metric to appear in the normal HUD.
- Before a completed encounter, the value displayed as `0`.

Status: PASS

---

## 4. Real-World Completed-Fight Test

A long open-world combat encounter was used for final runtime verification.

This provided a stronger validation than a simple training-golem test because the encounter contained both direct damage and buff / condition damage over an extended combat window.

### Combat Analyzer

- Last Arc StateChange: 2
- Direct Damage: 676123
- Buff Damage: 7757
- Last Fight Damage: 683880
- Last Fight Time: 346.60 seconds
- Last Fight DPS: 1973.1

The stored damage total was exactly:

`676123 + 7757 = 683880`

### Normal HUD

- Last Fight Damage: 683880

The normal HUD value exactly matched the CombatAnalyzer's stored completed-fight damage.

---

## 5. Damage Composition Verification

The test confirms that Last Fight Damage is not storing direct damage alone.

It correctly includes:

`Direct Damage + Buff / Condition Damage`

Verified example:

`676123 + 7757 = 683880`

CombatAnalyzer Last Fight Damage:

`683880`

Normal HUD Last Fight Damage:

`683880`

Status: PASS

---

## 6. Final Verification

- Metric ID: PASS
- Registry storage: PASS
- Setter / getter: PASS
- Metric registration: PASS
- Options checkbox: PASS
- Generic HUD rendering: PASS
- Initial zero state: PASS
- Completed-fight capture: PASS
- Direct damage inclusion: PASS
- Buff / condition damage inclusion: PASS
- CombatAnalyzer value match: PASS
- Normal HUD value match: PASS
- ArcDPS EXITCOMBAT capture: PASS
- Long-duration open-world encounter: PASS
- Runtime stability: PASS

---

## 7. Final Status

**LAST FIGHT DAMAGE HUD METRIC: COMPLETE**

MetricHUD can now retain and display all three core summary values from the most recently completed encounter:

- Last Fight DPS
- Last Fight Time
- Last Fight Damage

# ============================================================
# END CHECKPOINT 11: LAST FIGHT DAMAGE HUD METRIC
# ============================================================




---

# ============================================================
# CHECKPOINT 12: CURRENT FIGHT DAMAGE HUD METRIC
# ============================================================

**Status:** COMPLETE  
**Build:** PASS  
**Runtime:** PASS

---

## 1. Purpose

A live `Damage` metric was added to the normal MetricHUD HUD.

The goal is to show the total damage accumulated during the current combat encounter while preserving the existing completed-fight summary metrics separately.

The live value uses the already-verified CombatAnalyzer damage totals:

`Direct Damage + Buff / Condition Damage`

No CombatAnalyzer damage-accounting logic was changed.

---

## 2. Implementation

Added:

- `MetricID::Damage`
- `MetricRegistry` storage for current Damage
- `SetDamage()`
- `GetDamage()`
- `Damage` metric registration
- `Show Damage` Options checkbox
- Live registry update using CombatAnalyzer direct + buff damage

During combat:

`Damage = Total Direct Damage + Total Buff Damage`

Outside combat:

`Damage = 0`

The metric is disabled by default.

The normal HUD uses:

`MetricFormat::Integer`

---

## 3. Options and HUD Verification

Runtime testing confirmed:

- `Show Damage` appeared in MetricHUD Options.
- Enabling it caused `Damage` to appear in the normal HUD.
- Outside combat, the initial value displayed as `0`.

Status: PASS

---

## 4. Live Combat Verification

A Standard Kitty Golem encounter was used to verify the live value while combat was active.

Observed CombatAnalyzer values:

- Direct Damage: 86779
- Buff Damage: 5905
- Analyzer DPS: 4213.3
- Last Arc StateChange: 1

Calculated total:

`86779 + 5905 = 92684`

Normal HUD:

- Damage: 92684
- DPS: 4213

The HUD Damage value exactly matched the CombatAnalyzer direct + buff total.

Status: PASS

---

## 5. Post-Combat Reset Verification

The same encounter was allowed to continue and was then ended by removing the golem.

After ArcDPS EXITCOMBAT:

### Normal HUD

- DPS: 0
- Damage: 0

### Combat Analyzer

- Last Arc StateChange: 2
- Direct Damage: 212122
- Buff Damage: 7779
- Last Fight Damage: 219901
- Last Fight Time: 74.56 seconds
- Last Fight DPS: 2949.3

The completed total was:

`212122 + 7779 = 219901`

This confirmed that Current Fight Damage resets independently after combat while the completed-fight summary remains preserved.

---

## 6. Verified Combat Metric Lifecycle

During combat:

- DPS = current live DPS
- Damage = current accumulated damage

After combat:

- DPS = 0
- Damage = 0
- Last Fight DPS = completed encounter DPS
- Last Fight Time = completed encounter duration
- Last Fight Damage = completed encounter damage

This separation allows MetricHUD to expose both live combat information and completed-fight summaries without mixing the two states.

---

## 7. Final Verification

- Metric ID: PASS
- Registry storage: PASS
- Setter / getter: PASS
- Metric registration: PASS
- Options checkbox: PASS
- Generic HUD rendering: PASS
- Initial zero state: PASS
- Live direct damage inclusion: PASS
- Live buff / condition damage inclusion: PASS
- Live total exact match: PASS
- Live DPS compatibility: PASS
- Post-combat Damage reset: PASS
- Post-combat DPS reset: PASS
- Last Fight Damage preservation: PASS
- Last Fight Time preservation: PASS
- Last Fight DPS preservation: PASS
- ArcDPS EXITCOMBAT behavior: PASS
- Runtime stability: PASS

---

## 8. Final Status

**CURRENT FIGHT DAMAGE HUD METRIC: COMPLETE**

MetricHUD now exposes a complete basic combat-summary set:

### Live

- DPS
- Damage
- Combat Time

### Previous Completed Fight

- Last Fight DPS
- Last Fight Time
- Last Fight Damage

# ============================================================
# END CHECKPOINT 12: CURRENT FIGHT DAMAGE HUD METRIC
# ============================================================




# ============================================================
# CHECKPOINT 13: DOWNED COUNT
# ============================================================

## 13.1 Goal

Add a live HUD metric that tracks how many times the player enters the Downed state during the current combat encounter.

## 13.2 ArcDPS Down-State Investigation

Initial testing attempted to detect the ArcDPS down-state event through:

EV_ARCDPS_COMBATEVENT_LOCAL_RAW

ArcDPS state-change value 5 was monitored before all existing source and ownership filtering.

Results:

- Player visibly entered the Downed state.
- LOCAL_RAW did not produce state-change 5.
- A persistent raw state-5 counter remained at 0.
- Repeating the test while in a squad produced the same result through LOCAL_RAW.

A separate diagnostic callback was then created for:

EV_ARCDPS_COMBATEVENT_SQUAD_RAW

Results:

- SQUAD_RAW produced state-change 5 when the player entered the Downed state.
- A self-only test using combatData->src->IsSelf correctly identified the player's own down-state event.
- Other squad state-change traffic did not affect the self-only counter.

## 13.3 Reliability Testing

The self-only SQUAD_RAW down-state event was tested across multiple downs.

Results:

- First down -> count 1
- Second down in the same fight -> count 2
- No duplicate down events were observed.
- Starting a new combat encounter reset the analyzer Downed Count to 0.

This confirmed one self state-change 5 event per observed player down.

## 13.4 CombatAnalyzer Integration

CombatAnalyzer now owns the current-fight Downed Count.

Added:

- downedCount storage
- IncrementDownedCount()
- GetDownedCount()

downedCount is initialized to 0 and reset by ResetSession() at the beginning of each combat encounter.

The SQUAD_RAW callback increments CombatAnalyzer only when:

- IsStatechange == 5
- combatData->src is valid
- combatData->src->IsSelf != 0

Temporary diagnostic counters were removed after verification.

## 13.5 HUD Metric Integration

Added:

MetricID::DownedCount

Metric name:

Downed Count

Format:

Integer

Default visibility:

Disabled

The live HUD receives the current analyzer Downed Count while in combat and displays 0 outside combat.

A new option was added:

Show Downed Count

## 13.6 Runtime Verification

Verified in Guild Wars 2:

- Downed Count checkbox appears correctly.
- Outside combat -> Downed Count: 0
- Start of fight -> Downed Count: 0
- First player down -> Downed Count: 1
- Second player down in the same fight -> Downed Count: 2
- New combat encounter -> Downed Count resets to 0
- Existing MetricHUD combat metrics continued functioning normally.
- Final Release x64 rebuild succeeded.

## 13.7 Known Limitation

Realtime down-state detection was verified through SQUAD_RAW while the player was in a squad.

LOCAL_RAW did not provide the state-change 5 event during the solo or squad tests.

Therefore the current Downed Count implementation should be considered squad-dependent. Solo/open-world down detection without squad data is not currently supported by this event path.

# ============================================================
# END CHECKPOINT 13
# ============================================================




# ============================================================
# CHECKPOINT 14: DEATH COUNT
# ============================================================

## 14.1 Goal

Add a live Death Count metric that tracks how many times the player fully dies during the current combat encounter.

## 14.2 ArcDPS Death-State Integration

Death detection was added to the existing:

EV_ARCDPS_COMBATEVENT_SQUAD_RAW

callback that had already been verified for player Downed-state detection.

The self-only death condition is:

- IsStatechange == 4
- combatData->src is valid
- combatData->src->IsSelf != 0

When those conditions are met:

CombatAnalyzer::IncrementDeathCount()

is called.

The existing Downed Count path using state-change 5 remains unchanged.

## 14.3 CombatAnalyzer Integration

CombatAnalyzer now owns the current-fight Death Count.

Added:

- deathCount storage
- IncrementDeathCount()
- GetDeathCount()

deathCount is initialized to 0 and reset by ResetSession() when a new combat encounter begins.

## 14.4 HUD Metric Integration

Added:

MetricID::DeathCount

Metric name:

Death Count

Format:

Integer

Default visibility:

Disabled

A new option was added:

Show Death Count

Death Count is fed from CombatAnalyzer into MetricRegistry.

## 14.5 Encounter Result Retention

Initial testing showed that both Downed Count and Death Count were being forced back to 0 immediately when Mumble reported that combat had ended.

This prevented the player from seeing the final encounter values after dying.

The HUD update logic was changed so that:

- DPS resets to 0 outside combat.
- Damage resets to 0 outside combat.
- Downed Count retains the analyzer value after combat.
- Death Count retains the analyzer value after combat.
- Downed Count and Death Count reset when the next ArcDPS combat session begins through CombatAnalyzer::ResetSession().

This allows the completed fight's down/death result to remain visible until the next encounter starts.

## 14.6 Runtime Verification

Verified in Guild Wars 2:

- Show Death Count checkbox appears correctly.
- Outside combat before an encounter -> Death Count: 0
- Player enters Downed state -> Downed Count: 1, Death Count: 0
- Player becomes fully defeated -> Downed Count: 1, Death Count: 1
- Analyzer confirmed Downed Count: 1 and Death Count: 1 after full defeat.
- Completed-fight Downed Count and Death Count remain visible after combat ends.
- Beginning a new combat encounter resets both counts to 0.
- Existing DPS, Damage, combat timing, and other HUD metrics continued functioning normally.

## 14.7 Known Limitation

The current Death Count implementation uses EV_ARCDPS_COMBATEVENT_SQUAD_RAW.

Therefore the implemented realtime Death Count path is currently squad-dependent.

Death-state availability through LOCAL_RAW has not been separately verified and should not be assumed.

# ============================================================
# END CHECKPOINT 14
# ============================================================





# ============================================================
# CHECKPOINT 15: LAST FIGHT DOWNED / DEATH COUNTS
# ============================================================

## 15.1 Goal

Add completed-fight Downed Count and Death Count metrics so the player can review survival results from the previous combat encounter.

## 15.2 CombatAnalyzer Integration

Added completed-fight storage for:

- lastFightDownedCount
- lastFightDeathCount

Added getters:

- GetLastFightDownedCount()
- GetLastFightDeathCount()

CaptureLastFight() now stores the current fight's Downed Count and Death Count alongside Last Fight Damage, Time, and DPS.

The completed-fight values are not cleared by ResetSession(), allowing them to remain available while the next fight begins.

## 15.3 Metric Registry Integration

Added:

- MetricID::LastFightDownedCount
- MetricID::LastFightDeathCount

HUD labels:

- Last Fight Downs
- Last Fight Deaths

Both use Integer formatting and are disabled by default.

Added matching MetricRegistry storage, setters, and getters.

## 15.4 HUD Options

Added:

- Show Last Fight Downs
- Show Last Fight Deaths

Both metrics can be independently enabled from the MetricHUD options panel.

## 15.5 ArcDPS Event Ordering Issue

Initial runtime testing produced:

Downed Count: 1
Death Count: 1
Last Fight Downs: 1
Last Fight Deaths: 0

The underlying Death Count was correct, but the completed-fight Death Count was captured too early.

Testing showed that LOCAL_RAW EXITCOMBAT could trigger CaptureLastFight() before the SQUAD_RAW death state-change event arrived.

This created an event-ordering race:

1. EXITCOMBAT captured the fight while deathCount was still 0.
2. SQUAD_RAW state-change 4 then incremented deathCount to 1.

A RefreshLastFightSurvivalCounts() method was added to CombatAnalyzer.

When a self state-change 4 death event arrives after arcPlayerInCombat has already become false, the completed-fight Downed and Death counts are refreshed from the current analyzer counters.

This supports either callback ordering without changing the already verified damage/timing capture path.

## 15.6 Runtime Verification

Verified in Guild Wars 2:

Completed fight with one down and one death:

- Downed Count: 1
- Death Count: 1
- Last Fight Downs: 1
- Last Fight Deaths: 1

After starting a new combat encounter:

- Downed Count reset to 0.
- Death Count reset to 0.
- Last Fight Downs remained 1.
- Last Fight Deaths remained 1.

The completed-fight values therefore remain available while the live counters correctly reset for the new encounter.

Existing DPS, Damage, combat timing, Last Fight DPS, Last Fight Time, and Last Fight Damage behavior remained functional.

## 15.7 Known Limitation

The underlying realtime Downed and Death detection currently uses:

EV_ARCDPS_COMBATEVENT_SQUAD_RAW

Therefore Last Fight Downs and Last Fight Deaths inherit the same current squad-dependent limitation as the live Downed Count and Death Count metrics.

# ============================================================
# END CHECKPOINT 15
# ============================================================