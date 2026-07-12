#include "AppleMusic/AppleMusicClient.hpp"
#include "Configuration.hpp"
#include "Log.hpp"

#include "web-utils/shared/WebUtils.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include <thread>

namespace AppleMusicSearch {

AppleMusicClient& AppleMusicClient::instance() {
    static AppleMusicClient inst;
    return inst;
}

void AppleMusicClient::setServerAddress(const std::string& address) { _serverAddress = address; }
const std::string& AppleMusicClient::serverAddress() const { return _serverAddress; }

// Run a GET on a background thread; deliver parsed JSON doc to cb on main thread.
void AppleMusicClient::get(const std::string& path,
                           std::function<void(const rapidjson::Document*, std::string)> cb) {
    std::string url = "http://" + _serverAddress + path;
    AMS_LOG("GET {}", url);

    std::thread([url, cb = std::move(cb)]() mutable {
        auto response = WebUtils::Get<WebUtils::JsonResponse>(WebUtils::URLOptions(url));
        BSML::MainThreadScheduler::Schedule([response = std::move(response), cb = std::move(cb)]() mutable {
            if (!response.IsSuccessful() || !response.responseData) {
                cb(nullptr, "HTTP " + std::to_string(response.httpCode));
                return;
            }
            cb(&(*response.responseData), "");
        });
    }).detach();
}

// ── Parsing helpers ──────────────────────────────────────────────────────────

static AMSong parseSong(const rapidjson::Value& v) {
    AMSong s;
    if (v.HasMember("id") && v["id"].IsString())         s.id         = v["id"].GetString();
    if (v.HasMember("title") && v["title"].IsString())   s.title      = v["title"].GetString();
    if (v.HasMember("artist") && v["artist"].IsString()) s.artist     = v["artist"].GetString();
    if (v.HasMember("album") && v["album"].IsString())   s.album      = v["album"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString())
        s.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("durationMs") && v["durationMs"].IsInt())
        s.durationMs = v["durationMs"].GetInt();
    return s;
}

static AMAlbum parseAlbum(const rapidjson::Value& v) {
    AMAlbum a;
    if (v.HasMember("id") && v["id"].IsString())          a.id         = v["id"].GetString();
    if (v.HasMember("title") && v["title"].IsString())    a.title      = v["title"].GetString();
    if (v.HasMember("artist") && v["artist"].IsString())  a.artist     = v["artist"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString())
        a.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("trackCount") && v["trackCount"].IsInt())
        a.trackCount = v["trackCount"].GetInt();
    return a;
}

static AMPlaylist parsePlaylist(const rapidjson::Value& v) {
    AMPlaylist p;
    if (v.HasMember("id") && v["id"].IsString())        p.id         = v["id"].GetString();
    if (v.HasMember("name") && v["name"].IsString())    p.name       = v["name"].GetString();
    if (v.HasMember("artworkUrl") && v["artworkUrl"].IsString())
        p.artworkUrl = v["artworkUrl"].GetString();
    if (v.HasMember("trackCount") && v["trackCount"].IsInt())
        p.trackCount = v["trackCount"].GetInt();
    return p;
}

// ── Public API ───────────────────────────────────────────────────────────────

void AppleMusicClient::search(const std::string& term, SongsCallback cb) {
    std::string encoded;
    for (char c : term) {
        if (std::isalnum(c) || c == '-' || c == '_') encoded += c;
        else if (c == ' ') encoded += '+';
        else { char buf[4]; snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c); encoded += buf; }
    }
    get("/api/search?term=" + encoded, [cb](const rapidjson::Document* doc, std::string err) {
        if (!err.empty() || !doc || !doc->IsArray()) { cb({}, err.empty() ? "Invalid JSON" : err); return; }
        std::vector<AMSong> songs;
        for (auto& v : doc->GetArray()) songs.push_back(parseSong(v));
        cb(std::move(songs), "");
    });
}

void AppleMusicClient::fetchLibrarySongs(SongsCallback cb) {
    get("/api/library/songs", [cb](const rapidjson::Document* doc, std::string err) {
        if (!err.empty() || !doc || !doc->IsArray()) { cb({}, err.empty() ? "Invalid JSON" : err); return; }
        std::vector<AMSong> songs;
        for (auto& v : doc->GetArray()) songs.push_back(parseSong(v));
        cb(std::move(songs), "");
    });
}

void AppleMusicClient::fetchLibraryAlbums(AlbumsCallback cb) {
    get("/api/library/albums", [cb](const rapidjson::Document* doc, std::string err) {
        if (!err.empty() || !doc || !doc->IsArray()) { cb({}, err.empty() ? "Invalid JSON" : err); return; }
        std::vector<AMAlbum> albums;
        for (auto& v : doc->GetArray()) albums.push_back(parseAlbum(v));
        cb(std::move(albums), "");
    });
}

void AppleMusicClient::fetchLibraryPlaylists(PlaylistsCallback cb) {
    get("/api/library/playlists", [cb](const rapidjson::Document* doc, std::string err) {
        if (!err.empty() || !doc || !doc->IsArray()) { cb({}, err.empty() ? "Invalid JSON" : err); return; }
        std::vector<AMPlaylist> playlists;
        for (auto& v : doc->GetArray()) playlists.push_back(parsePlaylist(v));
        std::sort(playlists.begin(), playlists.end(),
                  [](const AMPlaylist& a, const AMPlaylist& b) { return a.name < b.name; });
        cb(std::move(playlists), "");
    });
}

void AppleMusicClient::fetchPlaylistTracks(const std::string& playlistId, SongsCallback cb) {
    get("/api/library/playlists/" + playlistId + "/tracks",
        [cb](const rapidjson::Document* doc, std::string err) {
            if (!err.empty() || !doc || !doc->IsArray()) { cb({}, err.empty() ? "Invalid JSON" : err); return; }
            std::vector<AMSong> songs;
            for (auto& v : doc->GetArray()) songs.push_back(parseSong(v));
            cb(std::move(songs), "");
        });
}

}
