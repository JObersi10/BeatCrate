#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "Configuration.hpp"
#include "Log.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "UnityEngine/GUIUtility.hpp"
#include "web-utils/shared/WebUtils.hpp"
#include <thread>

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void ServiceSelectViewController::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    BSML::parse_and_construct(IncludedAssets::ServiceSelectViewController_bsml,
                              get_transform(), this);
    std::string mut = getMut();
    if (mutStatusText_)
        mutStatusText_->set_text(StringW(mut.empty() ? "Not set" : "Set (" + std::to_string(mut.size()) + " chars)"));
    std::string jwt = getCachedJwt();
    if (jwtStatusText_)
        jwtStatusText_->set_text(StringW(jwt.empty() ? "Not set" : "Set (" + std::to_string(jwt.size()) + " chars)"));
}

static std::string trimStr(std::string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

void ServiceSelectViewController::onPasteMut() {
    auto clip = UnityEngine::GUIUtility::get_systemCopyBuffer();
    std::string s = trimStr(static_cast<std::string>(clip));
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

void ServiceSelectViewController::onPasteJwt() {
    auto clip = UnityEngine::GUIUtility::get_systemCopyBuffer();
    std::string s = trimStr(static_cast<std::string>(clip));
    if (s.rfind("Bearer ", 0) == 0) s = trimStr(s.substr(7));
    if (jwtStatusText_) jwtStatusText_->set_text(StringW(s.empty() ? "Clipboard empty" : "Set (" + std::to_string(s.size()) + " chars)"));
    if (s.empty()) return;
    setCachedJwt(s);
    AMS_LOG("JWT pasted ({} chars)", s.size());
}

void ServiceSelectViewController::onDebugHostChanged() {
    std::string h = static_cast<std::string>(get_debugHost());
    h = trimStr(h);
    if (h.empty()) return;
    setDebugHost(h);
    // Fire-and-forget ping so the user knows the server connection works
    std::thread([h]() {
        WebUtils::Get<WebUtils::StringResponse>(
            WebUtils::URLOptions("http://" + h + ":8080/log?msg=BeatCrate+connected+from+Quest"));
    }).detach();
}

StringW ServiceSelectViewController::get_mutToken()          { return StringW(getMut()); }
void    ServiceSelectViewController::set_mutToken(StringW v) { /* handled by onMutChanged */ }
StringW ServiceSelectViewController::get_debugHost()         { return StringW(getDebugHost()); }
void    ServiceSelectViewController::set_debugHost(StringW v){ setDebugHost(static_cast<std::string>(v)); }

}
