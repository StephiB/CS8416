/***************************************************************************************/
//	NAME:     CS8416.h
//  PURPOSE:  Header file with CS8416 Register definitions (Cirrus CS8416 receiver)
/**************************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif 
#include <Wire.h>


//  Start-of-communication byte
#define CS8416_RD               (0x01)     // Read from a control register
#define CS8416_WR               (0x00)     // Write to a control register
#define CS8416_ADDR				(0x20)     // Identifying address
#define CS8416_WRITE_REG        (CS8416_ADDR | CS8416_WR)     // Start of write sequence
#define CS8416_READ_REG         (CS8416_ADDR | CS8416_RD)     // Start of read sequence

//  Memory Address Pointer (MAP) Register Bit(s)
#define INCR					(0x80)     // Auto-increment MAP


//  Control Port Registers
#define CS8416_CTRL0            (0x00)     //  (R/W)
#define   CTRL0_TRUNC			(0x04)     //         Word length != encoding
#define   CTRL0_PDUR			(0x08)     //         Phase detector (normal/fast)
#define   CTRL0_FSWCLK			(0x40)     //         Set OMCLK to RMCLK

#define CS8416_CTRL1			(0x01)     //  (R/W)
#define   CTRL1_CHS				(0x01)     //         0=A/1=B to channel stat reg (0x0A)
#define   CTRL1_RMCKF			(0x02)     //         Recovered MCLK freq (0=256/1=128)
#define   CTRL1_HOLD			(0x0C)     //         Output behavior on receive error
#define   CTRL1_INT				(0x30)     //         Interrupt output polarity control
#define   CTRL1_MUTESAO			(0x40)     //         Mute serial audio out (1=mute)
#define   CTRL1_SWCLK			(0x80)     //         OMCLK -> RMCLK/OSCLK/OLRCLK

#define CS8416_CTRL2            (0x02)     //  (R/W)
#define   CTRL2_GPO0_SEL		(0x0F)     //         General Purpose Out 0 select
#define   CTRL2_EMPH_CNTL0		(0x70)     //         De-emphasis filter (0=none)
#define   CTRL2_DETCI			(0x80)     //         D -> E status transfer inhibit

#define CS8416_CTRL3            (0x03)     //  (R/W)
#define   CTRL3_GPO1SEL			(0xF0)     //         General Purpose Out 1 select
#define   CTRL3_GPO2SEL			(0x0F)     //         General Purpose Out 2 select

#define CS8416_CTRL4            (0x04)     //  (R/W)
#define   CTRL4_TXSEL			(0x07)     //         Assign RXPn to GPOn TX
#define   CTRL4_RXSEL			(0x38)     //         Assign RXPn to RX input
#define   CTRL4_RXD				(0x40)     //         Set RMCLK to high-Z output
#define   CTRL4_RUN				(0x80)     //         **NORMAL USE=1**  (default=0)

#define CS8416_DATA_FORMAT      (0x05)     //  (R/W)
#define   FRMT_SOLRPOL			(0x01)     //         LRCLK polarity (channel swap)
#define   FRMT_SOSPOL			(0x02)     //         Sample on rising/falling BCLK
#define   FRMT_SODEL			(0x04)     //         Delay SDOUT wrt OLRCLK in LJ
#define   FRMT_SOJUST			(0x08)     //         R/L justify SDOUT wrt OLROUT
#define   FRMT_SORES			(0x30)     //         Word width of serial data out
#define   FRMT_SOSF				(0x40)     //         OSCLK frequency
#define   FRMT_SOMS				(0x80)     //         OSCLK/OLRCLK are master/slave
#define   FRMT_I2S				(FRMT_SOMS | FRMT_SODEL | FRMT_SOLRPOL)
#define   FRMT_LJ				(FRMT_SOMS)
#define   FRMT_RJ24				(FRMT_SOMS | FRMT_SOJUST)

#define CS8416_RX_ERR_MASK      (0x06)     //  (R/W)
#define   ERRMASK_PAR			(0x01)     //         Parity bit error mask
#define   ERRMASK_BIP			(0x02)     //         Biphase error mask
#define   ERRMASK_CONF			(0x04)     //         Confidence bit mask
#define   ERRMASK_V				(0x08)     //         Validity bit mask
#define   ERRMASK_UNLOCK		(0x10)     //         PLL lock error mask
#define   ERRMASK_CCRC			(0x20)     //         Channel status CRC error mask
#define   ERRMASK_QCRC			(0x40)     //         Q-subcode CRC error mask

#define CS8416_INT_MASK         (0x07)     //  (R/W)
#define   INT_FCHM				(0x01)     //         Format change
#define   INT_QCHM				(0x02)     //         New Q-subcode block available
#define   INT_RERRM				(0x04)     //         Receiver error
#define   INT_CCHM				(0x08)     //         Channel status change
#define   INT_DETCM				(0x01)     //         D->E C-buffer transfer
#define   INT_OSLIPM			(0x20)     //         Serial data out drop/repeat
#define   INT_PCCHM				(0x40)     //         PC burst preamble change

#define CS8416_INT_MODE_MSB     (0x08)     //  (R/W)
#define   INT_FCH1				(0x01)     //         Format change
#define   INT_QCH1				(0x02)     //         New Q-subcode block available
#define   INT_RERR1				(0x04)     //         Receiver error
#define   INT_CCH1				(0x08)     //         Channel status change
#define   INT_DETC1				(0x01)     //         D->E C-buffer transfer
#define   INT_OSLIP1			(0x20)     //         Serial data out drop/repeat
#define   INT_PCCH1				(0x40)     //         PC burst preamble change

#define CS8416_INT_MODE_LSB     (0x09)     //  (R/W)
#define   INT_FCH0				(0x01)     //         Format change
#define   INT_QCH0				(0x02)     //         New Q-subcode block available
#define   INT_RERR0				(0x04)     //         Receiver error
#define   INT_CCH0				(0x08)     //         Channel status change
#define   INT_DETC0				(0x01)     //         D->E C-buffer transfer
#define   INT_OSLIP0			(0x20)     //         Serial data out drop/repeat
#define   INT_PCCH0				(0x40)     //         PC burst preamble change

#define CS8416_CH_STAT          (0x0A)     //   (R)
#define   CHSTAT_EMPH			(0x01)     //         Emphasis encoded in stream
#define   CHSTAT_ORIG			(0x02)     //         SCMS original (not copy)
#define   CHSTAT_COPY			(0x04)     //         Copying prohibited by SCMS
#define   CHSTAT_PRO			(0x08)     //         Ch stat is professional fmt
#define   CHSTAT_AUX			(0xF0)     //         Length of IEC60958 aux field

#define CS8416_FMT_DETECT       (0x0B)     //   (R)
#define   FMT_96KHZ				(0x01)     //         Fs encoded as >= 88.1KHz
#define   FMT_DGTL_SIL			(0x02)     //         Digital silence detected
#define   FMT_DTS_CD			(0x08)     //         DTS CD stream detected
#define   FMT_DTS_LD			(0x10)     //         DTS laser disc detected
#define   FMT_IEC61937			(0x20)     //         IEC61937 stream detected
#define   FMT_PCM				(0x40)     //         PCM data detected

#define CS8416_RX_ERROR         (0x0C)     //   (R)
#define   ERROR_PAR				(0x01)     //         Parity bit error
#define   ERROR_BIP				(0x02)     //         Biphase error
#define   ERROR_CONF			(0x04)     //         Confidence bit
#define   ERROR_V				(0x08)     //         Validity bit
#define   ERROR_UNLOCK			(0x10)     //         PLL lock error
#define   ERROR_CCRC			(0x20)     //         Channel status CRC error
#define   ERROR_QCRC			(0x40)     //         Q-subcode CRC error

#define CS8416_INT_STAT         (0x0D)     //   (R)
#define   INT_FCH				(0x01)     //         Format change
#define   INT_QCH				(0x02)     //         New Q-subcode block available
#define   INT_RERR				(0x04)     //         Receiver error
#define   INT_CCH				(0x08)     //         Channel status change
#define   INT_DETC				(0x10)     //         D->E C-buffer transfer
#define   INT_OSLIP				(0x20)     //         Serial data out drop/repeat
#define   INT_PPCH				(0x40)     //         PC burst preamble change

// Q-channel subcode status
#define CS8416_Q_CH_SUB_07_00   (0x0E)     //   (R)   Control
#define CS8416_Q_CH_SUB_15_08   (0x0F)     //   (R)   Track
#define CS8416_Q_CH_SUB_23_16   (0x10)     //   (R)   Index
#define CS8416_Q_CH_SUB_31_24   (0x11)     //   (R)   Minute
#define CS8416_Q_CH_SUB_39_32   (0x12)     //   (R)   Second
#define CS8416_Q_CH_SUB_47_40   (0x13)     //   (R)   Frame
#define CS8416_Q_CH_SUB_55_48   (0x14)     //   (R)   Zero
#define CS8416_Q_CH_SUB_63_56   (0x15)     //   (R)   ABS Minute
#define CS8416_Q_CH_SUB_71_64   (0x16)     //   (R)   ABS Second
#define CS8416_Q_CH_SUB_79_72   (0x17)     //   (R)   ABS Frame

// System clock mode (PLL out of lock)
#define CS8416_OMCLK_TO_RMCLK   (0x18)     //   (R)   Outside MCLK/Recovered MCLK

// Channel status A (left)
#define CS8416_CHAN_STAT_A0     (0x19)     //   (R)   Channel Status A: Byte 0
#define CS8416_CHAN_STAT_A1     (0x1A)     //   (R)   Channel Status A: Byte 1
#define CS8416_CHAN_STAT_A2     (0x1B)     //   (R)   Channel Status A: Byte 2
#define CS8416_CHAN_STAT_A3     (0x1C)     //   (R)   Channel Status A: Byte 3
#define CS8416_CHAN_STAT_A4     (0x1D)     //   (R)   Channel Status A: Byte 4

// Channel status B (right)
#define CS8416_CHAN_STAT_B0     (0x1E)     //   (R)   Channel Status B: Byte 0
#define CS8416_CHAN_STAT_B1     (0x1F)     //   (R)   Channel Status B: Byte 1
#define CS8416_CHAN_STAT_B2     (0x20)     //   (R)   Channel Status B: Byte 2
#define CS8416_CHAN_STAT_B3     (0x21)     //   (R)   Channel Status B: Byte 3
#define CS8416_CHAN_STAT_B4     (0x22)     //   (R)   Channel Status B: Byte 4

#define CS8416_BURST_PRE_PC0    (0x23)     //   (R)   Burst Preamble PC: Byte 0
#define CS8416_BURST_PRE_PC1    (0x24)     //   (R)   Burst Preamble PC: Byte 1
#define CS8416_BURST_PRE_PD0    (0x25)     //   (R)   Burst Preamble PD: Byte 0
#define CS8416_BURST_PRE_PD1    (0x26)     //   (R)   Burst Preamble PD: Byte 1

// IC revision
#define CS8416_ID_VERSION       (0x7F)     //   (R)   ID = 0x02, Version = 3:0

// General Purpose Output encodings
#define GPO_GND					(0x0)     //    Fixed LOW level
#define GPO_INV_EMPH			(0x1)     //    State of /EMPH on incoming stream
#define GPO_INT					(0x2)     //    CS8416 Interrupt output
#define GPO_C					(0x3)     //    Channel status bit
#define GPO_U					(0x4)     //    User data bit
#define GPO_RERR				(0x5)     //    Receiver error
#define GPO_NVERR				(0x6)     //    Non-Validity Receiver Error
#define GPO_RCBL				(0x7)     //    Receiver Channel Status Block
#define GPO_96KHZ				(0x8)     //    Sample rate >= 88.1 KHz
#define GPO_NON_AUDIO			(0x9)     //    Non-audio indicator for decoded input
#define GPO_VLRCLK				(0xA)     //    Virtual LR clock
#define GPO_TX					(0xB)     //    Pass through input spec'd by TXSEL[2:0]
#define GPO_VDD					(0xC)     //    VDD fixed HIGH level
#define GPO_HRMCK				(0xD)     //    512*Fs



class CS8416  {
	public:

	bool       begin(void);
	uint8_t    getDeviceID(void);
	void       writeRegister(uint8_t reg, uint8_t value);
	uint8_t    readRegister(uint8_t reg);
	void       changeInput(uint8_t num);
	void	   initiate(void);
	void	   clockSwitch(boolean cls);
	void	   muteOutput(boolean mto);
	
	private:

	inline uint8_t  i2cread(void);
	inline void     i2cwrite(uint8_t x);
};
