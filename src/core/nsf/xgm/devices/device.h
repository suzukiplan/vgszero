#ifndef _DEVICE_H_
#define _DEVICE_H_
#include <stdio.h>
#include <assert.h>
#include "../xtypes.h"
#include "devinfo.h"
#include "../debugout.h"

namespace xgm
{
const double DEFAULT_CLOCK = 1789772.0;
const int DEFAULT_RATE = 48000;

/**
 * �G�~�����[�^�Ŏg�p����f�o�C�X�̒���
 */
class IDevice
{
  public:
    /**
     * �f�o�C�X�̃��Z�b�g
     *
     * <P>
     * ���̃��\�b�h�̌Ăяo����C���̃f�o�C�X�͂ǂ̂悤�ȃ��\�b�h�̌Ă�
     * �o���ɑ΂��Ă��C���s���G���[���N�����Ă͂Ȃ�Ȃ��D�t�ɁC���̃��\�b
     * �h���ĂԈȑO�́C���̃��\�b�h�̓���͈�ؕۏ؂��Ȃ��Ă��ǂ��B
     * </P>
     */
    virtual void Reset() = 0;

    /**
     * �f�o�C�X�ւ̏�������
     *
     * @param adr �A�h���X
     * @param val �������ޒl
     * @param id  �f�o�C�X���ʏ��D��̃f�o�C�X��������IO���T�|�[�g���鎞�Ȃ�
     * @return ������ true ���s�� false
     */
    virtual bool Write(UINT32 adr, UINT32 val, UINT32 id = 0) = 0;

    /**
     * �f�o�C�X����ǂݍ���
     *
     * @param adr �A�h���X
     * @param val �ǂݏo�����l���󂯎��ϐ��D
     * @return ������ true ���s�� false
     */
    virtual bool Read(UINT32 adr, UINT32& val, UINT32 id = 0) = 0;

    /**
     * �e��I�v�V������ݒ肷��(���������)
     */
    virtual void SetOption(int id, int val){};
    virtual ~IDevice(){};
};

/**
 * �C���^�[�t�F�[�X�F�����̃����_�����O���\�ȃN���X
 */
class IRenderable
{
  public:
    /**
     * �����̃����_�����O
     *
     * @param b[2] �������ꂽ�f�[�^���i�[����z��D
     * b[0]�����`�����l���Cb[1]���E�`�����l���̉����f�[�^�D
     * @return ���������f�[�^�̃T�C�Y�D1�Ȃ烂�m�����D2�Ȃ�X�e���I�D0�͍������s�D
     */
    virtual UINT32 Render(INT32 b[2]) = 0;

    // When seeking, this replaces Render
    virtual void Skip() {}

    /**
     *  chip update/operation is now bound to CPU clocks
     *  Render() now simply mixes and outputs sound
     */
    virtual void Tick(UINT32 clocks) {}
    virtual ~IRenderable(){};
};

/**
 * ���������`�b�v
 */
class ISoundChip : public IDevice, virtual public IRenderable
{
  public:
    /**
     * Soundchip clocked by M2 (NTSC = ~1.789MHz)
     */
    virtual void Tick(UINT32 clocks) = 0;

    /**
     * �`�b�v�̓���N���b�N��ݒ�
     *
     * @param clock ������g��
     */
    virtual void SetClock(double clock) = 0;

    /**
     * �����������[�g�ݒ�
     *
     * @param rate �o�͎��g��
     */
    virtual void SetRate(double rate) = 0;

    /**
     * Channel mask.
     */
    virtual void SetMask(int mask) = 0;

    /**
     * Stereo mix.
     *   mixl = 0-256
     *   mixr = 0-256
     *     128 = neutral
     *     256 = double
     *     0 = nil
     *    <0 = inverted
     */
    virtual void SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr) = 0;

    /**
     * Track info for keyboard view.
     */
    virtual ITrackInfo* GetTrackInfo(int trk) { return NULL; }
    virtual ~ISoundChip(){};
};

/**
 * �o�X
 *
 * <P>
 * �����̃f�o�C�X�ɁC���Z�b�g�C�������݁C�ǂݍ��ݓ�����u���[�h�L���X�g����D
 * <P>
 */
class Bus : public IDevice
{
  protected:
    IDevice* vd_ptr[256];
    int vd_num;

  public:
    /**
     * ���Z�b�g
     *
     * <P>
     * ���t�����Ă���S�Ẵf�o�C�X�́CReset���\�b�h���Ăяo���D
     * �Ăяo�������́C�f�o�C�X�����t����ꂽ�����ɓ������D
     * </P>
     */
    void Reset()
    {
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Reset();
        }
    }

    /**
     * �S�f�o�C�X�̎��O��
     */
    void DetachAll()
    {
        vd_num = 0;
    }

    /**
     * �f�o�C�X�̎��t��
     *
     * <P>
     * ���̃o�X�Ƀf�o�C�X�����t����D
     * </P>
     *
     * @param d ���t����f�o�C�X�ւ̃|�C���^
     */
    void Attach(IDevice* d)
    {
        vd_ptr[vd_num++] = d;
    }

    /**
     * ��������
     *
     * <P>
     * ���t�����Ă���S�Ẵf�o�C�X�́CWrite���\�b�h���Ăяo���D
     * �Ăяo�������́C�f�o�C�X�����t����ꂽ�����ɓ������D
     * </P>
     */
    bool Write(UINT32 adr, UINT32 val, UINT32 id = 0)
    {
        bool ret = false;
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Write(adr, val);
        }
        return ret;
    }

    /**
     * �ǂݍ���
     *
     * <P>
     * ���t�����Ă���S�Ẵf�o�C�X��Read���\�b�h���Ăяo���D
     * �Ăяo�������́C�f�o�C�X�����t����ꂽ�����ɓ������D
     * �A��l�͗L����(Read���\�b�h��true��ԋp����)�f�o�C�X��
     * �Ԃ�l�̘_���a�D
     * </P>
     */
    bool Read(UINT32 adr, UINT32& val, UINT32 id = 0)
    {
        bool ret = false;
        UINT32 vtmp = 0;
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

/**
 * ���C���[
 *
 * <P>
 * �o�X�Ǝ��Ă��邪�C�ǂݏ����̓����S�f�o�C�X�ɓ`�d�����Ȃ��D
 * �ŏ��ɓǂݏ����ɐ��������f�o�C�X�𔭌��������_�ŏI������D
 * </P>
 */
class Layer : public Bus
{
  protected:
  public:
    /**
     * ��������
     *
     * <P>
     * ���t�����Ă���f�o�C�X��Write���\�b�h���Ăяo���D
     * �Ăяo�������́C�f�o�C�X�����t����ꂽ�����ɓ������D
     * Write�ɐ��������f�o�C�X�������������_�ŏI���D
     * </P>
     */
    bool Write(UINT32 adr, UINT32 val, UINT32 id = 0)
    {
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Write(adr, val)) {
                return true;
            }
        }
        return false;
    }

    /**
     * �ǂݍ���
     *
     * <P>
     * ���t�����Ă���f�o�C�X��Read���\�b�h���Ăяo���D
     * �Ăяo�������́C�f�o�C�X�����t����ꂽ�����ɓ������D
     * Read�ɐ��������f�o�C�X�������������_�ŏI���D
     * </P>
     */
    bool Read(UINT32 adr, UINT32& val, UINT32 id = 0)
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
