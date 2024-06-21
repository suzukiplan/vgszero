#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <set>
#include "../xtypes.h"
#include "../debugout.h"
#include "../../vcm/vcm.h"
#include "soundData.h"
#include "../devices/device.h"

namespace xgm
{

/**
 * ���������n�̐ݒ���Ǘ�����N���X
 * @see Player
 */
class PlayerConfig : public vcm::Configuration
{
};

/**
 * ���������n
 */
class Player : public vcm::ObserverI
{
  protected:
    PlayerConfig* config;

  public:
    /**
     * GetLoopCount���\�b�h�̓��ʂȖ߂�l
     * @see GetLoopCount
     */
    enum { NEVER_LOOP = -1,
           INFINITE_LOOP = 0 };

    Player()
    {
        config = NULL;
    }

    virtual ~Player()
    {
        if (config != NULL)
            config->DetachObserver(this);
    }

    /**
     * PlayerConfig�I�u�W�F�N�g���A�^�b�`����
     * <P>
     * PlayerConfig�I�u�W�F�N�g�̃I�u�U�[�o�ɁC���̃I�u�W�F�N�g�������I�ɓo�^�����D
     * </P>
     * @param pc �A�^�b�`����PlayerConfig�I�u�W�F�N�g
     */
    virtual void SetConfig(PlayerConfig* pc)
    {
        config = pc;
        config->AttachObserver(this);
    }

    virtual PlayerConfig* GetConfig()
    {
        return config;
    }

    /**
     * PlayerConfig�I�u�W�F�N�g���X�V���ꂽ���ɃR�[�������G���g��
     * @param param �X�V�X�e�[�^�X
     */
    virtual void Notify(int param){};

    /**
     * ���t�f�[�^�����[�h����
     * <P>
     * Player�I�u�W�F�N�g�́A�����ɉ��t�f�[�^�̃R�s�[���i�[���Ȃ��B
     * ���������āAPlayer�I�u�W�F�N�g�̉��t���ɉ��t�f�[�^�����ł����Ă͂����Ȃ��B
     * ���t�f�[�^�̐����Ə��ł�Player�I�u�W�F�N�g�̊O���ŊǗ�����B
     * </P>
     * @param data ���t�f�[�^�ւ̃|�C���^
     * @return ������ ture ���s�� false
     */
    virtual bool Load(SoundData* sdat) = 0;

    /**
     * �v���C��������������
     */
    virtual void Reset() = 0;

    /**
     * �Đ����[�g��ݒ肷��
     */
    virtual void SetPlayFreq(double rate) = 0;

    /**
     * Number of channels to output.
     */
    virtual void SetChannels(int channels) = 0;

    /**
     * �����f�[�^�̃����_�����O���s��
     * <P>
     * �o�b�t�@�T�C�Y�� samples * sizeof(INT16)�K�v
     * </P>
     * @param buf �����_�����O�f�[�^���i�[����o�b�t�@
     * @param samples �T���v���̌�
     *                0��^�����Ă��n���O�A�b�v���Ă͂Ȃ�Ȃ�
     * @return ���ۂɐ������ꂽ�T���v����
     */
    virtual UINT32 Render(INT16* buf, UINT32 samples) = 0;

    /** �t�F�[�h�A�E�g */
    virtual void FadeOut(int fade_in_ms) = 0;

    /**
     * �����f�[�^�̃����_�����O���X�L�b�v����
     * @param samples �X�L�b�v����T���v����
     *                0��^�����Ă��n���O�A�b�v���Ă͂Ȃ�Ȃ��D
     * @return ���ۂɃX�L�b�v�����T���v����
     */
    virtual UINT32 Skip(UINT32 samples) = 0;

    /**
     * ���t����~�������ǂ����𒲂ׂ�D
     * @return ��~���Ȃ�true�D���t���Ȃ�false�D
     */
    virtual bool IsStopped() = 0;

    /**
     * ���t�����[�v�����񐔂��`�F�b�N����D
     * <P>
     * ����̉��t��1���[�v�ڂƐ�����D
     * </P>
     * @return ���t�����[�v�����񐔁D
     NEVER_LOOP�̏ꍇ�̓��[�v���Ȃ��f�[�^�CINFINITE_LOOP�̏ꍇ�͖������[�v����f�[�^�ł���D
     */
    virtual int GetLoopCount() { return NEVER_LOOP; }
    virtual const char* GetTitleString() { return "UNKNOWN"; }
    virtual int GetLength() { return 5 * 60 * 1000; }

    /* �ԍ�id�̎���time�ł̃f�o�C�X�����擾 time==-1�̎��͌��݂̃f�o�C�X����Ԃ� */
    virtual IDeviceInfo* GetInfo(int time_in_ms, int device_id) { return NULL; }
};

class PlayerMSP : public Player
{
  public:
    PlayerMSP() : Player(){};
    virtual ~PlayerMSP(){};
    /**
     * ���̋Ȃɐi��
     * @param step �i�߂�Ȑ�
     * @return ������ true ���s�� false
     */
    virtual bool NextSong(int s) { return false; }

    /**
     * �O�̋Ȃɖ߂�
     * @param step �߂�Ȑ�
     * @return ������ true ���s�� false
     */
    virtual bool PrevSong(int s) { return false; }

    /**
     * �Ȕԍ��𒼐ڐݒ肷��
     * @param song �Ȕԍ�
     * @return ������ true ���s�� false
     */
    virtual bool SetSong(int song) { return false; }

    /**
     * ���ݑI�𒆂̋Ȕԍ����l������
     * @return �Ȕԍ�
     */
    virtual int GetSong() { return -1; }
};

} // namespace xgm
#endif
