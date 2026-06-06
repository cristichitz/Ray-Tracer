	.text
	.intel_syntax noprefix
	.file	"render_cpu2.c"
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function get_ray_color
.LCPI0_0:
	.long	0x3a83126f                      # float 0.00100000005
.LCPI0_1:
	.long	0x7f800000                      # float +Inf
	.text
	.globl	get_ray_color
	.p2align	4, 0x90
	.type	get_ray_color,@function
get_ray_color:                          # @get_ray_color
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	push	r15
	.cfi_def_cfa_offset 24
	push	r14
	.cfi_def_cfa_offset 32
	push	r12
	.cfi_def_cfa_offset 40
	push	rbx
	.cfi_def_cfa_offset 48
	sub	rsp, 336
	.cfi_def_cfa_offset 384
	.cfi_offset rbx, -48
	.cfi_offset r12, -40
	.cfi_offset r14, -32
	.cfi_offset r15, -24
	.cfi_offset rbp, -16
	mov	qword ptr [rsp + 248], offset ft_set_face_normal
	inc	qword ptr [rip + g_ray_count]
	test	edi, edi
	jle	.LBB0_1
# %bb.2:
	mov	ebp, edi
	lea	r12, [rsp + 432]
	lea	r14, [rsp + 384]
	mov	rbx, qword ptr [r14 + 24]
	lea	rdi, [rsp + 288]
	vmovss	xmm0, dword ptr [rip + .LCPI0_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI0_1] # xmm1 = mem[0],zero,zero,zero
	call	interval_init
	vmovups	ymm0, ymmword ptr [rsp + 288]
	vmovups	ymm1, ymmword ptr [rsp + 304]
	vmovups	ymmword ptr [rsp + 48], ymm1
	vmovups	ymmword ptr [rsp + 32], ymm0
	vmovups	ymm0, ymmword ptr [r12]
	vmovups	ymmword ptr [rsp], ymm0
	lea	rsi, [rsp + 168]
	mov	rdi, r14
	vzeroupper
	call	rbx
	test	al, al
	je	.LBB0_3
# %bb.4:
	lea	r15, [rsp + 192]
	vmovss	xmm0, dword ptr [rsp + 236]     # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rsp + 240]     # xmm1 = mem[0],zero,zero,zero
	vmovsd	xmm2, qword ptr [rsp + 168]     # xmm2 = mem[0],zero
	vmovss	xmm3, dword ptr [rsp + 176]     # xmm3 = mem[0],zero,zero,zero
	mov	rdi, r15
	call	qword ptr [rsp + 224]
	vmovaps	xmmword ptr [rsp + 144], xmm0   # 16-byte Spill
	vmovss	dword ptr [rsp + 124], xmm1     # 4-byte Spill
	vmovups	ymm0, ymmword ptr [rsp + 168]
	vmovups	ymm1, ymmword ptr [rsp + 200]
	vmovups	ymm2, ymmword ptr [rsp + 224]
	vmovups	ymmword ptr [rsp + 88], ymm2
	vmovups	ymmword ptr [rsp + 64], ymm1
	vmovups	ymmword ptr [rsp + 32], ymm0
	vmovups	ymm0, ymmword ptr [r12]
	vmovups	ymmword ptr [rsp], ymm0
	lea	rsi, [rsp + 128]
	lea	rdx, [rsp + 256]
	mov	rdi, r15
	vzeroupper
	call	qword ptr [rsp + 216]
	test	al, al
	je	.LBB0_5
# %bb.6:
	dec	ebp
	vmovups	ymm0, ymmword ptr [rsp + 256]
	vmovups	ymmword ptr [rsp + 48], ymm0
	vmovups	ymm0, ymmword ptr [r14]
	vmovups	ymm1, ymmword ptr [r14 + 16]
	vmovups	ymmword ptr [rsp + 16], ymm1
	vmovups	ymmword ptr [rsp], ymm0
	mov	edi, ebp
	vzeroupper
	call	get_ray_color
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	vmovsd	xmm0, qword ptr [rsp + 128]     # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [rsp + 136]     # xmm1 = mem[0],zero,zero,zero
	call	mult
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	vmovaps	xmm0, xmmword ptr [rsp + 144]   # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 124]     # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	add
	jmp	.LBB0_7
