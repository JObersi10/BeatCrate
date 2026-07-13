#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN_INTERFACES(AppleMusicSearch::UI, AMPlaylistTableViewDataSource, UnityEngine::MonoBehaviour, HMUI::TableView::IDataSource*) {
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);

public:
    std::vector<AppleMusicSearch::AMPlaylist> playlists_;
};
