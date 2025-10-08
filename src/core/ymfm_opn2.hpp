/**
 * Commented by Yoji Suzuki
 * This code extracts only the classes necessary for emulating the YM2612 (ym2612 class)
 * from ymfm, and modifies it into a single-header format while including its dependency code.
 * The license terms comply with the original ymfm.
 */

// BSD 3-Clause License
//
// Copyright (c) 2021, Aaron Giles
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef YMFM_H
#define YMFM_H

#pragma once

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#define YMFM_DEBUG_LOG_WAVFILES (0)

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

namespace ymfm
{

template <typename T>
inline T ymfm_max_value(T a, T b)
{
    return (a > b) ? a : b;
}

template <typename T>
inline T ymfm_min_value(T a, T b)
{
    return (a < b) ? a : b;
}

struct ymfm_state_buffer
{
    uint8_t* data;
    size_t size;
    size_t capacity;
};

static inline void ymfm_state_buffer_init(ymfm_state_buffer& buffer)
{
    buffer.data = nullptr;
    buffer.size = 0;
    buffer.capacity = 0;
}

static inline void ymfm_state_buffer_clear(ymfm_state_buffer& buffer)
{
    buffer.size = 0;
}

static inline void ymfm_state_buffer_free(ymfm_state_buffer& buffer)
{
    if (buffer.data != nullptr) {
        free(buffer.data);
        buffer.data = nullptr;
    }
    buffer.size = 0;
    buffer.capacity = 0;
}

static inline int ymfm_state_buffer_push(ymfm_state_buffer& buffer, uint8_t value)
{
    if (buffer.size >= buffer.capacity) {
        size_t new_capacity = (buffer.capacity == 0) ? 64 : buffer.capacity * 2;
        if (new_capacity <= buffer.size)
            new_capacity = buffer.size + 1;
        uint8_t* new_data = static_cast<uint8_t*>(realloc(buffer.data, new_capacity * sizeof(uint8_t)));
        if (new_data == nullptr)
            return 0;
        buffer.data = new_data;
        buffer.capacity = new_capacity;
    }
    buffer.data[buffer.size++] = value;
    return 1;
}

static inline size_t ymfm_state_buffer_size(const ymfm_state_buffer& buffer)
{
    return buffer.size;
}

static inline uint8_t* ymfm_state_buffer_data(ymfm_state_buffer& buffer)
{
    return buffer.data;
}

//*********************************************************
//  DEBUGGING
//*********************************************************

class debug
{
  public:
    // masks to help isolate specific channels
    static constexpr uint32_t GLOBAL_FM_CHANNEL_MASK = 0xffffffff;
    static constexpr uint32_t GLOBAL_ADPCM_A_CHANNEL_MASK = 0xffffffff;
    static constexpr uint32_t GLOBAL_ADPCM_B_CHANNEL_MASK = 0xffffffff;
    static constexpr uint32_t GLOBAL_PCM_CHANNEL_MASK = 0xffffffff;

    // types of logging
    static constexpr bool LOG_FM_WRITES = false;
    static constexpr bool LOG_KEYON_EVENTS = false;
    static constexpr bool LOG_UNEXPECTED_READ_WRITES = false;

    // helpers to write based on the log type
    template <typename... Params>
    static void log_fm_write(Params&&... args)
    {
        if (LOG_FM_WRITES) log(args...);
    }
    template <typename... Params>
    static void log_keyon(Params&&... args)
    {
        if (LOG_KEYON_EVENTS) log(args...);
    }
    template <typename... Params>
    static void log_unexpected_read_write(Params&&... args)
    {
        if (LOG_UNEXPECTED_READ_WRITES) log(args...);
    }

    // downstream helper to output log data; defaults to printf
    template <typename... Params>
    static void log(Params&&... args) { printf(args...); }
};

//*********************************************************
//  GLOBAL HELPERS
//*********************************************************

//-------------------------------------------------
//  bitfield - extract a bitfield from the given
//  value, starting at bit 'start' for a length of
//  'length' bits
//-------------------------------------------------

inline uint32_t bitfield(uint32_t value, int start, int length = 1)
{
    return (value >> start) & ((1 << length) - 1);
}

//-------------------------------------------------
//  clamp - clamp between the minimum and maximum
//  values provided
//-------------------------------------------------

inline int32_t clamp(int32_t value, int32_t minval, int32_t maxval)
{
    if (value < minval)
        return minval;
    if (value > maxval)
        return maxval;
    return value;
}

//-------------------------------------------------
//  count_leading_zeros - return the number of
//  leading zeros in a 32-bit value; CPU-optimized
//  versions for various architectures are included
//  below
//-------------------------------------------------

#if defined(__GNUC__)

inline uint8_t count_leading_zeros(uint32_t value)
{
    if (value == 0)
        return 32;
    return __builtin_clz(value);
}

#elif defined(_MSC_VER)

inline uint8_t count_leading_zeros(uint32_t value)
{
    unsigned long index;
    return _BitScanReverse(&index, value) ? uint8_t(31U - index) : 32U;
}

#else

inline uint8_t count_leading_zeros(uint32_t value)
{
    if (value == 0)
        return 32;
    uint8_t count;
    for (count = 0; int32_t(value) >= 0; count++)
        value <<= 1;
    return count;
}

#endif

// Many of the Yamaha FM chips emit a floating-point value, which is sent to
// a DAC for processing. The exact format of this floating-point value is
// documented below. This description only makes sense if the "internal"
// format treats sign as 1=positive and 0=negative, so the helpers below
// presume that.
//
// Internal OPx data      16-bit signed data     Exp Sign Mantissa
// =================      =================      === ==== ========
// 1 1xxxxxxxx------  ->  0 1xxxxxxxx------  ->  111   1  1xxxxxxx
// 1 01xxxxxxxx-----  ->  0 01xxxxxxxx-----  ->  110   1  1xxxxxxx
// 1 001xxxxxxxx----  ->  0 001xxxxxxxx----  ->  101   1  1xxxxxxx
// 1 0001xxxxxxxx---  ->  0 0001xxxxxxxx---  ->  100   1  1xxxxxxx
// 1 00001xxxxxxxx--  ->  0 00001xxxxxxxx--  ->  011   1  1xxxxxxx
// 1 000001xxxxxxxx-  ->  0 000001xxxxxxxx-  ->  010   1  1xxxxxxx
// 1 000000xxxxxxxxx  ->  0 000000xxxxxxxxx  ->  001   1  xxxxxxxx
// 0 111111xxxxxxxxx  ->  1 111111xxxxxxxxx  ->  001   0  xxxxxxxx
// 0 111110xxxxxxxx-  ->  1 111110xxxxxxxx-  ->  010   0  0xxxxxxx
// 0 11110xxxxxxxx--  ->  1 11110xxxxxxxx--  ->  011   0  0xxxxxxx
// 0 1110xxxxxxxx---  ->  1 1110xxxxxxxx---  ->  100   0  0xxxxxxx
// 0 110xxxxxxxx----  ->  1 110xxxxxxxx----  ->  101   0  0xxxxxxx
// 0 10xxxxxxxx-----  ->  1 10xxxxxxxx-----  ->  110   0  0xxxxxxx
// 0 0xxxxxxxx------  ->  1 0xxxxxxxx------  ->  111   0  0xxxxxxx

//-------------------------------------------------
//  encode_fp - given a 32-bit signed input value
//  convert it to a signed 3.10 floating-point
//  value
//-------------------------------------------------

inline int16_t encode_fp(int32_t value)
{
    // handle overflows first
    if (value < -32768)
        return (7 << 10) | 0x000;
    if (value > 32767)
        return (7 << 10) | 0x3ff;

    // we need to count the number of leading sign bits after the sign
    // we can use count_leading_zeros if we invert negative values
    int32_t scanvalue = value ^ (int32_t(value) >> 31);

    // exponent is related to the number of leading bits starting from bit 14
    int exponent = 7 - count_leading_zeros(scanvalue << 17);

    // smallest exponent value allowed is 1
    exponent = ymfm_max_value(exponent, 1);

    // mantissa
    int32_t mantissa = value >> (exponent - 1);

    // assemble into final form, inverting the sign
    return ((exponent << 10) | (mantissa & 0x3ff)) ^ 0x200;
}

//-------------------------------------------------
//  decode_fp - given a 3.10 floating-point value,
//  convert it to a signed 16-bit value
//-------------------------------------------------

inline int16_t decode_fp(int16_t value)
{
    // invert the sign and the exponent
    value ^= 0x1e00;

    // shift mantissa up to 16 bits then apply inverted exponent
    return int16_t(value << 6) >> bitfield(value, 10, 3);
}

//-------------------------------------------------
//  roundtrip_fp - compute the result of a round
//  trip through the encode/decode process above
//-------------------------------------------------

inline int16_t roundtrip_fp(int32_t value)
{
    // handle overflows first
    if (value < -32768)
        return -32768;
    if (value > 32767)
        return 32767;

    // we need to count the number of leading sign bits after the sign
    // we can use count_leading_zeros if we invert negative values
    int32_t scanvalue = value ^ (int32_t(value) >> 31);

    // exponent is related to the number of leading bits starting from bit 14
    int exponent = 7 - count_leading_zeros(scanvalue << 17);

    // smallest exponent value allowed is 1
    exponent = ymfm_max_value(exponent, 1);

    // apply the shift back and forth to zero out bits that are lost
    exponent -= 1;
    int32_t mask = (1 << exponent) - 1;
    return value & ~mask;
}

//*********************************************************
//  HELPER CLASSES
//*********************************************************

// various envelope states
enum envelope_state : uint32_t {
    EG_DEPRESS = 0, // OPLL only; set EG_HAS_DEPRESS to enable
    EG_ATTACK = 1,
    EG_DECAY = 2,
    EG_SUSTAIN = 3,
    EG_RELEASE = 4,
    EG_REVERB = 5, // OPQ/OPZ only; set EG_HAS_REVERB to enable
    EG_STATES = 6
};

// external I/O access classes
enum access_class : uint32_t {
    ACCESS_IO = 0,
    ACCESS_ADPCM_A,
    ACCESS_ADPCM_B,
    ACCESS_PCM,
    ACCESS_CLASSES
};

//*********************************************************
//  HELPER CLASSES
//*********************************************************

// ======================> ymfm_output

// struct containing an array of output values
template <int NumOutputs>
struct ymfm_output {
    // clear all outputs to 0
    ymfm_output& clear()
    {
        for (uint32_t index = 0; index < NumOutputs; index++)
            data[index] = 0;
        return *this;
    }

    // clamp all outputs to a 16-bit signed value
    ymfm_output& clamp16()
    {
        for (uint32_t index = 0; index < NumOutputs; index++)
            data[index] = clamp(data[index], -32768, 32767);
        return *this;
    }

    // run each output value through the floating-point processor
    ymfm_output& roundtrip_fp()
    {
        for (uint32_t index = 0; index < NumOutputs; index++)
            data[index] = ymfm::roundtrip_fp(data[index]);
        return *this;
    }

    // internal state
    int32_t data[NumOutputs];
};

// ======================> ymfm_saved_state

// this class contains a managed buffer of bytes that is used to save and
// restore state
class ymfm_saved_state
{
  public:
    // construction
    ymfm_saved_state(ymfm_state_buffer& buffer, bool saving) : m_buffer(buffer),
                                                               m_offset(saving ? -1 : 0)
    {
        if (saving)
            ymfm_state_buffer_clear(m_buffer);
    }

    // are we saving or restoring?
    bool saving() const { return (m_offset < 0); }

    // generic save/restore
    template <typename DataType>
    void save_restore(DataType& data)
    {
        if (saving())
            save(data);
        else
            restore(data);
    }

  public:
    // save data to the buffer
    void save(bool& data) { write(data ? 1 : 0); }
    void save(int8_t& data) { write(data); }
    void save(uint8_t& data) { write(data); }
    void save(int16_t& data) { write(uint8_t(data)).write(data >> 8); }
    void save(uint16_t& data) { write(uint8_t(data)).write(data >> 8); }
    void save(int32_t& data) { write(data).write(data >> 8).write(data >> 16).write(data >> 24); }
    void save(uint32_t& data) { write(data).write(data >> 8).write(data >> 16).write(data >> 24); }
    void save(envelope_state& data) { write(uint8_t(data)); }
    template <typename DataType, int Count>
    void save(DataType (&data)[Count])
    {
        for (uint32_t index = 0; index < Count; index++) save(data[index]);
    }

    // restore data from the buffer
    void restore(bool& data) { data = read() ? true : false; }
    void restore(int8_t& data) { data = read(); }
    void restore(uint8_t& data) { data = read(); }
    void restore(int16_t& data)
    {
        data = read();
        data |= read() << 8;
    }
    void restore(uint16_t& data)
    {
        data = read();
        data |= read() << 8;
    }
    void restore(int32_t& data)
    {
        data = read();
        data |= read() << 8;
        data |= read() << 16;
        data |= read() << 24;
    }
    void restore(uint32_t& data)
    {
        data = read();
        data |= read() << 8;
        data |= read() << 16;
        data |= read() << 24;
    }
    void restore(envelope_state& data) { data = envelope_state(read()); }
    template <typename DataType, int Count>
    void restore(DataType (&data)[Count])
    {
        for (uint32_t index = 0; index < Count; index++) restore(data[index]);
    }

    // internal helper
    ymfm_saved_state& write(uint8_t data)
    {
        ymfm_state_buffer_push(m_buffer, data);
        return *this;
    }
    uint8_t read()
    {
        size_t buffer_size = ymfm_state_buffer_size(m_buffer);
        return (m_offset < int32_t(buffer_size)) ? ymfm_state_buffer_data(m_buffer)[m_offset++] : 0;
    }

    // internal state
    ymfm_state_buffer& m_buffer;
    int32_t m_offset;
};

//*********************************************************
//  INTERFACE CLASSES
//*********************************************************

// ======================> ymfm_engine_callbacks

// this class represents functions in the engine that the ymfm_interface
// needs to be able to call; it is represented here as a separate interface
// that is independent of the actual engine implementation
class ymfm_engine_callbacks
{
  public:
    virtual ~ymfm_engine_callbacks() = default;

    // timer callback; called by the interface when a timer fires
    virtual void engine_timer_expired(uint32_t tnum) = 0;

    // check interrupts; called by the interface after synchronization
    virtual void engine_check_interrupts() = 0;

    // mode register write; called by the interface after synchronization
    virtual void engine_mode_write(uint8_t data) = 0;
};

// ======================> ymfm_interface

// this class represents the interface between the fm_engine and the outside
// world; it provides hooks for timers, synchronization, and I/O
class ymfm_interface
{
    // the engine is our friend
    template <typename RegisterType>
    friend class fm_engine_base;

  public:
    virtual ~ymfm_interface() = default;

    // the following functions must be implemented by any derived classes; the
    // default implementations are sufficient for some minimal operation, but will
    // likely need to be overridden to integrate with the outside world; they are
    // all prefixed with ymfm_ to reduce the likelihood of namespace collisions

    //
    // timing and synchronizaton
    //

    // the chip implementation calls this when a write happens to the mode
    // register, which could affect timers and interrupts; our responsibility
    // is to ensure the system is up to date before calling the engine's
    // engine_mode_write() method
    virtual void ymfm_sync_mode_write(uint8_t data) { m_engine->engine_mode_write(data); }

    // the chip implementation calls this when the chip's status has changed,
    // which may affect the interrupt state; our responsibility is to ensure
    // the system is up to date before calling the engine's
    // engine_check_interrupts() method
    virtual void ymfm_sync_check_interrupts() { m_engine->engine_check_interrupts(); }

    // the chip implementation calls this when one of the two internal timers
    // has changed state; our responsibility is to arrange to call the engine's
    // engine_timer_expired() method after the provided number of clocks; if
    // duration_in_clocks is negative, we should cancel any outstanding timers
    virtual void ymfm_set_timer(uint32_t tnum, int32_t duration_in_clocks) {}

    // the chip implementation calls this to indicate that the chip should be
    // considered in a busy state until the given number of clocks has passed;
    // our responsibility is to compute and remember the ending time based on
    // the chip's clock for later checking
    virtual void ymfm_set_busy_end(uint32_t clocks) {}

    // the chip implementation calls this to see if the chip is still currently
    // is a busy state, as specified by a previous call to ymfm_set_busy_end();
    // our responsibility is to compare the current time against the previously
    // noted busy end time and return true if we haven't yet passed it
    virtual bool ymfm_is_busy() { return false; }

    //
    // I/O functions
    //

    // the chip implementation calls this when the state of the IRQ signal has
    // changed due to a status change; our responsibility is to respond as
    // needed to the change in IRQ state, signaling any consumers
    virtual void ymfm_update_irq(bool asserted) {}

