; EC361 commonly used subroutines


	AREA mycode, CODE, READONLY

;------------------------------------------------------------------------------------------------
; 		Subroutine port_config 	
;			- Port 1: bits 31, 29, 28 are outputs; all others are inputs
;			- Port 2: bits 2, 3, 4, 5, 6 are outputs; all others are inputs
		EXPORT port_config
port_config 	PROC
		; Configure Port 1 (set bits P1.28, P1.29, P1.31 to outputs, others to inputs
		LDR 	R1, =0x2009C020  	; R1 = address of FIO1DIR
		LDR 	R2, =0xB0000000		; R2 = 0xB0000000
		STR		R2, [R1] 			; P1.31, 29 and 28 -> outputs, other bits -> inputs (FIO1DIR)

		;Configure Port 2 (set bits P2.6:P2.2 to outputs, others to inputs)
		MOVW	R2, #0x007C			; R2 = 0x0000007C
		STR		R2, [R1, #0x20]		; P2.6:P2.2 -> outputs, other bits -> inputs (FIO2DIR)
		
		BX		LR					; return from display subroutine
		ENDP						; end of LEDs subroutine
;*************************************************************************************************	

;-------------------------------------------------------------------------------------------------
; 		Subroutine LEDs
; 		Assumptions: 	
;			- R0 has the 8-bit number to be displayed on 8 LEDs
;			- Port 1: bits 31, 29, 28 are outputs; all others are inputs
;			- Port 2: bits 2, 3, 4, 5, 6 are outputs; all others are inputs
		EXPORT LEDs
LEDs	PROC
		RBIT	R1, R0				; R1 = reverse bits of N (R0)
		
		;Extract and align bits for Port 1 
		SBFX	R2, R1, #24, #3		; R2 = sign extend (R1(26:24)) 
		LSL		R2, R2, #28			; R2 = (R2 << 28) --> N(7)=P1.28, N(6)=P1.29, N(5)=P1.28	
		
		;Extract and align bits for Port 2
		LSR		R3, R1, #25 		; Extract bits 6:2
		
		;Store extract and aligned bits to ports 1 and 2
		LDR 	R1, =0x2009C034  	; R1= address of FIO1PIN,
		STR		R2, [R1, #0x00]		; write to port 1 (FIO1PIN)
		STR		R3, [R1, #0x20]		; write to port 2 (FIO2PIN)
		
		BX		LR					; return from display subroutine
		ENDP						; end of LEDs subroutine
;***************************************************************************************************	

;---------------------------------------------------------------------------------------------------
; 		Subroutine EINT0_init
; 			Enable EINT0 interrupt on falling edges 
;			
		EXPORT EINT0_init
EINT0_init	PROC
		; ------------------- Step 1 ------------------
	;Setup pin to be EINT0 –> PINSEL4(21:20) = “01”, other bits unchanged
		MOV		R0, #1 	    		; R0= bit mask
		LDR 	R2, =0x4002C010 	; R2= address of PINSEL4 register
		LDR 	R3, [R2]			; R3= current value of PINSEL4 register
		ORR		R3, R0, LSL #20		; set bit 20, leave other bits unchanged
		BIC		R3, R0, LSL #21		; clear bit 21, leave other bits unchanged
		STR		R3, [R2] 	    	; write back to PINSEL4 register	

	;Setup Mode in EXTMODE register -> EXTMODE(0) = ‘1’, other bits unchanged
		LDR 	R2, =0x400FC148 	; R2= Address of EXTMODE register
		LDR 	R3, [R2] 			; R3= current value of EXTMODE register
		ORR		R3, R3, #1	    	; set bit 0 for edge mode, leave other bits unchanged
		STR		R3, [R2] 	    	; write back to EXTMODE register
				
	;Setup Polarity in EXTPOLAR register -> EXTPOLAR(0) = ‘1’, other bits unchanged
		LDR 	R2, =0x400FC14C 	; R2= Address of EXTPOLAR register
		LDR 	R3, [R2]			; R3= current value of EXTPOLAR register	
		BIC		R3, R3, #1	    	; clear bit 0 for falling edge, leave other bits unchanged
		STR		R3, [R2] 	    	; write back to EXTPOLAR register
				
	; ------ Step 2: Enable interrupt for peripheral (EINT0) in ISER0 register 
	; ISER(18) = ‘1’, other bits unchanged
		LDR 	R2, =0xE000E100	; R2= Address of ISER0 register
		LDR 	R3, [R2]
		ORR		R3, R0, LSL #18	; set bit 18 to enable interrupt for EINT0, leave other bits unchanged
		STR		R3, [R2] 	     	; write back to ISER0 register
				
	; ----- Ignore Steps 3 & 4 : default ok
				
	; Pull registers (r4-r11) if used in subroutine (not applicable in this example)

		BX 		LR			     	; return from subroutine
		ENDP						; end of ENT0_init subroutine
;***************************************************************************************************
	
;---------------------------------------------------------------------------------------------------
; 		Subroutine delay_x_ms
; 		Assumptions: 	
;			- R0 (>0) has x (or number) of ms for delay
;			-
		EXPORT delay_x_ms
delay_x_ms	PROC
outerloop	
		LDR		R1, =25000			; inner loop count --> 25,000 iterations * 4 cyces/iteration * 10ns/cycle = 1ms
innerloop
		SUBS	R1, #1				; decrement R1
		NOP
		BNE		innerloop			; each innerloop iteration will take 4 cycles
		
		SUBS	R0, #1				; decrement x
		BNE		outerloop			; each outer iteration will add 4-5 additional cycles
		
		BX		LR					; return from subroutine
		ENDP						; end of LEDs subroutine
;***************************************************************************************************
