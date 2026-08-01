	.macro space64 x:req
		.if VER_64BIT == 1
			.space \x
		.endif
	.endm

	.macro ptrvalue x:req
		.if VER_64BIT == 1
			.quad \x
		.else
			.int \x
		.endif
	.endm
