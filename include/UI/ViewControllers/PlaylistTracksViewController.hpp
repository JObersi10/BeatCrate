#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>
#include <string>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, PlaylistTracksViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onTrackCellSelected, UnityW<HMUI::TableView> table, int index);
    DECLARE_INSTANCE_METHOD(void, onBackClicked);

public:
    void loadPlaylist(const AMPlaylist& playlist);

    bool    get_isLoading();     void set_isLoading(bool v);
    StringW get_playlistName();  void set_playlistName(StringW v);
    StringW get_statusText();    void set_statusText(StringW v);

private:
    AMPlaylist _playlist;
    std::vector<AMSong> _tracks;
    bool _isLoading = false;
    std::string _playlistName;
    std::string _statusText;
};
