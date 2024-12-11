enum IO {
    joypad = 0xA0                   ; Joypad (IN only)
    bank0 = 0xB0                    ; Bank0 (I/O)
    bank1                           ; Bank1 (I/O)
    bank2                           ; Bank2 (I/O)
    bank3                           ; Bank3 (I/O)
    ram_bank                        ; Extra RAM bank (I/O)
    ram_dup                         ; Duplicate Extra RAM Bank (OUT only)
    rom2chr = 0xC0                  ; ROM to Character DMA (OUT only)
    rom2ram                         ; ROM to Memory DMA (OUT only)
    memset                          ; memset DMA (OUT only)
    memcpy                          ; memcpy DMA (OUT only)
    collision                       ; Collision check (IN only)
    calculation                     ; MUL, DIV, MOD (OUT only)
    sin_io                          ; SIN (OUT only)
    cos_io                          ; COS (OUT only)
    atan2                           ; ATAN2 (IN only)
    rand8                           ; 8-bits random (I/O)
    rand16                          ; 16-bits random (I/O)
    perlin_seed                     ; Perlin Noise Seed (OUT only)
    perlin_x_scale                  ; Perlin Noise X Scale (OUT only)
    perlin_y_scale                  ; Perlin Noise Y Scale (OUT only)
    perlin_get                      ; Get Perlin Noise (IN only)
    perlin_get_oct                  ; Get Perlin Noise with Octave (IN only)
    angle                           ; Get angle (IN=atan2, OUT=specific angle)
    percent                         ; Calculate 0-255% of HL (OUT), or BC/DE (IN)
    save_load = 0xDA                ; Save (OUT) and Load (IN)
    bgm_play = 0xE0                 ; Play BGM (OUT)
    bgm_ctrl                        ; Pause(0), Resume(1), Fadeout(2) BGM (OUT)
    sfx_play = 0xF0                 ; Play SFX (OUT)
    sfx_stop                        ; Stop SFX (OUT)
    sfx_check                       ; Check playing SFX (OUT)
}

// IN A, (IO.joypad)
// BIT {below}, A
enum PAD {
    b                               ; B button (Fire, Dash)
    a                               ; A button (Bomber, Jump)
    select                          ; SELECT button
    start                           ; START button
    right                           ; Right cursor
    left                            ; Left cursor
    down                            ; Down cursor
    up                              ; Up cursor
}

struct OAM $9000 {
    y           ds.b    1           ; Y-coordinate
    x           ds.b    1           ; X-coordinate
    ptn         ds.b    1           ; Pattern number
    attr        ds.b    1           ; Attribute
    h           ds.b    1           ; Height
    w           ds.b    1           ; Width
    bank        ds.b    1           ; Bank number
    attr2       ds.b    1           ; Attribute 2
}

struct OAM16 $9A00 {
    y           ds.w    1           ; Y-coordinate
    x           ds.w    1           ; X-coordinate
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
    interlace   ds.b    1           ; R12: Interlace 
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
    OUT (IO.rom2chr), A
    POP AF
}

#macro dma2mem(destination, bank_number, bank_offset, bank_size)
{
    PUSH AF
    PUSH BC
    PUSH DE
    PUSH HL
    LD A, bank_number
    LD BC, bank_offset
    LD DE, bank_size
    LD HL, destination
    OUT (IO.rom2ram), A
    POP HL
    POP DE
    POP BC
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
    OUT (IO.memset), A
    POP HL
    POP BC
    POP AF
}

#macro bgm_play(number)
{
    PUSH AF
    LD A, number
    OUT (IO.bgm_play), A
    POP AF
}

#macro bgm_pause()
{
    PUSH AF
    LD A, $00
    OUT (IO.bgm_ctrl), A
    POP AF
}

#macro bgm_resume()
{
    PUSH AF
    LD A, $01
    OUT (IO.bgm_ctrl), A
    POP AF
}

#macro bgm_fadeout()
{
    PUSH AF
    LD A, $02
    OUT (IO.bgm_ctrl), A
    POP AF
}

#macro sfx_play(number)
{
    PUSH AF
    LD A, number
    OUT (IO.sfx_play), A
    POP AF
}

#macro sfx_stop(number)
{
    PUSH AF
    LD A, number
    OUT (IO.sfx_stop), A
    POP AF
}
