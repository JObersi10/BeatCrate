# BeatCrate — CLAUDE.md

Beat Saber Quest 3 mod (mod ID: `apple-music-search`) targeting BS 1.40.8.
All builds happen via GitHub Actions CI only — **no local builds**.

## What the mod does

- Adds a "Music Search" button to the Beat Saber main menu
- Service selector: Apple Music (functional), Spotify (coming soon / grayed out)
- Library tab: Songs / Albums / Playlists sorted A-Z
- Playlist drill-down → tracks list
- Tap a song → BeatSaver search for that title/artist
- Server address is configurable (`IP:8080`) and persisted via `Configuration`

## Toolchain

| Tool | Version |
|------|---------|
| QPM | v1.5.8 — `qpm.json` schema `"0.4.0"` |
| NDK | r27d (arm64-v8a, API 26) |
| C++ | C++23 |
| Compiler | Clang 18 (Android NDK) |

Key dependencies: `beatsaber-hook 6.4.2`, `custom-types 0.18.4`, `bsml 0.4.55`,
`web-utils 0.6.8`, `scotland2 0.1.7`, `songcore 1.1.26`, `song-details 1.0.4`.

## Project layout

```
assets/          # .bsml UI files (embedded at build time → assets.hpp)
cmake/
  assets.cmake   # Python-based embedder: reads *.bsml → raw string literals in assets.hpp
include/
  main.hpp
  Configuration.hpp
  Log.hpp        # AMS_LOG / AMS_ERROR macros
  UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp
  UI/ViewControllers/  # one .hpp per VC
  AppleMusic/AppleMusicClient.hpp
  BeatSaver/BeatSaverClient.hpp
src/
  main.cpp
  Configuration.cpp
  UI/FlowCoordinators/AppleMusicFlowCoordinator.cpp
  UI/ViewControllers/  # one .cpp per VC
  AppleMusic/AppleMusicClient.cpp
  BeatSaver/BeatSaverClient.cpp
```

## Critical API rules (hard-won from build failures)

### custom-types
- Macro: `DECLARE_CLASS_CODEGEN(ns, Name, Base) { body };` — body is inside braces, NOT a 4th arg
- `DECLARE_INSTANCE_FIELD` requires the field type to be **fully included**, not forward-declared
- Cell-selected BSML bindings pass **only `int index`** — no `UnityW<HMUI::TableView>` param

### BSML
- `BSML::parse_and_construct(asset, transform, this)` — from `bsml/shared/BSML.hpp` (NOT `BSML-Lite.hpp`)
- `BSML::Helpers::GetMainFlowCoordinator()` — `bsml/shared/Helpers/getters.hpp`
- `BSML::Helpers::CreateViewController<T*>()` / `CreateFlowCoordinator<T*>()` — `bsml/shared/Helpers/creation.hpp`
- `BSML::Helpers::GetComponentInChildren` **does not exist** — use `go->GetComponentInChildren<T>(false)`

### HMUI / FlowCoordinator
- Animation arg is `HMUI::ViewController_AnimationDirection::Vertical` (NOT `AnimationType::In/Out`)
- `DismissFlowCoordinator(this, AnimationDirection::Vertical, nullptr, false)`
- `PresentFlowCoordinator(fc, nullptr, AnimationDirection::Vertical, false, false)`
- `ReplaceTopViewController(vc, nullptr, AnimationType::In, AnimationDirection::Vertical)` — 4-arg form
- Parent accessor is `_parentFlowCoordinator` (prefixed underscore)
- `PushViewController` / `PopViewController` **do not exist** — use `ReplaceTopViewController`

### Casting UnityW<T>
`il2cpp_utils::try_cast<T>()` takes a raw pointer, not `UnityW<T>`. Pattern:
```cpp
auto _w = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
HMUI::FlowCoordinator* _raw = _w;   // implicit UnityW → T* conversion
auto* fc = il2cpp_utils::try_cast<MyFC>(_raw).value_or(nullptr);
```
`TryCast<T>()` does **not** exist on HMUI types.

### WebUtils
- HTTP calls are synchronous — always run in `std::thread` and dispatch results via `BSML::MainThreadScheduler::Schedule`
- `WebUtils::JsonResponse` is **not copy-constructible** — wrap in `std::make_shared`:
  ```cpp
  auto resp = std::make_shared<WebUtils::JsonResponse>(
      WebUtils::Get<WebUtils::JsonResponse>(WebUtils::URLOptions(url)));
  ```
- For binary/raw downloads use `WebUtils::StringResponse` (not `DataResponse<vector<uint8_t>>`)

### Scotland2 entry points
```cpp
extern "C" void setup(CModInfo* info) noexcept { ... }
extern "C" void late_load() noexcept { ... }
```
`MOD_EXTERN_FUNC` **does not exist** — use plain `extern "C"`.
`ModInfo` **does not exist** — the type is `CModInfo` (or `modloader::ModInfo` from beatsaber-hook).

### Namespaces
- ViewControllers live in `AppleMusicSearch::UI::ViewControllers` (split to avoid collision with `UnityEngine::UI`)
- FlowCoordinators live in `AppleMusicSearch::UI::FlowCoordinators`

### Asset embedding
`cmake/assets.cmake` generates `build/assets.hpp` at configure time via a Python `execute_process`.
Each `.bsml` file in `assets/` becomes `IncludedAssets::<FileName>_bsml` (dots/hyphens → underscores).
`#include "assets.hpp"` resolves because `CMAKE_CURRENT_BINARY_DIR` is added to the include path.

## CI workflow

Push to `main` → `.github/workflows/build.yml`:
1. Install cmake + ninja
2. Download QPM, restore dependencies
3. Download NDK r27d
4. `cmake -B build` (configure + asset generation)
5. `cmake --build build --parallel`
6. Package `BeatCrate-v<version>.zip` → upload artifact + create GitHub Release
