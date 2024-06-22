#ifndef _DEVICE_H_
#define _DEVICE_H_
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

namespace xgm
{
const int DEFAULT_CLOCK = 1789772;
const int DEFAULT_RATE = 48000;

class IDevice
{
  public:
    virtual void Reset() = 0;
    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0) = 0;
    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0) = 0;
    virtual void SetOption(int id, int val) {};
    virtual ~IDevice(){};
};

class IRenderable
{
  public:
    virtual uint32_t Render(int32_t b[2]) = 0;
    virtual void Skip() {}
    virtual void Tick(uint32_t clocks) {}
    virtual ~IRenderable(){};
};

class ISoundChip : public IDevice, virtual public IRenderable
{
  public:
    virtual void Tick(uint32_t clocks) = 0;
    virtual void SetClock(long clock) = 0;
    virtual void SetRate(long rate) = 0;
    virtual void SetMask(int mask) = 0;
    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr) = 0;
    virtual ~ISoundChip(){};
};

class Bus : public IDevice
{
  protected:
    IDevice* vd_ptr[256];
    int vd_num;

  public:
    void Reset()
    {
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Reset();
        }
    }

    void DetachAll()
    {
        vd_num = 0;
    }

    void Attach(IDevice* d)
    {
        vd_ptr[vd_num++] = d;
    }

    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        bool ret = false;
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Write(adr, val);
        }
        return ret;
    }

    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        bool ret = false;
        uint32_t vtmp = 0;
        val = 0;
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Read(adr, vtmp)) {
                val |= vtmp;
                ret = true;
            }
        }
        return ret;
    }
};

class Layer : public Bus
{
  public:
    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Write(adr, val)) {
                return true;
            }
        }
        return false;
    }

    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        val = 0;
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Read(adr, val)) {
                return true;
            }
        }
        return false;
    }
};

} // namespace xgm

#endif
