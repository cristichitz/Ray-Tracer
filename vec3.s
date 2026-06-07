	.text
	.intel_syntax noprefix
	.file	"vec3.c"
	.globl	make_vec                        # -- Begin function make_vec
	.p2align	4, 0x90
	.type	make_vec,@function
make_vec:                               # @make_vec
	.cfi_startproc
# %bb.0:
	vinsertps	xmm0, xmm0, xmm1, 16    # xmm0 = xmm0[0],xmm1[0],xmm0[2,3]
	vmovaps	xmm1, xmm2
	ret
.Lfunc_end0:
	.size	make_vec, .Lfunc_end0-make_vec
	.cfi_endproc
                                        # -- End function
	.globl	length_squared                  # -- Begin function length_squared
	.p2align	4, 0x90
	.type	length_squared,@function
length_squared:                         # @length_squared
	.cfi_startproc
# %bb.0:
	vmulps	xmm0, xmm0, xmm0
	vmovshdup	xmm2, xmm0              # xmm2 = xmm0[1,1,3,3]
	vaddss	xmm0, xmm0, xmm2
	vmulss	xmm1, xmm1, xmm1
	vaddss	xmm0, xmm1, xmm0
	ret
.Lfunc_end1:
	.size	length_squared, .Lfunc_end1-length_squared
	.cfi_endproc
                                        # -- End function
	.globl	random_vec_between              # -- Begin function random_vec_between
	.p2align	4, 0x90
	.type	random_vec_between,@function
random_vec_between:                     # @random_vec_between
	.cfi_startproc
# %bb.0:
	sub	rsp, 56
	.cfi_def_cfa_offset 64
	vmovss	dword ptr [rsp + 12], xmm1      # 4-byte Spill
	vmovss	dword ptr [rsp + 8], xmm0       # 4-byte Spill
	call	random_float
	vmovaps	xmmword ptr [rsp + 32], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rsp + 8]       # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rsp + 12]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmmword ptr [rsp + 16], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rsp + 8]       # 4-byte Reload
                                        # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rsp + 12]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmm1, xmm0
	vmovaps	xmm0, xmmword ptr [rsp + 32]    # 16-byte Reload
	vinsertps	xmm0, xmm0, dword ptr [rsp + 16], 16 # 16-byte Folded Reload
                                        # xmm0 = xmm0[0],mem[0],xmm0[2,3]
	add	rsp, 56
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end2:
	.size	random_vec_between, .Lfunc_end2-random_vec_between
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function random_unit_vector
.LCPI3_0:
	.long	0xbf800000                      # float -1
.LCPI3_1:
	.long	0x3f800000                      # float 1
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI3_2:
	.quad	0x3949f623d5a8a733              # double 1.0000000000000001E-32
	.text
	.globl	random_unit_vector
	.p2align	4, 0x90
	.type	random_unit_vector,@function
random_unit_vector:                     # @random_unit_vector
	.cfi_startproc
