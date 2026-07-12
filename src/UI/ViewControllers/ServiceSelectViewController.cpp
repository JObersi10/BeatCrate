#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "Configuration.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "Log.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "HMUI/ViewController.hpp"

// Asset is embedded at build time via cmake/assets.cmake
#include "assets.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController);

namespace AppleMusicSearch::UI::ViewControllers {

void ServiceSelectViewController::DidActivate(bool firstActivation,
                                              bool addedToHierarchy,
                                              bool screenSystemEnabling) {
    if (!firstActivation) return;

    // Load saved server address into client
    auto addr = AppleMusicSearch::getServerAddress();
    AppleMusicClient::instance().setServerAddress(addr);

    BSML::parse_and_construct(IncludedAssets::ServiceSelectViewController_bsml,
                              get_transform(), this);
}

StringW ServiceSelectViewController::get_serverAddress() {
    return StringW(AppleMusicSearch::getServerAddress());
}

void ServiceSelectViewController::set_serverAddress(StringW value) {
    // handled by onServerAddressSet
}

void ServiceSelectViewController::onAppleMusicClicked() {
    auto* child = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
    auto fc = il2cpp_utils::try_cast<AppleMusicSearch::UI::FlowCoordinators::AppleMusicFlowCoordinator>(child).value_or(nullptr);
    if (fc) fc->showAppleMusicHome();
}

void ServiceSelectViewController::onSpotifyClicked() {
    // Show "Coming Soon" toast — BSML floating notification
    BSML::Lite::CreateFloatingScreen("Spotify is coming soon!", nullptr, {0,0,2}, {}, 0, true, true);
    AMS_LOG("Spotify tapped — coming soon");
}

void ServiceSelectViewController::onServerAddressSet() {
    // The BSML string-setting fires this after the user confirms
    // Re-read from the property binding
    std::string addr = static_cast<std::string>(get_serverAddress());
    AppleMusicSearch::setServerAddress(addr);
    AppleMusicClient::instance().setServerAddress(addr);
    AMS_LOG("Server address updated to {}", addr);
}

}
