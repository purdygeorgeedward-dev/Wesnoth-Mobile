# Development Journal

Everything worked on and discussed for Wesnoth Mobile: ideas, designs, decisions, open items.
Times are Mountain Time (El Paso). Pushes are also listed in `DEVELOPER_LOG.md`.

---

## 2026-10-24 (Saturday)

### Wesnoth structure (background)
- Wesnoth has one layer of play: tactical hex battles. No overland or strategic map.
- A campaign is a chain of battles. Surviving units and some gold carry over. Story screens and a journey map run between battles, but the map is not playable.
- Combat is drawn on the hex map itself: the attacker lunges or fires a projectile, the defender plays a defend animation, damage numbers float up, and a dying unit fades out.
- Unit animations are WML blocks in each unit's `.cfg` (`[attack_anim]`, `[defend]`, `[death]`, `[movement_anim]`, `[idle]`). Sprites are PNG frames, usually 72×72. Team color is applied at runtime (`~TC()`, `~RC()`).

### Ideas considered
- **Separate tactical combat layer:** rejected. Months of C++ work, new AI, multiplayer and save problems, and a hard split from upstream.
- **Role-play scenes:** possible with WML and Lua alone (dialogue, choices, small town maps). No engine changes needed.
- **Battle cutscenes:** planned, low priority.
  - Fire Emblem–style panel over the map when an attack starts: terrain backdrop, both units enlarged 2–3×, acting out the strikes the game already rolled.
  - Character portraits (`profile=` images) shown in the panel, with HP bars and hit chances, like the attack prediction window.
  - Display only. No rule changes, so AI, saves and multiplayer are untouched.
  - Needs an off/skip setting.
  - 2D version: moderate C++ (hook the attack sequence, add a panel). Main new art: about a dozen terrain backdrops.

### 2.5D "diorama" map: design
George's concept: a flat 3D plane with the map on it, and objects standing on the hexes like cardstock cutouts, growing and shrinking with distance as the player scrolls. Similar to Paper Mario or Octopath Traveler.

**Priority:** at or near the bottom of the Wesnoth list. George: "the right direction."

