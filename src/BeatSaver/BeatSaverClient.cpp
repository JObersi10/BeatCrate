#include "BeatSaver/BeatSaverClient.hpp"
#include "Log.hpp"

#include "web-utils/shared/WebUtils.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "songcore/shared/SongCore.hpp"

#include "rapidjson/document.h"
#include <thread>
#include <filesystem>

namespace AppleMusicSearch {

BeatSaverClient& BeatSaverClient::instance() {
    static BeatSaverClient inst;
    return inst;
}

void BeatSaverClient::get(const std::string& url,
                          std::function<void(std::string, std::string)> cb) {
    AMS_LOG("BeatSaver GET {}", url);
    std::thread([url, cb = std::move(cb)]() mutable {
        WebUtils::GetAsync(url, [cb = std::move(cb)](long code, std::string body) mutable {
            BSML::MainThreadScheduler::Schedule([code, body, cb = std::move(cb)]() mutable {
                if (code == 200) cb(std::move(body), "");
                else             cb("", "HTTP " + std::to_string(code));
            });
        });
    }).detach();
}

static BSMap parseMap(const rapidjson::Value& v) {
    BSMap m;
    if (v.HasMember("id") && v["id"].IsString())
        m.id = v["id"].GetString();
    if (v.HasMember("name") && v["name"].IsString())
        m.name = v["name"].GetString();

    if (v.HasMember("metadata") && v["metadata"].IsObject()) {
        auto& meta = v["metadata"];
        if (meta.HasMember("duration") && meta["duration"].IsInt())
            m.durationSecs = meta["duration"].GetInt();
        if (meta.HasMember("bpm") && meta["bpm"].IsFloat())
            m.bpm = meta["bpm"].GetFloat();
    }

    if (v.HasMember("uploader") && v["uploader"].IsObject()) {
        auto& ul = v["uploader"];
        if (ul.HasMember("name") && ul["name"].IsString())
            m.uploaderName = ul["name"].GetString();
    }

    if (v.HasMember("versions") && v["versions"].IsArray() && !v["versions"].Empty()) {
        auto& ver = v["versions"][0];
        if (ver.HasMember("coverURL") && ver["coverURL"].IsString())
            m.artworkUrl = ver["coverURL"].GetString();
        if (ver.HasMember("downloadURL") && ver["downloadURL"].IsString())
            m.downloadUrl = ver["downloadURL"].GetString();
        if (ver.HasMember("diffs") && ver["diffs"].IsArray()) {
            for (auto& d : ver["diffs"].GetArray()) {
                if (d.HasMember("difficulty") && d["difficulty"].IsString())
                    m.difficulties.push_back(d["difficulty"].GetString());
            }
        }
    }

    if (v.HasMember("stats") && v["stats"].IsObject()) {
        auto& s = v["stats"];
        if (s.HasMember("upvotes") && s["upvotes"].IsInt())   m.upvotes   = s["upvotes"].GetInt();
        if (s.HasMember("downvotes") && s["downvotes"].IsInt()) m.downvotes = s["downvotes"].GetInt();
    }

    return m;
}

void BeatSaverClient::search(const std::string& songTitle,
                             const std::string& artist,
                             MapsCallback cb) {
    // Combine title + artist for best match; URL-encode spaces
    std::string query = songTitle + " " + artist;
    std::string encoded;
    for (char c : query) {
        if (std::isalnum(c) || c == '-' || c == '_') encoded += c;
        else if (c == ' ') encoded += '+';
        else { char buf[4]; snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c); encoded += buf; }
    }

    std::string url = std::string(BEATSAVER_API) + "/search/text/0?q=" + encoded + "&sortOrder=Relevance";

    get(url, [cb](std::string body, std::string err) {
        if (!err.empty()) { cb({}, err); return; }
        rapidjson::Document doc;
        doc.Parse(body.c_str());
        if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("docs")) {
            cb({}, "Invalid BeatSaver JSON"); return;
        }
        std::vector<BSMap> maps;
        for (auto& v : doc["docs"].GetArray())
            maps.push_back(parseMap(v));
        cb(std::move(maps), "");
    });
}

void BeatSaverClient::downloadMap(const BSMap& map,
                                  std::function<void(bool, std::string)> cb) {
    if (map.downloadUrl.empty()) { cb(false, "No download URL"); return; }

    std::string url = map.downloadUrl;
    std::string mapId = map.id;
    std::string mapName = map.name;

    std::thread([url, mapId, mapName, cb = std::move(cb)]() mutable {
        // Download zip bytes
        WebUtils::GetAsync(url, [mapId, mapName, cb = std::move(cb)](long code, std::string body) mutable {
            if (code != 200) {
                BSML::MainThreadScheduler::Schedule([cb, code]() mutable {
                    cb(false, "Download failed: HTTP " + std::to_string(code));
                });
                return;
            }

            // Write zip to custom songs directory
            auto customSongsPath = SongCore::API::Loading::GetPreferredCustomLevelPath();
            std::filesystem::path destDir = std::string(customSongsPath) + "/" + mapId + " (" + mapName + ")";
            std::filesystem::create_directories(destDir);

            // Write raw zip
            std::filesystem::path zipPath = destDir.parent_path() / (mapId + ".zip");
            {
                std::ofstream out(zipPath, std::ios::binary);
                out.write(body.data(), body.size());
            }

            // Extract zip (SongCore handles this via its own extractor on next load,
            // but we can trigger a refresh immediately)
            SongCore::API::Loading::RefreshSongs(false);

            BSML::MainThreadScheduler::Schedule([cb]() mutable {
                cb(true, "");
            });
        });
    }).detach();
}

}
