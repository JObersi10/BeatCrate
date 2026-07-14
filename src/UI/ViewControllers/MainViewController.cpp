#include "UI/ViewControllers/MainViewController.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "BeatSaver/BeatSaverClient.hpp"
#include "Log.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, MainViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void MainViewController::ctor() {
    INVOKE_BASE_CTOR(classof(HMUI::ViewController*));
    _selectedMapIndex = -1;
    _showingTracks    = false;
    _section          = Section::Playlists;
}

void MainViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::MainViewController_bsml, get_transform(), this);
}

void MainViewController::PostParse() {
    // Hide all overlays/buttons that start inactive
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(false);
    if (leftLoadingContainer_)  leftLoadingContainer_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)    leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)   leftStatusContainer_->get_gameObject()->set_active(false);
    if (mapLoadingContainer_)   mapLoadingContainer_->get_gameObject()->set_active(false);
    if (mapStatusContainer_)    mapStatusContainer_->get_gameObject()->set_active(false);
    if (mapListView_)           mapListView_->get_gameObject()->set_active(false);
    if (downloadButton_)        downloadButton_->get_gameObject()->set_active(false);
    if (searchContainer_)       searchContainer_->get_gameObject()->set_active(false);
    if (downloadStatusTextView_) downloadStatusTextView_->set_text("");
    clearMapPreview();

    UnityEngine::GameObject* go = get_gameObject();

    _playlistDS = go->GetComponent<AMPlaylistTableViewDataSource*>();
    if (!_playlistDS) _playlistDS = go->AddComponent<AMPlaylistTableViewDataSource*>();
    if (playlistListView_ && playlistListView_->tableView)
        playlistListView_->tableView->SetDataSource(
            reinterpret_cast<HMUI::TableView::IDataSource*>(_playlistDS.ptr()), true);

    _trackDS = go->GetComponent<AMTrackTableViewDataSource*>();
    if (!_trackDS) _trackDS = go->AddComponent<AMTrackTableViewDataSource*>();
    if (trackListView_ && trackListView_->tableView)
        trackListView_->tableView->SetDataSource(
            reinterpret_cast<HMUI::TableView::IDataSource*>(_trackDS.ptr()), true);

    _mapDS = go->GetComponent<BSMapTableViewDataSource*>();
    if (!_mapDS) _mapDS = go->AddComponent<BSMapTableViewDataSource*>();
    if (mapListView_ && mapListView_->tableView)
        mapListView_->tableView->SetDataSource(
            reinterpret_cast<HMUI::TableView::IDataSource*>(_mapDS.ptr()), true);

    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text("Playlists");
    showLeftPlaylists();
    loadPlaylists();
}

// ── Section switching ─────────────────────────────────────────────────────────

void MainViewController::onSectionPlaylists() {
    _section = Section::Playlists;
    _showingTracks = false;
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(false);
    if (searchContainer_)       searchContainer_->get_gameObject()->set_active(false);
    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text("Playlists");
    if (playlistListView_) playlistListView_->get_gameObject()->set_active(true);
    if (trackListView_)    trackListView_->get_gameObject()->set_active(false);
    showLeftLoading();
    loadPlaylists();
}

void MainViewController::onSectionSongs() {
    _section = Section::Songs;
    _showingTracks = false;
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(false);
    if (searchContainer_)       searchContainer_->get_gameObject()->set_active(false);
    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text("Library Songs");
    if (playlistListView_) playlistListView_->get_gameObject()->set_active(false);
    if (trackListView_)    trackListView_->get_gameObject()->set_active(true);
    showLeftLoading();
    loadLibrarySongs();
}

void MainViewController::onSectionSearch() {
    _section = Section::Search;
    _showingTracks = false;
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(false);
    if (searchContainer_)       searchContainer_->get_gameObject()->set_active(true);
    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text("Search");
    if (playlistListView_) playlistListView_->get_gameObject()->set_active(false);
    if (trackListView_)    trackListView_->get_gameObject()->set_active(false);
    if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)   leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)  leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::onRefreshClicked() {
    switch (_section) {
        case Section::Playlists:
            showLeftLoading();
            loadPlaylists();
            break;
        case Section::Songs:
            showLeftLoading();
            loadLibrarySongs();
            break;
        case Section::Search:
            if (!_searchTerm.empty()) {
                showLeftLoading();
                if (trackListView_) trackListView_->get_gameObject()->set_active(false);
                searchAppleMusic(_searchTerm);
            }
            break;
    }
}

void MainViewController::onSearchQueryChanged() {
    if (!_searchTerm.empty()) {
        showLeftLoading();
        if (trackListView_) trackListView_->get_gameObject()->set_active(false);
        searchAppleMusic(_searchTerm);
    }
}

void MainViewController::onSearchGoClicked() {
    // _searchTerm is set by set_searchTerm() when keyboard confirms
    if (!_searchTerm.empty()) {
        showLeftLoading();
        if (trackListView_) trackListView_->get_gameObject()->set_active(false);
        searchAppleMusic(_searchTerm);
    }
}

