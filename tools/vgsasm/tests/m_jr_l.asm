.label1
nop
label2:
nop
@foo
nop

jr label1
jr label2
jr @foo
jr foo@label2

