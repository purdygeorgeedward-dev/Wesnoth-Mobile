#!/usr/bin/env python3
"""Package game data from the checked-out Wesnoth-Mobile repository into the APK.

Only Git-tracked files in the five game-data directories are included. This
ensures the APK contains this fork's graphics rather than upstream downloads.
"""
from pathlib import Path
import os
import subprocess
import zipfile

root = Path(__file__).resolve().parents[2]
asset_dir = root / "packaging/android/app/src/main/assets"
asset_dir.mkdir(parents=True, exist_ok=True)
archive = asset_dir / "gamedata.zip"
version_file = asset_dir / "gamedata.version"
directories = ("data", "fonts", "images", "sounds", "translations")

paths = subprocess.check_output(
    ["git", "ls-files", "-z", "--", *directories], cwd=root
).split(b"\0")
tracked = sorted(
    path for item in paths if item
    for path in [os.fsdecode(item)]
    if (root / path).is_file()
)
if not tracked or not all(any(p.startswith(d + "/") for p in tracked) for d in directories):
    raise SystemExit("Missing game-data files; refusing to build an incomplete APK")

print(f"Bundling {len(tracked)} tracked game-data files from this repository", flush=True)
with zipfile.ZipFile(archive, "w", allowZip64=True) as zf:
    for i, path in enumerate(tracked, 1):
        # Artwork and audio are already compressed; deflating them wastes CI time.
        ext = Path(path).suffix.lower()
        mode = zipfile.ZIP_STORED if ext in {
            ".png", ".jpg", ".jpeg", ".webp", ".ogg", ".mp3", ".flac", ".zip"
        } else zipfile.ZIP_DEFLATED
        zf.write(root / path, path, compress_type=mode, compresslevel=1)
        if i % 2000 == 0:
            print(f"  packaged {i}/{len(tracked)} files", flush=True)

sha = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
version_file.write_text(sha + "\n", encoding="utf-8")
print(f"Packaged {archive.stat().st_size / (1024 * 1024):.1f} MiB; source {sha}", flush=True)
