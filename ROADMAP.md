# Essoth Roadmap

From the Wesnoth fork to **Essoth: Tactics of the Fallen Kingdom**, a phone-first, AAA-quality game, with the 2.5D map in the final stages.

Written 2026-10-26. Discussion and reasoning behind each item: `DEVELOPMENT_JOURNAL.md`. Every push: `DEVELOPER_LOG.md`.

**Order of work**

| # | Phase | Result |
|---|-------|--------|
| 0 | Foundation | The fork builds on the Oracle server and installs on the phone |
| 1 | Playable on a phone | Readable, touch-controlled, either orientation |
| 2 | Mobile interface | Hand-built phone screens and a new in-game layout |
| 3 | Art replacement | George's portraits and story art, in progress throughout |
| 4 | 2.5D art spec and new tiles/sprites | Art made once, ready for the 2.5D map |
| 5 | Battle scenes | Short animated attack scenes with portraits |
| 6 | 2.5D map | Tilted diorama map with standing cutouts, fog, sky and clouds |
| 7 | Playthrough and music | New score written for the moments it plays over |
| 8 | Essoth campaign | Original campaign: a little girl who becomes queen (position in order open) |
| 9 | Release | Store build, licensing and name checks |

Phase 3 runs alongside phases 1–2. Every phase ends with a build on the Oracle server and a test on the Samsung A26.

---

## Phase 0: Foundation

1. **Build the current fork on the Oracle server.**
   - Nothing since the Oct 19 refork has been compiled yet, including the animated startup logo (`InitActivity.java`, `activity_init.xml`).
2. **Install and test on the A26.**
   - Record the result.
3. **Write the build docs the README already links to.** They don't exist yet:
   - `BUILD_NOTES.md`: build-by-build results.
   - `docs/ANDROID_BUILD_GUIDE.md`: the Oracle build procedure.
   - `MOBILE_DEVELOPMENT_LOG.md`: or remove that link.
4. **Separate app identity.**
   - `packaging/android/app/build.gradle` still has `applicationId "org.wesnoth.Wesnoth"`, although the README says a separate ID was set.
   - A new ID lets Essoth install alongside official Wesnoth.
5. **Data packs.**
   - Stop any external asset downloads at first run, so everything ships inside the app.

**Done when:** the fork installs from an Oracle build and opens to the main menu.

---

## Phase 1: Playable on a phone

1. **Startup scale, like Krimble.**
   - On first launch, pick a UI scale and zoom from the screen's size and density, so the game is readable without screen magnification.
   - The scale stays adjustable in settings.
2. **Readability.**
   - Larger default fonts and a high-contrast option.
3. **Screen.**
   - Fullscreen, kept clear of the notch and system bars.
   - Rotation between landscape and portrait at any time.
4. **Touch controls.** The engine already handles finger events (`src/controller_base.cpp`, `src/events.cpp`); they need tuning:
   - Tap to select, tap a hex to move, tap an enemy to attack.
   - Long-press in place of right-click (context menu, unit details).
   - One-finger drag to scroll, pinch to zoom.
   - Confirm steps for moves and attacks where a mistap would hurt.
5. **Defaults.**
   - Turn off desktop-only behaviour such as hover tooltips and keyboard hints.

**Done when:** a full scenario can be played on the A26 by touch alone, in either orientation.

---

## Phase 2: Mobile interface

The approach is a hybrid:
- About 20 core screens are hand-designed.
- Rules handle the rest.
- Every layout targets a 1920×1080 landscape area, with a separate 1080×1920 portrait layout.

### 2a. Essoth GUI theme (the rules layer)
- New theme folder `data/gui/themes/essoth/`, built like the existing `celes` theme, which overrides selected pieces of the `default` theme.
- Widgets (`widgets/`): bigger touch targets (at least 48 dp), larger text, and spacing tuned for fingers.
- This alone improves all 109 dialogs in `data/gui/themes/default/dialogs/`.

### 2b. Hand-built screens (landscape and portrait each)
Dialogs choose a layout by window size with `[resolution]` blocks. Planned set:

1. Title screen
2. Campaign selection
3. Difficulty selection
4. Story screen (1920×1080 story art)
5. Character dialogue (large portraits)
6. Objectives
7. Recruit
8. Recall
9. Attack selection and prediction
10. Unit details
11. Unit list
12. Game menu
13. Load game
14. Save game
15. Preferences
16. Help
17. End of scenario and victory
18. Defeat
19. Confirmations
20. Map settings for skirmish

### 2c. In-game layout
- New in-game theme in `data/themes/` (alongside `default.cfg`) with landscape and portrait versions.
- Compact top bar.
- Unit info panel that can be hidden.
- Bottom action bar: end turn, undo, recruit, menu.
- Minimap on demand.

### 2d. Startup
- Animated logo (written, untested).
- Full-screen background art (3840×3840, key content in the centre ~1650 px).
- George's symphonic theme song at startup.
- New app icon (made in Krimble).
- Essoth logo.

**Done when:** every screen a player meets in a campaign looks designed for the phone in both orientations.

---

## Phase 3: Art replacement (George)

Runs in parallel from now on.

1. Campaign portraits: nearly done.
2. Core portraits: 287 in `data/core/images/portraits/`.
3. Story art: 142 images at 1920×1080.
4. Title and loading backgrounds.
5. Interface art for the Essoth theme.
6. Journey maps (`data/campaigns/<Campaign>/images/maps/`).
7. Campaign logos: one per campaign, used as `campaign_image` and on a new opening story screen that plays the campaign's theme song (`[part]` `music=`). The theme songs come with Phase 7.

