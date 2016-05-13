; Common definitions for TestICal, monitor, monolith and BSL

InitSP		EQU		$400			; Initial value of stack pointer

; Port 1 bit masks
Piezo		EQU		1<<0			; Piezo on P1.0
Rx			EQU		1<<1			; Receive from CMUs (UART input) on P1.1
TxPl 		EQU		1<<2			; Transmit+ to CMUs (UART output) on P1.2
TouchV		EQU		1<<3			; TouchV analog input (BMU) on P1.0,
VrefP		EQU		1<<4			; Analog reference output on P1.4
ArrayV_BoltVPl EQU	1<<5			; Analog input on P1.5
ShuntV_BoltVMi EQU	1<<6			; Analog input on P1.6
BatV_StrapVPl EQU	1<<7			; Analog input on P1.7

; Port 2 bit masks
RelayM_Byp	EQU		1<<0			; IMU relay for HazV- test (BMU) on P2.0
RelayP_57k6 EQU		1<<1			; IMU relay for HazV+ test (BMU), 57.6 kHz IR carrier (CMU) on P2.1
TxMiScu		EQU		1<<2			; Transmit- to SCU (TA1.1 output) on P2.2
PreI		EQU		1<<3			; Precharge contactor auxiliary contact input (BMU) on P2.3
RxScu		EQU		1<<4			; Receive from SCU (TA1.2 input) on P2.4
ErrLed		EQU		1<<5			; Error LED (red) on P2.5
Xin			EQU		1<<6			; Watch crystal in on P2.6
Xout		EQU		1<<7			; Watch crystal out on P2.7
Bypass		EQU		RelayM_Byp		; Aliases for code that's common to newer and older devices
BypPortDIR	EQU		P2DIR			; Bypass MOSFET output on port 2
BypPortSEL	EQU		P2SEL
BypPortOUT	EQU		P2OUT
RelayM		EQU		RelayM_Byp
RelayP		EQU		RelayP_57k6

; Port 3 bit masks. BMU only.
RxChg		EQU		1<<0			; Receive from charger (TA0.2 input) on P3.0
PreCtor		EQU		1<<1			; Precharge contactor output on P3.1
BatCtor		EQU		1<<2			; Battery contactor output  P3.2
ChgCtor		EQU		1<<3			; Charge sources contactor output  P3.3
DisCtor		EQU		1<<4			; Discharge or discretionary loads contactor output on P3.4
TxMiChg		EQU		1<<5			; Transmit- to charger (TA0.1 output) on P3.5
NrmCtor		EQU		1<<6			; Normal loads contactor output on P3.6
BatI		EQU		1<<7			; Battery contactor auxiliary contact input on P3.7
SocMeter	EQU		NrmCtor			; PWM output for SoC meter (monolith only)
SocPortDIR	EQU		P3DIR			; Soc meter PWM output is on port 3
SocPortSEL	EQU		P3SEL
SocPortOUT	EQU		P3OUT

; ADC channel numbers
TouchVChan	EQU		$0				; ADC channel number for touch voltage (BMU only)
VRefPChan	EQU		$4				; Vref+ out
ArrayV_BoltVPl_Chan	EQU	$5			; PV Array voltage (BMU) Bolt+ voltage (CMU)
ShuntV_BoltVMi_Chan	EQU	$6			; Current shunt voltage (BMU), Bolt- voltage (CMU)
BatV_StrapVPl_Chan 	EQU	$7			; Battery voltage (BMU), Strap+ (cell) voltage (CMU)
TempChan	EQU		$A				; Temperature
LinkVChan	EQU		ShuntV_BoltVMi_Chan	; Aliases for code that's common to newer and older devices
CellVChan	EQU		BatV_StrapVPl_Chan
BoltVPlChan EQU		ArrayV_BoltVPl_Chan
NumSamples	EQU		16				; Number of ADC over-samples (typ. 4 or 16)
; To get n more bits of ADC resolution, add up 4^n samples and shift the result right by n bits.
; i.e. By adding up 4^n samples you get 2n more bits in the result,
; but half of them are noise and should be thrown away,
; leaving us with n bits of additional information.

#define		PROG_START	$E000		// Start of program image in flash memory. Ends at $FDFF

#define		WATCHDOG	1			// True if watchdog timer is to be used (only turn off for debugging)
									// Turning it off doesn't work because BSL will still clear and
									// restart the watchdog timer on every call to ReadByte.
#define		BSL2_START	$FC00		// Start BSL2 1 KiB before the end.

; The address BSL2 downloads to is usually the same as PROG_START,
; but when making a transition between different download sizes, the version of TestICal that does the
; update to the new BSL2 will still need to be the old size, so it can be downloaded by the old BSL2.
#define		PROG_START_FOR_BSL		PROG_START	// Where the BSL should put the images it downloads.
//#define		PROG_START_FOR_BSL		$E000	// Would be used temporarily while changing to a BSL that
										//	loads a different sized image to the one it is contained in.
										//	The password may need changing too, in both BSL2 and Monitor.

			ORG		$1010			; Stay away from manufacturer supplied data
			; Calibration data
DATAVERS		EQU		6			; This is version 6 of the CMU info-flash data structure
infoDataStart						; Used when copying between ram and info-flash
infoVoltCal		ds		2			; Voltage scale calibration word
infoTempCal		ds		1			; Temperature offset calibration for internal sensor
infoLinkCal		ds		1			; Link voltage offset calibration data
info8MHzCalD	ds		1			; 8 MHz DCO frequency calibration byte
info8MHzCalB	ds		1			; 8 MHz DCO range calibration byte
infoID			ds		1			; Cell/CMU identifier byte; first cell is 1; written by 'i' cmd
infoDataVers	ds		1			; Data Version byte (cannot move). Must be set to DATAVERS value above
; Note that xxxDataEnd is one PAST the last calibration byte, i.e. the address of the start of what
;	comes after the calibration data
infoDataEnd							; Used when copying between ram and info-flash

; To allow moving old calibration data from the end of the A segment to the new D segment location
oldInfoDataStart	EQU	$10F8
oldInfoDataVers		EQU $10FF