    // the chip implementation calls this whenever data is read from outside
    // of the chip; our responsibility is to provide the data requested
    virtual uint8_t ymfm_external_read(access_class type, uint32_t address) { return 0; }

    // the chip implementation calls this whenever data is written outside
    // of the chip; our responsibility is to pass the written data on to any consumers
    virtual void ymfm_external_write(access_class type, uint32_t address, uint8_t data) {}

  protected:
    // pointer to engine callbacks -- this is set directly by the engine at
    // construction time
    ymfm_engine_callbacks* m_engine;
};

//--------------------------------------------------------------------------------
// NOTE: Merged ymfm_fm.h and ymfm_fm.ipp by Yoji Suzuki
// Removed SSG features
//--------------------------------------------------------------------------------
//*********************************************************
//  GLOBAL ENUMERATORS
//*********************************************************

// three different keyon sources; actual keyon is an OR over all of these
enum keyon_type : uint32_t {
    KEYON_NORMAL = 0,
    KEYON_RHYTHM = 1,
    KEYON_CSM = 2
};

//*********************************************************
//  CORE IMPLEMENTATION
//*********************************************************

// ======================> opdata_cache

// this class holds data that is computed once at the start of clocking
// and remains static during subsequent sound generation
struct opdata_cache {
    // set phase_step to this value to recalculate it each sample; needed
    // in the case of PM LFO changes
    static constexpr uint32_t PHASE_STEP_DYNAMIC = 1;

    uint16_t const* waveform;   // base of sine table
    uint32_t phase_step;        // phase step, or PHASE_STEP_DYNAMIC if PM is active
    uint32_t total_level;       // total level * 8 + KSL
    uint32_t block_freq;        // raw block frequency value (used to compute phase_step)
    int32_t detune;             // detuning value (used to compute phase_step)
    uint32_t multiple;          // multiple value (x.1, used to compute phase_step)
    uint32_t eg_sustain;        // sustain level, shifted up to envelope values
    uint8_t eg_rate[EG_STATES]; // envelope rate, including KSR
    uint8_t eg_shift = 0;       // envelope shift amount
};

// ======================> fm_registers_base

// base class for family-specific register classes; this provides a few
// constants, common defaults, and helpers, but mostly each derived class is
// responsible for defining all commonly-called methods
class fm_registers_base
{
  public:
    // this value is returned from the write() function for rhythm channels
    static constexpr uint32_t RHYTHM_CHANNEL = 0xff;

    // this is the size of a full sin waveform
    static constexpr uint32_t WAVEFORM_LENGTH = 0x400;

    //
    // the following constants need to be defined per family:
    //          uint32_t OUTPUTS: The number of outputs exposed (1-4)
    //         uint32_t CHANNELS: The number of channels on the chip
    //     uint32_t ALL_CHANNELS: A bitmask of all channels
    //        uint32_t OPERATORS: The number of operators on the chip
    //        uint32_t WAVEFORMS: The number of waveforms offered
    //        uint32_t REGISTERS: The number of 8-bit registers allocated
    // uint32_t DEFAULT_PRESCALE: The starting clock prescale
    // uint32_t EG_CLOCK_DIVIDER: The clock divider of the envelope generator
    // uint32_t CSM_TRIGGER_MASK: Mask of channels to trigger in CSM mode
    //         uint32_t REG_MODE: The address of the "mode" register controlling timers
    //     uint8_t STATUS_TIMERA: Status bit to set when timer A fires
    //     uint8_t STATUS_TIMERB: Status bit to set when tiemr B fires
    //       uint8_t STATUS_BUSY: Status bit to set when the chip is busy
    //        uint8_t STATUS_IRQ: Status bit to set when an IRQ is signalled
    //
    // the following constants are uncommon:
    //          bool DYNAMIC_OPS: True if ops/channel can be changed at runtime (OPL3+)
    //       bool EG_HAS_DEPRESS: True if the chip has a DP ("depress"?) envelope stage (OPLL)
    //        bool EG_HAS_REVERB: True if the chip has a faux reverb envelope stage (OPQ/OPZ)
    //           bool EG_HAS_SSG: True if the chip has SSG envelope support (OPN)
    //      bool MODULATOR_DELAY: True if the modulator is delayed by 1 sample (OPL pre-OPL3)
    //
    static constexpr bool DYNAMIC_OPS = false;
    static constexpr bool EG_HAS_DEPRESS = false;
    static constexpr bool EG_HAS_REVERB = false;
    static constexpr bool EG_HAS_SSG = false;
    static constexpr bool MODULATOR_DELAY = false;

    // system-wide register defaults
    uint32_t status_mask() const { return 0; }   // OPL only
    uint32_t irq_reset() const { return 0; }     // OPL only
    uint32_t noise_enable() const { return 0; }  // OPM only
    uint32_t rhythm_enable() const { return 0; } // OPL only

    // per-operator register defaults
    uint32_t op_ssg_eg_enable(uint32_t opoffs) const { return 0; } // OPN(A) only
    uint32_t op_ssg_eg_mode(uint32_t opoffs) const { return 0; }   // OPN(A) only

  protected:
    // helper to encode four operator numbers into a 32-bit value in the
    // operator maps for each register class
    static constexpr uint32_t operator_list(uint8_t o1 = 0xff, uint8_t o2 = 0xff, uint8_t o3 = 0xff, uint8_t o4 = 0xff)
    {
        return o1 | (o2 << 8) | (o3 << 16) | (o4 << 24);
    }

    // helper to apply KSR to the raw ADSR rate, ignoring ksr if the
    // raw value is 0, and clamping to 63
    static constexpr uint32_t effective_rate(uint32_t rawrate, uint32_t ksr)
    {
        return (rawrate == 0) ? 0 : ymfm_min_value<uint32_t>(rawrate + ksr, 63);
    }
};

//*********************************************************
//  CORE ENGINE CLASSES
//*********************************************************

// forward declarations
template <class RegisterType>
class fm_engine_base;

// ======================> fm_operator

// fm_operator represents an FM operator (or "slot" in FM parlance), which
// produces an output sine wave modulated by an envelope
template <class RegisterType>
class fm_operator
{
    // "quiet" value, used to optimize when we can skip doing work
    static constexpr uint32_t EG_QUIET = 0x380;

  public:
    // constructor
    fm_operator(fm_engine_base<RegisterType>& owner, uint32_t opoffs);

    // save/restore
    void save_restore(ymfm_saved_state& state);

    // reset the operator state
    void reset();

    // return the operator/channel offset
    uint32_t opoffs() const { return m_opoffs; }
    uint32_t choffs() const { return m_choffs; }

    // set the current channel
    void set_choffs(uint32_t choffs) { m_choffs = choffs; }

    // prepare prior to clocking
    bool prepare();

    // master clocking function
    void clock(uint32_t env_counter, int32_t lfo_raw_pm);

    // return the current phase value
    uint32_t phase() const { return m_phase >> 10; }

    // compute operator volume
    int32_t compute_volume(uint32_t phase, uint32_t am_offset) const;

    // compute volume for the OPM noise channel
    int32_t compute_noise_volume(uint32_t am_offset) const;

    // key state control
    void keyonoff(uint32_t on, keyon_type type);

    // return a reference to our registers
    RegisterType& regs() const { return m_regs; }

    // simple getters for debugging
    envelope_state debug_eg_state() const { return m_env_state; }
    uint16_t debug_eg_attenuation() const { return m_env_attenuation; }
    uint8_t debug_ssg_inverted() const { return m_ssg_inverted; }
    opdata_cache& debug_cache() { return m_cache; }

  private:
    // start the attack phase
    void start_attack(bool is_restart = false);

    // start the release phase
    void start_release();

    // clock phases
    void clock_keystate(uint32_t keystate);
    void clock_ssg_eg_state();
    void clock_envelope(uint32_t env_counter);
    void clock_phase(int32_t lfo_raw_pm);

    // return effective attenuation of the envelope
    uint32_t envelope_attenuation(uint32_t am_offset) const;

    // internal state
    uint32_t m_choffs;                     // channel offset in registers
    uint32_t m_opoffs;                     // operator offset in registers
    uint32_t m_phase;                      // current phase value (10.10 format)
    uint16_t m_env_attenuation;            // computed envelope attenuation (4.6 format)
    envelope_state m_env_state;            // current envelope state
    uint8_t m_ssg_inverted;                // non-zero if the output should be inverted (bit 0)
    uint8_t m_key_state;                   // current key state: on or off (bit 0)
    uint8_t m_keyon_live;                  // live key on state (bit 0 = direct, bit 1 = rhythm, bit 2 = CSM)
    opdata_cache m_cache;                  // cached values for performance
    RegisterType& m_regs;                  // direct reference to registers
    fm_engine_base<RegisterType>& m_owner; // reference to the owning engine
};

// ======================> fm_channel

// fm_channel represents an FM channel which combines the output of 2 or 4
// operators into a final result
template <class RegisterType>
class fm_channel
{
    using output_data = ymfm_output<RegisterType::OUTPUTS>;

  public:
    // constructor
    fm_channel(fm_engine_base<RegisterType>& owner, uint32_t choffs);

    // save/restore
    void save_restore(ymfm_saved_state& state);

    // reset the channel state
    void reset();

    // return the channel offset
    uint32_t choffs() const { return m_choffs; }

    // assign operators
    void assign(uint32_t index, fm_operator<RegisterType>* op)
    {
        assert(index < 4);
        m_op[index] = op;
        if (op != nullptr)
            op->set_choffs(m_choffs);
    }

    // signal key on/off to our operators
    void keyonoff(uint32_t states, keyon_type type, uint32_t chnum);

    // prepare prior to clocking
    bool prepare();

    // master clocking function
    void clock(uint32_t env_counter, int32_t lfo_raw_pm);

    // specific 2-operator and 4-operator output handlers
    void output_2op(output_data& output, uint32_t rshift, int32_t clipmax) const;
    void output_4op(output_data& output, uint32_t rshift, int32_t clipmax) const;

    // compute the special OPL rhythm channel outputs
    void output_rhythm_ch6(output_data& output, uint32_t rshift, int32_t clipmax) const;
    void output_rhythm_ch7(uint32_t phase_select, output_data& output, uint32_t rshift, int32_t clipmax) const;
    void output_rhythm_ch8(uint32_t phase_select, output_data& output, uint32_t rshift, int32_t clipmax) const;

    // are we a 4-operator channel or a 2-operator one?
    bool is4op() const
    {
        if (RegisterType::DYNAMIC_OPS)
            return (m_op[2] != nullptr);
        return (RegisterType::OPERATORS / RegisterType::CHANNELS == 4);
    }

    // return a reference to our registers
    RegisterType& regs() const { return m_regs; }

    // simple getters for debugging
    fm_operator<RegisterType>* debug_operator(uint32_t index) const { return m_op[index]; }

  private:
    // helper to add values to the outputs based on channel enables
    void add_to_output(uint32_t choffs, output_data& output, int32_t value) const
    {
        // create these constants to appease overzealous compilers checking array
        // bounds in unreachable code (looking at you, clang)
        constexpr int out0_index = 0;
        constexpr int out1_index = 1 % RegisterType::OUTPUTS;
        constexpr int out2_index = 2 % RegisterType::OUTPUTS;
        constexpr int out3_index = 3 % RegisterType::OUTPUTS;

        if (RegisterType::OUTPUTS == 1 || m_regs.ch_output_0(choffs))
            output.data[out0_index] += value;
        if (RegisterType::OUTPUTS >= 2 && m_regs.ch_output_1(choffs))
            output.data[out1_index] += value;
        if (RegisterType::OUTPUTS >= 3 && m_regs.ch_output_2(choffs))
            output.data[out2_index] += value;
        if (RegisterType::OUTPUTS >= 4 && m_regs.ch_output_3(choffs))
            output.data[out3_index] += value;
    }

    // internal state
    uint32_t m_choffs;                              // channel offset in registers
    int16_t m_feedback[2];                          // feedback memory for operator 1
    mutable int16_t m_feedback_in;                  // next input value for op 1 feedback (set in output)
    fm_operator<RegisterType>* m_op[4];             // up to 4 operators
    RegisterType& m_regs;                           // direct reference to registers
    fm_engine_base<RegisterType>& m_owner;          // reference to the owning engine
};

// ======================> fm_engine_base

// fm_engine_base represents a set of operators and channels which together
// form a Yamaha FM core; chips that implement other engines (ADPCM, wavetable,
// etc) take this output and combine it with the others externally
template <class RegisterType>
class fm_engine_base : public ymfm_engine_callbacks
{
  public:
    // expose some constants from the registers
    static constexpr uint32_t OUTPUTS = RegisterType::OUTPUTS;
    static constexpr uint32_t CHANNELS = RegisterType::CHANNELS;
    static constexpr uint32_t ALL_CHANNELS = RegisterType::ALL_CHANNELS;
    static constexpr uint32_t OPERATORS = RegisterType::OPERATORS;

    // also expose status flags for consumers that inject additional bits
    static constexpr uint8_t STATUS_TIMERA = RegisterType::STATUS_TIMERA;
    static constexpr uint8_t STATUS_TIMERB = RegisterType::STATUS_TIMERB;
    static constexpr uint8_t STATUS_BUSY = RegisterType::STATUS_BUSY;
    static constexpr uint8_t STATUS_IRQ = RegisterType::STATUS_IRQ;

    // expose the correct output class
    using output_data = ymfm_output<OUTPUTS>;

    // constructor
    fm_engine_base(ymfm_interface& intf);

    // destructor
    ~fm_engine_base();

    // save/restore
    void save_restore(ymfm_saved_state& state);

    // reset the overall state
    void reset();

    // master clocking function
    uint32_t clock(uint32_t chanmask);

    // compute sum of channel outputs
    void output(output_data& output, uint32_t rshift, int32_t clipmax, uint32_t chanmask) const;

    // write to the OPN registers
    void write(uint16_t regnum, uint8_t data);

    // return the current status
    uint8_t status() const;

    // set/reset bits in the status register, updating the IRQ status
    uint8_t set_reset_status(uint8_t set, uint8_t reset)
    {
        m_status = (m_status | set) & ~(reset | STATUS_BUSY);
        m_intf.ymfm_sync_check_interrupts();
        return m_status & ~m_regs.status_mask();
    }

    // set the IRQ mask
    void set_irq_mask(uint8_t mask)
    {
        m_irq_mask = mask;
        m_intf.ymfm_sync_check_interrupts();
    }

    // return the current clock prescale
    uint32_t clock_prescale() const { return m_clock_prescale; }

    // set prescale factor (2/3/6)
    void set_clock_prescale(uint32_t prescale) { m_clock_prescale = prescale; }

    // compute sample rate
    uint32_t sample_rate(uint32_t baseclock) const
    {
#if (YMFM_DEBUG_LOG_WAVFILES)
        for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
            m_wavfile[chnum].set_samplerate(baseclock / (m_clock_prescale * OPERATORS));
#endif
        return baseclock / (m_clock_prescale * OPERATORS);
    }

    // return the owning device
    ymfm_interface& intf() const { return m_intf; }

    // return a reference to our registers
    RegisterType& regs() { return m_regs; }

    // invalidate any caches
    void invalidate_caches() { m_modified_channels = RegisterType::ALL_CHANNELS; }

    // simple getters for debugging
    fm_channel<RegisterType>* debug_channel(uint32_t index) const { return m_channel[index]; }
    fm_operator<RegisterType>* debug_operator(uint32_t index) const { return m_operator[index]; }

  public:
    // timer callback; called by the interface when a timer fires
    virtual void engine_timer_expired(uint32_t tnum) override;

    // check interrupts; called by the interface after synchronization
    virtual void engine_check_interrupts() override;

    // mode register write; called by the interface after synchronization
    virtual void engine_mode_write(uint8_t data) override;

  protected:
    // assign the current set of operators to channels
    void assign_operators();

    // update the state of the given timer
    void update_timer(uint32_t which, uint32_t enable, int32_t delta_clocks);

