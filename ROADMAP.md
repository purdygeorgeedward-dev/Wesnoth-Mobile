# Wesnoth Mobile Roadmap

Plan for **Wesnoth Mobile**: a phone-first, polished Wesnoth, with the 2.5D map in the final stages.

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
| 8 | Release | Signed release build and store listing |

**Top priority (George, Oct 30): Phases 1–2, a new UI that scales cleanly to phones.** Phase 3 runs alongside phases 1–2. Every phase ends with a build on the Oracle server and a test on the Samsung A26.

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
   - A new ID lets Wesnoth Mobile install alongside official Wesnoth.
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

### Smooth camera (2D now, 2.5D later)

Rule: all camera state (position, zoom, and later tilt and rotation) lives in `display`. Input code only calls camera functions; nothing else touches `zoom_` or `viewport_origin_`. The 2.5D projection module (Phase 6) then replaces what sits behind those functions, and touch input needs no rewrite.

1. **Momentum panning.** Written 2026-11-01, not yet run.
   - `display::pan_sample / pan_release / pan_cancel / kinetic_pan_step`.
   - A finger flick keeps the map moving and slows it with friction (0.35 s time constant). A new touch or the map edge stops it.
   - Only when no unit is selected, same as plain drag.
2. **Smooth pinch zoom.** Next.
   - Track the two fingers from the raw finger events (SDL3 has no pinch event). Needs `src/events.cpp` and `src/controller_base.cpp`.
   - While pinching, the offscreen map texture `front_` is drawn scaled about the pinch centre when it is copied to the screen in `display::expose()`. GPU scaling, no re-render, so it follows the fingers at full frame rate.
   - When the preview scale drifts past about ±25%, or the fingers lift, commit: `set_zoom()` to the nearest hex size that is a multiple of 4, re-centred on the pinch centre, then redraw sharp. Range 16 to 288.
   - Double-tap on empty map: step between zoom levels.
3. **Pan and zoom share one view transform** (centre, scale) applied at the `expose()` copy.
4. **2.5D readiness.** That same copy is where Phase 6 draws `front_` onto the tilted hex mesh. The preview scale becomes the mesh scale, and the pan offset becomes the camera position. Add the tilt angle to the same transform later. Standing cutouts (units, trees) are drawn after it, as already planned.

---

## Phase 2: Mobile interface

The approach is a hybrid:
- About 20 core screens are hand-designed.
- Rules handle the rest.
- Every layout targets a 1920×1080 landscape area, with a separate 1080×1920 portrait layout.

### 2a. Wesnoth Mobile GUI theme (the rules layer)
- New theme folder `data/gui/themes/mobile/`, built like the existing `celes` theme, which overrides selected pieces of the `default` theme.
- Widgets (`widgets/`): bigger touch targets (at least 48 dp), larger text, and spacing tuned for fingers.
- This alone improves all 109 dialogs in `data/gui/themes/default/dialogs/`.
- Started 2026-10-31 (id `mobile`, shown as "Wesnoth Mobile"): theme registered; touch sizes for buttons, drop-downs, text boxes, sliders and scrollbars; Android starts in it. Android pixel scale done 2026-11-01 (short side ≥ 540 logical px, scale 2 on the A26).

### 2b. Hand-built screens (landscape and portrait each)
Dialogs choose a layout by window size with `[resolution]` blocks. Planned set:

1. Title screen: done 2026-11-01 (portrait and landscape)
2. Campaign selection: written 2026-11-01 (portrait and landscape; not yet built or run)
3. Difficulty selection: written 2026-11-08 (portrait and landscape; not yet built or run)
4. Story screen (1920×1080 story art): written 2026-11-08 (portrait and landscape; not yet built or run)
5. Character dialogue (large portraits): written 2026-11-08 (portrait and landscape, left/right/double; not yet built or run)
6. Objectives: written 2026-11-08 as the transient message dialog (portrait and landscape; not yet built or run)
7. Recruit: written 2026-11-08 as the shared units dialog (portrait and landscape; not yet built or run)
8. Recall: written 2026-11-08 as the shared units dialog (portrait and landscape; not yet built or run)
9. Attack selection and prediction: both written 2026-11-08 (portrait and landscape; not yet built or run)
10. Unit details: covered by the help browser (screen 16). The engine opens the help browser for a unit description (help::show_unit_description), so there is no separate dialog. Checked 2026-11-08.
11. Unit list: written 2026-11-08 as the shared units dialog (portrait and landscape; not yet built or run)
12. Game menu: not a GUI2 dialog. The in-game menu is built by the in-game theme, so it belongs to 2c below. Checked 2026-11-08.
13. Load game: written 2026-11-08 (portrait and landscape; not yet built or run)
14. Save game: written 2026-11-08 (portrait and landscape; not yet built or run)
15. Preferences: written 2026-11-08 (portrait and landscape; not yet built or run)
16. Help: written 2026-11-08 (portrait and landscape; not yet built or run)
17. End of scenario and victory: covered by the transient message dialog written for screen 6. The scenario end report is shown with show_transient_message (data/lua/carryover_gold.lua, show_popup_dialog). Checked 2026-11-08.
18. Defeat: covered by the transient message dialog written for screen 6, as for 17. Checked 2026-11-08.
19. Confirmations: written 2026-11-08 as the message dialog (portrait and landscape; not yet built or run)
20. Map settings for skirmish: written 2026-11-08 as the multiplayer create game dialog (portrait and landscape; not yet built or run)

