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
