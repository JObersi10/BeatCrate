#include "AppleMusic/AppleMusicClient.hpp"
#include "Configuration.hpp"
#include "Log.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include <thread>
#include <memory>
#include <regex>
#include <algorithm>

namespace AppleMusicSearch {

static const std::string BASE = "https://amp-api-edge.music.apple.com";
static const std::string USER_AGENT =
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15";

AppleMusicClient& AppleMusicClient::instance() {
    static AppleMusicClient i;
    return i;
}

// ─── Artwork helpers ─────────────────────────────────────────────────────────

static std::string fmtArt(const std::string& tmpl, int sz = 300) {
    if (tmpl.empty()) return "";
    std::string s = tmpl;
    auto rep = [&](const std::string& from, const std::string& to) {
        auto p = s.find(from);
        if (p != std::string::npos) s.replace(p, from.size(), to);
    };
    std::string n = std::to_string(sz);
    rep("{w}", n); rep("{h}", n); rep("{f}", "jpg");
    return s;
}

// ─── JSON helpers ─────────────────────────────────────────────────────────────

static std::string js(const rapidjson::Value& v, const char* k, const char* def = "") {
    if (v.HasMember(k) && v[k].IsString()) return v[k].GetString();
    return def;
}
static int ji(const rapidjson::Value& v, const char* k, int def = 0) {
    if (v.HasMember(k) && v[k].IsInt()) return v[k].GetInt();
    return def;
}
static bool hasMember(const rapidjson::Value& v, const char* k) {
    return v.IsObject() && v.HasMember(k);
}

static AMSong parseSong(const rapidjson::Value& item) {
    AMSong s;
    s.id = js(item, "id");
    if (!hasMember(item, "attributes")) return s;
    const auto& a = item["attributes"];
    s.title  = js(a, "name");
    s.artist = js(a, "artistName");
    s.album  = js(a, "albumName");
    s.durationMs = ji(a, "durationInMillis");
    if (hasMember(a, "artwork") && a["artwork"].IsObject())
        s.artworkUrl = fmtArt(js(a["artwork"], "url"));
    return s;
}

static AMAlbum parseAlbum(const rapidjson::Value& item) {
    AMAlbum a;
    a.id = js(item, "id");
    if (!hasMember(item, "attributes")) return a;
    const auto& at = item["attributes"];
    a.title      = js(at, "name");
    a.artist     = js(at, "artistName");
    a.trackCount = ji(at, "trackCount");
    if (hasMember(at, "artwork") && at["artwork"].IsObject())
        a.artworkUrl = fmtArt(js(at["artwork"], "url"));
    return a;
}

static AMPlaylist parsePlaylist(const rapidjson::Value& item) {
    AMPlaylist p;
    p.id = js(item, "id");
    if (!hasMember(item, "attributes")) return p;
    const auto& a = item["attributes"];
    p.name = js(a, "name");
    // Try attributes.artwork first, fallback to catalog relationship
    if (hasMember(a, "artwork") && a["artwork"].IsObject() && a["artwork"].HasMember("url"))
        p.artworkUrl = fmtArt(js(a["artwork"], "url"));
    else if (hasMember(item, "relationships") &&
             hasMember(item["relationships"], "catalog") &&
             item["relationships"]["catalog"].HasMember("data") &&
             item["relationships"]["catalog"]["data"].IsArray() &&
             !item["relationships"]["catalog"]["data"].Empty()) {
        const auto& cat = item["relationships"]["catalog"]["data"][0];
        if (hasMember(cat, "attributes") && hasMember(cat["attributes"], "artwork"))
            p.artworkUrl = fmtArt(js(cat["attributes"]["artwork"], "url"));
    }
    return p;
}

// ─── JWT fetch ────────────────────────────────────────────────────────────────

static std::string fetchJwtSync() {
    // Step 1: fetch music.apple.com HTML
    WebUtils::URLOptions htmlOpts("https://music.apple.com/");
    htmlOpts.headers = std::unordered_map<std::string, std::string>{
        {"User-Agent", USER_AGENT}
    };
    auto htmlResp = std::make_shared<WebUtils::StringResponse>(
        WebUtils::Get<WebUtils::StringResponse>(htmlOpts));
    if (!htmlResp->IsSuccessful() || !htmlResp->responseData) {
        AMS_ERROR("JWT: failed to fetch music.apple.com ({})", htmlResp->httpCode);
        return "";
    }
    const std::string& html = *htmlResp->responseData;

    // Find script path
    std::regex scriptRe("crossorigin src=\"(/assets/index\\.[^\"]+\\.js)\"");
    std::smatch sm;
    if (!std::regex_search(html, sm, scriptRe)) {
        AMS_ERROR("JWT: script path not found in HTML");
        return "";
    }
    std::string scriptPath = sm[1].str();
    AMS_LOG("JWT: found script {}", scriptPath);

    // Step 2: fetch the JS bundle
    WebUtils::URLOptions jsOpts("https://music.apple.com" + scriptPath);
    jsOpts.headers = std::unordered_map<std::string, std::string>{
        {"User-Agent", USER_AGENT}
    };
    auto jsResp = std::make_shared<WebUtils::StringResponse>(
        WebUtils::Get<WebUtils::StringResponse>(jsOpts));
    if (!jsResp->IsSuccessful() || !jsResp->responseData) {
        AMS_ERROR("JWT: failed to fetch JS bundle");
        return "";
    }
    const std::string& jsText = *jsResp->responseData;

    // Extract JWT
    std::regex jwtRe(R"((eyJ[A-Za-z0-9\-_]{10,}\.[A-Za-z0-9\-_]{10,}\.[A-Za-z0-9\-_]*))");
    std::smatch jm;
    if (!std::regex_search(jsText, jm, jwtRe)) {
        AMS_ERROR("JWT: token not found in JS bundle");
        return "";
    }
    std::string jwt = jm[1].str();
    AMS_LOG("JWT: extracted (len={})", jwt.size());
    return jwt;
}

void AppleMusicClient::withJwt(std::function<void(std::string)> cb) {
    // Use in-memory cache first, then config cache
    if (!_jwt.empty()) { cb(_jwt); return; }
    std::string cached = getCachedJwt();
    if (!cached.empty()) { _jwt = cached; cb(_jwt); return; }

    std::thread([this, cb = std::move(cb)]() mutable {
        std::string jwt = fetchJwtSync();
        if (!jwt.empty()) {
            _jwt = jwt;
            setCachedJwt(jwt);
        }
        BSML::MainThreadScheduler::Schedule([jwt, cb = std::move(cb)]() {
            cb(jwt);
        });
    }).detach();
}

// ─── Core GET ─────────────────────────────────────────────────────────────────

void AppleMusicClient::apiGet(const std::string& url, bool needsMut,
    std::function<void(const rapidjson::Document*, std::string)> cb) {

    withJwt([url, needsMut, cb = std::move(cb)](std::string jwt) {
        if (jwt.empty()) { cb(nullptr, "Could not obtain Bearer token"); return; }
        std::string mut = getMut();
        if (needsMut && mut.empty()) { cb(nullptr, "No MUT set — open Mod Settings → BeatCrate"); return; }

        std::thread([url, jwt, mut, needsMut, cb = std::move(cb)]() mutable {
            WebUtils::URLOptions opts(url);
            std::unordered_map<std::string, std::string> hdrs{
                {"Authorization",   "Bearer " + jwt},
                {"Origin",          "https://music.apple.com"},
                {"User-Agent",      USER_AGENT},
            };
            if (needsMut && !mut.empty())
                hdrs["Music-User-Token"] = mut;
            opts.headers = hdrs;

            auto resp = std::make_shared<WebUtils::JsonResponse>(
                WebUtils::Get<WebUtils::JsonResponse>(opts));
            BSML::MainThreadScheduler::Schedule([resp, url, cb = std::move(cb)]() mutable {
                if (!resp->IsSuccessful() || !resp->responseData) {
                    AMS_ERROR("API {} -> HTTP {}", url, resp->httpCode);
                    cb(nullptr, "HTTP " + std::to_string(resp->httpCode));
                    return;
                }
                cb(&(*resp->responseData), "");
            });
        }).detach();
    });
}

// ─── URL helpers ──────────────────────────────────────────────────────────────

static std::string urlEncode(const std::string& s) {
    std::string out;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c=='-' || c=='_' || c=='.' || c=='~') out += c;
        else if (c == ' ') out += '+';
        else { char b[4]; snprintf(b, sizeof(b), "%%%02X", c); out += b; }
    }
    return out;
}