.LBB0_1:
	vxorps	xmm0, xmm0, xmm0
	vxorps	xmm1, xmm1, xmm1
	vxorps	xmm2, xmm2, xmm2
	call	make_vec
	jmp	.LBB0_7
.LBB0_3:
	vmovsd	xmm0, qword ptr [r14 + 32]      # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [r14 + 40]      # xmm1 = mem[0],zero,zero,zero
	jmp	.LBB0_7
.LBB0_5:
	vmovaps	xmm0, xmmword ptr [rsp + 144]   # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 124]     # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
.LBB0_7:
	add	rsp, 336
	.cfi_def_cfa_offset 48
	pop	rbx
	.cfi_def_cfa_offset 40
	pop	r12
	.cfi_def_cfa_offset 32
	pop	r14
	.cfi_def_cfa_offset 24
	pop	r15
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end0:
	.size	get_ray_color, .Lfunc_end0-get_ray_color
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function sample_square
.LCPI1_0:
	.long	0x3f800000                      # float 1
.LCPI1_1:
	.long	0xbf000000                      # float -0.5
	.text
	.globl	sample_square
	.p2align	4, 0x90
	.type	sample_square,@function
sample_square:                          # @sample_square
	.cfi_startproc
# %bb.0:
	push	rax
	.cfi_def_cfa_offset 16
	vmovss	xmm1, dword ptr [rip + .LCPI1_0] # xmm1 = mem[0],zero,zero,zero
	vxorps	xmm0, xmm0, xmm0
	call	random_float
	vaddss	xmm0, xmm0, dword ptr [rip + .LCPI1_1]
	vmovss	dword ptr [rsp + 4], xmm0       # 4-byte Spill
	vxorps	xmm0, xmm0, xmm0
	vmovss	xmm1, dword ptr [rip + .LCPI1_0] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vaddss	xmm1, xmm0, dword ptr [rip + .LCPI1_1]
	vxorps	xmm2, xmm2, xmm2
	vmovss	xmm0, dword ptr [rsp + 4]       # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	pop	rax
	.cfi_def_cfa_offset 8
	jmp	make_vec                        # TAILCALL
.Lfunc_end1:
	.size	sample_square, .Lfunc_end1-sample_square
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function get_ray
.LCPI2_0:
	.long	0x3f800000                      # float 1
.LCPI2_1:
	.long	0xbf000000                      # float -0.5
	.text
	.globl	get_ray
	.p2align	4, 0x90
	.type	get_ray,@function
