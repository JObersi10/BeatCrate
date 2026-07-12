#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, AppleMusicFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::FlowCoordinator::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed,
        &HMUI::FlowCoordinator::BackButtonWasPressed,
        HMUI::ViewController* topViewController);

    // Called by child VCs to show BeatSaver results for a tapped song
    void showBeatSaverResults(const std::string& songTitle, const std::string& artist);

    void reset();
)
