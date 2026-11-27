# Chaos Jump

Standalone Geode mod targeting Geometry Dash 2.2081, with Android arm64 CI.

## Implemented in source
- Hooks the start of a vanilla jump and applies a randomized vertical-force multiplier.
- Geode settings: enable/disable, intensity, minimum/maximum force, super-jump chance and force.
- Uses the game's own jump and gravity update; it does not replace collision handling.

## Planned / not yet implemented
- Custom editor trigger objects and their serialization
- Jump Force, Random Jump, Gravity Chaos and Chaos Reset trigger execution
- Custom settings popup and visual effects
- In-game Android verification

## Build
GitHub Actions builds an Android arm64 `.geode` artifact on pushes to `main` and via **Actions → Build Chaos Jump → Run workflow**.

## Status
The gameplay hook and settings are committed, but the build must pass CI and be tested in Geometry Dash before this is considered a working release.
