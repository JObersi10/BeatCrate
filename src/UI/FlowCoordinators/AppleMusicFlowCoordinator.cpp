#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/ViewControllers/SearchViewController.hpp"
#include "UI/ViewControllers/PlaylistTracksViewController.hpp"
#include "UI/ViewControllers/BeatSaverResultsViewController.hpp"
#include "Log.hpp"

#include "bsml/shared/BSML-Lite.hpp"
#include "Zenject/DiContainer.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, AppleMusicFlowCoordinator);

namespace AppleMusicSearch::UI {

// ── Lifecycle ────────────────────────────────────────────────────────────────

void AppleMusicFlowCoordinator::DidActivate(bool firstActivation,
                                            bool addedToHierarchy,
                                            bool screenSystemEnabling) {
    if (!firstActivation) return;

    SetTitle("Music Search", HMUI::ViewController::AnimationType::In);
    showBackButton = true;

    _serviceSelect  = BSML::Helpers::CreateViewController<ServiceSelectViewController*>();
    _library        = BSML::Helpers::CreateViewController<LibraryViewController*>();
    _search         = BSML::Helpers::CreateViewController<SearchViewController*>();
    _playlistTracks = BSML::Helpers::CreateViewController<PlaylistTracksViewController*>();
    _bsResults      = BSML::Helpers::CreateViewController<BeatSaverResultsViewController*>();

    // Start on the service-select screen (center only, no left/right panels)
    ProvideInitialViewControllers(_serviceSelect, nullptr, nullptr, nullptr, nullptr);
}

void AppleMusicFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* /*top*/) {
    parentFlowCoordinator->DismissFlowCoordinator(this,
        HMUI::ViewController::AnimationType::Out, nullptr, false);
}

// ── Navigation helpers called by child VCs ───────────────────────────────────

// Show Apple Music home: Library (left) + Search (right)
void AppleMusicFlowCoordinator::showAppleMusicHome() {
    SetTitle("Apple Music", HMUI::ViewController::AnimationType::In);
    ReplaceTopViewController(_library,  this, this, nullptr,
                             HMUI::ViewController::AnimationType::In);
    SetLeftScreenViewController(_library, HMUI::ViewController::AnimationType::In);
    SetRightScreenViewController(_search,  HMUI::ViewController::AnimationType::In);
}

// Drill into a playlist's tracks
void AppleMusicFlowCoordinator::showPlaylistTracks(const AMPlaylist& playlist) {
    _playlistTracks->loadPlaylist(playlist);
    PushViewController(_playlistTracks, HMUI::ViewController::AnimationType::In);
}

// Show BeatSaver results panel for a tapped song
void AppleMusicFlowCoordinator::showBeatSaverResults(const std::string& title,
                                                      const std::string& artist) {
    _bsResults->searchFor(title, artist);
    PushViewController(_bsResults, HMUI::ViewController::AnimationType::In);
}

void AppleMusicFlowCoordinator::popToAppleMusicHome() {
    PopViewController(HMUI::ViewController::AnimationType::Out);
}

void AppleMusicFlowCoordinator::popToPreviousView() {
    PopViewController(HMUI::ViewController::AnimationType::Out);
}

void AppleMusicFlowCoordinator::reset() {
    parentFlowCoordinator->DismissFlowCoordinator(this,
        HMUI::ViewController::AnimationType::Out, nullptr, false);
}

}
