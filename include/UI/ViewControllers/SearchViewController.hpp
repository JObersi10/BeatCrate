#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "AppleMusic/Models.hpp"
#include <vector>
#include <string>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, SearchViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onSearchSubmitted);
    DECLARE_INSTANCE_METHOD(void, onClearClicked);
    DECLARE_INSTANCE_METHOD(void, onResultCellSelected, int index);

public:
    StringW get_searchQuery();  void set_searchQuery(StringW v);
    bool    get_isLoading();    void set_isLoading(bool v);
    StringW get_statusText();   void set_statusText(StringW v);

private:
    std::vector<AMSong> _results;
    std::string _searchQuery;
    bool _isLoading = false;
    std::string _statusText;
};