    // internal state
    ymfm_interface& m_intf;                                           // reference to the system interface
    uint32_t m_env_counter;                                           // envelope counter; low 2 bits are sub-counter
    uint8_t m_status;                                                 // current status register
    uint8_t m_clock_prescale;                                         // prescale factor (2/3/6)
    uint8_t m_irq_mask;                                               // mask of which bits signal IRQs
    uint8_t m_irq_state;                                              // current IRQ state
    uint8_t m_timer_running[2];                                       // current timer running state
    uint8_t m_total_clocks;                                           // low 8 bits of the total number of clocks processed
    uint32_t m_active_channels;                                       // mask of active channels (computed by prepare)
    uint32_t m_modified_channels;                                     // mask of channels that have been modified
    uint32_t m_prepare_count;                                         // counter to do periodic prepare sweeps
    RegisterType m_regs;                                              // register accessor
    fm_channel<RegisterType>* m_channel[CHANNELS];    // channel pointers
    fm_operator<RegisterType>* m_operator[OPERATORS]; // operator pointers
#if (YMFM_DEBUG_LOG_WAVFILES)
    mutable ymfm_wavfile<1> m_wavfile[CHANNELS]; // for debugging
#endif
};

//*********************************************************
//  GLOBAL TABLE LOOKUPS
//*********************************************************

//-------------------------------------------------
//  abs_sin_attenuation - given a sin (phase) input
//  where the range 0-2*PI is mapped onto 10 bits,
//  return the absolute value of sin(input),
//  logarithmically-adjusted and treated as an
//  attenuation value, in 4.8 fixed point format
//-------------------------------------------------

inline uint32_t abs_sin_attenuation(uint32_t input)
{
    // the values here are stored as 4.8 logarithmic values for 1/4 phase
    // this matches the internal format of the OPN chip, extracted from the die
    static uint16_t const s_sin_table[256] =
        {
            0x859, 0x6c3, 0x607, 0x58b, 0x52e, 0x4e4, 0x4a6, 0x471, 0x443, 0x41a, 0x3f5, 0x3d3, 0x3b5, 0x398, 0x37e, 0x365,
            0x34e, 0x339, 0x324, 0x311, 0x2ff, 0x2ed, 0x2dc, 0x2cd, 0x2bd, 0x2af, 0x2a0, 0x293, 0x286, 0x279, 0x26d, 0x261,
            0x256, 0x24b, 0x240, 0x236, 0x22c, 0x222, 0x218, 0x20f, 0x206, 0x1fd, 0x1f5, 0x1ec, 0x1e4, 0x1dc, 0x1d4, 0x1cd,
            0x1c5, 0x1be, 0x1b7, 0x1b0, 0x1a9, 0x1a2, 0x19b, 0x195, 0x18f, 0x188, 0x182, 0x17c, 0x177, 0x171, 0x16b, 0x166,
            0x160, 0x15b, 0x155, 0x150, 0x14b, 0x146, 0x141, 0x13c, 0x137, 0x133, 0x12e, 0x129, 0x125, 0x121, 0x11c, 0x118,
            0x114, 0x10f, 0x10b, 0x107, 0x103, 0x0ff, 0x0fb, 0x0f8, 0x0f4, 0x0f0, 0x0ec, 0x0e9, 0x0e5, 0x0e2, 0x0de, 0x0db,
            0x0d7, 0x0d4, 0x0d1, 0x0cd, 0x0ca, 0x0c7, 0x0c4, 0x0c1, 0x0be, 0x0bb, 0x0b8, 0x0b5, 0x0b2, 0x0af, 0x0ac, 0x0a9,
            0x0a7, 0x0a4, 0x0a1, 0x09f, 0x09c, 0x099, 0x097, 0x094, 0x092, 0x08f, 0x08d, 0x08a, 0x088, 0x086, 0x083, 0x081,
            0x07f, 0x07d, 0x07a, 0x078, 0x076, 0x074, 0x072, 0x070, 0x06e, 0x06c, 0x06a, 0x068, 0x066, 0x064, 0x062, 0x060,
            0x05e, 0x05c, 0x05b, 0x059, 0x057, 0x055, 0x053, 0x052, 0x050, 0x04e, 0x04d, 0x04b, 0x04a, 0x048, 0x046, 0x045,
            0x043, 0x042, 0x040, 0x03f, 0x03e, 0x03c, 0x03b, 0x039, 0x038, 0x037, 0x035, 0x034, 0x033, 0x031, 0x030, 0x02f,
            0x02e, 0x02d, 0x02b, 0x02a, 0x029, 0x028, 0x027, 0x026, 0x025, 0x024, 0x023, 0x022, 0x021, 0x020, 0x01f, 0x01e,
            0x01d, 0x01c, 0x01b, 0x01a, 0x019, 0x018, 0x017, 0x017, 0x016, 0x015, 0x014, 0x014, 0x013, 0x012, 0x011, 0x011,
            0x010, 0x00f, 0x00f, 0x00e, 0x00d, 0x00d, 0x00c, 0x00c, 0x00b, 0x00a, 0x00a, 0x009, 0x009, 0x008, 0x008, 0x007,
            0x007, 0x007, 0x006, 0x006, 0x005, 0x005, 0x005, 0x004, 0x004, 0x004, 0x003, 0x003, 0x003, 0x002, 0x002, 0x002,
            0x002, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000};

    // if the top bit is set, we're in the second half of the curve
    // which is a mirror image, so invert the index
    if (bitfield(input, 8))
        input = ~input;

    // return the value from the table
    return s_sin_table[input & 0xff];
}

//-------------------------------------------------
//  attenuation_to_volume - given a 5.8 fixed point
//  logarithmic attenuation value, return a 13-bit
//  linear volume
//-------------------------------------------------

inline uint32_t attenuation_to_volume(uint32_t input)
{
    // the values here are 10-bit mantissas with an implied leading bit
    // this matches the internal format of the OPN chip, extracted from the die

    // as a nod to performance, the implicit 0x400 bit is pre-incorporated, and
    // the values are left-shifted by 2 so that a simple right shift is all that
    // is needed; also the order is reversed to save a NOT on the input
#define X(a) (((a) | 0x400) << 2)
    static uint16_t const s_power_table[256] =
        {
            X(0x3fa), X(0x3f5), X(0x3ef), X(0x3ea), X(0x3e4), X(0x3df), X(0x3da), X(0x3d4),
            X(0x3cf), X(0x3c9), X(0x3c4), X(0x3bf), X(0x3b9), X(0x3b4), X(0x3ae), X(0x3a9),
            X(0x3a4), X(0x39f), X(0x399), X(0x394), X(0x38f), X(0x38a), X(0x384), X(0x37f),
            X(0x37a), X(0x375), X(0x370), X(0x36a), X(0x365), X(0x360), X(0x35b), X(0x356),
            X(0x351), X(0x34c), X(0x347), X(0x342), X(0x33d), X(0x338), X(0x333), X(0x32e),
            X(0x329), X(0x324), X(0x31f), X(0x31a), X(0x315), X(0x310), X(0x30b), X(0x306),
            X(0x302), X(0x2fd), X(0x2f8), X(0x2f3), X(0x2ee), X(0x2e9), X(0x2e5), X(0x2e0),
            X(0x2db), X(0x2d6), X(0x2d2), X(0x2cd), X(0x2c8), X(0x2c4), X(0x2bf), X(0x2ba),
            X(0x2b5), X(0x2b1), X(0x2ac), X(0x2a8), X(0x2a3), X(0x29e), X(0x29a), X(0x295),
            X(0x291), X(0x28c), X(0x288), X(0x283), X(0x27f), X(0x27a), X(0x276), X(0x271),
            X(0x26d), X(0x268), X(0x264), X(0x25f), X(0x25b), X(0x257), X(0x252), X(0x24e),
            X(0x249), X(0x245), X(0x241), X(0x23c), X(0x238), X(0x234), X(0x230), X(0x22b),
            X(0x227), X(0x223), X(0x21e), X(0x21a), X(0x216), X(0x212), X(0x20e), X(0x209),
            X(0x205), X(0x201), X(0x1fd), X(0x1f9), X(0x1f5), X(0x1f0), X(0x1ec), X(0x1e8),
            X(0x1e4), X(0x1e0), X(0x1dc), X(0x1d8), X(0x1d4), X(0x1d0), X(0x1cc), X(0x1c8),
            X(0x1c4), X(0x1c0), X(0x1bc), X(0x1b8), X(0x1b4), X(0x1b0), X(0x1ac), X(0x1a8),
            X(0x1a4), X(0x1a0), X(0x19c), X(0x199), X(0x195), X(0x191), X(0x18d), X(0x189),
            X(0x185), X(0x181), X(0x17e), X(0x17a), X(0x176), X(0x172), X(0x16f), X(0x16b),
            X(0x167), X(0x163), X(0x160), X(0x15c), X(0x158), X(0x154), X(0x151), X(0x14d),
            X(0x149), X(0x146), X(0x142), X(0x13e), X(0x13b), X(0x137), X(0x134), X(0x130),
            X(0x12c), X(0x129), X(0x125), X(0x122), X(0x11e), X(0x11b), X(0x117), X(0x114),
            X(0x110), X(0x10c), X(0x109), X(0x106), X(0x102), X(0x0ff), X(0x0fb), X(0x0f8),
            X(0x0f4), X(0x0f1), X(0x0ed), X(0x0ea), X(0x0e7), X(0x0e3), X(0x0e0), X(0x0dc),
            X(0x0d9), X(0x0d6), X(0x0d2), X(0x0cf), X(0x0cc), X(0x0c8), X(0x0c5), X(0x0c2),
            X(0x0be), X(0x0bb), X(0x0b8), X(0x0b5), X(0x0b1), X(0x0ae), X(0x0ab), X(0x0a8),
            X(0x0a4), X(0x0a1), X(0x09e), X(0x09b), X(0x098), X(0x094), X(0x091), X(0x08e),
            X(0x08b), X(0x088), X(0x085), X(0x082), X(0x07e), X(0x07b), X(0x078), X(0x075),
            X(0x072), X(0x06f), X(0x06c), X(0x069), X(0x066), X(0x063), X(0x060), X(0x05d),
            X(0x05a), X(0x057), X(0x054), X(0x051), X(0x04e), X(0x04b), X(0x048), X(0x045),
            X(0x042), X(0x03f), X(0x03c), X(0x039), X(0x036), X(0x033), X(0x030), X(0x02d),
            X(0x02a), X(0x028), X(0x025), X(0x022), X(0x01f), X(0x01c), X(0x019), X(0x016),
            X(0x014), X(0x011), X(0x00e), X(0x00b), X(0x008), X(0x006), X(0x003), X(0x000)};
#undef X

    // look up the fractional part, then shift by the whole
    return s_power_table[input & 0xff] >> (input >> 8);
}

//-------------------------------------------------
//  attenuation_increment - given a 6-bit ADSR
//  rate value and a 3-bit stepping index,
//  return a 4-bit increment to the attenutaion
//  for this step (or for the attack case, the
//  fractional scale factor to decrease by)
//-------------------------------------------------

inline uint32_t attenuation_increment(uint32_t rate, uint32_t index)
{
    static uint32_t const s_increment_table[64] =
        {
            0x00000000, 0x00000000, 0x10101010, 0x10101010, // 0-3    (0x00-0x03)
            0x10101010, 0x10101010, 0x11101110, 0x11101110, // 4-7    (0x04-0x07)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 8-11   (0x08-0x0B)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 12-15  (0x0C-0x0F)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 16-19  (0x10-0x13)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 20-23  (0x14-0x17)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 24-27  (0x18-0x1B)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 28-31  (0x1C-0x1F)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 32-35  (0x20-0x23)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 36-39  (0x24-0x27)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 40-43  (0x28-0x2B)
            0x10101010, 0x10111010, 0x11101110, 0x11111110, // 44-47  (0x2C-0x2F)
            0x11111111, 0x21112111, 0x21212121, 0x22212221, // 48-51  (0x30-0x33)
            0x22222222, 0x42224222, 0x42424242, 0x44424442, // 52-55  (0x34-0x37)
            0x44444444, 0x84448444, 0x84848484, 0x88848884, // 56-59  (0x38-0x3B)
            0x88888888, 0x88888888, 0x88888888, 0x88888888  // 60-63  (0x3C-0x3F)
        };
    return bitfield(s_increment_table[rate], 4 * index, 4);
}

//-------------------------------------------------
//  detune_adjustment - given a 5-bit key code
//  value and a 3-bit detune parameter, return a
//  6-bit signed phase displacement; this table
//  has been verified against Nuked's equations,
//  but the equations are rather complicated, so
//  we'll keep the simplicity of the table
//-------------------------------------------------

inline int32_t detune_adjustment(uint32_t detune, uint32_t keycode)
{
    static uint8_t const s_detune_adjustment[32][4] =
        {
            {0, 0, 1, 2}, {0, 0, 1, 2}, {0, 0, 1, 2}, {0, 0, 1, 2}, {0, 1, 2, 2}, {0, 1, 2, 3}, {0, 1, 2, 3}, {0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 3, 4}, {0, 1, 3, 4}, {0, 1, 3, 5}, {0, 2, 4, 5}, {0, 2, 4, 6}, {0, 2, 4, 6}, {0, 2, 5, 7}, {0, 2, 5, 8}, {0, 3, 6, 8}, {0, 3, 6, 9}, {0, 3, 7, 10}, {0, 4, 8, 11}, {0, 4, 8, 12}, {0, 4, 9, 13}, {0, 5, 10, 14}, {0, 5, 11, 16}, {0, 6, 12, 17}, {0, 6, 13, 19}, {0, 7, 14, 20}, {0, 8, 16, 22}, {0, 8, 16, 22}, {0, 8, 16, 22}, {0, 8, 16, 22}};
    int32_t result = s_detune_adjustment[keycode][detune & 3];
    return bitfield(detune, 2) ? -result : result;
}

//-------------------------------------------------
//  opm_key_code_to_phase_step - converts an
//  OPM concatenated block (3 bits), keycode
//  (4 bits) and key fraction (6 bits) to a 0.10
//  phase step, after applying the given delta;
//  this applies to OPM and OPZ, so it lives here
//  in a central location
//-------------------------------------------------

inline uint32_t opm_key_code_to_phase_step(uint32_t block_freq, int32_t delta)
{
    // The phase step is essentially the fnum in OPN-speak. To compute this table,
    // we used the standard formula for computing the frequency of a note, and
    // then converted that frequency to fnum using the formula documented in the
    // YM2608 manual.
    //
    // However, the YM2608 manual describes everything in terms of a nominal 8MHz
    // clock, which produces an FM clock of:
    //
    //    8000000 / 24(operators) / 6(prescale) = 55555Hz FM clock
    //
    // Whereas the descriptions for the YM2151 use a nominal 3.579545MHz clock:
    //
    //    3579545 / 32(operators) / 2(prescale) = 55930Hz FM clock
    //
    // To correct for this, the YM2608 formula was adjusted to use a clock of
    // 8053920Hz, giving this equation for the fnum:
    //
    //    fnum = (double(144) * freq * (1 << 20)) / double(8053920) / 4;
    //
    // Unfortunately, the computed table differs in a few spots from the data
    // verified from an actual chip. The table below comes from David Viens'
    // analysis, used with his permission.
    static const uint32_t s_phase_step[12 * 64] =
        {
            41568, 41600, 41632, 41664, 41696, 41728, 41760, 41792, 41856, 41888, 41920, 41952, 42016, 42048, 42080, 42112,
            42176, 42208, 42240, 42272, 42304, 42336, 42368, 42400, 42464, 42496, 42528, 42560, 42624, 42656, 42688, 42720,
            42784, 42816, 42848, 42880, 42912, 42944, 42976, 43008, 43072, 43104, 43136, 43168, 43232, 43264, 43296, 43328,
            43392, 43424, 43456, 43488, 43552, 43584, 43616, 43648, 43712, 43744, 43776, 43808, 43872, 43904, 43936, 43968,
            44032, 44064, 44096, 44128, 44192, 44224, 44256, 44288, 44352, 44384, 44416, 44448, 44512, 44544, 44576, 44608,
            44672, 44704, 44736, 44768, 44832, 44864, 44896, 44928, 44992, 45024, 45056, 45088, 45152, 45184, 45216, 45248,
            45312, 45344, 45376, 45408, 45472, 45504, 45536, 45568, 45632, 45664, 45728, 45760, 45792, 45824, 45888, 45920,
            45984, 46016, 46048, 46080, 46144, 46176, 46208, 46240, 46304, 46336, 46368, 46400, 46464, 46496, 46528, 46560,
            46656, 46688, 46720, 46752, 46816, 46848, 46880, 46912, 46976, 47008, 47072, 47104, 47136, 47168, 47232, 47264,
            47328, 47360, 47392, 47424, 47488, 47520, 47552, 47584, 47648, 47680, 47744, 47776, 47808, 47840, 47904, 47936,
            48032, 48064, 48096, 48128, 48192, 48224, 48288, 48320, 48384, 48416, 48448, 48480, 48544, 48576, 48640, 48672,
            48736, 48768, 48800, 48832, 48896, 48928, 48992, 49024, 49088, 49120, 49152, 49184, 49248, 49280, 49344, 49376,
            49440, 49472, 49504, 49536, 49600, 49632, 49696, 49728, 49792, 49824, 49856, 49888, 49952, 49984, 50048, 50080,
            50144, 50176, 50208, 50240, 50304, 50336, 50400, 50432, 50496, 50528, 50560, 50592, 50656, 50688, 50752, 50784,
            50880, 50912, 50944, 50976, 51040, 51072, 51136, 51168, 51232, 51264, 51328, 51360, 51424, 51456, 51488, 51520,
            51616, 51648, 51680, 51712, 51776, 51808, 51872, 51904, 51968, 52000, 52064, 52096, 52160, 52192, 52224, 52256,
            52384, 52416, 52448, 52480, 52544, 52576, 52640, 52672, 52736, 52768, 52832, 52864, 52928, 52960, 52992, 53024,
            53120, 53152, 53216, 53248, 53312, 53344, 53408, 53440, 53504, 53536, 53600, 53632, 53696, 53728, 53792, 53824,
            53920, 53952, 54016, 54048, 54112, 54144, 54208, 54240, 54304, 54336, 54400, 54432, 54496, 54528, 54592, 54624,
            54688, 54720, 54784, 54816, 54880, 54912, 54976, 55008, 55072, 55104, 55168, 55200, 55264, 55296, 55360, 55392,
            55488, 55520, 55584, 55616, 55680, 55712, 55776, 55808, 55872, 55936, 55968, 56032, 56064, 56128, 56160, 56224,
            56288, 56320, 56384, 56416, 56480, 56512, 56576, 56608, 56672, 56736, 56768, 56832, 56864, 56928, 56960, 57024,
            57120, 57152, 57216, 57248, 57312, 57376, 57408, 57472, 57536, 57568, 57632, 57664, 57728, 57792, 57824, 57888,
            57952, 57984, 58048, 58080, 58144, 58208, 58240, 58304, 58368, 58400, 58464, 58496, 58560, 58624, 58656, 58720,
            58784, 58816, 58880, 58912, 58976, 59040, 59072, 59136, 59200, 59232, 59296, 59328, 59392, 59456, 59488, 59552,
            59648, 59680, 59744, 59776, 59840, 59904, 59936, 60000, 60064, 60128, 60160, 60224, 60288, 60320, 60384, 60416,
            60512, 60544, 60608, 60640, 60704, 60768, 60800, 60864, 60928, 60992, 61024, 61088, 61152, 61184, 61248, 61280,
            61376, 61408, 61472, 61536, 61600, 61632, 61696, 61760, 61824, 61856, 61920, 61984, 62048, 62080, 62144, 62208,
            62272, 62304, 62368, 62432, 62496, 62528, 62592, 62656, 62720, 62752, 62816, 62880, 62944, 62976, 63040, 63104,
            63200, 63232, 63296, 63360, 63424, 63456, 63520, 63584, 63648, 63680, 63744, 63808, 63872, 63904, 63968, 64032,
            64096, 64128, 64192, 64256, 64320, 64352, 64416, 64480, 64544, 64608, 64672, 64704, 64768, 64832, 64896, 64928,
            65024, 65056, 65120, 65184, 65248, 65312, 65376, 65408, 65504, 65536, 65600, 65664, 65728, 65792, 65856, 65888,
            65984, 66016, 66080, 66144, 66208, 66272, 66336, 66368, 66464, 66496, 66560, 66624, 66688, 66752, 66816, 66848,
            66944, 66976, 67040, 67104, 67168, 67232, 67296, 67328, 67424, 67456, 67520, 67584, 67648, 67712, 67776, 67808,
            67904, 67936, 68000, 68064, 68128, 68192, 68256, 68288, 68384, 68448, 68512, 68544, 68640, 68672, 68736, 68800,
            68896, 68928, 68992, 69056, 69120, 69184, 69248, 69280, 69376, 69440, 69504, 69536, 69632, 69664, 69728, 69792,
            69920, 69952, 70016, 70080, 70144, 70208, 70272, 70304, 70400, 70464, 70528, 70560, 70656, 70688, 70752, 70816,
            70912, 70976, 71040, 71104, 71136, 71232, 71264, 71360, 71424, 71488, 71552, 71616, 71648, 71744, 71776, 71872,
            71968, 72032, 72096, 72160, 72192, 72288, 72320, 72416, 72480, 72544, 72608, 72672, 72704, 72800, 72832, 72928,
            72992, 73056, 73120, 73184, 73216, 73312, 73344, 73440, 73504, 73568, 73632, 73696, 73728, 73824, 73856, 73952,
            74080, 74144, 74208, 74272, 74304, 74400, 74432, 74528, 74592, 74656, 74720, 74784, 74816, 74912, 74944, 75040,
            75136, 75200, 75264, 75328, 75360, 75456, 75488, 75584, 75648, 75712, 75776, 75840, 75872, 75968, 76000, 76096,
            76224, 76288, 76352, 76416, 76448, 76544, 76576, 76672, 76736, 76800, 76864, 76928, 77024, 77120, 77152, 77248,
            77344, 77408, 77472, 77536, 77568, 77664, 77696, 77792, 77856, 77920, 77984, 78048, 78144, 78240, 78272, 78368,
            78464, 78528, 78592, 78656, 78688, 78784, 78816, 78912, 78976, 79040, 79104, 79168, 79264, 79360, 79392, 79488,
            79616, 79680, 79744, 79808, 79840, 79936, 79968, 80064, 80128, 80192, 80256, 80320, 80416, 80512, 80544, 80640,
            80768, 80832, 80896, 80960, 80992, 81088, 81120, 81216, 81280, 81344, 81408, 81472, 81568, 81664, 81696, 81792,
            81952, 82016, 82080, 82144, 82176, 82272, 82304, 82400, 82464, 82528, 82592, 82656, 82752, 82848, 82880, 82976};

    // extract the block (octave) first
    uint32_t block = bitfield(block_freq, 10, 3);

    // the keycode (bits 6-9) is "gappy", mapping 12 values over 16 in each
    // octave; to correct for this, we multiply the 4-bit value by 3/4 (or
    // rather subtract 1/4); note that a (invalid) value of 15 will bleed into
    // the next octave -- this is confirmed
    uint32_t adjusted_code = bitfield(block_freq, 6, 4) - bitfield(block_freq, 8, 2);

    // now re-insert the 6-bit fraction
    int32_t eff_freq = (adjusted_code << 6) | bitfield(block_freq, 0, 6);

    // now that the gaps are removed, add the delta
    eff_freq += delta;

    // handle over/underflow by adjusting the block:
    if (uint32_t(eff_freq) >= 768) {
        // minimum delta is -512 (PM), so we can only underflow by 1 octave
        if (eff_freq < 0) {
            eff_freq += 768;
            if (block-- == 0)
                return s_phase_step[0] >> 7;
        }

        // maximum delta is +512+608 (PM+detune), so we can overflow by up to 2 octaves
        else {
            eff_freq -= 768;
            if (eff_freq >= 768)
                block++, eff_freq -= 768;
            if (block++ >= 7)
                return s_phase_step[767];
        }
    }

    // look up the phase shift for the key code, then shift by octave
    return s_phase_step[eff_freq] >> (block ^ 7);
}

//-------------------------------------------------
//  opn_lfo_pm_phase_adjustment - given the 7 most
//  significant frequency number bits, plus a 3-bit
//  PM depth value and a signed 5-bit raw PM value,
//  return a signed PM adjustment to the frequency;
//  algorithm written to match Nuked behavior
//-------------------------------------------------

inline int32_t opn_lfo_pm_phase_adjustment(uint32_t fnum_bits, uint32_t pm_sensitivity, int32_t lfo_raw_pm)
{
    // this table encodes 2 shift values to apply to the top 7 bits
    // of fnum; it is effectively a cheap multiply by a constant
    // value containing 0-2 bits
    static uint8_t const s_lfo_pm_shifts[8][8] =
        {
            {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77},
            {0x77, 0x77, 0x77, 0x77, 0x72, 0x72, 0x72, 0x72},
            {0x77, 0x77, 0x77, 0x72, 0x72, 0x72, 0x17, 0x17},
            {0x77, 0x77, 0x72, 0x72, 0x17, 0x17, 0x12, 0x12},
            {0x77, 0x77, 0x72, 0x17, 0x17, 0x17, 0x12, 0x07},
            {0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01},
            {0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01},
            {0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01}};

    // look up the relevant shifts
    int32_t abs_pm = (lfo_raw_pm < 0) ? -lfo_raw_pm : lfo_raw_pm;
    uint32_t const shifts = s_lfo_pm_shifts[pm_sensitivity][bitfield(abs_pm, 0, 3)];

    // compute the adjustment
    int32_t adjust = (fnum_bits >> bitfield(shifts, 0, 4)) + (fnum_bits >> bitfield(shifts, 4, 4));
    if (pm_sensitivity > 5)
        adjust <<= pm_sensitivity - 5;
    adjust >>= 2;

    // every 16 cycles it inverts sign
    return (lfo_raw_pm < 0) ? -adjust : adjust;
}

//*********************************************************
//  FM OPERATOR
//*********************************************************

//-------------------------------------------------
//  fm_operator - constructor
//-------------------------------------------------

template <class RegisterType>
fm_operator<RegisterType>::fm_operator(fm_engine_base<RegisterType>& owner, uint32_t opoffs) : m_choffs(0),
                                                                                               m_opoffs(opoffs),
                                                                                               m_phase(0),
                                                                                               m_env_attenuation(0x3ff),
                                                                                               m_env_state(EG_RELEASE),
                                                                                               m_ssg_inverted(false),
                                                                                               m_key_state(0),
                                                                                               m_keyon_live(0),
                                                                                               m_regs(owner.regs()),
                                                                                               m_owner(owner)
{
}

//-------------------------------------------------
//  reset - reset the channel state
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::reset()
{
    // reset our data
    m_phase = 0;
    m_env_attenuation = 0x3ff;
    m_env_state = EG_RELEASE;
    m_ssg_inverted = 0;
    m_key_state = 0;
    m_keyon_live = 0;
}

//-------------------------------------------------
//  save_restore - save or restore the data
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::save_restore(ymfm_saved_state& state)
{
    state.save_restore(m_phase);
    state.save_restore(m_env_attenuation);
    state.save_restore(m_env_state);
    state.save_restore(m_ssg_inverted);
    state.save_restore(m_key_state);
    state.save_restore(m_keyon_live);
}

//-------------------------------------------------
//  prepare - prepare for clocking
//-------------------------------------------------

template <class RegisterType>
bool fm_operator<RegisterType>::prepare()
{
    // cache the data
    m_regs.cache_operator_data(m_choffs, m_opoffs, m_cache);

    // clock the key state
    clock_keystate(uint32_t(m_keyon_live != 0));
    m_keyon_live &= ~(1 << KEYON_CSM);

    // we're active until we're quiet after the release
    return (m_env_state != (RegisterType::EG_HAS_REVERB ? EG_REVERB : EG_RELEASE) || m_env_attenuation < EG_QUIET);
}

//-------------------------------------------------
//  clock - master clocking function
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::clock(uint32_t env_counter, int32_t lfo_raw_pm)
{
    // clock the SSG-EG state (OPN/OPNA)
    if (m_regs.op_ssg_eg_enable(m_opoffs))
        clock_ssg_eg_state();
    else
        m_ssg_inverted = false;

    // clock the envelope if on an envelope cycle; env_counter is a x.2 value
    if (bitfield(env_counter, 0, 2) == 0)
        clock_envelope(env_counter >> 2);

    // clock the phase
    clock_phase(lfo_raw_pm);
}

//-------------------------------------------------
//  compute_volume - compute the 14-bit signed
//  volume of this operator, given a phase
//  modulation and an AM LFO offset
//-------------------------------------------------

template <class RegisterType>
int32_t fm_operator<RegisterType>::compute_volume(uint32_t phase, uint32_t am_offset) const
{
    // the low 10 bits of phase represents a full 2*PI period over
    // the full sin wave

    // early out if the envelope is effectively off
    if (m_env_attenuation > EG_QUIET)
        return 0;

    // get the absolute value of the sin, as attenuation, as a 4.8 fixed point value
    uint32_t sin_attenuation = m_cache.waveform[phase & (RegisterType::WAVEFORM_LENGTH - 1)];

    // get the attenuation from the evelope generator as a 4.6 value, shifted up to 4.8
    uint32_t env_attenuation = envelope_attenuation(am_offset) << 2;

    // combine into a 5.8 value, then convert from attenuation to 13-bit linear volume
    int32_t result = attenuation_to_volume((sin_attenuation & 0x7fff) + env_attenuation);

    // negate if in the negative part of the sin wave (sign bit gives 14 bits)
    return bitfield(sin_attenuation, 15) ? -result : result;
}

//-------------------------------------------------
//  compute_noise_volume - compute the 14-bit
//  signed noise volume of this operator, given a
//  noise input value and an AM offset
//-------------------------------------------------

template <class RegisterType>
int32_t fm_operator<RegisterType>::compute_noise_volume(uint32_t am_offset) const
{
    // application manual says the logarithmic transform is not applied here, so we
    // just use the raw envelope attenuation, inverted (since 0 attenuation should be
    // maximum), and shift it up from a 10-bit value to an 11-bit value
    int32_t result = (envelope_attenuation(am_offset) ^ 0x3ff) << 1;

    // QUESTION: is AM applied still?

    // negate based on the noise state
    return bitfield(m_regs.noise_state(), 0) ? -result : result;
}

//-------------------------------------------------
//  keyonoff - signal a key on/off event
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::keyonoff(uint32_t on, keyon_type type)
{
    m_keyon_live = (m_keyon_live & ~(1 << int(type))) | (bitfield(on, 0) << int(type));
}

//-------------------------------------------------
//  start_attack - start the attack phase; called
//  when a keyon happens or when an SSG-EG cycle
//  is complete and restarts
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::start_attack(bool is_restart)
{
    // don't change anything if already in attack state
    if (m_env_state == EG_ATTACK)
        return;
    m_env_state = EG_ATTACK;

    // generally not inverted at start, except if SSG-EG is enabled and
    // one of the inverted modes is specified; leave this alone on a
    // restart, as it is managed by the clock_ssg_eg_state() code
    if (RegisterType::EG_HAS_SSG && !is_restart)
        m_ssg_inverted = m_regs.op_ssg_eg_enable(m_opoffs) & bitfield(m_regs.op_ssg_eg_mode(m_opoffs), 2);

    // reset the phase when we start an attack due to a key on
    // (but not when due to an SSG-EG restart except in certain cases
    // managed directly by the SSG-EG code)
    if (!is_restart)
        m_phase = 0;

    // if the attack rate >= 62 then immediately go to max attenuation
    if (m_cache.eg_rate[EG_ATTACK] >= 62)
        m_env_attenuation = 0;
}

//-------------------------------------------------
//  start_release - start the release phase;
//  called when a keyoff happens
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::start_release()
{
    // don't change anything if already in release state
    if (m_env_state >= EG_RELEASE)
        return;
    m_env_state = EG_RELEASE;

    // if attenuation if inverted due to SSG-EG, snap the inverted attenuation
    // as the starting point
    if (RegisterType::EG_HAS_SSG && m_ssg_inverted) {
        m_env_attenuation = (0x200 - m_env_attenuation) & 0x3ff;
        m_ssg_inverted = false;
    }
}

//-------------------------------------------------
//  clock_keystate - clock the keystate to match
//  the incoming keystate
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::clock_keystate(uint32_t keystate)
{
    assert(keystate == 0 || keystate == 1);

    // has the key changed?
    if ((keystate ^ m_key_state) != 0) {
        m_key_state = keystate;

        // if the key has turned on, start the attack
        if (keystate != 0) {
            // OPLL has a DP ("depress"?) state to bring the volume
            // down before starting the attack
            if (RegisterType::EG_HAS_DEPRESS && m_env_attenuation < 0x200)
                m_env_state = EG_DEPRESS;
            else
                start_attack();
        }

        // otherwise, start the release
        else
            start_release();
    }
}

//-------------------------------------------------
//  clock_ssg_eg_state - clock the SSG-EG state;
//  should only be called if SSG-EG is enabled
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::clock_ssg_eg_state()
{
    // work only happens once the attenuation crosses above 0x200
    if (!bitfield(m_env_attenuation, 9))
        return;

    // 8 SSG-EG modes:
    //    000: repeat normally
    //    001: run once, hold low
    //    010: repeat, alternating between inverted/non-inverted
    //    011: run once, hold high
    //    100: inverted repeat normally
    //    101: inverted run once, hold low
    //    110: inverted repeat, alternating between inverted/non-inverted
    //    111: inverted run once, hold high
    uint32_t mode = m_regs.op_ssg_eg_mode(m_opoffs);

    // hold modes (1/3/5/7)
    if (bitfield(mode, 0)) {
        // set the inverted flag to the end state (0 for modes 1/7, 1 for modes 3/5)
        m_ssg_inverted = bitfield(mode, 2) ^ bitfield(mode, 1);

        // if holding, force the attenuation to the expected value once we're
        // past the attack phase
        if (m_env_state != EG_ATTACK)
            m_env_attenuation = m_ssg_inverted ? 0x200 : 0x3ff;
    }

    // continuous modes (0/2/4/6)
    else {
        // toggle invert in alternating mode (even in attack state)
        m_ssg_inverted ^= bitfield(mode, 1);

        // restart attack if in decay/sustain states
        if (m_env_state == EG_DECAY || m_env_state == EG_SUSTAIN)
            start_attack(true);

        // phase is reset to 0 in modes 0/4
        if (bitfield(mode, 1) == 0)
            m_phase = 0;
    }

    // in all modes, once we hit release state, attenuation is forced to maximum
    if (m_env_state == EG_RELEASE)
        m_env_attenuation = 0x3ff;
}

//-------------------------------------------------
//  clock_envelope - clock the envelope state
//  according to the given count
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::clock_envelope(uint32_t env_counter)
{
    // handle attack->decay transitions
    if (m_env_state == EG_ATTACK && m_env_attenuation == 0)
        m_env_state = EG_DECAY;

    // handle decay->sustain transitions; it is important to do this immediately
    // after the attack->decay transition above in the event that the sustain level
    // is set to 0 (in which case we will skip right to sustain without doing any
    // decay); as an example where this can be heard, check the cymbals sound
    // in channel 0 of shinobi's test mode sound #5
    if (m_env_state == EG_DECAY && m_env_attenuation >= m_cache.eg_sustain)
        m_env_state = EG_SUSTAIN;

    // fetch the appropriate 6-bit rate value from the cache
    uint32_t rate = m_cache.eg_rate[m_env_state];

    // compute the rate shift value; this is the shift needed to
    // apply to the env_counter such that it becomes a 5.11 fixed
    // point number
    uint32_t rate_shift = rate >> 2;
    env_counter <<= rate_shift;

    // see if the fractional part is 0; if not, it's not time to clock
    if (bitfield(env_counter, 0, 11) != 0)
        return;

    // determine the increment based on the non-fractional part of env_counter
    uint32_t relevant_bits = bitfield(env_counter, (rate_shift <= 11) ? 11 : rate_shift, 3);
    uint32_t increment = attenuation_increment(rate, relevant_bits);

    // attack is the only one that increases
    if (m_env_state == EG_ATTACK) {
        // glitch means that attack rates of 62/63 don't increment if
        // changed after the initial key on (where they are handled
        // specially); nukeykt confirms this happens on OPM, OPN, OPL/OPLL
        // at least so assuming it is true for everyone
        if (rate < 62)
            m_env_attenuation += (~m_env_attenuation * increment) >> 4;
    }

    // all other cases are similar
    else {
        // non-SSG-EG cases just apply the increment
        if (!m_regs.op_ssg_eg_enable(m_opoffs))
            m_env_attenuation += increment;

        // SSG-EG only applies if less than mid-point, and then at 4x
        else if (m_env_attenuation < 0x200)
            m_env_attenuation += 4 * increment;

        // clamp the final attenuation
        if (m_env_attenuation >= 0x400)
            m_env_attenuation = 0x3ff;

        // transition from depress to attack
        if (RegisterType::EG_HAS_DEPRESS && m_env_state == EG_DEPRESS && m_env_attenuation >= 0x200)
            start_attack();

        // transition from release to reverb, should switch at -18dB
        if (RegisterType::EG_HAS_REVERB && m_env_state == EG_RELEASE && m_env_attenuation >= 0xc0)
            m_env_state = EG_REVERB;
    }
}

//-------------------------------------------------
//  clock_phase - clock the 10.10 phase value; the
//  OPN version of the logic has been verified
//  against the Nuked phase generator
//-------------------------------------------------

template <class RegisterType>
void fm_operator<RegisterType>::clock_phase(int32_t lfo_raw_pm)
{
    // read from the cache, or recalculate if PM active
    uint32_t phase_step = m_cache.phase_step;
    if (phase_step == opdata_cache::PHASE_STEP_DYNAMIC)
        phase_step = m_regs.compute_phase_step(m_choffs, m_opoffs, m_cache, lfo_raw_pm);

    // finally apply the step to the current phase value
    m_phase += phase_step;
}

//-------------------------------------------------
//  envelope_attenuation - return the effective
//  attenuation of the envelope
//-------------------------------------------------

template <class RegisterType>
uint32_t fm_operator<RegisterType>::envelope_attenuation(uint32_t am_offset) const
{
    uint32_t result = m_env_attenuation >> m_cache.eg_shift;

    // invert if necessary due to SSG-EG
    if (RegisterType::EG_HAS_SSG && m_ssg_inverted)
        result = (0x200 - result) & 0x3ff;

    // add in LFO AM modulation
    if (m_regs.op_lfo_am_enable(m_opoffs))
        result += am_offset;

    // add in total level and KSL from the cache
    result += m_cache.total_level;

    // clamp to max, apply shift, and return
    return ymfm_min_value<uint32_t>(result, 0x3ff);
}

//*********************************************************
//  FM CHANNEL
//*********************************************************

//-------------------------------------------------
//  fm_channel - constructor
//-------------------------------------------------

template <class RegisterType>
fm_channel<RegisterType>::fm_channel(fm_engine_base<RegisterType>& owner, uint32_t choffs) : m_choffs(choffs),
                                                                                             m_feedback{0, 0},
                                                                                             m_feedback_in(0),
                                                                                             m_op{nullptr, nullptr, nullptr, nullptr},
                                                                                             m_regs(owner.regs()),
                                                                                             m_owner(owner)
{
}

//-------------------------------------------------
//  reset - reset the channel state
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::reset()
{
    // reset our data
    m_feedback[0] = m_feedback[1] = 0;
    m_feedback_in = 0;
}

//-------------------------------------------------
//  save_restore - save or restore the data
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::save_restore(ymfm_saved_state& state)
{
    state.save_restore(m_feedback[0]);
    state.save_restore(m_feedback[1]);
    state.save_restore(m_feedback_in);
}

//-------------------------------------------------
//  keyonoff - signal key on/off to our operators
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::keyonoff(uint32_t states, keyon_type type, uint32_t chnum)
{
    for (uint32_t opnum = 0; opnum < 4; opnum++)
        if (m_op[opnum] != nullptr)
            m_op[opnum]->keyonoff(bitfield(states, opnum), type);

    if (debug::LOG_KEYON_EVENTS && ((debug::GLOBAL_FM_CHANNEL_MASK >> chnum) & 1) != 0)
        for (uint32_t opnum = 0; opnum < 4; opnum++)
            if (m_op[opnum] != nullptr)
                debug::log_keyon("%c%s\n", bitfield(states, opnum) ? '+' : '-', m_regs.log_keyon(m_choffs, m_op[opnum]->opoffs()));
}

//-------------------------------------------------
//  prepare - prepare for clocking
//-------------------------------------------------

template <class RegisterType>
bool fm_channel<RegisterType>::prepare()
{
    uint32_t active_mask = 0;

    // prepare all operators and determine if they are active
    for (uint32_t opnum = 0; opnum < 4; opnum++)
        if (m_op[opnum] != nullptr)
            if (m_op[opnum]->prepare())
                active_mask |= 1 << opnum;

    return (active_mask != 0);
}

//-------------------------------------------------
//  clock - master clock of all operators
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::clock(uint32_t env_counter, int32_t lfo_raw_pm)
{
    // clock the feedback through
    m_feedback[0] = m_feedback[1];
    m_feedback[1] = m_feedback_in;

    for (uint32_t opnum = 0; opnum < 4; opnum++)
        if (m_op[opnum] != nullptr)
            m_op[opnum]->clock(env_counter, lfo_raw_pm);

    /*
    useful temporary code for envelope debugging
    if (m_choffs == 0x101)
    {
        for (uint32_t opnum = 0; opnum < m_op.size(); opnum++)
        {
            auto &op = *m_op[((opnum & 1) << 1) | ((opnum >> 1) & 1)];
            printf(" %c%03X%c%c ",
                "PADSRV"[op.debug_eg_state()],
                op.debug_eg_attenuation(),
                op.debug_ssg_inverted() ? '-' : '+',
                m_regs.op_ssg_eg_enable(op.opoffs()) ? '0' + m_regs.op_ssg_eg_mode(op.opoffs()) : ' ');
        }
    printf(" -- ");
    }
    */
}

//-------------------------------------------------
//  output_2op - combine 4 operators according to
//  the specified algorithm, returning a sum
//  according to the rshift and clipmax parameters,
//  which vary between different implementations
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::output_2op(output_data& output, uint32_t rshift, int32_t clipmax) const
{
    // The first 2 operators should be populated
    assert(m_op[0] != nullptr);
    assert(m_op[1] != nullptr);

    // AM amount is the same across all operators; compute it once
    uint32_t am_offset = m_regs.lfo_am_offset(m_choffs);

    // operator 1 has optional self-feedback
    int32_t opmod = 0;
    uint32_t feedback = m_regs.ch_feedback(m_choffs);
    if (feedback != 0)
        opmod = (m_feedback[0] + m_feedback[1]) >> (10 - feedback);

    // compute the 14-bit volume/value of operator 1 and update the feedback
    int32_t op1value = m_feedback_in = m_op[0]->compute_volume(m_op[0]->phase() + opmod, am_offset);

    // now that the feedback has been computed, skip the rest if all volumes
    // are clear; no need to do all this work for nothing
    if (m_regs.ch_output_any(m_choffs) == 0)
        return;

    // Algorithms for two-operator case:
    //    0: O1 -> O2 -> out
    //    1: (O1 + O2) -> out
    int32_t result;
    if (bitfield(m_regs.ch_algorithm(m_choffs), 0) == 0) {
        // some OPL chips use the previous sample for modulation instead of
        // the current sample
        opmod = (RegisterType::MODULATOR_DELAY ? m_feedback[1] : op1value) >> 1;
        result = m_op[1]->compute_volume(m_op[1]->phase() + opmod, am_offset) >> rshift;
    } else {
        result = (RegisterType::MODULATOR_DELAY ? m_feedback[1] : op1value) >> rshift;
        result += m_op[1]->compute_volume(m_op[1]->phase(), am_offset) >> rshift;
        int32_t clipmin = -clipmax - 1;
        result = clamp(result, clipmin, clipmax);
    }

    // add to the output
    add_to_output(m_choffs, output, result);
}

//-------------------------------------------------
//  output_4op - combine 4 operators according to
//  the specified algorithm, returning a sum
//  according to the rshift and clipmax parameters,
//  which vary between different implementations
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::output_4op(output_data& output, uint32_t rshift, int32_t clipmax) const
{
    // all 4 operators should be populated
    assert(m_op[0] != nullptr);
    assert(m_op[1] != nullptr);
    assert(m_op[2] != nullptr);
    assert(m_op[3] != nullptr);

    // AM amount is the same across all operators; compute it once
    uint32_t am_offset = m_regs.lfo_am_offset(m_choffs);

    // operator 1 has optional self-feedback
    int32_t opmod = 0;
    uint32_t feedback = m_regs.ch_feedback(m_choffs);
    if (feedback != 0)
        opmod = (m_feedback[0] + m_feedback[1]) >> (10 - feedback);

    // compute the 14-bit volume/value of operator 1 and update the feedback
    int32_t op1value = m_feedback_in = m_op[0]->compute_volume(m_op[0]->phase() + opmod, am_offset);

    // now that the feedback has been computed, skip the rest if all volumes
    // are clear; no need to do all this work for nothing
    if (m_regs.ch_output_any(m_choffs) == 0)
        return;

// OPM/OPN offer 8 different connection algorithms for 4 operators,
// and OPL3 offers 4 more, which we designate here as 8-11.
//
// The operators are computed in order, with the inputs pulled from
// an array of values (opout) that is populated as we go:
//    0 = 0
//    1 = O1
//    2 = O2
//    3 = O3
//    4 = (O4)
//    5 = O1+O2
//    6 = O1+O3
//    7 = O2+O3
//
// The s_algorithm_ops table describes the inputs and outputs of each
// algorithm as follows:
//
//      ---------x use opout[x] as operator 2 input
//      ------xxx- use opout[x] as operator 3 input
//      ---xxx---- use opout[x] as operator 4 input
//      --x------- include opout[1] in final sum
//      -x-------- include opout[2] in final sum
//      x--------- include opout[3] in final sum
#define ALGORITHM(op2in, op3in, op4in, op1out, op2out, op3out) \
    ((op2in) | ((op3in) << 1) | ((op4in) << 4) | ((op1out) << 7) | ((op2out) << 8) | ((op3out) << 9))
    static uint16_t const s_algorithm_ops[8 + 4] =
        {
            ALGORITHM(1, 2, 3, 0, 0, 0), //  0: O1 -> O2 -> O3 -> O4 -> out (O4)
            ALGORITHM(0, 5, 3, 0, 0, 0), //  1: (O1 + O2) -> O3 -> O4 -> out (O4)
            ALGORITHM(0, 2, 6, 0, 0, 0), //  2: (O1 + (O2 -> O3)) -> O4 -> out (O4)
            ALGORITHM(1, 0, 7, 0, 0, 0), //  3: ((O1 -> O2) + O3) -> O4 -> out (O4)
            ALGORITHM(1, 0, 3, 0, 1, 0), //  4: ((O1 -> O2) + (O3 -> O4)) -> out (O2+O4)
            ALGORITHM(1, 1, 1, 0, 1, 1), //  5: ((O1 -> O2) + (O1 -> O3) + (O1 -> O4)) -> out (O2+O3+O4)
            ALGORITHM(1, 0, 0, 0, 1, 1), //  6: ((O1 -> O2) + O3 + O4) -> out (O2+O3+O4)
            ALGORITHM(0, 0, 0, 1, 1, 1), //  7: (O1 + O2 + O3 + O4) -> out (O1+O2+O3+O4)
            ALGORITHM(1, 2, 3, 0, 0, 0), //  8: O1 -> O2 -> O3 -> O4 -> out (O4)         [same as 0]
            ALGORITHM(0, 2, 3, 1, 0, 0), //  9: (O1 + (O2 -> O3 -> O4)) -> out (O1+O4)   [unique]
            ALGORITHM(1, 0, 3, 0, 1, 0), // 10: ((O1 -> O2) + (O3 -> O4)) -> out (O2+O4) [same as 4]
            ALGORITHM(0, 2, 0, 1, 0, 1)  // 11: (O1 + (O2 -> O3) + O4) -> out (O1+O3+O4) [unique]
        };
    uint32_t algorithm_ops = s_algorithm_ops[m_regs.ch_algorithm(m_choffs)];

    // populate the opout table
    int16_t opout[8];
    opout[0] = 0;
    opout[1] = op1value;

    // compute the 14-bit volume/value of operator 2
    opmod = opout[bitfield(algorithm_ops, 0, 1)] >> 1;
    opout[2] = m_op[1]->compute_volume(m_op[1]->phase() + opmod, am_offset);
    opout[5] = opout[1] + opout[2];

    // compute the 14-bit volume/value of operator 3
    opmod = opout[bitfield(algorithm_ops, 1, 3)] >> 1;
    opout[3] = m_op[2]->compute_volume(m_op[2]->phase() + opmod, am_offset);
    opout[6] = opout[1] + opout[3];
    opout[7] = opout[2] + opout[3];

    // compute the 14-bit volume/value of operator 4; this could be a noise
    // value on the OPM; all algorithms consume OP4 output at a minimum
    int32_t result;
    if (m_regs.noise_enable() && m_choffs == 7)
        result = m_op[3]->compute_noise_volume(am_offset);
    else {
        opmod = opout[bitfield(algorithm_ops, 4, 3)] >> 1;
        result = m_op[3]->compute_volume(m_op[3]->phase() + opmod, am_offset);
    }
    result >>= rshift;

    // optionally add OP1, OP2, OP3
    int32_t clipmin = -clipmax - 1;
    if (bitfield(algorithm_ops, 7) != 0)
        result = clamp(result + (opout[1] >> rshift), clipmin, clipmax);
    if (bitfield(algorithm_ops, 8) != 0)
        result = clamp(result + (opout[2] >> rshift), clipmin, clipmax);
    if (bitfield(algorithm_ops, 9) != 0)
        result = clamp(result + (opout[3] >> rshift), clipmin, clipmax);

    // add to the output
    add_to_output(m_choffs, output, result);
}

//-------------------------------------------------
//  output_rhythm_ch6 - special case output
//  computation for OPL channel 6 in rhythm mode,
//  which outputs a Bass Drum instrument
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::output_rhythm_ch6(output_data& output, uint32_t rshift, int32_t clipmax) const
{
    // AM amount is the same across all operators; compute it once
    uint32_t am_offset = m_regs.lfo_am_offset(m_choffs);

    // Bass Drum: this uses operators 12 and 15 (i.e., channel 6)
    // in an almost-normal way, except that if the algorithm is 1,
    // the first operator is ignored instead of added in

    // operator 1 has optional self-feedback
    int32_t opmod = 0;
    uint32_t feedback = m_regs.ch_feedback(m_choffs);
    if (feedback != 0)
        opmod = (m_feedback[0] + m_feedback[1]) >> (10 - feedback);

    // compute the 14-bit volume/value of operator 1 and update the feedback
    int32_t opout1 = m_feedback_in = m_op[0]->compute_volume(m_op[0]->phase() + opmod, am_offset);

    // compute the 14-bit volume/value of operator 2, which is the result
    opmod = bitfield(m_regs.ch_algorithm(m_choffs), 0) ? 0 : (opout1 >> 1);
    int32_t result = m_op[1]->compute_volume(m_op[1]->phase() + opmod, am_offset) >> rshift;

    // add to the output
    add_to_output(m_choffs, output, result * 2);
}

//-------------------------------------------------
//  output_rhythm_ch7 - special case output
//  computation for OPL channel 7 in rhythm mode,
//  which outputs High Hat and Snare Drum
//  instruments
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::output_rhythm_ch7(uint32_t phase_select, output_data& output, uint32_t rshift, int32_t clipmax) const
{
    // AM amount is the same across all operators; compute it once
    uint32_t am_offset = m_regs.lfo_am_offset(m_choffs);
    uint32_t noise_state = bitfield(m_regs.noise_state(), 0);

    // High Hat: this uses the envelope from operator 13 (channel 7),
    // and a combination of noise and the operator 13/17 phase select
    // to compute the phase
    uint32_t phase = (phase_select << 9) | (0xd0 >> (2 * (noise_state ^ phase_select)));
    int32_t result = m_op[0]->compute_volume(phase, am_offset) >> rshift;

    // Snare Drum: this uses the envelope from operator 16 (channel 7),
    // and a combination of noise and operator 13 phase to pick a phase
    uint32_t op13phase = m_op[0]->phase();
    phase = (0x100 << bitfield(op13phase, 8)) ^ (noise_state << 8);
    result += m_op[1]->compute_volume(phase, am_offset) >> rshift;
    result = clamp(result, -clipmax - 1, clipmax);

    // add to the output
    add_to_output(m_choffs, output, result * 2);
}

//-------------------------------------------------
//  output_rhythm_ch8 - special case output
//  computation for OPL channel 8 in rhythm mode,
//  which outputs Tom Tom and Top Cymbal instruments
//-------------------------------------------------

template <class RegisterType>
void fm_channel<RegisterType>::output_rhythm_ch8(uint32_t phase_select, output_data& output, uint32_t rshift, int32_t clipmax) const
{
    // AM amount is the same across all operators; compute it once
    uint32_t am_offset = m_regs.lfo_am_offset(m_choffs);

    // Tom Tom: this is just a single operator processed normally
    int32_t result = m_op[0]->compute_volume(m_op[0]->phase(), am_offset) >> rshift;

    // Top Cymbal: this uses the envelope from operator 17 (channel 8),
    // and the operator 13/17 phase select to compute the phase
    uint32_t phase = 0x100 | (phase_select << 9);
    result += m_op[1]->compute_volume(phase, am_offset) >> rshift;
    result = clamp(result, -clipmax - 1, clipmax);

    // add to the output
    add_to_output(m_choffs, output, result * 2);
}

//*********************************************************
//  FM ENGINE BASE
//*********************************************************

//-------------------------------------------------
//  fm_engine_base - constructor
//-------------------------------------------------

template <class RegisterType>
fm_engine_base<RegisterType>::fm_engine_base(ymfm_interface& intf) : m_intf(intf),
                                                                     m_env_counter(0),
                                                                     m_status(0),
                                                                     m_clock_prescale(RegisterType::DEFAULT_PRESCALE),
                                                                     m_irq_mask(STATUS_TIMERA | STATUS_TIMERB),
                                                                     m_irq_state(0),
                                                                     m_timer_running{0, 0},
                                                                     m_total_clocks(0),
                                                                     m_active_channels(ALL_CHANNELS),
                                                                     m_modified_channels(ALL_CHANNELS),
                                                                     m_prepare_count(0)
{
    // inform the interface of their engine
    m_intf.m_engine = this;

    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        m_channel[chnum] = nullptr;
    for (uint32_t opnum = 0; opnum < OPERATORS; opnum++)
        m_operator[opnum] = nullptr;

    // create the channels
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        m_channel[chnum] = new fm_channel<RegisterType>(*this, RegisterType::channel_offset(chnum));

    // create the operators
    for (uint32_t opnum = 0; opnum < OPERATORS; opnum++)
        m_operator[opnum] = new fm_operator<RegisterType>(*this, RegisterType::operator_offset(opnum));

#if (YMFM_DEBUG_LOG_WAVFILES)
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        m_wavfile[chnum].set_index(chnum);
#endif

    // do the initial operator assignment
    assign_operators();
}

//-------------------------------------------------
//  fm_engine_base - destructor
//-------------------------------------------------

template <class RegisterType>
fm_engine_base<RegisterType>::~fm_engine_base()
{
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++) {
        if (m_channel[chnum] != nullptr) {
            delete m_channel[chnum];
            m_channel[chnum] = nullptr;
        }
    }
    for (uint32_t opnum = 0; opnum < OPERATORS; opnum++) {
        if (m_operator[opnum] != nullptr) {
            delete m_operator[opnum];
            m_operator[opnum] = nullptr;
        }
    }
}

