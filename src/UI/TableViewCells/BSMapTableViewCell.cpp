#include "UI/TableViewCells/BSMapTableViewCell.hpp"
#include "UnityEngine/Color.hpp"
#include <format>

DEFINE_TYPE(AppleMusicSearch::UI, BSMapTableViewCell);

using namespace AppleMusicSearch::UI;

void BSMapTableViewCell::ctor() {
    INVOKE_BASE_CTOR(classof(HMUI::TableCell*));
}

void BSMapTableViewCell::SelectionDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void BSMapTableViewCell::HighlightDidChange(HMUI::SelectableCell::TransitionType) { updateBackground(); }
void BSMapTableViewCell::WasPreparedForReuse() {}

void BSMapTableViewCell::updateBackground() {
    root_->set_color(UnityEngine::Color(0, 0, 0, selected || highlighted ? 0.8f : 0.45f));
}

void BSMapTableViewCell::setMap(const AppleMusicSearch::BSMap& map) {
    mapNameTextView_->set_text(map.name);
    std::string sub = map.uploaderName;
    if (map.durationSecs > 0) {
        int m = map.durationSecs / 60, s = map.durationSecs % 60;
        sub += std::format("  ·  {}:{:02d}", m, s);
    }
    uploaderTextView_->set_text(sub);
}
