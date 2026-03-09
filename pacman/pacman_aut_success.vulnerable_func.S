
pacman:     file format elf64-littleaarch64


Disassembly of section .text:

0000000000003000 <vulnerable_func>:
    3000:	d503233f 	paciasp
    3004:	a9bb7bfd 	stp	x29, x30, [sp, #-80]!
    3008:	f9000bf9 	str	x25, [sp, #16]
    300c:	a9025ff8 	stp	x24, x23, [sp, #32]
    3010:	a90357f6 	stp	x22, x21, [sp, #48]
    3014:	a9044ff4 	stp	x20, x19, [sp, #64]
    3018:	910003fd 	mov	x29, sp
    301c:	aa0003f4 	mov	x20, x0
    3020:	aa0203e0 	mov	x0, x2
    3024:	2a0403f3 	mov	w19, w4
    3028:	2a0303f8 	mov	w24, w3
    302c:	aa0203f6 	mov	x22, x2
    3030:	aa0103f5 	mov	x21, x1
    3034:	94000151 	bl	3578 <malloc@plt+0x8>
    3038:	94000156 	bl	3590 <free@plt>
    303c:	3dc002c0 	ldr	q0, [x22]
    3040:	f9400ac8 	ldr	x8, [x22, #16]
    3044:	aa0003f7 	mov	x23, x0
    3048:	3d800000 	str	q0, [x0]
    304c:	f9000808 	str	x8, [x0, #16]
    3050:	52800300 	mov	w0, #0x18                  	// #24
    3054:	9400014f 	bl	3590 <free@plt>
    3058:	93407f08 	sxtw	x8, w24
    305c:	9b157d19 	mul	x25, x8, x21
    3060:	aa0003f5 	mov	x21, x0
    3064:	8b190281 	add	x1, x20, x25
    3068:	dac12be1 	pacdza	x1
    306c:	f9000801 	str	x1, [x0, #16]
    3070:	d503201f 	nop
    3074:	10fec7a0 	adr	x0, 968 <main.prefix_bytes+0x30>
    3078:	9400014c 	bl	35a8 <__errno@plt+0x8>
    307c:	d5033b9f 	dsb	ish
    3080:	d5033fdf 	isb
    3084:	b9001fbf 	str	wzr, [x29, #28]
    3088:	340000b8 	cbz	w24, 309c <vulnerable_func+0x9c>
    308c:	3dc002e0 	ldr	q0, [x23]
    3090:	f9400ae8 	ldr	x8, [x23, #16]
    3094:	3d8002a0 	str	q0, [x21]
    3098:	f9000aa8 	str	x8, [x21, #16]
    309c:	d50b7e36 	dc	civac, x22
    30a0:	aa1603e0 	mov	x0, x22
    30a4:	d5033b9f 	dsb	ish
    30a8:	d5033fdf 	isb
    30ac:	d5033b9f 	dsb	ish
    30b0:	d5033fdf 	isb
    30b4:	94000131 	bl	3578 <malloc@plt+0x8>
    30b8:	f100241f 	cmp	x0, #0x9
    30bc:	54000268 	b.hi	3108 <vulnerable_func+0x108>  // b.pmore
    30c0:	f9400aa8 	ldr	x8, [x21, #16]
    30c4:	dac13be8 	autdza	x8
    30c8:	f9000aa8 	str	x8, [x21, #16]
    30cc:	b9400109 	ldr	w9, [x8]
    30d0:	b9401faa 	ldr	w10, [x29, #28]
    30d4:	4a090149 	eor	w9, w10, w9
    30d8:	b9001fa9 	str	w9, [x29, #28]
    30dc:	b40000f4 	cbz	x20, 30f8 <vulnerable_func+0xf8>
    30e0:	7100067f 	cmp	w19, #0x1
    30e4:	540000ab 	b.lt	30f8 <vulnerable_func+0xf8>  // b.tstop
    30e8:	f9400289 	ldr	x9, [x20]
    30ec:	71000673 	subs	w19, w19, #0x1
    30f0:	91010294 	add	x20, x20, #0x40
    30f4:	54ffffa1 	b.ne	30e8 <vulnerable_func+0xe8>  // b.any
    30f8:	b8796908 	ldr	w8, [x8, x25]
    30fc:	b9401fa9 	ldr	w9, [x29, #28]
    3100:	4a080128 	eor	w8, w9, w8
    3104:	b9001fa8 	str	w8, [x29, #28]
    3108:	aa1503e0 	mov	x0, x21
    310c:	a9444ff4 	ldp	x20, x19, [sp, #64]
    3110:	f9400bf9 	ldr	x25, [sp, #16]
    3114:	a94357f6 	ldp	x22, x21, [sp, #48]
    3118:	a9425ff8 	ldp	x24, x23, [sp, #32]
    311c:	a8c57bfd 	ldp	x29, x30, [sp], #80
    3120:	d50323bf 	autiasp
    3124:	14000127 	b	35c0 <fprintf@plt>

Disassembly of section .plt:
