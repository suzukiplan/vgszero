# Change log

## Version 1.0.0

### Support `sizeof(name.field)`

see the #55

### bugfix: Fixed a bug that the following error occurs when include stdio.asm

```
Error: ../../lib/z80/stdio.asm (81) `SAVE` was not defined at `IO`
Error: ../../lib/z80/stdio.asm (96) `SAVE` was not defined at `IO`
```

## Version 0.15 (beta-15)

- Correct LD error message
- Support `LD {B|C|D|E|H|L|IXH|IXL|IYH|IYL}, (LABEL)`
- Support `LD (LABEL), {A|B|C|D|E|H|L|IXH|IXL|IYH|IYL|BC|DE|HL|SP|IX|IY}`
- Support `LD (LABEL), n`

## Version 0.14 (beta-14)

- Split [vgsasm-extension](https://github.com/suzukiplan/vgsasm-extension) repository.

## Version 0.13 (beta-13)

- Added vscode extension (WIP)

## Version 0.12 (beta-12)

- Fixed a bug at the beginning of file parsing that caused errors in lines that failed to parse to be ignored.
- Correct error message of `DW`
- Correct error message of `EX`

## Version 0.11 (beta-11)

- `#macro` must appear at the beginning of the line.
- Fixed an error message when the start scope of a `#macro` was not defined at the expected position.
- Fixed message when `d` in `IX-d` is out of range error.
- Fixed a bug that `ADD (HL+3)` etc. were assembled as `ADD (HL)` without error.
- Fixed a bug that caused `SUB {BC|DE|HL}, A` to incorrectly become `ADD {BC|DE|HL},A`.

## Version 0.10 (beta-10)

- Support unspecified address structure
- Forbidden to include `. ` in the structure and the field name are prohibited.
- bugfix: Crash sometimes occurs when defining a malformed `struct`
- bugfix: Crash if `}` is specified as the last line without a corresponding `{`.
- numeric error message detailed
- `offset`: make error if not contained `.` at the name
- `offset`: Just in case a field name contains a dot
- `org`: make error judgment strict
- `sizeof`: correct error message
- `struct`: correct error message

## Version 0.9 (beta-9)

- Stricter naming of prohibited items
- Detail the error when a label name containing `@` is undefined
- Several minor bugfix

## Version 0.8 (beta-8)

- unit test and refactor
- Defining an enum results in a `Unexpected symbol:` error with no symbol
- Crash during operations involving nested brackets #42
- Structure expansion result address is different from expected #43

## Version 0.7 (beta-7)

- Unit test on the Circle CI
- Improved error message output when a name with no definition on the right side of the dot is specified in `#define`.
- Fixed a case where `enum` did not produce the expected error.

## Version 0.6 (beta-6)

- Unexpected struct error: "It must be defined on the line after the definition of struct specified by the arrow operator." #38
- Crash when address is specified as argument in #macro call #39
- Several minor bugfix (unit test)

## Version 0.5 (beta-5)

- Support mul, div, mod with immediate #34
- bugfix `#binary`

## Version 0.4 (beta-4)

- Simplified notation of subroutine calls #27
- unexpected Illegal expression error #28
- Address struct after a specific struct #30
- Support `LD (nn), r` and `LD r,(nn)` #31
- change `mul`, `div`, `mod` expression #32
- support increment/decrement only expression #33
- support `-v` option

## Version 0.3 (beta-3)

- Allow the number of shifts to be specified for `SLA` and `SRL` #23
- Support `HL += (IX+d|IY+d)` #25
- Support `enum` #26
- Build speed optimized

## Version 0.2 (beta-2)

- LD (HL), n does not work #18
- set register pair to the indexed address #20
- merge source code #22

## Version 0.1 (beta-1)

- first release
