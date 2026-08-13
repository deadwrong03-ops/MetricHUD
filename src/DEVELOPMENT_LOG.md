# MetricHUD Development Log

This file records major development checkpoints, successful tests, known issues,
and the current development state of MetricHUD.

Its purpose is to make development recoverable if conversation history or other
development notes are lost.

---

# Current Development State

## Latest Stable Checkpoint

MetricHUD currently has a functioning Combat Analyzer and live DPS metric.

### Combat Analyzer

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

### DPS Metric

- Live DPS metric implemented
- DPS registered through MetricRegistry
- Show DPS option implemented
- DPS updates during combat
- DPS resets to 0 when combat ends
- New combat encounters begin with fresh analyzer data
- Tested successfully across multiple separate fights

### Mumble Link Metrics

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
- Metric visibility checkboxes work
- Map Name resolves correctly
- Character Name resolves correctly
- Map ID works
- Player Speed works

---

# Important Recovery Notes

If development context is lost:

1. Check this file first.
2. Check the latest Git commits.
3. Treat the repository code as the source of truth.
4. Do not rebuild features listed as completed here unless testing shows they are broken.
5. Resume development from the latest unfinished feature below.

---

# Next Development Work

Update this section whenever development stops.

Current next feature:

**To be determined from the current MetricHUD roadmap before additional coding begins.**

---

# Development Rule

After completing and successfully testing a meaningful feature:

1. Save all files.
2. Rebuild Release | x64.
3. Test in Guild Wars 2.
4. Commit the working checkpoint.
5. Push to GitHub.
6. Update this DEVELOPMENT_LOG.md with the new checkpoint.