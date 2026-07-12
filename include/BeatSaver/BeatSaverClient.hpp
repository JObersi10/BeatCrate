#pragma once
#include <functional>
#include <string>
#include <vector>

namespace AppleMusicSearch {

struct BSMap {
    std::string id;          // BeatSaver map key (e.g. "1a2b3")
    std::string name;
    std::string author;
    std::string uploaderName;
    std::string artworkUrl;
    float bpm = 0.f;
    int durationSecs = 0;
    int upvotes = 0;
    int downvotes = 0;
    // Diffs available: "Easy","Normal","Hard","Expert","ExpertPlus"
    std::vector<std::string> difficulties;
    std::string downloadUrl;
};

using MapsCallback = std::function<void(std::vector<BSMap>, std::string /*error*/)>;

class BeatSaverClient {
public:
    static BeatSaverClient& instance();

    // Search BeatSaver for maps matching song title + artist
    void search(const std::string& songTitle,
                const std::string& artist,
                MapsCallback cb);

    // Download a map .zip to the custom songs directory
    void downloadMap(const BSMap& map,
                     std::function<void(bool /*success*/, std::string /*error*/)> cb);

private:
    BeatSaverClient() = default;
    void get(const std::string& url,
             std::function<void(std::string, std::string)> cb);
};

static constexpr const char* BEATSAVER_API = "https://api.beatsaver.com";

}
