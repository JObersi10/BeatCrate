#pragma once
#include "Models.hpp"
#include "web-utils/shared/WebUtils.hpp"
#include <functional>
#include <string>
#include <vector>

namespace AppleMusicSearch {

using SongsCallback     = std::function<void(std::vector<AMSong>, std::string)>;
using AlbumsCallback    = std::function<void(std::vector<AMAlbum>, std::string)>;
using PlaylistsCallback = std::function<void(std::vector<AMPlaylist>, std::string)>;

class AppleMusicClient {
public:
    static AppleMusicClient& instance();
    void setServerAddress(const std::string& address);
    const std::string& serverAddress() const;
    void search(const std::string& term, SongsCallback cb);
    void fetchLibrarySongs(SongsCallback cb);
    void fetchLibraryAlbums(AlbumsCallback cb);
    void fetchLibraryPlaylists(PlaylistsCallback cb);
    void fetchPlaylistTracks(const std::string& playlistId, SongsCallback cb);
private:
    AppleMusicClient() = default;
    void get(const std::string& path,
             std::function<void(const rapidjson::Document*, std::string)> cb);
    std::string _serverAddress;
};

}
