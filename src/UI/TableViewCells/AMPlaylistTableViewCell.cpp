#include "UI/TableViewCells/AMPlaylistTableViewCell.hpp"
#include "UnityEngine/Color.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, AMPlaylistTableViewCell);

using namespace AppleMusicSearch::UI;

void AMPlaylistTableViewCell::ctor() {
    INVOKE_BASE_CTOR(classof(HMUI::TableCell*));
}

void AMPlaylistTableViewCell::SelectionDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void AMPlaylistTableViewCell::HighlightDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void AMPlaylistTableViewCell::WasPreparedForReuse() {}

void AMPlaylistTableViewCell::updateBackground() {
    root_->set_color(UnityEngine::Color(0, 0, 0, selected || highlighted ? 0.8f : 0.45f));
}

void AMPlaylistTableViewCell::setPlaylist(const AppleMusicSearch::AMPlaylist& pl) {
    nameTextView_->set_text(pl.name);
    if (pl.trackCount > 0)
        countTextView_->set_text(std::to_string(pl.trackCount) + " tracks");
    else
        countTextView_->set_text("");
}
