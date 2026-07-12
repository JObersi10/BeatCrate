#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "BeatSaver/BeatSaverClient.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(AppleMusicSearch::UI, BeatSaverResultsViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
        &HMUI::ViewController::DidActivate,
        bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    // Called by FlowCoordinator to kick off a new search
    void searchFor(const std::string& songTitle, const std::string& artist);

    DECLARE_BSML_PROPERTY(StringW, queryLabel);   // "Results for: Mr. Brightside"
    DECLARE_BSML_PROPERTY(bool, isLoading);
    DECLARE_BSML_PROPERTY(StringW, statusText);

    // Detail panel (right side) — populated on cell tap
    DECLARE_BSML_PROPERTY(StringW, detailTitle);
    DECLARE_BSML_PROPERTY(StringW, detailUploader);
    DECLARE_BSML_PROPERTY(StringW, detailDuration);
    DECLARE_BSML_PROPERTY(StringW, detailDiffs);

    DECLARE_INSTANCE_METHOD(void, onDownloadClicked);
    DECLARE_INSTANCE_METHOD(void, onBackClicked);

    void onMapCellSelected(int index);

private:
    std::vector<BSMap> _maps;
    int _selectedIndex = -1;
    bool _isDownloading = false;
)
