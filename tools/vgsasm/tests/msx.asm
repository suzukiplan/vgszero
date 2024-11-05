; vgsasm msx.asm -o msx.rom
org $4000
.Header
@ID     DB "AB"
@Init   DW Start
@State  DW 0
@Device DW 0
@Text   DW 0
@Rsrvd  DB 0, 0, 0, 0, 0, 0

org $4010
.Start
    ; スタックポインタを初期化
    ; 参考: https://qiita.com/suzukiplan/items/9d4b814d53ce96e4ea35
    sp = $F380

    ; VRAM の アクセスアドレス を NameTable の先頭 ($1800) に書き込みモードで設定
    ; ※VDP アドレスポート を書き込む時は di ~ ei で割り込みを禁止
    a++ = ($0007)
    c = a
    di
    a = 0
    out (c), a
    a = $18 + $40
    out (c), a
    ei

    ; 画面をクリア
    call ClearScreen

    ; VRAM の アクセスアドレス を NameTable の $1989 に書き込みモードで設定
    ; (ココから Hello, World! を書き込めばだいたい中心に描画される)
    c++
    di
    a = $89
    out (c), a
    a = $19 + $40
    out (c), a
    ei

    ; NameTable に Hello, World! を書き込む
    c--
    hl = "Hello, World!"
    b = 13
@Loop
    push af
    pop af
    a = (hl++)
    out (c), a
    djnz @Loop

    ; プログラム終了 (無限ループしておく)
    jr @+0

.ClearScreen
    ; 画面を空白で埋める
    c = ($0007)
    a = ' '
    b = $00
    d = $03
@Loop
    push af
    pop af
    out (c), a
    djnz @Loop
    d--
    jr nz, @Loop
    ret

