#include "vgm.hpp"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

class Ym2612TestInterface : public ymfm::ymfm_interface
{
};

static void writeYm2612Register(ymfm::ym2612& chip, uint32_t reg, uint8_t value)
{
    const uint32_t port = (reg & 0x100) ? 2 : 0;
    chip.write(port, static_cast<uint8_t>(reg));
    chip.write(port + 1, value);
}

static uint64_t hashVgsxOpn2Output()
{
    Ym2612TestInterface interface;
    ymfm::ym2612 chip(interface);
    chip.reset();
    writeYm2612Register(chip, 0x22, 0x08);
    for (uint32_t reg = 0x30; reg <= 0x9f; reg++)
        writeYm2612Register(chip, reg, static_cast<uint8_t>((reg * 37 + 11) & 0xff));
    writeYm2612Register(chip, 0xa0, 0x98);
    writeYm2612Register(chip, 0xa4, 0x22);
    writeYm2612Register(chip, 0xb0, 0x32);
    writeYm2612Register(chip, 0xb4, 0xc7);
    writeYm2612Register(chip, 0x28, 0xf0);

    uint64_t hash = 1469598103934665603ull;
    for (int i = 0; i < 4096; i++) {
        ymfm::ym2612::output_data output;
        chip.generate(&output);
        hash ^= static_cast<uint32_t>(output.data[0]);
        hash *= 1099511628211ull;
        hash ^= static_cast<uint32_t>(output.data[1]);
        hash *= 1099511628211ull;
    }
    return hash;
}

static void makeDacVgm(uint8_t data[0x110])
{
    memset(data, 0, 0x110);
    memcpy(data, "Vgm ", 4);
    const uint32_t version = 0x171;
    const uint32_t ym2612Clock = 7670454;
    const uint32_t dataOffset = 0xcc;
    memcpy(&data[0x08], &version, sizeof(version));
    memcpy(&data[0x2c], &ym2612Clock, sizeof(ym2612Clock));
    memcpy(&data[0x34], &dataOffset, sizeof(dataOffset));
    const uint8_t commands[] = {
        0x52,
        0x2b,
        0x80,
        0x52,
        0x2a,
        0xff,
        0x61,
        0x00,
        0x04,
        0x66,
    };
    memcpy(&data[0x100], commands, sizeof(commands));
}

static uint64_t hashRenderedDac(bool analogEnabled)
{
    uint8_t data[0x110];
    int16_t samples[512];
    makeDacVgm(data);
    VgmManager manager;
    if (analogEnabled)
        manager.useYm2612AnalogRealPreset();
    assert(manager.load(data, sizeof(data)));
    manager.render(samples, 512);

    uint64_t hash = 1469598103934665603ull;
    for (int i = 0; i < 512; i++) {
        hash ^= static_cast<uint16_t>(samples[i]);
        hash *= 1099511628211ull;
    }
    return hash;
}

int main()
{
    // This value is produced by the matching VGS-X OPN2 core for this register stream.
    assert(hashVgsxOpn2Output() == 0xef8937cff6d33589ull);

    VgmManager manager;
    assert(!manager.isYm2612AnalogEnabled());
    manager.setYm2612AnalogEnabled(true);
    assert(manager.isYm2612AnalogEnabled());
    manager.reset();
    assert(manager.isYm2612AnalogEnabled());
    manager.useYm2612AnalogCleanPreset();
    assert(!manager.isYm2612AnalogEnabled());
    manager.useYm2612AnalogSubtlePreset();
    assert(manager.isYm2612AnalogEnabled());
    manager.useYm2612AnalogRealPreset();
    assert(manager.isYm2612AnalogEnabled());
    manager.useYm2612AnalogRe1ePreset();
    assert(manager.isYm2612AnalogEnabled());
    manager.useYm2612AnalogWarmPreset();
    assert(manager.isYm2612AnalogEnabled());

    const uint64_t cleanHash = hashRenderedDac(false);
    const uint64_t analogHash = hashRenderedDac(true);
    // These hashes are produced by VGS-X's VgmDriver with the same VGM input.
    assert(cleanHash == 0x19df7ca6a32b639eull);
    assert(analogHash == 0xf3a3ab0907379172ull);
    printf("OPN2=%016llx clean=%016llx analog=%016llx\n",
           static_cast<unsigned long long>(hashVgsxOpn2Output()),
           static_cast<unsigned long long>(cleanHash),
           static_cast<unsigned long long>(analogHash));
    return 0;
}
