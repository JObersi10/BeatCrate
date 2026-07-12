#pragma once
#include "Models.hpp"
#include <functional>
#include <string>
#include <vector>

namespace AppleMusicSearch {

using SongsCallback    = std::function<void(std::vector<AMSong>, std::string /*error*/)>;
using AlbumsCallback   = std::function<void(std::vector<AMAlbum>, std::string)>;
using PlaylistsCallback = std::function<void(std::vector<AMPlaylist>, std::string)>;

class AppleMusicClient {
public:
    static AppleMusicClient& instance();

    void setServerAddress(const std::string& address);
    const std::string& serverAddress() const;

    // Search catalog — no MUT needed
    void search(const std::string& term, SongsCallback cb);

    // Library — requires MUT set on the server
    void fetchLibrarySongs(SongsCallback cb);
    void fetchLibraryAlbums(AlbumsCallback cb);
    void fetchLibraryPlaylists(PlaylistsCallback cb);
    void fetchPlaylistTracks(const std::string& playlistId, SongsCallback cb);

private:
    AppleMusicClient() = default;

    // Fires GET {serverAddress}/path, calls cb(responseBody, "")
    // or cb("", errorMessage) on failure. Non-blocking.
    void get(const std::string& path,
             std::function<void(std::string /*body*/, std::string /*error*/)> cb);

    std::string _serverAddress;
};

}
