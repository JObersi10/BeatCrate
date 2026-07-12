#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/ViewControllers/SearchViewController.hpp"
#include "UI/ViewControllers/PlaylistTracksViewController.hpp"
#include "UI/ViewControllers/BeatSaverResultsViewController.hpp"
#include <string>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::FlowCoordinators, AppleMusicFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed,
        HMUI::ViewController* topViewController);

    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::LibraryViewController>,          _library);
    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::SearchViewController>,           _search);
    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::PlaylistTracksViewController>,   _playlistTracks);
    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::BeatSaverResultsViewController>, _bsResults);

public:
    void showPlaylistTracks(const AMPlaylist& playlist);
    void showBeatSaverResults(const std::string& title, const std::string& artist);
    void popToLibrary();
    void popToPreviousView();
    void reset();
};
