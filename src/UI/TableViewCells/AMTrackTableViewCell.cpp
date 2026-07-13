#include "UI/TableViewCells/AMTrackTableViewCell.hpp"
#include "UnityEngine/Color.hpp"

DEFINE_TYPE(AppleMusicSearch::UI, AMTrackTableViewCell);

using namespace AppleMusicSearch::UI;

void AMTrackTableViewCell::ctor() {
    INVOKE_BASE_CTOR(classof(HMUI::TableCell*));
}

void AMTrackTableViewCell::SelectionDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void AMTrackTableViewCell::HighlightDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void AMTrackTableViewCell::WasPreparedForReuse() {}

void AMTrackTableViewCell::updateBackground() {
    root_->set_color(UnityEngine::Color(0, 0, 0, selected || highlighted ? 0.8f : 0.45f));
}

void AMTrackTableViewCell::setTrack(const AppleMusicSearch::AMSong& track) {
    titleTextView_->set_text(track.title);
    artistTextView_->set_text(track.artist);
}
