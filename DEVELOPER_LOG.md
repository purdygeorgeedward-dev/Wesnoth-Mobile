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

## 2026-11-01 08:37 MST — Smooth camera plan; momentum panning

**Files**
- `src/display.hpp`, `src/display.cpp`: momentum panning functions and state.
- `src/mouse_events.cpp`: drag pan feeds `pan_sample`.
- `src/mouse_handler_base.cpp`: touch down cancels, touch release starts the fling.
- `src/controller_base.cpp`: fling applied each frame; edge stops it.
- `ROADMAP.md`: "Smooth camera (2D now, 2.5D later)" plan.
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** not compiled or run yet (no build server).

## 2026-11-01 12:56 MST — 2.5D normal map notes

**Files**
- `ROADMAP.md`: "Normal maps (investigated 2026-11-01)" under Phase 6.
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 13:19 MST — Decision: terrain-only normal maps

**Files**
- `ROADMAP.md`: normal map decision recorded in Phase 6.
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 13:20 MST — Cloud layer in art spec

**Files**
- `ROADMAP.md`: clouds added to Phase 4 spec; draw order and shadow notes in Phase 6.
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 13:22 MST — Campaign portraits complete

**Files**
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 13:24 MST — Core portraits progress

**Files**
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 13:24 MST — Plan note

**Files**
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-01 15:40 MST — Backdrop crop method decision

**Files**
- `DEVELOPMENT_JOURNAL.md`: entry.

**Status:** notes only, no code.

## 2026-11-02 22:28 MST — Portrait renames to match stock file names

**Files**
- 11 new portraits renamed over the stock file of the same portrait so the WML loads them: Liberty/dommel, Son_Of_The_Black_Eye/kapoue-angry, The_Deceivers_Gambit/lionel-old and methor-old, The_Rise_Of_Wesnoth/addroran, core humans/javelineer, nagas/naga-mace_warrior1-3, undead/bone-shooter, undead/skeletal_rider.
- Old misspelled names removed (drommel, kapoue_angry, lionel_old, methor_old, addoran, javelinier, naga-mace-warrior1-3, bone-shoiter, skeletal-rider).

**Status:** file renames only, no code. Not built or run.

## 2026-11-08 10:53 MST — Difficulty selection screen (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/campaign_difficulty.cfg`: new. Portrait (base resolution): title, message, tall difficulty rows, Play and Cancel side by side. Landscape (800x400 and up): title, message and buttons left, rows right. Widget ids kept from the default theme (title, message, listbox, icon, victory, label, ok, cancel). Row padding raised from 5 to 10 for touch.
- `ROADMAP.md`: screen 3 marked written.

**Status:** written, not built or run. Tag balance and widget ids checked by script only.

## 2026-11-08 10:57 MST — Story screen (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/story_viewer.cfg`: new. Portrait: tall text panel at the bottom (at least 260 logical px, a third of the screen), with Back / Skip / Next as a row of touch-size buttons under the text (the default theme's two 128 px arrows would take 256 of 540 px of width). Landscape: default layout (arrows at the sides, Skip under the text), text panel at least 170 px and 2/7 of the screen height.
- `ROADMAP.md`: screen 4 marked written.

**Status:** written, not built or run. Tag balance and widget ids checked by script only. The stacked_widget layer order (spacer, panel, controls) is kept because story_viewer.cpp reads text_panel from the background layer.

## 2026-11-08 11:08 MST — Character dialogue screens (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/wml_message.cfg`: new. Replaces the windows wml_message_left, wml_message_right and wml_message_double, each with a landscape and a portrait layout. Six new window definitions (mobile_wml_message_{left,right,double}_{landscape,portrait}).
- `ROADMAP.md`: screen 5 marked written. `DEVELOPER_LOG.md`: corrected a wording slip in the story screen entry ("3.5th" should read 2/7).

**What changed from the default theme:** the default scales a portrait by width/500, assuming ~400 px stock portraits. The replacement portraits are about 1000 to 1500 px, which would draw far too large. Each portrait is now fitted (aspect kept) into a fixed box: landscape 34% of the game width by 88% of the window height (27% wide each for two portraits); portrait 62% of the width by 38% of the screen height (46% by 30% for two). The dialog cannot know the image size, so it reserves the box and the window definition draws the image inside it using image_original_width and image_original_height. Portrait layout: window fills the lower 74% of the screen, portrait box on top inside the text panel, message, input list and OK underneath. Message and input widgets are copies of the default ones with 10 px borders.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risk: macros with arguments inside quoted formulas were written from the default theme's patterns but have not been through the real WML preprocessor.

