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

## 2026-10-26 13:55 MDT — Roadmap

**Files**
- `ROADMAP.md`: new. Full plan, phases 0–9, from the Oracle build and phone playability through the mobile interface, art, battle scenes and the 2.5D map, to music and release.
- `DEVELOPMENT_JOURNAL.md`: Oct 26 entry with the roadmap and repo findings (app ID still `org.wesnoth.Wesnoth`; three README-linked docs missing; refork not yet compiled).

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

## 2026-10-30 21:36 MDT — Journal: The Deceiver's Gambit replaces Delfador's Memoirs

**Files**
- `DEVELOPMENT_JOURNAL.md`: Delfador's Memoirs dropped in favour of The Deceiver's Gambit; README section and logo done by George; open item for the in-game selection image.

## 2026-10-31 18:18 MDT — Wesnoth Mobile GUI theme started

**Files**
- `data/gui/themes/mobile/_main.cfg`: new theme, id `mobile` ("Wesnoth Mobile").
- `data/gui/themes/mobile/widgets/`: button_default, menu_button_default, text_box_default, slider_default, vertical_scrollbar_default, horizontal_scrollbar_default, each enlarged for touch.
- `data/gui/_main.cfg`: registers the Wesnoth Mobile theme.
- `src/preferences/preferences.cpp`: Android first launch uses the Wesnoth Mobile theme (desktop logic kept under `#else`).
- `ROADMAP.md`, `DEVELOPMENT_JOURNAL.md`: progress and the pixel-scale finding.

**Status:** wmllint clean; not compiled or run yet (no build server).


## 2026-10-31 18:25 MDT — History cleanup (force push)

- Private planning notes removed from `DEVELOPMENT_JOURNAL.md`, `ROADMAP.md`, `DEVELOPER_LOG.md` and from commit messages, across commits from 2026-10-24 13:50 onward. History on `master` was rewritten and force-pushed.
- Any other clone of this repo made before this push needs a fresh clone.
- Some commit hashes quoted in the journal from Oct 24 onward refer to the pre-cleanup commits.

## 2026-11-01 00:59 MDT — Phone pixel scale; phone main menu

**Files**
- `src/preferences/preferences.hpp`: Android minimum logical window 540×540 (desktop values kept under `#else`).
- `src/video.cpp`: Android automatic pixel scale keeps the short side ≥ 540 logical px (desktop logic kept under `#else`).
- `data/gui/themes/mobile/dialogs/title_screen.cfg`: new phone main menu, portrait and landscape layouts.
- `data/gui/themes/mobile/widgets/image_mobile.cfg`: `mobile_logo` image definition (logo capped at 480×160).
- `data/gui/themes/mobile/_main.cfg`: loads `dialogs/`.
- `DEVELOPMENT_JOURNAL.md`, `ROADMAP.md`: progress.

**Status:** wmllint clean; not compiled or run yet.

## 2026-11-01 07:22 MST — Mobile UI: campaign selection

**Files**
- `data/gui/themes/mobile/dialogs/campaign_dialog.cfg`: new, portrait and landscape layouts.
- `ROADMAP.md`: item 2 (Campaign selection) marked written.
- `DEVELOPMENT_JOURNAL.md`: entry added.