# %bb.0:
	sub	rsp, 56
	.cfi_def_cfa_offset 64
	.p2align	4, 0x90
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	vmovss	xmm0, dword ptr [rip + .LCPI3_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI3_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmmword ptr [rsp + 32], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rip + .LCPI3_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI3_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmmword ptr [rsp + 16], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rip + .LCPI3_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI3_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmm3, xmm0
	vmovaps	xmm0, xmmword ptr [rsp + 32]    # 16-byte Reload
	vmulss	xmm0, xmm0, xmm0
	vmovaps	xmm1, xmmword ptr [rsp + 16]    # 16-byte Reload
	vmulss	xmm1, xmm1, xmm1
	vaddss	xmm0, xmm0, xmm1
	vmulss	xmm1, xmm3, xmm3
	vaddss	xmm0, xmm1, xmm0
	vcvtss2sd	xmm1, xmm0, xmm0
	vmovss	xmm2, dword ptr [rip + .LCPI3_1] # xmm2 = mem[0],zero,zero,zero
	vucomiss	xmm2, xmm0
	jb	.LBB3_1
# %bb.2:                                #   in Loop: Header=BB3_1 Depth=1
	vucomisd	xmm1, qword ptr [rip + .LCPI3_2]
	jbe	.LBB3_1
# %bb.3:
	vxorps	xmm1, xmm1, xmm1
	vucomiss	xmm0, xmm1
	jb	.LBB3_5
# %bb.4:
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB3_6
.LBB3_5:
	vmovss	dword ptr [rsp + 12], xmm3      # 4-byte Spill
	call	sqrtf
	vmovss	xmm3, dword ptr [rsp + 12]      # 4-byte Reload
                                        # xmm3 = mem[0],zero,zero,zero
.LBB3_6:
	vmovaps	xmm1, xmmword ptr [rsp + 32]    # 16-byte Reload
	vinsertps	xmm1, xmm1, dword ptr [rsp + 16], 16 # 16-byte Folded Reload
                                        # xmm1 = xmm1[0],mem[0],xmm1[2,3]
	vbroadcastss	xmm2, xmm0
	vdivps	xmm2, xmm1, xmm2
	vdivss	xmm1, xmm3, xmm0
	vmovaps	xmm0, xmm2
	add	rsp, 56
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end3:
	.size	random_unit_vector, .Lfunc_end3-random_unit_vector
	.cfi_endproc
                                        # -- End function
	.globl	divide                          # -- Begin function divide
	.p2align	4, 0x90
	.type	divide,@function
divide:                                 # @divide
	.cfi_startproc
# %bb.0:
	vbroadcastss	xmm3, xmm2
	vdivps	xmm0, xmm0, xmm3
	vdivss	xmm1, xmm1, xmm2
	ret
.Lfunc_end4:
	.size	divide, .Lfunc_end4-divide
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function random_on_hemisphere
.LCPI5_0:
	.long	0xbf800000                      # float -1
.LCPI5_1:
	.long	0x3f800000                      # float 1
.LCPI5_3:
	.long	0x80000000                      # float -0
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI5_2:
	.quad	0x3949f623d5a8a733              # double 1.0000000000000001E-32
	.text
	.globl	random_on_hemisphere
	.p2align	4, 0x90
	.type	random_on_hemisphere,@function
random_on_hemisphere:                   # @random_on_hemisphere
	.cfi_startproc
# %bb.0:
	sub	rsp, 72
	.cfi_def_cfa_offset 80
	vmovss	dword ptr [rsp + 12], xmm1      # 4-byte Spill
	vmovaps	xmmword ptr [rsp + 48], xmm0    # 16-byte Spill
	.p2align	4, 0x90
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	vmovss	xmm0, dword ptr [rip + .LCPI5_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI5_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmmword ptr [rsp + 32], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rip + .LCPI5_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI5_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmmword ptr [rsp + 16], xmm0    # 16-byte Spill
	vmovss	xmm0, dword ptr [rip + .LCPI5_0] # xmm0 = mem[0],zero,zero,zero
	vmovss	xmm1, dword ptr [rip + .LCPI5_1] # xmm1 = mem[0],zero,zero,zero
	call	random_float
	vmovaps	xmm3, xmm0
	vmovaps	xmm0, xmmword ptr [rsp + 32]    # 16-byte Reload
	vmulss	xmm0, xmm0, xmm0
	vmovaps	xmm1, xmmword ptr [rsp + 16]    # 16-byte Reload
	vmulss	xmm1, xmm1, xmm1
	vaddss	xmm0, xmm0, xmm1
	vmulss	xmm1, xmm3, xmm3
	vaddss	xmm0, xmm1, xmm0
	vcvtss2sd	xmm1, xmm0, xmm0
	vmovss	xmm2, dword ptr [rip + .LCPI5_1] # xmm2 = mem[0],zero,zero,zero
	vucomiss	xmm2, xmm0
	jb	.LBB5_1
# %bb.2:                                #   in Loop: Header=BB5_1 Depth=1
	vucomisd	xmm1, qword ptr [rip + .LCPI5_2]
	jbe	.LBB5_1
# %bb.3:
	vxorps	xmm5, xmm5, xmm5
	vucomiss	xmm0, xmm5
	jb	.LBB5_5
# %bb.4:
	vsqrtss	xmm1, xmm0, xmm0
	jmp	.LBB5_6
.LBB5_5:
	vmovss	dword ptr [rsp + 8], xmm3       # 4-byte Spill
	call	sqrtf
	vmovss	xmm3, dword ptr [rsp + 8]       # 4-byte Reload
                                        # xmm3 = mem[0],zero,zero,zero
	vxorps	xmm5, xmm5, xmm5
	vmovaps	xmm1, xmm0
.LBB5_6:
	vmovaps	xmm0, xmmword ptr [rsp + 32]    # 16-byte Reload
	vinsertps	xmm0, xmm0, dword ptr [rsp + 16], 16 # 16-byte Folded Reload
                                        # xmm0 = xmm0[0],mem[0],xmm0[2,3]
	vbroadcastss	xmm2, xmm1
	vdivps	xmm0, xmm0, xmm2
	vdivss	xmm1, xmm3, xmm1
	vmulps	xmm2, xmm0, xmmword ptr [rsp + 48] # 16-byte Folded Reload
	vmovshdup	xmm3, xmm2              # xmm3 = xmm2[1,1,3,3]
	vaddss	xmm2, xmm2, xmm3
	vmulss	xmm3, xmm1, dword ptr [rsp + 12] # 4-byte Folded Reload
	vaddss	xmm2, xmm3, xmm2
	vucomiss	xmm2, xmm5
	vbroadcastss	xmm4, dword ptr [rip + .LCPI5_3] # xmm4 = [-0.0E+0,-0.0E+0,-0.0E+0,-0.0E+0]
	vxorps	xmm3, xmm1, xmm4
	ja	.LBB5_8
# %bb.7:
	vxorps	xmm0, xmm0, xmm4
.LBB5_8:
	vcmpltss	xmm2, xmm5, xmm2
	vblendvps	xmm1, xmm3, xmm1, xmm2
	add	rsp, 72
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end5:
	.size	random_on_hemisphere, .Lfunc_end5-random_on_hemisphere
	.cfi_endproc
                                        # -- End function
	.globl	dot                             # -- Begin function dot
	.p2align	4, 0x90
	.type	dot,@function
dot:                                    # @dot
	.cfi_startproc
# %bb.0:
	vmulps	xmm0, xmm0, xmm2
	vmovshdup	xmm2, xmm0              # xmm2 = xmm0[1,1,3,3]
	vaddss	xmm0, xmm0, xmm2
	vmulss	xmm1, xmm1, xmm3
	vaddss	xmm0, xmm1, xmm0
	ret
.Lfunc_end6:
	.size	dot, .Lfunc_end6-dot
	.cfi_endproc
                                        # -- End function
	.globl	scale                           # -- Begin function scale
	.p2align	4, 0x90
	.type	scale,@function
scale:                                  # @scale
	.cfi_startproc
# %bb.0:
	vbroadcastss	xmm3, xmm2
	vmulps	xmm0, xmm3, xmm0
	vmulss	xmm1, xmm1, xmm2
	ret
.Lfunc_end7:
	.size	scale, .Lfunc_end7-scale
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2                               # -- Begin function near_zero
.LCPI8_0:
	.long	0x7fffffff                      # float NaN
.LCPI8_1:
	.long	0x322bcc77                      # float 9.99999993E-9
	.text
	.globl	near_zero
	.p2align	4, 0x90
	.type	near_zero,@function
near_zero:                              # @near_zero
	.cfi_startproc
# %bb.0:
	vbroadcastss	xmm2, dword ptr [rip + .LCPI8_0] # xmm2 = [NaN,NaN,NaN,NaN]
	vandps	xmm4, xmm0, xmm2
	vmovss	xmm3, dword ptr [rip + .LCPI8_1] # xmm3 = mem[0],zero,zero,zero
	vucomiss	xmm3, xmm4
	jbe	.LBB8_4
# %bb.1:
	vmovshdup	xmm0, xmm0              # xmm0 = xmm0[1,1,3,3]
	vandps	xmm0, xmm0, xmm2
	vucomiss	xmm3, xmm0
	jbe	.LBB8_4
# %bb.2:
	vandps	xmm0, xmm1, xmm2
	vucomiss	xmm3, xmm0
	seta	al
                                        # kill: def $al killed $al killed $eax
	ret
.LBB8_4:
	xor	eax, eax
                                        # kill: def $al killed $al killed $eax
	ret
.Lfunc_end8:
	.size	near_zero, .Lfunc_end8-near_zero
	.cfi_endproc
                                        # -- End function
	.globl	mult                            # -- Begin function mult
	.p2align	4, 0x90
	.type	mult,@function
mult:                                   # @mult
	.cfi_startproc
# %bb.0:
	vmulps	xmm0, xmm0, xmm2
	vmulss	xmm1, xmm1, xmm3
	ret
.Lfunc_end9:
	.size	mult, .Lfunc_end9-mult
	.cfi_endproc
                                        # -- End function
	.globl	add                             # -- Begin function add
	.p2align	4, 0x90
	.type	add,@function
add:                                    # @add
	.cfi_startproc
# %bb.0:
	vaddps	xmm0, xmm0, xmm2
	vaddss	xmm1, xmm1, xmm3
	ret
.Lfunc_end10:
	.size	add, .Lfunc_end10-add
	.cfi_endproc
                                        # -- End function
	.globl	sub                             # -- Begin function sub
	.p2align	4, 0x90
	.type	sub,@function
sub:                                    # @sub
	.cfi_startproc
# %bb.0:
	vsubps	xmm0, xmm0, xmm2
	vsubss	xmm1, xmm1, xmm3
	ret
.Lfunc_end11:
	.size	sub, .Lfunc_end11-sub
	.cfi_endproc
                                        # -- End function
	.globl	cross                           # -- Begin function cross
	.p2align	4, 0x90
	.type	cross,@function
cross:                                  # @cross
	.cfi_startproc
# %bb.0:
	vmovshdup	xmm4, xmm0              # xmm4 = xmm0[1,1,3,3]
	vmulss	xmm5, xmm4, xmm3
	vmovshdup	xmm6, xmm2              # xmm6 = xmm2[1,1,3,3]
	vmulss	xmm7, xmm6, xmm1
	vsubss	xmm5, xmm5, xmm7
	vmulss	xmm1, xmm2, xmm1
	vmulss	xmm3, xmm0, xmm3
	vsubss	xmm1, xmm1, xmm3
	vinsertps	xmm3, xmm5, xmm1, 16    # xmm3 = xmm5[0],xmm1[0],xmm5[2,3]
	vmulss	xmm0, xmm0, xmm6
	vmulss	xmm1, xmm4, xmm2
	vsubss	xmm1, xmm0, xmm1
	vmovaps	xmm0, xmm3
	ret
.Lfunc_end12:
	.size	cross, .Lfunc_end12-cross
	.cfi_endproc
                                        # -- End function
	.globl	norm                            # -- Begin function norm
	.p2align	4, 0x90
	.type	norm,@function
norm:                                   # @norm
	.cfi_startproc
# %bb.0:
	vmovaps	xmm3, xmm0
	vmulps	xmm0, xmm0, xmm0
	vmovshdup	xmm2, xmm0              # xmm2 = xmm0[1,1,3,3]
	vaddss	xmm0, xmm0, xmm2
	vmulss	xmm2, xmm1, xmm1
	vaddss	xmm0, xmm2, xmm0
	vxorps	xmm2, xmm2, xmm2
	vucomiss	xmm0, xmm2
	jb	.LBB13_2
# %bb.1:
	vsqrtss	xmm0, xmm0, xmm0
	jmp	.LBB13_3
.LBB13_2:
	sub	rsp, 40
	.cfi_def_cfa_offset 48
	vmovss	dword ptr [rsp + 12], xmm1      # 4-byte Spill
	vmovaps	xmmword ptr [rsp + 16], xmm3    # 16-byte Spill
	call	sqrtf
	vmovaps	xmm3, xmmword ptr [rsp + 16]    # 16-byte Reload
	vmovss	xmm1, dword ptr [rsp + 12]      # 4-byte Reload
                                        # xmm1 = mem[0],zero,zero,zero
	add	rsp, 40
	.cfi_def_cfa_offset 8
.LBB13_3:
	vbroadcastss	xmm2, xmm0
	vdivps	xmm2, xmm3, xmm2
	vdivss	xmm1, xmm1, xmm0
	vmovaps	xmm0, xmm2
	ret
.Lfunc_end13:
	.size	norm, .Lfunc_end13-norm
	.cfi_endproc
                                        # -- End function
	.globl	print_vec                       # -- Begin function print_vec
	.p2align	4, 0x90
	.type	print_vec,@function
print_vec:                              # @print_vec
	.cfi_startproc
# %bb.0:
	vcvtss2sd	xmm4, xmm0, xmm0
	vmovshdup	xmm0, xmm0              # xmm0 = xmm0[1,1,3,3]
	vcvtss2sd	xmm3, xmm0, xmm0
	vcvtss2sd	xmm2, xmm1, xmm1
	mov	edi, offset .L.str
	vmovaps	xmm0, xmm4
	vmovaps	xmm1, xmm3
	mov	al, 3
	jmp	printf                          # TAILCALL
.Lfunc_end14:
	.size	print_vec, .Lfunc_end14-print_vec
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"(%f, %f, %f)\n"
	.size	.L.str, 14

	.ident	"Ubuntu clang version 12.0.1-19ubuntu3"
	.section	".note.GNU-stack","",@progbits
	.addrsig
