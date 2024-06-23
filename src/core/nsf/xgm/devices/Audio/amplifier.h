#ifndef _AMPLIFIER_H_
#define _AMPLIFIER_H_
#include <math.h>

namespace xgm
{

class Amplifier : virtual public IRenderable
{
  protected:
    IRenderable* target;
    int mute, volume;
    int combo;

  public:
    Amplifier()
    {
        target = NULL;
        mute = false;
        volume = 64;
    }

    ~Amplifier()
    {
    }

    void Attach(IRenderable* p)
    {
        target = p;
    }

    void Tick(uint32_t clocks)
    {
        target->Tick(clocks);
    }

    uint32_t Render(int32_t b[2])
    {
        if (mute) {
            b[0] = b[1] = 0;
            return 2;
        }
        target->Render(b);
        b[0] = (b[0] * volume) / 16;
        b[1] = (b[1] * volume) / 16;

        return 2;
    }

    void SetVolume(int v)
    {
        volume = v;
    }
    int GetVolume()
    {
        return volume;
    }
    void SetMute(int m)
    {
        mute = m;
    }
    int GetMute()
    {
        return mute;
    }
};

} // namespace xgm

#endif
