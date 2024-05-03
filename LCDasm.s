;/*****************************************************************************/
; LCDasm.s: A single low-level command, parrotdelay. Modified from osasm.s.
; */

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXPORT  parrotdelay

parrotdelay
        SUBS    r0, #1
		BNE     parrotdelay
        BX      LR

    ALIGN
    END