//-------------------------------------------------
//  reset - reset the overall state
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::reset()
{
    // reset all status bits
    set_reset_status(0, 0xff);

    // register type-specific initialization
    m_regs.reset();

    // explicitly write to the mode register since it has side-effects
    // QUESTION: old cores initialize this to 0x30 -- who is right?
    write(RegisterType::REG_MODE, 0);

    // reset the channels
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        if (m_channel[chnum] != nullptr)
            m_channel[chnum]->reset();

    // reset the operators
    for (uint32_t opnum = 0; opnum < OPERATORS; opnum++)
        if (m_operator[opnum] != nullptr)
            m_operator[opnum]->reset();
}

//-------------------------------------------------
//  save_restore - save or restore the data
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::save_restore(ymfm_saved_state& state)
{
    // save our data
    state.save_restore(m_env_counter);
    state.save_restore(m_status);
    state.save_restore(m_clock_prescale);
    state.save_restore(m_irq_mask);
    state.save_restore(m_irq_state);
    state.save_restore(m_timer_running[0]);
    state.save_restore(m_timer_running[1]);
    state.save_restore(m_total_clocks);

    // save the register/family data
    m_regs.save_restore(state);

    // save channel data
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        m_channel[chnum]->save_restore(state);

    // save operator data
    for (uint32_t opnum = 0; opnum < OPERATORS; opnum++)
        m_operator[opnum]->save_restore(state);

    // invalidate any caches
    invalidate_caches();
}

