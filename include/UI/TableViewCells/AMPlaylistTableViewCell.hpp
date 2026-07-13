#pragma once
#include "HMUI/ImageView.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "AppleMusic/Models.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, AMPlaylistTableViewCell, HMUI::TableCell) {
    DECLARE_CTOR(ctor);
    DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);

    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, root_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, nameTextView_);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, countTextView_);

public:
    static constexpr std::string_view CELL_REUSE_ID = "AMPlaylistTableViewCell";
    void setPlaylist(const AppleMusicSearch::AMPlaylist& playlist);
private:
    void updateBackground();
};
