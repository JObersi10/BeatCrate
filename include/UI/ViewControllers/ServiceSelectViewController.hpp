#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, ServiceSelectViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_BSML_PROPERTY(StringW, serverAddress);

    DECLARE_INSTANCE_METHOD(void, onAppleMusicClicked);
    DECLARE_INSTANCE_METHOD(void, onSpotifyClicked);      // shows "Coming Soon" toast
    DECLARE_INSTANCE_METHOD(void, onServerAddressSet);
)
