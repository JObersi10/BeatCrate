#include "UI/ViewControllers/SearchViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, SearchViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void SearchViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::SearchViewController_bsml,
                              get_transform(), this);
}

static AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator* getFC() {
    return [&]{ 
    auto _w = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf(); 
    HMUI::FlowCoordinator* _raw = _w; 
    return il2cpp_utils::try_cast<AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator>(_raw).value_or(nullptr); 
}();
}

void SearchViewController::onSearchSubmitted() {
    std::string term = static_cast<std::string>(get_searchQuery());
    if (term.empty()) return;
    set_isLoading(true);
    set_statusText("Searching…");
    _results.clear();

    AppleMusicClient::instance().search(term, [this](std::vector<AMSong> songs, std::string err) {
        set_isLoading(false);
        if (!err.empty()) { set_statusText("Error: " + err); return; }
        if (songs.empty()) { set_statusText("No results."); return; }
        _results = std::move(songs);
        set_statusText("");
        auto* list = get_gameObject()->GetComponentInChildren<BSML::CustomListTableData*>(false);
        if (!list) return;
        list->data.clear();
        for (auto& s : _results)
            list->data.push_back(BSML::CustomCellInfo::construct(StringW(s.title), StringW(s.artist), nullptr));
        list->tableView->ReloadData();
    });
}

void SearchViewController::onClearClicked() {
    set_searchQuery("");
    _results.clear();
    set_statusText("");
    auto* list = get_gameObject()->GetComponentInChildren<BSML::CustomListTableData*>(false);
    if (list) { list->data.clear(); list->tableView->ReloadData(); }
}

void SearchViewController::onResultCellSelected(int index) {
    if (index < 0 || index >= (int)_results.size()) return;
    auto& s = _results[index];
    if (auto fc = getFC()) fc->showBeatSaverResults(s.title, s.artist);
}

StringW SearchViewController::get_searchQuery()          { return StringW(_searchQuery); }
void    SearchViewController::set_searchQuery(StringW v) { _searchQuery = static_cast<std::string>(v); }
bool    SearchViewController::get_isLoading()            { return _isLoading; }
void    SearchViewController::set_isLoading(bool v)      { _isLoading = v; }
StringW SearchViewController::get_statusText()           { return StringW(_statusText); }
void    SearchViewController::set_statusText(StringW v)  { _statusText = static_cast<std::string>(v); }

}
