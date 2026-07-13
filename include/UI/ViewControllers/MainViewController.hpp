#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "AppleMusic/Models.hpp"
#include "BeatSaver/BeatSaverClient.hpp"
#include <vector>
#include <string>
#include <atomic>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, MainViewController, HMUI::ViewController) {
    DECLARE_CTOR(ctor);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    // Left panel
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>,              backToPlaylistsButton_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*,                      leftColumnTitleTextView_);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalLayoutGroup>, leftLoadingContainer_);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::VerticalLayoutGroup>,   leftErrorContainer_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*,                      leftErrorTextView_);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::VerticalLayoutGroup>,   leftStatusContainer_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*,                      leftStatusTextView_);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::CustomListTableData>,            playlistListView_);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::CustomListTableData>,            trackListView_);

    DECLARE_INSTANCE_METHOD(void, onPlaylistSelected, int index);
    DECLARE_INSTANCE_METHOD(void, onTrackSelected,    int index);
    DECLARE_INSTANCE_METHOD(void, onBackToPlaylistsClicked);

    // Center panel
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalLayoutGroup>, mapLoadingContainer_);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::VerticalLayoutGroup>,   mapStatusContainer_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*,                      mapStatusTextView_);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::CustomListTableData>,            mapListView_);

    DECLARE_INSTANCE_METHOD(void, onMapSelected, int index);

    // Right panel
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, previewMapNameTextView_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, previewUploaderTextView_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, previewDurationTextView_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, previewDiffsTextView_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, downloadStatusTextView_);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, downloadButton_);

    DECLARE_INSTANCE_METHOD(void, onDownloadClicked);

private:
    std::vector<AppleMusicSearch::AMPlaylist> _playlists;
    std::vector<AppleMusicSearch::AMSong>     _tracks;
    std::vector<AppleMusicSearch::BSMap>      _maps;

    int  _selectedMapIndex   = -1;
    bool _showingTracks      = false;
    std::atomic<bool> _isDownloading{false};

    void loadPlaylists();
    void loadTracksForPlaylist(const std::string& playlistId, const std::string& playlistName);
    void searchBeatSaver(const std::string& title, const std::string& artist);

    void showLeftLoading();
    void showLeftPlaylists();
    void showLeftTracks();
    void showLeftError(const std::string& msg);
    void showLeftStatus(const std::string& msg);

    void showMapLoading();
    void showMapList();
    void showMapStatus(const std::string& msg);

    void clearMapPreview();
    void showMapPreview(const AppleMusicSearch::BSMap& map);
};
