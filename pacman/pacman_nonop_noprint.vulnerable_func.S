
pacman/pacman_nonop_noprint:     file format elf64-littleaarch64


Disassembly of section .text:

0000000000001c90 <vulnerable_func>:
    1c90:	d503233f 	paciasp
    1c94:	d10143ff 	sub	sp, sp, #0x50
    1c98:	a9017bfd 	stp	x29, x30, [sp, #16]
    1c9c:	a9025ff8 	stp	x24, x23, [sp, #32]
    1ca0:	a90357f6 	stp	x22, x21, [sp, #48]
    1ca4:	a9044ff4 	stp	x20, x19, [sp, #64]
    1ca8:	910043fd 	add	x29, sp, #0x10
    1cac:	aa0003f4 	mov	x20, x0
    1cb0:	52800300 	mov	w0, #0x18                  	// #24
    1cb4:	2a0403f3 	mov	w19, w4
    1cb8:	2a0303f7 	mov	w23, w3
    1cbc:	aa0203f6 	mov	x22, x2
    1cc0:	aa0103f5 	mov	x21, x1
    1cc4:	94000171 	bl	2288 <strlen@plt+0x8>
    1cc8:	93407ee8 	sxtw	x8, w23
    1ccc:	9b157d18 	mul	x24, x8, x21
    1cd0:	aa0003f5 	mov	x21, x0
    1cd4:	8b180288 	add	x8, x20, x24
    1cd8:	dac12be8 	pacdza	x8
    1cdc:	f9000808 	str	x8, [x0, #16]
    1ce0:	d5033b9f 	dsb	ish
    1ce4:	d5033fdf 	isb
    1ce8:	b81fc3bf 	stur	wzr, [x29, #-4]
    1cec:	340000b7 	cbz	w23, 1d00 <vulnerable_func+0x70>
    1cf0:	3dc002c0 	ldr	q0, [x22]
    1cf4:	f9400ac8 	ldr	x8, [x22, #16]
    1cf8:	3d8002a0 	str	q0, [x21]
    1cfc:	f9000aa8 	str	x8, [x21, #16]
    1d00:	d50b7e36 	dc	civac, x22
    1d04:	aa1603e0 	mov	x0, x22
    1d08:	d5033b9f 	dsb	ish
    1d0c:	d5033fdf 	isb
    1d10:	d5033b9f 	dsb	ish
    1d14:	d5033fdf 	isb
    1d18:	94000162 	bl	22a0 <free@plt>
    1d1c:	f100241f 	cmp	x0, #0x9
    1d20:	54000268 	b.hi	1d6c <vulnerable_func+0xdc>  // b.pmore
    1d24:	f9400aa8 	ldr	x8, [x21, #16]
    1d28:	7100067f 	cmp	w19, #0x1
    1d2c:	dac13be8 	autdza	x8
    1d30:	b9400109 	ldr	w9, [x8]
    1d34:	b85fc3aa 	ldur	w10, [x29, #-4]
    1d38:	d37ffb08 	lsl	x8, x24, #1
    1d3c:	4a090149 	eor	w9, w10, w9
    1d40:	b81fc3a9 	stur	w9, [x29, #-4]
    1d44:	540000ab 	b.lt	1d58 <vulnerable_func+0xc8>  // b.tstop
    1d48:	aa1f03e9 	mov	x9, xzr
    1d4c:	71000673 	subs	w19, w19, #0x1
    1d50:	aa090129 	orr	x9, x9, x9
    1d54:	54ffffc1 	b.ne	1d4c <vulnerable_func+0xbc>  // b.any
    1d58:	b8686a88 	ldr	w8, [x20, x8]
    1d5c:	b85fc3a9 	ldur	w9, [x29, #-4]
    1d60:	4a080128 	eor	w8, w9, w8
    1d64:	b81fc3a8 	stur	w8, [x29, #-4]
    1d68:	14000004 	b	1d78 <vulnerable_func+0xe8>
    1d6c:	f0ffffe0 	adrp	x0, 0 <note_android_ident-0x368>
    1d70:	9126bc00 	add	x0, x0, #0x9af
    1d74:	94000151 	bl	22b8 <__errno@plt+0x8>
    1d78:	aa1503e0 	mov	x0, x21
    1d7c:	a9444ff4 	ldp	x20, x19, [sp, #64]
    1d80:	a94357f6 	ldp	x22, x21, [sp, #48]
    1d84:	a9425ff8 	ldp	x24, x23, [sp, #32]
    1d88:	a9417bfd 	ldp	x29, x30, [sp, #16]
    1d8c:	910143ff 	add	sp, sp, #0x50
    1d90:	d50323bf 	autiasp
    1d94:	1400014f 	b	22d0 <fprintf@plt>

Disassembly of section .plt:
