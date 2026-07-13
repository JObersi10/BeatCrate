# BeatCrate — CLAUDE.md

Beat Saber Quest 3 mod (mod ID: `apple-music-search`) targeting BS 1.40.8.
All builds happen via GitHub Actions CI only — **no local builds**.

## What the mod does

- Adds a "BeatCrate" button to the Beat Saber main menu
- Opens `AppleMusicFlowCoordinator` → `LibraryViewController` (Songs / Albums / Playlists)
- Playlist drill-down → `PlaylistTracksViewController`
- Tap a song → `BeatSaverResultsViewController` (BeatSaver search + download)
- Mod settings entry: MUT paste field (Apple Music), Spotify greyed out
- **No external server** — hits `amp-api-edge.music.apple.com` directly
- JWT scraped from `music.apple.com` JS bundle at first use, cached in config
- MUT (Media User Token) entered by user in Mod Settings, stored in config

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
  Configuration.hpp    # getMut/setMut, getCachedJwt/setCachedJwt, getStorefront
  Log.hpp              # AMS_LOG / AMS_ERROR macros
  UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp
  UI/ViewControllers/  # one .hpp per VC
  AppleMusic/AppleMusicClient.hpp   # hits Apple Music API directly
  BeatSaver/BeatSaverClient.hpp
src/
  main.cpp
  Configuration.cpp
  UI/FlowCoordinators/AppleMusicFlowCoordinator.cpp
  UI/ViewControllers/  # one .cpp per VC
  AppleMusic/AppleMusicClient.cpp
  BeatSaver/BeatSaverClient.cpp
HANDOFF.md       # session-by-session progress log — read this too
```

## Critical API rules (hard-won from build/runtime failures)

### custom-types
- Macro: `DECLARE_CLASS_CODEGEN(ns, Name, Base) { body };` — body is inside braces, NOT a 4th arg
- `DECLARE_INSTANCE_FIELD` requires the field type to be **fully included**, not forward-declared
- Cell-selected BSML bindings pass **only `int index`** — no `UnityW<HMUI::TableView>` param

### BSML
- `BSML::parse_and_construct(asset, transform, this)` — from `bsml/shared/BSML.hpp` (NOT `BSML-Lite.hpp`)
- `BSML::Helpers::GetMainFlowCoordinator()` — `bsml/shared/Helpers/getters.hpp`
- `BSML::Helpers::CreateViewController<T*>()` / `CreateFlowCoordinator<T*>()` — `bsml/shared/Helpers/creation.hpp`
- `BSML::Helpers::GetComponentInChildren` **does not exist** — use `go->GetComponentInChildren<T>(false)`

### HMUI / FlowCoordinator — AnimationType/AnimationDirection

**Use UNDERSCORE form for cordl 4008 — nested form compiles but crashes at runtime in `ReplaceTopViewController`:**
```cpp
using AT = HMUI::ViewController_AnimationType;        // CORRECT for cordl 4008
using AD = HMUI::ViewController_AnimationDirection;   // CORRECT for cordl 4008
// NOT: HMUI::ViewController::AnimationType (nested — runtime "Argcount mismatch" crash)
```

### FlowCoordinator presentation — must match this pattern exactly

Wrong pattern causes the FC to overlap the main menu instead of replacing it:
```cpp
// OPEN — use YoungestChildFlowCoordinatorOrSelf, Horizontal, explicit UnityW cast
auto _w = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
HMUI::FlowCoordinator* parent = _w;   // explicit cast — auto* fails (UnityW<FC> ≠ FC*)
if (parent) parent->PresentFlowCoordinator(fc.ptr(), nullptr,
    HMUI::ViewController_AnimationDirection::Horizontal, false, false);

// DISMISS — use _parentFlowCoordinator, Horizontal
_parentFlowCoordinator->DismissFlowCoordinator(this,
    HMUI::ViewController_AnimationDirection::Horizontal, nullptr, false);
```

DO NOT use `GetMainFlowCoordinator()` as the presenter — causes overlay bug.
DO NOT use `Vertical` — causes overlay bug.

### Other FlowCoordinator rules
- `ReplaceTopViewController(vc, nullptr, AT::In, AD::Horizontal)` — 4-arg form
- `SetTitle("Name", AT::In)` where AT = `HMUI::ViewController_AnimationType`
- `showBackButton = true` must be set before `ProvideInitialViewControllers`
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
- For string responses use `WebUtils::StringResponse`
- Custom headers via `opts.headers = std::unordered_map<std::string, std::string>{...}`

### Raw string literals — delimiter clash
C++ raw string `R"(...)"` is terminated by `)"`. If regex contains `)"`, use escaped string:
```cpp
// WRONG — terminates early:  R"(crossorigin src="(/assets/index\.[^"]+\.js)")"
// RIGHT:  "crossorigin src=\"(/assets/index\\.[^\"]+\\.js)\""
```

### Scotland2 entry points
```cpp
// main.hpp defines:
#define MOD_EXPORT __attribute__((visibility("default")))
#define MOD_EXTERN_FUNC extern "C" MOD_EXPORT
// Without MOD_EXPORT, -fvisibility=hidden hides setup/late_load from Scotland2
```

### Namespaces
- ViewControllers live in `AppleMusicSearch::UI::ViewControllers` (split to avoid collision with `UnityEngine::UI`)
- FlowCoordinators live in `AppleMusicSearch::UI::FlowCoordinators`

### Asset embedding
`cmake/assets.cmake` generates `build/assets.hpp` at configure time via a Python `execute_process`.
Each `.bsml` file in `assets/` becomes `IncludedAssets::<FileName>_bsml` (dots/hyphens → underscores).
`#include "assets.hpp"` resolves because `CMAKE_CURRENT_BINARY_DIR` is added to the include path.

## Apple Music API (no developer token)

- **Base URL**: `https://amp-api-edge.music.apple.com`
- **JWT**: scraped from `music.apple.com` HTML → JS bundle. Cached in config.
- **MUT**: user pastes from Mod Settings clipboard button. Stored in config.
- **Required headers**: `Authorization: Bearer {jwt}`, `Music-User-Token: {mut}`, `Origin: https://music.apple.com`
- **Storefront**: defaults `"us"`, stored in config key `storefront`
- Library endpoints need MUT. Catalog search does not.
- See `HANDOFF.md` for API endpoint details and `docs/apple-music-api.md` for full reference.

## CI workflow

Push to `main` → `.github/workflows/build.yml`:
1. Install cmake + ninja
2. Download QPM, restore dependencies
3. Download NDK r27d
4. `cmake -B build` (configure + asset generation)
5. `cmake --build build --parallel`
6. Package `BeatCrate-v<version>.zip` → upload artifact + create GitHub Release