//-------------------------------------------------
//  clock - iterate over all channels, clocking
//  them forward one step
//-------------------------------------------------

template <class RegisterType>
uint32_t fm_engine_base<RegisterType>::clock(uint32_t chanmask)
{
    // update the clock counter
    m_total_clocks++;

    // if something was modified, prepare
    // also prepare every 4k samples to catch ending notes
    if (m_modified_channels != 0 || m_prepare_count++ >= 4096) {
        // reassign operators to channels if dynamic
        if (RegisterType::DYNAMIC_OPS)
            assign_operators();

        // call each channel to prepare
        m_active_channels = 0;
        for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
            if (bitfield(chanmask, chnum))
                if (m_channel[chnum]->prepare())
                    m_active_channels |= 1 << chnum;

        // reset the modified channels and prepare count
        m_modified_channels = m_prepare_count = 0;
    }

    // if the envelope clock divider is 1, just increment by 4;
    // otherwise, increment by 1 and manually wrap when we reach the divide count
    if (RegisterType::EG_CLOCK_DIVIDER == 1)
        m_env_counter += 4;
    else if (bitfield(++m_env_counter, 0, 2) == RegisterType::EG_CLOCK_DIVIDER)
        m_env_counter += 4 - RegisterType::EG_CLOCK_DIVIDER;

    // clock the noise generator
    int32_t lfo_raw_pm = m_regs.clock_noise_and_lfo();

    // now update the state of all the channels and operators
    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        if (bitfield(chanmask, chnum))
            m_channel[chnum]->clock(m_env_counter, lfo_raw_pm);

    // return the envelope counter as it is used to clock ADPCM-A
    return m_env_counter;
}

