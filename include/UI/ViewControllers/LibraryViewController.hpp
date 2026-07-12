#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, LibraryViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    // Called by the flow coordinator to push fresh data
    void setData(
        const std::vector<AMSong>& recentSongs,
        const std::vector<AMAlbum>& albums,
        const std::vector<AMPlaylist>& playlists
    );

    DECLARE_INSTANCE_METHOD(void, onSongsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onAlbumsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onPlaylistsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onRefreshClicked);

    DECLARE_BSML_PROPERTY(bool, isLoading);
    DECLARE_BSML_PROPERTY(StringW, statusText);

    // index of tapped song/album/playlist cell — handled via table callbacks
    void onSongCellSelected(int index);
    void onAlbumCellSelected(int index);
    void onPlaylistCellSelected(int index);

private:
    std::vector<AMSong>     _songs;
    std::vector<AMAlbum>    _albums;
    std::vector<AMPlaylist> _playlists;
    int _activeTab = 0; // 0=songs 1=albums 2=playlists
)