## 2026-11-08 11:10 MST — Objectives and confirmation dialogs (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/transient_message.cfg`: new. Scenario objectives are shown with this dialog (play_controller::show_objectives), as are many short notices. Portrait and landscape resolutions; width is the screen width minus 20 (at most 900 in landscape) instead of a fixed 800, 10 px borders, height capped to the screen. Click anywhere to close, as before. The message must stay a label (the engine registers it as one), so a very long message is cut off rather than scrolled.
- `data/gui/themes/mobile/dialogs/message.cfg`: new. Confirmation and notice dialog with up to four buttons (left_side, ok, cancel, right_side). Portrait: nearly full width, buttons stacked and full width. Landscape: buttons in one row at the right, 10 px borders.
- `ROADMAP.md`: screens 6 and 19 marked written.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. The macro for the buttons passes an empty argument `()` for the two buttons that have no fixed label.

## 2026-11-08 11:11 MST — Units dialog: recruit, recall and unit list (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/units_dialog.cfg`: new. One window (units_dialog) serves recruit, recall, the unit list and debug unit creation. Portrait: fills the screen; title, search, the list (all spare height), the preview pane under it, the gender/variation row, then two rows of full-width buttons. Landscape: fills the screen; title and search on one row, preview pane left and list right, then one row of buttons (help at left; dismiss, rename, favorite, ok, cancel at right). The default theme window can be 750 tall and does not fit a 540 tall landscape screen.
- `ROADMAP.md`: screens 7, 8 and 11 marked written.

**Kept on purpose:** the list (header, sort toggles and row template, macro _GUI_RECALL_LIST) is copied unchanged from the default theme, because units_dialog.cpp registers a sort option for every header column id. Only the row padding changed (8 instead of 5). Wide rows scroll sideways in portrait.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risk: the preview pane has a natural size that may be tall; in portrait it is placed under the list, and if it is too tall the list gets squeezed.

## 2026-11-08 11:12 MST — Attack selection dialog (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/unit_attack.cfg`: new. Portrait: fills the screen; attacker panel above defender panel, weapon list takes the spare height, Attack and Cancel side by side, Damage Calculation as a full-width row under them. Landscape: fills the screen; panels side by side as in the default theme, weapon list under them, one row of buttons (Damage Calculation at the left, Attack and Cancel at the right) so it fits 540 tall.
- `ROADMAP.md`: screen 9 noted: attack selection written, the damage prediction dialog is not done.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. The weapon list is the default theme's with 8 px borders. Widget ids match unit_attack.cpp.

## 2026-11-08 11:16 MST — Load game and save game dialogs (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/game_load.cfg`: new. Portrait: fills the screen; title, saves folder row, search box, saved games list and details pane share the spare height, then toggles with Delete, then Load and Cancel as full-width buttons. Landscape: fills the screen; left column title, saved games list and folder row; right column search box, details pane and option toggles; one bottom row with Delete at the left and Load and Cancel at the right. The default window is two columns and up to 850 tall, which does not fit a phone. All widget ids the engine requires are kept. The widget blocks (list, folder row, details pane, toggles, buttons) were copied from the default theme by script; the saved games list has 8 px row padding instead of 5.
- `data/gui/themes/mobile/dialogs/game_save.cfg`: new. Placed at the TOP of the screen because the on-screen keyboard covers the lower half; the default theme centers it, which puts the name box under the keyboard. Portrait: nearly full width, Save and Cancel as equal buttons side by side. Landscape: up to 900 wide, buttons at the right.
- `ROADMAP.md`: screens 13 and 14 marked written.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Not covered: the game_save_message and game_save_oos windows (small prompts) still use the default theme.

## 2026-11-08 11:18 MST — Preferences dialog (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/preferences.cfg`: new. The six pages are the default theme's, included unchanged from `data/gui/themes/default/dialogs/preferences/`. The default window is up to 650 tall and its pages do not scroll, so on a 540 tall landscape screen the lower part of each page would be cut off. Now: the window fills the screen in both orientations; the tab container sits inside a scrollbar_panel so a page taller than the screen scrolls; portrait uses the existing "horizontal" tab container (page tabs in a row across the top) so the page gets the full width; landscape keeps the vertical tab list on the left. Bottom buttons have 10 px borders. The macros the pages use were copied from the top of the default preferences.cfg (it removes them at its end) and removed again at the end of this file.
- `ROADMAP.md`: screen 15 marked written.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risks: (1) the engine finds the tab container by id "pager" inside a scroll panel, which I expect to work (find_widget recurses) but have not confirmed in tab_container.cpp; (2) pages are wider than 540 in portrait and will scroll sideways.

