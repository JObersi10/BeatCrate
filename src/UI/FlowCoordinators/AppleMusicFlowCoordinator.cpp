#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/ViewControllers/SearchViewController.hpp"
#include "UI/ViewControllers/PlaylistTracksViewController.hpp"
#include "UI/ViewControllers/BeatSaverResultsViewController.hpp"
#include "Log.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/Helpers/getters.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::FlowCoordinators, AppleMusicFlowCoordinator);

namespace AppleMusicSearch::UI::FlowCoordinators {

using namespace AppleMusicSearch::UI::ViewControllers;
using AT = HMUI::ViewController::AnimationType;
using AD = HMUI::ViewController::AnimationDirection;

#define REPLACE(vc, type, dir) \
    ReplaceTopViewController(vc, nullptr, AT::type, AD::dir)

void AppleMusicFlowCoordinator::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;

    SetTitle("Music Search", AT::In);
    showBackButton = true;

    _serviceSelect  = BSML::Helpers::CreateViewController<ServiceSelectViewController*>();
    _library        = BSML::Helpers::CreateViewController<LibraryViewController*>();
    _search         = BSML::Helpers::CreateViewController<SearchViewController*>();
    _playlistTracks = BSML::Helpers::CreateViewController<PlaylistTracksViewController*>();
    _bsResults      = BSML::Helpers::CreateViewController<BeatSaverResultsViewController*>();

    ProvideInitialViewControllers(_serviceSelect, nullptr, nullptr, nullptr, nullptr);
}

void AppleMusicFlowCoordinator::BackButtonWasPressed(HMUI::ViewController*) {
    _parentFlowCoordinator->DismissFlowCoordinator(this, AD::Vertical, nullptr, false);
}

void AppleMusicFlowCoordinator::showAppleMusicHome() {
    SetTitle("Apple Music", AT::In);
    REPLACE(_library, In, Vertical);
}

void AppleMusicFlowCoordinator::showPlaylistTracks(const AMPlaylist& playlist) {
    _playlistTracks->loadPlaylist(playlist);
    REPLACE(_playlistTracks, In, Horizontal);
}

void AppleMusicFlowCoordinator::showBeatSaverResults(const std::string& title,
                                                      const std::string& artist) {
    _bsResults->searchFor(title, artist);
    REPLACE(_bsResults, In, Horizontal);
}

void AppleMusicFlowCoordinator::popToAppleMusicHome() {
    REPLACE(_library, Out, Horizontal);
}

void AppleMusicFlowCoordinator::popToPreviousView() {
    REPLACE(_library, Out, Horizontal);
}

void AppleMusicFlowCoordinator::reset() {
    _parentFlowCoordinator->DismissFlowCoordinator(this, AD::Vertical, nullptr, false);
}

}