StringW MainViewController::get_searchTerm() { return StringW(_searchTerm); }
void    MainViewController::set_searchTerm(StringW v) { _searchTerm = static_cast<std::string>(v); }

// ── Left panel helpers ────────────────────────────────────────────────────────

void MainViewController::showLeftLoading() {
    if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(true);
    if (leftErrorContainer_)   leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)  leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftPlaylists() {
    _showingTracks = false;
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(false);
    if (playlistListView_)      playlistListView_->get_gameObject()->set_active(true);
    if (trackListView_)         trackListView_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)    leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)   leftStatusContainer_->get_gameObject()->set_active(false);
    if (leftLoadingContainer_)  leftLoadingContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftTracks() {
    _showingTracks = true;
    if (backToPlaylistsButton_) backToPlaylistsButton_->get_gameObject()->set_active(
        _section == Section::Playlists);
    if (trackListView_)         trackListView_->get_gameObject()->set_active(true);
    if (playlistListView_)      playlistListView_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)    leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)   leftStatusContainer_->get_gameObject()->set_active(false);
    if (leftLoadingContainer_)  leftLoadingContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftError(const std::string& msg) {
    if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)   leftErrorContainer_->get_gameObject()->set_active(true);
    if (leftErrorTextView_)    leftErrorTextView_->set_text(msg);
    if (leftStatusContainer_)  leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftStatus(const std::string& msg) {
    if (leftLoadingContainer_)  leftLoadingContainer_->get_gameObject()->set_active(false);
    if (leftErrorContainer_)    leftErrorContainer_->get_gameObject()->set_active(false);
    if (leftStatusContainer_)   leftStatusContainer_->get_gameObject()->set_active(true);
    if (leftStatusTextView_)    leftStatusTextView_->set_text(msg);
}

// ── Center panel helpers ──────────────────────────────────────────────────────

void MainViewController::showMapLoading() {
    if (mapLoadingContainer_) mapLoadingContainer_->get_gameObject()->set_active(true);
    if (mapStatusContainer_)  mapStatusContainer_->get_gameObject()->set_active(false);
    if (mapListView_)         mapListView_->get_gameObject()->set_active(false);
}

void MainViewController::showMapList() {
    if (mapLoadingContainer_) mapLoadingContainer_->get_gameObject()->set_active(false);
    if (mapStatusContainer_)  mapStatusContainer_->get_gameObject()->set_active(false);
    if (mapListView_)         mapListView_->get_gameObject()->set_active(true);
}

void MainViewController::showMapStatus(const std::string& msg) {
    if (mapLoadingContainer_) mapLoadingContainer_->get_gameObject()->set_active(false);
    if (mapStatusContainer_)  mapStatusContainer_->get_gameObject()->set_active(true);
    if (mapStatusTextView_)   mapStatusTextView_->set_text(msg);
    if (mapListView_)         mapListView_->get_gameObject()->set_active(false);
}

// ── Right panel helpers ───────────────────────────────────────────────────────

void MainViewController::clearMapPreview() {
    if (previewMapNameTextView_)  previewMapNameTextView_->set_text("Select a map");
    if (previewUploaderTextView_) previewUploaderTextView_->set_text("");
    if (previewDurationTextView_) previewDurationTextView_->set_text("");
    if (previewDiffsTextView_)    previewDiffsTextView_->set_text("");
    if (downloadButton_)          downloadButton_->get_gameObject()->set_active(false);
    if (downloadStatusTextView_)  downloadStatusTextView_->set_text("");
}

void MainViewController::showMapPreview(const AppleMusicSearch::BSMap& map) {
    if (previewMapNameTextView_)  previewMapNameTextView_->set_text(map.name);
    if (previewUploaderTextView_) previewUploaderTextView_->set_text("by " + map.uploaderName);
    if (previewDurationTextView_) {
        if (map.durationSecs > 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d:%02d", map.durationSecs / 60, map.durationSecs % 60);
            previewDurationTextView_->set_text(buf);
        } else {
            previewDurationTextView_->set_text("");
        }
    }
    if (previewDiffsTextView_) {
        std::string diffs;
        for (size_t i = 0; i < map.difficulties.size(); ++i) {
            if (i) diffs += "  ";
            diffs += map.difficulties[i];
        }
        previewDiffsTextView_->set_text(diffs);
    }
    if (downloadStatusTextView_) downloadStatusTextView_->set_text("");
    if (downloadButton_)         downloadButton_->get_gameObject()->set_active(true);
}

// ── Track list reload helper ──────────────────────────────────────────────────

void MainViewController::reloadTrackList(std::vector<AppleMusicSearch::AMSong> songs) {
    _tracks = std::move(songs);
    if (_trackDS) {
        _trackDS->tracks_ = _tracks;
        if (trackListView_ && trackListView_->tableView)
            trackListView_->tableView->ReloadData();
    }
}

// ── API calls ─────────────────────────────────────────────────────────────────

