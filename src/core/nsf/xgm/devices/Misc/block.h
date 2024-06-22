#ifndef _BLOCK_H_
#define _BLOCK_H_
#include "../device.h"

namespace xgm
{
// Layer�ɋ���ŁC���̃f�o�C�X�ȉ��̃f�o�C�X�A�N�Z�X�����S�Ƀu���b�N����f�o�C�X
class BlockingDevice : public IDevice
{
  public:
    void Reset() {}
    bool Write(UINT32 adr, UINT32 val, UINT32 id = 0)
    {
        return blocking;
    }
    bool Read(UINT32 adr, UINT32& val, UINT32 id = 0)
    {
        return false;
    }
};
} // namespace xgm

#endif _BLOCK_H_