// ─── Public API ───────────────────────────────────────────────────────────────

void AppleMusicClient::search(const std::string& term, SongsCallback cb) {
    std::string sf = getStorefront();
    std::string url = BASE + "/v1/catalog/" + sf + "/search?term=" +
                      urlEncode(term) + "&types=songs&limit=25&l=en-US";
    apiGet(url, false, [cb](const rapidjson::Document* d, std::string err) {
        if (!d) { cb({}, err); return; }
        std::vector<AMSong> v;
        if (d->HasMember("results") && (*d)["results"].HasMember("songs") &&
            (*d)["results"]["songs"].HasMember("data")) {
            for (auto& item : (*d)["results"]["songs"]["data"].GetArray())
                v.push_back(parseSong(item));
        }
        cb(std::move(v), "");
    });
}

void AppleMusicClient::fetchLibrarySongs(SongsCallback cb) {
    std::string url = BASE + "/v1/me/library/songs?limit=100&l=en-US";
    apiGet(url, true, [cb](const rapidjson::Document* d, std::string err) {
        if (!d) { cb({}, err); return; }
        std::vector<AMSong> v;
        if (d->HasMember("data") && (*d)["data"].IsArray()) {
            for (auto& item : (*d)["data"].GetArray())
                v.push_back(parseSong(item));
        }
        std::sort(v.begin(), v.end(), [](const AMSong& a, const AMSong& b){
            return a.title < b.title;
        });
        cb(std::move(v), "");
    });
}

