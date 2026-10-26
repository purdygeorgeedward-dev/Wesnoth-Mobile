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

### Goal, licensing and name (13:50 MDT)
- **Goal:** convert Wesnoth into a AAA-quality game: one consistent art direction from title screen to final cutscene, a unified score, the 2.5D map, battle scenes, and a phone-first interface, on top of Wesnoth's proven game design.
- **Music timing:** George will do a full playthrough before writing the new tracks, so music is written for the moments it plays over. (Offered: a list of which scenarios use which tracks.)
- **Licensing (general information, not legal advice):**
  - Wesnoth's code is GPL. Changes must stay GPL and source must be available when the game is distributed.
  - Wesnoth's existing art, music and story are GPL too. Anything kept or directly edited from them stays GPL.
  - Original new art and music are George's own work, and he chooses their license. Precedent: id Software released the Doom and Quake engines as GPL but kept the game data commercial.
  - Gray area: new art of Wesnoth's characters (Konrad, Li'sar, etc.). Get an IP lawyer's read before selling.
  - Selling is allowed under the GPL. Buyers must be able to get the source (the public repo covers this), and they can't be stopped from sharing. Krita (Krimble's upstream) is GPL and sold on Steam and the Microsoft Store.
  - Use a name of its own in stores, not "The Battle for Wesnoth."
- **Name: "Essoth."** No existing game or product by that name was found in a quick web search. The echo of Essos (*A Song of Ice and Fire*) is intentional. Esso (ExxonMobil fuel) is a different market. A USPTO trademark search is still to do before launch.
- **Title (13:52 MDT):** just "Essoth", not "The Battle for Essoth". It avoids sitting next to "The Battle for Wesnoth" and works better as a one-word store and home-screen name. Subtitle idea George liked: **"Essoth: Tactics of the Fallen Kingdom."**
- **Existing campaigns (13:53 MDT):** can be included, changed and sold. They stay GPL (story, scenarios, WML). Keep the original authors' credits (`data/core/about.cfg`) and add George's alongside. Mark modified files as changed. The in-story kingdom name "Wesnoth" is fine as a place name, or it can be renamed.
- **Essoth campaign:** George will write an original Essoth campaign last, after everything else is done.
  - Concept: a little girl who takes over the world.
  - Core choice: the player plays her as a loving ruler or a little tyrant.
  - Supported by WML: dialogue choices, campaign variables that persist between scenarios, branching to different next scenarios, changing recruits/allies/income, multiple endings.
  - Ideas discussed: a hidden "mercy vs. fear" score from her choices; allies join or defect, villages stay loyal or rebel (income), different scenario paths, separate endings; her portraits change with the path (George's art).
  - Model: like *Princess Maker* (raising sim with stats and many endings), combined with tactical battles.
  - Feasible in Wesnoth: between-battle "upbringing" screens with Lua/GUI dialogs; stats kept in campaign variables; training choices change her unit (abilities, traits, advancement); endings chosen from the stats.
  - Her theme music can shift with her path (tender vs. darker), alongside the portraits.
  - Alignment stat: a moral score (e.g. -100 evil to +100 good) tracking drift either way. Tie-in to Wesnoth's existing unit alignment: good path → lawful (stronger by day), evil path → chaotic (stronger at night), middle → neutral or liminal. Applied with `[modify_unit]` as she drifts.
  - Alignment also drives the art style: different portrait and story-art sets per range (swapped by variable at thresholds); whole-screen color grading via `[color_adjust]` and image path functions (`~CS()`, `~BLEND()`, etc.), warm and bright toward good, cold and desaturated toward evil; in the 2.5D view, the sky, fog and cloud colors follow too.

### Open items
- Stray files the game never loads (kept on purpose): underscore copies in `Heir_To_The_Throne` (`lisar_*`, `isolde_old`, `harper_old`, `delfador_mentoring`), `konrad-chainmail-cup`, `konrad-chainmail-defeated`; in Classic `harper_old`, `isolde_old`, `konrad-elvish-sceptre`; Legend of Wesmere `haldricll`, `kalenz-young`; Dead Water `portraits/campaign_image.png`.
- Secrets of the Ancients: new portraits are PNG, pending conversion. The game loads `campaign-image.webp` (hyphen).
- README "Direction" section doesn't yet mention the 2.5D map or battle cutscenes.
- A design document for the 2.5D map can be added to the repo if wanted.
- USPTO trademark check for "Essoth" before launch.
- IP lawyer review of licensing for new art and music before selling.

---

## 2026-10-26 (Monday)

### Roadmap (13:55 MDT)
- George asked for a detailed plan for making the game mobile, with 2.5D in the final plan. Written to `ROADMAP.md`.
- Phase order: 0 Foundation → 1 Playable on a phone → 2 Mobile interface → 3 Art replacement (parallel) → 4 2.5D art spec, tiles and sprites → 5 Battle scenes → 6 2.5D map → 7 Playthrough and music → 8 Essoth campaign → 9 Release.
- Repo findings while planning:
  - Nothing since the Oct 19 refork has been compiled, including the animated startup logo.
  - `packaging/android/app/build.gradle` still has `applicationId "org.wesnoth.Wesnoth"`, although the README says a separate ID was set.
  - The README links to `BUILD_NOTES.md`, `MOBILE_DEVELOPMENT_LOG.md` and `docs/ANDROID_BUILD_GUIDE.md`, which don't exist.
  - GUI: 109 dialogs and 72 widgets in `data/gui/themes/default/`. The `celes` theme shows how a theme overrides selected parts. Dialogs can switch layouts by window size with `[resolution]` blocks.
  - The in-game layout is a separate theme in `data/themes/default.cfg`.
  - The engine already handles finger events (`src/controller_base.cpp`, `src/events.cpp`).

### Essoth campaign concept, refined (14:09 MDT)
- A little girl who becomes queen. She must make decisions that lead her, step by step, toward good or evil. It will have some elements of *Princess Maker*.
- Where it falls in the roadmap order is not decided yet.

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
