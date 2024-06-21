#ifndef _DEVINFO_H_
#define _DEVINFO_H_
#include <math.h>
// �f�o�C�X���
namespace xgm
{
class IDeviceInfo
{
  public:
    virtual IDeviceInfo* Clone() = 0;
    virtual ~IDeviceInfo(){};
};

class ITrackInfo : public IDeviceInfo
{
  public:
    virtual IDeviceInfo* Clone() = 0;
    // ���݂̏o�͒l�����̂܂ܕԂ�
    virtual INT32 GetOutput() = 0;
    // ���g����Hz�ŕԂ�
    virtual double GetFreqHz() = 0;
    // ���g�����f�o�C�X�ˑ��l�ŕԂ��D
    virtual UINT32 GetFreq() = 0;
    // ���ʂ�Ԃ�
    virtual INT32 GetVolume() = 0;
    // ���ʂ̍ő�l��Ԃ�
    virtual INT32 GetMaxVolume() = 0;
    // �������Ȃ�true OFF�Ȃ�false
    virtual bool GetKeyStatus() = 0;
    // �g�[���ԍ�
    virtual INT32 GetTone() = 0;

    // ���g�����m�[�g�ԍ��ɕϊ��D0x60��o4c 0�͖���
    static int GetNote(double freq)
    {
        const double LOG2_440 = 8.7813597135246596040696824762152;
        const double LOG_2 = 0.69314718055994530941723212145818;
        const int NOTE_440HZ = 0x69;

        if (freq > 1.0)
            return (int)((12 * (log(freq) / LOG_2 - LOG2_440) + NOTE_440HZ + 0.5));
        else
            return 0;
    }
};

/* TrackInfo �� �o�b�t�@�����O */
class InfoBuffer
{
    int bufmax;
    int index;
    struct DevicePair {
        IDeviceInfo* ptr;
        int pos;
    } buffer[1024];

  public:
    InfoBuffer(int max = 60 * 10)
    {
        index = 0;
        bufmax = max;
        for (int i = 0; i < bufmax; i++) {
            buffer[i].ptr = nullptr;
        }
    }
    virtual ~InfoBuffer()
    {
        for (int i = 0; i < bufmax; i++) {
            if (buffer[i].ptr) {
                delete buffer[i].ptr;
                buffer[i].ptr = nullptr;
            }
        }
    }

    virtual void Clear()
    {
        for (int i = 0; i < bufmax; i++) {
            if (buffer[i].ptr) {
                delete buffer[i].ptr;
                buffer[i].ptr = nullptr;
            }
        }
    }

    virtual void AddInfo(int pos, IDeviceInfo* di)
    {
        if (di) {
            if (buffer[index].ptr) {
                delete buffer[index].ptr;
            }
            buffer[index].pos = pos;
            buffer[index].ptr = di->Clone();
            index = (index + 1) % bufmax;
        }
    }

    virtual IDeviceInfo* GetInfo(int pos)
    {
        if (pos == -1)
            return buffer[(index + bufmax - 1) % bufmax].ptr;

        for (int i = (index + bufmax - 1) % bufmax; i != index; i = (i + bufmax - 1) % bufmax)
            if (buffer[i].pos <= pos) return buffer[i].ptr;
        return NULL;
    }
};

class TrackInfoBasic : public ITrackInfo
{
  public:
    INT32 output;
    INT32 volume;
    INT32 max_volume;
    UINT32 _freq;
    double freq;
    bool key;
    INT32 tone;
    virtual IDeviceInfo* Clone() { return new TrackInfoBasic(*this); }
    virtual INT32 GetOutput() { return output; }
    virtual double GetFreqHz() { return freq; }
    virtual UINT32 GetFreq() { return _freq; }
    virtual bool GetKeyStatus() { return key; }
    virtual INT32 GetVolume() { return volume; }
    virtual INT32 GetMaxVolume() { return max_volume; }
    virtual INT32 GetTone() { return tone; };
};

} // namespace xgm
#endif
