#include "UI/ViewControllers/LibraryViewController.hpp"
DEFINE_TYPE(AppleMusicSearch::UI::ViewControllers, LibraryViewController);
namespace AppleMusicSearch::UI::ViewControllers {
void LibraryViewController::ctor() {
    INVOKE_BASE_CTOR(classof(HMUI::ViewController*));
}
void LibraryViewController::DidActivate(bool, bool, bool) {}
}
