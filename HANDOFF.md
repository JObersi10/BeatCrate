# BeatCrate — Handoff

## Current State (v1.1.0)

Build: compiles clean 14/14. Mod installs and shows "BeatCrate" button on main menu.

**What works:**
- BeatCrate button appears on main menu
- Clicking it opens `AppleMusicFlowCoordinator` → `LibraryViewController`
- Mod settings entry registered under "BeatCrate" (in-game Mod Settings)
- Settings screen has MUT paste field + Spotify "Coming Soon"
- JWT is scraped from `music.apple.com` at first API call and cached in config
- Library tabs: Songs / Albums / Playlists (fetches from `amp-api-edge.music.apple.com`)
- Playlist drill-down → `PlaylistTracksViewController`
- Tap song → `BeatSaverResultsViewController` (BeatSaver search)
- Back button dismisses cleanly

**Known unknowns (not yet tested in-headset):**
- Whether the JWT scrape from music.apple.com actually works on Quest network
- Whether `UnityEngine::GUIUtility::get_systemCopyBuffer()` works for clipboard paste on Quest
- Whether the `amp-api-edge` API calls succeed (HTTPS + correct headers)
- 100-item cap on library (no pagination yet)

---

## Architecture

```
main.cpp
  late_load() → BSML::Init → AutoRegister → RegisterMenuButton("BeatCrate") → openBeatCrate()
                                           → RegisterSettingsMenu<ServiceSelectViewController>

openBeatCrate():
  CreateFlowCoordinator<AppleMusicFlowCoordinator>  (cached SafePtrUnity)
  YoungestChildFlowCoordinatorOrSelf()->PresentFlowCoordinator(..., Horizontal, false, false)

AppleMusicFlowCoordinator::DidActivate():
  CreateViewController × 4 (Library, Search, PlaylistTracks, BeatSaverResults)
  ProvideInitialViewControllers(_library, ...)

BackButtonWasPressed():
  _parentFlowCoordinator->DismissFlowCoordinator(this, Horizontal, nullptr, false)
```

### Apple Music API (no developer token)

- **JWT**: scraped from `music.apple.com` HTML → JS bundle regex at startup. Cached in config JSON.
- **MUT**: user pastes from clipboard in Mod Settings. Stored in config JSON.
- **Base URL**: `https://amp-api-edge.music.apple.com`
- **Headers**: `Authorization: Bearer {jwt}`, `Music-User-Token: {mut}`, `Origin: https://music.apple.com`
- **Storefront**: defaults to `"us"` (stored in config, not yet auto-detected)

### Config keys (JSON at `/sdcard/ModData/.../Mods/apple-music-search/apple-music-search.json`)
```json
{ "mut": "...", "jwt": "...", "storefront": "us" }
```

---

## Critical patterns (hard-won)

### Presentation (must match this exactly or it overlaps the menu)
```cpp
// open
auto _w = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
HMUI::FlowCoordinator* parent = _w;   // explicit cast — auto* fails
parent->PresentFlowCoordinator(fc.ptr(), nullptr, HMUI::ViewController_AnimationDirection::Horizontal, false, false);

// dismiss
_parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController_AnimationDirection::Horizontal, nullptr, false);
```

### AnimationType/AnimationDirection — use UNDERSCORE form for cordl 4008
```cpp
using AT = HMUI::ViewController_AnimationType;        // NOT HMUI::ViewController::AnimationType
using AD = HMUI::ViewController_AnimationDirection;   // NOT HMUI::ViewController::AnimationDirection
```
Using nested namespace form compiles but causes runtime "Argcount mismatch" crash in `ReplaceTopViewController`.

### ReplaceTopViewController — 4-arg form
```cpp
ReplaceTopViewController(vc, nullptr, AT::In, AD::Horizontal);
```

### Raw string literals with `")` inside
C++ raw string `R"(...)"` is terminated by `)"`. If the regex contains `)"`, use escaped string instead:
```cpp
// WRONG:  std::regex scriptRe(R"(crossorigin src="(/assets/index\.[^"]+\.js)")");
// RIGHT:  std::regex scriptRe("crossorigin src=\"(/assets/index\\.[^\"]+\\.js)\"");
```

### WebUtils headers
```cpp
WebUtils::URLOptions opts(url);
opts.headers = std::unordered_map<std::string, std::string>{
    {"Authorization", "Bearer " + jwt},
    {"Music-User-Token", mut},
    {"Origin", "https://music.apple.com"}
};
auto resp = std::make_shared<WebUtils::JsonResponse>(WebUtils::Get<WebUtils::JsonResponse>(opts));
```

### UnityW cast
`YoungestChildFlowCoordinatorOrSelf()` returns `UnityW<FlowCoordinator>`. Must cast explicitly:
```cpp
HMUI::FlowCoordinator* raw = _w;   // NOT auto*
```

---

## Files changed this session (v1.1.0)

| File | What changed |
|------|-------------|
| `src/AppleMusic/AppleMusicClient.cpp` | Full rewrite — hits Apple Music API directly, scrapes JWT |
| `include/AppleMusic/AppleMusicClient.hpp` | Removed serverAddress, added withJwt |
| `src/Configuration.cpp` | Removed serverAddress, added mut/jwt/storefront |
| `include/Configuration.hpp` | Same |
| `src/UI/ViewControllers/ServiceSelectViewController.cpp` | Now a settings VC (MUT paste) |
| `include/UI/ViewControllers/ServiceSelectViewController.hpp` | Same |
| `assets/ServiceSelectViewController.bsml` | MUT field + paste button + Spotify greyed |
| `src/UI/FlowCoordinators/AppleMusicFlowCoordinator.cpp` | Removed _serviceSelect, Library is initial VC |
| `include/UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp` | Same |
| `src/UI/ViewControllers/LibraryViewController.cpp` | MUT check before fetch |
| `src/main.cpp` | openBeatCrate: YoungestChild + Horizontal |

---

## What's next

- [ ] Test JWT scrape + API calls in-headset (logcat will show results)
- [ ] If JWT scrape fails: consider hardcoding a long-lived JWT or adding a manual JWT field in settings alongside MUT
- [ ] Storefront auto-detect: hit `/v1/me/storefront` with JWT+MUT and cache result
- [ ] Pagination: currently fetches only first 100 items per category
- [ ] Spotify tab (greyed out, future)
- [ ] Search tab UI (SearchViewController exists but isn't wired into the tab bar yet — currently only accessible via the FC's `_search` VC which is never shown)
