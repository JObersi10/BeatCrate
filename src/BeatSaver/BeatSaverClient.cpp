#include "BeatSaver/BeatSaverClient.hpp"
#include "Log.hpp"

#include "web-utils/shared/WebUtils.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "songcore/shared/SongCore.hpp"
#include <thread>
#include <filesystem>
#include <fstream>

namespace AppleMusicSearch {

BeatSaverClient& BeatSaverClient::instance() {
    static BeatSaverClient inst;
    return inst;
}

static BSMap parseMap(const rapidjson::Value& v) {
    BSMap m;
    if (v.HasMember("id") && v["id"].IsString())     m.id   = v["id"].GetString();
    if (v.HasMember("name") && v["name"].IsString()) m.name = v["name"].GetString();

    if (v.HasMember("metadata") && v["metadata"].IsObject()) {
        auto& meta = v["metadata"];
        if (meta.HasMember("duration") && meta["duration"].IsInt()) m.durationSecs = meta["duration"].GetInt();
        if (meta.HasMember("bpm") && meta["bpm"].IsDouble())        m.bpm = (float)meta["bpm"].GetDouble();
    }
    if (v.HasMember("uploader") && v["uploader"].IsObject()) {
        auto& ul = v["uploader"];
        if (ul.HasMember("name") && ul["name"].IsString()) m.uploaderName = ul["name"].GetString();
    }
    if (v.HasMember("versions") && v["versions"].IsArray() && v["versions"].Size() > 0) {
        auto& ver = v["versions"][0];
        if (ver.HasMember("coverURL") && ver["coverURL"].IsString())
            m.artworkUrl = ver["coverURL"].GetString();
        if (ver.HasMember("downloadURL") && ver["downloadURL"].IsString())
            m.downloadUrl = ver["downloadURL"].GetString();
        if (ver.HasMember("diffs") && ver["diffs"].IsArray()) {
            for (auto& d : ver["diffs"].GetArray())
                if (d.HasMember("difficulty") && d["difficulty"].IsString())
                    m.difficulties.push_back(d["difficulty"].GetString());
        }
    }
    if (v.HasMember("stats") && v["stats"].IsObject()) {
        auto& s = v["stats"];
        if (s.HasMember("upvotes") && s["upvotes"].IsInt())     m.upvotes   = s["upvotes"].GetInt();
        if (s.HasMember("downvotes") && s["downvotes"].IsInt()) m.downvotes = s["downvotes"].GetInt();
    }
    return m;
}

void BeatSaverClient::search(const std::string& songTitle, const std::string& artist, MapsCallback cb) {
    std::string query = songTitle + " " + artist;
    std::string encoded;
    for (char c : query) {
        if (std::isalnum(c) || c == '-' || c == '_') encoded += c;
        else if (c == ' ') encoded += '+';
        else { char buf[4]; snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c); encoded += buf; }
    }

    std::string url = std::string(BEATSAVER_API) + "/search/text/0?q=" + encoded + "&sortOrder=Relevance";
    AMS_LOG("BeatSaver search: {}", url);

    std::thread([url, cb = std::move(cb)]() mutable {
        auto response = WebUtils::Get<WebUtils::JsonResponse>(WebUtils::URLOptions(url));
        BSML::MainThreadScheduler::Schedule([response = std::move(response), cb = std::move(cb)]() mutable {
            if (!response.IsSuccessful() || !response.responseData) {
                cb({}, "HTTP " + std::to_string(response.httpCode));
                return;
            }
            auto& doc = *response.responseData;
            if (!doc.IsObject() || !doc.HasMember("docs")) { cb({}, "Invalid BeatSaver JSON"); return; }
            std::vector<BSMap> maps;
            for (auto& v : doc["docs"].GetArray()) maps.push_back(parseMap(v));
            cb(std::move(maps), "");
        });
    }).detach();
}

void BeatSaverClient::downloadMap(const BSMap& map,
                                  std::function<void(bool, std::string)> cb) {
    if (map.downloadUrl.empty()) { cb(false, "No download URL"); return; }

    std::string url   = map.downloadUrl;
    std::string mapId = map.id;

    std::thread([url, mapId, cb = std::move(cb)]() mutable {
        auto response = WebUtils::Get<WebUtils::DataResponse<std::vector<uint8_t>>>(
            WebUtils::URLOptions(url));

        if (!response.IsSuccessful() || !response.responseData) {
            BSML::MainThreadScheduler::Schedule([cb, code = response.httpCode]() mutable {
                cb(false, "Download failed HTTP " + std::to_string(code));
            });
            return;
        }

        auto customSongsPath = SongCore::API::Loading::GetPreferredCustomLevelPath();
        std::filesystem::path zipPath =
            std::string(customSongsPath) + "/" + mapId + ".zip";

        std::ofstream out(zipPath, std::ios::binary);
        out.write(reinterpret_cast<const char*>(response.responseData->data()),
                  response.responseData->size());
        out.close();

        SongCore::API::Loading::RefreshSongs(false);

        BSML::MainThreadScheduler::Schedule([cb]() mutable { cb(true, ""); });
    }).detach();
}

}
