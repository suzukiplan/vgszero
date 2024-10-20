# WIP: Z80 Assembler for VGS-Zero

Z80 Assembler for VGS-Zero is a Z80 assembler that supports the entire Z80 instruction set, including hidden instructions, and enables highly readable full assembly language programming using [“structures”](#struct).

Although it was developed for use in game development on the VGS-Zero, it is versatile enough to be used for game development for a wide range of game consoles and PCs, including SMS, GameGear, MSX, and PC-88. _(However, the design guideline is to specialize in VGS-Zero, but not in other console or PC-specific enhancements.)_

## WIP Status

- It is still under development.
- We plan to add version `1.0.0` tags when all functions are completed.
- See the [issues](https://github.com/suzukiplan/vgsasm/issues?q=is%3Aopen+is%3Aissue+label%3AWIP).

## Support OS

- **Linux** (confirmed Ubuntu Desktop)
- macOS (not confirmed)

## Usage

```
vgsasm [-b size_of_binary] /path/to/source.asm
```

- `-b size_of_binary`
  - Specify the size of the output binary.
  - The `size_of_binary` can be specified in decimal or hexadecimal (ex: `0xABCD`)
  - The `size_of_binary` cannot be less than `1`.
  - The upper limit of `size_of_binary` is `65536` (`0x10000`)
  - Assembly will fail if the specified size is exceeded.
  - Boundary areas that do not meet the specified size are filled with `0xFF`.

## Basic Syntax

- Case insensitive (except for string literals)
- Hexadecimal numbers can be written in the format `$ABCD` or `0xABCD`.
- Binary numbers can be written in the format `%0101` or `0b0101`.
- Binary, decimal, and hexadecimal numbers can be four arithmetic operations (`+`, `-`, `*`, `/`)
  - `LD A, 1 + 2 + 3` is `LD A, 6`
  - `LD A, (0x10 + 3 * 2 - %0101)` is `LD A, (17)`
  - `LD A, (0x10 + 3 * 2 - %0101) / 3` is `LD A, 5`
- Strings after `;` or `//` are ignored as comments.

## `#include`

```z80
#include "/path/to/file"
```

- Other source files can be combined.
- Duplicate `#include` for the same source file are automatically ignored.

## `#binary`

```z80
#binary "/path/to/binary.bin"
```

Incorporates a binary file as code at the current program counter location.

## `#define`

```
#define DEFINITION_NAME Expanded expression
```

The `DEFINITION_NAME` in the source code is expanded to an `Expanded expression`.

## `org`

Specifies the starting address for binary output.

```z80
org $0000
```

In vgsasm, multiple `ORG`s can be specified, for example, if the code for IM1 interrupt is to be generated from 0x0038, it can be written as follows:

```z80
org $0000
.Reset
    ld sp, 0
    im 1
    ei
    jp Main

org $0038
.Interrupt
    // Interrupt procedure
    reti

org $0100
.Main
    // Main procedure
```

The following points should be noted:

- `ORG` must be defined in ascending order from the beginning of the source code.
- The free space from `ORG` to the next `ORG` is filled with 0xFF.

## Labels

```z80
// Normal label
FOO:

// Normal label
LABEL1:

// Inner label at LABEL1: FOO@LABEL1
@FOO

JP FOO          ; Jump to FOO:
JP @FOO         ; Jump to FOO@LABEL1
JP FOO@LABEL1   ; Jump to FOO@LABEL1
JP FOO@LABEL2   ; Jump to FOO@LABEL2

// Normal label (same as LABEL:)
.LABEL2

// Inner label at LABEL2: FOO@LABEL2
@FOO

JP FOO          ; Jump to FOO:
JP @FOO         ; Jump to FOO@LABEL2
JP FOO@LABEL1   ; Jump to FOO@LABEL1
JP FOO@LABEL2   ; Jump to FOO@LABEL2
```

- Labels are written in the format `LABEL:` or `.LABEL`.
- Inner labels are written in `@LABEL` format.
- Labels and inner labels are not case sensitive.
- The label and inner label are symbols that uniquely identify all source files with `#include`.

## `struct`

```z80
struct name $C000 {
    var1 ds.b 1     ; name.var1 = $C000 ... offset(name.var1) = 0
    var2 ds.w 1     ; name.var2 = $C001 ... offset(name.var2) = 1
    var3 ds.b 4     ; name.var3 = $C003 ... offset(name.var3) = 3
    var4 ds.w 4     ; name.var4 = $C007 ... offset(name.var4) = 7
}                   ; sizeof(name) = 15

// Array access
// name[0].var1 = $C000
// name[1].var1 = $C00F
// name[2].var1 = $C01E
```

- You can define a structure with `struct name start_address`.
- A struct is a grouping of single or multiple attributes in a common namespace.
- Attributes are specified in the form `attribute_name {ds.b|ds.w|name} count`.
  - `ds.b` ... 1 byte
  - `ds.w` ... 2 bytes
  - `name` ... `sizeof(name)`
- The `start_address` is usually assumed to be an absolute address in RAM (0xC000 to 0xFFFF).
- This structure has no boundary.
- Can also be accessed as an array in the form `name[index]`.
- `sizeof(structure_name)`: size of structure
- `offset(structure_name.field)` : field address offset

Utilizing `sizeof` and `offset` makes structure access with `LD (IX+d)` smarter.

```z80
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

org $0000

.Main
    ld ix, OAM[16]                      ; ix = $9000 + 8 * 16
    ld b, 4                             ; djnz loop times = 4
@Loop
    ld (ix + offset(OAM.x)), 0x10       ; ld (ix + 1), 0x10
    ld (ix + offset(OAM.y)), 0x20       ; ld (ix + 0), 0x20
    ld (ix + offset(OAM.ptn)), 0x30     ; ld (ix + 2), 0x30
    ld (ix + offset(OAM.bank)), 0x40    ; ld (ix + 6), 0x40
    add ix, sizeof(OAM)                 ; ix += 8
    djnz @Loop                          ; loop
```

## Literal Data Definition

```z80
Data:
@Text8  DB "Hello", ",", "World!", 0
@Num8   DB -128, -1, 0, 1, 127, 255
@Hex8   DEFB $00, $10, $20, $30, $40, $50, $60, $70, $80, $90, $A0, $B0, $C0, $D0, $E0, $F0

@Text16 DW "Hello", ",", "World!", 0
@Num16  DW -32768, -1, 0, 1, 32767, 65535
@Hex16  DEFW $0123, $1234, $2345, $3456, $4567, $5678, $6789, $789A, $89AB, $9ABC, $ABCD
@Labels DW Main, Data, Stack@Main, @Text8
```

- `DB` (Byte Data)
  - A single byte number (`-128` ~ `127`, `0` ~ `255`, `0x00` ~ `0xFF`)
  - Character string (`"String"`)
- `DW` (Word Data)
  - Two bytes number (`-32768` ~ `32767`, `0` ~ `65535`, `0x0000` ~ `0xFFFF`)
  - Character string (`"String"`) *Upper 8 bits are always 0
  - [Labels](#labels)

## Increment and Decrement

Increments and decrements can be automatically inserted before and after a register by adding `++` or `--` before or after the register.

| Source Code | Extract As | Type |
|:-------|:------------|:-----|
|`LD (HL++), A` | `LD (HL), A` <br> `INC HL` | Post Increment |
|`LD (++HL), A` |  `INC HL` <br> `LD (HL), A` | Pre Increment |
|`LD (HL--), A` | `LD (HL), A` <br> `DEC HL` | Post Decrement |
|`LD (--HL), A` |  `DEC HL` <br> `LD (HL), A` | Pre Decrement |

## Support Instructions

- Supports all Z80 instructions, including undocumented.
- Some undocumented instructions are in a slightly special format.
- All instructions are described in [./test/all.asm](./test/all.asm).

## Instructions Specialized for VSS-Zero

### MUL - Multiplication

```z80
    MUL BC      ; BC = B * C
    MUL DE      ; DE = D * C
    MUL HL      ; HL = H * L <faster than BC,DE>
    MUL HL, A   ; HL *= A
    MUL HL, B   ; HL *= B
    MUL HL, C   ; HL *= C <faster than A,B,D,E>
    MUL HL, D   ; HL *= D
    MUL HL, E   ; HL *= E
```

Using `MULS` makes it a signed operation.

```z80
    MULS BC     ; BC = B * C
    MULS DE     ; DE = D * C
    MULS HL     ; HL = H * L <faster than BC,DE>
    MULS HL, A  ; HL *= A
    MULS HL, B  ; HL *= B
    MULS HL, C  ; HL *= C <faster than A,B,D,E>
    MULS HL, D  ; HL *= D
    MULS HL, E  ; HL *= E
```

- The above instructions internally use the [Hardware Calculation](https://github.com/suzukiplan/vgszero/blob/master/README-en.md#hardware-calculation) of VGS-Zero.
- The result of the operation leaves a remainder at 65536, and there is no way to detect its carry.
- No flag bit set

### DIV - Division

```z80
    DIV BC      ; BC = B / C
    DIV DE      ; DE = D / C
    DIV HL      ; HL = H / L <faster than BC,DE>
    DIV HL, A   ; HL /= A
    DIV HL, B   ; HL /= B
    DIV HL, C   ; HL /= C <faster than A,B,D,E>
    DIV HL, D   ; HL /= D
    DIV HL, E   ; HL /= E
```

Using `DIVS` makes it a signed operation.

```z80
    DIVS BC     ; BC = B / C
    DIVS DE     ; DE = D / C
    DIVS HL     ; HL = H / L <faster than BC,DE>
    DIVS HL, A  ; HL /= A
    DIVS HL, B  ; HL /= B
    DIVS HL, C  ; HL /= C <faster than A,B,D,E>
    DIVS HL, D  ; HL /= D
    DIVS HL, E  ; HL /= E
```

- The above instructions internally use the [Hardware Calculation](https://github.com/suzukiplan/vgszero/blob/master/README-en.md#hardware-calculation) of VGS-Zero.
- Zero division results in 65535 (0xFFFF).
- No flag bit set

### MOD - Modulo

```z80
    MOD BC      ; BC = B % C
    MOD DE      ; DE = D % C
    MOD HL      ; HL = H % L <faster than BC,DE>
    MOD HL, A   ; HL %= A
    MOD HL, B   ; HL %= B
    MOD HL, C   ; HL %= C <faster than A,B,D,E>
    MOD HL, D   ; HL %= D
    MOD HL, E   ; HL %= E
```

- The above instructions internally use the [Hardware Calculation](https://github.com/suzukiplan/vgszero/blob/master/README-en.md#hardware-calculation) of VGS-Zero.
- Zero division results in 65535 (0xFFFF).
- No flag bit set

### ATN2 - atan2

```z80
    ATN2 A, BC  ; A = atan2(B,C) ... B=y2-y1, C=x2-x1
    ATN2 A, DE  ; A = atan2(D,E) ... D=y2-y1, E=x2-x1
    ATN2 A, HL  ; A = atan2(H,L) ... H=y2-y1, L=x2-x1 <faster than BC,DE>
```

`ATN2` is an instruction equivalent to the atan2 function in C and FORTRAN, which is specialized for 8-bit coordinate systems. By giving the difference of the Y coordinate (y2-y1) to High and the difference of the X coordinate (x2-x1) to Low in the pair register, the angle values between points 1 (x1,y1) and 2 (x2,y2) can be obtained.

The obtained angle values can then be given to `SIN` and `COS` to obtain the 16-bit fixed-point minority part (8 bits).

### SIN

```z80
    SIN A, A    ; A = sin(A × π ÷ 128.0)
    SIN A, B    ; A = sin(B × π ÷ 128.0)
    SIN A, C    ; A = sin(C × π ÷ 128.0)
    SIN A, D    ; A = sin(D × π ÷ 128.0)
    SIN A, E    ; A = sin(E × π ÷ 128.0)
    SIN A, H    ; A = sin(H × π ÷ 128.0)
    SIN A, L    ; A = sin(L × π ÷ 128.0)
```

- `SIN` for trigonometric functions, but it specializes in finding the value of **X in the direction of travel** (8-bit fractional part of 16-bit fixed minority points) from the angle value obtained by `ATN2`.
- `SIN A,` can be abbreviated to `SIN`.

### COS

```z80
    COS A, A    ; A = cos(A × π ÷ 128.0)
    COS A, B    ; A = cos(B × π ÷ 128.0)
    COS A, C    ; A = cos(C × π ÷ 128.0)
    COS A, D    ; A = cos(D × π ÷ 128.0)
    COS A, E    ; A = cos(E × π ÷ 128.0)
    COS A, H    ; A = cos(H × π ÷ 128.0)
    COS A, L    ; A = cos(L × π ÷ 128.0)
```

- `COS` for trigonometric functions, but it specializes in finding the value of **Y in the direction of travel** (8-bit fractional part of 16-bit fixed minority points) from the angle value obtained by `ATN2`.
- `COS A,` can be abbreviated to `COS`.

## Auto Expand Instructions

In vgsasm, instructions that __do not exist in the Z80__ are complemented by existing instructions.

| Instruction | Auto-expand |
|:------------|:------------|
| `LD RP1, RP2` <br> `*RP = {BC｜DE｜HL｜IX｜IY}` | `LD r1H,r2H`, `LD r1L,r2L`<br>or `PUSH RP2`, `POP RP1`|
| `LD BC,nn` | `LD B,n(high)`, `LD C,n(low)` |
| `LD DE,nn` | `LD D,n(high)`, `LD E,n(low)` |
| `LD HL,nn` | `LD H,n(high)`, `LD L,n(low)` |
| `LD IX,nn` | `LD IXH,n(high)`, `LD IXL,n(low)` |
| `LD IY,nn` | `LD IXH,n(high)`, `LD IXL,n(low)` |
| `LD {IXH｜IXL｜IYH｜IYL},(HL)` | `PUSH AF`, `LD A,(HL)`, `LD {IXH｜IXL｜IYH｜IYL},A`, `POP AF` |
| `LD (HL),{IXH｜IXL｜IYH｜IYL}` | `PUSH AF`, `LD A,{IXH｜IXL｜IYH｜IYL}`, `LD (HL),A`, `POP AF` |
| `LD {BC｜DE},(HL)` | `LD rL,(HL)`, `INC HL`, `LD rH,(HL)`, `DEC HL` |
| `LD {IX｜IY},(HL)` | `PUSH AF`, `LD A,(HL)`, `LD I{X｜Y}L,A`, `INC HL`,<br> `LD A,(HL)`, `LD I{X｜Y}H,A`, `DEC HL`, `POP AF` |
| `ADD HL,nn` | `PUSH DE`, `LD DE,nn`, `ADD HL,DE`, `POP DE`|
| `ADD IX,nn` | `PUSH DE`, `LD DE,nn`, `ADD IX,DE`, `POP DE`|
| `ADD IY,nn` | `PUSH DE`, `LD DE,nn`, `ADD IY,DE`, `POP DE`|
| `ADD BC,A` | `ADD C`, `LD C,A`, `JR NC, +1`, `INC B` |
| `ADD DE,A` | `ADD E`, `LD E,A`, `JR NC, +1`, `INC D` |
| `ADD HL,A` | `ADD L`, `LD L,A`, `JR NC, +1`, `INC H` |
| `ADD (nn)` | `PUSH HL`, `LD L,nL`, `LD H,nH`, `ADD (HL)`, `POP HL` |
| `ADC (nn)` | `PUSH HL`, `LD L,nL`, `LD H,nH`, `ADC (HL)`, `POP HL` |
| `SUB (nn)` | `PUSH HL`, `LD L,nL`, `LD H,nH`, `SUB (HL)`, `POP HL` |
| `SBC (nn)` | `PUSH HL`, `LD L,nL`, `LD H,nH`, `SBC (HL)`, `POP HL` |
| `INC (nn)` | `PUSH HL`, `LD HL,nn`, `INC (HL)` `POP HL`|
| `DEC (nn)` | `PUSH HL`, `LD HL,nn`, `DEC (HL)` `POP HL`|
| `JP (BC)` | `PUSH BC`, `RET` |
| `JP (DE)` | `PUSH DE`, `RET` |

> All VGS-Zero specialized instructions are auto-expand.

## License

[GPLv3](LICENSE.txt)
