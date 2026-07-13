#include "UI/ViewControllers/MainViewController.hpp"
#include "UI/TableViewDataSources/AMPlaylistTableViewDataSource.hpp"
#include "UI/TableViewDataSources/AMTrackTableViewDataSource.hpp"
#include "UI/TableViewDataSources/BSMapTableViewDataSource.hpp"
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
    _selectedMapIndex = -1;
    _showingTracks    = false;
}

void MainViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::MainViewController_bsml, get_transform(), this);
}

void MainViewController::PostParse() {
    // Hide back-to-playlists button initially (we start on the playlist view)
    backToPlaylistsButton_->get_gameObject()->set_active(false);

    // Hide all overlays initially
    leftLoadingContainer_->get_gameObject()->set_active(false);
    leftErrorContainer_->get_gameObject()->set_active(false);
    leftStatusContainer_->get_gameObject()->set_active(false);
    mapLoadingContainer_->get_gameObject()->set_active(false);
    mapStatusContainer_->get_gameObject()->set_active(false);
    mapListView_->get_gameObject()->set_active(false);
    downloadButton_->get_gameObject()->set_active(false);
    downloadStatusTextView_->set_text("");

    clearMapPreview();

    // Set up playlist data source
    auto* playlistDS = get_gameObject()->GetComponent<AMPlaylistTableViewDataSource*>();
    if (!playlistDS) {
        playlistDS = get_gameObject()->AddComponent<AMPlaylistTableViewDataSource*>();
    }
    playlistListView_->tableView->SetDataSource(
        reinterpret_cast<HMUI::TableView::IDataSource*>(playlistDS), true);

    // Set up track data source
    auto* trackDS = get_gameObject()->GetComponent<AMTrackTableViewDataSource*>();
    if (!trackDS) {
        trackDS = get_gameObject()->AddComponent<AMTrackTableViewDataSource*>();
    }
    trackListView_->tableView->SetDataSource(
        reinterpret_cast<HMUI::TableView::IDataSource*>(trackDS), true);

    // Set up map data source
    auto* mapDS = get_gameObject()->GetComponent<BSMapTableViewDataSource*>();
    if (!mapDS) {
        mapDS = get_gameObject()->AddComponent<BSMapTableViewDataSource*>();
    }
    mapListView_->tableView->SetDataSource(
        reinterpret_cast<HMUI::TableView::IDataSource*>(mapDS), true);

    // Show playlist view and load playlists
    showLeftPlaylists();
    leftColumnTitleTextView_->set_text("Apple Music");
    loadPlaylists();
}

// ─── Left panel helpers ───────────────────────────────────────────────────────

