                    im        0                             ;[0000] ed 46
                    im        1                             ;[0002] ed 56
                    im        2                             ;[0004] ed 5e
                    di                                      ;[0006] f3
                    ei                                      ;[0007] fb
                    halt                                    ;[0008] 76
                    push      af                            ;[0009] f5
                    push      bc                            ;[000a] c5
                    push      de                            ;[000b] d5
                    push      hl                            ;[000c] e5
                    push      ix                            ;[000d] dd e5
                    push      iy                            ;[000f] fd e5
                    pop       af                            ;[0011] f1
                    pop       bc                            ;[0012] c1
                    pop       de                            ;[0013] d1
                    pop       hl                            ;[0014] e1
                    pop       ix                            ;[0015] dd e1
                    pop       iy                            ;[0017] fd e1
                    ex        de,hl                         ;[0019] eb
                    ex        af,af'                        ;[001a] 08
                    ex        (sp),hl                       ;[001b] e3
                    ex        (sp),ix                       ;[001c] dd e3
                    ex        (sp),iy                       ;[001e] fd e3
                    exx                                     ;[0020] d9
                    ldi                                     ;[0021] ed a0
                    ldir                                    ;[0023] ed b0
                    ldd                                     ;[0025] ed a8
                    lddr                                    ;[0027] ed b8
                    cpi                                     ;[0029] ed a1
                    cpir                                    ;[002b] ed b1
                    cpd                                     ;[002d] ed a9
                    cpdr                                    ;[002f] ed b9
                    outi                                    ;[0031] ed a3
                    otir                                    ;[0033] ed b3
                    otir                                    ;[0035] ed b3
                    outd                                    ;[0037] ed ab
                    otdr                                    ;[0039] ed bb
                    otdr                                    ;[003b] ed bb
                    nop                                     ;[003d] 00
                    daa                                     ;[003e] 27
                    cpl                                     ;[003f] 2f
                    ccf                                     ;[0040] 3f
                    scf                                     ;[0041] 37
                    neg                                     ;[0042] ed 44
                    and       a                             ;[0044] a7
                    and       b                             ;[0045] a0
                    and       c                             ;[0046] a1
                    and       d                             ;[0047] a2
                    and       e                             ;[0048] a3
                    and       h                             ;[0049] a4
                    and       l                             ;[004a] a5
                    and       ixh                           ;[004b] dd a4
                    and       ixl                           ;[004d] dd a5
                    and       iyh                           ;[004f] fd a4
                    and       iyl                           ;[0051] fd a5
                    and       $ff                           ;[0053] e6 ff
                    and       (hl)                          ;[0055] a6
                    and       (ix+$00)                      ;[0056] dd a6 00
                    and       (ix-$80)                      ;[0059] dd a6 80
                    and       (ix+$7f)                      ;[005c] dd a6 7f
                    and       (iy+$00)                      ;[005f] fd a6 00
                    and       (iy-$80)                      ;[0062] fd a6 80
                    and       (iy+$7f)                      ;[0065] fd a6 7f
                    and       a                             ;[0068] a7
                    and       b                             ;[0069] a0
                    and       c                             ;[006a] a1
                    and       d                             ;[006b] a2
                    and       e                             ;[006c] a3
                    and       h                             ;[006d] a4
                    and       l                             ;[006e] a5
                    and       ixh                           ;[006f] dd a4
                    and       ixl                           ;[0071] dd a5
                    and       iyh                           ;[0073] fd a4
                    and       iyl                           ;[0075] fd a5
                    and       $ff                           ;[0077] e6 ff
                    and       (hl)                          ;[0079] a6
                    and       (ix+$00)                      ;[007a] dd a6 00
                    and       (ix-$80)                      ;[007d] dd a6 80
                    and       (ix+$7f)                      ;[0080] dd a6 7f
                    and       (iy+$00)                      ;[0083] fd a6 00
                    and       (iy-$80)                      ;[0086] fd a6 80
                    and       (iy+$7f)                      ;[0089] fd a6 7f
                    or        a                             ;[008c] b7
                    or        b                             ;[008d] b0
                    or        c                             ;[008e] b1
                    or        d                             ;[008f] b2
                    or        e                             ;[0090] b3
                    or        h                             ;[0091] b4
                    or        l                             ;[0092] b5
                    or        ixh                           ;[0093] dd b4
                    or        ixl                           ;[0095] dd b5
                    or        iyh                           ;[0097] fd b4
                    or        iyl                           ;[0099] fd b5
                    or        $ff                           ;[009b] f6 ff
                    or        (hl)                          ;[009d] b6
                    or        (ix+$00)                      ;[009e] dd b6 00
                    or        (ix-$80)                      ;[00a1] dd b6 80
                    or        (ix+$7f)                      ;[00a4] dd b6 7f
                    or        (iy+$00)                      ;[00a7] fd b6 00
                    or        (iy-$80)                      ;[00aa] fd b6 80
                    or        (iy+$7f)                      ;[00ad] fd b6 7f
                    or        a                             ;[00b0] b7
                    or        b                             ;[00b1] b0
                    or        c                             ;[00b2] b1
                    or        d                             ;[00b3] b2
                    or        e                             ;[00b4] b3
                    or        h                             ;[00b5] b4
                    or        l                             ;[00b6] b5
                    or        ixh                           ;[00b7] dd b4
                    or        ixl                           ;[00b9] dd b5
                    or        iyh                           ;[00bb] fd b4
                    or        iyl                           ;[00bd] fd b5
                    or        $ff                           ;[00bf] f6 ff
                    or        (hl)                          ;[00c1] b6
                    or        (ix+$00)                      ;[00c2] dd b6 00
                    or        (ix-$80)                      ;[00c5] dd b6 80
                    or        (ix+$7f)                      ;[00c8] dd b6 7f
                    or        (iy+$00)                      ;[00cb] fd b6 00
                    or        (iy-$80)                      ;[00ce] fd b6 80
                    or        (iy+$7f)                      ;[00d1] fd b6 7f
                    xor       a                             ;[00d4] af
                    xor       b                             ;[00d5] a8
                    xor       c                             ;[00d6] a9
                    xor       d                             ;[00d7] aa
                    xor       e                             ;[00d8] ab
                    xor       h                             ;[00d9] ac
                    xor       l                             ;[00da] ad
                    xor       ixh                           ;[00db] dd ac
                    xor       ixl                           ;[00dd] dd ad
                    xor       iyh                           ;[00df] fd ac
                    xor       iyl                           ;[00e1] fd ad
                    xor       $ff                           ;[00e3] ee ff
                    xor       (hl)                          ;[00e5] ae
                    xor       (ix+$00)                      ;[00e6] dd ae 00
                    xor       (ix-$80)                      ;[00e9] dd ae 80
                    xor       (ix+$7f)                      ;[00ec] dd ae 7f
                    xor       (iy+$00)                      ;[00ef] fd ae 00
                    xor       (iy-$80)                      ;[00f2] fd ae 80
                    xor       (iy+$7f)                      ;[00f5] fd ae 7f
                    xor       a                             ;[00f8] af
                    xor       b                             ;[00f9] a8
                    xor       c                             ;[00fa] a9
                    xor       d                             ;[00fb] aa
                    xor       e                             ;[00fc] ab
                    xor       h                             ;[00fd] ac
                    xor       l                             ;[00fe] ad
                    xor       ixh                           ;[00ff] dd ac
                    xor       ixl                           ;[0101] dd ad
                    xor       iyh                           ;[0103] fd ac
                    xor       iyl                           ;[0105] fd ad
                    xor       $ff                           ;[0107] ee ff
                    xor       (hl)                          ;[0109] ae
                    xor       (ix+$00)                      ;[010a] dd ae 00
                    xor       (ix-$80)                      ;[010d] dd ae 80
                    xor       (ix+$7f)                      ;[0110] dd ae 7f
                    xor       (iy+$00)                      ;[0113] fd ae 00
                    xor       (iy-$80)                      ;[0116] fd ae 80
                    xor       (iy+$7f)                      ;[0119] fd ae 7f
                    cp        a                             ;[011c] bf
                    cp        b                             ;[011d] b8
                    cp        c                             ;[011e] b9
                    cp        d                             ;[011f] ba
                    cp        e                             ;[0120] bb
                    cp        h                             ;[0121] bc
                    cp        l                             ;[0122] bd
                    cp        ixh                           ;[0123] dd bc
                    cp        ixl                           ;[0125] dd bd
                    cp        iyh                           ;[0127] fd bc
                    cp        iyl                           ;[0129] fd bd
                    cp        $ff                           ;[012b] fe ff
                    cp        (hl)                          ;[012d] be
                    cp        (ix+$00)                      ;[012e] dd be 00
                    cp        (ix-$80)                      ;[0131] dd be 80
                    cp        (ix+$7f)                      ;[0134] dd be 7f
                    cp        (iy+$00)                      ;[0137] fd be 00
                    cp        (iy-$80)                      ;[013a] fd be 80
                    cp        (iy+$7f)                      ;[013d] fd be 7f
                    cp        a                             ;[0140] bf
                    cp        b                             ;[0141] b8
                    cp        c                             ;[0142] b9
                    cp        d                             ;[0143] ba
                    cp        e                             ;[0144] bb
                    cp        h                             ;[0145] bc
                    cp        l                             ;[0146] bd
                    cp        ixh                           ;[0147] dd bc
                    cp        ixl                           ;[0149] dd bd
                    cp        iyh                           ;[014b] fd bc
                    cp        iyl                           ;[014d] fd bd
                    cp        $ff                           ;[014f] fe ff
                    cp        (hl)                          ;[0151] be
                    cp        (ix+$00)                      ;[0152] dd be 00
                    cp        (ix-$80)                      ;[0155] dd be 80
                    cp        (ix+$7f)                      ;[0158] dd be 7f
                    cp        (iy+$00)                      ;[015b] fd be 00
                    cp        (iy-$80)                      ;[015e] fd be 80
                    cp        (iy+$7f)                      ;[0161] fd be 7f
                    add       a                             ;[0164] 87
                    add       b                             ;[0165] 80
                    add       c                             ;[0166] 81
                    add       d                             ;[0167] 82
                    add       e                             ;[0168] 83
                    add       h                             ;[0169] 84
                    add       l                             ;[016a] 85
                    add       ixh                           ;[016b] dd 84
                    add       ixl                           ;[016d] dd 85
                    add       iyh                           ;[016f] fd 84
                    add       iyl                           ;[0171] fd 85
                    add       $ff                           ;[0173] c6 ff
                    add       (hl)                          ;[0175] 86
                    add       (ix+$00)                      ;[0176] dd 86 00
                    add       (ix-$80)                      ;[0179] dd 86 80
                    add       (ix+$7f)                      ;[017c] dd 86 7f
                    add       (iy+$00)                      ;[017f] fd 86 00
                    add       (iy-$80)                      ;[0182] fd 86 80
                    add       (iy+$7f)                      ;[0185] fd 86 7f
                    add       a                             ;[0188] 87
                    add       b                             ;[0189] 80
                    add       c                             ;[018a] 81
                    add       d                             ;[018b] 82
                    add       e                             ;[018c] 83
                    add       h                             ;[018d] 84
                    add       l                             ;[018e] 85
                    add       ixh                           ;[018f] dd 84
                    add       ixl                           ;[0191] dd 85
                    add       iyh                           ;[0193] fd 84
                    add       iyl                           ;[0195] fd 85
                    add       $ff                           ;[0197] c6 ff
                    add       (hl)                          ;[0199] 86
                    add       (ix+$00)                      ;[019a] dd 86 00
                    add       (ix-$80)                      ;[019d] dd 86 80
                    add       (ix+$7f)                      ;[01a0] dd 86 7f
                    add       (iy+$00)                      ;[01a3] fd 86 00
                    add       (iy-$80)                      ;[01a6] fd 86 80
                    add       (iy+$7f)                      ;[01a9] fd 86 7f
                    push      hl                            ;[01ac] e5
                    ld        l,$34                         ;[01ad] 2e 34
                    ld        h,$12                         ;[01af] 26 12
                    add       (hl)                          ;[01b1] 86
                    pop       hl                            ;[01b2] e1
                    add       hl,bc                         ;[01b3] 09
                    add       hl,de                         ;[01b4] 19
                    add       hl,hl                         ;[01b5] 29
                    add       hl,sp                         ;[01b6] 39
                    add       ix,bc                         ;[01b7] dd 09
                    add       ix,de                         ;[01b9] dd 19
                    add       ix,ix                         ;[01bb] dd 29
                    add       ix,sp                         ;[01bd] dd 39
                    add       iy,bc                         ;[01bf] fd 09
                    add       iy,de                         ;[01c1] fd 19
                    add       iy,iy                         ;[01c3] fd 29
                    add       iy,sp                         ;[01c5] fd 39
                    add       c                             ;[01c7] 81
                    ld        c,a                           ;[01c8] 4f
                    jr        nc,$01cc                      ;[01c9] 30 01
                    inc       b                             ;[01cb] 04
                    add       e                             ;[01cc] 83
                    ld        e,a                           ;[01cd] 5f
                    jr        nc,$01d1                      ;[01ce] 30 01
                    inc       d                             ;[01d0] 14
                    add       l                             ;[01d1] 85
                    ld        l,a                           ;[01d2] 6f
                    jr        nc,$01d6                      ;[01d3] 30 01
                    inc       h                             ;[01d5] 24
                    push      de                            ;[01d6] d5
                    ld        d,$12                         ;[01d7] 16 12
                    ld        e,$34                         ;[01d9] 1e 34
                    add       hl,de                         ;[01db] 19
                    pop       de                            ;[01dc] d1
                    push      de                            ;[01dd] d5
                    ld        d,$56                         ;[01de] 16 56
                    ld        e,$78                         ;[01e0] 1e 78
                    add       ix,de                         ;[01e2] dd 19
                    pop       de                            ;[01e4] d1
                    push      de                            ;[01e5] d5
                    ld        d,$9a                         ;[01e6] 16 9a
                    ld        e,$bc                         ;[01e8] 1e bc
                    add       iy,de                         ;[01ea] fd 19
                    pop       de                            ;[01ec] d1
                    adc       a                             ;[01ed] 8f
                    adc       b                             ;[01ee] 88
                    adc       c                             ;[01ef] 89
                    adc       d                             ;[01f0] 8a
                    adc       e                             ;[01f1] 8b
                    adc       h                             ;[01f2] 8c
                    adc       l                             ;[01f3] 8d
                    adc       ixh                           ;[01f4] dd 8c
                    adc       ixl                           ;[01f6] dd 8d
                    adc       iyh                           ;[01f8] fd 8c
                    adc       iyl                           ;[01fa] fd 8d
                    adc       $ff                           ;[01fc] ce ff
                    adc       (hl)                          ;[01fe] 8e
                    adc       (ix+$00)                      ;[01ff] dd 8e 00
                    adc       (ix-$80)                      ;[0202] dd 8e 80
                    adc       (ix+$7f)                      ;[0205] dd 8e 7f
                    adc       (iy+$00)                      ;[0208] fd 8e 00
                    adc       (iy-$80)                      ;[020b] fd 8e 80
                    adc       (iy+$7f)                      ;[020e] fd 8e 7f
                    adc       a                             ;[0211] 8f
                    adc       b                             ;[0212] 88
                    adc       c                             ;[0213] 89
                    adc       d                             ;[0214] 8a
                    adc       e                             ;[0215] 8b
                    adc       h                             ;[0216] 8c
                    adc       l                             ;[0217] 8d
                    adc       ixh                           ;[0218] dd 8c
                    adc       ixl                           ;[021a] dd 8d
                    adc       iyh                           ;[021c] fd 8c
                    adc       iyl                           ;[021e] fd 8d
                    adc       $ff                           ;[0220] ce ff
                    adc       (hl)                          ;[0222] 8e
                    adc       (ix+$00)                      ;[0223] dd 8e 00
                    adc       (ix-$80)                      ;[0226] dd 8e 80
                    adc       (ix+$7f)                      ;[0229] dd 8e 7f
                    adc       (iy+$00)                      ;[022c] fd 8e 00
                    adc       (iy-$80)                      ;[022f] fd 8e 80
                    adc       (iy+$7f)                      ;[0232] fd 8e 7f
                    push      hl                            ;[0235] e5
                    ld        l,$34                         ;[0236] 2e 34
                    ld        h,$12                         ;[0238] 26 12
                    adc       (hl)                          ;[023a] 8e
                    pop       hl                            ;[023b] e1
                    adc       hl,bc                         ;[023c] ed 4a
                    adc       hl,de                         ;[023e] ed 5a
                    adc       hl,hl                         ;[0240] ed 6a
                    adc       hl,sp                         ;[0242] ed 7a
                    sub       a                             ;[0244] 97
                    sub       b                             ;[0245] 90
                    sub       c                             ;[0246] 91
                    sub       d                             ;[0247] 92
                    sub       e                             ;[0248] 93
                    sub       h                             ;[0249] 94
                    sub       l                             ;[024a] 95
                    sub       ixh                           ;[024b] dd 94
                    sub       ixl                           ;[024d] dd 95
                    sub       iyh                           ;[024f] fd 94
                    sub       iyl                           ;[0251] fd 95
                    sub       $ff                           ;[0253] d6 ff
                    sub       (hl)                          ;[0255] 96
                    sub       (ix+$00)                      ;[0256] dd 96 00
                    sub       (ix-$80)                      ;[0259] dd 96 80
                    sub       (ix+$7f)                      ;[025c] dd 96 7f
                    sub       (iy+$00)                      ;[025f] fd 96 00
                    sub       (iy-$80)                      ;[0262] fd 96 80
                    sub       (iy+$7f)                      ;[0265] fd 96 7f
                    push      hl                            ;[0268] e5
                    ld        l,$34                         ;[0269] 2e 34
                    ld        h,$12                         ;[026b] 26 12
                    sub       (hl)                          ;[026d] 96
                    pop       hl                            ;[026e] e1
                    sub       a                             ;[026f] 97
                    sub       b                             ;[0270] 90
                    sub       c                             ;[0271] 91
                    sub       d                             ;[0272] 92
                    sub       e                             ;[0273] 93
                    sub       h                             ;[0274] 94
                    sub       l                             ;[0275] 95
                    sub       ixh                           ;[0276] dd 94
                    sub       ixl                           ;[0278] dd 95
                    sub       iyh                           ;[027a] fd 94
                    sub       iyl                           ;[027c] fd 95
                    sub       $ff                           ;[027e] d6 ff
                    sub       (hl)                          ;[0280] 96
                    sub       (ix+$00)                      ;[0281] dd 96 00
                    sub       (ix-$80)                      ;[0284] dd 96 80
                    sub       (ix+$7f)                      ;[0287] dd 96 7f
                    sub       (iy+$00)                      ;[028a] fd 96 00
                    sub       (iy-$80)                      ;[028d] fd 96 80
                    sub       (iy+$7f)                      ;[0290] fd 96 7f
                    sbc       a                             ;[0293] 9f
                    sbc       b                             ;[0294] 98
                    sbc       c                             ;[0295] 99
                    sbc       d                             ;[0296] 9a
                    sbc       e                             ;[0297] 9b
                    sbc       h                             ;[0298] 9c
                    sbc       l                             ;[0299] 9d
                    sbc       ixh                           ;[029a] dd 9c
                    sbc       ixl                           ;[029c] dd 9d
                    sbc       iyh                           ;[029e] fd 9c
                    sbc       iyl                           ;[02a0] fd 9d
                    sbc       $ff                           ;[02a2] de ff
                    sbc       (hl)                          ;[02a4] 9e
                    sbc       (ix+$00)                      ;[02a5] dd 9e 00
                    sbc       (ix-$80)                      ;[02a8] dd 9e 80
                    sbc       (ix+$7f)                      ;[02ab] dd 9e 7f
                    sbc       (iy+$00)                      ;[02ae] fd 9e 00
                    sbc       (iy-$80)                      ;[02b1] fd 9e 80
                    sbc       (iy+$7f)                      ;[02b4] fd 9e 7f
                    sbc       a                             ;[02b7] 9f
                    sbc       b                             ;[02b8] 98
                    sbc       c                             ;[02b9] 99
                    sbc       d                             ;[02ba] 9a
                    sbc       e                             ;[02bb] 9b
                    sbc       h                             ;[02bc] 9c
                    sbc       l                             ;[02bd] 9d
                    sbc       ixh                           ;[02be] dd 9c
                    sbc       ixl                           ;[02c0] dd 9d
                    sbc       iyh                           ;[02c2] fd 9c
                    sbc       iyl                           ;[02c4] fd 9d
                    sbc       $ff                           ;[02c6] de ff
                    sbc       (hl)                          ;[02c8] 9e
                    sbc       (ix+$00)                      ;[02c9] dd 9e 00
                    sbc       (ix-$80)                      ;[02cc] dd 9e 80
                    sbc       (ix+$7f)                      ;[02cf] dd 9e 7f
                    sbc       (iy+$00)                      ;[02d2] fd 9e 00
                    sbc       (iy-$80)                      ;[02d5] fd 9e 80
                    sbc       (iy+$7f)                      ;[02d8] fd 9e 7f
                    push      hl                            ;[02db] e5
                    ld        l,$34                         ;[02dc] 2e 34
                    ld        h,$12                         ;[02de] 26 12
                    sbc       (hl)                          ;[02e0] 9e
                    pop       hl                            ;[02e1] e1
                    sbc       hl,bc                         ;[02e2] ed 42
                    sbc       hl,de                         ;[02e4] ed 52
                    sbc       hl,hl                         ;[02e6] ed 62
                    sbc       hl,sp                         ;[02e8] ed 72
                    bit       0,a                           ;[02ea] cb 47
                    bit       1,b                           ;[02ec] cb 48
                    bit       2,c                           ;[02ee] cb 51
                    bit       3,d                           ;[02f0] cb 5a
                    bit       4,e                           ;[02f2] cb 63
                    bit       5,h                           ;[02f4] cb 6c
                    bit       6,l                           ;[02f6] cb 75
                    set       7,a                           ;[02f8] cb ff
                    set       0,b                           ;[02fa] cb c0
                    set       1,c                           ;[02fc] cb c9
                    set       2,d                           ;[02fe] cb d2
                    set       3,e                           ;[0300] cb db
                    set       4,h                           ;[0302] cb e4
                    set       5,l                           ;[0304] cb ed
                    res       6,a                           ;[0306] cb b7
                    res       7,b                           ;[0308] cb b8
                    res       0,c                           ;[030a] cb 81
                    res       1,d                           ;[030c] cb 8a
                    res       2,e                           ;[030e] cb 93
                    res       3,h                           ;[0310] cb 9c
                    res       4,l                           ;[0312] cb a5
                    bit       0,(hl)                        ;[0314] cb 46
                    set       1,(hl)                        ;[0316] cb ce
                    res       2,(hl)                        ;[0318] cb 96
                    bit       0,(ix+$00)                    ;[031a] dd cb 00 46
                    set       1,(ix+$00)                    ;[031e] dd cb 00 ce
                    res       2,(ix+$00)                    ;[0322] dd cb 00 96
                    bit       0,(iy+$00)                    ;[0326] fd cb 00 46
                    set       1,(iy+$00)                    ;[032a] fd cb 00 ce
                    res       2,(iy+$00)                    ;[032e] fd cb 00 96
                    bit       0,(ix+$7f)                    ;[0332] dd cb 7f 46
                    set       1,(ix+$00)                    ;[0336] dd cb 00 ce
                    res       2,(ix-$80)                    ;[033a] dd cb 80 96
                    bit       0,(iy-$01)                    ;[033e] fd cb ff 46
                    set       1,(iy+$40)                    ;[0342] fd cb 40 ce
                    res       2,(iy-$40)                    ;[0346] fd cb c0 96
                    inc       a                             ;[034a] 3c
                    inc       b                             ;[034b] 04
                    inc       c                             ;[034c] 0c
                    inc       d                             ;[034d] 14
                    inc       e                             ;[034e] 1c
                    inc       h                             ;[034f] 24
                    inc       l                             ;[0350] 2c
                    inc       ixh                           ;[0351] dd 24
                    inc       ixl                           ;[0353] dd 2c
                    inc       iyh                           ;[0355] fd 24
                    inc       iyl                           ;[0357] fd 2c
                    inc       bc                            ;[0359] 03
                    inc       de                            ;[035a] 13
                    inc       hl                            ;[035b] 23
                    inc       sp                            ;[035c] 33
                    inc       ix                            ;[035d] dd 23
                    inc       iy                            ;[035f] fd 23
                    inc       (hl)                          ;[0361] 34
                    inc       (ix+$00)                      ;[0362] dd 34 00
                    inc       (iy+$00)                      ;[0365] fd 34 00
                    inc       (ix-$80)                      ;[0368] dd 34 80
                    inc       (iy-$01)                      ;[036b] fd 34 ff
                    inc       (ix+$01)                      ;[036e] dd 34 01
                    inc       (iy+$7f)                      ;[0371] fd 34 7f
                    push      hl                            ;[0374] e5
                    ld        l,$34                         ;[0375] 2e 34
                    ld        h,$12                         ;[0377] 26 12
                    inc       (hl)                          ;[0379] 34
                    pop       hl                            ;[037a] e1
                    dec       a                             ;[037b] 3d
                    dec       b                             ;[037c] 05
                    dec       c                             ;[037d] 0d
                    dec       d                             ;[037e] 15
                    dec       e                             ;[037f] 1d
                    dec       h                             ;[0380] 25
                    dec       l                             ;[0381] 2d
                    dec       ixh                           ;[0382] dd 25
                    dec       ixl                           ;[0384] dd 2d
                    dec       iyh                           ;[0386] fd 25
                    dec       iyl                           ;[0388] fd 2d
                    dec       bc                            ;[038a] 0b
                    dec       de                            ;[038b] 1b
                    dec       hl                            ;[038c] 2b
                    dec       sp                            ;[038d] 3b
                    dec       ix                            ;[038e] dd 2b
                    dec       iy                            ;[0390] fd 2b
                    dec       (hl)                          ;[0392] 35
                    dec       (ix+$00)                      ;[0393] dd 35 00
                    dec       (iy+$00)                      ;[0396] fd 35 00
                    dec       (ix-$80)                      ;[0399] dd 35 80
                    dec       (iy-$01)                      ;[039c] fd 35 ff
                    dec       (ix+$01)                      ;[039f] dd 35 01
                    dec       (iy+$7f)                      ;[03a2] fd 35 7f
                    push      hl                            ;[03a5] e5
                    ld        l,$34                         ;[03a6] 2e 34
                    ld        h,$12                         ;[03a8] 26 12
                    dec       (hl)                          ;[03aa] 35
                    pop       hl                            ;[03ab] e1
                    rlca                                    ;[03ac] 07
                    rla                                     ;[03ad] 17
                    rrca                                    ;[03ae] 0f
                    rra                                     ;[03af] 1f
                    rlc       a                             ;[03b0] cb 07
                    rlc       b                             ;[03b2] cb 00
                    rlc       c                             ;[03b4] cb 01
                    rlc       d                             ;[03b6] cb 02
                    rlc       e                             ;[03b8] cb 03
                    rlc       h                             ;[03ba] cb 04
                    rlc       l                             ;[03bc] cb 05
                    rlc       (hl)                          ;[03be] cb 06
                    rlc       (ix+$00)                      ;[03c0] dd cb 00 06
                    rlc       (iy+$00)                      ;[03c4] fd cb 00 06
                    rlc       (ix-$80)                      ;[03c8] dd cb 80 06
                    rlc       (iy-$01)                      ;[03cc] fd cb ff 06
                    rlc       (ix+$01)                      ;[03d0] dd cb 01 06
                    rlc       (iy+$7f)                      ;[03d4] fd cb 7f 06
                    rl        a                             ;[03d8] cb 17
                    rl        b                             ;[03da] cb 10
                    rl        c                             ;[03dc] cb 11
                    rl        d                             ;[03de] cb 12
                    rl        e                             ;[03e0] cb 13
                    rl        h                             ;[03e2] cb 14
                    rl        l                             ;[03e4] cb 15
                    rl        (hl)                          ;[03e6] cb 16
                    rl        (ix+$00)                      ;[03e8] dd cb 00 16
                    rl        (iy+$00)                      ;[03ec] fd cb 00 16
                    rl        (ix-$80)                      ;[03f0] dd cb 80 16
                    rl        (iy-$01)                      ;[03f4] fd cb ff 16
                    rl        (ix+$01)                      ;[03f8] dd cb 01 16
                    rl        (iy+$7f)                      ;[03fc] fd cb 7f 16
                    rrc       a                             ;[0400] cb 0f
                    rrc       b                             ;[0402] cb 08
                    rrc       c                             ;[0404] cb 09
                    rrc       d                             ;[0406] cb 0a
                    rrc       e                             ;[0408] cb 0b
                    rrc       h                             ;[040a] cb 0c
                    rrc       l                             ;[040c] cb 0d
                    rrc       (hl)                          ;[040e] cb 0e
                    rrc       (ix+$00)                      ;[0410] dd cb 00 0e
                    rrc       (iy+$00)                      ;[0414] fd cb 00 0e
                    rrc       (ix-$80)                      ;[0418] dd cb 80 0e
                    rrc       (iy-$01)                      ;[041c] fd cb ff 0e
                    rrc       (ix+$01)                      ;[0420] dd cb 01 0e
                    rrc       (iy+$7f)                      ;[0424] fd cb 7f 0e
                    rr        a                             ;[0428] cb 1f
                    rr        b                             ;[042a] cb 18
                    rr        c                             ;[042c] cb 19
                    rr        d                             ;[042e] cb 1a
                    rr        e                             ;[0430] cb 1b
                    rr        h                             ;[0432] cb 1c
                    rr        l                             ;[0434] cb 1d
                    rr        (hl)                          ;[0436] cb 1e
                    rr        (ix+$00)                      ;[0438] dd cb 00 1e
                    rr        (iy+$00)                      ;[043c] fd cb 00 1e
                    rr        (ix-$80)                      ;[0440] dd cb 80 1e
                    rr        (iy-$01)                      ;[0444] fd cb ff 1e
                    rr        (ix+$01)                      ;[0448] dd cb 01 1e
                    rr        (iy+$7f)                      ;[044c] fd cb 7f 1e
                    sla       a                             ;[0450] cb 27
                    sla       b                             ;[0452] cb 20
                    sla       c                             ;[0454] cb 21
                    sla       d                             ;[0456] cb 22
                    sla       e                             ;[0458] cb 23
                    sla       h                             ;[045a] cb 24
                    sla       l                             ;[045c] cb 25
                    sla       (hl)                          ;[045e] cb 26
                    sla       (ix+$00)                      ;[0460] dd cb 00 26
                    sla       (iy+$00)                      ;[0464] fd cb 00 26
                    sla       (ix-$80)                      ;[0468] dd cb 80 26
                    sla       (iy-$01)                      ;[046c] fd cb ff 26
                    sla       (ix+$01)                      ;[0470] dd cb 01 26
                    sla       (iy+$7f)                      ;[0474] fd cb 7f 26
                    sla       a                             ;[0478] cb 27
                    sla       b                             ;[047a] cb 20
                    sla       c                             ;[047c] cb 21
                    sla       d                             ;[047e] cb 22
                    sla       e                             ;[0480] cb 23
                    sla       h                             ;[0482] cb 24
                    sla       l                             ;[0484] cb 25
                    sla       (hl)                          ;[0486] cb 26
                    sla       (ix+$00)                      ;[0488] dd cb 00 26
                    sla       (iy+$00)                      ;[048c] fd cb 00 26
                    sla       (ix-$80)                      ;[0490] dd cb 80 26
                    sla       (iy-$01)                      ;[0494] fd cb ff 26
                    sla       (ix+$01)                      ;[0498] dd cb 01 26
                    sla       (iy+$7f)                      ;[049c] fd cb 7f 26
                    sll       a                             ;[04a0] cb 37
                    sll       b                             ;[04a2] cb 30
                    sll       c                             ;[04a4] cb 31
                    sll       d                             ;[04a6] cb 32
                    sll       e                             ;[04a8] cb 33
                    sll       h                             ;[04aa] cb 34
                    sll       l                             ;[04ac] cb 35
                    sll       (hl)                          ;[04ae] cb 36
                    sll       (ix+$00)                      ;[04b0] dd cb 00 36
                    sll       (iy+$00)                      ;[04b4] fd cb 00 36
                    sll       (ix-$80)                      ;[04b8] dd cb 80 36
                    sll       (iy-$01)                      ;[04bc] fd cb ff 36
                    sll       (ix+$01)                      ;[04c0] dd cb 01 36
                    sll       (iy+$7f)                      ;[04c4] fd cb 7f 36
                    sra       a                             ;[04c8] cb 2f
                    sra       b                             ;[04ca] cb 28
                    sra       c                             ;[04cc] cb 29
                    sra       d                             ;[04ce] cb 2a
                    sra       e                             ;[04d0] cb 2b
                    sra       h                             ;[04d2] cb 2c
                    sra       l                             ;[04d4] cb 2d
                    sra       (hl)                          ;[04d6] cb 2e
                    sra       (ix+$00)                      ;[04d8] dd cb 00 2e
                    sra       (iy+$00)                      ;[04dc] fd cb 00 2e
                    sra       (ix-$80)                      ;[04e0] dd cb 80 2e
                    sra       (iy-$01)                      ;[04e4] fd cb ff 2e
                    sra       (ix+$01)                      ;[04e8] dd cb 01 2e
                    sra       (iy+$7f)                      ;[04ec] fd cb 7f 2e
                    srl       a                             ;[04f0] cb 3f
                    srl       b                             ;[04f2] cb 38
                    srl       c                             ;[04f4] cb 39
                    srl       d                             ;[04f6] cb 3a
                    srl       e                             ;[04f8] cb 3b
                    srl       h                             ;[04fa] cb 3c
                    srl       l                             ;[04fc] cb 3d
                    srl       (hl)                          ;[04fe] cb 3e
                    srl       (ix+$00)                      ;[0500] dd cb 00 3e
                    srl       (iy+$00)                      ;[0504] fd cb 00 3e
                    srl       (ix-$80)                      ;[0508] dd cb 80 3e
                    srl       (iy-$01)                      ;[050c] fd cb ff 3e
                    srl       (ix+$01)                      ;[0510] dd cb 01 3e
                    srl       (iy+$7f)                      ;[0514] fd cb 7f 3e
                    srl       a                             ;[0518] cb 3f
                    srl       b                             ;[051a] cb 38
                    srl       c                             ;[051c] cb 39
                    srl       d                             ;[051e] cb 3a
                    srl       e                             ;[0520] cb 3b
                    srl       h                             ;[0522] cb 3c
                    srl       l                             ;[0524] cb 3d
                    srl       (hl)                          ;[0526] cb 3e
                    srl       (ix+$00)                      ;[0528] dd cb 00 3e
                    srl       (iy+$00)                      ;[052c] fd cb 00 3e
                    srl       (ix-$80)                      ;[0530] dd cb 80 3e
                    srl       (iy-$01)                      ;[0534] fd cb ff 3e
                    srl       (ix+$01)                      ;[0538] dd cb 01 3e
                    srl       (iy+$7f)                      ;[053c] fd cb 7f 3e
                    rld                                     ;[0540] ed 6f
                    rrd                                     ;[0542] ed 67
                    ld        a,rlc (ix+$0a)                ;[0544] dd cb 0a 07
                    ld        b,rlc (ix+$0a)                ;[0548] dd cb 0a 00
                    ld        c,rlc (ix+$0a)                ;[054c] dd cb 0a 01
                    ld        d,rlc (ix+$0a)                ;[0550] dd cb 0a 02
                    ld        e,rlc (ix+$0a)                ;[0554] dd cb 0a 03
                    rlc       (ix+$0a)                      ;[0558] dd cb 0a 06
                    ld        h,rlc (ix+$0a)                ;[055c] dd cb 0a 04
                    ld        l,rlc (ix+$0a)                ;[0560] dd cb 0a 05
                    ld        b,rrc (ix+$0a)                ;[0564] dd cb 0a 08
                    ld        b,rl (ix+$0a)                 ;[0568] dd cb 0a 10
                    ld        b,rr (ix+$0a)                 ;[056c] dd cb 0a 18
                    ld        b,sla (ix+$0a)                ;[0570] dd cb 0a 20
                    ld        b,sra (ix+$0a)                ;[0574] dd cb 0a 28
                    ld        b,sll (ix+$0a)                ;[0578] dd cb 0a 30
                    ld        b,srl (ix+$0a)                ;[057c] dd cb 0a 38
                    ld        b,res 0,(ix+$0a)              ;[0580] dd cb 0a 80
                    ld        b,res 1,(ix+$0a)              ;[0584] dd cb 0a 88
                    ld        b,res 2,(ix+$0a)              ;[0588] dd cb 0a 90
                    ld        b,res 3,(ix+$0a)              ;[058c] dd cb 0a 98
                    ld        b,res 4,(ix+$0a)              ;[0590] dd cb 0a a0
                    ld        b,res 5,(ix+$0a)              ;[0594] dd cb 0a a8
                    ld        b,res 6,(ix+$0a)              ;[0598] dd cb 0a b0
                    ld        b,res 7,(ix+$0a)              ;[059c] dd cb 0a b8
                    ld        b,set 0,(ix+$0a)              ;[05a0] dd cb 0a c0
                    ld        b,set 1,(ix+$0a)              ;[05a4] dd cb 0a c8
                    ld        b,set 2,(ix+$0a)              ;[05a8] dd cb 0a d0
                    ld        b,set 3,(ix+$0a)              ;[05ac] dd cb 0a d8
                    ld        b,set 4,(ix+$0a)              ;[05b0] dd cb 0a e0
                    ld        b,set 5,(ix+$0a)              ;[05b4] dd cb 0a e8
                    ld        b,set 6,(ix+$0a)              ;[05b8] dd cb 0a f0
                    ld        b,set 7,(ix+$0a)              ;[05bc] dd cb 0a f8
                    in        a,($80)                       ;[05c0] db 80
                    in        a,(c)                         ;[05c2] ed 78
                    in        b,(c)                         ;[05c4] ed 40
                    in        c,(c)                         ;[05c6] ed 48
                    in        d,(c)                         ;[05c8] ed 50
                    in        e,(c)                         ;[05ca] ed 58
                    in        (c)                           ;[05cc] ed 70
                    in        h,(c)                         ;[05ce] ed 60
                    in        l,(c)                         ;[05d0] ed 68
                    out       ($ff),a                       ;[05d2] d3 ff
                    out       (c),a                         ;[05d4] ed 79
                    out       (c),b                         ;[05d6] ed 41
                    out       (c),c                         ;[05d8] ed 49
                    out       (c),d                         ;[05da] ed 51
                    out       (c),e                         ;[05dc] ed 59
                    out       (c),0                         ;[05de] ed 71
                    out       (c),0                         ;[05e0] ed 71
                    out       (c),h                         ;[05e2] ed 61
                    out       (c),l                         ;[05e4] ed 69
                    ini                                     ;[05e6] ed a2
                    inir                                    ;[05e8] ed b2
                    ind                                     ;[05ea] ed aa
                    indr                                    ;[05ec] ed ba
                    outi                                    ;[05ee] ed a3
                    otir                                    ;[05f0] ed b3
                    otir                                    ;[05f2] ed b3
                    outd                                    ;[05f4] ed ab
                    otdr                                    ;[05f6] ed bb
                    otdr                                    ;[05f8] ed bb
                    ld        a,a                           ;[05fa] 7f
                    ld        a,b                           ;[05fb] 78
                    ld        a,c                           ;[05fc] 79
                    ld        a,d                           ;[05fd] 7a
                    ld        a,e                           ;[05fe] 7b
                    ld        a,h                           ;[05ff] 7c
                    ld        a,l                           ;[0600] 7d
                    ld        a,ixh                         ;[0601] dd 7c
                    ld        a,ixl                         ;[0603] dd 7d
                    ld        a,iyh                         ;[0605] fd 7c
                    ld        a,iyl                         ;[0607] fd 7d
                    ld        b,a                           ;[0609] 47
                    ld        b,b                           ;[060a] 40
                    ld        b,c                           ;[060b] 41
                    ld        b,d                           ;[060c] 42
                    ld        b,e                           ;[060d] 43
                    ld        b,h                           ;[060e] 44
                    ld        b,l                           ;[060f] 45
                    ld        b,ixh                         ;[0610] dd 44
                    ld        b,ixl                         ;[0612] dd 45
                    ld        b,iyh                         ;[0614] fd 44
                    ld        b,iyl                         ;[0616] fd 45
                    ld        c,a                           ;[0618] 4f
                    ld        c,b                           ;[0619] 48
                    ld        c,c                           ;[061a] 49
                    ld        c,d                           ;[061b] 4a
                    ld        c,e                           ;[061c] 4b
                    ld        c,h                           ;[061d] 4c
                    ld        c,l                           ;[061e] 4d
                    ld        c,ixh                         ;[061f] dd 4c
                    ld        c,ixl                         ;[0621] dd 4d
                    ld        c,iyh                         ;[0623] fd 4c
                    ld        c,iyl                         ;[0625] fd 4d
                    ld        d,a                           ;[0627] 57
                    ld        d,b                           ;[0628] 50
                    ld        d,c                           ;[0629] 51
                    ld        d,d                           ;[062a] 52
                    ld        d,e                           ;[062b] 53
                    ld        d,h                           ;[062c] 54
                    ld        d,l                           ;[062d] 55
                    ld        d,ixh                         ;[062e] dd 54
                    ld        d,ixl                         ;[0630] dd 55
                    ld        d,iyh                         ;[0632] fd 54
                    ld        d,iyl                         ;[0634] fd 55
                    ld        e,a                           ;[0636] 5f
                    ld        e,b                           ;[0637] 58
                    ld        e,c                           ;[0638] 59
                    ld        e,d                           ;[0639] 5a
                    ld        e,e                           ;[063a] 5b
                    ld        e,h                           ;[063b] 5c
                    ld        e,l                           ;[063c] 5d
                    ld        e,ixh                         ;[063d] dd 5c
                    ld        e,ixl                         ;[063f] dd 5d
                    ld        e,iyh                         ;[0641] fd 5c
                    ld        e,iyl                         ;[0643] fd 5d
                    ld        h,a                           ;[0645] 67
                    ld        h,b                           ;[0646] 60
                    ld        h,c                           ;[0647] 61
                    ld        h,d                           ;[0648] 62
                    ld        h,e                           ;[0649] 63
                    ld        h,h                           ;[064a] 64
                    ld        h,l                           ;[064b] 65
                    ld        l,a                           ;[064c] 6f
                    ld        l,b                           ;[064d] 68
                    ld        l,c                           ;[064e] 69
                    ld        l,d                           ;[064f] 6a
                    ld        l,e                           ;[0650] 6b
                    ld        l,h                           ;[0651] 6c
                    ld        l,l                           ;[0652] 6d
                    ld        ixh,a                         ;[0653] dd 67
                    ld        ixh,b                         ;[0655] dd 60
                    ld        ixh,c                         ;[0657] dd 61
                    ld        ixh,d                         ;[0659] dd 62
                    ld        ixh,e                         ;[065b] dd 63
                    ld        ixh,ixh                       ;[065d] dd 64
                    ld        ixh,ixl                       ;[065f] dd 65
                    ld        ixl,a                         ;[0661] dd 6f
                    ld        ixl,b                         ;[0663] dd 68
                    ld        ixl,c                         ;[0665] dd 69
                    ld        ixl,d                         ;[0667] dd 6a
                    ld        ixl,e                         ;[0669] dd 6b
                    ld        ixl,ixh                       ;[066b] dd 6c
                    ld        ixl,ixl                       ;[066d] dd 6d
                    ld        iyh,a                         ;[066f] fd 67
                    ld        iyh,b                         ;[0671] fd 60
                    ld        iyh,c                         ;[0673] fd 61
                    ld        iyh,d                         ;[0675] fd 62
                    ld        iyh,e                         ;[0677] fd 63
                    ld        iyh,iyh                       ;[0679] fd 64
                    ld        iyh,iyl                       ;[067b] fd 65
                    ld        iyl,a                         ;[067d] fd 6f
                    ld        iyl,b                         ;[067f] fd 68
                    ld        iyl,c                         ;[0681] fd 69
                    ld        iyl,d                         ;[0683] fd 6a
                    ld        iyl,e                         ;[0685] fd 6b
                    ld        iyl,iyh                       ;[0687] fd 6c
                    ld        iyl,iyl                       ;[0689] fd 6d
                    ld        b,b                           ;[068b] 40
                    ld        c,c                           ;[068c] 49
                    ld        b,d                           ;[068d] 42
                    ld        c,e                           ;[068e] 4b
                    ld        b,h                           ;[068f] 44
                    ld        c,l                           ;[0690] 4d
                    ld        b,ixh                         ;[0691] dd 44
                    ld        c,ixl                         ;[0693] dd 4d
                    ld        b,iyh                         ;[0695] fd 44
                    ld        c,iyl                         ;[0697] fd 4d
                    ld        d,b                           ;[0699] 50
                    ld        e,c                           ;[069a] 59
                    ld        d,d                           ;[069b] 52
                    ld        e,e                           ;[069c] 5b
                    ld        d,h                           ;[069d] 54
                    ld        e,l                           ;[069e] 5d
                    ld        d,ixh                         ;[069f] dd 54
                    ld        c,ixl                         ;[06a1] dd 4d
                    ld        d,iyh                         ;[06a3] fd 54
                    ld        c,iyl                         ;[06a5] fd 4d
                    ld        h,b                           ;[06a7] 60
                    ld        l,c                           ;[06a8] 69
                    ld        h,d                           ;[06a9] 62
                    ld        l,e                           ;[06aa] 6b
                    ld        h,h                           ;[06ab] 64
                    ld        l,l                           ;[06ac] 6d
                    push      ix                            ;[06ad] dd e5
                    pop       hl                            ;[06af] e1
                    push      iy                            ;[06b0] fd e5
                    pop       hl                            ;[06b2] e1
                    ld        ixh,b                         ;[06b3] dd 60
                    ld        ixl,c                         ;[06b5] dd 69
                    ld        ixh,d                         ;[06b7] dd 62
                    ld        ixl,e                         ;[06b9] dd 6b
                    push      hl                            ;[06bb] e5
                    pop       ix                            ;[06bc] dd e1
                    ld        ixh,ixh                       ;[06be] dd 64
                    ld        ixl,ixl                       ;[06c0] dd 6d
                    push      iy                            ;[06c2] fd e5
                    pop       ix                            ;[06c4] dd e1
                    ld        iyh,b                         ;[06c6] fd 60
                    ld        iyl,c                         ;[06c8] fd 69
                    ld        iyh,d                         ;[06ca] fd 62
                    ld        iyl,e                         ;[06cc] fd 6b
                    push      hl                            ;[06ce] e5
                    pop       iy                            ;[06cf] fd e1
                    push      ix                            ;[06d1] dd e5
                    pop       iy                            ;[06d3] fd e1
                    ld        iyh,iyh                       ;[06d5] fd 64
                    ld        iyl,iyl                       ;[06d7] fd 6d
                    ld        sp,hl                         ;[06d9] f9
                    ld        sp,ix                         ;[06da] dd f9
                    ld        sp,iy                         ;[06dc] fd f9
                    ld        a,(hl)                        ;[06de] 7e
                    ld        b,(hl)                        ;[06df] 46
                    ld        c,(hl)                        ;[06e0] 4e
                    ld        d,(hl)                        ;[06e1] 56
                    ld        e,(hl)                        ;[06e2] 5e
                    ld        h,(hl)                        ;[06e3] 66
                    ld        l,(hl)                        ;[06e4] 6e
                    push      af                            ;[06e5] f5
                    ld        a,(hl)                        ;[06e6] 7e
                    ld        ixh,a                         ;[06e7] dd 67
                    pop       af                            ;[06e9] f1
                    push      af                            ;[06ea] f5
                    ld        a,(hl)                        ;[06eb] 7e
                    ld        ixl,a                         ;[06ec] dd 6f
                    pop       af                            ;[06ee] f1
                    push      af                            ;[06ef] f5
                    ld        a,(hl)                        ;[06f0] 7e
                    ld        iyh,a                         ;[06f1] fd 67
                    pop       af                            ;[06f3] f1
                    push      af                            ;[06f4] f5
                    ld        a,(hl)                        ;[06f5] 7e
                    ld        iyl,a                         ;[06f6] fd 6f
                    pop       af                            ;[06f8] f1
                    ld        c,(hl)                        ;[06f9] 4e
                    inc       hl                            ;[06fa] 23
                    ld        b,(hl)                        ;[06fb] 46
                    dec       hl                            ;[06fc] 2b
                    ld        e,(hl)                        ;[06fd] 5e
                    inc       hl                            ;[06fe] 23
                    ld        d,(hl)                        ;[06ff] 56
                    dec       hl                            ;[0700] 2b
                    push      af                            ;[0701] f5
                    ld        a,(hl)                        ;[0702] 7e
                    ld        ixl,a                         ;[0703] dd 6f
                    inc       hl                            ;[0705] 23
                    ld        a,(hl)                        ;[0706] 7e
                    ld        ixh,a                         ;[0707] dd 67
                    dec       hl                            ;[0709] 2b
                    pop       af                            ;[070a] f1
                    push      af                            ;[070b] f5
                    ld        a,(hl)                        ;[070c] 7e
                    ld        iyl,a                         ;[070d] fd 6f
                    inc       hl                            ;[070f] 23
                    ld        a,(hl)                        ;[0710] 7e
                    ld        iyh,a                         ;[0711] fd 67
                    dec       hl                            ;[0713] 2b
                    pop       af                            ;[0714] f1
                    ld        (hl),a                        ;[0715] 77
                    ld        (hl),b                        ;[0716] 70
                    ld        (hl),c                        ;[0717] 71
                    ld        (hl),d                        ;[0718] 72
                    ld        (hl),e                        ;[0719] 73
                    ld        (hl),h                        ;[071a] 74
                    ld        (hl),l                        ;[071b] 75
                    push      af                            ;[071c] f5
                    ld        a,ixh                         ;[071d] dd 7c
                    ld        (hl),a                        ;[071f] 77
                    pop       af                            ;[0720] f1
                    push      af                            ;[0721] f5
                    ld        a,ixl                         ;[0722] dd 7d
                    ld        (hl),a                        ;[0724] 77
                    pop       af                            ;[0725] f1
                    push      af                            ;[0726] f5
                    ld        a,iyh                         ;[0727] fd 7c
                    ld        (hl),a                        ;[0729] 77
                    pop       af                            ;[072a] f1
                    push      af                            ;[072b] f5
                    ld        a,iyl                         ;[072c] fd 7d
                    ld        (hl),a                        ;[072e] 77
                    pop       af                            ;[072f] f1
                    ld        ($1234),a                     ;[0730] 32 34 12
                    ld        ($1234),bc                    ;[0733] ed 43 34 12
                    ld        ($1234),de                    ;[0737] ed 53 34 12
                    ld        ($1234),hl                    ;[073b] 22 34 12
                    ld        ($1234),sp                    ;[073e] ed 73 34 12
                    ld        ($1234),ix                    ;[0742] dd 22 34 12
                    ld        ($1234),iy                    ;[0746] fd 22 34 12
                    push      af                            ;[074a] f5
                    ld        a,$56                         ;[074b] 3e 56
                    ld        ($1234),a                     ;[074d] 32 34 12
                    pop       af                            ;[0750] f1
                    ld        a,$80                         ;[0751] 3e 80
                    ld        b,$ff                         ;[0753] 06 ff
                    ld        c,$00                         ;[0755] 0e 00
                    ld        d,$01                         ;[0757] 16 01
                    ld        e,$7f                         ;[0759] 1e 7f
                    ld        h,$80                         ;[075b] 26 80
                    ld        l,$ff                         ;[075d] 2e ff
                    ld        ixh,$11                       ;[075f] dd 26 11
                    ld        ixl,$22                       ;[0762] dd 2e 22
                    ld        iyh,$88                       ;[0765] fd 26 88
                    ld        iyl,$99                       ;[0768] fd 2e 99
                    ld        bc,$8000                      ;[076b] 01 00 80
                    ld        de,$ffff                      ;[076e] 11 ff ff
                    ld        hl,$1234                      ;[0771] 21 34 12
                    ld        ix,$7fff                      ;[0774] dd 21 ff 7f
                    ld        iy,$ffff                      ;[0778] fd 21 ff ff
                    ld        bc,$078d                      ;[077c] 01 8d 07
                    ld        de,$05fa                      ;[077f] 11 fa 05
                    ld        hl,$0009                      ;[0782] 21 09 00
                    ld        ix,$0751                      ;[0785] dd 21 51 07
                    ld        iy,$0000                      ;[0789] fd 21 00 00
                    nop                                     ;[078d] 00
                    ld        a,($1234)                     ;[078e] 3a 34 12
                    ld        bc,($5678)                    ;[0791] ed 4b 78 56
                    ld        de,($9abc)                    ;[0795] ed 5b bc 9a
                    ld        hl,($def0)                    ;[0799] 2a f0 de
                    ld        sp,($2525)                    ;[079c] ed 7b 25 25
                    ld        ix,($0831)                    ;[07a0] dd 2a 31 08
                    ld        iy,($8931)                    ;[07a4] fd 2a 31 89
                    ld        a,($008c)                     ;[07a8] 3a 8c 00
                    ld        bc,($07a8)                    ;[07ab] ed 4b a8 07
                    ld        de,($07c2)                    ;[07af] ed 5b c2 07
                    ld        hl,($078d)                    ;[07b3] 2a 8d 07
                    ld        sp,($077c)                    ;[07b6] ed 7b 7c 07
                    ld        ix,($06f9)                    ;[07ba] dd 2a f9 06
                    ld        iy,($06d9)                    ;[07be] fd 2a d9 06
                    nop                                     ;[07c2] 00
                    ld        a,(bc)                        ;[07c3] 0a
                    ld        a,(de)                        ;[07c4] 1a
                    ld        (bc),a                        ;[07c5] 02
                    ld        (de),a                        ;[07c6] 12
                    ld        a,(ix-$80)                    ;[07c7] dd 7e 80
                    ld        b,(ix-$01)                    ;[07ca] dd 46 ff
                    ld        c,(ix+$00)                    ;[07cd] dd 4e 00
                    ld        d,(ix+$00)                    ;[07d0] dd 56 00
                    ld        e,(ix+$01)                    ;[07d3] dd 5e 01
                    ld        h,(ix+$7f)                    ;[07d6] dd 66 7f
                    ld        l,(ix+$40)                    ;[07d9] dd 6e 40
                    ld        a,(iy-$80)                    ;[07dc] fd 7e 80
                    ld        b,(iy-$01)                    ;[07df] fd 46 ff
                    ld        c,(iy+$00)                    ;[07e2] fd 4e 00
                    ld        d,(iy+$00)                    ;[07e5] fd 56 00
                    ld        e,(iy+$01)                    ;[07e8] fd 5e 01
                    ld        h,(iy+$7f)                    ;[07eb] fd 66 7f
                    ld        l,(iy+$40)                    ;[07ee] fd 6e 40
                    ld        (ix-$80),a                    ;[07f1] dd 77 80
                    ld        (ix-$01),b                    ;[07f4] dd 70 ff
                    ld        (ix+$00),c                    ;[07f7] dd 71 00
                    ld        (ix+$00),d                    ;[07fa] dd 72 00
                    ld        (ix+$01),e                    ;[07fd] dd 73 01
                    ld        (ix+$7f),h                    ;[0800] dd 74 7f
                    ld        (ix+$40),l                    ;[0803] dd 75 40
                    ld        (iy-$80),a                    ;[0806] fd 77 80
                    ld        (iy-$01),b                    ;[0809] fd 70 ff
                    ld        (iy+$00),c                    ;[080c] fd 71 00
                    ld        (iy+$00),d                    ;[080f] fd 72 00
                    ld        (iy+$01),e                    ;[0812] fd 73 01
                    ld        (iy+$7f),h                    ;[0815] fd 74 7f
                    ld        (iy+$40),l                    ;[0818] fd 75 40
                    ld        (ix-$80),$80                  ;[081b] dd 36 80 80
                    ld        (ix-$80),$ff                  ;[081f] dd 36 80 ff
                    ld        (ix-$80),$00                  ;[0823] dd 36 80 00
                    ld        (iy+$7f),$01                  ;[0827] fd 36 7f 01
                    ld        (iy+$7f),$7f                  ;[082b] fd 36 7f 7f
                    ld        (iy+$7f),$ff                  ;[082f] fd 36 7f ff
                    ld        (ix+$00),$80                  ;[0833] dd 36 00 80
                    ld        (ix+$00),$ff                  ;[0837] dd 36 00 ff
                    ld        (ix+$00),$00                  ;[083b] dd 36 00 00
                    ld        (iy+$00),$01                  ;[083f] fd 36 00 01
                    ld        (iy+$00),$7f                  ;[0843] fd 36 00 7f
                    ld        (iy+$00),$ff                  ;[0847] fd 36 00 ff
                    jp        $0000                         ;[084b] c3 00 00
                    jp        $1234                         ;[084e] c3 34 12
                    jp        $ffff                         ;[0851] c3 ff ff
                    jp        $084b                         ;[0854] c3 4b 08
                    jp        $0854                         ;[0857] c3 54 08
                    jp        $0009                         ;[085a] c3 09 00
                    jr        $07df                         ;[085d] 18 80
                    jr        $08e0                         ;[085f] 18 7f
                    jr        $08a3                         ;[0861] 18 40
                    jr        $085d                         ;[0863] 18 f8
                    jr        $084b                         ;[0865] 18 e4
                    jr        $07f1                         ;[0867] 18 88
                    jp        z,$1234                       ;[0869] ca 34 12
                    jp        nz,$1234                      ;[086c] c2 34 12
                    jp        c,$1234                       ;[086f] da 34 12
                    jp        nc,$1234                      ;[0872] d2 34 12
                    jp        pe,$1234                      ;[0875] ea 34 12
                    jp        po,$1234                      ;[0878] e2 34 12
                    jp        p,$1234                       ;[087b] f2 34 12
                    jp        m,$1234                       ;[087e] fa 34 12
                    jp        z,$0869                       ;[0881] ca 69 08
                    jp        nz,$084b                      ;[0884] c2 4b 08
                    jp        c,$084b                       ;[0887] da 4b 08
                    jp        nc,$0000                      ;[088a] d2 00 00
                    jp        pe,$0009                      ;[088d] ea 09 00
                    jp        po,$084b                      ;[0890] e2 4b 08
                    jp        p,$0863                       ;[0893] f2 63 08
                    jp        m,$085d                       ;[0896] fa 5d 08
                    jr        z,$081b                       ;[0899] 28 80
                    jr        nz,$089c                      ;[089b] 20 ff
                    jr        c,$08a0                       ;[089d] 38 01
                    jr        nc,$0920                      ;[089f] 30 7f
                    jr        z,$0869                       ;[08a1] 28 c6
                    jr        nz,$084b                      ;[08a3] 20 a6
                    jr        c,$084b                       ;[08a5] 38 a4
                    jr        nc,$08a1                      ;[08a7] 30 f8
                    jp        z,$1234                       ;[08a9] ca 34 12
                    jp        nz,$1234                      ;[08ac] c2 34 12
                    jp        c,$1234                       ;[08af] da 34 12
                    jp        nc,$1234                      ;[08b2] d2 34 12
                    jp        pe,$1234                      ;[08b5] ea 34 12
                    jp        po,$1234                      ;[08b8] e2 34 12
                    jp        p,$1234                       ;[08bb] f2 34 12
                    jp        m,$1234                       ;[08be] fa 34 12
                    jp        m,$1234                       ;[08c1] fa 34 12
                    djnz      $0846                         ;[08c4] 10 80
                    djnz      $08c7                         ;[08c6] 10 ff
                    djnz      $08cb                         ;[08c8] 10 01
                    djnz      $094b                         ;[08ca] 10 7f
                    djnz      $085d                         ;[08cc] 10 8f
                    djnz      $08c4                         ;[08ce] 10 f4
                    djnz      $08c4                         ;[08d0] 10 f2
                    push      bc                            ;[08d2] c5
                    ret                                     ;[08d3] c9

                    push      de                            ;[08d4] d5
                    ret                                     ;[08d5] c9

                    jp        (hl)                          ;[08d6] e9
                    jp        (ix)                          ;[08d7] dd e9
                    jp        (iy)                          ;[08d9] fd e9
                    push      bc                            ;[08db] c5
                    ret                                     ;[08dc] c9

                    push      de                            ;[08dd] d5
                    ret                                     ;[08de] c9

                    jp        (hl)                          ;[08df] e9
                    jp        (ix)                          ;[08e0] dd e9
                    jp        (iy)                          ;[08e2] fd e9
                    call      $0000                         ;[08e4] cd 00 00
                    call      $1234                         ;[08e7] cd 34 12
                    call      $ffff                         ;[08ea] cd ff ff
                    call      $08e4                         ;[08ed] cd e4 08
                    call      $0000                         ;[08f0] cd 00 00
                    call      $0009                         ;[08f3] cd 09 00
                    call      nz,$1234                      ;[08f6] c4 34 12
                    call      z,$1234                       ;[08f9] cc 34 12
                    call      nc,$1234                      ;[08fc] d4 34 12
                    call      c,$1234                       ;[08ff] dc 34 12
                    call      po,$1234                      ;[0902] e4 34 12
                    call      pe,$1234                      ;[0905] ec 34 12
                    call      p,$1234                       ;[0908] f4 34 12
                    call      m,$1234                       ;[090b] fc 34 12
                    call      nz,$090e                      ;[090e] c4 0e 09
                    call      z,$08f6                       ;[0911] cc f6 08
                    call      nc,$08e4                      ;[0914] d4 e4 08
                    call      c,$08e4                       ;[0917] dc e4 08
                    call      po,$0009                      ;[091a] e4 09 00
                    call      pe,$0000                      ;[091d] ec 00 00
                    call      p,$0009                       ;[0920] f4 09 00
                    call      m,$0000                       ;[0923] fc 00 00
                    ret                                     ;[0926] c9

                    reti                                    ;[0927] ed 4d

                    retn                                    ;[0929] ed 45

                    ret       nz                            ;[092b] c0
                    ret       z                             ;[092c] c8
                    ret       nc                            ;[092d] d0
                    ret       c                             ;[092e] d8
                    ret       po                            ;[092f] e0
                    ret       pe                            ;[0930] e8
                    ret       p                             ;[0931] f0
                    ret       m                             ;[0932] f8
                    rst       $00                           ;[0933] c7
                    rst       $00                           ;[0934] c7
                    rst       $00                           ;[0935] c7
                    rst       $08                           ;[0936] cf
                    rst       $08                           ;[0937] cf
                    rst       $08                           ;[0938] cf
                    rst       $10                           ;[0939] d7
                    rst       $10                           ;[093a] d7
                    rst       $10                           ;[093b] d7
                    rst       $18                           ;[093c] df
                    rst       $18                           ;[093d] df
                    rst       $18                           ;[093e] df
                    rst       $20                           ;[093f] e7
                    rst       $20                           ;[0940] e7
                    rst       $20                           ;[0941] e7
                    rst       $28                           ;[0942] ef
                    rst       $28                           ;[0943] ef
                    rst       $28                           ;[0944] ef
                    rst       $30                           ;[0945] f7
                    rst       $30                           ;[0946] f7
                    rst       $30                           ;[0947] f7
                    rst       $38                           ;[0948] ff
                    rst       $38                           ;[0949] ff
                    rst       $38                           ;[094a] ff
                    ld        c,b                           ;[094b] 48
                    ld        h,l                           ;[094c] 65
                    ld        l,h                           ;[094d] 6c
                    ld        l,h                           ;[094e] 6c
                    ld        l,a                           ;[094f] 6f
                    inc       l                             ;[0950] 2c
                    ld        d,a                           ;[0951] 57
                    ld        l,a                           ;[0952] 6f
                    ld        (hl),d                        ;[0953] 72
                    ld        l,h                           ;[0954] 6c
                    ld        h,h                           ;[0955] 64
                    ld        hl,$8000                      ;[0956] 21 00 80
                    rst       $38                           ;[0959] ff
                    nop                                     ;[095a] 00
                    ld        bc,$ff7f                      ;[095b] 01 7f ff
                    nop                                     ;[095e] 00
                    djnz      $0981                         ;[095f] 10 20
                    jr        nc,$09a3                      ;[0961] 30 40
                    ld        d,b                           ;[0963] 50
                    ld        h,b                           ;[0964] 60
                    ld        (hl),b                        ;[0965] 70
                    add       b                             ;[0966] 80
                    sub       b                             ;[0967] 90
                    and       b                             ;[0968] a0
                    or        b                             ;[0969] b0
                    ret       nz                            ;[096a] c0
                    ret       nc                            ;[096b] d0
                    ret       po                            ;[096c] e0
                    ret       p                             ;[096d] f0
                    ld        c,b                           ;[096e] 48
                    nop                                     ;[096f] 00
                    ld        h,l                           ;[0970] 65
                    nop                                     ;[0971] 00
                    ld        l,h                           ;[0972] 6c
                    nop                                     ;[0973] 00
                    ld        l,h                           ;[0974] 6c
                    nop                                     ;[0975] 00
                    ld        l,a                           ;[0976] 6f
                    nop                                     ;[0977] 00
                    inc       l                             ;[0978] 2c
                    nop                                     ;[0979] 00
                    ld        d,a                           ;[097a] 57
                    nop                                     ;[097b] 00
                    ld        l,a                           ;[097c] 6f
                    nop                                     ;[097d] 00
                    ld        (hl),d                        ;[097e] 72
                    nop                                     ;[097f] 00
                    ld        l,h                           ;[0980] 6c
                    nop                                     ;[0981] 00
                    ld        h,h                           ;[0982] 64
                    nop                                     ;[0983] 00
                    ld        hl,$0000                      ;[0984] 21 00 00
                    nop                                     ;[0987] 00
                    nop                                     ;[0988] 00
                    add       b                             ;[0989] 80
                    rst       $38                           ;[098a] ff
                    rst       $38                           ;[098b] ff
                    nop                                     ;[098c] 00
                    nop                                     ;[098d] 00
                    ld        bc,$ff00                      ;[098e] 01 00 ff
                    ld        a,a                           ;[0991] 7f
                    rst       $38                           ;[0992] ff
                    rst       $38                           ;[0993] ff
                    inc       hl                            ;[0994] 23
                    ld        bc,$1234                      ;[0995] 01 34 12
                    ld        b,l                           ;[0998] 45
                    inc       hl                            ;[0999] 23
                    ld        d,(hl)                        ;[099a] 56
                    inc       (hl)                          ;[099b] 34
                    ld        h,a                           ;[099c] 67
                    ld        b,l                           ;[099d] 45
                    ld        a,b                           ;[099e] 78
                    ld        d,(hl)                        ;[099f] 56
                    adc       c                             ;[09a0] 89
                    ld        h,a                           ;[09a1] 67
                    sbc       d                             ;[09a2] 9a
                    ld        a,b                           ;[09a3] 78
                    xor       e                             ;[09a4] ab
                    adc       c                             ;[09a5] 89
                    cp        h                             ;[09a6] bc
                    sbc       d                             ;[09a7] 9a
                    call      $00ab                         ;[09a8] cd ab 00
                    nop                                     ;[09ab] 00
                    ld        c,e                           ;[09ac] 4b
                    add       hl,bc                         ;[09ad] 09
                    add       hl,bc                         ;[09ae] 09
                    nop                                     ;[09af] 00
                    ld        c,e                           ;[09b0] 4b
                    add       hl,bc                         ;[09b1] 09
                    push      af                            ;[09b2] f5
                    push      hl                            ;[09b3] e5
                    ld        h,b                           ;[09b4] 60
                    ld        l,c                           ;[09b5] 69
                    ld        a,$00                         ;[09b6] 3e 00
                    out       ($c5),a                       ;[09b8] d3 c5
                    ld        b,h                           ;[09ba] 44
                    ld        c,l                           ;[09bb] 4d
                    pop       hl                            ;[09bc] e1
                    pop       af                            ;[09bd] f1
                    push      af                            ;[09be] f5
                    push      hl                            ;[09bf] e5
                    ld        h,d                           ;[09c0] 62
                    ld        l,e                           ;[09c1] 6b
                    ld        a,$00                         ;[09c2] 3e 00
                    out       ($c5),a                       ;[09c4] d3 c5
                    ld        d,h                           ;[09c6] 54
                    ld        e,l                           ;[09c7] 5d
                    pop       hl                            ;[09c8] e1
                    pop       af                            ;[09c9] f1
                    push      af                            ;[09ca] f5
                    ld        a,$00                         ;[09cb] 3e 00
                    out       ($c5),a                       ;[09cd] d3 c5
                    pop       af                            ;[09cf] f1
                    push      af                            ;[09d0] f5
                    push      bc                            ;[09d1] c5
                    ld        c,a                           ;[09d2] 4f
                    ld        a,$80                         ;[09d3] 3e 80
                    out       ($c5),a                       ;[09d5] d3 c5
                    pop       bc                            ;[09d7] c1
                    pop       af                            ;[09d8] f1
                    push      af                            ;[09d9] f5
                    push      bc                            ;[09da] c5
                    ld        c,b                           ;[09db] 48
                    ld        a,$80                         ;[09dc] 3e 80
                    out       ($c5),a                       ;[09de] d3 c5
                    pop       bc                            ;[09e0] c1
                    pop       af                            ;[09e1] f1
                    push      af                            ;[09e2] f5
                    ld        a,$80                         ;[09e3] 3e 80
                    out       ($c5),a                       ;[09e5] d3 c5
                    pop       af                            ;[09e7] f1
                    push      af                            ;[09e8] f5
                    push      bc                            ;[09e9] c5
                    ld        c,d                           ;[09ea] 4a
                    ld        a,$80                         ;[09eb] 3e 80
                    out       ($c5),a                       ;[09ed] d3 c5
                    pop       bc                            ;[09ef] c1
                    pop       af                            ;[09f0] f1
                    push      af                            ;[09f1] f5
                    push      bc                            ;[09f2] c5
                    ld        c,e                           ;[09f3] 4b
                    ld        a,$80                         ;[09f4] 3e 80
                    out       ($c5),a                       ;[09f6] d3 c5
                    pop       bc                            ;[09f8] c1
                    pop       af                            ;[09f9] f1
                    push      af                            ;[09fa] f5
                    push      hl                            ;[09fb] e5
                    ld        h,b                           ;[09fc] 60
                    ld        l,c                           ;[09fd] 69
                    ld        a,$40                         ;[09fe] 3e 40
                    out       ($c5),a                       ;[0a00] d3 c5
                    ld        b,h                           ;[0a02] 44
                    ld        c,l                           ;[0a03] 4d
                    pop       hl                            ;[0a04] e1
                    pop       af                            ;[0a05] f1
                    push      af                            ;[0a06] f5
                    push      hl                            ;[0a07] e5
                    ld        h,d                           ;[0a08] 62
                    ld        l,e                           ;[0a09] 6b
                    ld        a,$40                         ;[0a0a] 3e 40
                    out       ($c5),a                       ;[0a0c] d3 c5
                    ld        d,h                           ;[0a0e] 54
                    ld        e,l                           ;[0a0f] 5d
                    pop       hl                            ;[0a10] e1
                    pop       af                            ;[0a11] f1
                    push      af                            ;[0a12] f5
                    ld        a,$40                         ;[0a13] 3e 40
                    out       ($c5),a                       ;[0a15] d3 c5
                    pop       af                            ;[0a17] f1
                    push      af                            ;[0a18] f5
                    push      bc                            ;[0a19] c5
                    ld        c,a                           ;[0a1a] 4f
                    ld        a,$c0                         ;[0a1b] 3e c0
                    out       ($c5),a                       ;[0a1d] d3 c5
                    pop       bc                            ;[0a1f] c1
                    pop       af                            ;[0a20] f1
                    push      af                            ;[0a21] f5
                    push      bc                            ;[0a22] c5
                    ld        c,b                           ;[0a23] 48
                    ld        a,$c0                         ;[0a24] 3e c0
                    out       ($c5),a                       ;[0a26] d3 c5
                    pop       bc                            ;[0a28] c1
                    pop       af                            ;[0a29] f1
                    push      af                            ;[0a2a] f5
                    ld        a,$c0                         ;[0a2b] 3e c0
                    out       ($c5),a                       ;[0a2d] d3 c5
                    pop       af                            ;[0a2f] f1
                    push      af                            ;[0a30] f5
                    push      bc                            ;[0a31] c5
                    ld        c,d                           ;[0a32] 4a
                    ld        a,$c0                         ;[0a33] 3e c0
                    out       ($c5),a                       ;[0a35] d3 c5
                    pop       bc                            ;[0a37] c1
                    pop       af                            ;[0a38] f1
                    push      af                            ;[0a39] f5
                    push      bc                            ;[0a3a] c5
                    ld        c,e                           ;[0a3b] 4b
                    ld        a,$c0                         ;[0a3c] 3e c0
                    out       ($c5),a                       ;[0a3e] d3 c5
                    pop       bc                            ;[0a40] c1
                    pop       af                            ;[0a41] f1
                    push      af                            ;[0a42] f5
                    push      hl                            ;[0a43] e5
                    ld        h,b                           ;[0a44] 60
                    ld        l,c                           ;[0a45] 69
                    ld        a,$01                         ;[0a46] 3e 01
                    out       ($c5),a                       ;[0a48] d3 c5
                    ld        b,h                           ;[0a4a] 44
                    ld        c,l                           ;[0a4b] 4d
                    pop       hl                            ;[0a4c] e1
                    pop       af                            ;[0a4d] f1
                    push      af                            ;[0a4e] f5
                    push      hl                            ;[0a4f] e5
                    ld        h,d                           ;[0a50] 62
                    ld        l,e                           ;[0a51] 6b
                    ld        a,$01                         ;[0a52] 3e 01
                    out       ($c5),a                       ;[0a54] d3 c5
                    ld        d,h                           ;[0a56] 54
                    ld        e,l                           ;[0a57] 5d
                    pop       hl                            ;[0a58] e1
                    pop       af                            ;[0a59] f1
                    push      af                            ;[0a5a] f5
                    ld        a,$01                         ;[0a5b] 3e 01
                    out       ($c5),a                       ;[0a5d] d3 c5
                    pop       af                            ;[0a5f] f1
                    push      af                            ;[0a60] f5
                    push      bc                            ;[0a61] c5
                    ld        c,a                           ;[0a62] 4f
                    ld        a,$81                         ;[0a63] 3e 81
                    out       ($c5),a                       ;[0a65] d3 c5
                    pop       bc                            ;[0a67] c1
                    pop       af                            ;[0a68] f1
                    push      af                            ;[0a69] f5
                    push      bc                            ;[0a6a] c5
                    ld        c,b                           ;[0a6b] 48
                    ld        a,$81                         ;[0a6c] 3e 81
                    out       ($c5),a                       ;[0a6e] d3 c5
                    pop       bc                            ;[0a70] c1
                    pop       af                            ;[0a71] f1
                    push      af                            ;[0a72] f5
                    ld        a,$81                         ;[0a73] 3e 81
                    out       ($c5),a                       ;[0a75] d3 c5
                    pop       af                            ;[0a77] f1
                    push      af                            ;[0a78] f5
                    push      bc                            ;[0a79] c5
                    ld        c,d                           ;[0a7a] 4a
                    ld        a,$81                         ;[0a7b] 3e 81
                    out       ($c5),a                       ;[0a7d] d3 c5
                    pop       bc                            ;[0a7f] c1
                    pop       af                            ;[0a80] f1
                    push      af                            ;[0a81] f5
                    push      bc                            ;[0a82] c5
                    ld        c,e                           ;[0a83] 4b
                    ld        a,$81                         ;[0a84] 3e 81
                    out       ($c5),a                       ;[0a86] d3 c5
                    pop       bc                            ;[0a88] c1
                    pop       af                            ;[0a89] f1
                    push      af                            ;[0a8a] f5
                    push      hl                            ;[0a8b] e5
                    ld        h,b                           ;[0a8c] 60
                    ld        l,c                           ;[0a8d] 69
                    ld        a,$41                         ;[0a8e] 3e 41
                    out       ($c5),a                       ;[0a90] d3 c5
                    ld        b,h                           ;[0a92] 44
                    ld        c,l                           ;[0a93] 4d
                    pop       hl                            ;[0a94] e1
                    pop       af                            ;[0a95] f1
                    push      af                            ;[0a96] f5
                    push      hl                            ;[0a97] e5
                    ld        h,d                           ;[0a98] 62
                    ld        l,e                           ;[0a99] 6b
                    ld        a,$41                         ;[0a9a] 3e 41
                    out       ($c5),a                       ;[0a9c] d3 c5
                    ld        d,h                           ;[0a9e] 54
                    ld        e,l                           ;[0a9f] 5d
                    pop       hl                            ;[0aa0] e1
                    pop       af                            ;[0aa1] f1
                    push      af                            ;[0aa2] f5
                    ld        a,$41                         ;[0aa3] 3e 41
                    out       ($c5),a                       ;[0aa5] d3 c5
                    pop       af                            ;[0aa7] f1
                    push      af                            ;[0aa8] f5
                    push      bc                            ;[0aa9] c5
                    ld        c,a                           ;[0aaa] 4f
                    ld        a,$c1                         ;[0aab] 3e c1
                    out       ($c5),a                       ;[0aad] d3 c5
                    pop       bc                            ;[0aaf] c1
                    pop       af                            ;[0ab0] f1
                    push      af                            ;[0ab1] f5
                    push      bc                            ;[0ab2] c5
                    ld        c,b                           ;[0ab3] 48
                    ld        a,$c1                         ;[0ab4] 3e c1
                    out       ($c5),a                       ;[0ab6] d3 c5
                    pop       bc                            ;[0ab8] c1
                    pop       af                            ;[0ab9] f1
                    push      af                            ;[0aba] f5
                    ld        a,$c1                         ;[0abb] 3e c1
                    out       ($c5),a                       ;[0abd] d3 c5
                    pop       af                            ;[0abf] f1
                    push      af                            ;[0ac0] f5
                    push      bc                            ;[0ac1] c5
                    ld        c,d                           ;[0ac2] 4a
                    ld        a,$c1                         ;[0ac3] 3e c1
                    out       ($c5),a                       ;[0ac5] d3 c5
                    pop       bc                            ;[0ac7] c1
                    pop       af                            ;[0ac8] f1
                    push      af                            ;[0ac9] f5
                    push      bc                            ;[0aca] c5
                    ld        c,e                           ;[0acb] 4b
                    ld        a,$c1                         ;[0acc] 3e c1
                    out       ($c5),a                       ;[0ace] d3 c5
                    pop       bc                            ;[0ad0] c1
                    pop       af                            ;[0ad1] f1
                    push      af                            ;[0ad2] f5
                    push      hl                            ;[0ad3] e5
                    ld        h,b                           ;[0ad4] 60
                    ld        l,c                           ;[0ad5] 69
                    ld        a,$02                         ;[0ad6] 3e 02
                    out       ($c5),a                       ;[0ad8] d3 c5
                    ld        b,h                           ;[0ada] 44
                    ld        c,l                           ;[0adb] 4d
                    pop       hl                            ;[0adc] e1
                    pop       af                            ;[0add] f1
                    push      af                            ;[0ade] f5
                    push      hl                            ;[0adf] e5
                    ld        h,d                           ;[0ae0] 62
                    ld        l,e                           ;[0ae1] 6b
                    ld        a,$02                         ;[0ae2] 3e 02
                    out       ($c5),a                       ;[0ae4] d3 c5
                    ld        d,h                           ;[0ae6] 54
                    ld        e,l                           ;[0ae7] 5d
                    pop       hl                            ;[0ae8] e1
                    pop       af                            ;[0ae9] f1
                    push      af                            ;[0aea] f5
                    ld        a,$02                         ;[0aeb] 3e 02
                    out       ($c5),a                       ;[0aed] d3 c5
                    pop       af                            ;[0aef] f1
                    push      af                            ;[0af0] f5
                    push      bc                            ;[0af1] c5
                    ld        c,a                           ;[0af2] 4f
                    ld        a,$82                         ;[0af3] 3e 82
                    out       ($c5),a                       ;[0af5] d3 c5
                    pop       bc                            ;[0af7] c1
                    pop       af                            ;[0af8] f1
                    push      af                            ;[0af9] f5
                    push      bc                            ;[0afa] c5
                    ld        c,b                           ;[0afb] 48
                    ld        a,$82                         ;[0afc] 3e 82
                    out       ($c5),a                       ;[0afe] d3 c5
                    pop       bc                            ;[0b00] c1
                    pop       af                            ;[0b01] f1
                    push      af                            ;[0b02] f5
                    ld        a,$82                         ;[0b03] 3e 82
                    out       ($c5),a                       ;[0b05] d3 c5
                    pop       af                            ;[0b07] f1
                    push      af                            ;[0b08] f5
                    push      bc                            ;[0b09] c5
                    ld        c,d                           ;[0b0a] 4a
                    ld        a,$82                         ;[0b0b] 3e 82
                    out       ($c5),a                       ;[0b0d] d3 c5
                    pop       bc                            ;[0b0f] c1
                    pop       af                            ;[0b10] f1
                    push      af                            ;[0b11] f5
                    push      bc                            ;[0b12] c5
                    ld        c,e                           ;[0b13] 4b
                    ld        a,$82                         ;[0b14] 3e 82
                    out       ($c5),a                       ;[0b16] d3 c5
                    pop       bc                            ;[0b18] c1
                    pop       af                            ;[0b19] f1
                    push      hl                            ;[0b1a] e5
                    ld        h,b                           ;[0b1b] 60
                    ld        l,c                           ;[0b1c] 69
                    in        a,($c8)                       ;[0b1d] db c8
                    pop       hl                            ;[0b1f] e1
                    push      hl                            ;[0b20] e5
                    ld        h,d                           ;[0b21] 62
                    ld        l,e                           ;[0b22] 6b
                    in        a,($c8)                       ;[0b23] db c8
                    pop       hl                            ;[0b25] e1
                    in        a,($c8)                       ;[0b26] db c8
                    out       ($c6),a                       ;[0b28] d3 c6
                    ld        a,b                           ;[0b2a] 78
                    out       ($c6),a                       ;[0b2b] d3 c6
                    ld        a,c                           ;[0b2d] 79
                    out       ($c6),a                       ;[0b2e] d3 c6
                    ld        a,d                           ;[0b30] 7a
                    out       ($c6),a                       ;[0b31] d3 c6
                    ld        a,e                           ;[0b33] 7b
                    out       ($c6),a                       ;[0b34] d3 c6
                    ld        a,h                           ;[0b36] 7c
                    out       ($c6),a                       ;[0b37] d3 c6
                    ld        a,l                           ;[0b39] 7d
                    out       ($c6),a                       ;[0b3a] d3 c6
                    out       ($c6),a                       ;[0b3c] d3 c6
                    ld        a,b                           ;[0b3e] 78
                    out       ($c6),a                       ;[0b3f] d3 c6
                    ld        a,c                           ;[0b41] 79
                    out       ($c6),a                       ;[0b42] d3 c6
                    ld        a,d                           ;[0b44] 7a
                    out       ($c6),a                       ;[0b45] d3 c6
                    ld        a,e                           ;[0b47] 7b
                    out       ($c6),a                       ;[0b48] d3 c6
                    ld        a,h                           ;[0b4a] 7c
                    out       ($c6),a                       ;[0b4b] d3 c6
                    ld        a,l                           ;[0b4d] 7d
                    out       ($c6),a                       ;[0b4e] d3 c6
                    out       ($c7),a                       ;[0b50] d3 c7
                    ld        a,b                           ;[0b52] 78
                    out       ($c7),a                       ;[0b53] d3 c7
                    ld        a,c                           ;[0b55] 79
                    out       ($c7),a                       ;[0b56] d3 c7
                    ld        a,d                           ;[0b58] 7a
                    out       ($c7),a                       ;[0b59] d3 c7
                    ld        a,e                           ;[0b5b] 7b
                    out       ($c7),a                       ;[0b5c] d3 c7
                    ld        a,h                           ;[0b5e] 7c
                    out       ($c7),a                       ;[0b5f] d3 c7
                    ld        a,l                           ;[0b61] 7d
                    out       ($c7),a                       ;[0b62] d3 c7
                    out       ($c7),a                       ;[0b64] d3 c7
                    ld        a,b                           ;[0b66] 78
                    out       ($c7),a                       ;[0b67] d3 c7
                    ld        a,c                           ;[0b69] 79
                    out       ($c7),a                       ;[0b6a] d3 c7
                    ld        a,d                           ;[0b6c] 7a
                    out       ($c7),a                       ;[0b6d] d3 c7
                    ld        a,e                           ;[0b6f] 7b
                    out       ($c7),a                       ;[0b70] d3 c7
                    ld        a,h                           ;[0b72] 7c
                    out       ($c7),a                       ;[0b73] d3 c7
                    ld        a,l                           ;[0b75] 7d
                    out       ($c7),a                       ;[0b76] d3 c7
                    ld        (hl),$01                      ;[0b78] 36 01
                    push      hl                            ;[0b7a] e5
                    ld        h,b                           ;[0b7b] 60
                    ld        l,c                           ;[0b7c] 69
                    ld        (hl),$02                      ;[0b7d] 36 02
                    pop       hl                            ;[0b7f] e1
                    push      hl                            ;[0b80] e5
                    ld        h,d                           ;[0b81] 62
                    ld        l,e                           ;[0b82] 6b
                    ld        (hl),$03                      ;[0b83] 36 03
                    pop       hl                            ;[0b85] e1
                    ld        c,(hl)                        ;[0b86] 4e
                    inc       hl                            ;[0b87] 23
                    ld        b,(hl)                        ;[0b88] 46
                    dec       hl                            ;[0b89] 2b
                    ld        e,(hl)                        ;[0b8a] 5e
                    inc       hl                            ;[0b8b] 23
                    ld        d,(hl)                        ;[0b8c] 56
                    dec       hl                            ;[0b8d] 2b
                    push      af                            ;[0b8e] f5
                    ld        a,(hl)                        ;[0b8f] 7e
                    ld        ixl,a                         ;[0b90] dd 6f
                    inc       hl                            ;[0b92] 23
                    ld        a,(hl)                        ;[0b93] 7e
                    ld        ixh,a                         ;[0b94] dd 67
                    dec       hl                            ;[0b96] 2b
                    pop       af                            ;[0b97] f1
                    push      af                            ;[0b98] f5
                    ld        a,(hl)                        ;[0b99] 7e
                    ld        iyl,a                         ;[0b9a] fd 6f
                    inc       hl                            ;[0b9c] 23
                    ld        a,(hl)                        ;[0b9d] 7e
                    ld        iyh,a                         ;[0b9e] fd 67
                    dec       hl                            ;[0ba0] 2b
                    pop       af                            ;[0ba1] f1
                    ld        (hl),c                        ;[0ba2] 71
                    inc       hl                            ;[0ba3] 23
                    ld        (hl),b                        ;[0ba4] 70
                    dec       hl                            ;[0ba5] 2b
                    ld        (hl),e                        ;[0ba6] 73
                    inc       hl                            ;[0ba7] 23
                    ld        (hl),d                        ;[0ba8] 72
                    dec       hl                            ;[0ba9] 2b
                    push      af                            ;[0baa] f5
                    ld        a,ixl                         ;[0bab] dd 7d
                    ld        (hl),a                        ;[0bad] 77
                    inc       hl                            ;[0bae] 23
                    ld        a,ixh                         ;[0baf] dd 7c
                    ld        (hl),a                        ;[0bb1] 77
                    dec       hl                            ;[0bb2] 2b
                    pop       af                            ;[0bb3] f1
                    push      af                            ;[0bb4] f5
                    ld        a,iyl                         ;[0bb5] fd 7d
                    ld        (hl),a                        ;[0bb7] 77
                    inc       hl                            ;[0bb8] 23
                    ld        a,iyh                         ;[0bb9] fd 7c
                    ld        (hl),a                        ;[0bbb] 77
                    dec       hl                            ;[0bbc] 2b
                    pop       af                            ;[0bbd] f1
                    ld        c,(ix+$00)                    ;[0bbe] dd 4e 00
                    ld        b,(ix+$01)                    ;[0bc1] dd 46 01
                    ld        e,(ix+$00)                    ;[0bc4] dd 5e 00
                    ld        d,(ix+$01)                    ;[0bc7] dd 56 01
                    ld        l,(ix+$00)                    ;[0bca] dd 6e 00
                    ld        h,(ix+$01)                    ;[0bcd] dd 66 01
                    ld        c,(ix+$01)                    ;[0bd0] dd 4e 01
                    ld        b,(ix+$02)                    ;[0bd3] dd 46 02
                    ld        e,(ix+$02)                    ;[0bd6] dd 5e 02
                    ld        d,(ix+$03)                    ;[0bd9] dd 56 03
                    ld        l,(ix+$03)                    ;[0bdc] dd 6e 03
                    ld        h,(ix+$04)                    ;[0bdf] dd 66 04
                    ld        (ix+$00),c                    ;[0be2] dd 71 00
                    ld        (ix+$01),b                    ;[0be5] dd 70 01
                    ld        (ix+$00),e                    ;[0be8] dd 73 00
                    ld        (ix+$01),d                    ;[0beb] dd 72 01
                    ld        (ix+$00),l                    ;[0bee] dd 75 00
                    ld        (ix+$01),h                    ;[0bf1] dd 74 01
                    ld        (ix+$01),c                    ;[0bf4] dd 71 01
                    ld        (ix+$02),b                    ;[0bf7] dd 70 02
                    ld        (ix+$02),e                    ;[0bfa] dd 73 02
                    ld        (ix+$03),d                    ;[0bfd] dd 72 03
                    ld        (ix+$03),l                    ;[0c00] dd 75 03
                    ld        (ix+$04),h                    ;[0c03] dd 74 04
                    ld        c,(iy+$00)                    ;[0c06] fd 4e 00
                    ld        b,(iy+$01)                    ;[0c09] fd 46 01
                    ld        e,(iy+$00)                    ;[0c0c] fd 5e 00
                    ld        d,(iy+$01)                    ;[0c0f] fd 56 01
                    ld        l,(iy+$00)                    ;[0c12] fd 6e 00
                    ld        h,(iy+$01)                    ;[0c15] fd 66 01
                    ld        c,(iy+$01)                    ;[0c18] fd 4e 01
                    ld        b,(iy+$02)                    ;[0c1b] fd 46 02
                    ld        e,(iy+$02)                    ;[0c1e] fd 5e 02
                    ld        d,(iy+$03)                    ;[0c21] fd 56 03
                    ld        l,(iy+$03)                    ;[0c24] fd 6e 03
                    ld        h,(iy+$04)                    ;[0c27] fd 66 04
                    ld        (iy+$00),c                    ;[0c2a] fd 71 00
                    ld        (iy+$01),b                    ;[0c2d] fd 70 01
                    ld        (iy+$00),e                    ;[0c30] fd 73 00
                    ld        (iy+$01),d                    ;[0c33] fd 72 01
                    ld        (iy+$00),l                    ;[0c36] fd 75 00
                    ld        (iy+$01),h                    ;[0c39] fd 74 01
                    ld        (iy+$01),c                    ;[0c3c] fd 71 01
                    ld        (iy+$02),b                    ;[0c3f] fd 70 02
                    ld        (iy+$02),e                    ;[0c42] fd 73 02
                    ld        (iy+$03),d                    ;[0c45] fd 72 03
                    ld        (iy+$03),l                    ;[0c48] fd 75 03
                    ld        (iy+$04),h                    ;[0c4b] fd 74 04