get_ray:                                # @get_ray
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	push	r15
	.cfi_def_cfa_offset 24
	push	r14
	.cfi_def_cfa_offset 32
	push	rbx
	.cfi_def_cfa_offset 40
	sub	rsp, 56
	.cfi_def_cfa_offset 96
	.cfi_offset rbx, -40
	.cfi_offset r14, -32
	.cfi_offset r15, -24
	.cfi_offset rbp, -16
	mov	r15d, ecx
	mov	ebp, edx
	mov	rbx, rsi
	mov	r14, rdi
	vmovss	xmm1, dword ptr [rip + .LCPI2_0] # xmm1 = mem[0],zero,zero,zero
	vxorps	xmm0, xmm0, xmm0
	call	random_float
	vaddss	xmm0, xmm0, dword ptr [rip + .LCPI2_1]
	vmovss	dword ptr [rsp], xmm0           # 4-byte Spill
	vxorps	xmm0, xmm0, xmm0
	vmovss	xmm1, dword ptr [rip + .LCPI2_0] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vaddss	xmm1, xmm0, dword ptr [rip + .LCPI2_1]
	vxorps	xmm2, xmm2, xmm2
	vmovss	xmm0, dword ptr [rsp]           # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	call	make_vec
	vmovaps	xmmword ptr [rsp + 32], xmm0    # 16-byte Spill
	mov	eax, ebp
	vcvtsi2ss	xmm1, xmm3, rax
	vaddss	xmm2, xmm0, xmm1
	vmovsd	xmm0, qword ptr [rbx + 188]     # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [rbx + 196]     # xmm1 = mem[0],zero,zero,zero
	call	scale
	mov	eax, r15d
	vcvtsi2ss	xmm2, xmm3, rax
	vmovaps	xmmword ptr [rsp + 16], xmm0    # 16-byte Spill
	vmovss	dword ptr [rsp], xmm1           # 4-byte Spill
	vmovshdup	xmm0, xmmword ptr [rsp + 32] # 16-byte Folded Reload
                                        # xmm0 = mem[1,1,3,3]
	vaddss	xmm2, xmm0, xmm2
	vmovsd	xmm0, qword ptr [rbx + 200]     # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [rbx + 208]     # xmm1 = mem[0],zero,zero,zero
	call	scale
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	vmovaps	xmm0, xmmword ptr [rsp + 16]    # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp]           # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	add
	vmovsd	xmm2, qword ptr [rbx + 152]     # xmm2 = mem[0],zero
	vmovss	xmm3, dword ptr [rbx + 160]     # xmm3 = mem[0],zero,zero,zero
	call	add
	vmovsd	xmm2, qword ptr [rbx + 140]     # xmm2 = mem[0],zero
	vmovaps	xmmword ptr [rsp], xmm2         # 16-byte Spill
	vmovss	xmm3, dword ptr [rbx + 148]     # xmm3 = mem[0],zero,zero,zero
	vmovss	dword ptr [rsp + 16], xmm3      # 4-byte Spill
	call	sub
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	mov	rdi, r14
	vmovaps	xmm0, xmmword ptr [rsp]         # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 16]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	make_ray
	mov	rax, r14
	add	rsp, 56
	.cfi_def_cfa_offset 40
	pop	rbx
	.cfi_def_cfa_offset 32
	pop	r14
	.cfi_def_cfa_offset 24
	pop	r15
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end2:
	.size	get_ray, .Lfunc_end2-get_ray
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function write_color
.LCPI3_0:
	.long	0x3f7fbe77                      # float 0.999000012
.LCPI3_1:
	.long	0x43800000                      # float 256
.LCPI3_2:
	.long	0x00000000                      # float 0
	.text
	.globl	write_color
	.p2align	4, 0x90
	.type	write_color,@function
write_color:                            # @write_color
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	push	r15
	.cfi_def_cfa_offset 24
	push	r14
	.cfi_def_cfa_offset 32
	push	r13
	.cfi_def_cfa_offset 40
	push	r12
	.cfi_def_cfa_offset 48
	push	rbx
	.cfi_def_cfa_offset 56
	sub	rsp, 136
	.cfi_def_cfa_offset 192
	.cfi_offset rbx, -56
	.cfi_offset r12, -48
	.cfi_offset r13, -40
	.cfi_offset r14, -32
	.cfi_offset r15, -24
	.cfi_offset rbp, -16
	vmovss	dword ptr [rsp + 12], xmm1      # 4-byte Spill
	vmovaps	xmmword ptr [rsp + 16], xmm0    # 16-byte Spill
	mov	r15d, edx
	mov	r14d, esi
	mov	r12, rdi
	lea	rdi, [rsp + 88]
	vmovss	xmm1, dword ptr [rip + .LCPI3_0] # xmm1 = mem[0],zero,zero,zero
	vxorps	xmm0, xmm0, xmm0
	call	interval_init
	vmovups	ymm0, ymmword ptr [rsp + 88]
	vmovups	ymm1, ymmword ptr [rsp + 104]
	vmovups	ymmword ptr [rsp + 48], ymm1
	vmovups	ymmword ptr [rsp + 32], ymm0
	mov	rbx, qword ptr [rsp + 64]
	vmovaps	xmm0, xmmword ptr [rsp + 16]    # 16-byte Reload
	vucomiss	xmm0, dword ptr [.LCPI3_2]
	jb	.LBB3_2