Additional dialogs found by scanning all 98 default dialog files (2026-11-08). Beyond the 20, these appear in a normal campaign and have size caps larger than a phone screen:
- Written 2026-11-08 (not yet built or run): drop_down_list (drop-down menu popup, taller rows), unit_advance (level-up choice, portrait and landscape), edit_text, edit_label and prompt (placed at the top because of the on-screen keyboard), wml_error, language_selection, sp_options_configure (size caps changed to the screen size).
- Looked at, no change needed: surrender_quit, game_save_overwrite, game_delete (small prompts, centered, narrower than the screen).
- Written 2026-11-08 (second batch, not yet built or run): statistics_dialog and game_stats (full screen, the middle part scrolls), achievements_dialog, game_version (About), label_settings, theme_list and simple_item_selector (size caps changed to the screen size).
- Looked at, no change needed: end_credits, loading_screen and outro are already full screen ({GUI_WINDOW_FULLSCREEN}). Their art (loading screen logo and decoration) belongs to 2d.
- Tooltips (tooltip_floating, tooltip_large), checked 2026-11-08, no layout change written: the placement formulas already work from the screen size and the pointer position (they choose above, below or "too big" and keep the tooltip on screen), so they fit any size. What is not known without running it is whether a tap or long press on Android produces the pointer-hover that makes a tooltip appear; to be checked on the phone. Tooltip text for keyboard shortcuts will not mean much on a phone.
- Not planned unless they become part of the product: add-on dialogs (about 10), multiplayer (about 17), map editor (about 15), debug and log dialogs (about 8).
- Larger than all of these: the in-game layout (2c) and startup (2d).

### 2c. In-game layout
- Written 2026-11-08, not yet built or run: the Wesnoth Mobile in-game theme, `data/themes/mobile.cfg`, with landscape (1000x480 and up) and portrait (480x1000 and up) layouts. Android now starts with it (`prefs::theme()` in `src/preferences/preferences.cpp`); a phone that already saved a theme choice keeps it until changed in preferences. **Changed 2026-11-11:** Android now always uses the Wesnoth Mobile theme (forced in `gui2::switch_theme`, `src/gui/gui.cpp`); the saved choice is ignored there. Not yet built.
- Compact top bar: menu, turn, gold, villages, units, upkeep, income, battery and time of day (portrait leaves out upkeep, income and battery).
- Bottom bar: the selected unit's image, name, type, level, HP, XP and moves on the left, and Undo, Recruit, Actions and End Turn on the right. Portrait uses two bars: unit info, then the four buttons.
- Buttons are 96, 112 and 168 wide and 44 high (88 px on the A26), made from the stock large button (`images/buttons/mobile/`).
- Not done: minimap on demand (no minimap element in the theme yet), hiding the unit panel (a theme cannot toggle it; needs C++), replay controls and the countdown timer, terrain and position readouts, and final art for the bars (placeholder flat tiles in `data/core/images/themes/mobile/`).

### 2d. Startup
- Animated logo (written, untested).
- Title screen background: written 2026-11-08 from George's logo-free splash set (landscape 2400x1080, portrait 1080x2400, square 2400x2400), `data/core/images/maps/splash-landscape.webp`, `splash-portrait.webp`, `splash-square.webp` (byte-for-byte copies of George's own WebP files, not re-encoded); the title screen picks one by the window shape and scales it to cover the window without stretching, centered, so the extra spills off the edges (on the A26 about 15 logical pixels are cut off each side in landscape and each end in portrait). Not yet seen on a phone: the menu and the animated logo are drawn over the faces. The splash with the logo is kept for the Android start-up screen (not wired yet).
- George's symphonic theme song at startup.
- New app icon: written 2026-11-08 from George's transparent shield-and-swords logo (`images/logos/logo-bg.png`): legacy launcher icons in all five densities, a 432 px adaptive-icon foreground inside the 66% safe zone, a monochrome version for themed icons, and a dark blue adaptive background (was tan). Not yet seen on a phone.
- Wesnoth Mobile logo.

