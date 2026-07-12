#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>
#include <string>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, LibraryViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onSongsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onAlbumsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onPlaylistsTabClicked);
    DECLARE_INSTANCE_METHOD(void, onRefreshClicked);
    DECLARE_INSTANCE_METHOD(void, onSongCellSelected, int index);
    DECLARE_INSTANCE_METHOD(void, onAlbumCellSelected, int index);
    DECLARE_INSTANCE_METHOD(void, onPlaylistCellSelected, int index);

public:
    void refresh();

    bool    get_isLoading();    void set_isLoading(bool v);
    StringW get_statusText();   void set_statusText(StringW v);

private:
    std::vector<AMSong>     _songs;
    std::vector<AMAlbum>    _albums;
    std::vector<AMPlaylist> _playlists;
    int  _activeTab   = 0;
    bool _isLoading   = false;
    std::string _statusText;

    void refreshSongList();
    void refreshAlbumList();
    void refreshPlaylistList();
};
