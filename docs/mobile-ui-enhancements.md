# Wesnoth Mobile — UI Enhancement Project

**Branch:** `mobile-ui-enhancements` (created from `master` on 2026-10-28)  
**Status:** Requirements and implementation planning; branch creation alone does not implement the redesign.  
**Scope:** Mobile interface, navigation, ergonomics, screen adaptation, and UI asset handling. The future 2.5D map renderer is a separate phase.

## 1. Project objective

Replace the desktop-oriented Battle for Wesnoth interface with a coherent, touch-first, resolution-independent interface for Android phones, tablets, and foldables. Preserve the underlying game rules, campaigns, content and save-game semantics wherever possible. The interface must adapt smoothly rather than being designed around a fixed handset resolution. The new UI is a **prerequisite** for the separate 2.5D visual upgrade.

## 2. Confirmed product decisions

- Create a dedicated development branch named `mobile-ui-enhancements`, based on `master`; leave `master` intact until changes are reviewed and merged.
- Make layout adaptive to differing physical sizes, logical resolutions, aspect ratios, and density factors.
- Include foldable devices, specifically Samsung Galaxy Z Fold-style folded and unfolded configurations.
- Scale the map display to the available viewport instead of assuming a fixed desktop-sized play area.
- Size and place buttons and panels proportionally to available UI space, with minimum usable touch-target sizes. Avoid using screen-percentage scaling alone when that would produce tiny or oversized controls.
- Consider higher-resolution source artwork for interface graphics so it can be resampled down cleanly; some existing UI assets may need at least twice their source resolution.
- Replace **at least twenty** basic screens; other screens must follow shared adaptive rules rather than individual pixel-level patches.
- Treat language-specific Wesnoth logos in `images/misc/l10n` as localized logo substitutes; they are not, by themselves, a set of independently redesigned screens. Plan new translated logo artwork separately, retaining accurate language and branding.
- Keep this UI work separate from terrain meshes, height editing, normal mapping and tessellation.

## 3. Design principles

**One adaptive UI system:** shared tokens for spacing, type, control dimensions, safe areas, panels and images. **Touch first:** large targets, deliberate gesture ownership, no reliance on mouse hover or right-click. **Game first:** playfield remains prominent and menus may collapse or overlay without obscuring the current action. **Accessible and legible:** reliable contrast, scalable typography, predictable back behavior, clear selected/disabled states. **Content preservation:** respect campaign dialogs, large translated strings, portrait artwork, tutorials and user-generated content.

## 4. Display adaptation

Support portrait and landscape where gameplay and the platform permit; landscape can be the preferred play mode without hardcoding all dialogs for it. Measure the actual usable viewport **after** status/navigation bars, cutouts, hinges and system insets. Derive the UI from logical dimensions and density, not raw bitmap pixel counts.

Use adaptive breakpoints based on available space (compact phone, wide phone, tablet/unfolded foldable), subject to device testing. Use constraints: maximum panel widths, minimum touch-target dimensions, flexible margins, scrolling where necessary, and font-size/asset-density variants. Let percentage-based dimensions guide flexible regions but clamp them to ergonomic limits. On foldables, rebuild layout safely when the window changes size or spans screens; keep gameplay state and active selections.

Map zoom and UI scale are separate controls. Resizing the window must not unintentionally change selected units, target hexes, or turn state. Account for on-screen keyboards and dialog safe areas.

## 5. Proposed screen inventory (audit required)

The following is a **working replacement checklist**, not a claim that these correspond one-for-one to currently separate source-code screens:

1. Splash/loading and branding
2. Main menu
3. Game mode / new game selection
4. Campaign browser
5. Campaign detail and difficulty
6. Multiplayer entry / service choice
7. Multiplayer lobby
8. Multiplayer game creation/configuration
9. Multiplayer game join / side selection
10. Load game / saved games
11. Save game / save confirmation
12. Preferences navigation
13. Video/display settings
14. Audio settings
15. Controls / input / accessibility settings
16. Add-ons browser
17. Add-on detail / install / update
18. Game setup / era / faction configuration
19. In-game HUD / turn controls
20. Unit information and abilities
21. Recruitment and recall
22. Combat/attack choice and preview
23. Scenario objectives / status
24. Campaign dialogue / portraits
25. In-game menu / pause / exit
26. Chat and message history
27. Help / encyclopedia
28. Victory / defeat / scenario transition

A source-code audit must determine which are actual scenes, dialogs, overlays or shared widgets, and mark each as replace, adapt or retain. Do not duplicate an existing generic UI system needlessly.

## 6. In-game HUD and controls

Prioritize map space. Provide touch-appropriate selection, movement, inspect/details, attack, undo, end turn and menu access. Distinguish a tap from a pan and a pan from a pinch; do not make movement confirmation accidental. Use context-sensitive controls only when discoverable and provide clear, interruptible interaction states. Panels should open as sheets, overlays or side panels according to viewport width, preserving essential map context where possible. Test long unit names, multiple abilities, large attack lists and right-to-left or expanded translations.

