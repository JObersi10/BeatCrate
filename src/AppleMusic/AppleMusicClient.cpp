#include "AppleMusic/AppleMusicClient.hpp"
#include "Configuration.hpp"
#include "Log.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include <thread>
#include <memory>

namespace AppleMusicSearch {

AppleMusicClient& AppleMusicClient::instance() { static AppleMusicClient i; return i; }
void AppleMusicClient::setServerAddress(const std::string& a) { _serverAddress = a; }
const std::string& AppleMusicClient::serverAddress() const { return _serverAddress; }

void AppleMusicClient::get(const std::string& path,
                           std::function<void(const rapidjson::Document*, std::string)> cb) {
    std::string url = "http://" + _serverAddress + path;
    AMS_LOG("GET {}", url);
    std::thread([url, cb = std::move(cb)]() mutable {
        auto resp = std::make_shared<WebUtils::JsonResponse>(
            WebUtils::Get<WebUtils::JsonResponse>(WebUtils::URLOptions(url)));
        BSML::MainThreadScheduler::Schedule([resp, cb = std::move(cb)]() mutable {
            if (!resp->IsSuccessful() || !resp->responseData) {
                cb(nullptr, "HTTP " + std::to_string(resp->httpCode)); return;
            }
            cb(&(*resp->responseData), "");
        });
    }).detach();
}

static AMSong parseSong(const rapidjson::Value& v) {
    AMSong s;
    if (v.HasMember("id") && v["id"].IsString())         s.id         = v["id"].GetString();
    if (v.HasMember("title") && v["title"].IsString())   s.title      = v["title"].GetString();
    if (v.HasMember("artist") && v["artist"].IsString()) s.artist     = v["artist"].GetString();
    if (v.HasMember("album") && v["album"].IsString())   s.album      = v["album"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString()) s.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("durationMs") && v["durationMs"].IsInt()) s.durationMs = v["durationMs"].GetInt();
    return s;
}
static AMAlbum parseAlbum(const rapidjson::Value& v) {
    AMAlbum a;
    if (v.HasMember("id") && v["id"].IsString())   a.id = v["id"].GetString();
    if (v.HasMember("title") && v["title"].IsString()) a.title = v["title"].GetString();
    if (v.HasMember("artist") && v["artist"].IsString()) a.artist = v["artist"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString()) a.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("trackCount") && v["trackCount"].IsInt()) a.trackCount = v["trackCount"].GetInt();
    return a;
}
static AMPlaylist parsePlaylist(const rapidjson::Value& v) {
    AMPlaylist p;
    if (v.HasMember("id") && v["id"].IsString())   p.id = v["id"].GetString();
    if (v.HasMember("name") && v["name"].IsString()) p.name = v["name"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString()) p.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("trackCount") && v["trackCount"].IsInt()) p.trackCount = v["trackCount"].GetInt();
    return p;
}

void AppleMusicClient::search(const std::string& term, SongsCallback cb) {
    std::string enc;
    for (char c : term) {
        if (std::isalnum(c) || c=='-' || c=='_') enc+=c;
        else if (c==' ') enc+='+';
        else { char b[4]; snprintf(b,sizeof(b),"%%%02X",(unsigned char)c); enc+=b; }
    }
    get("/api/search?term="+enc, [cb](const rapidjson::Document* d, std::string e) {
        if (!d || !d->IsArray()) { cb({}, e.empty()?"Bad JSON":e); return; }
        std::vector<AMSong> v; for (auto& x:d->GetArray()) v.push_back(parseSong(x)); cb(std::move(v),"");
    });
}
void AppleMusicClient::fetchLibrarySongs(SongsCallback cb) {
    get("/api/library/songs", [cb](const rapidjson::Document* d, std::string e) {
        if (!d||!d->IsArray()){cb({},e.empty()?"Bad JSON":e);return;}
        std::vector<AMSong> v; for(auto& x:d->GetArray()) v.push_back(parseSong(x)); cb(std::move(v),"");
    });
}
void AppleMusicClient::fetchLibraryAlbums(AlbumsCallback cb) {
    get("/api/library/albums", [cb](const rapidjson::Document* d, std::string e) {
        if (!d||!d->IsArray()){cb({},e.empty()?"Bad JSON":e);return;}
        std::vector<AMAlbum> v; for(auto& x:d->GetArray()) v.push_back(parseAlbum(x)); cb(std::move(v),"");
    });
}
void AppleMusicClient::fetchLibraryPlaylists(PlaylistsCallback cb) {
    get("/api/library/playlists", [cb](const rapidjson::Document* d, std::string e) {
        if (!d||!d->IsArray()){cb({},e.empty()?"Bad JSON":e);return;}
        std::vector<AMPlaylist> v;
        for(auto& x:d->GetArray()) v.push_back(parsePlaylist(x));
        std::sort(v.begin(),v.end(),[](const AMPlaylist& a,const AMPlaylist& b){return a.name<b.name;});
        cb(std::move(v),"");
    });
}
void AppleMusicClient::fetchPlaylistTracks(const std::string& id, SongsCallback cb) {
    get("/api/library/playlists/"+id+"/tracks", [cb](const rapidjson::Document* d, std::string e) {
        if (!d||!d->IsArray()){cb({},e.empty()?"Bad JSON":e);return;}
        std::vector<AMSong> v; for(auto& x:d->GetArray()) v.push_back(parseSong(x)); cb(std::move(v),"");
    });
}

}
