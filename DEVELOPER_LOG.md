# Developer Log

Dated record of every push. Times are Mountain Time (El Paso).

## 2026-10-19 13:21 MDT — Startup screen: animated layered logo

**Files**
- `packaging/android/app/src/main/res/layout/activity_init.xml`: old static `wesnoth_icon` ImageView commented out; added `logo_stack` FrameLayout with `logo_shield` and `logo_title` layers.
- `packaging/android/app/src/main/java/org/wesnoth/Wesnoth/InitActivity.java`: added `layoutLogo()` and `playLogoIntro()`, called from `onCreate()`.
- `packaging/android/app/src/main/res/drawable-nodpi/splash_shield.webp`: copy of `images/logos/logo-bg.webp`.
- `packaging/android/app/src/main/res/drawable-nodpi/splash_title.webp`: copy of `images/logos/logo.webp`.

**Behavior**
- Shield starts 3.2× size, black, blurred (Android 12+), tilted 22° toward the camera. Over 900 ms it scales into place while brightening, sharpening and leveling, with a small overshoot settle.
- The title text then does the same over 750 ms, after a 100 ms gap.
- The message, progress bar and "Tap to Start" fade in over 400 ms after the title lands.
- The logo is sized to 80% of screen width, capped at 55% of screen height, so it works in portrait and landscape.
- All tuning constants sit at the top of the startup-intro block in `InitActivity.java`.

**Status:** not yet compiled. Needs an Oracle server build and a device test.

**Pending**
- Full-screen background art under the logo. Planned at 3840×3840 with key content in the center ~1650 px.
- Theme song at startup. Will be wired in when uploaded.

## 2026-10-24 11:49 MDT — Portrait filename fixes; README cleanup

**Portraits** (new art copied onto the names the game loads; uploaded originals kept)
- `Heir_To_The_Throne_Classic/images/portraits/`: `lisar_glad` → `lisar-glad`, `lisar_mad` → `lisar-mad`, `lisar_sceptre` → `lisar-sceptre`, `lisar_defeat` → `lisar-defeat`, `delfador_mentoring` → `delfador-mentoring` (all .webp). Replaces upstream art.
- `Descent_Into_Darkness/images/portraits/`: `dela_mad.webp` → `dela-mad.webp`.
- `Sceptre_of_Fire/images/portraits/`: `haldric-II.webp` → `haldric-ii.webp` (Android filenames are case-sensitive).
- `Heir_To_The_Throne/images/portraits/`: `lisar-defeated.webp` → `lisar-defeat.webp`; `isolde-old.webp` converted to `isolde-old.png` (code loads the .png).

**README.md**
- Status disclaimers commented out (top note, illustrations line, two Development Progress sentences, Building from Source sentence).
- Konrad image alt text now names each variant.

**Known, not fixed**
- `Heir_To_The_Throne_Classic/units/Tutorial_Warrior.cfg` and `Tutorial_Fighter.cfg` reference `portraits/konrad.webp`, which does not exist (same in upstream).

## 2026-10-24 13:39 MDT — Development journal added

**Files**
- `DEVELOPMENT_JOURNAL.md`: new. Running record of everything worked on and discussed (ideas, designs, decisions, open items). First entry covers Oct 24: battle cutscenes, 2.5D diorama map design (render passes, hex mesh, fog/sky/clouds, edge rings, build order), art plan, naming rules, work done.

