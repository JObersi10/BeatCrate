#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, ServiceSelectViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onPasteMut);
    DECLARE_INSTANCE_METHOD(void, onMutChanged);

public:
    StringW get_mutToken();
    void    set_mutToken(StringW v);
    StringW get_mutStatus();
    void    set_mutStatus(StringW v);

private:
    std::string _mutStatus;
};
