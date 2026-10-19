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
