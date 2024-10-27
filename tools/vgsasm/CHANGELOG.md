# Change log

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
