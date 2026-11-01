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