**Rules:**
- Match each existing filename exactly.
- Filenames are case-sensitive on Android.
- Art is checked against upstream by content hash to catch anything still original.

---

## Phase 4: 2.5D art spec, then tiles and sprites

The spec is set before any tile or sprite is drawn, so nothing is made twice.

**Spec:**
- **Ground tiles:** drawn straight top-down, with edge transitions.
- **Standing cutouts:** units, trees, mountains, castles and villages drawn from the front, with the ground contact point at bottom-centre.
- **Shadows:** separate soft blobs, not painted in.
- **Team color:** keep the magenta palette so `~TC()` still recolors.
- **Resolution:** fixed, crisp at the closest zoom.
- **Style reference:** `images/maps/wesnoth_map_1920x1080.jpg`.

**Order:**
- Terrain tiles first, since the map needs them before anything else.
- Then unit sprites, one faction at a time.

---

## Phase 5: Battle scenes

- When an attack starts, a short scene plays in a panel over the map:
  - A backdrop from the terrain.
  - Both units enlarged.
  - The strikes acted out, using the results the game already rolled.
- Portraits of both fighters, with HP bars and hit chances.
- Display only. Rules, AI, saves and multiplayer are untouched.
- Off and skip settings.
- Art: about a dozen terrain backdrops. Units reuse their sprites.

---

## Phase 6: 2.5D map

### How it fits the engine
- The engine is on SDL3.
- `SDL_RenderGeometry` is already used in `src/draw.cpp` (`draw::smooth_shaded`).
- The map is drawn flat into the offscreen texture `front_` and copied to the screen in `display::expose()`. The tilt goes in at that copy.

### Rendering passes
1. **Ground, flat (existing code).** Terrain, grid, footsteps, reach shading, fog and shroud, arrows, and unit shadows are drawn into `front_`.
   - `front_` is drawn larger and sharper than the screen, because the near rows are enlarged.
2. **Tilt.** `front_` is drawn onto the hex mesh in perspective.
3. **Standing cutouts.** Units, halos, HP bars, projectiles and standing terrain.
   - Each is drawn upright, scaled to its row, with its base on its hex.
   - They're drawn back to front.

### Hex mesh
- 6 triangles per hex: a centre point plus 6 corners.
- In each 72×72 hex box:
  - Corners at (18,0), (54,0), (72,36), (54,72), (18,72), (0,36).
  - Centre at (36,36).
  - Columns are 54 px apart; odd columns are shifted down 36 px.
- Neighbouring hexes share corner points, so no cracks.
- Built from the map size, so every existing map works unchanged.
- About 1,800 triangles on screen.

### Build steps
1. **Projection module** `src/display_projection.cpp`.
   - Map ↔ screen math.
   - Tilt angle.
   - On/off setting.
2. **Tilted ground with the hex mesh.**
   - Units temporarily lie flat.
   - Touch picking: the tap is converted back to its flat-map position, then passed to `pixel_position_to_hex()`.
   - First visible test on the A26.
3. **Upright units and shadows** (Pass 3).
4. **Standing terrain.**
   - A new WML key in terrain graphics marks an image as a standing cutout.
   - Uses the Phase 4 art.
5. **Floating interface.** Damage numbers, the attack indicator, labels and move info are placed through the projection.
6. **Atmosphere and edges.**
   - Distance fog: mesh vertex colours fade the far rows.
   - Sky gradient tinted by time of day.
   - Drifting cloud layer with alpha, densest at the horizon.
   - Optional cloud shadows.
   - 2–3 scenery hex rings beyond the map border, continuing the edge terrain, with clouds over them. The play area is never covered.
7. **Classic flat view toggle.** The map editor stays flat.
8. **Performance on the A26.**
   - Batching and texture atlases.
   - Draw only what's on screen.
9. **Terrain height (if performance allows).**
   - Raise and lower mesh points by terrain type: mountains and hills up, water down.
   - Cutouts sit at their hex's height.
   - Touch picking searches along the ray.
   - Gentle heights only.

### Camera
- Pan and zoom only, no rotation, because the sprites are drawn from one angle.

---

## Phase 7: Playthrough and music

- Full playthrough to hear where each track lands.
- New score replacing all 45 shared tracks (`data/core/music/`) and the campaign tracks.
- Each replacement is an OGG file with the same name.

---

## Phase 8: Essoth campaign

- A little girl who becomes queen. She must make decisions that lead her, step by step, toward good or evil. Some elements of *Princess Maker*.
- Position in the order is open (George, Oct 26).
- Upbringing screens between battles (Lua dialogs).
- Stats kept in campaign variables.
- A moral score from -100 to +100:
  - Good drifts her lawful (strong by day).
  - Evil drifts her chaotic (strong at night).
  - In between: neutral or liminal.
- Portraits, story art, color grading (`[color_adjust]`), the 2.5D sky and fog, and her music theme all shift with her path.
- Branching scenarios and multiple endings.

---

## Phase 9: Release

- Name **Essoth**. Subtitle: *Tactics of the Fallen Kingdom*.
- USPTO trademark search.
- IP lawyer review of the licensing for the new art and music.
- GPL compliance:
  - Source available (public repo).
  - Original credits kept (`data/core/about.cfg`), with George's added.
  - Changed files marked as changed.
- Store listing: the fastlane metadata in `fastlane/metadata/android/` updated with Essoth text and screenshots.
- Signed release build from the Oracle server.
