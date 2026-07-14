#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "Configuration.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "UnityEngine/GUIUtility.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void ServiceSelectViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::ServiceSelectViewController_bsml,
                              get_transform(), this);
    std::string mut = getMut();
    if (mutStatusText_)
        mutStatusText_->set_text(StringW(mut.empty() ? "Not set" : "Set (" + std::to_string(mut.size()) + " chars)"));
}

void ServiceSelectViewController::onPasteMut() {
    auto clip = UnityEngine::GUIUtility::get_systemCopyBuffer();
    std::string s = static_cast<std::string>(clip);
    if (mutStatusText_) mutStatusText_->set_text(StringW(s.empty() ? "Clipboard empty" : "Set (" + std::to_string(s.size()) + " chars)"));
    if (s.empty()) return;
    setMut(s);
    AMS_LOG("MUT pasted ({} chars)", s.size());
}

void ServiceSelectViewController::onMutChanged() {
    std::string mut = static_cast<std::string>(get_mutToken());
    if (!mut.empty()) {
        setMut(mut);
        if (mutStatusText_) mutStatusText_->set_text(StringW("Set (" + std::to_string(mut.size()) + " chars)"));
    }
}

StringW ServiceSelectViewController::get_mutToken()          { return StringW(getMut()); }
void    ServiceSelectViewController::set_mutToken(StringW v) { /* handled by onMutChanged */ }

}
