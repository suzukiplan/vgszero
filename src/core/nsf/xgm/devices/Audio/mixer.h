#ifndef _MIXER_H_
#define _MIXER_H_
#include "../device.h"

namespace xgm
{
class Mixer : virtual public IRenderable
{
  protected:
    IRenderable* dlist[256];
    int dlist_num;

  public:
    Mixer()
    {
        Reset();
    }

    ~Mixer()
    {
    }

    void DetachAll()
    {
        dlist_num = 0;
    }

    void Attach(IRenderable* dev)
    {
        dlist[dlist_num++] = dev;
    }

    void Reset()
    {
    }

    virtual void Skip()
    {
        // Should pass Skip on to all attached devices, but nothing currently requires it.

        // DeviceList::iterator it;
        // for (it = dlist.begin (); it != dlist.end (); it++)
        //{
        //   (*it)->Skip ();
        // }
    }

    virtual void Tick(UINT32 clocks)
    {
        for (int i = 0; i < dlist_num; i++) {
            dlist[i]->Tick(clocks);
        }
    }

    virtual UINT32 Render(INT32 b[2])
    {
        INT32 tmp[2];

        b[0] = b[1] = 0;

        for (int i = 0; i < dlist_num; i++) {
            dlist[i]->Render(tmp);
            b[0] += tmp[0];
            b[1] += tmp[1];
        }

        return 2;
    }
};

} // namespace xgm

#endif
