# BeatCrate 🎵

> Browse your Apple Music library inside Beat Saber and instantly find matching custom maps on BeatSaver.

![Beat Saber](https://img.shields.io/badge/Beat%20Saber-1.40.8-purple?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Quest%203-black?style=flat-square)
![Build](https://github.com/jobersi10/BeatCrate/actions/workflows/build.yml/badge.svg)
![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)

---

## What it does

BeatCrate adds a **Music Search** button to your Beat Saber main menu. Tap it and you can:

- **Browse your Apple Music library** — Songs, Albums, and Playlists (sorted A–Z)
- **Drill into any playlist** and see all its tracks
- **Search your Apple Music catalog** directly from the headset
- **Tap any song** → BeatCrate searches BeatSaver for matching custom maps
- **Download maps in one tap**, no browser required

Spotify support is coming soon.

---

## Requirements

| Requirement | Details |
|---|---|
| Beat Saber | 1.40.8 (Quest) |
| Modloader | Scotland2 |
| Quest | Quest 3 (Quest 2 untested) |
| Apple Music server | Running on your phone — see below |

---

## Setup

### 1. Install the mod

Download `BeatCrate-vX.X.X.qmod` from the [Releases](../../releases) page and sideload it with **BMBF** or **ModsBeforeFriday**.

### 2. Run the Apple Music server

BeatCrate talks to a small server running on your phone that handles Apple Music authentication.

> The server lives in a separate repo — link coming soon.

Start it, then note your phone's local IP address (e.g. `192.168.1.42`).

### 3. Set the server address in-game

1. Open Beat Saber → tap **Music Search**
2. In the **Server IP:Port** field, enter your phone's IP and port — e.g. `192.168.1.42:8080`
3. Tap **Apple Music** — your library loads automatically

---

## Usage

```
Main Menu
  └─ Music Search
       ├─ 🎵 Apple Music
       │    ├─ Library
       │    │    ├─ Songs      (recently saved)
       │    │    ├─ Albums
       │    │    └─ Playlists  (A–Z) → tap → tracks
       │    └─ Search          (catalog search)
       │
       └─ 🎧 Spotify           (Coming Soon)

Tap any song anywhere → BeatSaver results panel
  ├─ Map list with uploader + duration
  ├─ Detail panel (diffs, BPM)
  └─ ⬇ Download → added to custom songs instantly
```

---

## Building from source

### Prerequisites

- [Android NDK r26d](https://developer.android.com/ndk/downloads)
- [QPM CLI](https://github.com/QuestPackageManager/QPM.CLI)
- CMake 3.22+

### Steps

```bash
git clone https://github.com/jobersi10/BeatCrate
cd BeatCrate

qpm restore

cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/ndk/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel
```

The output `.so` is at `build/libapple-music-search.so`. Package it into a `.qmod` with the workflow or manually zip it alongside `mod.json`.

---

## CI / Releases

Every push to `main` builds the mod and uploads a `.qmod` artifact. Tagging a commit `vX.Y.Z` creates a GitHub Release automatically.

---

## License

MIT — see [LICENSE](LICENSE).
