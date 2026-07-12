#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, SearchViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_BSML_PROPERTY(StringW, searchQuery);
    DECLARE_BSML_PROPERTY(bool, isLoading);
    DECLARE_BSML_PROPERTY(StringW, statusText);

    DECLARE_INSTANCE_METHOD(void, onSearchSubmitted);
    DECLARE_INSTANCE_METHOD(void, onClearClicked);

    void onResultCellSelected(int index);

private:
    std::vector<AMSong> _results;
)