# %bb.1:
	vmovaps	xmm0, xmmword ptr [rsp + 16]    # 16-byte Reload
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB3_3
.LBB3_2:
	vmovaps	xmm0, xmmword ptr [rsp + 16]    # 16-byte Reload
	vzeroupper
	call	sqrtf
.LBB3_3:
	lea	rdi, [rsp + 32]
	vzeroupper
	call	rbx
	vmulss	xmm0, xmm0, dword ptr [rip + .LCPI3_1]
	vcvttss2si	rbp, xmm0
	mov	rbx, qword ptr [rsp + 64]
	vmovshdup	xmm0, xmmword ptr [rsp + 16] # 16-byte Folded Reload
                                        # xmm0 = mem[1,1,3,3]
	vucomiss	xmm0, dword ptr [.LCPI3_2]
	jb	.LBB3_5
# %bb.4:
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB3_6
.LBB3_5:
	call	sqrtf
.LBB3_6:
	lea	rdi, [rsp + 32]
	call	rbx
	vmulss	xmm0, xmm0, dword ptr [rip + .LCPI3_1]
	vcvttss2si	rbx, xmm0
	mov	r13, qword ptr [rsp + 64]
	vxorps	xmm1, xmm1, xmm1
	vmovss	xmm0, dword ptr [rsp + 12]      # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	vucomiss	xmm0, xmm1
	jb	.LBB3_8
# %bb.7:
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB3_9
.LBB3_8:
	call	sqrtf
.LBB3_9:
	lea	rdi, [rsp + 32]
	call	r13
	vmulss	xmm0, xmm0, dword ptr [rip + .LCPI3_1]
	vcvttss2si	rcx, xmm0
	shl	ebp, 24
	shl	ebx, 16
	shl	ecx, 8
	or	ecx, ebp
	or	ecx, ebx
	or	ecx, 255
	cmp	byte ptr [r12 + 24], 0
	je	.LBB3_11
# %bb.10:
	mov	rax, qword ptr [r12 + 16]
	mov	edx, r15d
	mov	esi, dword ptr [r12 + 28]
	imul	rsi, rdx
	mov	edx, r14d
	add	rdx, rsi
	mov	dword ptr [rax + 4*rdx], ecx
	jmp	.LBB3_12
.LBB3_11:
	mov	rdi, qword ptr [r12 + 8]
	mov	esi, r14d
	mov	edx, r15d
                                        # kill: def $ecx killed $ecx killed $rcx
	call	mlx_put_pixel
.LBB3_12:
	add	rsp, 136
	.cfi_def_cfa_offset 56
	pop	rbx
	.cfi_def_cfa_offset 48
	pop	r12
	.cfi_def_cfa_offset 40
	pop	r13
	.cfi_def_cfa_offset 32
	pop	r14
	.cfi_def_cfa_offset 24
	pop	r15
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end3:
	.size	write_color, .Lfunc_end3-write_color
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function render_frame
.LCPI4_0:
	.long	0x3f800000                      # float 1
.LCPI4_1:
	.long	0xbf000000                      # float -0.5
.LCPI4_2:
	.long	0x3f7fbe77                      # float 0.999000012
.LCPI4_3:
	.long	0x43800000                      # float 256
.LCPI4_4:
	.long	0x00000000                      # float 0
	.text
	.globl	render_frame
	.p2align	4, 0x90
	.type	render_frame,@function
render_frame:                           # @render_frame
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	push	r15
	.cfi_def_cfa_offset 24
	push	r14
	.cfi_def_cfa_offset 32
	push	r13
	.cfi_def_cfa_offset 40
	push	r12
	.cfi_def_cfa_offset 48
	push	rbx
	.cfi_def_cfa_offset 56
	sub	rsp, 328
	.cfi_def_cfa_offset 384
	.cfi_offset rbx, -56
	.cfi_offset r12, -48
	.cfi_offset r13, -40
	.cfi_offset r14, -32
	.cfi_offset r15, -24
	.cfi_offset rbp, -16
	mov	ecx, dword ptr [rdi + 32]
	test	ecx, ecx
	je	.LBB4_10
