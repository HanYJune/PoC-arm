
pacman_nop_noprint:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000001c90 <vulnerable_func>:
    1c90: d503233f     	paciasp
    1c94: d10143ff     	sub	sp, sp, #0x50
    1c98: a9017bfd     	stp	x29, x30, [sp, #0x10]
    1c9c: a9025ff8     	stp	x24, x23, [sp, #0x20]
    1ca0: a90357f6     	stp	x22, x21, [sp, #0x30]
    1ca4: a9044ff4     	stp	x20, x19, [sp, #0x40]
    1ca8: 910043fd     	add	x29, sp, #0x10
    1cac: aa0003f4     	mov	x20, x0
    1cb0: 52800300     	mov	w0, #0x18               // =24
    1cb4: 2a0403f3     	mov	w19, w4
    1cb8: 2a0303f7     	mov	w23, w3
    1cbc: aa0203f6     	mov	x22, x2
    1cc0: aa0103f5     	mov	x21, x1
    1cc4: 94000175     	bl	0x2298 <malloc@plt>
    1cc8: 93407ee8     	sxtw	x8, w23
    1ccc: 9b157d18     	mul	x24, x8, x21
    1cd0: aa0003f5     	mov	x21, x0
    1cd4: 8b180288     	add	x8, x20, x24
    1cd8: dac12be8     	pacdza	x8
    1cdc: f9000808     	str	x8, [x0, #0x10]
    1ce0: d503201f     	nop
    1ce4: d503201f     	nop
    1ce8: d503201f     	nop
    1cec: d503201f     	nop
    1cf0: d503201f     	nop
    1cf4: d5033b9f     	dsb	ish
    1cf8: d5033fdf     	isb
    1cfc: b81fc3bf     	stur	wzr, [x29, #-0x4]
    1d00: 340000b7     	cbz	w23, 0x1d14 <vulnerable_func+0x84>
    1d04: 3dc002c0     	ldr	q0, [x22]
    1d08: f9400ac8     	ldr	x8, [x22, #0x10]
    1d0c: 3d8002a0     	str	q0, [x21]
    1d10: f9000aa8     	str	x8, [x21, #0x10]
    1d14: d50b7e36     	dc	civac, x22
    1d18: aa1603e0     	mov	x0, x22
    1d1c: d5033b9f     	dsb	ish
    1d20: d5033fdf     	isb
    1d24: d5033b9f     	dsb	ish
    1d28: d5033fdf     	isb
    1d2c: 94000161     	bl	0x22b0 <strlen@plt>
    1d30: f100241f     	cmp	x0, #0x9
    1d34: 54000268     	b.hi	0x1d80 <vulnerable_func+0xf0>
    1d38: f9400aa8     	ldr	x8, [x21, #0x10]
    1d3c: 7100067f     	cmp	w19, #0x1
    1d40: dac13be8     	autdza	x8
    1d44: b9400109     	ldr	w9, [x8]
    1d48: b85fc3aa     	ldur	w10, [x29, #-0x4]
    1d4c: d37ffb08     	lsl	x8, x24, #1
    1d50: 4a090149     	eor	w9, w10, w9
    1d54: b81fc3a9     	stur	w9, [x29, #-0x4]
    1d58: 540000ab     	b.lt	0x1d6c <vulnerable_func+0xdc>
    1d5c: aa1f03e9     	mov	x9, xzr
    1d60: 71000673     	subs	w19, w19, #0x1
    1d64: aa090129     	orr	x9, x9, x9
    1d68: 54ffffc1     	b.ne	0x1d60 <vulnerable_func+0xd0>
    1d6c: b8686a88     	ldr	w8, [x20, x8]
    1d70: b85fc3a9     	ldur	w9, [x29, #-0x4]
    1d74: 4a080128     	eor	w8, w9, w8
    1d78: b81fc3a8     	stur	w8, [x29, #-0x4]
    1d7c: 14000004     	b	0x1d8c <vulnerable_func+0xfc>
    1d80: f0ffffe0     	adrp	x0, 0x0 <sysconf>
    1d84: 9126bc00     	add	x0, x0, #0x9af
    1d88: 94000150     	bl	0x22c8 <puts@plt>
    1d8c: aa1503e0     	mov	x0, x21
    1d90: a9444ff4     	ldp	x20, x19, [sp, #0x40]
    1d94: a94357f6     	ldp	x22, x21, [sp, #0x30]
    1d98: a9425ff8     	ldp	x24, x23, [sp, #0x20]
    1d9c: a9417bfd     	ldp	x29, x30, [sp, #0x10]
    1da0: 910143ff     	add	sp, sp, #0x50
    1da4: d50323bf     	autiasp
    1da8: 1400014e     	b	0x22e0 <free@plt>
