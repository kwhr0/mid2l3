program_start	import
__start	export
	section	start
__start
	lbra	program_start
	endsection

_polkbd	export
_outscr	export
_zichr	export
	section	code
_polkbd
	pshs	y,u
	swi2
	fcb	$41
	fcb	$01
	beq	polkbd1
	tfr	a,b
	lda	#0
	puls	y,u,pc
polkbd1
	ldd	#-1
	puls	y,u,pc
_outscr
	lda	3,s
	pshs	y,u
	swi2
	fcb	$41
	fcb	$05
	puls	y,u,pc
_zichr
	lda	3,s
	pshs	y,u
	swi2
	fcb	$41
	fcb	$11
	bmi	zichr1
	tfr	a,b
	lda	#0
	puls	y,u,pc
zichr1
	ldd	#-1
	puls	y,u,pc
	endsection
