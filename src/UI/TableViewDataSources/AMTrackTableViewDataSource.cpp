#include "UI/TableViewDataSources/AMTrackTableViewDataSource.hpp"
#include "UI/TableViewCells/AMTrackTableViewCell.hpp"
#include "HMUI/Touchable.hpp"
#include "bsml/shared/BSML.hpp"
#include "UnityEngine/GameObject.hpp"
#include "assets.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, AMTrackTableViewDataSource);

using namespace AppleMusicSearch::UI;

HMUI::TableCell* AMTrackTableViewDataSource::CellForIdx(HMUI::TableView* tableView, int idx) {
    auto tcd = tableView->DequeueReusableCellForIdentifier(AMTrackTableViewCell::CELL_REUSE_ID);
    AMTrackTableViewCell* cell;
    if (!tcd) {
        auto go = UnityEngine::GameObject::New_ctor("AMTrackCell");
        cell = go->AddComponent<AMTrackTableViewCell*>();
        cell->set_interactable(true);
        cell->set_reuseIdentifier(AMTrackTableViewCell::CELL_REUSE_ID);
        BSML::parse_and_construct(IncludedAssets::AMTrackTableViewCell_bsml, cell->get_transform(), cell);
        cell->get_gameObject()->AddComponent<HMUI::Touchable*>();
    } else {
        cell = tcd->GetComponent<AMTrackTableViewCell*>();
    }
    cell->setTrack(tracks_.at(idx));
    return cell;
}

int   AMTrackTableViewDataSource::NumberOfCells() { return (int)tracks_.size(); }
float AMTrackTableViewDataSource::CellSize()      { return 8.0f; }
