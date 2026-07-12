#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML-Lite.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, LibraryViewController);

namespace AppleMusicSearch::UI {

void LibraryViewController::DidActivate(bool firstActivation,
                                        bool addedToHierarchy,
                                        bool screenSystemEnabling) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::LibraryViewController_bsml,
                              get_transform(), this);
    refresh();
}

void LibraryViewController::refresh() {
    set_isLoading(true);
    set_statusText("Loading library…");

    auto& client = AppleMusicClient::instance();

    client.fetchLibrarySongs([this](std::vector<AMSong> songs, std::string err) {
        if (!err.empty()) { AMS_ERROR("Songs: {}", err); return; }
        _songs = std::move(songs);
        if (_activeTab == 0) refreshSongList();
    });

    client.fetchLibraryAlbums([this](std::vector<AMAlbum> albums, std::string err) {
        if (!err.empty()) { AMS_ERROR("Albums: {}", err); return; }
        _albums = std::move(albums);
        if (_activeTab == 1) refreshAlbumList();
    });

    client.fetchLibraryPlaylists([this](std::vector<AMPlaylist> playlists, std::string err) {
        if (!err.empty()) { AMS_ERROR("Playlists: {}", err); return; }
        _playlists = std::move(playlists); // already sorted A-Z by client
        if (_activeTab == 2) refreshPlaylistList();
        set_isLoading(false);
        set_statusText("");
    });
}

// ── Tab helpers ──────────────────────────────────────────────────────────────

void LibraryViewController::refreshSongList() {
    auto* list = BSML::Helpers::GetComponentInChildren<BSML::CustomListTableData*>(
                     get_gameObject(), "songList");
    if (!list) return;
    list->data.clear();
    for (auto& s : _songs) {
        auto cell = BSML::CustomCellInfo::construct(
            StringW(s.title), StringW(s.artist), nullptr);
        list->data.push_back(cell);
    }
    list->tableView->ReloadData();
}

void LibraryViewController::refreshAlbumList() {
    auto* list = BSML::Helpers::GetComponentInChildren<BSML::CustomListTableData*>(
                     get_gameObject(), "songList"); // reuse same list
    if (!list) return;
    list->data.clear();
    for (auto& a : _albums) {
        auto cell = BSML::CustomCellInfo::construct(
            StringW(a.title), StringW(a.artist + " · " + std::to_string(a.trackCount) + " tracks"),
            nullptr);
        list->data.push_back(cell);
    }
    list->tableView->ReloadData();
}

void LibraryViewController::refreshPlaylistList() {
    auto* list = BSML::Helpers::GetComponentInChildren<BSML::CustomListTableData*>(
                     get_gameObject(), "songList");
    if (!list) return;
    list->data.clear();
    for (auto& p : _playlists) {
        auto cell = BSML::CustomCellInfo::construct(
            StringW(p.name),
            StringW(std::to_string(p.trackCount) + " songs"),
            nullptr);
        list->data.push_back(cell);
    }
    list->tableView->ReloadData();
}

// ── Tab buttons ──────────────────────────────────────────────────────────────

void LibraryViewController::onSongsTabClicked()     { _activeTab = 0; refreshSongList(); }
void LibraryViewController::onAlbumsTabClicked()    { _activeTab = 1; refreshAlbumList(); }
void LibraryViewController::onPlaylistsTabClicked() { _activeTab = 2; refreshPlaylistList(); }
void LibraryViewController::onRefreshClicked()      { refresh(); }

// ── Cell selection ───────────────────────────────────────────────────────────

void LibraryViewController::onSongCellSelected(int index) {
    if (index < 0 || index >= (int)_songs.size()) return;
    auto& s = _songs[index];
    auto fc = BSML::Helpers::GetMainFlowCoordinator()
                  ->YoungestChildFlowCoordinatorOrSelf()
                  ->TryCast<AppleMusicFlowCoordinator>();
    if (fc) fc->showBeatSaverResults(s.title, s.artist);
}

void LibraryViewController::onAlbumCellSelected(int /*index*/) {
    // Albums: no BeatSaver search — nothing to do for now
}

void LibraryViewController::onPlaylistCellSelected(int index) {
    if (index < 0 || index >= (int)_playlists.size()) return;
    auto fc = BSML::Helpers::GetMainFlowCoordinator()
                  ->YoungestChildFlowCoordinatorOrSelf()
                  ->TryCast<AppleMusicFlowCoordinator>();
    if (fc) fc->showPlaylistTracks(_playlists[index]);
}

// ── BSML property stubs ──────────────────────────────────────────────────────

bool  LibraryViewController::get_isLoading()       { return _isLoading; }
void  LibraryViewController::set_isLoading(bool v) { _isLoading = v; }
StringW LibraryViewController::get_statusText()       { return StringW(_statusText); }
void    LibraryViewController::set_statusText(StringW v) { _statusText = static_cast<std::string>(v); }

}