# %bb.1:
	mov	rbx, rdi
	lea	r12, [rdi + 216]
	mov	eax, dword ptr [rdi + 28]
	xor	edx, edx
	mov	qword ptr [rsp + 88], rdx       # 8-byte Spill
	jmp	.LBB4_2
	.p2align	4, 0x90
.LBB4_8:                                #   in Loop: Header=BB4_2 Depth=1
	mov	ecx, dword ptr [rbx + 32]
.LBB4_9:                                #   in Loop: Header=BB4_2 Depth=1
	mov	rdi, qword ptr [rsp + 88]       # 8-byte Reload
	inc	rdi
	mov	edx, ecx
	mov	rsi, rdi
	mov	qword ptr [rsp + 88], rdi       # 8-byte Spill
	cmp	rdi, rdx
	jae	.LBB4_10
.LBB4_2:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB4_4 Depth 2
                                        #       Child Loop BB4_23 Depth 3
	test	eax, eax
	mov	eax, 0
	je	.LBB4_9
# %bb.3:                                #   in Loop: Header=BB4_2 Depth=1
	mov	eax, dword ptr [rsp + 88]       # 4-byte Reload
	vcvtsi2ss	xmm0, xmm4, rax
	xor	ebp, ebp
	vmovss	dword ptr [rsp + 120], xmm0     # 4-byte Spill
	jmp	.LBB4_4
	.p2align	4, 0x90
.LBB4_20:                               #   in Loop: Header=BB4_4 Depth=2
	mov	rdi, qword ptr [rbx + 8]
	mov	rbp, qword ptr [rsp + 152]      # 8-byte Reload
	mov	esi, ebp
	mov	rdx, qword ptr [rsp + 88]       # 8-byte Reload
                                        # kill: def $edx killed $edx killed $rdx
                                        # kill: def $ecx killed $ecx killed $rcx
	call	mlx_put_pixel
.LBB4_21:                               #   in Loop: Header=BB4_4 Depth=2
	inc	rbp
	mov	eax, dword ptr [rbx + 28]
	cmp	rbp, rax
	jae	.LBB4_8
.LBB4_4:                                #   Parent Loop BB4_2 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB4_23 Depth 3
	vxorps	xmm0, xmm0, xmm0
	vxorps	xmm1, xmm1, xmm1
	vxorps	xmm2, xmm2, xmm2
	call	make_vec
	vmovaps	xmm3, xmm0
	cmp	dword ptr [rbx + 40], 0
	mov	qword ptr [rsp + 152], rbp      # 8-byte Spill
	je	.LBB4_5
# %bb.22:                               #   in Loop: Header=BB4_4 Depth=2
	mov	eax, ebp
	vcvtsi2ss	xmm0, xmm4, rax
	vmovss	dword ptr [rsp + 124], xmm0     # 4-byte Spill
	xor	r15d, r15d
	lea	rbp, [rsp + 176]
	.p2align	4, 0x90
