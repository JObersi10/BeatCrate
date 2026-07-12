#include "main.hpp"
#include "Configuration.hpp"
#include "AppleMusic/AppleMusicClient.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "Log.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML-Lite.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "HMUI/FlowCoordinator.hpp"

static ModInfo modInfo{MOD_ID, VERSION, 0};

// Hook: inject "Music Search" button into the main menu
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate,
                &GlobalNamespace::MainMenuViewController::DidActivate,
                void,
                GlobalNamespace::MainMenuViewController* self,
                bool firstActivation,
                bool addedToHierarchy,
                bool screenSystemEnabling) {

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (!firstActivation) return;

    BSML::Lite::CreateUIButton(self->get_transform(), "Music Search", [self]() {
        auto fc = BSML::Helpers::CreateFlowCoordinator<AppleMusicSearch::UI::AppleMusicFlowCoordinator*>();
        BSML::Helpers::GetMainFlowCoordinator()->PresentFlowCoordinator(
            fc, nullptr, HMUI::ViewController::AnimationType::In, false, false);
    });

    AMS_LOG("Music Search button injected into main menu");
}

extern "C" void setup(CModInfo* info) {
    info->id      = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;

    AppleMusicSearch::getConfig().Load();
    AppleMusicSearch::AppleMusicClient::instance()
        .setServerAddress(AppleMusicSearch::getServerAddress());

    AMS_LOG("Setup complete — server: {}", AppleMusicSearch::getServerAddress());
}

extern "C" void late_load() {
    il2cpp_functions::Init();

    custom_types::Register::AutoRegister();

    INSTALL_HOOK(AppleMusicSearch::logger, MainMenuViewController_DidActivate);

    AMS_LOG("Hooks installed");
}
