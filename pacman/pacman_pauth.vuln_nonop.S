
pacman_pauth:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000001ca8 <vulnerable_func>:
    1ca8: d503233f     	paciasp
    1cac: d10183ff     	sub	sp, sp, #0x60
    1cb0: a9017bfd     	stp	x29, x30, [sp, #0x10]
    1cb4: a90267fa     	stp	x26, x25, [sp, #0x20]
    1cb8: a9035ff8     	stp	x24, x23, [sp, #0x30]
    1cbc: a90457f6     	stp	x22, x21, [sp, #0x40]
    1cc0: a9054ff4     	stp	x20, x19, [sp, #0x50]
    1cc4: 910043fd     	add	x29, sp, #0x10
    1cc8: aa0003f5     	mov	x21, x0
    1ccc: 52800800     	mov	w0, #0x40               // =64
    1cd0: 2a0403f4     	mov	w20, w4
    1cd4: 2a0303f8     	mov	w24, w3
    1cd8: aa0203f7     	mov	x23, x2
    1cdc: aa0103f6     	mov	x22, x1
    1ce0: 9400017e     	bl	0x22d8 <malloc@plt>
    1ce4: b4000780     	cbz	x0, 0x1dd4 <vulnerable_func+0x12c>
    1ce8: 3dc002e0     	ldr	q0, [x23]
    1cec: aa0003f3     	mov	x19, x0
    1cf0: 3900601f     	strb	wzr, [x0, #0x18]
    1cf4: 3d800000     	str	q0, [x0]
    1cf8: 52800300     	mov	w0, #0x18               // =24
    1cfc: 94000177     	bl	0x22d8 <malloc@plt>
    1d00: 93407f08     	sxtw	x8, w24
    1d04: 9b167d1a     	mul	x26, x8, x22
    1d08: aa0003f6     	mov	x22, x0
    1d0c: 8b1a02b9     	add	x25, x21, x26
    1d10: dac12bf9     	pacdza	x25
    1d14: f9000819     	str	x25, [x0, #0x10]
    1d18: d503201f     	nop
    1d1c: 10ff63e0     	adr	x0, 0x998 <main.prefix_bytes+0x30>
    1d20: aa1903e1     	mov	x1, x25
    1d24: 94000173     	bl	0x22f0 <printf@plt>
    1d28: f9000a79     	str	x25, [x19, #0x10]
    1d2c: d5033b9f     	dsb	ish
    1d30: d5033fdf     	isb
    1d34: b81fc3bf     	stur	wzr, [x29, #-0x4]
    1d38: 340000b8     	cbz	w24, 0x1d4c <vulnerable_func+0xa4>
    1d3c: 3dc00260     	ldr	q0, [x19]
    1d40: f9400a68     	ldr	x8, [x19, #0x10]
    1d44: 3d8002c0     	str	q0, [x22]
    1d48: f9000ac8     	str	x8, [x22, #0x10]
    1d4c: d50b7e37     	dc	civac, x23
    1d50: aa1703e0     	mov	x0, x23
    1d54: d5033b9f     	dsb	ish
    1d58: d5033fdf     	isb
    1d5c: d5033b9f     	dsb	ish
    1d60: d5033fdf     	isb
    1d64: 94000169     	bl	0x2308 <strlen@plt>
    1d68: f100241f     	cmp	x0, #0x9
    1d6c: 54000268     	b.hi	0x1db8 <vulnerable_func+0x110>
    1d70: f9400ac8     	ldr	x8, [x22, #0x10]
    1d74: 7100069f     	cmp	w20, #0x1
    1d78: dac13be8     	autdza	x8
    1d7c: b9400109     	ldr	w9, [x8]
    1d80: b85fc3aa     	ldur	w10, [x29, #-0x4]
    1d84: d37ffb48     	lsl	x8, x26, #1
    1d88: 4a090149     	eor	w9, w10, w9
    1d8c: b81fc3a9     	stur	w9, [x29, #-0x4]
    1d90: 540000ab     	b.lt	0x1da4 <vulnerable_func+0xfc>
    1d94: aa1f03e9     	mov	x9, xzr
    1d98: 71000694     	subs	w20, w20, #0x1
    1d9c: aa090129     	orr	x9, x9, x9
    1da0: 54ffffc1     	b.ne	0x1d98 <vulnerable_func+0xf0>
    1da4: b8686aa8     	ldr	w8, [x21, x8]
    1da8: b85fc3a9     	ldur	w9, [x29, #-0x4]
    1dac: 4a080128     	eor	w8, w9, w8
    1db0: b81fc3a8     	stur	w8, [x29, #-0x4]
    1db4: 14000004     	b	0x1dc4 <vulnerable_func+0x11c>
    1db8: f0ffffe0     	adrp	x0, 0x0 <sysconf>
    1dbc: 91270000     	add	x0, x0, #0x9c0
    1dc0: 94000158     	bl	0x2320 <puts@plt>
    1dc4: aa1303e0     	mov	x0, x19
    1dc8: 9400015c     	bl	0x2338 <free@plt>
    1dcc: aa1603e0     	mov	x0, x22
    1dd0: 9400015a     	bl	0x2338 <free@plt>
    1dd4: a9454ff4     	ldp	x20, x19, [sp, #0x50]
    1dd8: a94457f6     	ldp	x22, x21, [sp, #0x40]
    1ddc: a9435ff8     	ldp	x24, x23, [sp, #0x30]
    1de0: a94267fa     	ldp	x26, x25, [sp, #0x20]
    1de4: a9417bfd     	ldp	x29, x30, [sp, #0x10]
    1de8: 910183ff     	add	sp, sp, #0x60
    1dec: d65f0bff     	retaa