.LBB4_23:                               #   Parent Loop BB4_2 Depth=1
                                        #     Parent Loop BB4_4 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	vmovaps	xmmword ptr [rsp + 160], xmm3   # 16-byte Spill
	vmovss	dword ptr [rsp + 96], xmm1      # 4-byte Spill
	vxorps	xmm0, xmm0, xmm0
	vmovss	xmm1, dword ptr [rip + .LCPI4_0] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovss	xmm1, dword ptr [rip + .LCPI4_1] # xmm1 = mem[0],zero,zero,zero
	vaddss	xmm0, xmm0, xmm1
	vmovss	dword ptr [rsp + 128], xmm0     # 4-byte Spill
	vxorps	xmm0, xmm0, xmm0
	vmovss	xmm1, dword ptr [rip + .LCPI4_0] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vaddss	xmm1, xmm0, dword ptr [rip + .LCPI4_1]
	vxorps	xmm2, xmm2, xmm2
	vmovss	xmm0, dword ptr [rsp + 128]     # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	call	make_vec
	vmovaps	xmmword ptr [rsp + 224], xmm0   # 16-byte Spill
	vaddss	xmm2, xmm0, dword ptr [rsp + 124] # 4-byte Folded Reload
	vmovsd	xmm0, qword ptr [rbx + 188]     # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [rbx + 196]     # xmm1 = mem[0],zero,zero,zero
	call	scale
	vmovaps	xmmword ptr [rsp + 128], xmm0   # 16-byte Spill
	vmovss	dword ptr [rsp + 84], xmm1      # 4-byte Spill
	vmovshdup	xmm0, xmmword ptr [rsp + 224] # 16-byte Folded Reload
                                        # xmm0 = mem[1,1,3,3]
	vaddss	xmm2, xmm0, dword ptr [rsp + 120] # 4-byte Folded Reload
	vmovsd	xmm0, qword ptr [rbx + 200]     # xmm0 = mem[0],zero
	vmovss	xmm1, dword ptr [rbx + 208]     # xmm1 = mem[0],zero,zero,zero
	call	scale
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	vmovaps	xmm0, xmmword ptr [rsp + 128]   # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 84]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	add
	vmovsd	xmm2, qword ptr [rbx + 152]     # xmm2 = mem[0],zero
	vmovss	xmm3, dword ptr [rbx + 160]     # xmm3 = mem[0],zero,zero,zero
	call	add
	vmovsd	xmm2, qword ptr [rbx + 140]     # xmm2 = mem[0],zero
	vmovaps	xmmword ptr [rsp + 128], xmm2   # 16-byte Spill
	vmovss	xmm3, dword ptr [rbx + 148]     # xmm3 = mem[0],zero,zero,zero
	vmovss	dword ptr [rsp + 84], xmm3      # 4-byte Spill
	call	sub
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	mov	rdi, rbp
	vmovaps	xmm0, xmmword ptr [rsp + 128]   # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 84]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	make_ray
	vmovups	ymm0, ymmword ptr [rsp + 176]
	vmovups	ymmword ptr [rsp + 240], ymm0
	mov	edi, dword ptr [rbx + 36]
	vmovups	ymm0, ymmword ptr [rsp + 240]
	vmovups	ymmword ptr [rsp + 48], ymm0
	vmovups	ymm0, ymmword ptr [r12]
	vmovups	ymm1, ymmword ptr [r12 + 16]
	vmovups	ymmword ptr [rsp + 16], ymm1
	vmovups	ymmword ptr [rsp], ymm0
	vzeroupper
	call	get_ray_color
	vmovaps	xmm2, xmm0
	vmovaps	xmm3, xmm1
	vmovaps	xmm0, xmmword ptr [rsp + 160]   # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 96]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	add
	vmovaps	xmm3, xmm0
	inc	r15d
	cmp	r15d, dword ptr [rbx + 40]
	jb	.LBB4_23
	jmp	.LBB4_6
	.p2align	4, 0x90
.LBB4_5:                                #   in Loop: Header=BB4_4 Depth=2
	lea	rbp, [rsp + 176]
.LBB4_6:                                #   in Loop: Header=BB4_4 Depth=2
	vmovss	xmm2, dword ptr [rbx + 44]      # xmm2 = mem[0],zero,zero,zero
	vmovaps	xmm0, xmm3
	call	scale
	vmovaps	xmmword ptr [rsp + 96], xmm0    # 16-byte Spill
	vmovss	dword ptr [rsp + 160], xmm1     # 4-byte Spill
	vxorps	xmm0, xmm0, xmm0
	lea	rdi, [rsp + 280]
	vmovss	xmm1, dword ptr [rip + .LCPI4_2] # xmm1 = mem[0],zero,zero,zero
	call	interval_init
	vmovups	ymm0, ymmword ptr [rsp + 280]
	vmovups	ymm1, ymmword ptr [rsp + 296]
	vmovups	ymmword ptr [rsp + 192], ymm1
	vmovups	ymmword ptr [rsp + 176], ymm0
	mov	r14, qword ptr [rsp + 208]
	vmovaps	xmm0, xmmword ptr [rsp + 96]    # 16-byte Reload
	vucomiss	xmm0, dword ptr [.LCPI4_4]
	jb	.LBB4_11
