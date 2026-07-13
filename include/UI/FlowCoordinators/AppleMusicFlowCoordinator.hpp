#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "UI/ViewControllers/MainViewController.hpp"
#include "UI/ViewControllers/LibraryViewController.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::FlowCoordinators, AppleMusicFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed,
        HMUI::ViewController* topViewController);

    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::MainViewController>, _mainVC);
    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::LibraryViewController>, _leftVC);
    DECLARE_INSTANCE_FIELD(UnityW<AppleMusicSearch::UI::ViewControllers::LibraryViewController>, _rightVC);
};
