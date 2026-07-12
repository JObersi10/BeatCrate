#pragma once
#include <string>
#include <vector>

namespace AppleMusicSearch {

struct AMSong {
    std::string id;
    std::string title;
    std::string artist;
    std::string album;
    std::string artworkUrl;
    int durationMs = 0;
};

struct AMAlbum {
    std::string id;
    std::string title;
    std::string artist;
    std::string artworkUrl;
    int trackCount = 0;
};

struct AMPlaylist {
    std::string id;
    std::string name;
    std::string artworkUrl;
    int trackCount = 0;
};

struct AMLibrary {
    std::vector<AMSong> songs;
    std::vector<AMAlbum> albums;
    std::vector<AMPlaylist> playlists;
};

}
