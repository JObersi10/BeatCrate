#include "UI/TableViewDataSources/AMPlaylistTableViewDataSource.hpp"
#include "UI/TableViewCells/AMPlaylistTableViewCell.hpp"
#include "HMUI/Touchable.hpp"
#include "bsml/shared/BSML.hpp"
#include "UnityEngine/GameObject.hpp"
#include "assets.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, AMPlaylistTableViewDataSource);

using namespace AppleMusicSearch::UI;

HMUI::TableCell* AMPlaylistTableViewDataSource::CellForIdx(HMUI::TableView* tableView, int idx) {
    auto tcd = tableView->DequeueReusableCellForIdentifier(AMPlaylistTableViewCell::CELL_REUSE_ID);
    AMPlaylistTableViewCell* cell;
    if (!tcd) {
        auto go = UnityEngine::GameObject::New_ctor("AMPlaylistCell");
        cell = go->AddComponent<AMPlaylistTableViewCell*>();
        cell->set_interactable(true);
        cell->set_reuseIdentifier(AMPlaylistTableViewCell::CELL_REUSE_ID);
        BSML::parse_and_construct(IncludedAssets::AMPlaylistTableViewCell_bsml, cell->get_transform(), cell);
        cell->get_gameObject()->AddComponent<HMUI::Touchable*>();
    } else {
        cell = tcd->GetComponent<AMPlaylistTableViewCell*>();
    }
    cell->setPlaylist(playlists_.at(idx));
    return cell;
}

int   AMPlaylistTableViewDataSource::NumberOfCells() { return (int)playlists_.size(); }
float AMPlaylistTableViewDataSource::CellSize()      { return 8.0f; }