void MainViewController::loadPlaylists() {
    AppleMusicClient::instance().fetchLibraryPlaylists(
        [this](std::vector<AMPlaylist> playlists, std::string err) {
            if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(false);
            if (!err.empty()) { showLeftError(err); return; }
            if (playlists.empty()) { showLeftStatus("No playlists found.\nCheck MUT in Mod Settings."); return; }
            _playlists = std::move(playlists);
            if (_playlistDS) {
                _playlistDS->playlists_ = _playlists;
                if (playlistListView_ && playlistListView_->tableView)
                    playlistListView_->tableView->ReloadData();
            }
            showLeftPlaylists();
        });
}

void MainViewController::loadLibrarySongs() {
    AppleMusicClient::instance().fetchLibrarySongs(
        [this](std::vector<AMSong> songs, std::string err) {
            if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(false);
            if (!err.empty()) { showLeftError(err); return; }
            if (songs.empty()) { showLeftStatus("No songs in library.\nCheck MUT in Mod Settings."); return; }
            reloadTrackList(std::move(songs));
            showLeftTracks();
        });
}

void MainViewController::searchAppleMusic(const std::string& query) {
    AppleMusicClient::instance().search(query,
        [this](std::vector<AMSong> songs, std::string err) {
            if (leftLoadingContainer_) leftLoadingContainer_->get_gameObject()->set_active(false);
            if (!err.empty()) { showLeftError(err); return; }
            if (songs.empty()) { showLeftStatus("No songs found on Apple Music."); return; }
            reloadTrackList(std::move(songs));
            showLeftTracks();
        });
}

void MainViewController::loadTracksForPlaylist(const std::string& playlistId, const std::string& playlistName) {
    showLeftLoading();
    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text(playlistName);
    AppleMusicClient::instance().fetchPlaylistTracks(
        playlistId,
        [this](std::vector<AMSong> tracks, std::string err) {
            if (!err.empty()) { showLeftError(err); return; }
            if (tracks.empty()) { showLeftStatus("No tracks in this playlist."); return; }
            reloadTrackList(std::move(tracks));
            showLeftTracks();
        });
}

void MainViewController::searchBeatSaver(const std::string& title, const std::string& artist) {
    _selectedMapIndex = -1;
    _maps.clear();
    clearMapPreview();
    showMapLoading();
    BeatSaverClient::instance().search(
        title, artist,
        [this](std::vector<BSMap> maps, std::string err) {
            if (!err.empty()) { showMapStatus("Error: " + err); return; }
            if (maps.empty())  { showMapStatus("No maps found on BeatSaver."); return; }
            _maps = std::move(maps);
            if (_mapDS) {
                _mapDS->maps_ = _maps;
                if (mapListView_ && mapListView_->tableView)
                    mapListView_->tableView->ReloadData();
            }
            showMapList();
        });
}

// ── Callbacks ─────────────────────────────────────────────────────────────────

void MainViewController::onPlaylistSelected(UnityW<HMUI::TableView>, int index) {
    if (index < 0 || index >= (int)_playlists.size()) return;
    auto& pl = _playlists[index];
    loadTracksForPlaylist(pl.id, pl.name);
}

void MainViewController::onTrackSelected(UnityW<HMUI::TableView>, int index) {
    if (index < 0 || index >= (int)_tracks.size()) return;
    auto& t = _tracks[index];
    searchBeatSaver(t.title, t.artist);
}

void MainViewController::onBackToPlaylistsClicked() {
    if (leftColumnTitleTextView_) leftColumnTitleTextView_->set_text("Playlists");
    _tracks.clear();
    if (_trackDS) { _trackDS->tracks_.clear(); if (trackListView_ && trackListView_->tableView) trackListView_->tableView->ReloadData(); }
    showLeftPlaylists();
    _maps.clear();
    if (_mapDS) { _mapDS->maps_.clear(); if (mapListView_ && mapListView_->tableView) mapListView_->tableView->ReloadData(); }
    showMapStatus("");
    clearMapPreview();
}

void MainViewController::onMapSelected(UnityW<HMUI::TableView>, int index) {
    if (index < 0 || index >= (int)_maps.size()) return;
    _selectedMapIndex = index;
    showMapPreview(_maps[index]);
}

void MainViewController::onDownloadClicked() {
    if (_selectedMapIndex < 0 || _selectedMapIndex >= (int)_maps.size()) return;
    if (_isDownloading.load()) return;
    _isDownloading = true;
    if (downloadButton_)         downloadButton_->get_gameObject()->set_active(false);
    if (downloadStatusTextView_) downloadStatusTextView_->set_text("Downloading...");
    auto map = _maps[_selectedMapIndex];
    BeatSaverClient::instance().downloadMap(
        map,
        [this](bool ok, std::string err) {
            _isDownloading = false;
            if (downloadButton_)         downloadButton_->get_gameObject()->set_active(true);
            if (downloadStatusTextView_) downloadStatusTextView_->set_text(ok ? "Downloaded!" : "Failed: " + err);
        });
}

} // namespace AppleMusicSearch::UI::ViewControllers
