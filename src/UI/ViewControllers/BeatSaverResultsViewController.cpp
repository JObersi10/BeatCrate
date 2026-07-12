#include "UI/ViewControllers/BeatSaverResultsViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "BeatSaver/BeatSaverClient.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, BeatSaverResultsViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void BeatSaverResultsViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::BeatSaverResultsViewController_bsml,
                              get_transform(), this);
}

void BeatSaverResultsViewController::searchFor(const std::string& songTitle,
                                               const std::string& artist) {
    _maps.clear();
    _selectedIndex = -1;
    set_queryLabel("Results for: " + songTitle);
    set_isLoading(true);
    set_statusText("Searching BeatSaver…");
    clearDetail();

    BeatSaverClient::instance().search(songTitle, artist,
        [this](std::vector<BSMap> maps, std::string err) {
            set_isLoading(false);
            if (!err.empty()) { set_statusText("Error: " + err); return; }
            if (maps.empty()) { set_statusText("No maps found on BeatSaver."); return; }
            _maps = std::move(maps);
            set_statusText("");

            auto* list = get_gameObject()->GetComponentInChildren<BSML::CustomListTableData*>(false);
            if (!list) return;
            list->data.clear();
            for (auto& m : _maps) {
                std::string sub = m.uploaderName;
                if (m.durationSecs > 0) {
                    int mins = m.durationSecs / 60, secs = m.durationSecs % 60;
                    char buf[16]; snprintf(buf, sizeof(buf), "%d:%02d", mins, secs);
                    sub += "  ·  " + std::string(buf);
                }
                list->data.push_back(BSML::CustomCellInfo::construct(
                    StringW(m.name), StringW(sub), nullptr));
            }
            list->tableView->ReloadData();
        });
}

void BeatSaverResultsViewController::onMapCellSelected(int index) {
    if (index < 0 || index >= (int)_maps.size()) return;
    _selectedIndex = index;
    auto& m = _maps[index];
    set_detailTitle(m.name);
    set_detailUploader("by " + m.uploaderName);
    if (m.durationSecs > 0) {
        int mins = m.durationSecs / 60, secs = m.durationSecs % 60;
        char buf[16]; snprintf(buf, sizeof(buf), "%d:%02d", mins, secs);
        set_detailDuration(std::string(buf));
    } else {
        set_detailDuration("");
    }
    std::string diffs;
    for (size_t i = 0; i < m.difficulties.size(); ++i) {
        if (i) diffs += "  ";
        diffs += m.difficulties[i];
    }
    set_detailDiffs(diffs);
}

void BeatSaverResultsViewController::onDownloadClicked() {
    if (_selectedIndex < 0 || _selectedIndex >= (int)_maps.size() || _isDownloading) return;
    _isDownloading = true;
    set_statusText("Downloading…");
    BeatSaverClient::instance().downloadMap(_maps[_selectedIndex], [this](bool ok, std::string err) {
        _isDownloading = false;
        set_statusText(ok ? "Downloaded! ✓" : "Download failed: " + err);
    });
}

void BeatSaverResultsViewController::onBackClicked() {
    auto* child = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
    auto fc = il2cpp_utils::try_cast<AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator>(child).value_or(nullptr);
    if (fc) fc->popToPreviousView();
}

void BeatSaverResultsViewController::clearDetail() {
    set_detailTitle("Select a map");
    set_detailUploader(""); set_detailDuration(""); set_detailDiffs("");
}

StringW BeatSaverResultsViewController::get_queryLabel()               { return StringW(_queryLabel); }
void    BeatSaverResultsViewController::set_queryLabel(StringW v)      { _queryLabel = static_cast<std::string>(v); }
bool    BeatSaverResultsViewController::get_isLoading()                { return _isLoading; }
void    BeatSaverResultsViewController::set_isLoading(bool v)          { _isLoading = v; }
StringW BeatSaverResultsViewController::get_statusText()               { return StringW(_statusText); }
void    BeatSaverResultsViewController::set_statusText(StringW v)      { _statusText = static_cast<std::string>(v); }
StringW BeatSaverResultsViewController::get_detailTitle()              { return StringW(_detailTitle); }
void    BeatSaverResultsViewController::set_detailTitle(StringW v)     { _detailTitle = static_cast<std::string>(v); }
StringW BeatSaverResultsViewController::get_detailUploader()           { return StringW(_detailUploader); }
void    BeatSaverResultsViewController::set_detailUploader(StringW v)  { _detailUploader = static_cast<std::string>(v); }
StringW BeatSaverResultsViewController::get_detailDuration()           { return StringW(_detailDuration); }
void    BeatSaverResultsViewController::set_detailDuration(StringW v)  { _detailDuration = static_cast<std::string>(v); }
StringW BeatSaverResultsViewController::get_detailDiffs()              { return StringW(_detailDiffs); }
void    BeatSaverResultsViewController::set_detailDiffs(StringW v)     { _detailDiffs = static_cast<std::string>(v); }

}
