#include "main.hpp"
#include "Configuration.hpp"
#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "UI/ViewControllers/ServiceSelectViewController.hpp"
#include "Log.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "scotland2/shared/modloader.h"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/creation.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "HMUI/FlowCoordinator.hpp"

using namespace AppleMusicSearch::UI;

MAKE_HOOK_MATCH(MainMenuViewController_DidActivate,
                &GlobalNamespace::MainMenuViewController::DidActivate,
                void,
                GlobalNamespace::MainMenuViewController* self,
                bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (!firstActivation) return;

    BSML::Lite::CreateUIButton(self->get_transform(), "Music Search", []() {
        auto fc = BSML::Helpers::CreateFlowCoordinator<FlowCoordinators::AppleMusicFlowCoordinator*>();
        BSML::Helpers::GetMainFlowCoordinator()->PresentFlowCoordinator(
            fc, nullptr, HMUI::ViewController_AnimationDirection::Vertical, false, false);
    });
}

extern "C" void setup(CModInfo* info) noexcept {
    *info = AppleMusicSearch::modInfo.to_c();
    AMS_LOG("BeatCrate setup");
}

extern "C" void late_load() noexcept {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    INSTALL_HOOK(AppleMusicSearch::logger, MainMenuViewController_DidActivate);
    BSML::Register::RegisterSettingsMenu<ViewControllers::ServiceSelectViewController*>("BeatCrate");
    AMS_LOG("BeatCrate loaded");
}
