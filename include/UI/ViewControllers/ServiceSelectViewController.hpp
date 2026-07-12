#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onAppleMusicClicked);
    DECLARE_INSTANCE_METHOD(void, onSpotifyClicked);
    DECLARE_INSTANCE_METHOD(void, onServerAddressSet);

public:
    StringW get_serverAddress();
    void    set_serverAddress(StringW v);
};
