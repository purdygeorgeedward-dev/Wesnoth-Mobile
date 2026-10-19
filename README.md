<p align="center">
  <img src="doc/Images/wesnoth-mobile-logo.png" alt="Wesnoth Mobile fantasy crest and title logo" width="400">
</p>

# Wesnoth Mobile

**A phone-first Android adaptation of [The Battle for Wesnoth](https://www.wesnoth.org/).**

[![Development](https://img.shields.io/badge/status-active%20development-orange)](https://github.com/purdygeorgeedward-dev/Wesnoth-Mobile/tree/master)
[![Platform](https://img.shields.io/badge/platform-Android-3DDC84)](packaging/android)
[![License](https://img.shields.io/badge/code-GPL--2.0--or--later-blue)](COPYING)

Wesnoth Mobile aims to bring the depth of Wesnoth's turn-based fantasy strategy to modern Android phones **without treating a phone like a miniature desktop**. The project builds on Wesnoth's existing game engine and content while developing a touch-oriented, responsive interface designed for portrait and landscape play.

> **Development status:** This fork is a work in progress. The Android foundation has been modified, but the planned native interface redesign is not complete. No APK from the current source changes has yet been built and verified. Features described as *planned* below are not promises of current functionality.

## The game

[The Battle for Wesnoth](https://www.wesnoth.org/) is a free and open-source, turn-based tactical strategy game set in a richly developed fantasy world. Recruit armies, capture villages, manage gold and terrain advantages, advance veteran units, and fight through story-driven campaigns. The upstream game includes single-player adventures, multiplayer modes, user-created content, and extensive modding capabilities.

Wesnoth Mobile intends to preserve that strategic experience while making it comfortable to play on a touchscreen.

## Our direction

| Area | Goal | Status |
| --- | --- | --- |
| Android application identity | Install independently of the upstream Android app | Implemented in source; untested |
| Phone-friendly launcher | Responsive startup layout and larger settings touch target | Initial changes committed |
| Portrait and landscape | Support both device orientations | Orientation restrictions removed; native UI work pending |
| Touch controls | Tap selection, intuitive map panning, pinch zoom, and contextual actions | Planned |
| Responsive game interface | Rework menus, dialogs, information panels, and battlefield controls for small screens | Planned |
| Startup experience | Animated, skippable opening with a custom visual and musical identity | Planned |
| Original presentation | Develop distinctive interface styling and, later, original visual and musical assets | Planned |

The guiding principle is straightforward: **keep the strategy; rethink the interaction.**

## Development progress

Development takes place directly on [`master`](https://github.com/purdygeorgeedward-dev/Wesnoth-Mobile/tree/master). The upstream gameplay and content remain the starting point.

The first source changes establish a separate Android application ID, improve launcher sizing and accessibility, remove forced orientation restrictions, correct launch argument formatting, address first-run status-file initialization, and improve ZIP archive extraction safety. These changes still require compilation and device testing.

The Android source changes have not yet been compiled or tested on a device.

### Follow the work

- [Developer log](DEVELOPER_LOG.md) — ongoing engineering decisions, progress, and risks.
- [Development change log](MOBILE_DEVELOPMENT_LOG.md) — detailed changes and validation requirements.
- [Build notes](BUILD_NOTES.md) — build-by-build results and device observations once APKs are published.
- [GitHub commits](https://github.com/purdygeorgeedward-dev/Wesnoth-Mobile/commits/master) — the source history.

## Building from source

This repository contains Wesnoth's upstream C++ game engine and an Android packaging project under [`packaging/android/`](packaging/android/). Native dependencies, Android SDK/NDK tooling, and source submodules are required.

For the general engine requirements, see [INSTALL.md](INSTALL.md). Clone with submodules:

```bash
git clone --recurse-submodules https://github.com/purdygeorgeedward-dev/Wesnoth-Mobile.git
cd Wesnoth-Mobile
```

The Android Gradle project is in `packaging/android`. For the full dependency → C++ engine → APK/AAB procedure, including GitHub Actions and local Linux commands, see the [Android Build Guide](docs/ANDROID_BUILD_GUIDE.md). The instructions are source-verified but a successful fork build and device test have not yet been recorded. Check [BUILD_NOTES.md](BUILD_NOTES.md) for actual build results.

## Upstream and community

Wesnoth Mobile is an **independent fork**, not an official release of The Battle for Wesnoth. The original project and its contributors created the game on which this work depends.

For upstream gameplay documentation, campaigns, community content, and support:

- [Official Wesnoth website](https://www.wesnoth.org/)
- [Wesnoth wiki](https://wiki.wesnoth.org/)
- [Official forums](https://forums.wesnoth.org/)
- [Upstream source repository](https://github.com/wesnoth/wesnoth)
- [Wesnoth manual](https://wiki.wesnoth.org/WesnothManual)

## Licensing and attribution

Wesnoth is free and open-source software. The game's source code is licensed under the **GNU General Public License, version 2 or later**; see [COPYING](COPYING). Artwork, music, and other content may have different applicable licenses and attribution requirements. See the upstream [copyright and licensing information](https://wiki.wesnoth.org/Wesnoth:Copyrights) and individual file notices.

This fork retains the obligations and notices of the upstream project. New mobile-specific work must comply with the applicable licenses.

---

*Wesnoth Mobile — a familiar world, redesigned for your fingertips.*