//-------------------------------------------------
//  output - compute a sum over the relevant
//  channels
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::output(output_data& output, uint32_t rshift, int32_t clipmax, uint32_t chanmask) const
{
    // mask out some channels for debug purposes
    chanmask &= debug::GLOBAL_FM_CHANNEL_MASK;

    // mask out inactive channels
    if (!YMFM_DEBUG_LOG_WAVFILES)
        chanmask &= m_active_channels;

    // handle the rhythm case, where some of the operators are dedicated
    // to percussion (this is an OPL-specific feature)
    if (m_regs.rhythm_enable()) {
        // we don't support the OPM noise channel here; ensure it is off
        assert(m_regs.noise_enable() == 0);

        // precompute the operator 13+17 phase selection value
        uint32_t op13phase = m_operator[13]->phase();
        uint32_t op17phase = m_operator[17]->phase();
        uint32_t phase_select = (bitfield(op13phase, 2) ^ bitfield(op13phase, 7)) | bitfield(op13phase, 3) | (bitfield(op17phase, 5) ^ bitfield(op17phase, 3));

        // sum over all the desired channels
        for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
            if (bitfield(chanmask, chnum)) {
#if (YMFM_DEBUG_LOG_WAVFILES)
                auto reference = output;
#endif
                if (chnum == 6)
                    m_channel[chnum]->output_rhythm_ch6(output, rshift, clipmax);
                else if (chnum == 7)
                    m_channel[chnum]->output_rhythm_ch7(phase_select, output, rshift, clipmax);
                else if (chnum == 8)
                    m_channel[chnum]->output_rhythm_ch8(phase_select, output, rshift, clipmax);
                else if (m_channel[chnum]->is4op())
                    m_channel[chnum]->output_4op(output, rshift, clipmax);
                else
                    m_channel[chnum]->output_2op(output, rshift, clipmax);
#if (YMFM_DEBUG_LOG_WAVFILES)
                m_wavfile[chnum].add(output, reference);
#endif
            }
    } else {
        // sum over all the desired channels
        for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
            if (bitfield(chanmask, chnum)) {
#if (YMFM_DEBUG_LOG_WAVFILES)
                auto reference = output;
#endif
                if (m_channel[chnum]->is4op())
                    m_channel[chnum]->output_4op(output, rshift, clipmax);
                else
                    m_channel[chnum]->output_2op(output, rshift, clipmax);
#if (YMFM_DEBUG_LOG_WAVFILES)
                m_wavfile[chnum].add(output, reference);
#endif
            }
    }
}

//-------------------------------------------------
//  write - handle writes to the OPN registers
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::write(uint16_t regnum, uint8_t data)
{
    debug::log_fm_write("%03X = %02X\n", regnum, data);

    // special case: writes to the mode register can impact IRQs;
    // schedule these writes to ensure ordering with timers
    if (regnum == RegisterType::REG_MODE) {
        m_intf.ymfm_sync_mode_write(data);
        return;
    }

    // for now just mark all channels as modified
    m_modified_channels = ALL_CHANNELS;

    // most writes are passive, consumed only when needed
    uint32_t keyon_channel;
    uint32_t keyon_opmask;
    if (m_regs.write(regnum, data, keyon_channel, keyon_opmask)) {
        // handle writes to the keyon register(s)
        if (keyon_channel < CHANNELS) {
            // normal channel on/off
            m_channel[keyon_channel]->keyonoff(keyon_opmask, KEYON_NORMAL, keyon_channel);
        } else if (CHANNELS >= 9 && keyon_channel == RegisterType::RHYTHM_CHANNEL) {
            // special case for the OPL rhythm channels
            m_channel[6]->keyonoff(bitfield(keyon_opmask, 4) ? 3 : 0, KEYON_RHYTHM, 6);
            m_channel[7]->keyonoff(bitfield(keyon_opmask, 0) | (bitfield(keyon_opmask, 3) << 1), KEYON_RHYTHM, 7);
            m_channel[8]->keyonoff(bitfield(keyon_opmask, 2) | (bitfield(keyon_opmask, 1) << 1), KEYON_RHYTHM, 8);
        }
    }
}

//-------------------------------------------------
//  status - return the current state of the
//  status flags
//-------------------------------------------------

template <class RegisterType>
uint8_t fm_engine_base<RegisterType>::status() const
{
    return m_status & ~STATUS_BUSY & ~m_regs.status_mask();
}

//-------------------------------------------------
//  assign_operators - get the current mapping of
//  operators to channels and assign them all
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::assign_operators()
{
    typename RegisterType::operator_mapping map;
    m_regs.operator_map(map);

    for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
        for (uint32_t index = 0; index < 4; index++) {
            uint32_t opnum = bitfield(map.chan[chnum], 8 * index, 8);
            m_channel[chnum]->assign(index, (opnum == 0xff) ? nullptr : m_operator[opnum]);
        }
}

//-------------------------------------------------
//  update_timer - update the state of the given
//  timer
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::update_timer(uint32_t tnum, uint32_t enable, int32_t delta_clocks)
{
    // if the timer is live, but not currently enabled, set the timer
    if (enable && !m_timer_running[tnum]) {
        // period comes from the registers, and is different for each
        uint32_t period = (tnum == 0) ? (1024 - m_regs.timer_a_value()) : 16 * (256 - m_regs.timer_b_value());

        // caller can also specify a delta to account for other effects
        period += delta_clocks;

        // reset it
        m_intf.ymfm_set_timer(tnum, period * OPERATORS * m_clock_prescale);
        m_timer_running[tnum] = 1;
    }

    // if the timer is not live, ensure it is not enabled
    else if (!enable) {
        m_intf.ymfm_set_timer(tnum, -1);
        m_timer_running[tnum] = 0;
    }
}

//-------------------------------------------------
//  engine_timer_expired - timer has expired - signal
//  status and possibly IRQs
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::engine_timer_expired(uint32_t tnum)
{
    assert(tnum == 0 || tnum == 1);

    // update status
    if (tnum == 0 && m_regs.enable_timer_a())
        set_reset_status(STATUS_TIMERA, 0);
    else if (tnum == 1 && m_regs.enable_timer_b())
        set_reset_status(STATUS_TIMERB, 0);

    // if timer A fired in CSM mode, trigger CSM on all relevant channels
    if (tnum == 0 && m_regs.csm())
        for (uint32_t chnum = 0; chnum < CHANNELS; chnum++)
            if (bitfield(RegisterType::CSM_TRIGGER_MASK, chnum)) {
                m_channel[chnum]->keyonoff(0xf, KEYON_CSM, chnum);
                m_modified_channels |= 1 << chnum;
            }

    // reset
    m_timer_running[tnum] = false;
    update_timer(tnum, 1, 0);
}

//-------------------------------------------------
//  check_interrupts - check the interrupt sources
//  for interrupts
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::engine_check_interrupts()
{
    // update the state
    uint8_t old_state = m_irq_state;
    m_irq_state = ((m_status & m_irq_mask & ~m_regs.status_mask()) != 0);

    // set the IRQ status bit
    if (m_irq_state)
        m_status |= STATUS_IRQ;
    else
        m_status &= ~STATUS_IRQ;

    // if changed, signal the new state
    if (old_state != m_irq_state)
        m_intf.ymfm_update_irq(m_irq_state ? true : false);
}

//-------------------------------------------------
//  engine_mode_write - handle a mode register write
//  via timer callback
//-------------------------------------------------

template <class RegisterType>
void fm_engine_base<RegisterType>::engine_mode_write(uint8_t data)
{
    // mark all channels as modified
    m_modified_channels = ALL_CHANNELS;

    // actually write the mode register now
    uint32_t dummy1, dummy2;
    m_regs.write(RegisterType::REG_MODE, data, dummy1, dummy2);

    // reset IRQ status -- when written, all other bits are ignored
    // QUESTION: should this maybe just reset the IRQ bit and not all the bits?
    //   That is, check_interrupts would only set, this would only clear?
    if (m_regs.irq_reset())
        set_reset_status(0, 0x78);
    else {
        // reset timer status
        uint8_t reset_mask = 0;
        if (m_regs.reset_timer_b())
            reset_mask |= RegisterType::STATUS_TIMERB;
        if (m_regs.reset_timer_a())
            reset_mask |= RegisterType::STATUS_TIMERA;
        set_reset_status(0, reset_mask);

        // load timers; note that timer B gets a small negative adjustment because
        // the *16 multiplier is free-running, so the first tick of the clock
        // is a bit shorter
        update_timer(1, m_regs.load_timer_b(), -(m_total_clocks & 15));
        update_timer(0, m_regs.load_timer_a(), 0);
    }
}

