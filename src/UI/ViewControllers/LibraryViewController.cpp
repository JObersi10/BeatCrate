#include "UI/ViewControllers/LibraryViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, LibraryViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void LibraryViewController::DidActivate(bool firstActivation, bool, bool) {
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
        _playlists = std::move(playlists);
        if (_activeTab == 2) refreshPlaylistList();
        set_isLoading(false);
        set_statusText("");
    });
}

static BSML::CustomListTableData* findList(UnityEngine::GameObject* go) {
    return go->GetComponentInChildren<BSML::CustomListTableData*>(false);
}

void LibraryViewController::refreshSongList() {
    auto* list = findList(get_gameObject()); if (!list) return;
    list->data.clear();
    for (auto& s : _songs)
        list->data.push_back(BSML::CustomCellInfo::construct(StringW(s.title), StringW(s.artist), nullptr));
    list->tableView->ReloadData();
}
void LibraryViewController::refreshAlbumList() {
    auto* list = findList(get_gameObject()); if (!list) return;
    list->data.clear();
    for (auto& a : _albums)
        list->data.push_back(BSML::CustomCellInfo::construct(
            StringW(a.title), StringW(a.artist + " · " + std::to_string(a.trackCount) + " tracks"), nullptr));
    list->tableView->ReloadData();
}
void LibraryViewController::refreshPlaylistList() {
    auto* list = findList(get_gameObject()); if (!list) return;
    list->data.clear();
    for (auto& p : _playlists)
        list->data.push_back(BSML::CustomCellInfo::construct(
            StringW(p.name), StringW(std::to_string(p.trackCount) + " songs"), nullptr));
    list->tableView->ReloadData();
}

void LibraryViewController::onSongsTabClicked()     { _activeTab = 0; refreshSongList(); }
void LibraryViewController::onAlbumsTabClicked()    { _activeTab = 1; refreshAlbumList(); }
void LibraryViewController::onPlaylistsTabClicked() { _activeTab = 2; refreshPlaylistList(); }
void LibraryViewController::onRefreshClicked()      { refresh(); }

static AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator* getFC() {
    auto* child = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
    return il2cpp_utils::try_cast<AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator>(child).value_or(nullptr);
}

void LibraryViewController::onSongCellSelected(int index) {
    if (index < 0 || index >= (int)_songs.size()) return;
    auto& s = _songs[index];
    if (auto fc = getFC()) fc->showBeatSaverResults(s.title, s.artist);
}
void LibraryViewController::onAlbumCellSelected(int /*index*/) {}
void LibraryViewController::onPlaylistCellSelected(int index) {
    if (index < 0 || index >= (int)_playlists.size()) return;
    if (auto fc = getFC()) fc->showPlaylistTracks(_playlists[index]);
}

bool    LibraryViewController::get_isLoading()            { return _isLoading; }
void    LibraryViewController::set_isLoading(bool v)      { _isLoading = v; }
StringW LibraryViewController::get_statusText()           { return StringW(_statusText); }
void    LibraryViewController::set_statusText(StringW v)  { _statusText = static_cast<std::string>(v); }

}
