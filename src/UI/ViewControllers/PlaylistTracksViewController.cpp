#include "UI/ViewControllers/PlaylistTracksViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML-Lite.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, PlaylistTracksViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void PlaylistTracksViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::PlaylistTracksViewController_bsml,
                              get_transform(), this);
}

void PlaylistTracksViewController::loadPlaylist(const AMPlaylist& playlist) {
    _playlist = playlist;
    set_playlistName(StringW(playlist.name));
    set_isLoading(true);
    set_statusText("Loading tracks…");

    AppleMusicClient::instance().fetchPlaylistTracks(
        playlist.id,
        [this](std::vector<AMSong> tracks, std::string err) {
            set_isLoading(false);
            if (!err.empty()) {
                set_statusText("Error: " + err);
                return;
            }
            _tracks = std::move(tracks);
            set_statusText("");

            auto* list = BSML::Helpers::GetComponentInChildren<BSML::CustomListTableData*>(
                             get_gameObject(), "trackList");
            if (!list) return;
            list->data.clear();
            for (auto& s : _tracks) {
                list->data.push_back(BSML::CustomCellInfo::construct(
                    StringW(s.title), StringW(s.artist), nullptr));
            }
            list->tableView->ReloadData();
        });
}

void PlaylistTracksViewController::onTrackCellSelected(int index) {
    if (index < 0 || index >= (int)_tracks.size()) return;
    auto& s = _tracks[index];
    auto fc = BSML::Helpers::GetMainFlowCoordinator()
                  ->YoungestChildFlowCoordinatorOrSelf()
                  ->TryCast<AppleMusicFlowCoordinator>();
    if (fc) fc->showBeatSaverResults(s.title, s.artist);
}

void PlaylistTracksViewController::onBackClicked() {
    auto fc = BSML::Helpers::GetMainFlowCoordinator()
                  ->YoungestChildFlowCoordinatorOrSelf()
                  ->TryCast<AppleMusicFlowCoordinator>();
    if (fc) fc->popToAppleMusicHome();
}

bool    PlaylistTracksViewController::get_isLoading()            { return _isLoading; }
void    PlaylistTracksViewController::set_isLoading(bool v)      { _isLoading = v; }
StringW PlaylistTracksViewController::get_playlistName()         { return StringW(_playlistName); }
void    PlaylistTracksViewController::set_playlistName(StringW v){ _playlistName = static_cast<std::string>(v); }
StringW PlaylistTracksViewController::get_statusText()           { return StringW(_statusText); }
void    PlaylistTracksViewController::set_statusText(StringW v)  { _statusText = static_cast<std::string>(v); }

}
