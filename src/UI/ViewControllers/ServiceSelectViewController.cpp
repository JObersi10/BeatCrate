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
    set_mutStatus(StringW(mut.empty() ? "Not set" : "Set (" + std::to_string(mut.size()) + " chars)"));
}

void ServiceSelectViewController::onPasteMut() {
    auto clip = UnityEngine::GUIUtility::get_systemCopyBuffer();
    std::string s = static_cast<std::string>(clip);
    if (s.empty()) { set_mutStatus(StringW("Clipboard empty")); return; }
    setMut(s);
    set_mutStatus(StringW("Set (" + std::to_string(s.size()) + " chars)"));
    AMS_LOG("MUT pasted ({} chars)", s.size());
}

void ServiceSelectViewController::onMutChanged() {
    // string-setting fires this on confirm — re-read binding value
    std::string mut = static_cast<std::string>(get_mutToken());
    if (!mut.empty()) {
        setMut(mut);
        set_mutStatus(StringW("Set (" + std::to_string(mut.size()) + " chars)"));
    }
}

StringW ServiceSelectViewController::get_mutToken()           { return StringW(getMut()); }
void    ServiceSelectViewController::set_mutToken(StringW v)  { /* handled by onMutChanged */ }
StringW ServiceSelectViewController::get_mutStatus()          { return StringW(_mutStatus); }
void    ServiceSelectViewController::set_mutStatus(StringW v) { _mutStatus = static_cast<std::string>(v); }

}
