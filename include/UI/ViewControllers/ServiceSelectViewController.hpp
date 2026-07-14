#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onPasteMut);
    DECLARE_INSTANCE_METHOD(void, onMutChanged);
    DECLARE_INSTANCE_METHOD(void, onPasteJwt);

    DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, mutStatusText_);
    DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, jwtStatusText_);

public:
    StringW get_mutToken();
    void    set_mutToken(StringW v);
};
