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

### Open items
- Stray files the game never loads (kept on purpose): underscore copies in `Heir_To_The_Throne` (`lisar_*`, `isolde_old`, `harper_old`, `delfador_mentoring`), `konrad-chainmail-cup`, `konrad-chainmail-defeated`; in Classic `harper_old`, `isolde_old`, `konrad-elvish-sceptre`; Legend of Wesmere `haldricll`, `kalenz-young`; Dead Water `portraits/campaign_image.png`.
- Secrets of the Ancients: new portraits are PNG, pending conversion. The game loads `campaign-image.webp` (hyphen).
- README "Direction" section doesn't yet mention the 2.5D map or battle cutscenes.
- A design document for the 2.5D map can be added to the repo if wanted.
- USPTO trademark check for "Essoth" before launch.
- IP lawyer review of licensing for new art and music before selling.