void MainViewController::showLeftLoading() {
    leftLoadingContainer_->get_gameObject()->set_active(true);
    leftErrorContainer_->get_gameObject()->set_active(false);
    leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftPlaylists() {
    _showingTracks = false;
    backToPlaylistsButton_->get_gameObject()->set_active(false);
    playlistListView_->get_gameObject()->set_active(true);
    trackListView_->get_gameObject()->set_active(false);
    leftErrorContainer_->get_gameObject()->set_active(false);
    leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftTracks() {
    _showingTracks = true;
    backToPlaylistsButton_->get_gameObject()->set_active(true);
    trackListView_->get_gameObject()->set_active(true);
    playlistListView_->get_gameObject()->set_active(false);
    leftErrorContainer_->get_gameObject()->set_active(false);
    leftStatusContainer_->get_gameObject()->set_active(false);
    leftLoadingContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftError(const std::string& msg) {
    leftLoadingContainer_->get_gameObject()->set_active(false);
    leftErrorContainer_->get_gameObject()->set_active(true);
    leftErrorTextView_->set_text(msg);
    leftStatusContainer_->get_gameObject()->set_active(false);
}

void MainViewController::showLeftStatus(const std::string& msg) {
    leftLoadingContainer_->get_gameObject()->set_active(false);
    leftErrorContainer_->get_gameObject()->set_active(false);
    leftStatusContainer_->get_gameObject()->set_active(true);
    leftStatusTextView_->set_text(msg);
}

// ─── Center panel helpers ─────────────────────────────────────────────────────

void MainViewController::showMapLoading() {
    mapLoadingContainer_->get_gameObject()->set_active(true);
    mapStatusContainer_->get_gameObject()->set_active(false);
    mapListView_->get_gameObject()->set_active(false);
}

void MainViewController::showMapList() {
    mapLoadingContainer_->get_gameObject()->set_active(false);
    mapStatusContainer_->get_gameObject()->set_active(false);
    mapListView_->get_gameObject()->set_active(true);
}

void MainViewController::showMapStatus(const std::string& msg) {
    mapLoadingContainer_->get_gameObject()->set_active(false);
    mapStatusContainer_->get_gameObject()->set_active(true);
    mapStatusTextView_->set_text(msg);
    mapListView_->get_gameObject()->set_active(false);
}

// ─── Right panel helpers ──────────────────────────────────────────────────────

void MainViewController::clearMapPreview() {
    previewMapNameTextView_->set_text("Select a map");
    previewUploaderTextView_->set_text("");
    previewDurationTextView_->set_text("");
    previewDiffsTextView_->set_text("");
    downloadButton_->get_gameObject()->set_active(false);
    downloadStatusTextView_->set_text("");
}

void MainViewController::showMapPreview(const AppleMusicSearch::BSMap& map) {
    previewMapNameTextView_->set_text(map.name);
    previewUploaderTextView_->set_text("by " + map.uploaderName);

    if (map.durationSecs > 0) {
        int m = map.durationSecs / 60, s = map.durationSecs % 60;
        char buf[16]; snprintf(buf, sizeof(buf), "%d:%02d", m, s);
        previewDurationTextView_->set_text(buf);
    } else {
        previewDurationTextView_->set_text("");
    }

    std::string diffs;
    for (size_t i = 0; i < map.difficulties.size(); ++i) {
        if (i) diffs += "  ";
        diffs += map.difficulties[i];
    }
    previewDiffsTextView_->set_text(diffs);

    downloadStatusTextView_->set_text("");
    downloadButton_->get_gameObject()->set_active(true);
}

// ─── Load playlists ───────────────────────────────────────────────────────────

void MainViewController::loadPlaylists() {
    showLeftLoading();
    AppleMusicClient::instance().fetchLibraryPlaylists(
        [this](std::vector<AMPlaylist> playlists, std::string err) {
            leftLoadingContainer_->get_gameObject()->set_active(false);
            if (!err.empty()) {
                showLeftError(err);
                return;
            }
            if (playlists.empty()) {
                showLeftStatus("No playlists found.\nCheck MUT in Mod Settings.");
                return;
            }
            _playlists = std::move(playlists);

            auto* ds = get_gameObject()->GetComponent<AMPlaylistTableViewDataSource*>();
            if (ds) {
                ds->playlists_ = _playlists;
                playlistListView_->tableView->ReloadData();
            }
        });
}

// ─── Load tracks for selected playlist ───────────────────────────────────────

void MainViewController::loadTracksForPlaylist(const std::string& playlistId,
                                                const std::string& playlistName) {
    showLeftLoading();
    leftColumnTitleTextView_->set_text(playlistName);

    AppleMusicClient::instance().fetchPlaylistTracks(
        playlistId,
        [this](std::vector<AMSong> tracks, std::string err) {
            if (!err.empty()) {
                showLeftError(err);
                return;
            }
            if (tracks.empty()) {
                showLeftStatus("No tracks in this playlist.");
                return;
            }
            _tracks = std::move(tracks);

            auto* ds = get_gameObject()->GetComponent<AMTrackTableViewDataSource*>();
            if (ds) {
                ds->tracks_ = _tracks;
                trackListView_->tableView->ReloadData();
            }
            showLeftTracks();
        });
}

// ─── BeatSaver search ─────────────────────────────────────────────────────────

void MainViewController::searchBeatSaver(const std::string& title, const std::string& artist) {
    _selectedMapIndex = -1;
    _maps.clear();
    clearMapPreview();
    showMapLoading();

    BeatSaverClient::instance().search(
        title, artist,
        [this](std::vector<BSMap> maps, std::string err) {
            if (!err.empty()) {
                showMapStatus("Error: " + err);
                return;
            }
            if (maps.empty()) {
                showMapStatus("No maps found on BeatSaver.");
                return;
            }
            _maps = std::move(maps);

            auto* ds = get_gameObject()->GetComponent<BSMapTableViewDataSource*>();
            if (ds) {
                ds->maps_ = _maps;
                mapListView_->tableView->ReloadData();
            }
            showMapList();
        });
}

// ─── Callbacks ────────────────────────────────────────────────────────────────

void MainViewController::onPlaylistSelected(int index) {
    if (index < 0 || index >= (int)_playlists.size()) return;
    auto& pl = _playlists[index];
    loadTracksForPlaylist(pl.id, pl.name);
}

void MainViewController::onTrackSelected(int index) {
    if (index < 0 || index >= (int)_tracks.size()) return;
    auto& t = _tracks[index];
    searchBeatSaver(t.title, t.artist);
}

void MainViewController::onBackToPlaylistsClicked() {
    leftColumnTitleTextView_->set_text("Apple Music");
    _tracks.clear();
    auto* ds = get_gameObject()->GetComponent<AMTrackTableViewDataSource*>();
    if (ds) { ds->tracks_.clear(); trackListView_->tableView->ReloadData(); }
    showLeftPlaylists();
    // Clear center and right panels
    _maps.clear();
    auto* mapDS = get_gameObject()->GetComponent<BSMapTableViewDataSource*>();
    if (mapDS) { mapDS->maps_.clear(); mapListView_->tableView->ReloadData(); }
    showMapStatus("");
    clearMapPreview();
}

void MainViewController::onMapSelected(int index) {
    if (index < 0 || index >= (int)_maps.size()) return;
    _selectedMapIndex = index;
    showMapPreview(_maps[index]);
}

void MainViewController::onDownloadClicked() {
    if (_selectedMapIndex < 0 || _selectedMapIndex >= (int)_maps.size()) return;
    if (_isDownloading.load()) return;
    _isDownloading = true;
    downloadButton_->get_gameObject()->set_active(false);
    downloadStatusTextView_->set_text("Downloading…");

    auto map = _maps[_selectedMapIndex];
    BeatSaverClient::instance().downloadMap(
        map,
        [this](bool ok, std::string err) {
            _isDownloading = false;
            downloadButton_->get_gameObject()->set_active(true);
            downloadStatusTextView_->set_text(ok ? "Downloaded!" : "Failed: " + err);
        });
}

} // namespace AppleMusicSearch::UI::ViewControllers