void AppleMusicClient::fetchLibraryAlbums(AlbumsCallback cb) {
    std::string url = BASE + "/v1/me/library/albums?limit=100&l=en-US";
    apiGet(url, true, [cb](const rapidjson::Document* d, std::string err) {
        if (!d) { cb({}, err); return; }
        std::vector<AMAlbum> v;
        if (d->HasMember("data") && (*d)["data"].IsArray()) {
            for (auto& item : (*d)["data"].GetArray())
                v.push_back(parseAlbum(item));
        }
        std::sort(v.begin(), v.end(), [](const AMAlbum& a, const AMAlbum& b){
            return a.title < b.title;
        });
        cb(std::move(v), "");
    });
}

void AppleMusicClient::fetchLibraryPlaylists(PlaylistsCallback cb) {
    std::string url = BASE + "/v1/me/library/playlists?limit=100&l=en-US";
    apiGet(url, true, [cb](const rapidjson::Document* d, std::string err) {
        if (!d) { cb({}, err); return; }
        std::vector<AMPlaylist> v;
        if (d->HasMember("data") && (*d)["data"].IsArray()) {
            for (auto& item : (*d)["data"].GetArray())
                v.push_back(parsePlaylist(item));
        }
        std::sort(v.begin(), v.end(), [](const AMPlaylist& a, const AMPlaylist& b){
            return a.name < b.name;
        });
        cb(std::move(v), "");
    });
}

void AppleMusicClient::fetchPlaylistTracks(const std::string& id, SongsCallback cb) {
    std::string url;
    // p. = user playlist (library), pl. = catalog/editorial
    if (id.rfind("pl.", 0) == 0)
        url = BASE + "/v1/catalog/" + getStorefront() + "/playlists/" + id + "/tracks?limit=100&l=en-US";
    else
        url = BASE + "/v1/me/library/playlists/" + id + "/tracks?limit=100&l=en-US";

    bool needsMut = (id.rfind("pl.", 0) != 0); // catalog playlists don't strictly require MUT
    apiGet(url, needsMut, [cb](const rapidjson::Document* d, std::string err) {
        if (!d) { cb({}, err); return; }
        std::vector<AMSong> v;
        if (d->HasMember("data") && (*d)["data"].IsArray()) {
            for (auto& item : (*d)["data"].GetArray())
                v.push_back(parseSong(item));
        }
        cb(std::move(v), "");
    });
}

}
