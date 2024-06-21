#include "log_cpu.h"

namespace xgm
{

CPULogger::CPULogger() {}
CPULogger::~CPULogger() {}
void CPULogger::Reset() {}
bool CPULogger::Write(UINT32 adr, UINT32 val, UINT32 id) { return false; }
bool CPULogger::Read(UINT32 adr, UINT32& val, UINT32 id) { return false; }
void CPULogger::SetOption(int id, int val) {}
int CPULogger::GetLogLevel() const { return 0; }
void CPULogger::SetSoundchip(UINT8 soundchip_) {}
void CPULogger::SetFilename(const char* filename_) {}
void CPULogger::Begin(const char* title) {}
void CPULogger::Init(UINT8 reg_a, UINT8 reg_x) {}
void CPULogger::Play() {}
void CPULogger::SetCPU(NES_CPU* c) {}
void CPULogger::SetNSF(NSF* n) {}

} // namespace xgm
