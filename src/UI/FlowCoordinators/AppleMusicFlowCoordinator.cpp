#include "UI/FlowCoordinators/AppleMusicFlowCoordinator.hpp"
#include "Log.hpp"
#include "bsml/shared/Helpers/creation.hpp"

DEFINE_TYPE(AppleMusicSearch::UI::FlowCoordinators, AppleMusicFlowCoordinator);

namespace AppleMusicSearch::UI::FlowCoordinators {

using AT = HMUI::ViewController_AnimationType;
using AD = HMUI::ViewController_AnimationDirection;

void AppleMusicFlowCoordinator::DidActivate(bool firstActivation, bool, bool) {
    if (!firstActivation) return;
    SetTitle("BeatCrate", AT::In);
    showBackButton = true;
    _mainVC = BSML::Helpers::CreateViewController<ViewControllers::MainViewController*>();
    ProvideInitialViewControllers(_mainVC, nullptr, nullptr, nullptr, nullptr);
}

void AppleMusicFlowCoordinator::BackButtonWasPressed(HMUI::ViewController*) {
    _parentFlowCoordinator->DismissFlowCoordinator(this, AD::Horizontal, nullptr, false);
}

} // namespace AppleMusicSearch::UI::FlowCoordinators
