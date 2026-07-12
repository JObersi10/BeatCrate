#include "BeatSaver/BeatSaverClient.hpp"
#include "Log.hpp"
#include "web-utils/shared/WebUtils.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "songcore/shared/SongCore.hpp"
#include <thread>
#include <memory>
#include <filesystem>
#include <fstream>

namespace AppleMusicSearch {

BeatSaverClient& BeatSaverClient::instance() { static BeatSaverClient i; return i; }

static BSMap parseMap(const rapidjson::Value& v) {
    BSMap m;
    if (v.HasMember("id") && v["id"].IsString())     m.id   = v["id"].GetString();
    if (v.HasMember("name") && v["name"].IsString()) m.name = v["name"].GetString();
    if (v.HasMember("metadata") && v["metadata"].IsObject()) {
        auto& meta = v["metadata"];
        if (meta.HasMember("duration") && meta["duration"].IsInt()) m.durationSecs = meta["duration"].GetInt();
        if (meta.HasMember("bpm") && meta["bpm"].IsDouble()) m.bpm = (float)meta["bpm"].GetDouble();
    }
    if (v.HasMember("uploader") && v["uploader"].IsObject()) {
        auto& ul = v["uploader"];
        if (ul.HasMember("name") && ul["name"].IsString()) m.uploaderName = ul["name"].GetString();
    }
    if (v.HasMember("versions") && v["versions"].IsArray() && v["versions"].Size() > 0) {
        auto& ver = v["versions"][0];
        if (ver.HasMember("coverURL") && ver["coverURL"].IsString()) m.artworkUrl = ver["coverURL"].GetString();
        if (ver.HasMember("downloadURL") && ver["downloadURL"].IsString()) m.downloadUrl = ver["downloadURL"].GetString();
        if (ver.HasMember("diffs") && ver["diffs"].IsArray())
            for (auto& d : ver["diffs"].GetArray())
                if (d.HasMember("difficulty") && d["difficulty"].IsString())
                    m.difficulties.push_back(d["difficulty"].GetString());
    }
    if (v.HasMember("stats") && v["stats"].IsObject()) {
        if (v["stats"].HasMember("upvotes"))   m.upvotes   = v["stats"]["upvotes"].GetInt();
        if (v["stats"].HasMember("downvotes")) m.downvotes = v["stats"]["downvotes"].GetInt();
    }
    return m;
}

void BeatSaverClient::search(const std::string& title, const std::string& artist, MapsCallback cb) {
    std::string q = title + " " + artist, enc;
    for (char c : q) {
        if (std::isalnum(c)||c=='-'||c=='_') enc+=c;
        else if (c==' ') enc+='+';
        else { char b[4]; snprintf(b,sizeof(b),"%%%02X",(unsigned char)c); enc+=b; }
    }
    std::string url = std::string(BEATSAVER_API)+"/search/text/0?q="+enc+"&sortOrder=Relevance";
    AMS_LOG("BeatSaver: {}", url);
    std::thread([url, cb=std::move(cb)]() mutable {
        auto resp = std::make_shared<WebUtils::JsonResponse>(
            WebUtils::Get<WebUtils::JsonResponse>(WebUtils::URLOptions(url)));
        BSML::MainThreadScheduler::Schedule([resp, cb=std::move(cb)]() mutable {
            if (!resp->IsSuccessful()||!resp->responseData) { cb({},"HTTP "+std::to_string(resp->httpCode)); return; }
            auto& doc = *resp->responseData;
            if (!doc.IsObject()||!doc.HasMember("docs")) { cb({},"Bad BeatSaver JSON"); return; }
            std::vector<BSMap> maps;
            for (auto& v : doc["docs"].GetArray()) maps.push_back(parseMap(v));
            cb(std::move(maps),"");
        });
    }).detach();
}

void BeatSaverClient::downloadMap(const BSMap& map, std::function<void(bool,std::string)> cb) {
    if (map.downloadUrl.empty()) { cb(false,"No download URL"); return; }
    std::string url = map.downloadUrl, id = map.id;
    std::thread([url, id, cb=std::move(cb)]() mutable {
        // Download as string (raw bytes)
        auto resp = std::make_shared<WebUtils::StringResponse>(
            WebUtils::Get<WebUtils::StringResponse>(WebUtils::URLOptions(url)));
        if (!resp->IsSuccessful() || !resp->responseData) {
            BSML::MainThreadScheduler::Schedule([resp, cb=std::move(cb)]() mutable {
                cb(false, "Download failed HTTP "+std::to_string(resp->httpCode));
            });
            return;
        }
        auto path = std::string(SongCore::API::Loading::GetPreferredCustomLevelPath());
        std::filesystem::path zip = path + "/" + id + ".zip";
        { std::ofstream f(zip, std::ios::binary); f.write(resp->responseData->data(), resp->responseData->size()); }
        SongCore::API::Loading::RefreshSongs(false);
        BSML::MainThreadScheduler::Schedule([cb=std::move(cb)]() mutable { cb(true,""); });
    }).detach();
}

}