//--------------------------------------------------------------------------------
// NOTE: Merged ymfm_opn.h and ymfm_opn.cpp by Yoji Suzuki
// **Extracted YM2612 and depended code only.**
//--------------------------------------------------------------------------------

//*********************************************************
//  REGISTER CLASSES
//*********************************************************

// ======================> opn_registers_base

//
// OPN register map:
//
//      System-wide registers:
//           21 xxxxxxxx Test register
//           22 ----x--- LFO enable [OPNA+ only]
//              -----xxx LFO rate [OPNA+ only]
//           24 xxxxxxxx Timer A value (upper 8 bits)
//           25 ------xx Timer A value (lower 2 bits)
//           26 xxxxxxxx Timer B value
//           27 xx------ CSM/Multi-frequency mode for channel #2
//              --x----- Reset timer B
//              ---x---- Reset timer A
//              ----x--- Enable timer B
//              -----x-- Enable timer A
//              ------x- Load timer B
//              -------x Load timer A
//           28 x------- Key on/off operator 4
//              -x------ Key on/off operator 3
//              --x----- Key on/off operator 2
//              ---x---- Key on/off operator 1
//              ------xx Channel select
//
//     Per-channel registers (channel in address bits 0-1)
//     Note that all these apply to address+100 as well on OPNA+
//        A0-A3 xxxxxxxx Frequency number lower 8 bits
//        A4-A7 --xxx--- Block (0-7)
//              -----xxx Frequency number upper 3 bits
//        B0-B3 --xxx--- Feedback level for operator 1 (0-7)
//              -----xxx Operator connection algorithm (0-7)
//        B4-B7 x------- Pan left [OPNA]
//              -x------ Pan right [OPNA]
//              --xx---- LFO AM shift (0-3) [OPNA+ only]
//              -----xxx LFO PM depth (0-7) [OPNA+ only]
//
//     Per-operator registers (channel in address bits 0-1, operator in bits 2-3)
//     Note that all these apply to address+100 as well on OPNA+
//        30-3F -xxx---- Detune value (0-7)
//              ----xxxx Multiple value (0-15)
//        40-4F -xxxxxxx Total level (0-127)
//        50-5F xx------ Key scale rate (0-3)
//              ---xxxxx Attack rate (0-31)
//        60-6F x------- LFO AM enable [OPNA]
//              ---xxxxx Decay rate (0-31)
//        70-7F ---xxxxx Sustain rate (0-31)
//        80-8F xxxx---- Sustain level (0-15)
//              ----xxxx Release rate (0-15)
//        90-9F ----x--- SSG-EG enable
//              -----xxx SSG-EG envelope (0-7)
//
//     Special multi-frequency registers (channel implicitly #2; operator in address bits 0-1)
//        A8-AB xxxxxxxx Frequency number lower 8 bits
//        AC-AF --xxx--- Block (0-7)
//              -----xxx Frequency number upper 3 bits
//
//     Internal (fake) registers:
//        B8-BB --xxxxxx Latched frequency number upper bits (from A4-A7)
//        BC-BF --xxxxxx Latched frequency number upper bits (from AC-AF)
//

template <bool IsOpnA>
class opn_registers_base : public fm_registers_base
{
  public:
    // constants
    static constexpr uint32_t OUTPUTS = IsOpnA ? 2 : 1;
    static constexpr uint32_t CHANNELS = IsOpnA ? 6 : 3;
    static constexpr uint32_t ALL_CHANNELS = (1 << CHANNELS) - 1;
    static constexpr uint32_t OPERATORS = CHANNELS * 4;
    static constexpr uint32_t WAVEFORMS = 1;
    static constexpr uint32_t REGISTERS = IsOpnA ? 0x200 : 0x100;
    static constexpr uint32_t REG_MODE = 0x27;
    static constexpr uint32_t DEFAULT_PRESCALE = 6;
    static constexpr uint32_t EG_CLOCK_DIVIDER = 3;
    static constexpr bool EG_HAS_SSG = true;
    static constexpr bool MODULATOR_DELAY = false;
    static constexpr uint32_t CSM_TRIGGER_MASK = 1 << 2;
    static constexpr uint8_t STATUS_TIMERA = 0x01;
    static constexpr uint8_t STATUS_TIMERB = 0x02;
    static constexpr uint8_t STATUS_BUSY = 0x80;
    static constexpr uint8_t STATUS_IRQ = 0;

    // constructor
    opn_registers_base();

    // reset to initial state
    void reset();

    // save/restore
    void save_restore(ymfm_saved_state& state);

    // map channel number to register offset
    static constexpr uint32_t channel_offset(uint32_t chnum)
    {
        assert(chnum < CHANNELS);
        if (!IsOpnA)
            return chnum;
        else
            return (chnum % 3) + 0x100 * (chnum / 3);
    }

    // map operator number to register offset
    static constexpr uint32_t operator_offset(uint32_t opnum)
    {
        assert(opnum < OPERATORS);
        if (!IsOpnA)
            return opnum + opnum / 3;
        else
            return (opnum % 12) + ((opnum % 12) / 3) + 0x100 * (opnum / 12);
    }

    // return an array of operator indices for each channel
    struct operator_mapping {
        uint32_t chan[CHANNELS];
    };
    void operator_map(operator_mapping& dest) const;

    // read a register value
    uint8_t read(uint16_t index) const { return m_regdata[index]; }

    // handle writes to the register array
    bool write(uint16_t index, uint8_t data, uint32_t& chan, uint32_t& opmask);

    // clock the noise and LFO, if present, returning LFO PM value
    int32_t clock_noise_and_lfo();

    // reset the LFO
    void reset_lfo() { m_lfo_counter = 0; }

    // return the AM offset from LFO for the given channel
    uint32_t lfo_am_offset(uint32_t choffs) const;

    // return LFO/noise states
    uint32_t noise_state() const { return 0; }

    // caching helpers
    void cache_operator_data(uint32_t choffs, uint32_t opoffs, opdata_cache& cache);

    // compute the phase step, given a PM value
    uint32_t compute_phase_step(uint32_t choffs, uint32_t opoffs, opdata_cache const& cache, int32_t lfo_raw_pm);

    // log a key-on event
    const char* log_keyon(uint32_t choffs, uint32_t opoffs);

    // system-wide registers
    uint32_t test() const { return byte(0x21, 0, 8); }
    uint32_t lfo_enable() const { return IsOpnA ? byte(0x22, 3, 1) : 0; }
    uint32_t lfo_rate() const { return IsOpnA ? byte(0x22, 0, 3) : 0; }
    uint32_t timer_a_value() const { return word(0x24, 0, 8, 0x25, 0, 2); }
    uint32_t timer_b_value() const { return byte(0x26, 0, 8); }
    uint32_t csm() const { return (byte(0x27, 6, 2) == 2); }
    uint32_t multi_freq() const { return (byte(0x27, 6, 2) != 0); }
    uint32_t reset_timer_b() const { return byte(0x27, 5, 1); }
    uint32_t reset_timer_a() const { return byte(0x27, 4, 1); }
    uint32_t enable_timer_b() const { return byte(0x27, 3, 1); }
    uint32_t enable_timer_a() const { return byte(0x27, 2, 1); }
    uint32_t load_timer_b() const { return byte(0x27, 1, 1); }
    uint32_t load_timer_a() const { return byte(0x27, 0, 1); }
    uint32_t multi_block_freq(uint32_t num) const { return word(0xac, 0, 6, 0xa8, 0, 8, num); }

    // per-channel registers
    uint32_t ch_block_freq(uint32_t choffs) const { return word(0xa4, 0, 6, 0xa0, 0, 8, choffs); }
    uint32_t ch_feedback(uint32_t choffs) const { return byte(0xb0, 3, 3, choffs); }
    uint32_t ch_algorithm(uint32_t choffs) const { return byte(0xb0, 0, 3, choffs); }
    uint32_t ch_output_any(uint32_t choffs) const { return IsOpnA ? byte(0xb4, 6, 2, choffs) : 1; }
    uint32_t ch_output_0(uint32_t choffs) const { return IsOpnA ? byte(0xb4, 7, 1, choffs) : 1; }
    uint32_t ch_output_1(uint32_t choffs) const { return IsOpnA ? byte(0xb4, 6, 1, choffs) : 0; }
    uint32_t ch_output_2(uint32_t choffs) const { return 0; }
    uint32_t ch_output_3(uint32_t choffs) const { return 0; }
    uint32_t ch_lfo_am_sens(uint32_t choffs) const { return IsOpnA ? byte(0xb4, 4, 2, choffs) : 0; }
    uint32_t ch_lfo_pm_sens(uint32_t choffs) const { return IsOpnA ? byte(0xb4, 0, 3, choffs) : 0; }

    // per-operator registers
    uint32_t op_detune(uint32_t opoffs) const { return byte(0x30, 4, 3, opoffs); }
    uint32_t op_multiple(uint32_t opoffs) const { return byte(0x30, 0, 4, opoffs); }
    uint32_t op_total_level(uint32_t opoffs) const { return byte(0x40, 0, 7, opoffs); }
    uint32_t op_ksr(uint32_t opoffs) const { return byte(0x50, 6, 2, opoffs); }
    uint32_t op_attack_rate(uint32_t opoffs) const { return byte(0x50, 0, 5, opoffs); }
    uint32_t op_decay_rate(uint32_t opoffs) const { return byte(0x60, 0, 5, opoffs); }
    uint32_t op_lfo_am_enable(uint32_t opoffs) const { return IsOpnA ? byte(0x60, 7, 1, opoffs) : 0; }
    uint32_t op_sustain_rate(uint32_t opoffs) const { return byte(0x70, 0, 5, opoffs); }
    uint32_t op_sustain_level(uint32_t opoffs) const { return byte(0x80, 4, 4, opoffs); }
    uint32_t op_release_rate(uint32_t opoffs) const { return byte(0x80, 0, 4, opoffs); }
    uint32_t op_ssg_eg_enable(uint32_t opoffs) const { return byte(0x90, 3, 1, opoffs); }
    uint32_t op_ssg_eg_mode(uint32_t opoffs) const { return byte(0x90, 0, 3, opoffs); }

  protected:
    // return a bitfield extracted from a byte
    uint32_t byte(uint32_t offset, uint32_t start, uint32_t count, uint32_t extra_offset = 0) const
    {
        return bitfield(m_regdata[offset + extra_offset], start, count);
    }

    // return a bitfield extracted from a pair of bytes, MSBs listed first
    uint32_t word(uint32_t offset1, uint32_t start1, uint32_t count1, uint32_t offset2, uint32_t start2, uint32_t count2, uint32_t extra_offset = 0) const
    {
        return (byte(offset1, start1, count1, extra_offset) << count2) | byte(offset2, start2, count2, extra_offset);
    }

    // internal state
    uint32_t m_lfo_counter;                          // LFO counter
    uint8_t m_lfo_am;                                // current LFO AM value
    uint8_t m_regdata[REGISTERS];                    // register data
    uint16_t m_waveform[WAVEFORMS][WAVEFORM_LENGTH]; // waveforms
};

using opn_registers = opn_registers_base<false>;
using opna_registers = opn_registers_base<true>;

// ======================> ym2612

class ym2612
{
  public:
    using fm_engine = fm_engine_base<opna_registers>;
    static constexpr uint32_t OUTPUTS = fm_engine::OUTPUTS;
    using output_data = fm_engine::output_data;

    // constructor
    ym2612(ymfm_interface& intf);

    // reset
    void reset();

    // save/restore
    void save_restore(ymfm_saved_state& state);

    // pass-through helpers
    uint32_t sample_rate(uint32_t input_clock) const { return m_fm.sample_rate(input_clock); }
    void invalidate_caches() { m_fm.invalidate_caches(); }

    // read access
    uint8_t read_status();
    uint8_t read(uint32_t offset);

    // write access
    void write_address(uint8_t data);
    void write_data(uint8_t data);
    void write_address_hi(uint8_t data);
    void write_data_hi(uint8_t data);
    void write(uint32_t offset, uint8_t data);

    // generate one sample of sound
    void generate(output_data* output, uint32_t numsamples = 1);

  protected:
    // simulate the DAC discontinuity
    constexpr int32_t dac_discontinuity(int32_t value) const { return (value < 0) ? (value - 3) : (value + 4); }

    // internal state
    uint16_t m_address;   // address register
    uint16_t m_dac_data;  // 9-bit DAC data
    uint8_t m_dac_enable; // DAC enabled?
    fm_engine m_fm;       // core FM engine
};

//*********************************************************
//  OPN/OPNA REGISTERS
//*********************************************************

//-------------------------------------------------
//  opn_registers_base - constructor
//-------------------------------------------------

template <bool IsOpnA>
opn_registers_base<IsOpnA>::opn_registers_base() : m_lfo_counter(0),
                                                   m_lfo_am(0)
{
    // create the waveforms
    for (uint32_t index = 0; index < WAVEFORM_LENGTH; index++)
        m_waveform[0][index] = abs_sin_attenuation(index) | (bitfield(index, 9) << 15);
}

//-------------------------------------------------
//  reset - reset to initial state
//-------------------------------------------------

template <bool IsOpnA>
void opn_registers_base<IsOpnA>::reset()
{
    for (uint32_t index = 0; index < REGISTERS; index++)
        m_regdata[index] = 0;
    if (IsOpnA) {
        // enable output on both channels by default
        m_regdata[0xb4] = m_regdata[0xb5] = m_regdata[0xb6] = 0xc0;
        m_regdata[0x1b4] = m_regdata[0x1b5] = m_regdata[0x1b6] = 0xc0;
    }
}

//-------------------------------------------------
//  save_restore - save or restore the data
//-------------------------------------------------

template <bool IsOpnA>
void opn_registers_base<IsOpnA>::save_restore(ymfm_saved_state& state)
{
    if (IsOpnA) {
        state.save_restore(m_lfo_counter);
        state.save_restore(m_lfo_am);
    }
    state.save_restore(m_regdata);
}

//-------------------------------------------------
//  operator_map - return an array of operator
//  indices for each channel; for OPN this is fixed
//-------------------------------------------------

template <>
inline void opn_registers_base<false>::operator_map(operator_mapping& dest) const
{
    // Note that the channel index order is 0,2,1,3, so we bitswap the index.
    //
    // This is because the order in the map is:
    //    carrier 1, carrier 2, modulator 1, modulator 2
    //
    // But when wiring up the connections, the more natural order is:
    //    carrier 1, modulator 1, carrier 2, modulator 2
    static const operator_mapping s_fixed_map =
        {{
            operator_list(0, 6, 3, 9),  // Channel 0 operators
            operator_list(1, 7, 4, 10), // Channel 1 operators
            operator_list(2, 8, 5, 11), // Channel 2 operators
        }};
    dest = s_fixed_map;
}

template <>
inline void opn_registers_base<true>::operator_map(operator_mapping& dest) const
{
    // Note that the channel index order is 0,2,1,3, so we bitswap the index.
    //
    // This is because the order in the map is:
    //    carrier 1, carrier 2, modulator 1, modulator 2
    //
    // But when wiring up the connections, the more natural order is:
    //    carrier 1, modulator 1, carrier 2, modulator 2
    static const operator_mapping s_fixed_map =
        {{
            operator_list(0, 6, 3, 9),     // Channel 0 operators
            operator_list(1, 7, 4, 10),    // Channel 1 operators
            operator_list(2, 8, 5, 11),    // Channel 2 operators
            operator_list(12, 18, 15, 21), // Channel 3 operators
            operator_list(13, 19, 16, 22), // Channel 4 operators
            operator_list(14, 20, 17, 23), // Channel 5 operators
        }};
    dest = s_fixed_map;
}

//-------------------------------------------------
//  write - handle writes to the register array
//-------------------------------------------------

template <bool IsOpnA>
bool opn_registers_base<IsOpnA>::write(uint16_t index, uint8_t data, uint32_t& channel, uint32_t& opmask)
{
    assert(index < REGISTERS);

    // writes in the 0xa0-af/0x1a0-af region are handled as latched pairs
    // borrow unused registers 0xb8-bf/0x1b8-bf as temporary holding locations
    if ((index & 0xf0) == 0xa0) {
        if (bitfield(index, 0, 2) == 3)
            return false;

        uint32_t latchindex = 0xb8 | bitfield(index, 3);
        if (IsOpnA)
            latchindex |= index & 0x100;

        // writes to the upper half just latch (only low 6 bits matter)
        if (bitfield(index, 2))
            m_regdata[latchindex] = data | 0x80;

        // writes to the lower half only commit if the latch is there
        else if (bitfield(m_regdata[latchindex], 7)) {
            m_regdata[index] = data;
            m_regdata[index | 4] = m_regdata[latchindex] & 0x3f;
            m_regdata[latchindex] = 0;
        }
        return false;
    } else if ((index & 0xf8) == 0xb8) {
        // registers 0xb8-0xbf are used internally
        return false;
    }

    // everything else is normal
    m_regdata[index] = data;

    // handle writes to the key on index
    if (index == 0x28) {
        channel = bitfield(data, 0, 2);
        if (channel == 3)
            return false;
        if (IsOpnA)
            channel += bitfield(data, 2, 1) * 3;
        opmask = bitfield(data, 4, 4);
        return true;
    }
    return false;
}

