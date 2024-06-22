#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "nsf.h"

namespace xgm
{

uint8_t CONVERT_REGN[4] = {0x01, 0x02, 0x03, 0x03}; // NTSC, PAL, DUAL, DUAL
int CONVERT_REGN_PREF[4] = {-1, -1, 0, 1};          // Any,  Any, NTSC,  PAL

NSF::NSF()
{
    body = NULL;
    default_playtime = 5 * 60 * 1000;
    default_fadetime = 5 * 1000;
    default_loopnum = 0;
    nsfe_image = NULL;
    for (unsigned int i = 0; i < NSFE_MIXES; ++i) {
        nsfe_mixe[i] = NSFE_MIXE_DEFAULT;
    }
}

NSF::~NSF()
{
    delete[] body;
    delete[] nsfe_image;
}

void NSF::SetDefaults(int p, int f, int l)
{
    default_playtime = p;
    default_fadetime = f;
    default_loopnum = l;
}

void NSF::SetLength(int t)
{
    time_in_ms = t;
    playtime_unknown = false;
}

int NSF::GetPlayTime()
{
    if (nsfe_entry[song].time >= 0) {
        return nsfe_entry[song].time;
    }

    return time_in_ms < 0 ? default_playtime : time_in_ms;
}

int NSF::GetLoopTime()
{
    return loop_in_ms < 0 ? 0 : loop_in_ms;
}

int NSF::GetFadeTime()
{
    if (nsfe_entry[song].fade >= 0) {
        return nsfe_entry[song].fade;
    }

    if (fade_in_ms < 0)
        return default_fadetime;
    else if (fade_in_ms == 0)
        return 50;
    else
        return fade_in_ms;
}

int NSF::GetLoopNum()
{
    return loop_num > 0 ? loop_num - 1 : default_loopnum - 1;
}

int NSF::GetLength()
{
    return GetPlayTime() + GetLoopTime() * GetLoopNum() + GetFadeTime();
}

int NSF::GetSong()
{
    return song;
}

int NSF::GetSongNum()
{
    return songs;
}

bool NSF::UseNSFePlaytime()
{
    return false;
}

void NSF::SetSong(int s)
{
    song = s % songs;
    title_unknown = true;
}

bool NSF::Load(uint8_t* image, uint32_t size)
{
    if (size < 4) // no FourCC
    {
        return false;
    }

    nsf2_bits = 0;
    vrc7_type = -1;      // default
    vrc7_patches = NULL; // none

    // entries 'tlbl', 'taut', 'time', 'fade', 'psfx'
    for (unsigned int i = 0; i < NSFE_ENTRIES; ++i) {
        nsfe_entry[i].tlbl = "";
        nsfe_entry[i].taut = "";
        nsfe_entry[i].time = -1;
        nsfe_entry[i].fade = -1;
        nsfe_entry[i].psfx = false;
    }

    // 'mixe'
    for (unsigned int i = 0; i < NSFE_MIXES; ++i) nsfe_mixe[i] = NSFE_MIXE_DEFAULT;

    // load the NSF or NSFe

    memcpy(magic, image, 4);
    magic[4] = '\0';

    if (strcmp("NESM", magic)) {
        bool result = LoadNSFe(image, size, false);
        return result;
    }

    if (size < 0x80) // no header?
    {
        return false;
    }

    version = image[0x05];
    total_songs = songs = image[0x06];
    start = image[0x07];
    load_address = image[0x08] | (image[0x09] << 8);
    init_address = image[0x0a] | (image[0x0B] << 8);
    play_address = image[0x0c] | (image[0x0D] << 8);
    speed_ntsc = image[0x6e] | (image[0x6f] << 8);
    memcpy(bankswitch, image + 0x70, 8);
    speed_pal = image[0x78] | (image[0x79] << 8);
    pal_ntsc = image[0x7a];
    soundchip = image[0x7b];
    nsf2_bits = image[0x7c];
    unsigned int suffix =
        (image[0x7d] << 0) |
        (image[0x7e] << 8) |
        (image[0x7f] << 16);

    if (version < 2) nsf2_bits = 0;

    regn = CONVERT_REGN[pal_ntsc & 3];
    regn_pref = CONVERT_REGN_PREF[pal_ntsc & 3];

    speed_ntsc = 16639;
    speed_pal = 19997;
    speed_dendy = speed_pal;

    use_vrc6 = soundchip & 1 ? true : false;
    use_vrc7 = soundchip & 2 ? true : false;
    use_fds = soundchip & 4 ? true : false;
    use_mmc5 = soundchip & 8 ? true : false;
    use_n106 = soundchip & 16 ? true : false;
    use_fme7 = soundchip & 32 ? true : false;

    if (use_vrc7 || use_fme7 || use_fds || use_mmc5 || use_n106) {
        return false;
    }
    memcpy(extra, image + 0x7c, 4);

    delete[] body;
    body = new uint8_t[size - 0x80];
    memcpy(body, image + 0x80, size - 0x80);
    bodysize = size - 0x80;

    song = start - 1;

    if (suffix != 0) {
        suffix += 0x80; // add header to suffix location
        int suffix_size = size - suffix;
        if (suffix_size < 0) suffix_size = 0; // shouldn't happen?
        bool result = LoadNSFe(image + suffix, uint32_t(suffix_size), true);
        if ((nsf2_bits & 0x80) && !result) {
            return false; // NSF2 bit 7 indicates metadata parsing is mandatory
        }
        bodysize = suffix - 0x80;
    }

    return true;
}

bool NSF::LoadNSFe(uint8_t* image, uint32_t size, bool nsf2)
{
// helper for parsing strings
#define NSFE_STRING(p)                           \
    if (n >= chunk_size) break;                  \
    p = reinterpret_cast<char*>(chunk + n);      \
    while (n < chunk_size && chunk[n] != 0) ++n; \
    if (chunk[n] == 0)                           \
        ++n;                                     \
    else                                         \
        p = "<invalid>";

    // store entire file for string references, etc.
    delete[] nsfe_image;
    nsfe_image = new uint8_t[size + 1];
    ::memcpy(nsfe_image, image, size);
    nsfe_image[size] = 0; // null terminator for safety
    image = nsfe_image;

    bool info = false;
    bool data = false;
    uint32_t chunk_offset = 0;
    if (!nsf2) {
        if (size < 4) // no FourCC
        {
            return false;
        }

        memcpy(magic, image, 4);
        magic[4] = '\0';

        if (strcmp("NSFE", magic)) {
            return false;
        }
        chunk_offset = 4; // skip 'NSFE'

        // NSFe has no speed specification by default (see RATE chunk)
        speed_ntsc = 16639; // 60.09Hz
        speed_pal = 19997;  // 50.00Hz
        speed_dendy = speed_pal;
    } else {
        info = true;
        data = true;
    }

    while (true) {
        if ((size - chunk_offset) == 0) break; // end of file without NEND, acceptable.

        if ((size - chunk_offset) < 8) // not enough data for chunk size + FourCC
        {
            return false;
        }

        uint8_t* chunk = image + chunk_offset;

        unsigned int chunk_size =
            (chunk[0]) + (chunk[1] << 8) + (chunk[2] << 16) + (chunk[3] << 24);

        if ((size - chunk_offset) < (chunk_size + 8)) // not enough data for chunk
        {
            return false;
        }

        char cid[5];
        memcpy(cid, chunk + 4, 4);
        cid[4] = 0;

        chunk_offset += 8;
        chunk += 8;

        if (!strcmp(cid, "NEND")) // end of chunks
        {
            break;
        }

        if (!strcmp(cid, "INFO")) {
            if (info == true) {
                return false;
            }

            if (chunk_size < 0x0A) {
                return false;
            }

            version = 1;
            load_address = chunk[0x00] | (chunk[0x01] << 8);
            init_address = chunk[0x02] | (chunk[0x03] << 8);
            play_address = chunk[0x04] | (chunk[0x05] << 8);
            pal_ntsc = chunk[0x06];
            soundchip = chunk[0x07];
            songs = chunk[0x08];
            start = chunk[0x09] + 1; // note NSFe is 0 based, unlike NSF
            total_songs = songs;

            regn = CONVERT_REGN[pal_ntsc & 3];
            regn_pref = CONVERT_REGN_PREF[pal_ntsc & 3];

            // other variables contained in other banks
            memset(bankswitch, 0, 8);
            memset(extra, 0, 4);

            // setup derived variables
            use_vrc6 = soundchip & 1 ? true : false;
            use_vrc7 = soundchip & 2 ? true : false;
            use_fds = soundchip & 4 ? true : false;
            use_mmc5 = soundchip & 8 ? true : false;
            use_n106 = soundchip & 16 ? true : false;
            use_fme7 = soundchip & 32 ? true : false;
            song = start - 1;

            // body should follow in 'DATA' chunk
            delete[] body;
            body = NULL;
            bodysize = 0;

            // INFO chunk read
            info = true;
        } else if (!strcmp(cid, "DATA")) {
            if (!info) {
                return false;
            }
            if (data) {
                return false;
            }

            delete[] body;
            body = new uint8_t[chunk_size];
            memcpy(body, chunk, chunk_size);
            bodysize = chunk_size;

            // DATA chunk read
            data = true;
        } else if (!strcmp(cid, "BANK")) {
            for (unsigned int i = 0; i < 8 && i < chunk_size; ++i) {
                bankswitch[i] = chunk[i];
            }
        } else if (!strcmp(cid, "RATE")) {
            if (chunk_size >= 2) speed_ntsc = chunk[0] | (chunk[1] << 8);
            if (chunk_size >= 4) speed_pal = chunk[2] | (chunk[3] << 8);
            if (chunk_size >= 6) speed_dendy = chunk[4] | (chunk[5] << 8);
            if (chunk_size < 6) speed_dendy = speed_pal;
        } else if (!strcmp(cid, "NSF2")) {
            version = 2;
            if (chunk_size < 1) {
                return false;
            }
            nsf2_bits = chunk[0];
        } else if (!strcmp(cid, "VRC7")) {
            if (chunk_size < 1) {
                return false;
            } else if (chunk_size > 1 && chunk_size < (1 + 16 * 8)) {
                return false;
            }
            vrc7_type = chunk[0];
            if (vrc7_type > 1) {
                return false;
            }
            if (chunk_size >= (1 + 16 * 8)) {
                vrc7_patches = chunk + 1;
            }
        } else if (!strcmp(cid, "auth")) {
        } else if (!strcmp(cid, "plst")) {
        } else if (!strcmp(cid, "time")) {
            unsigned int i = 0;
            unsigned int n = 0;
            while (i < NSFE_ENTRIES && (n + 3) < chunk_size) {
                uint32_t value =
                    (chunk[n + 0]) + (chunk[n + 1] << 8) + (chunk[n + 2] << 16) + (chunk[n + 3] << 24);
                nsfe_entry[i].time = int32_t(value);
                ++i;
                n += 4;
            }
        } else if (!strcmp(cid, "fade")) {
            unsigned int i = 0;
            unsigned int n = 0;
            while (i < NSFE_ENTRIES && (n + 3) < chunk_size) {
                uint32_t value =
                    (chunk[n + 0]) + (chunk[n + 1] << 8) + (chunk[n + 2] << 16) + (chunk[n + 3] << 24);
                nsfe_entry[i].fade = int32_t(value);
                ++i;
                n += 4;
            }
        } else if (!strcmp(cid, "tlbl")) {
            unsigned int n = 0;
            for (unsigned int i = 0; i < NSFE_ENTRIES; ++i) {
                NSFE_STRING(nsfe_entry[i].tlbl);
            }
        } else if (!strcmp(cid, "taut")) {
            unsigned int n = 0;
            for (unsigned int i = 0; i < NSFE_ENTRIES; ++i) {
                NSFE_STRING(nsfe_entry[i].taut);
            }
        } else if (!strcmp(cid, "psfx")) {
            for (unsigned int i = 0; i < chunk_size; ++i) {
                uint8_t track = chunk[i];
                nsfe_entry[track].psfx = true;
            }
        } else if (!strcmp(cid, "text")) {
        } else if (!strcmp(cid, "mixe")) {
            unsigned int pos = 0;
            while (pos + 3 <= chunk_size) {
                unsigned int mixe_index = chunk[pos + 0];
                int16_t mixe_value = uint16_t(chunk[pos + 1] + (chunk[pos + 2] << 8));

                if (mixe_index >= NSFE_MIXES) {
                    return false; // invalid mixe index
                }

                // max value should never be used, but just in case, fake it with max-1
                // (was using max value to specify "default" instead)
                if (mixe_value == NSFE_MIXE_DEFAULT) mixe_value = NSFE_MIXE_DEFAULT - 1;

                nsfe_mixe[mixe_index] = mixe_value;
                pos += 3;
            }
        } else if (!strcmp(cid, "regn")) {
            if (chunk_size >= 1) regn = chunk[0];
            if (chunk_size >= 2) regn_pref = chunk[1];
        } else // unknown chunk
        {
            if (cid[0] >= 'A' && cid[0] <= 'Z') {
                return false;
            }
        }

        // next chunk
        chunk_offset += chunk_size;
    }

    return true;
}

} // namespace xgm