## 2026-11-08 11:19 MST — Help browser (mobile theme)

**Files**
- `data/gui/themes/mobile/dialogs/help_browser.cfg`: new. The default window is a fixed 1350x800 centered, larger than a phone in either orientation. Landscape: fills the screen; topic tree at the left; at the right the title/contents/back/next/search row, the scrollable text and Close. Portrait: fills the screen, stacked: the top row, the topic tree, the scrollable text, Close as a full-width button. The Contents toggle hides the tree (the engine toggles the tree panel) so the text can use the whole screen. The tree, top row, text panel and Close button were copied from the default theme by script; ids unchanged.
- `ROADMAP.md`: screen 16 marked written.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risk: in portrait the tree and the text share the height equally until Contents is pressed.

## 2026-11-08 11:20 MST — Damage calculations dialog; roadmap checks for screens 10, 12, 17, 18

**Files**
- `data/gui/themes/mobile/dialogs/attack_predictions.cfg`: new. Fills the screen in both orientations. The two data columns (attacker, defender) are inside a scrollbar_panel so they scroll when too tall for the screen. Landscape: attacker and defender side by side as in the default theme. Portrait: attacker above defender (two columns would get only about 250 px each). Close is a full-width button in portrait, at the right in landscape. The column macros are copied unchanged from the default theme.
- `ROADMAP.md`: screen 9 both halves written. Screens 10, 12, 17 and 18 re-examined in the source: 10 is the help browser (screen 16); 12 is not a GUI2 dialog (it belongs to the in-game theme, 2c); 17 and 18 are the transient message dialog (screen 6). Notes in the roadmap explain each.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script.

## 2026-11-08 11:31 MST — Create game / map settings dialog (mobile theme), last of the 20 screens

**Files**
- `data/gui/themes/mobile/dialogs/mp_create_game.cfg`: new. The default theme has three size variants (1200x700, 900x700 switched off, 800x600), all at least 600 tall, so on a 540 tall landscape screen the engine falls back to the 800x600 one (it picks the closest variant when none fits: gui_definition.cpp, get_best_resolution). That variant is full screen and tabbed but cannot show a whole page. This file is based on it with two changes: the pages (stacked_widget "pager") are inside a scrollbar_panel so a page taller than the screen scrolls; landscape (window 800x400 and up) keeps the header as one row, portrait (base resolution) puts the tab bar on its own row under the title. Widget ids and all macros are unchanged (macros copied from the default file, removed again at the end of this one). The window definition "mp_create" comes from the default theme.
- `ROADMAP.md`: screen 20 marked written.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risk: the engine looks up the pager inside a scroll panel; I expect find_widget to recurse but have not confirmed it in mp_create_game.cpp.

## 2026-11-08 11:39 MST — Additional mobile dialogs from the full dialog scan

**Files (all in `data/gui/themes/mobile/dialogs/`)**
- `drop_down_list.cfg`: drop-down menu popup. Same as the default, with taller rows (9 px padding instead of 5) for touch. Its height formula already adapts to the free space above or below the button.
- `unit_advance.cfg`: level-up choice. Portrait: choice list on top (spare height), preview below, full-width OK. Landscape: fills the screen with the default arrangement (preview left, list right). The default window can be 700 tall.
- `edit_text.cfg`, `edit_label.cfg`, `prompt.cfg`: copies of the default dialogs placed at the top of the screen because the on-screen keyboard covers the lower half; width capped to the screen (at most 900).
- `wml_error.cfg`, `language_selection.cfg`, `sp_options_configure.cfg`: copies with the 600 to 900 wide and 600 to 800 tall caps replaced by the screen size minus 20.
- `ROADMAP.md`: new list of additional dialogs with what is written, what needed no change (surrender_quit, game_save_overwrite, game_delete) and what is still to do.

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. The size-cap changes assume maximum_width and maximum_height accept formulas, as they do in the dialogs written earlier today.

## 2026-11-08 12:36 MST — Mobile dialogs, second batch (statistics and Tier 2)

