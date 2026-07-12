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

using namespace AppleMusicSearch::UI;

static void openBeatCrate() {
    static SafePtrUnity<FlowCoordinators::AppleMusicFlowCoordinator> fc;
    if (!fc) fc = BSML::Helpers::CreateFlowCoordinator<FlowCoordinators::AppleMusicFlowCoordinator*>();
    auto parent = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
    if (parent) parent->PresentFlowCoordinator(fc.ptr(), nullptr, HMUI::ViewController_AnimationDirection::Horizontal, false, false);
}

MOD_EXTERN_FUNC void setup(CModInfo* info) noexcept {
    *info = AppleMusicSearch::modInfo.to_c();
    AMS_LOG("BeatCrate setup");
}

MOD_EXTERN_FUNC void late_load() noexcept {
    il2cpp_functions::Init();
    BSML::Init();
    custom_types::Register::AutoRegister();
    BSML::Register::RegisterMenuButton("BeatCrate", "Browse your Apple Music library", openBeatCrate);
    BSML::Register::RegisterSettingsMenu<ViewControllers::ServiceSelectViewController*>("BeatCrate", false);
    AMS_LOG("BeatCrate loaded");
}
