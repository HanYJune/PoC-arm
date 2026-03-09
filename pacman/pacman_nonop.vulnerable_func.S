
pacman_nonop:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000001ca0 <vulnerable_func>:
    1ca0: d503233f     	paciasp
    1ca4: d10143ff     	sub	sp, sp, #0x50
    1ca8: a9017bfd     	stp	x29, x30, [sp, #0x10]
    1cac: a9025ff8     	stp	x24, x23, [sp, #0x20]
    1cb0: a90357f6     	stp	x22, x21, [sp, #0x30]
    1cb4: a9044ff4     	stp	x20, x19, [sp, #0x40]
    1cb8: 910043fd     	add	x29, sp, #0x10
    1cbc: aa0003f4     	mov	x20, x0
    1cc0: 52800300     	mov	w0, #0x18               // =24
    1cc4: 2a0403f3     	mov	w19, w4
    1cc8: 2a0303f7     	mov	w23, w3
    1ccc: aa0203f6     	mov	x22, x2
    1cd0: aa0103f5     	mov	x21, x1
    1cd4: 94000171     	bl	0x2298 <malloc@plt>
    1cd8: 93407ee8     	sxtw	x8, w23
    1cdc: 9b157d18     	mul	x24, x8, x21
    1ce0: aa0003f5     	mov	x21, x0
    1ce4: 8b180281     	add	x1, x20, x24
    1ce8: dac12be1     	pacdza	x1
    1cec: f9000801     	str	x1, [x0, #0x10]
    1cf0: d503201f     	nop
    1cf4: 10ff6520     	adr	x0, 0x998 <main.prefix_bytes+0x30>
    1cf8: 9400016e     	bl	0x22b0 <printf@plt>
    1cfc: d5033b9f     	dsb	ish
    1d00: d5033fdf     	isb
    1d04: b81fc3bf     	stur	wzr, [x29, #-0x4]
    1d08: 340000b7     	cbz	w23, 0x1d1c <vulnerable_func+0x7c>
    1d0c: 3dc002c0     	ldr	q0, [x22]
    1d10: f9400ac8     	ldr	x8, [x22, #0x10]
    1d14: 3d8002a0     	str	q0, [x21]
    1d18: f9000aa8     	str	x8, [x21, #0x10]
    1d1c: d50b7e36     	dc	civac, x22
    1d20: aa1603e0     	mov	x0, x22
    1d24: d5033b9f     	dsb	ish
    1d28: d5033fdf     	isb
    1d2c: d5033b9f     	dsb	ish
    1d30: d5033fdf     	isb
    1d34: 94000165     	bl	0x22c8 <strlen@plt>
    1d38: f100241f     	cmp	x0, #0x9
    1d3c: 54000268     	b.hi	0x1d88 <vulnerable_func+0xe8>
    1d40: f9400aa8     	ldr	x8, [x21, #0x10]
    1d44: 7100067f     	cmp	w19, #0x1
    1d48: dac13be8     	autdza	x8
    1d4c: b9400109     	ldr	w9, [x8]
    1d50: b85fc3aa     	ldur	w10, [x29, #-0x4]
    1d54: d37ffb08     	lsl	x8, x24, #1
    1d58: 4a090149     	eor	w9, w10, w9
    1d5c: b81fc3a9     	stur	w9, [x29, #-0x4]
    1d60: 540000ab     	b.lt	0x1d74 <vulnerable_func+0xd4>
    1d64: aa1f03e9     	mov	x9, xzr
    1d68: 71000673     	subs	w19, w19, #0x1
    1d6c: aa090129     	orr	x9, x9, x9
    1d70: 54ffffc1     	b.ne	0x1d68 <vulnerable_func+0xc8>
    1d74: b8686a88     	ldr	w8, [x20, x8]
    1d78: b85fc3a9     	ldur	w9, [x29, #-0x4]
    1d7c: 4a080128     	eor	w8, w9, w8
    1d80: b81fc3a8     	stur	w8, [x29, #-0x4]
    1d84: 14000004     	b	0x1d94 <vulnerable_func+0xf4>
    1d88: f0ffffe0     	adrp	x0, 0x0 <sysconf>
    1d8c: 91270000     	add	x0, x0, #0x9c0
    1d90: 94000154     	bl	0x22e0 <puts@plt>
    1d94: aa1503e0     	mov	x0, x21
    1d98: a9444ff4     	ldp	x20, x19, [sp, #0x40]
    1d9c: a94357f6     	ldp	x22, x21, [sp, #0x30]
    1da0: a9425ff8     	ldp	x24, x23, [sp, #0x20]
    1da4: a9417bfd     	ldp	x29, x30, [sp, #0x10]
    1da8: 910143ff     	add	sp, sp, #0x50
    1dac: d50323bf     	autiasp
    1db0: 14000152     	b	0x22f8 <free@plt>