# %bb.7:                                #   in Loop: Header=BB4_4 Depth=2
	vmovaps	xmm0, xmmword ptr [rsp + 96]    # 16-byte Reload
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB4_12
	.p2align	4, 0x90
.LBB4_11:                               #   in Loop: Header=BB4_4 Depth=2
	vmovaps	xmm0, xmmword ptr [rsp + 96]    # 16-byte Reload
	vzeroupper
	call	sqrtf
.LBB4_12:                               #   in Loop: Header=BB4_4 Depth=2
	mov	rdi, rbp
	vzeroupper
	call	r14
	vmulss	xmm1, xmm0, dword ptr [rip + .LCPI4_3]
	mov	r14, qword ptr [rsp + 208]
	vmovshdup	xmm0, xmmword ptr [rsp + 96] # 16-byte Folded Reload
                                        # xmm0 = mem[1,1,3,3]
	vucomiss	xmm0, dword ptr [.LCPI4_4]
	jb	.LBB4_14
# %bb.13:                               #   in Loop: Header=BB4_4 Depth=2
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB4_15
	.p2align	4, 0x90
.LBB4_14:                               #   in Loop: Header=BB4_4 Depth=2
	vmovss	dword ptr [rsp + 96], xmm1      # 4-byte Spill
	call	sqrtf
	vmovss	xmm1, dword ptr [rsp + 96]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
.LBB4_15:                               #   in Loop: Header=BB4_4 Depth=2
	vcvttss2si	r15, xmm1
	mov	rdi, rbp
	call	r14
	vmulss	xmm0, xmm0, dword ptr [rip + .LCPI4_3]
	vcvttss2si	r14, xmm0
	mov	r13, qword ptr [rsp + 208]
	vmovss	xmm0, dword ptr [rsp + 160]     # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	vucomiss	xmm0, dword ptr [.LCPI4_4]
	jb	.LBB4_17
# %bb.16:                               #   in Loop: Header=BB4_4 Depth=2
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB4_18
	.p2align	4, 0x90
.LBB4_17:                               #   in Loop: Header=BB4_4 Depth=2
	call	sqrtf
.LBB4_18:                               #   in Loop: Header=BB4_4 Depth=2
	mov	rdi, rbp
	call	r13
	vmulss	xmm0, xmm0, dword ptr [rip + .LCPI4_3]
	vcvttss2si	rcx, xmm0
	shl	r15d, 24
	shl	r14d, 16
	shl	ecx, 8
	or	ecx, r15d
	or	ecx, r14d
	or	ecx, 255
	cmp	byte ptr [rbx + 24], 0
	je	.LBB4_20
# %bb.19:                               #   in Loop: Header=BB4_4 Depth=2
	mov	rax, qword ptr [rbx + 16]
	mov	edx, dword ptr [rbx + 28]
	imul	rdx, qword ptr [rsp + 88]       # 8-byte Folded Reload
	mov	rbp, qword ptr [rsp + 152]      # 8-byte Reload
	add	rdx, rbp
	mov	dword ptr [rax + 4*rdx], ecx
	jmp	.LBB4_21
.LBB4_10:
	xor	eax, eax
	add	rsp, 328
	.cfi_def_cfa_offset 56
	pop	rbx
	.cfi_def_cfa_offset 48
	pop	r12
	.cfi_def_cfa_offset 40
	pop	r13
	.cfi_def_cfa_offset 32
	pop	r14
	.cfi_def_cfa_offset 24
	pop	r15
	.cfi_def_cfa_offset 16
	pop	rbp
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end4:
	.size	render_frame, .Lfunc_end4-render_frame
	.cfi_endproc
                                        # -- End function
	.ident	"Ubuntu clang version 12.0.1-19ubuntu3"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym ft_set_face_normal
