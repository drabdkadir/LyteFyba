; We must test &infoID, not &ID in the macros below, as they are used in the BSL
; and we don't want it using &ramID as this only exists in TestICal.

ActLedOff	MACRO
#if !G2553			// Activity LEDs are done in hardware on newer devices
			tst.b	&infoID
			_IF		NZ
				bit.b	#ErrLed,&P2OUT		; Activity and error LEDs share an output.
				_IF		Z					; If the error LED is off
					bic.b	#ErrLed,&P2DIR		; make output high-Z so activity LED goes off
				_ENDIF							; without turning error LED on.
			_ENDIF
#endif
			ENDM

ActLedOn	MACRO
#if !G2553			// Activity LEDs are done in hardware on newer devices
			; No provision for oscillating to light both LEDs yet.
			; Error LED has priority.
			tst.b	&infoID
			_IF		NZ
				bis.b	#ErrLed,&P2DIR		; Make it a proper output again
			_ENDIF
#endif
			ENDM

ErrLedOff	MACRO
			tst.b	&infoID
			_IF		NZ
				bic.b	#ErrLed,&P2DIR		; Make output high-Z so activity LED will not come on
				bic.b	#ErrLed,&P2OUT		; Turn off the error LED (so ActLedOff/On can tell)
			_ENDIF
			ENDM

ErrLedOn	MACRO
			tst.b	&infoID
			_IF		NZ
				; No provision for oscillating to light both LEDs yet.
				; Error LED has priority.
				bis.b	#ErrLed,&P2DIR		; Make it a proper output
				bis.b	#ErrLed,&P2OUT		; Turn on the error LED
			_ENDIF
			ENDM

ErrLedToggle	MACRO
			tst.b	&infoID
			_IF		NZ
				xor.b	#ErrLed,&P2DIR		; Toggle high-Z-ness so activity LED will not come on
				xor.b	#ErrLed,&P2OUT		; Toggle the error LED (so ActLedOff/On can tell)
			_ENDIF
			ENDM

;
; Macros giving meaningful names to some obscure instruction sequences we have used repeatedly.
;
NCtoAllBits	MACRO	dest
			subc	dest,dest	; Sets all bits to not carry
			ENDM

CtoAllBits	MACRO	dest
			subc	dest,dest	; Sets all bits to not carry
			inv		dest		; Sets all bits to carry
			ENDM

allBitsIfZ	MACRO	src,dest
			and		src,src		; Sets carry if src is not zero
			subc	dest,dest	; Sets all bits to not carry = zero
			ENDM

allBitsIfNZ	MACRO	src,dest
			and		src,src		; Sets carry if src is not zero
			subc	dest,dest	; Sets all bits to not carry = zero
			inv		dest		; Sets all bits to carry = not zero
			ENDM

movBits		MACRO	src,mask,dest	; Trashes src
			xor		dest,src	; Get one bits in src for bits that need to toggle
			and		mask,src	; Mask so we only change those we want to change
			xor		src,dest	; Toggle bits as required
			ENDM

movBits_B	MACRO	src,mask,dest	; Trashes src
			xor.b	dest,src	; Get one bits in src for bits that need to toggle
			and.b	mask,src	; Mask so we only change those we want to change
			xor.b	src,dest	; Toggle bits as required
			ENDM

; Replace e.g. pop.B &interpFlags with popBits_B #bHexOutput, &interpFlags
popBits_B	MACRO	mask,dest
			xor.b	dest,0(SP)	; Get one bits in TOS for bits that need to toggle
			and.b	mask,0(SP)	; Mask so we only change those we want to change
			xor.b	@SP+,dest	; Toggle bits as required and drop from stack
			ENDM

rra2		MACRO  dest
			rra	dest
			rra	dest
			ENDM

rra3		MACRO  dest
			rra	dest
			rra	dest
			rra	dest
			ENDM

rra4		MACRO  dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			ENDM

rra5		MACRO  dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			ENDM

rra6		MACRO  dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			rra	dest
			ENDM

rla2		MACRO  dest
			rla	dest
			rla	dest
			ENDM

rla3		MACRO  dest
			rla	dest
			rla	dest
			rla	dest
			ENDM

rla4		MACRO  dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			ENDM

rla5		MACRO  dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			ENDM

rla6		MACRO  dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			rla	dest
			ENDM

ClearWatchdog MACRO
#if WATCHDOG
			mov.w	#WDTPW+WDTCNTCL,&WDTCTL	; Clear and restart watchdog timer 32k cyc. BSL sets 64 cyc.
#else
			mov.w	#WDTPW+WDTHOLD,&WDTCTL	; Stop Watchdog Timer (bad idea, except while debugging)
#endif
			ENDM

abs			MACRO	dest
			cmp		#0,dest
			_IF		L
				inv		dest
				inc		dest
			_ENDIF
			ENDM

neg			MACRO	dest
			inv		dest
			inc		dest
			ENDM

rra8_l		MACRO	hi,lo
			swpb	hi
			swpb	lo
			xor.b	hi,lo
			xor.w	hi,lo
			mov.b	hi,hi
			ENDM

rla8_l		MACRO	hi,lo
			xor.b	lo,hi
			xor.w	lo,hi
			swpb	hi
			mov.b	lo,lo
			swpb	lo
			ENDM

or_w		MACRO	src,dest
			bis		src,dest			; Badly named instruction at times
			ENDM

or_b		MACRO	src,dest
			bis.b	src,dest			; Badly named instruction at times
			ENDM

swpb_b_R	MACRO	regNum				; Undocumented. Same as swpb, but clears hi byte of reg after.
			; Would have liked to make it just swpb_b and give it R8 etc as dest,
			; but assembler seems broken in that regard. It hangs when I try to invoke such a macro.
			IF regNum=4
				dw		$10C4				; Undocumented swpb.b R4
			ENDIF
			IF regNum=5
				dw		$10C5				; Undocumented swpb.b R5
			ENDIF
			IF regNum=6
				dw		$10C6				; Undocumented swpb.b R6
			ENDIF
			IF regNum=7
				dw		$10C7				; Undocumented swpb.b R7
			ENDIF
			IF regNum=8
				dw		$10C8				; Undocumented swpb.b R8
			ENDIF
			IF regNum=9
				dw		$10C9				; Undocumented swpb.b R9
			ENDIF
			IF regNum=10
				dw		$10CA				; Undocumented swpb.b R10
			ENDIF
			IF regNum=11
				dw		$10CB				; Undocumented swpb.b R11
			ENDIF
			IF regNum=12
				dw		$10CC				; Undocumented swpb.b R12
			ENDIF
			IF regNum=13
				dw		$10CD				; Undocumented swpb.b R13
			ENDIF
			IF regNum=14
				dw		$10CE				; Undocumented swpb.b R14
			ENDIF
			IF regNum=15
				dw		$10CF				; Undocumented swpb.b R15
			ENDIF
			ENDM
