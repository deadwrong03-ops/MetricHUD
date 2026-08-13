# MetricHUD Development Log

This file records major development checkpoints, successful tests, known issues,
and the current development state of MetricHUD.

Its purpose is to make development recoverable if conversation history or other
development notes are lost.

---

# Current Development State

## Latest Stable Checkpoint

MetricHUD currently has a functioning Combat Analyzer, live DPS metric,
per-encounter tracking, last-fight summary capture, and dynamic ArcDPS
skill-name resolution.

---

## Combat Analyzer

- Combat event reception working
- Combat record tracking working
- Damage event classification working
- Direct damage accumulation working
- Combat duration tracking working
- DPS calculation working
- Analyzer DPS successfully connected to MetricRegistry
- Combat sessions reset when a new fight begins
- Last-fight combat summary capture implemented
- Recent skill history tracking implemented
- Recent combat records tracking implemented
- Collapsible Combat Analyzer debug panel implemented
- Dynamic ArcDPS skill-name resolution implemented
- Skill IDs are automatically associated with ArcDPS-provided skill names
- Recent Skills debug display shows Skill Name + Skill ID
- Skill-name mappings persist across combat sessions
- No manually maintained skill-ID table required

---

## DPS Metric

- Live DPS metric implemented
- DPS registered through MetricRegistry
- Show DPS option implemented
- DPS updates during combat
- DPS resets to 0 when combat ends
- New combat encounters begin with fresh analyzer data
- Tested successfully across multiple separate fights

---

## Mumble Link Metrics

- Mumble Link integration working
- Map ID working
- Map Name working
- Character Name working
- Player Speed working

A previous regression caused Map Name and Character Name to display "Unknown".

This was fixed and verified in-game.

Relevant Git commit:

`Restore Mumble map and character metric updates`

Commit: `618caa1`

---

# Current HUD Metrics

- FPS
- Ping
- Combat Timer
- DPS
- Map Name
- Map ID
- Character Name
- Player Speed

---

# Skill Tracking

MetricHUD receives skill information through the ArcDPS combat event stream.

Current functionality:

- Skill IDs are captured from combat events
- Recent skill history is maintained
- ArcDPS-provided skill names are captured dynamically
- Skill IDs are associated with their actual ArcDPS skill names
- Debug display shows both skill name and skill ID
- Skill-name mappings remain available between combat encounters

Example verified output:

- Screech (2855)
- Life Rend (29442)
- Soul Eater (30539)
- Gravedigger (30163)

This replaced the temporary hardcoded skill-ID resolver.

The hardcoded approach was rejected because ArcDPS-reported IDs did not
reliably match the manually assumed IDs and would not scale across professions,
weapons, elite specializations, or future Guild Wars 2 changes.

---

# Last-Fight Tracking

MetricHUD captures information from the completed combat encounter before
starting a new combat session.

Currently captured:

- Last Fight Damage
- Last Fight Duration
- Last Fight DPS

This provides the foundation for future encounter summaries, self-comparison,
and Training Mode analysis.

---

# Testing Status

Latest in-game testing confirmed:

- MetricHUD loads successfully
- No crash during normal combat testing
- Combat Analyzer receives events
- Direct damage tracking works
- Combat duration works
- DPS works
- DPS resets after combat
- Analyzer resets at the beginning of a new encounter
- Second fight does not inherit first-fight analyzer data
- Last-fight summary capture works
- Metric visibility checkboxes work
- Map Name resolves correctly
- Character Name resolves correctly
- Map ID works
- Player Speed works
- Dynamic skill-name resolution verified in-game
- Multiple skills correctly display their ArcDPS-provided names
- Repeated skill IDs consistently resolve to the same skill name
- Recent Skills display shows Skill Name + Skill ID
- No crash during dynamic skill-name testing

---

# Important Recovery Notes

If development context is lost:

1. Check this file first.
2. Check the latest Git commits.
3. Treat the repository code as the source of truth.
4. Do not rebuild features listed as completed here unless testing shows they
   are broken.
5. Resume development from the latest unfinished feature below.

---

# Development Philosophy

MetricHUD should provide useful information without overwhelming the player.

Core principles:

- Precision over repetition
- Immediate useful feedback
- Stability before features
- Show the information the player wants and nothing they don't
- Training feedback should be constructive rather than judgmental
- Training should identify optimization opportunities rather than label player
  actions as wrong
- Player self-comparison is preferred over forcing elite benchmark comparison
- Rotation coaching should understand priorities and encounter phases rather
  than enforce one rigid sequence

---

# Planned Training System

The long-term Training Mode is intended to use MetricHUD's combat data to help
players understand and improve their own gameplay.

Planned concepts include:

- Golem practice analysis
- Guided Training Mode
- Independent Training Mode
- Benchmark Mode
- Rotation and priority analysis
- Phase-based rotation coaching
- Execute-phase guidance
- DPS coaching
- Player-selected DPS goals
- Build and gear analysis against the player's selected goal
- Boon uptime analysis
- Skill downtime analysis
- Burst-window analysis
- Consistency scoring
- Survivability/downed context
- Personal performance history
- Self-comparison between encounters
- Constructive optimization suggestions

---

# Planned MetricHUD Features

Future/non-training features include:

- Expanded customizable HUD
- Maximum visible metric limit to reduce clutter
- Additional combat metrics
- Downed count
- Death count
- Boon uptime
- Food timer
- Utility timer
- Squad statistics
- Encounter summary
- World boss logging (experimental)
- Jade Bot buff tracking (experimental)
- Additional MetricRegistry metrics
- Additional HUD customization
- Persistent window positioning and layout improvements

---

# Next Development Work

Update this section whenever development stops.

Current next feature:

**Combat skill usage statistics**

Goal:

Use the skill IDs and dynamically resolved ArcDPS skill names already captured
by CombatAnalyzer to calculate per-encounter skill usage counts.

This will become a foundation for:

- Skill frequency analysis
- Rotation history
- Priority analysis
- Skill downtime analysis
- Rotation consistency
- Training Mode coaching

---

# Development Procedure

After completing and successfully testing a meaningful feature:

1. Save All.
2. Rebuild using Release | x64.
3. Test one change at a time in Guild Wars 2.
4. Confirm the feature works without crashes or regressions.
5. Commit the working checkpoint.
6. Push to GitHub.
7. Update DEVELOPMENT_LOG.md with the new checkpoint.

Never move several untested changes forward at once.

---

# Latest Completed Development Milestone

**Dynamic ArcDPS Skill Name Resolution**

MetricHUD now receives the actual skill name supplied by ArcDPS alongside
combat events.

The Combat Analyzer dynamically associates each SkillID with its corresponding
skill name.

Verified in-game with multiple skills including:

- Screech
- Life Rend
- Soul Eater
- Gravedigger

This eliminates the need for a manually maintained Guild Wars 2 skill-ID
database for combat skill-name resolution.

Status:

**BUILT — TESTED — COMMITTED — PUSHED**