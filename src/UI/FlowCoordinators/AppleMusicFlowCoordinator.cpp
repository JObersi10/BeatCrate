#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/ViewControllers/SearchViewController.hpp"
#include "UI/ViewControllers/PlaylistTracksViewController.hpp"
#include "UI/ViewControllers/BeatSaverResultsViewController.hpp"
#include "Log.hpp"

#include "bsml/shared/BSML-Lite.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/Helpers/getters.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::FlowCoordinators, AppleMusicFlowCoordinator);

namespace AppleMusicSearch::UI::FlowCoordinators {

using namespace AppleMusicSearch::UI::ViewControllers;

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

    ProvideInitialViewControllers(_serviceSelect, nullptr, nullptr, nullptr, nullptr);
}

void AppleMusicFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* /*top*/) {
    _parentFlowCoordinator->DismissFlowCoordinator(this,
        HMUI::ViewController_AnimationDirection::Vertical, nullptr, false);
}

void AppleMusicFlowCoordinator::showAppleMusicHome() {
    SetTitle("Apple Music", HMUI::ViewController::AnimationType::In);
    ReplaceTopViewController(_library, nullptr,
        HMUI::ViewController::AnimationType::In,
        HMUI::ViewController_AnimationDirection::Vertical);
    SetLeftScreenViewController(nullptr, HMUI::ViewController::AnimationType::Out);
    SetRightScreenViewController(nullptr, HMUI::ViewController::AnimationType::Out);
}

void AppleMusicFlowCoordinator::showPlaylistTracks(const AMPlaylist& playlist) {
    _playlistTracks->loadPlaylist(playlist);
    ReplaceTopViewController(_playlistTracks, nullptr,
        HMUI::ViewController::AnimationType::In,
        HMUI::ViewController_AnimationDirection::Horizontal);
}

void AppleMusicFlowCoordinator::showBeatSaverResults(const std::string& title,
                                                      const std::string& artist) {
    _bsResults->searchFor(title, artist);
    ReplaceTopViewController(_bsResults, nullptr,
        HMUI::ViewController::AnimationType::In,
        HMUI::ViewController_AnimationDirection::Horizontal);
}

void AppleMusicFlowCoordinator::popToAppleMusicHome() {
    ReplaceTopViewController(_library, nullptr,
        HMUI::ViewController::AnimationType::Out,
        HMUI::ViewController_AnimationDirection::Horizontal);
}

void AppleMusicFlowCoordinator::popToPreviousView() {
    ReplaceTopViewController(_library, nullptr,
        HMUI::ViewController::AnimationType::Out,
        HMUI::ViewController_AnimationDirection::Horizontal);
}

void AppleMusicFlowCoordinator::reset() {
    _parentFlowCoordinator->DismissFlowCoordinator(this,
        HMUI::ViewController_AnimationDirection::Vertical, nullptr, false);
}

}
