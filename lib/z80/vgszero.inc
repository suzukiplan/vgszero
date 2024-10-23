#define IO.joypad       0xA0
#define IO.bank0        0xB0
#define IO.bank1        0xB1
#define IO.bank2        0xB2
#define IO.bank3        0xB3
#define IO.ram.bank     0xB4
#define IO.ram.dup      0xB5
#define IO.dma          0xC0
#define IO.bank         0xC1
#define IO.memset       0xC2
#define IO.memcpy       0xC3
#define IO.collision    0xC4
#define IO.rand8        0xC9
#define IO.rand16       0xCA
#define IO.save         0xDA
#define IO.bgm.play     0xE0
#define IO.bgm.ctrl     0xE1
#define IO.eff.play     0xF0
#define IO.eff.stop     0xF1
#define IO.eff.check    0xF2

// IN A, (IO.joypad)
// BIT {below}, A
#define PAD.up          7
#define PAD.down        6
#define PAD.left        5
#define PAD.right       4
#define PAD.start       3
#define PAD.select      2
#define PAD.a           1
#define PAD.b           0

struct OAM $9000 {
    y           ds.b    1
    x           ds.b    1
    ptn         ds.b    1
    attr        ds.b    1
    h           ds.b    1
    w           ds.b    1
    bank        ds.b    1
    reserved    ds.b    1
}

struct OAM16 $9A00 {
    y           ds.w    1
    x           ds.w    1
}

struct VRAM $8000 {
    bg_name     ds.b    32 * 32     ; BG name table
    bg_attr     ds.b    32 * 32     ; BG attribute table
    fg_name     ds.b    32 * 32     ; FG name table
    fg_attr     ds.b    32 * 32     ; FG attribute table
    oam         OAM     256         ; Sprites (Object Attribute Memory)
    palette     ds.w    16 * 16     ; Palettes
    oam16       OAM16   256         ; 16bit position for sprites
    reserved    ds.b    0x100       ; reserved area
    vcnt        ds.b    1           ; R0: Scanline Vertical Counter
    hcnt        ds.b    1           ; R1: Scanline Horizontal Counter
    bgSX        ds.b    1           ; R2: BG Scroll X
    bgSY        ds.b    1           ; R3: BG Scroll Y
    fgSX        ds.b    1           ; R4: FG Scroll X
    fgSY        ds.b    1           ; R5: FG Scroll Y
    irq         ds.b    1           ; R6: IRQ scanline position
    status      ds.b    1           ; R7: Status
    bgDPM       ds.b    1           ; R8: BG Direct Pattern Maaping
    fgDPM       ds.b    1           ; R9: FG Direct Pattern Maaping
    spDPM       ds.b    1           ; R10: Sprite Direct Pattern Maaping
    mode1024    ds.b    1           ; R11: 1024 pattern mode
}

#macro wait_vblank()
{
    PUSH AF
    LD A, (VRAM.status)
    BIT 7, A
    JR Z, -7
    POP AF
}

#macro dma2chr(bank_number)
{
    PUSH AF
    LD A, bank_number
    OUT (IO.dma), A
    POP AF
}

#macro memcpy(destination, source, size)
{
    PUSH BC
    PUSH DE
    PUSH HL
    LD BC, destination
    LD DE, source
    LD HL, size
    OUT (IO.memcpy), A
    POP HL
    POP DE
    POP BC
}

#macro memset(destination, value, size)
{
    PUSH AF
    PUSH BC
    PUSH HL
    LD BC, destination
    LD A, value
    LD HL, size
    OUT (IO.memcpy), A
    POP HL
    POP BC
    POP AF
}

#macro bgm_play(bgm_number)
{
    PUSH AF
    LD A, bgm_number
    OUT (IO.bgm.play), A
    POP AF
}

#macro bgm_pause()
{
    PUSH AF
    LD A, $00
    OUT (IO.bgm.ctrl), A
    POP AF
}

#macro bgm_resume()
{
    PUSH AF
    LD A, $01
    OUT (IO.bgm.ctrl), A
    POP AF
}

#macro bgm_fadeout()
{
    PUSH AF
    LD A, $02
    OUT (IO.bgm.ctrl), A
    POP AF
}

#macro eff_play(eff_number)
{
    PUSH AF
    LD A, eff_number
    OUT (IO.eff.play), A
    POP AF
}

#macro eff_stop(eff_number)
{
    PUSH AF
    LD A, eff_number
    OUT (IO.eff.stop), A
    POP AF
}