## 7. UI asset audit

Inventory existing icons, buttons, panels, backgrounds, frames, typography and logos with source path, actual dimensions, intended on-screen size, format, transparency, localization and likely scaling artifacts. Identify images that need 2x or higher-density redraws and verify image sharpness in realistic rendered views. Resample down rather than stretching small sources upward; preserve aspect ratios. Check memory costs before indiscriminately doubling large images.

Localized logos in `images/misc/l10n` should be handled by language and compared against updated branding. The previously discussed approach of composing multiple localized logos onto shared review sheets can be used for manual separation and correction; final game assets must remain correctly mapped to locales.

## 8. Implementation architecture (proposed, not yet verified against source)

1. Identify the existing GUI toolkit, window management and theme/style definitions in this repository.
2. Add a central viewport metrics and safe-area service, with a consistent logical coordinate system.
3. Add reusable layout primitives and tokens for touch controls, cards, lists, sheets and dialogs.
4. Make a small set of representative screens adaptive first (main menu, preferences, gameplay HUD, campaign browser), before mass conversion.
5. Build a common touch interaction layer for pan, pinch, tap, long press, cancellation and keyboard/back navigation.
6. Convert the remaining screens iteratively, reusing shared components and avoiding isolated per-device hacks.
7. Migrate assets, localizations and tests in parallel with screen conversion.
8. Test on actual compact phone and folded/unfolded layouts; verify no regressions on desktop dimensions if the same UI code is shared.

Do not commit to a specific framework, class, or file location until the repository's actual SDL/GUI/Android integration is inspected.

## 9. Test matrix and acceptance criteria

Test narrow and tall phones, common 16:9 and 19.5:9 variants, high-density Android displays, landscape/portrait transitions, small tablets, large tablets, and Z Fold-like cover and interior dimensions. Include Android font scaling, display scaling, navigation-bar modes, cutouts, and multiwindow/resizing if supported.

Acceptance requirements:
- No clipped critical controls, text, portraits or modal actions at supported sizes.
- Controls remain touchable, with usable hitboxes independent of rendered icon size.
- No required hover, mouse wheel or right-click interactions.
- Map pans and zooms smoothly; interface dimensions do not track camera zoom.
- Scenario play remains functional: select unit, move, attack, recruit/recall, end turn, inspect objectives, save/load.
- Dialogs scroll where needed; focus/back/cancel recover predictably.
- Localized text does not overlap controls; fallback behavior works on lengthy strings.
- Fold/unfold or resize does not lose state or corrupt layout.
- UI sources and assets remain manageable in memory and render crisply.

## 10. Dependency on future 2.5D terrain work

**Not part of this branch:** the planned 2.5D display converts existing hex maps into 3D surfaces, initially with six triangles per hex. Later options include independently editable vertex heights, normal maps for surface detail without increasing polygon counts, terrain sculpting workflows in Mudbox or ZBrush, and possible tessellation. The UI should avoid coupling screen geometry to today's flat renderer so these upgrades can replace the map presentation without another interface rewrite. This is a dependency boundary, not a 2.5D implementation plan or promise that terrain tools are integrated.

## 11. Suggested implementation milestones

**M0 — Audit:** enumerate actual screens, UI code, current assets and localization paths; record screenshots and failure modes at mobile sizes.  
**M1 — Foundations:** responsive viewport metrics, scaling, safe areas, shared styles, input conventions.  
**M2 — Core navigation:** main menu, campaign and save/load flows, preferences.  
**M3 — Gameplay:** HUD, contextual commands, unit/combat/recruitment interactions.  
**M4 — Remaining interfaces:** multiplayer, add-ons, help, dialogs and less common flows.  
**M5 — Artwork/localization:** high-resolution assets, localized logos, translations.  
**M6 — Validation:** device matrix, performance/memory profiling, accessibility and regression tests.

## 12. Open technical questions

Which existing UI screens already have mobile-specific handling? Which toolkit constructs are responsive versus hard-coded? Which graphics are texture-bound or memory-sensitive? What orientation modes are actually enabled in the Android build? Which multiplayer/add-on screens are implemented and supported? Which Android window insets and fold posture signals are available through the present port? These require source inspection before code changes.

## 13. Branch workflow

Commit UI-related changes and this documentation on `mobile-ui-enhancements`. Keep unrelated portrait/campaign content replacements and the 2.5D renderer out of this branch. Use small reviewable commits and validate the core campaign loop before merging into `master`. This document is a living design specification; update decisions and checklists as the implementation develops.

## 14. Decision and progress log

- **2026-10-28:** UI conversion identified as a prerequisite to the 2.5D version.
- **2026-10-28:** Adaptive screens, controls and map display confirmed as priorities, including Z Fold support.
- **2026-10-28:** At least twenty screens flagged for redesign, with remaining screens governed by systemic rules.
- **2026-10-28:** Dedicated branch `mobile-ui-enhancements` created from `master`.
- **2026-10-28:** This planning document added; no application code changes are claimed here.
