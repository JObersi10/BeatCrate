#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, PlaylistTracksViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    void loadPlaylist(const AMPlaylist& playlist);

    DECLARE_BSML_PROPERTY(bool, isLoading);
    DECLARE_BSML_PROPERTY(StringW, playlistName);
    DECLARE_BSML_PROPERTY(StringW, statusText);

    void onTrackCellSelected(int index);
    DECLARE_INSTANCE_METHOD(void, onBackClicked);

private:
    AMPlaylist _playlist;
    std::vector<AMSong> _tracks;
)
