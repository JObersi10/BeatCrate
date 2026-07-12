#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "BeatSaver/BeatSaverClient.hpp"
#include <vector>
#include <string>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI::ViewControllers, BeatSaverResultsViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INSTANCE_METHOD(void, onMapCellSelected, UnityW<HMUI::TableView> table, int index);
    DECLARE_INSTANCE_METHOD(void, onDownloadClicked);
    DECLARE_INSTANCE_METHOD(void, onBackClicked);

public:
    void searchFor(const std::string& songTitle, const std::string& artist);

    StringW get_queryLabel();    void set_queryLabel(StringW v);
    bool    get_isLoading();     void set_isLoading(bool v);
    StringW get_statusText();    void set_statusText(StringW v);
    StringW get_detailTitle();   void set_detailTitle(StringW v);
    StringW get_detailUploader();void set_detailUploader(StringW v);
    StringW get_detailDuration();void set_detailDuration(StringW v);
    StringW get_detailDiffs();   void set_detailDiffs(StringW v);

private:
    std::vector<BSMap> _maps;
    int  _selectedIndex  = -1;
    bool _isLoading      = false;
    bool _isDownloading  = false;
    std::string _queryLabel, _statusText;
    std::string _detailTitle, _detailUploader, _detailDuration, _detailDiffs;

    void clearDetail();
};
