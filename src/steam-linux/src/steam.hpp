/**
 * VGS-Zero - SDK for Steam
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2024, SUZUKI PLAN
 */
#include "../sdk/public/steam/steam_api.h"

class CSteam
{
  private:
    bool initialized;
    bool leaderboardFound;
    bool overlay;
    void (*putlog)(const char*, ...);
    InputHandle_t inputHandles[STEAM_INPUT_MAX_COUNT];
    InputActionSetHandle_t act;
    InputAnalogActionHandle_t actMove;
    InputDigitalActionHandle_t actB;
    InputDigitalActionHandle_t actA;
    InputDigitalActionHandle_t actStart;
    InputDigitalActionHandle_t actSelect;
    STEAM_CALLBACK_MANUAL(CSteam, onGameOverlayActivated, GameOverlayActivated_t, callbackGameOverlayActivated);

  public:
    CSteam(void (*putlog)(const char*, ...))
    {
        this->putlog = putlog;
        this->initialized = false;
        this->leaderboardFound = false;
        this->overlay = false;
    }

    ~CSteam()
    {
        if (this->initialized) {
            putlog("Teminating Steam...");
            SteamAPI_Shutdown();
        }
    }

    void init()
    {
        putlog("Initializing Steam...");
        if (!SteamAPI_Init()) {
            putlog("SteamAPI_Init failed");
            exit(-1);
        } else {
            this->initialized = true;
            if (!SteamUserStats()->RequestCurrentStats()) {
                putlog("SteamUserStats::RequestCurrentStats failed!");
            }
            callbackGameOverlayActivated.Register(this, &CSteam::onGameOverlayActivated);
            if (SteamInput()->Init(true)) {
                this->act = SteamInput()->GetActionSetHandle("InGameControls");
                if (!this->act) {
                    putlog("Error: action set handle does not exist!");
                }
                this->actA = SteamInput()->GetDigitalActionHandle("jump");
                if (!this->actA) {
                    putlog("Error: \"A\" action handle does not exist!");
                }
                this->actB = SteamInput()->GetDigitalActionHandle("fire ");
                if (!this->actB) {
                    putlog("Error: \"B\" action handle does not exist!");
                }
                this->actStart = SteamInput()->GetDigitalActionHandle("Start");
                if (!this->actStart) {
                    putlog("Error: \"Start\" action handle does not exist!");
                }
                this->actSelect = SteamInput()->GetDigitalActionHandle("Select");
                if (!this->actSelect) {
                    putlog("Error: \"Select\" action handle does not exist!");
                }
                this->actMove = SteamInput()->GetAnalogActionHandle("Move");
                if (!this->act) {
                    putlog("Error: \"Move\" action handle does not exist!");
                }
            } else {
                putlog("SteamInput::Init failed!");
            }
        }
    }

    uint8_t getJoypad(bool* connected)
    {
        SteamInput()->RunFrame();
        int num = SteamInput()->GetConnectedControllers(inputHandles);
        *connected = 0 < num;
        if (num < 1) {
            return 0;
        }
        uint8_t result = 0;
        for (int i = 0; i < num; i++) {
            result |= SteamInput()->GetDigitalActionData(inputHandles[i], actB).bState ? VGS0_JOYPAD_T2 : 0;
            result |= SteamInput()->GetDigitalActionData(inputHandles[i], actA).bState ? VGS0_JOYPAD_T1 : 0;
            result |= SteamInput()->GetDigitalActionData(inputHandles[i], actStart).bState ? VGS0_JOYPAD_ST : 0;
            result |= SteamInput()->GetDigitalActionData(inputHandles[i], actSelect).bState ? VGS0_JOYPAD_SE : 0;
            auto move = SteamInput()->GetAnalogActionData(inputHandles[i], actMove);
            result |= move.x < 0 ? VGS0_JOYPAD_LE : 0;
            result |= 0 < move.x ? VGS0_JOYPAD_RI : 0;
            result |= move.y < 0 ? VGS0_JOYPAD_DW : 0;
            result |= 0 < move.y ? VGS0_JOYPAD_UP : 0;
        }
        return result;
    }

    inline bool isOverlay() { return this->overlay; }
};

void CSteam::onGameOverlayActivated(GameOverlayActivated_t* args)
{
    this->overlay = args->m_bActive;
}