**Files (all in `data/gui/themes/mobile/dialogs/`)**
- `statistics_dialog.cfg`, `game_stats.cfg`: copies of the default dialogs. The window fills the screen (the defaults are centered with height caps of 500 and about 750) and the middle part (the statistics lists; the game status pager) is inside a scrollbar_panel so it scrolls instead of being cut off. The title row, tab bar and buttons stay in view. One layout serves both orientations.
- `achievements_dialog.cfg`, `game_version.cfg`, `label_settings.cfg`, `theme_list.cfg`, `simple_item_selector.cfg`: copies with the size caps (up to 1200 wide, 600 to 750 tall) replaced by the screen size minus 20, at most 900 wide.
- `ROADMAP.md`: additional dialog list updated. end_credits, loading_screen and outro need no change (already full screen).

**Status:** written, not built or run. wmllint reports nothing; tag balance checked by script. Risks: the engine looks up the pager in game_stats inside a scroll panel (expected to work: find_widget recurses); text in game_version may wrap more at 900 wide than at the default 1200.

## 2026-11-08 13:51 MST — Tooltips checked, no change written

**Files**
- `ROADMAP.md`: tooltip entry updated.

**Findings:** tooltip_floating and tooltip_large place themselves from the screen size and the pointer position (formulas choose Above, Below or TooBig and keep the tooltip on screen), so no layout change is needed for the phone screen. Unknown without running the game: whether a tap or long press on Android makes the tooltips appear (they depend on pointer hover). Text for keyboard shortcuts will not be useful on a phone.

**Status:** notes only, no code.

## 2026-11-08 13:57 MST — In-game theme (2c)

**Files**
- `data/themes/mobile.cfg`: new theme "Wesnoth Mobile" (id Mobile). Two resolutions, landscape first because the engine takes the first one that fits the screen (theme.cpp, theme::set_resolution): landscape 1000x480 and up, portrait 480x1000 and up. Top bar 48 high; bottom bar 60 high (portrait: two bottom bars). Elements use plain coordinates and anchors: a left or top anchored one stretches with the extra screen size, a right or bottom anchored one moves with it (theme.cpp, theme::object::location), so the same file fits the A26 at about 1170x540 and 540x1170 logical pixels.
- `images/buttons/mobile/button-96*.png`, `button-112*.png` (normal, pressed, active, active-pressed, touched): the stock large button (168x44) cut into a 9-slice and resized by script. Buttons draw at their image size (button.cpp, calculate_size), so a 44 high image is the touch target (88 px on the A26).
- `data/core/images/themes/mobile/top-bar.png`, `bottom-bar.png`: flat placeholder panel tiles (dark brown with a thin gold edge line).
- `src/preferences/preferences.cpp`: on Android, prefs::theme() returns "Mobile" instead of "Default" when no theme has been chosen. The old line is kept inside the #else branch.
- `ROADMAP.md`: 2c updated with what is written and what is not.

**Status:** written, not built or run. wmllint reports nothing; tag balance and duplicate ids checked by script. Risks: (1) the theme parser has never seen this file, so a typo shows up as a theme load error when the game starts; (2) the logical screen size reported by Android may be a little smaller than 1170x540, so the 1000x480 minimum was chosen with room to spare; (3) a phone that already saved a theme choice keeps the old one.

## 2026-11-08 15:53 MST — Android launcher icon from the transparent logo

**Files (all under `packaging/android/app/src/main/res/`)**
- `mipmap-mdpi`, `-hdpi`, `-xhdpi`, `-xxhdpi`, `-xxxhdpi`/`wesnoth_icon.png` (48, 72, 96, 144, 192 px): the stock Wesnoth icon replaced by the shield-and-swords art (`images/logos/logo-bg.png`, alpha trimmed) at 80% of the square, on the dark blue #343D4D.
- `drawable/wesnoth_logo.png`: adaptive-icon foreground, now 432x432 (was 108x108), art at 60% of the square so it sits inside the 66% safe zone that Android keeps when it crops to a circle or squircle.
- `drawable/wesnoth_logo_bw.png`: monochrome version for themed icons, 432x432, black from the logo's alpha.
- `mipmap-anydpi-v26/wesnoth_icon.xml`: adaptive background changed from colorAccent (tan #DFBE7E) to colorPrimary (dark blue); the old line is kept as a comment.
- `ROADMAP.md`: 2d app icon line updated.

**Status:** images generated and viewed on a contact sheet; not built or run. Not covered: `drawable/wesnoth_icon.png` (441x198, the old static logo, only referenced from a commented-out block) and the in-game window icon (`data/core/images/wesnoth-icon.png`, already George's 128x128 icon).