//-------------------------------------------------
//  clock_noise_and_lfo - clock the noise and LFO,
//  handling clock division, depth, and waveform
//  computations
//-------------------------------------------------

template <bool IsOpnA>
int32_t opn_registers_base<IsOpnA>::clock_noise_and_lfo()
{
    // OPN has no noise generation

    // if LFO not enabled (not present on OPN), quick exit with 0s
    if (!IsOpnA || !lfo_enable()) {
        m_lfo_counter = 0;

        // special case: if LFO is disabled on OPNA, it basically just keeps the counter
        // at 0; since position 0 gives an AM value of 0x3f, it is important to reflect
        // that here; for example, MegaDrive Venom plays some notes with LFO globally
        // disabled but enabling LFO on the operators, and it expects this added attenutation
        m_lfo_am = IsOpnA ? 0x3f : 0x00;
        return 0;
    }

    // this table is based on converting the frequencies in the applications
    // manual to clock dividers, based on the assumption of a 7-bit LFO value
    static uint8_t const lfo_max_count[8] = {109, 78, 72, 68, 63, 45, 9, 6};
    uint32_t subcount = uint8_t(m_lfo_counter++);

    // when we cross the divider count, add enough to zero it and cause an
    // increment at bit 8; the 7-bit value lives from bits 8-14
    if (subcount >= lfo_max_count[lfo_rate()]) {
        // note: to match the published values this should be 0x100 - subcount;
        // however, tests on the hardware and nuked bear out an off-by-one
        // error exists that causes the max LFO rate to be faster than published
        m_lfo_counter += 0x101 - subcount;
    }

    // AM value is 7 bits, staring at bit 8; grab the low 6 directly
    m_lfo_am = bitfield(m_lfo_counter, 8, 6);

    // first half of the AM period (bit 6 == 0) is inverted
    if (bitfield(m_lfo_counter, 8 + 6) == 0)
        m_lfo_am ^= 0x3f;

    // PM value is 5 bits, starting at bit 10; grab the low 3 directly
    int32_t pm = bitfield(m_lfo_counter, 10, 3);

    // PM is reflected based on bit 3
    if (bitfield(m_lfo_counter, 10 + 3))
        pm ^= 7;

    // PM is negated based on bit 4
    return bitfield(m_lfo_counter, 10 + 4) ? -pm : pm;
}

//-------------------------------------------------
//  lfo_am_offset - return the AM offset from LFO
//  for the given channel
//-------------------------------------------------

template <bool IsOpnA>
uint32_t opn_registers_base<IsOpnA>::lfo_am_offset(uint32_t choffs) const
{
    // shift value for AM sensitivity is [7, 3, 1, 0],
    // mapping to values of [0, 1.4, 5.9, and 11.8dB]
    uint32_t am_shift = (1 << (ch_lfo_am_sens(choffs) ^ 3)) - 1;

    // QUESTION: max sensitivity should give 11.8dB range, but this value
    // is directly added to an x.8 attenuation value, which will only give
    // 126/256 or ~4.9dB range -- what am I missing? The calculation below
    // matches several other emulators, including the Nuked implemenation.

    // raw LFO AM value on OPN is 0-3F, scale that up by a factor of 2
    // (giving 7 bits) before applying the final shift
    return (m_lfo_am << 1) >> am_shift;
}

//-------------------------------------------------
//  cache_operator_data - fill the operator cache
//  with prefetched data
//-------------------------------------------------

template <bool IsOpnA>
void opn_registers_base<IsOpnA>::cache_operator_data(uint32_t choffs, uint32_t opoffs, opdata_cache& cache)
{
    // set up the easy stuff
    cache.waveform = &m_waveform[0][0];

    // get frequency from the channel
    uint32_t block_freq = cache.block_freq = ch_block_freq(choffs);

    // if multi-frequency mode is enabled and this is channel 2,
    // fetch one of the special frequencies
    if (multi_freq() && choffs == 2) {
        if (opoffs == 2)
            block_freq = cache.block_freq = multi_block_freq(1);
        else if (opoffs == 10)
            block_freq = cache.block_freq = multi_block_freq(2);
        else if (opoffs == 6)
            block_freq = cache.block_freq = multi_block_freq(0);
    }

    // compute the keycode: block_freq is:
    //
    //     BBBFFFFFFFFFFF
    //     ^^^^???
    //
    // the 5-bit keycode uses the top 4 bits plus a magic formula
    // for the final bit
    uint32_t keycode = bitfield(block_freq, 10, 4) << 1;

    // lowest bit is determined by a mix of next lower FNUM bits
    // according to this equation from the YM2608 manual:
    //
    //   (F11 & (F10 | F9 | F8)) | (!F11 & F10 & F9 & F8)
    //
    // for speed, we just look it up in a 16-bit constant
    keycode |= bitfield(0xfe80, bitfield(block_freq, 7, 4));

    // detune adjustment
    cache.detune = detune_adjustment(op_detune(opoffs), keycode);

    // multiple value, as an x.1 value (0 means 0.5)
    cache.multiple = op_multiple(opoffs) * 2;
    if (cache.multiple == 0)
        cache.multiple = 1;

    // phase step, or PHASE_STEP_DYNAMIC if PM is active; this depends on
    // block_freq, detune, and multiple, so compute it after we've done those
    if (!IsOpnA || lfo_enable() == 0 || ch_lfo_pm_sens(choffs) == 0)
        cache.phase_step = compute_phase_step(choffs, opoffs, cache, 0);
    else
        cache.phase_step = opdata_cache::PHASE_STEP_DYNAMIC;

    // total level, scaled by 8
    cache.total_level = op_total_level(opoffs) << 3;

    // 4-bit sustain level, but 15 means 31 so effectively 5 bits
    cache.eg_sustain = op_sustain_level(opoffs);
    cache.eg_sustain |= (cache.eg_sustain + 1) & 0x10;
    cache.eg_sustain <<= 5;

    // determine KSR adjustment for enevlope rates
    uint32_t ksrval = keycode >> (op_ksr(opoffs) ^ 3);
    cache.eg_rate[EG_ATTACK] = effective_rate(op_attack_rate(opoffs) * 2, ksrval);
    cache.eg_rate[EG_DECAY] = effective_rate(op_decay_rate(opoffs) * 2, ksrval);
    cache.eg_rate[EG_SUSTAIN] = effective_rate(op_sustain_rate(opoffs) * 2, ksrval);
    cache.eg_rate[EG_RELEASE] = effective_rate(op_release_rate(opoffs) * 4 + 2, ksrval);
}

//-------------------------------------------------
//  compute_phase_step - compute the phase step
//-------------------------------------------------

template <bool IsOpnA>
uint32_t opn_registers_base<IsOpnA>::compute_phase_step(uint32_t choffs, uint32_t opoffs, opdata_cache const& cache, int32_t lfo_raw_pm)
{
    // OPN phase calculation has only a single detune parameter
    // and uses FNUMs instead of keycodes

    // extract frequency number (low 11 bits of block_freq)
    uint32_t fnum = bitfield(cache.block_freq, 0, 11) << 1;

    // if there's a non-zero PM sensitivity, compute the adjustment
    uint32_t pm_sensitivity = ch_lfo_pm_sens(choffs);
    if (pm_sensitivity != 0) {
        // apply the phase adjustment based on the upper 7 bits
        // of FNUM and the PM depth parameters
        fnum += opn_lfo_pm_phase_adjustment(bitfield(cache.block_freq, 4, 7), pm_sensitivity, lfo_raw_pm);

        // keep fnum to 12 bits
        fnum &= 0xfff;
    }

    // apply block shift to compute phase step
    uint32_t block = bitfield(cache.block_freq, 11, 3);
    uint32_t phase_step = (fnum << block) >> 2;

    // apply detune based on the keycode
    phase_step += cache.detune;

    // clamp to 17 bits in case detune overflows
    // QUESTION: is this specific to the YM2612/3438?
    phase_step &= 0x1ffff;

    // apply frequency multiplier (which is cached as an x.1 value)
    return (phase_step * cache.multiple) >> 1;
}

//-------------------------------------------------
//  log_keyon - log a key-on event
//-------------------------------------------------

template <bool IsOpnA>
const char* opn_registers_base<IsOpnA>::log_keyon(uint32_t choffs, uint32_t opoffs)
{
    uint32_t chnum = (choffs & 3) + 3 * bitfield(choffs, 8);
    uint32_t opnum = (opoffs & 15) - ((opoffs & 15) / 4) + 12 * bitfield(opoffs, 8);

    uint32_t block_freq = ch_block_freq(choffs);
    if (multi_freq() && choffs == 2) {
        if (opoffs == 2)
            block_freq = multi_block_freq(1);
        else if (opoffs == 10)
            block_freq = multi_block_freq(2);
        else if (opoffs == 6)
            block_freq = multi_block_freq(0);
    }

    static char buffer[256];
    int end = 0;
    buffer[0] = '\0';

    end += snprintf(&buffer[end], sizeof(buffer) - end, "%u.%02u freq=%04X dt=%u fb=%u alg=%X mul=%X tl=%02X ksr=%u adsr=%02X/%02X/%02X/%X sl=%X",
                    chnum, opnum,
                    block_freq,
                    op_detune(opoffs),
                    ch_feedback(choffs),
                    ch_algorithm(choffs),
                    op_multiple(opoffs),
                    op_total_level(opoffs),
                    op_ksr(opoffs),
                    op_attack_rate(opoffs),
                    op_decay_rate(opoffs),
                    op_sustain_rate(opoffs),
                    op_release_rate(opoffs),
                    op_sustain_level(opoffs));

    if (OUTPUTS > 1)
        end += snprintf(&buffer[end], sizeof(buffer) - end, " out=%c%c",
                        ch_output_0(choffs) ? 'L' : '-',
                        ch_output_1(choffs) ? 'R' : '-');
    if (op_ssg_eg_enable(opoffs))
        end += snprintf(&buffer[end], sizeof(buffer) - end, " ssg=%X", op_ssg_eg_mode(opoffs));
    bool am = (op_lfo_am_enable(opoffs) && ch_lfo_am_sens(choffs) != 0);
    if (am)
        end += snprintf(&buffer[end], sizeof(buffer) - end, " am=%u", ch_lfo_am_sens(choffs));
    bool pm = (ch_lfo_pm_sens(choffs) != 0);
    if (pm)
        end += snprintf(&buffer[end], sizeof(buffer) - end, " pm=%u", ch_lfo_pm_sens(choffs));
    if (am || pm)
        end += snprintf(&buffer[end], sizeof(buffer) - end, " lfo=%02X", lfo_rate());
    if (multi_freq() && choffs == 2)
        end += snprintf(&buffer[end], sizeof(buffer) - end, " multi=1");

    return buffer;
}

//*********************************************************
//  YM2612
//*********************************************************

//-------------------------------------------------
//  ym2612 - constructor
//-------------------------------------------------

inline ym2612::ym2612(ymfm_interface& intf) : m_address(0),
                                              m_dac_data(0),
                                              m_dac_enable(0),
                                              m_fm(intf)
{
}

//-------------------------------------------------
//  reset - reset the system
//-------------------------------------------------

inline void ym2612::reset()
{
    // reset the engines
    m_fm.reset();
}

//-------------------------------------------------
//  save_restore - save or restore the data
//-------------------------------------------------

inline void ym2612::save_restore(ymfm_saved_state& state)
{
    state.save_restore(m_address);
    state.save_restore(m_dac_data);
    state.save_restore(m_dac_enable);
    m_fm.save_restore(state);
}

//-------------------------------------------------
//  read_status - read the status register
//-------------------------------------------------

inline uint8_t ym2612::read_status()
{
    uint8_t result = m_fm.status();
    if (m_fm.intf().ymfm_is_busy())
        result |= fm_engine::STATUS_BUSY;
    return result;
}

//-------------------------------------------------
//  read - handle a read from the device
//-------------------------------------------------

inline uint8_t ym2612::read(uint32_t offset)
{
    uint8_t result = 0;
    switch (offset & 3) {
        case 0: // status port, YM2203 compatible
            result = read_status();
            break;

        case 1: // data port (unused)
        case 2: // status port, extended
        case 3: // data port (unused)
            debug::log_unexpected_read_write("Unexpected read from YM2612 offset %d\n", offset & 3);
            break;
    }
    return result;
}

//-------------------------------------------------
//  write_address - handle a write to the address
//  register
//-------------------------------------------------

inline void ym2612::write_address(uint8_t data)
{
    // just set the address
    m_address = data;
}

//-------------------------------------------------
//  write_data - handle a write to the data
//  register
//-------------------------------------------------

inline void ym2612::write_data(uint8_t data)
{
    // ignore if paired with upper address
    if (bitfield(m_address, 8))
        return;

    if (m_address == 0x2a) {
        // 2A: DAC data (most significant 8 bits)
        m_dac_data = (m_dac_data & ~0x1fe) | ((data ^ 0x80) << 1);
    } else if (m_address == 0x2b) {
        // 2B: DAC enable (bit 7)
        m_dac_enable = bitfield(data, 7);
    } else if (m_address == 0x2c) {
        // 2C: test/low DAC bit
        m_dac_data = (m_dac_data & ~1) | bitfield(data, 3);
    } else {
        // 00-29, 2D-FF: write to FM
        m_fm.write(m_address, data);
    }

    // mark busy for a bit
    m_fm.intf().ymfm_set_busy_end(32 * m_fm.clock_prescale());
}

//-------------------------------------------------
//  write_address_hi - handle a write to the upper
//  address register
//-------------------------------------------------

inline void ym2612::write_address_hi(uint8_t data)
{
    // just set the address
    m_address = 0x100 | data;
}

//-------------------------------------------------
//  write_data_hi - handle a write to the upper
//  data register
//-------------------------------------------------

inline void ym2612::write_data_hi(uint8_t data)
{
    // ignore if paired with upper address
    if (!bitfield(m_address, 8))
        return;

    // 100-1FF: write to FM
    m_fm.write(m_address, data);

    // mark busy for a bit
    m_fm.intf().ymfm_set_busy_end(32 * m_fm.clock_prescale());
}

//-------------------------------------------------
//  write - handle a write to the register
//  interface
//-------------------------------------------------

inline void ym2612::write(uint32_t offset, uint8_t data)
{
    switch (offset & 3) {
        case 0: // address port
            write_address(data);
            break;

        case 1: // data port
            write_data(data);
            break;

        case 2: // upper address port
            write_address_hi(data);
            break;

        case 3: // upper data port
            write_data_hi(data);
            break;
    }
}

//-------------------------------------------------
//  generate - generate one sample of sound
//-------------------------------------------------

inline void ym2612::generate(output_data* output, uint32_t numsamples)
{
    for (uint32_t samp = 0; samp < numsamples; samp++, output++) {
        // clock the system
        m_fm.clock(fm_engine::ALL_CHANNELS);

        // sum individual channels to apply DAC discontinuity on each
        output->clear();
        output_data temp;

        // first do FM-only channels; OPN2 is 9-bit with intermediate clipping
        int const last_fm_channel = m_dac_enable ? 5 : 6;
        for (int chan = 0; chan < last_fm_channel; chan++) {
            m_fm.output(temp.clear(), 5, 256, 1 << chan);
            output->data[0] += dac_discontinuity(temp.data[0]);
            output->data[1] += dac_discontinuity(temp.data[1]);
        }

        // add in DAC
        if (m_dac_enable) {
            // DAC enabled: start with DAC value then add the first 5 channels only
            int32_t dacval = dac_discontinuity(int16_t(m_dac_data << 7) >> 7);
            output->data[0] += m_fm.regs().ch_output_0(0x102) ? dacval : dac_discontinuity(0);
            output->data[1] += m_fm.regs().ch_output_1(0x102) ? dacval : dac_discontinuity(0);
        }

        // output is technically multiplexed rather than mixed, but that requires
        // a better sound mixer than we usually have, so just average over the six
        // channels; also apply a 64/65 factor to account for the discontinuity
        // adjustment above
        output->data[0] = (output->data[0] * 128) * 64 / (6 * 65);
        output->data[1] = (output->data[1] * 128) * 64 / (6 * 65);
    }
}

} // namespace ymfm

#endif // YMFM_H
