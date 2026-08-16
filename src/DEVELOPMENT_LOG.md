EVTC Offline Combat Analysis

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

EVTCAnalyzer

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

EVTC File Loading

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

EVTC Encounter Identification

The parser successfully reads the encounter identifier from the EVTC header.

Verified example:

EVTC Encounter ID: 16199

This establishes that MetricHUD can identify the encounter represented by the EVTC file.

Status:

PASS

EVTC Agent Table Parsing

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

EVTC Skill Table Parsing

The EVTC skill table is now parsed.

Verified test file:

EVTC Skill Count: 125

This provides the mapping required to convert EVTC SkillIDs into readable Guild Wars 2 skill names.

This is separate from the live ArcDPS callback skill-name mapping and allows saved combat logs to be analyzed independently.

Status:

PASS

EVTC Combat Event Extraction

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

EVTC Damage Event Identification

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

EVTC Damage Source Resolution

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

Dynamic Player Identification

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

EVTC Player Direct Damage

After resolving the player dynamically, MetricHUD filtered damage events to the player's outgoing direct damage.

Verified test result:

Player Direct Damage: 27551
Player Direct Damage Events: 15

The values remained identical after replacing the temporary hard-coded player address with dynamic player identification.

This confirmed that dynamic player resolution was functioning correctly.

Status:

PASS

Player Damage Timestamp Tracking

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

First EVTC-Derived Player DPS

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

Important EVTC DPS Limitation

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

EVTC Debug Interface

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

Current Dual Combat-Analysis Architecture

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

Major Technical Milestone

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

Current Stable Checkpoint — EVTC Analyzer

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

Resume Here — EVTC DPS Refinement

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

Latest Verified State

MetricHUD now has a working end-to-end EVTC parsing foundation.

A real ArcDPS EVTC log has been successfully loaded, its agent/skill/combat-event structures parsed, the player's character dynamically identified, outgoing direct damage isolated, the first/last player-damage timestamps measured, and an independent player DPS value calculated from those records.

The latest verified test produced:

27551 player direct damage
15 player direct-damage events
8957 ms damage interval
3075.9 EVTC-derived DPS

This is the current recovery point if development context is lost.

Safe EVTC checkpoint reached.

EVTC Damage Model Investigation

Condition / Buff Damage Path Verified

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

Verified EVTC Damage Fields

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

Important Representation Difference

The parsed EVTC log currently exposes condition damage through buffDmg as positive values.

Earlier live ArcDPS callback inspection showed CombatRecord::buffDamage values using a negative representation.

These two data paths must not automatically be assumed to use identical sign conventions.

MetricHUD will preserve this distinction until both representations have been fully validated.

Current Damage Model

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

Latest Recovery Point

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





Combined Direct + Condition Damage Checkpoint

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

---

# Owned-Agent Damage and Live Combat Analyzer Checkpoint

Development continued from the verified EVTC direct + condition damage model into player-owned agent attribution and the live CombatAnalyzer pipeline.

## EVTC Owned-Agent Attribution

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

The second condition allows damage generated by pets, minions, and other player-owned agents to be attributed back to the player.

Status:

PASS

EVTC Owned-Agent Damage

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

Combined Player + Owned EVTC DPS

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

Live CombatAnalyzer Owned-Agent Integration

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

Live Player Instance Identification

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

Live Pet-Only Validation

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

Live Buff / Condition Damage

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

Live DPS Comparison Testing

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

Live Combat Timing Diagnostics

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

Current Stable Checkpoint — Owned Agents and Live DPS

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

Resume Here — Live DPS Refinement

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





---

# Live ArcDPS Damage-Stream Investigation and Analyzer Validation

Development continued from the stable owned-agent + live buff-damage checkpoint to investigate the remaining small difference between MetricHUD's live DPS calculation and ArcDPS.

The purpose of this investigation was to determine whether MetricHUD was losing damage before events reached CombatAnalyzer, particularly condition damage generated by player-owned agents.

## Live Bleeding Damage Investigation

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

## LOCAL_RAW Bleeding Verification

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

## Ownership Filter Investigation

A temporary Rejected Bleeding Damage counter was added before the ownership-filter return.

The purpose was to determine whether valid pet Bleeding events were reaching the callback but being discarded because the source was not recognized as the player or a player-owned agent.

Controlled testing did not identify the missing damage as rejected Bleeding through this filter.

This eliminated the ownership-filter rejection theory as the primary explanation for the remaining DPS difference.

The previously implemented player-instance / SrcMasterInstanceID ownership logic remains the verified live ownership model.

Status:

PASS

## SQUAD_RAW Diagnostic Stream

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

## ArcDPS API Verification

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

## Diagnostic Cleanup

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

## Post-Cleanup Runtime Sanity Test

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

## Important Timing Observation

The final sanity test displayed:

First Damage Time: 263152112

Last Damage Time: 263170669

Analyzer Combat Time: 18.56

Analyzer DPS: 1186.2

The damage counters and per-skill attribution are internally consistent, but the remaining ArcDPS-versus-MetricHUD DPS discrepancy should still be treated as unresolved.

Do not introduce an arbitrary correction factor.

Future work should continue to verify exactly which timing window ArcDPS uses for its displayed DPS and whether additional combat-event inclusion rules affect the result.

## Current Live Damage Pipeline

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

## Current Stable Checkpoint — Live Analyzer Stream Investigation

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

## Resume Here — Live DPS Timing Refinement

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