**Engine findings (from the fork's source)**
- The engine is on SDL3.
- `draw::smooth_shaded()` in `src/draw.cpp` already calls `SDL_RenderGeometry`, so free-form textured triangles are available without OpenGL.
- The map is drawn flat into an offscreen texture `front_`, then copied to the screen in `display::expose()`. The tilt goes in at that copy.
- Draw order runs through `drawing_buffer_add(layer, loc, …)` with the layers in `src/drawing_layer.hpp`.
- Hex picking: `display::hex_clicked_on()` and `pixel_position_to_hex()`. Hex to screen: `get_location()`.

**Rendering passes**
1. **Flat ground (existing code):** terrain, grid, footsteps, reach shading, fog and shroud, arrows, and unit shadows are drawn flat into `front_`, larger and sharper than the screen. Units are left out.
2. **Tilt:** `front_` is drawn onto a mesh in perspective in `expose()`.
3. **Standing cutouts:** units, halos, HP bars, projectiles, and later standing terrain, drawn upright, scaled to their row, base on the hex, back to front.

**Mesh: hex-aligned from the start**
- 6 triangles per hex: a center point plus 6 corners.
- Corners within each 72×72 hex box: (18,0), (54,0), (72,36), (54,72), (18,72), (0,36); center at (36,36). Columns 54 px apart, odd columns shifted down 36 px.
- Neighboring hexes share corner points, so no cracks open.
- Built from the map size, so every existing map converts automatically.
- About 300 visible hexes × 6 = roughly 1,800 triangles, trivial for the GPU.
- Why not 2 triangles: `SDL_RenderGeometry` stretches the image evenly across each triangle (no depth correction). Two triangles would give evenly spaced rows plus a bend along the diagonal. Many small triangles approximate true perspective.

**Touch:** a tap is converted back to its flat-map position with inverse math, then passed to the existing hex lookup.

**Atmosphere**
- Distance fog: each mesh point gets a color that fades toward a fog color on the far rows. Standing cutouts fade by row too.
- Sky: a gradient behind the far edge, tinted by time of day (blue by day, orange at dusk, deep blue at night).
- Cloud layer: a tiling cloud texture with alpha, drifting slowly, densest at the horizon and fading out before the play area.
- Optional cloud shadows sliding across the ground.

**Map edges**
- Clouds hang over the edges of the map so it never ends in a hard line.
- The renderer adds 2–3 extra hex rings beyond Wesnoth's existing border ring, continuing the edge terrain outward. Clouds sit on those rings.
- The play area is never covered. The extra rings are scenery only, with no gameplay, map-file or save changes.
- Scrolling stops just past the play area so the edge and its clouds are visible.

**Terrain height: later**
- Start perfectly flat. If performance is good, add height to the mesh points: mountains and hills up, water and valleys down, taken from terrain type with no new map data.
- Height also means: cutouts sit at their hex's height, touch picking searches along the ray, and gentle heights look best because the flat ground image stretches on steep slopes.

**Other decisions**
- Pan and zoom only, no camera rotation, because sprites are drawn from one angle.
- A classic flat view toggle stays. The map editor stays flat.

**Build order**
1. Projection module, `src/display_projection.cpp`: screen ↔ map math, tilt, on/off setting.
2. Tilted ground with the hex mesh. Units are temporarily flat. Touch picking. This is the first visible test.
3. Upright units (Pass 3) and shadows.
4. Standing terrain, using a new WML key in terrain graphics to mark cutouts. Needs George's new art.
5. Floating UI: damage numbers, attack indicator, labels, move info.
6. Fog, sky, clouds, edge rings, classic toggle.
7. Performance on the Samsung A26.

**Art spec, set before drawing tiles and sprites**
- Ground tiles drawn straight top-down.
- Cutouts drawn from the front with the ground contact point at bottom-center.
- Shadows as separate soft blobs.
- Team-color magenta palette kept so `~TC()` still works.
- Unit sprites and terrain tiles wait for this spec so they aren't drawn twice.

**Style reference:** `images/maps/wesnoth_map_1920x1080.jpg`: tilted landscape, miniature castles and ships, warm cinematic light, haze, clouds at the edges.
- Open: the label "Morogor" on that map is not a Wesnoth place name. Keep or fix.

### Art plan
- Order: campaign portraits (nearly done) → core portraits → story art → then decide what else benefits most (title and loading screens, UI frames for phone, unit sprites and terrain tiles after the art spec).
- **Core portraits:** 287 files in `data/core/images/portraits/`, by folder: drakes 10, dunefolk 3, dwarves 12, elves 18, goblins 8, humans 63, merfolk 11, monsters 65, nagas 8, orcs 16, saurians 2, transport 13, trolls 7, undead 44, wolves 4, woses 2. All `.webp` except `trolls/troll.png` and `woses/ancient-wose.png`. George is doing all of them himself.
- **Story art:** 142 images in total: 115 in `data/campaigns/<Campaign>/images/story/` across 12 campaigns, plus 27 shared in `data/core/images/story/`. Referenced from `[story]` → `[part]` → `background=` in scenario `.cfg` files.
  - New standard: 1920×1080. The story screen scales backgrounds, so moving from 4:3 to 16:9 is fine.
  - Keep the exact filename and folder to swap without code changes.
  - Format notes: The Deceiver's Gambit uses `.png` and `.jpg`; Rise of Wesnoth has several `.jpg`. Keep those formats or edit the `.cfg` files.
  - Rise of Wesnoth's `trow-logo` plus 12 translated copies in `l10n/` are text logos, best left for later.
  - Sceptre of Fire `cave-rails-beam` and `cave-rails-glow` are light effects and may not need replacing.
- **Journey maps:** `data/campaigns/<Campaign>/images/maps/`.
- **Heir to the Throne, two portrait sets:** Classic Enhanced → `Heir_To_The_Throne_Classic`; the reinvention → `Heir_To_The_Throne`. Already in place.
  - Konrad: Classic is young and calm, elvish green or plain plate. The reinvention ages him across the campaign (young ranger → elvish ranger → bearded knight in blue-and-white lion heraldry → adult in red), with dynamic poses.
  - Li'sar: Classic is martial, with braids, chainmail and a red cape. The reinvention is regal, with curls, a circlet, white-and-gold plate and the same lion heraldry as Konrad; her sceptre is a burning gem on a staff.
  - Classic portraits are split only into `konrad-elvish*` and `konrad-human*`.
- **Batch replacement idea:** zip uploads keep original filenames and times, which could be matched in order to target filenames with a visual check. George decided to rename and place the files himself.

### Naming rules learned
- Match each existing filename exactly. Wesnoth mixes hyphens and underscores (e.g. `lisar-glad.webp` but `kalenz_young.webp`).
- Android filenames are case-sensitive.
- Original vs. new art is checked by git content hash against upstream Wesnoth, not by timestamps.

### Work done
- **README.md** (pushed 11:49): status disclaimers commented out; Konrad alt text names each variant. Earlier note about the number of full-width portraits withdrawn; the portraits stay full-width.
- **Portrait filename fixes** (pushed 11:49): new art copied onto the names the game loads; George's uploads kept.
  - Heir to the Throne Classic: `lisar-glad`, `lisar-mad`, `lisar-sceptre`, `lisar-defeat`, `delfador-mentoring`.
  - Descent into Darkness: `dela-mad`.
  - Sceptre of Fire: `haldric-ii` (from `haldric-II`).
  - Heir to the Throne: `lisar-defeat` (from `lisar-defeated`); `isolde-old.png` converted from `isolde-old.webp`.
  - Classic `lisar-sceptre` was confirmed by George as the leftover old image.
- **Classic tutorial Konrad** (George's upload, commit `437be11`): added `Heir_To_The_Throne_Classic/images/portraits/konrad.webp`. Upstream has no such file even though `units/Tutorial_Warrior.cfg` and `units/Tutorial_Fighter.cfg` load it.
  - In the Classic tutorial Delfador explains (29 lines in part 1) and the student, Konrad or Li'sar, answers (11 lines).
  - The tutorial Konrad is a human sword Fighter (`units/konrad-fighter.png`).
  - Li'sar is selectable only in the tutorial; Konrad is the lead for the rest of the campaign, which is why he has far more portraits.

### Music (13:44 MDT)
- Shared soundtrack: 45 OGG tracks in `data/core/music/`, about 171 MB. Orchestral fantasy (strings, brass, choir, percussion, some Celtic and folk). Volunteer composers over many years, so production quality varies.
- Groups: main themes (`main_menu`, `return_to_wesnoth`, `journeys_end`); faction themes (`loyalists`, `northerners`, `knalgan_theme`, `elvish-theme`, `elf-land`); battle; exploration and mood; story moments; short victory/defeat cues.
- Scenarios choose playlists with `[music]` in WML. A track can be replaced by dropping in an OGG with the same name.
- Campaign-only music in `data/campaigns/<Campaign>/music/`:
  - Heir to the Throne: `juggernaut-louder`, `kvothe`, `primal-carnage`
  - The Deceiver's Gambit: `decoherence`, `helios`, `phase_shift`
  - Eastern Invasion: `heavens-remix`, `journeys_end_faststart`
  - Winds of Fate: `scary_ambient_wind`
- **Plan:** after all graphics work is done, George will replace all the music for a consistent soundtrack.

### Music timing (13:50 MDT)
- George will do a full playthrough before writing the new tracks, so music is written for the moments it plays over. (Offered: a list of which scenarios use which tracks.)

### Open items
- Stray files the game never loads (kept on purpose): underscore copies in `Heir_To_The_Throne` (`lisar_*`, `isolde_old`, `harper_old`, `delfador_mentoring`), `konrad-chainmail-cup`, `konrad-chainmail-defeated`; in Classic `harper_old`, `isolde_old`, `konrad-elvish-sceptre`; Legend of Wesmere `haldricll`, `kalenz-young`; Dead Water `portraits/campaign_image.png`.
- Secrets of the Ancients: new portraits are PNG, pending conversion. The game loads `campaign-image.webp` (hyphen).
- README "Direction" section doesn't yet mention the 2.5D map or battle cutscenes.
- A design document for the 2.5D map can be added to the repo if wanted.

---

## 2026-10-26 (Monday)

### Roadmap (13:55 MDT)
- George asked for a detailed plan for making the game mobile, with 2.5D in the final plan. Written to `ROADMAP.md`.
- Phase order: 0 Foundation → 1 Playable on a phone → 2 Mobile interface → 3 Art replacement (parallel) → 4 2.5D art spec, tiles and sprites → 5 Battle scenes → 6 2.5D map → 7 Playthrough and music → 8 Release.
- Repo findings while planning:
  - Nothing since the Oct 19 refork has been compiled, including the animated startup logo.
  - `packaging/android/app/build.gradle` still has `applicationId "org.wesnoth.Wesnoth"`, although the README says a separate ID was set.
  - The README links to `BUILD_NOTES.md`, `MOBILE_DEVELOPMENT_LOG.md` and `docs/ANDROID_BUILD_GUIDE.md`, which don't exist.
  - GUI: 109 dialogs and 72 widgets in `data/gui/themes/default/`. The `celes` theme shows how a theme overrides selected parts. Dialogs can switch layouts by window size with `[resolution]` blocks.
  - The in-game layout is a separate theme in `data/themes/default.cfg`.
  - The engine already handles finger events (`src/controller_base.cpp`, `src/events.cpp`).

### Sprite sheets and high-res sprites (14:38 MDT)
- Question: can ganged sprite sheets made in ChatGPT be cut apart and put into the game?
  - Yes. Claude can cut out each frame, trim it, line every frame up on the same ground point, name the files and write the animation WML.
  - Risks with AI sheets:
    - Frames that drift in proportions or details.
    - Fake transparency (painted checkerboard or white backgrounds).
    - Jitter between frames.
    - Team color: Wesnoth recolors only exact magenta palette colors. The generator won't produce those, so team-color areas would need to be painted in one flat marker color and remapped to the palette.
  - Each sheet needs labelling: which unit, which animation, frame order.
- Question: any advantage in high-res sprites?
  - Yes. Current sprites are 72×72, and the A26 shows each game pixel with 2–3 screen pixels, so sprites are enlarged and soft. Zoom levels are 36/72/144 (`src/game_config.cpp`).
  - High-res matters most for battle scenes (units shown 2–3×) and the 2.5D near rows.
  - Engine: textures already carry a separate draw size (`set_draw_size`, used for text, UI and the map buffer). Unit, terrain and halo drawing would need to treat a high-res sprite as 72 logical pixels. Moderate change, mainly in `src/units/drawer.cpp`.
  - Cost: 2× resolution = 4× pixels per frame, so a larger app and more memory. Proposal: 2× (144 px) as the standard, matching the top zoom level.

### Campaign logos and theme songs (14:53 MDT)
- `campaign_image` (set by `image=` in each campaign's `_main.cfg`) is the picture in the campaign selection screen's details pane, next to the description. It can be a logo, key art or character art. A transparent-background title logo works well, and a matching set reads as a series. The small list picture is a separate setting, `icon=`.
- **George wants each campaign to open with its logo and a theme song.**
  - How: a new first `[part]` in the opening `[story]` of each campaign's first scenario, showing the logo (`[background_layer]` or `[image]`).
  - The story screen supports `music=` and `sound=` per part (`src/storyscreen/part.cpp`), so the theme starts with the logo and no engine change is needed.
  - `[image]` entries with `delay=` can reveal the logo in steps. A fully animated logo would need Lua or engine work.
  - The campaign's theme song is one OGG per campaign, in its own `music/` folder.
- (14:55 MDT) George is making the campaign logos now. Each campaign's theme music waits until its story screens are done and he has played the campaign.

---

## 2026-10-30 (Friday)

### Top priority (13:48 MDT)
- George: the most important thing is a new UI that scales cleanly to small screens (phones). That's roadmap Phases 1–2 (startup scale, touch, Wesnoth Mobile GUI theme, hand-built screens, in-game layout).
- Found while checking the repo: `Secrets_of_the_Ancients/_main.cfg` line 27 still points to `images/portraits/campaign-image.webp`, which was deleted Oct 28, so that campaign shows no picture on the selection screen until a new file (e.g. the campaign logo) is in place or the line is changed.
- (20:23 MDT) Secrets of the Ancients campaign image fixed by George: logo uploaded as `images/portraits/campaign-image.webp` (commit `1fde389`), the name `_main.cfg` loads. Underscore copy removed.

### Campaign image names (20:26 MDT)
- Upstream is inconsistent: 15 campaigns use `images/campaign_image.png` (underscore, PNG). Secrets of the Ancients alone uses `images/portraits/campaign-image.webp` (hyphen, WebP, in portraits). That's why following the other campaigns gave the wrong name there.
- Eastern Invasion loaded its image with `~FL()` (mirrored left to right), which would show a logo with backwards text. Removed in `_main.cfg`, with the original line kept as a comment.

### Delfador's Memoirs → The Deceiver's Gambit (21:36 MDT)
- Delfador's Memoirs isn't in the repo: upstream replaced it with The Deceiver's Gambit in 1.19.9. It survives only as an add-on.
- Decision: no Delfador's Memoirs. The Deceiver's Gambit is the official rewrite of the same story; a second origin story would contradict the lore and add art and upkeep.
- README: George replaced the Delfador's Memoirs section with The Deceiver's Gambit (blurb by Claude) and added `images/logos/deceiversgambit.jpg` (commits `7918e12`, `87d9a5d`).
- Open: in the game, The Deceiver's Gambit `_main.cfg` still shows Delfador portraits on the selection screen (lines 142, 170), not George's logo `images/campaign_image.png`.

---

## 2026-10-31 (Saturday)

### Mobile UI: first WML work (18:18 MDT)
- No build server yet. George: build WML as far as possible in the meantime. GitHub Actions screenshot builds were offered as an option.
- Claude's sandbox can't compile Wesnoth (1 CPU, 3 GB RAM, resets between tasks).

**How the engine scales today (key finding)**
- `src/video.cpp` picks a whole-number pixel scale. The minimum logical window is 1280×720 (`src/preferences/preferences.hpp`).
- The A26 is 2340×1080, so it can only get scale 1. The UI is drawn at native pixels: tiny. This is why the Oct 18 APK needed screen magnification.
- Portrait (1080 wide) is below the 1280 minimum altogether.
- Fix (C++, not done yet): on Android, aim for logical short side ≈ 540, giving scale 2 on the A26: about 1170×540 landscape and 540×1170 portrait.
  - Default dialogs assume at least 720 high, so this has to land together with the hand-built phone dialogs.
- Upstream already enlarges fonts on Android (`data/gui/macros/_initial.cfg`, `#ifdef ANDROID`: default 22 instead of 17).

**GUI theme mechanics**
- Themes live in `data/gui/themes/<name>/`. Any widget or dialog a theme doesn't define falls back to the default (Parchment) theme (`src/gui/core/gui_definition.cpp`).
- Each widget and dialog can have several `[resolution]` blocks. `window_width`/`window_height` are minimums; the largest one that fits the screen is used. So a landscape block (e.g. 900×500) plus a base block gives separate landscape and portrait layouts.

**Added**
- New theme `data/gui/themes/mobile/` (id `mobile`, shown as "Wesnoth Mobile"), registered in `data/gui/_main.cfg`.
- Touch-size overrides, copied from default with sizes enlarged (logical px, aimed at scale 2):
  - Buttons: 26/30/40 → 44/48/60 tall, wider, default font size.
  - Drop-down menus: 30 → 44 tall, default font.
  - Text boxes: 30 → 44 tall.
  - Sliders: 22 → 40 tall, handle 16 → 28.
  - Scrollbars: 15–25 → 34 thick, handle min 11 → 24; bar images stretched to the full thickness.
- `src/preferences/preferences.cpp`: on Android, first launch starts in the Wesnoth Mobile theme. The desktop logic is kept under `#else`.
- Checked with `wmllint`: no errors. Untested in game until a build server is running.

---

## 2026-11-01 (Sunday)

### Mobile UI: phone pixel scale and main menu (00:59 MDT)
**Pixel scale (Android only, C++)**
- `src/preferences/preferences.hpp`: on Android the minimum logical window is 540×540 (desktop keeps 1280×720 under `#else`).
- `src/video.cpp`: on Android, automatic scale = the largest whole number that keeps the short side at least 540 logical px. A26 (1080×2340) gets scale 2: about 540×1170 portrait, 1170×540 landscape. Text and widgets are twice the size of the Oct 18 build.

**Main menu for phones**
- `data/gui/themes/mobile/dialogs/title_screen.cfg`: two layouts, chosen by screen size.
  - **Portrait** (base): logo, then the menu (two buttons per row), then tips, then footer (About, version, Language).
  - **Landscape** (800×400 and up): logo and tips on the left, menu on the right, footer along the bottom.
- Every widget id the code requires is present (`src/gui/dialogs/title_screen.cpp`); if one is missing, the game silently drops back to the default theme. Help moved from the tips box into the menu.
- `data/gui/themes/mobile/widgets/image_mobile.cfg`: `mobile_logo` image definition. It caps the 600×200 logo at 480×160 (same shape) so it fits a 540-wide portrait screen.
- `data/gui/themes/mobile/_main.cfg` now loads the theme's `dialogs/` folder.
- wmllint clean. Not compiled or run yet (no build server).

**Known gaps until more screens are done**
- Stock dialogs assume at least 720 px of height. At 540 some may not fit until their phone versions are built.
- The in-game layout (`data/themes/default.cfg`) is designed for 800×600 and up, so it needs its own phone version (roadmap 2c).

### Mobile UI: campaign selection screen (07:22 MST)
- `data/gui/themes/mobile/dialogs/campaign_dialog.cfg`: new phone version of the campaign selection window (`campaign_selection`). Two layouts by screen size, both full-screen.
  - **Portrait** (base, about 540×1170): title, search box, then one panel with the sort/filter header, the campaign list, and the description below it; then Modifications, Combat and Difficulty, one label-and-menu pair per row; then Play and Cancel.
  - **Landscape** (800×400 and up, about 1170×540): title and search on one row; list on the left, description on the right; the three menus in one row; Play and Cancel underneath.
- List rows have 10 px borders instead of 5 for finger taps.
- The list, description pane, sort header and Combat/Difficulty menus are copied from the default theme's `campaign_dialog.cfg`, so the translated text stays the same.
- Checked: wmllint clean; macros expanded and brackets balance; every widget id `src/gui/dialogs/campaign_selection.cpp` looks up is present. Not compiled or run (no build server).
- Risk: the window now fills the screen with `automatic_placement = false` and `width/height = screen`. If it opens at the wrong size on the A26, that's the first thing to adjust.

### Smooth camera: plan and momentum panning (08:37 MST)
- George: plan ahead for 2.5D; until then he wants smooth scaling and smooth motion.
- Findings (source): finger drag already pans the map one step per touch event, but only with no unit selected. No momentum. No pinch (only a TODO in `src/events.cpp`). Zoom is stepped hex sizes, changed by hotkeys and wheel only.
- Plan added to `ROADMAP.md` (Phase 1, "Smooth camera"). Key rule: camera state lives in `display`, input only calls camera functions, so the 2.5D projection can replace what is behind them.
- **Momentum panning written** (not compiled or run):
  - `src/display.hpp/.cpp`: `pan_sample`, `pan_release`, `pan_cancel`, `kinetic_pan_step`. Speed is measured from the last drag steps (blended), a fling starts only if the finger was moving above 250 px/s and lifted within 80 ms of the last move, capped at 8000 px/s, friction time constant 0.35 s, stops below 40 px/s.
  - `src/mouse_events.cpp`: drag pan now also feeds `pan_sample` (original line kept as a comment).
  - `src/mouse_handler_base.cpp`: touch down calls `pan_cancel`; touch release calls `pan_release`.
  - `src/controller_base.cpp`: `handle_scroll` adds the fling each frame; reaching the map edge cancels it (original line kept as a comment).
- Next: smooth pinch zoom.

### 2.5D map: hex mesh and normal maps (12:56 MST)
- George asked whether notes exist on the 2.5D map (yes: Phase 6 and the 2.5D design entry above; hex = 6 triangles, centre plus 6 corners, about 1,800 triangles on screen) and whether normal maps can be used.
- Findings added to `ROADMAP.md` Phase 6, "Normal maps (investigated 2026-11-01)": needs SDL 3.4's custom GPU render state and the SDL GPU renderer; the current renderer setup in `src/sdl/window.cpp` would need to change; ground needs a parallel normal buffer and a normal map per tile; cheaper fallbacks are per-vertex light colour and baked lit tile variants.
- Not decided: whether Phase 4 art includes normal maps.

### Decision: normal maps on terrain only (13:19 MST)
- George: terrain will have normal maps; sprites and cutouts will not.
- `ROADMAP.md` Phase 6 updated: Phase 4 art spec includes a normal map per ground tile and transition piece; units and standing terrain stay plain.

### Cloud layer: added to art spec (13:20 MST)
- George: don't forget the cloud layer. It was already in Phase 6 (drifting layer, shadows, clouds over the scenery rings) but missing from the Phase 4 art spec.
- `ROADMAP.md`: Phase 4 spec now lists cloud textures (2 to 3 tileable alpha layers, one shadow mask). Phase 6 notes draw order (after cutouts, far rows only), slower scroll than the ground, and how shadows reach the ground.

### Campaign portraits complete (13:22 MST)
- George: all of the campaign portraits are done. Some campaigns have almost none, which he finds odd.
- Previous status (Oct 23): portraits made up through Secrets of the Ancients. Next on his list: all core unit portraits, then decide which other graphics most benefit from revision.

### Core unit portraits in progress (13:24 MST)
- George is making the core unit portraits and has reached the undead units.

### Next after core portraits (13:24 MST)
- George: after the core unit portraits, he will scan for other graphics to update.

### Title screen backdrop: crop method (15:40 MST)
- George is developing a castle, grass and sky backdrop behind the logo, to work at any resolution and in both orientations.
- Candidates: generated in Perchance at 512 px, then upscaled (images 1 to 9 are 2048×2048; 10 to 14 are the 512 px originals). Not picked yet.
- Crop-tested six at 2340×1080 and 1080×2340. Held up in both: 5, 6, 7, 4 (4 needs the landscape position raised). 3 and 9 clip the castle in one crop.
- **Decision (George):** custom horizontal and vertical crop positions per orientation, since a single fixed crop is what breaks across resolutions.
- Method: in `data/gui/themes/mobile/dialogs/title_screen.cfg`, scale the image to cover the screen using `width`, `height`, `image_original_width` and `image_original_height`, then shift it by two numbers (0 to 1) per orientation. No C++ change. Portrait and landscape blocks each get their own pair.
- Tuning is done on the phone in testing.

### Portrait file names: typos found and fixed (2026-11-02 22:28 MST)
- Scan against stock Wesnoth showed 40 stock portraits unchanged. Several of George's new files had spelling or dash/underscore differences, so the WML still loaded the stock file.
- George: the stock names are the ones in use; the new files must match them exactly. Stock names mix dashes and underscores, so there is no single convention.
- 11 files renamed to the stock names (see DEVELOPER_LOG.md). Still to check: extension mismatches (old_orcish_shaman, orcish_shaman, shan_taum are .png; troll and ancient-wose new files are .webp over stock .png), sun-singer, sun-sylph, naga-ringcast, skeleton-2, and files in images/characters.

### Difficulty selection screen written (2026-11-08 10:53 MST)
- Screen 3 of the 20 hand-built screens. Built the same way as campaign selection: the base resolution is portrait and a second resolution block (window 800x400 and up) is landscape.
- Rows copied from the default theme with larger padding. Buttons use the mobile button definition, so their touch size comes from the theme.
- George's decision (Nov 8): UI screens come before getting a build working, since a build with the old UI is pointless.
- Not tested. Remaining screens: 4 to 20.

### Story screen written (2026-11-08 10:57 MST)
- Screen 4 of 20. The default story viewer is already full-screen and adaptive. The problem on a narrow phone was the two fixed 128 px arrow buttons, so portrait gets text buttons in a row under the text; landscape keeps the arrows.
- Back and Next labels are plain English strings in the wesnoth-lib textdomain (not checked against existing translations).
- Not tested. Remaining screens: 5 to 20.

### Character dialogue screens written (2026-11-08 11:08 MST)
- Screen 5 of 20. Found that the default portrait sizing assumes about 400 px portraits and scales by width/500, so the new 1000 to 1500 px art needed a different method: fit into a fixed box, keep aspect ratio.
- The dialog cannot read the image size (image_original_* only exist inside the window definition's image shape), so the box is reserved in the dialog and the image drawn into it by the window definition.
- Portrait orientation puts the portrait box above the text inside the same panel; landscape keeps the default arrangement (portrait at the side, text up to 675 wide).
- Not tested. Remaining screens: 6 to 20.

### Objectives and confirmations written (2026-11-08 11:10 MST)
- Screens 6 and 19 of 20. Neither has its own dialog: objectives go through transient_message and confirmations through message, which many other prompts also use, so these two changes reach more than just those screens.
- Open question for later: objectives for long campaigns may be taller than the screen. transient_message cannot scroll because its message is a label, so the text would be cut. Fixing that needs a C++ change (or a different dialog for objectives).
- Not tested. Remaining screens: 7 to 18 and 20.

### Units dialog written (2026-11-08 11:11 MST)
- Screens 7, 8 and 11 of 20 share one dialog (units_dialog), so one file covers recruit, recall and the unit list.
- Considered dropping list columns in portrait. Not done: the engine sets up sorting for every header column and a missing id would probably break it. Wide rows scroll sideways instead.
- Not tested. Remaining screens: 9, 10, 12 to 18 and 20.

### Attack selection written (2026-11-08 11:12 MST)
- Half of screen 9. The damage calculation (prediction) dialog is a separate window and still uses the default theme.
- The default dialog is centered and sizes itself, and with two unit panels, a weapon list and two button rows it is too tall for 540. Both layouts now fill the screen and give the weapon list the spare height.
- Not tested. Remaining screens: 9 (prediction), 10, 12 to 18 and 20.

### Load and save written (2026-11-08 11:16 MST)
- Screens 13 and 14 of 20. Load needed every widget kept (the engine requires them all), so the window was rebuilt around widget blocks copied out of the default theme by script rather than retyped.
- Save is placed at the top because of the on-screen keyboard. If the Android keyboard resizes the window instead of covering it, this can go back to centered.
- Not tested. Remaining screens: 9 (prediction), 10, 12, 15 to 18 and 20.

### Preferences written (2026-11-08 11:18 MST)
- Screen 15 of 20, the biggest dialog (about 2000 lines of pages). Not rewritten: the pages are reused, and only the window around them changed.
- The pages have no scrolling of their own, which is the real problem on a 540 tall screen. A scroll panel around the whole tab container fixes that without editing the pages.
- Not tested. Remaining screens: 9 (prediction), 10, 12, 16 to 18 and 20.

### Help browser written (2026-11-08 11:19 MST)
- Screen 16 of 20. The default help window is a fixed size, so landscape only needed to fill the screen; portrait needed a stacked arrangement. The tree panel can only be shown or hidden by the engine (Contents toggle), so portrait starts with the tree visible.
- Not tested. Remaining screens: 9 (prediction), 10, 12, 17, 18 and 20.

### Damage calculations written; four screens turned out to be covered (2026-11-08 11:20 MST)
- Screen 9 is now complete. Checked the engine for 10, 12, 17 and 18: unit details open the help browser; the scenario end report (victory and defeat) is shown with the transient message dialog; the in-game menu belongs to the in-game theme. So those four need no new dialog beyond the ones already written.
- Remaining: screen 20 (skirmish map settings, the multiplayer create-game dialog, about 1500 lines in three size variants) and the in-game layout (2c).
- Not tested.

### Screen 20 written; all 20 screens accounted for (2026-11-08 11:31 MST)
- Written as new dialog files: 2 to 9, 11, 13 to 16, 19, 20 plus the title screen (1). Covered by an existing one: 10 (help browser), 17 and 18 (transient message). Not a dialog: 12 (game menu, part of the in-game theme, 2c).
- Nothing has been built or run. The first build will show which of these load; a dialog with a WML error should fail with a message naming the file and line.
- Remaining UI work from the roadmap: the in-game layout (2c) and startup (2d).

### Additional dialogs, prioritized (2026-11-08 11:39 MST)
- Scanned all 98 default dialog files for fixed sizes and size caps, then ordered the unfinished ones by how likely a campaign player is to reach them. The assessment is in the roadmap.
- Written now: the popup menu, level-up choice, text entry prompts and three dialogs with oversized caps. The small yes/no prompts already fit.
- Largest remaining pieces: in-game layout (2c) and startup (2d), then statistics, credits, loading screen and outro.

### Second batch of dialogs (2026-11-08 12:36 MST)
- Statistics and game status now scroll in their middle section. The full-screen dialogs (credits, loading screen, outro) were checked and need nothing.
- Tooltips are the last dialog item; the larger pieces left are the in-game layout (2c) and startup (2d).

### In-game theme written (2026-11-08 13:57 MST)
- 2c of the roadmap. Read theme.cpp to learn how resolutions and anchors work, then wrote the theme with absolute coordinates instead of the default theme's chained references (the default positions each element from the last one, which is hard to change safely without running the game).
- Decisions: the top bar shows only what a campaign player needs; the unit panel is a strip in the bottom bar instead of a sidebar; End Turn is the largest button and sits at the bottom right.
- Known gaps: no minimap, no hide/show for the unit panel, no replay or timer controls.
- Not tested.

### App icon applied (2026-11-08 15:53 MST)
- George pointed out the icon art already existed with transparency. Found it: `images/logos/logo-bg.png` (shield and swords without the title text). The launcher icons were still the stock Wesnoth ones, so they were rebuilt from it.
- Dark blue adaptive background chosen over the tan accent because gold on tan has little contrast.
- Open for 2d: logo-free splash art for the in-game title screen (George is exporting it) and the symphonic theme song.

### Title screen background art (2026-11-08 16:26 MST)
- George sent the three logo-free splash images (PNG and WebP of each; the square only as WebP, one lossless 8 MB and one heavily compressed 0.8 MB). Used the lossless square and the two PNGs as sources and re-encoded at quality 90 so all three match.
- George also sent a 1024 logo icon with the title text and a soft shadow. Not used for the launcher icon (text unreadable at small sizes); kept for later.
- Open: how the menu and logo should sit over the faces, and the Android start-up screen.

### Background no longer stretched (2026-11-08 16:36 MST)
- George: no stretching, let the images spill off the edges. Changed to cover-and-center. On his phone the crop is about 2.5% of the width or height, split between the two edges.

### Forced Wesnoth Mobile theme on Android (07:08 MST)
- George: portraits and campaign logos are far too big on the phone. The APK came from commit `d3ec967` (Android CI #9, Nov 10), which already contains the Mobile theme and the portrait fitting in `data/gui/themes/mobile/dialogs/wml_message.cfg`. The only later commit, `46c9140`, changed two logo images.
- Cause found in source: `prefs::set_initial_gui2_theme()` (the Android default) is only called from `migrate_version_selection`, and that only runs when the user-data folder has no `logs` directory (`src/filesystem.cpp`, `setup_user_data_dir`). If the app ever ran on the phone before, no theme is saved, and the game uses the stock Default theme.
- George: force the mobile theme. `gui2::switch_theme` now ignores the requested theme on Android and always loads `mobile`. Other platforms unchanged. If `mobile` is missing, the existing fallback to Default still applies.
- Not compiled or run. Next: the next Android CI build is the first test.

### Default fonts: Quattrocento (2026-11-11 10:58 MST)
- George: Quattrocento, in both weights (Regular and Bold), is the default font for Wesnoth.
- Added `fonts/Quattrocento-Regular.ttf` and `fonts/Quattrocento-Bold.ttf` (from google/fonts, OFL) plus `fonts/Quattrocento-OFL.txt`; license entry added to `fonts/COPYING`.
- `data/hardwired/fonts.cfg`: `family_order` now `Quattrocento`. The Lato line is commented out, not deleted. Monospace and script families unchanged.
- Lato files stay in `fonts/` (still the fallback if a glyph is missing).
- Side effect: the old per-language translations of "Lato" (e.g. Arabic) no longer match, so all languages use Quattrocento first and fall back to other installed fonts for glyphs it lacks.
- Not compiled or run. Quattrocento has no italic; italic text will be synthesized.

### Linear filtering for all images (2026-11-11 14:20 MST)
- George: nearest-neighbor filtering is unacceptable for the high-resolution art.
- `src/picture.cpp`: `image::get_texture(locator, type, skip_cache)` (the overload without a quality argument) now uses `scale_quality::linear`; it was `nearest`. Lit (time-of-day) textures are now created with linear filtering too. Old lines commented out, not deleted.
- `src/gui/core/canvas.cpp`: GUI images always use linear filtering; before, only `stretch` and `scale` resize modes did. Old block commented out.
- Textures have no mipmaps. Shrinking more than about 2x still gets noisy, so the pre-built tiers (sharpen, then halve) are still needed.
- Not compiled or run. Pixel-art-style images (minimap, grid overlays) now also blur slightly when scaled. Needs a look on the device.

### Long-press inspects on Android (2026-11-11 14:27 MST)
- George: controls should work the Android way. First change: long-press is inspect, not the right-click context menu.
- `src/controller_base.cpp`, `controller_base::long_touch_callback`: under `__ANDROID__`, a long press runs the unit description command if a unit is under the finger, else the terrain description command. Other platforms still open the context menu (old line kept, in the `#else` branch).
- Not compiled or run. Needs a look on the device: whether the mouse hex is current when the timer fires, and the delay (`gui2::settings::popup_show_delay`).

### Touch replacements for keyboard-only commands, step 1 (2026-11-11 14:28 MST)
- George: replace all keyboard controls (except name entry) and mouse clicks with touch controls.
- Audit: the engine already turns taps into left clicks, and tapping a selected unit deselects it on Android. Keyboard-only commands with no touch route in `data/themes/mobile.cfg`: cancel, previous unit, end unit turn, hold position, scroll to leader, repeat recruit, continue move, zoom in/out/default, accelerated speed, mute, search, labels.
- `data/themes/mobile.cfg`: `actions-menu` (landscape and portrait) now lists them. Old items line kept as a comment.
- `src/mouse_handler_base.cpp`: on Android the long-touch release no longer fires a right-click. Without this the context menu would open after the new long-press inspect. Old code kept inside `#ifndef __ANDROID__`.
- Not compiled or run. Menu is a list under Actions, not one-tap buttons yet.
- Still open: pinch zoom (zoom menu items are the stopgap), skipping unit animations by touch, replay controls, Esc/Enter-only dialog paths not yet audited.