**Done when:** every screen a player meets in a campaign looks designed for the phone in both orientations.

---

## Phase 3: Art replacement (George)

Runs in parallel from now on.

1. Campaign portraits: nearly done.
2. Core portraits: 287 in `data/core/images/portraits/`.
3. Story art: 142 images at 1920×1080.
4. Title and loading backgrounds.
5. Interface art for the Wesnoth Mobile theme.
6. Journey maps (`data/campaigns/<Campaign>/images/maps/`).
7. Campaign logos: one per campaign, used as `campaign_image` and on a new opening story screen that plays the campaign's theme song (`[part]` `music=`). The theme songs come with Phase 7.

**Rules:**
- Match each existing filename exactly.
- Filenames are case-sensitive on Android.
- Art is checked against upstream by content hash to catch anything still original.
- High-resolution masters (3840 max). Smaller tiers are built from them: sharpen gently first, then halve, sizes tied to real device screens. The engine has no mipmaps, so the tiers are pre-built. (Decided 2026-11-11.)
- Every texture uses linear filtering, not nearest-neighbor (changed 2026-11-11; not yet compiled or run).

---

## Phase 4: 2.5D art spec, then tiles and sprites

The spec is set before any tile or sprite is drawn, so nothing is made twice.

**Spec:**
- **Ground tiles:** drawn straight top-down, with edge transitions.
- **Standing cutouts:** units, trees, mountains, castles and villages drawn from the front, with the ground contact point at bottom-centre.
- **Shadows:** separate soft blobs, not painted in.
- **Team color:** keep the magenta palette so `~TC()` still recolors.
- **Clouds (added 2026-11-01):** tileable cloud textures with alpha, 2 to 3 layers (high and thin, low and dense). One soft cloud-shadow mask for the ground. Painted to match the style reference, not photographic.
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
     - Drawn after the cutouts, over the far rows only, so distant units sit inside the haze. Never over the near rows.
     - Scrolls slower than the ground, which gives depth when the camera pans.
     - Cloud shadows darken the ground. With terrain normal maps this goes through the shader as a shadow mask. Without it, as vertex colour.
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

### Normal maps (investigated 2026-11-01)
- **Plain `SDL_RenderGeometry` can't do it.** It has no per-pixel lighting.
- **SDL 3.4 can.** Its custom GPU render state (`SDL_CreateGPURenderState`) takes our own fragment shader, extra texture samplers (the normal map) and uniforms (light direction and colour).
- **Not yet checked on our build:**
  - Which SDL version the Android build uses. `CMakeLists.txt` asks for SDL3 with no minimum version.
  - `src/sdl/window.cpp` creates the renderer with `SDL_CreateRendererWithProperties` and falls back to OpenGL, then software. The custom render state needs the SDL GPU renderer, so the renderer choice would have to change for the 2.5D map.
  - The shader has to ship precompiled (SPIR-V for Vulkan on Android).
- **The 6-triangle hex mesh is about perspective, not lighting.** Lighting happens per pixel in the shader, so the mesh doesn't change.
- **Cost on the ground:** `front_` is built from many layered tiles and blended transitions. Normal-mapped ground needs a matching normal buffer drawn alongside it, which roughly doubles the ground drawing, and every tile and transition needs a normal map (Phase 4 art).
- **Cutouts:** no normal maps (decided). They can still pick up simple tinting and fog by row.
- **Cheaper fallbacks:**
  - Per-vertex light colour on the mesh: free, gives time-of-day and distance shading.
  - Lit tile variants baked offline from the normal maps: no runtime shader.
- **Decided 2026-11-01 (George):** terrain gets normal maps. Sprites and cutouts do not. Phase 4 art spec: each ground tile and transition piece gets a normal map file (for example `name_n.png`). Units, trees, mountains and castles stay plain.

---

## Phase 7: Playthrough and music

- Full playthrough to hear where each track lands.
- New score replacing all 45 shared tracks (`data/core/music/`) and the campaign tracks.
- Each replacement is an OGG file with the same name.

---

## Phase 8: Release

- GPL compliance:
  - Source available (public repo).
  - Original credits kept (`data/core/about.cfg`), with George's added.
  - Changed files marked as changed.
- Store listing: the fastlane metadata in `fastlane/metadata/android/` updated with Wesnoth Mobile text and screenshots.
- Signed release build from the Oracle server.
