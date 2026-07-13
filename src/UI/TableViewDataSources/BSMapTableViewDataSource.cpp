#include "UI/TableViewDataSources/BSMapTableViewDataSource.hpp"
#include "UI/TableViewCells/BSMapTableViewCell.hpp"
#include "HMUI/Touchable.hpp"
#include "bsml/shared/BSML.hpp"
#include "UnityEngine/GameObject.hpp"
#include "assets.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, BSMapTableViewDataSource);

using namespace AppleMusicSearch::UI;

HMUI::TableCell* BSMapTableViewDataSource::CellForIdx(HMUI::TableView* tableView, int idx) {
    auto tcd = tableView->DequeueReusableCellForIdentifier(BSMapTableViewCell::CELL_REUSE_ID);
    BSMapTableViewCell* cell;
    if (!tcd) {
        auto go = UnityEngine::GameObject::New_ctor("BSMapCell");
        cell = go->AddComponent<BSMapTableViewCell*>();
        cell->set_interactable(true);
        cell->set_reuseIdentifier(BSMapTableViewCell::CELL_REUSE_ID);
        BSML::parse_and_construct(IncludedAssets::BSMapTableViewCell_bsml, cell->get_transform(), cell);
        cell->get_gameObject()->AddComponent<HMUI::Touchable*>();
    } else {
        cell = tcd->GetComponent<BSMapTableViewCell*>();
    }
    cell->setMap(maps_.at(idx));
    return cell;
}

int   BSMapTableViewDataSource::NumberOfCells() { return (int)maps_.size(); }
float BSMapTableViewDataSource::CellSize()      { return 8.0f; }
