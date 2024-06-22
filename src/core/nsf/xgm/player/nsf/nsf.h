#ifndef _NSF_H_
#define _NSF_H_
#include <stdint.h>

namespace xgm
{
struct NSFE_Entry {
    const char* tlbl;
    const char* taut;
    int time;
    int fade;
    bool psfx;
};
const unsigned int NSFE_ENTRIES = 256;
const unsigned int NSFE_MIXES = 8;
const int16_t NSFE_MIXE_DEFAULT = 32767;

class NSF
{
  public:
    char magic[5];
    uint8_t version;
    uint8_t songs;
    uint8_t total_songs;
    uint8_t start;
    uint16_t load_address;
    uint16_t init_address;
    uint16_t play_address;
    uint16_t speed_ntsc;
    uint8_t bankswitch[8];
    uint16_t speed_pal;
    uint8_t pal_ntsc;
    uint8_t soundchip;
    bool use_vrc7;
    bool use_vrc6;
    bool use_fds;
    bool use_fme7;
    bool use_mmc5;
    bool use_n106;
    uint8_t extra[4];
    uint8_t* body;
    int bodysize;
    uint8_t* nsfe_image;
    NSFE_Entry nsfe_entry[NSFE_ENTRIES];
    int16_t nsfe_mixe[NSFE_MIXES];
    int vrc7_type;
    const uint8_t* vrc7_patches;
    uint16_t speed_dendy;
    uint8_t regn;
    int regn_pref;
    uint8_t nsf2_bits;

    int song;
    bool playlist_mode;
    int time_in_ms, default_playtime;
    int loop_in_ms;
    int fade_in_ms, default_fadetime;
    int loop_num, default_loopnum;
    bool playtime_unknown;
    bool title_unknown;

    NSF();
    ~NSF();
    bool Load(uint8_t* image, uint32_t size);
    bool LoadNSFe(uint8_t* image, uint32_t size, bool nsf2);
    int GetLength();
    void SetDefaults(int playtime, int fadetime, int loopnum);
    void SetLength(int time_in_ms);
    void SetSong(int);
    int GetSong();
    int GetSongNum();
    int GetPlayTime();
    int GetLoopTime();
    int GetFadeTime();
    int GetLoopNum();
    bool UseNSFePlaytime();
};

} // namespace xgm
#endif