**Update to earlier known issue**
- `portraits/konrad.webp` for the Classic tutorial units now exists (George's upload, commit `437be11`).

## 2026-10-24 13:44 MDT — Journal: music notes

**Files**
- `DEVELOPMENT_JOURNAL.md`: added a Music section to the Oct 24 entry (track inventory, campaign-only tracks, plan to replace all music after graphics).

## 2026-10-24 13:50 MDT — Journal: goal, licensing, name

**Files**
- `DEVELOPMENT_JOURNAL.md`: added AAA-conversion goal, playthrough before music, GPL licensing notes, and the product name "Essoth" (Essos echo intentional). Two new open items: USPTO trademark check, IP lawyer review.

## 2026-10-24 13:52 MDT — Journal: title decision

**Files**
- `DEVELOPMENT_JOURNAL.md`: title is "Essoth"; subtitle idea "Essoth: Tactics of the Fallen Kingdom".

## 2026-10-24 13:53 MDT — Journal: campaigns

**Files**
- `DEVELOPMENT_JOURNAL.md`: existing campaigns can be included under GPL terms; original Essoth campaign planned last.

## 2026-10-24 13:54 MDT — Journal: Essoth campaign concept

**Files**
- `DEVELOPMENT_JOURNAL.md`: Essoth campaign concept, a little girl who takes over the world.

## 2026-10-24 13:55 MDT — Journal: Essoth campaign choice system

**Files**
- `DEVELOPMENT_JOURNAL.md`: loving ruler vs. little tyrant choice, and how WML supports it.

## 2026-10-24 13:55 MDT — Journal: Princess Maker model

**Files**
- `DEVELOPMENT_JOURNAL.md`: Essoth campaign modeled on Princess Maker, and how Wesnoth can support it.

## 2026-10-24 13:58 MDT — Journal: alignment stat

**Files**
- `DEVELOPMENT_JOURNAL.md`: moral alignment stat and its tie-in to Wesnoth's lawful/chaotic unit alignment; her theme music shifting with path.

## 2026-10-24 13:59 MDT — Journal: alignment-driven art style

**Files**
- `DEVELOPMENT_JOURNAL.md`: art style shifting with her alignment (art sets, color grading, sky/fog).

## 2026-10-26 13:55 MDT — Roadmap

**Files**
- `ROADMAP.md`: new. Full plan, phases 0–9, from the Oracle build and phone playability through the mobile interface, art, battle scenes and the 2.5D map, to music, the Essoth campaign and release.
- `DEVELOPMENT_JOURNAL.md`: Oct 26 entry with the roadmap and repo findings (app ID still `org.wesnoth.Wesnoth`; three README-linked docs missing; refork not yet compiled).

## 2026-10-26 14:09 MDT — Essoth campaign concept refined

**Files**
- `ROADMAP.md`: Phase 8 now reads: a little girl who becomes queen; her decisions lead her step by step toward good or evil; some Princess Maker elements; position in the order open.
- `DEVELOPMENT_JOURNAL.md`: Oct 26 entry for the refined concept.

## 2026-10-26 14:38 MDT — Journal: sprite sheets and high-res sprites

**Files**
- `DEVELOPMENT_JOURNAL.md`: cutting AI sprite sheets into frames (process, risks, team-color remap) and the case for high-res sprites (engine support, cost, 2× proposal).

## 2026-10-26 14:53 MDT — Campaign logos and theme songs

**Files**
- `DEVELOPMENT_JOURNAL.md`: campaign_image can be a logo; plan for each campaign to open with its logo and a theme song (opening story part with `music=`).
- `ROADMAP.md`: Phase 3 item 7, campaign logos plus opening logo screen with theme song.

## 2026-10-26 14:55 MDT — Journal: logo and theme timing

**Files**
- `DEVELOPMENT_JOURNAL.md`: logos in progress; theme music after each campaign's story screens are done and it has been played.

## 2026-10-30 13:48 MDT — Priority: phone UI

**Files**
- `ROADMAP.md`: marked Phases 1–2 (phone UI) as top priority.
- `DEVELOPMENT_JOURNAL.md`: Oct 30 entry; noted Secrets of the Ancients `image=` points to the deleted `campaign-image.webp`.

## 2026-10-30 20:23 MDT — Journal: Secrets of the Ancients image resolved

**Files**
- `DEVELOPMENT_JOURNAL.md`: campaign image now matches `_main.cfg` (George's upload `1fde389`).

## 2026-10-30 20:26 MDT — Eastern Invasion: campaign image no longer mirrored

**Files**
- `data/campaigns/Eastern_Invasion/_main.cfg`: `image=` no longer ends in `~FL()`, so the new logo displays the right way round. Original line commented out above it.
- `DEVELOPMENT_JOURNAL.md`: campaign image naming notes and this change.
