/**
 * \file
 *
 * \brief AT90USB USB register definitions
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#ifndef AT90USB_REGS_H_INCLUDED
#define AT90USB_REGS_H_INCLUDED

/*
 * AT90USB USB core registers
 */

/** \name AT90USB USB hardware control register */
//@{
#define AT90USB_UHWCON_UVREGE	(1 << 0) //!< USB pad regulator enable
#define AT90USB_UHWCON_UVCONE	(1 << 4) //!< UVCON pin enable
#define AT90USB_UHWCON_UIDE	(1 << 6) //!< UID pin enable
#define AT90USB_UHWCON_UIMOD	(1 << 7) //!< USB mode
//@}

/** \name AT90USB USB control register */
//@{
#define AT90USB_USBCON_VBUSTE	(1 << 0) //!< VBUS transition interrupt enable
#define AT90USB_USBCON_IDTE	(1 << 1) //!< ID transition interrupt enable
#define AT90USB_USBCON_OTGPADE	(1 << 4) //!< OTG pad enable
#define AT90USB_USBCON_FRZCLK	(1 << 5) //!< Freeze USB clock
#define AT90USB_USBCON_HOST	(1 << 6) //!< HOST enable
#define AT90USB_USBCON_USBE	(1 << 7) //!< USB macro enable
//@}

/** \name AT90USB USB status register */
//@{
#define AT90USB_USBSTA_VBUS	(1 << 0) //!< VBus flag
#define AT90USB_USBSTA_ID	(1 << 1) //!< IUD pin flag
#define AT90USB_USBSTA_SPEED	(1 << 3) //!< Speed status flag
//@}

/** \name AT90USB USB interrupt register */
//@{
#define AT90USB_USBINT_VBUSTI	(1 << 0) //!< IVBUS transition interrupt flag
#define AT90USB_USBINT_IDTI	(1 << 1) //!< D transition interrupt flag
//@}

/** \name AT90USB USB OTG control register */
//@{
#define AT90USB_USBOTG_VBUSRQC	(1 << 0) //!< VBUS request clear
#define AT90USB_USBOTG_VBUSREQ	(1 << 1) //!< VBUS request
#define AT90USB_USBOTG_VBUSHWC	(1 << 2) //!< VBus hardware control
#define AT90USB_USBOTG_SRPSEL	(1 << 3) //!< SRP selection
#define AT90USB_USBOTG_SRPREQ	(1 << 4) //!< SRP request
#define AT90USB_USBOTG_HNPREQ	(1 << 5) //!< HNP request
//@}

/** \name AT90USB USB OTG timer control register */
//@{
#define AT90USB_OTGTCON_VALUE	(1 << 0) //!< Value of the timer
#define AT90USB_OTGTCON_PAGE	(1 << 1) //!< Timer page access
//@}

/** \name AT90USB USB OTG interrupt enable register */
//@{
#define AT90USB_OTGIEN_SRPE	(1 << 0) //!< SRP interrupt enable
#define AT90USB_OTGIEN_VBERRE	(1 << 1) //!< VBus error interrupt enable
#define AT90USB_OTGIEN_BCERRE	(1 << 2) //!< B-Connection error interrupt enable
#define AT90USB_OTGIEN_ROLEEXE	(1 << 3) //!< Role exchange interrupt enable
#define AT90USB_OTGIEN_HNPERRE	(1 << 4) //!< HNP error interrupt enable
#define AT90USB_OTGIEN_STOE	(1 << 5) //!< Suspend time out error interrupt enable
//@}

/** \name AT90USB USB OTG interrupt register */
//@{
#define AT90USB_OTGINT_SRPI	(1 << 0) //!< SRP interrupt flag
#define AT90USB_OTGINT_VBERRI	(1 << 1) //!< VBus error interrupt flag
#define AT90USB_OTGINT_BCERRI	(1 << 2) //!< B-Connection error interrupt flag
#define AT90USB_OTGINT_ROLEEXI	(1 << 3) //!< Role exchange interrupt flag
#define AT90USB_OTGINT_HNPERRI	(1 << 4) //!< HNP error interrupt flag
#define AT90USB_OTGINT_STOI	(1 << 5) //!< Suspend time out error interrupt flag
//@}

/** \name AT90USB USB device control register */
//@{
#define AT90USB_UDCON_DETACH	(1 << 0) //!< Detatch the device
#define AT90USB_UDCON_RMWKUP	(1 << 1) //!< Remote wake up
#define AT90USB_UDCON_LSM	(1 << 2) //!< USB device low-speed mode selection
//@}

/** \name AT90USB USB device interrupt register */
//@{
#define AT90USB_UDINT_SUSPI	(1 << 0) //!< Suspend interrupt flag
#define AT90USB_UDINT_SOFI	(1 << 2) //!< Start of frame interrupt flag
#define AT90USB_UDINT_EORSTI	(1 << 3) //!< End of reset interrupt flag
#define AT90USB_UDINT_WAKEUPI	(1 << 4) //!< Wake-up CPU interrupt flag
#define AT90USB_UDINT_EORSMI	(1 << 5) //!< End of resume interrupt flag
#define AT90USB_UDINT_UPRSMI	(1 << 6) //!< Upstream resume interrupt flag
//@}

/** \name AT90USB USB device interrupt enable register */
//@{
#define AT90USB_UDIEN_SUSPE	(1 << 0) //!< Suspend interrupt enable
#define AT90USB_UDIEN_SOFE	(1 << 2) //!< Start of frame interrupt enable
#define AT90USB_UDIEN_EORSTE	(1 << 3) //!< End of reset interrupt enable
#define AT90USB_UDIEN_WAKEUPE	(1 << 4) //!< Wake-up CPU interrupt enable
#define AT90USB_UDIEN_EORSME	(1 << 5) //!< End of resume interrupt enable
#define AT90USB_UDIEN_UPRSME	(1 << 6) //!< Upstream resume interrupt enable
//@}

/** \name AT90USB USB address register */
//@{
#define AT90USB_UDADDR_UADD		 (1 << 0) //!< USB address bit field
# define AT90USB_UDADDR_UADD_MASK	((1 << 8) - 1) //!< USB address bit mask
#define AT90USB_UDADDR_ADDEN		 (1 << 7) //!< Address enable
//@}

/** \name AT90USB USB frame number upper register */
//@{
#define AT90USB_UDFNUMH_FNUM	(1 << 0) //!< Frame number upper value
//@}

/** \name AT90USB USB frame number lower register */
//@{
#define AT90USB_UDFNUML_FNUM	(1 << 0) //!< Frame number lower value
//@}

/** \name AT90USB USB frame number register */
//@{
#define AT90USB_UDMFN_FNCERR	(1 << 4) //!< Frame number CRC error flag
//@}

/*
 * AT90USB USB endpoints registers
 */

/** \name AT90USB USB endpoint number register */
//@{
#define AT90USB_UENUM_EPNUM(x)	((x) << 0) //!< Endpoint number
//@}

/** \name AT90USB USB endpoint FIFO reset register */
//@{
#define AT90USB_UERST_EPRST(x)	(1 << (x)) //!< Endpoint x reset
//@}

/** \name AT90USB USB endpoint X control register */
//@{
#define AT90USB_UECONX_EPEN	(1 << 0) //!< Endpoint enable
#define AT90USB_UECONX_RSTDT	(1 << 3) //!< Reset data toggle
#define AT90USB_UECONX_STALLRQC	(1 << 4) //!< STALL request disable
#define AT90USB_UECONX_STALLRQ	(1 << 5) //!< STALL request enable
//@}

/** \name AT90USB USB endpoint X config 0 register */
//@{
#define AT90USB_UECFG0X_EPDIR		(1 << 0)   //!< Endpoint direction
# define AT90USB_UECFG0X_EPDIR_IN	(1 << 0)   //!< IN endpoint
# define AT90USB_UECFG0X_EPDIR_OUT	(0 << 0)   //!< OUT endpoint
#define AT90USB_UECFG0X_EPTYPE(x)	((x) << 6) //!< Endpoint type
# define AT90USB_UECFG0X_EPTYPE_CONTROL	(0 << 6)   //!< Control endpoint
# define AT90USB_UECFG0X_EPTYPE_ISO	(1 << 6)   //!< Isochronous endpoint
# define AT90USB_UECFG0X_EPTYPE_BULK	(2 << 6)   //!< Bulk endpoint
# define AT90USB_UECFG0X_EPTYPE_INT	(3 << 6)   //!< Interrupt endpoint
//@}

/** \name AT90USB USB endpoint X config 1 register */
//@{
#define AT90USB_UECFG1X_ALLOC		(1 << 1)   //!< Endpoint allocate
#define AT90USB_UECFG1X_EPBK(x)		((x) << 2) //!< Endpoint bank
# define AT90USB_UECFG1X_GET_EPBK(x)	((x & 3) >> 2) //!< # banks
#define AT90USB_UECFG1X_EPSIZE(x)	((x) << 4) //!< Endpoint size
# define AT90USB_UECFG1X_EPSIZE_8	(0 << 4)   //!<   8 byte endpoint size
# define AT90USB_UECFG1X_EPSIZE_16	(1 << 4)   //!<  16 byte endpoint size
# define AT90USB_UECFG1X_EPSIZE_32	(2 << 4)   //!<  32 byte endpoint size
# define AT90USB_UECFG1X_EPSIZE_64	(3 << 4)   //!<  64 byte endpoint size
# define AT90USB_UECFG1X_EPSIZE_128	(4 << 4)   //!< 128 byte endpoint size
# define AT90USB_UECFG1X_EPSIZE_256	(5 << 4)   //!< 256 byte endpoint size
//@}

/** \name AT90USB USB endpoint X status 0 register */
//@{
#define AT90USB_UESTA0X_NBUSYBK		(1 << 0)  //!< Busy bank flag
# define AT90USB_UESTA0X_GET_NBUSYBK(x)	((x) & 3) //!< # busy banks
#define AT90USB_UESTA0X_DTSEQ		(1 << 2)  //!< Data toggle sequencing flag
#define AT90USB_UESTA0X_UDERFI		(1 << 5)  //!< Underflow error interrupt
#define AT90USB_UESTA0X_OVERFI		(1 << 6)  //!< Overflow error interrupt
#define AT90USB_UESTA0X_CFGOK		(1 << 7)  //!< Configuration status
//@}

/** \name AT90USB USB endpoint X status 1 register */
//@{
#define AT90USB_UESTA1X_CURRBK	(1 << 0) //!< Current bank flag
#define AT90USB_UESTA1X_CTRLDIR	(1 << 2) //!< Control direction flag
//@}

/** \name AT90USB USB endpoint X interrupt register */
//@{
#define AT90USB_UEINTX_TXINI	(1 << 0) //!< Transmitter ready interrupt flag
#define AT90USB_UEINTX_STALLEDI	(1 << 1) //!< STALL interrupt flag
#define AT90USB_UEINTX_RXOUTI	(1 << 2) //!< Received OUT data interrupt flag
#define AT90USB_UEINTX_KILLBK	(1 << 2) //!< Kill last written bank
#define AT90USB_UEINTX_RXSTPI	(1 << 3) //!< Received SETUP interrupt flag
#define AT90USB_UEINTX_NAKOUTI	(1 << 4) //!< NAK OUT received interrupt flag
#define AT90USB_UEINTX_RWAL	(1 << 5) //!< Read/write allowed flag
#define AT90USB_UEINTX_NAKINI	(1 << 6) //!< NAK IN received interrupt flag
#define AT90USB_UEINTX_FIFOCON	(1 << 7) //!< FIFO control
//@}

/** \name AT90USB USB endpoint X interrupt enable register */
//@{
#define AT90USB_UEIENX_TXINE	(1 << 0) //!< Transmitter ready interrupt enable
#define AT90USB_UEIENX_STALLEDE	(1 << 1) //!< STALL interrupt enable
#define AT90USB_UEIENX_RXOUTE	(1 << 2) //!< Received OUT data interrupt enable
#define AT90USB_UEIENX_RXSTPE	(1 << 3) //!< Received SETUP interrupt enable
#define AT90USB_UEIENX_NAKOUTE	(1 << 4) //!< NAK OUT received interrupt enable
#define AT90USB_UEIENX_NAKINE	(1 << 6) //!< NAK IN received interrupt enable
#define AT90USB_UEIENX_FLERRE	(1 << 7) //!< Flow error interrupt enable
//@}

/** \name AT90USB USB endpoint X data register */
//@{
#define AT90USB_UEDATX_DAT	(1 << 0) //!< Data
//@}

/** \name AT90USB USB endpoint X byte count high register */
//@{
#define AT90USB_UEBCHX_BYCT	(1 << 0) //!< Byte count MSB
//@}

/** \name AT90USB USB endpoint X byte count low register */
//@{
#define AT90USB_UEBCLX_BYCT	(1 << 0) //!< Byte count LSB
//@}

/** \name AT90USB USB endpoint interrupts register */
//@{
#define AT90USB_UEINT_EP(x)	(1 << (x)) //!< Endpoint interrupts
//@}

/*
 * AT90USB USB host registers
 */

/** \name AT90USB USB host control register */
//@{
#define AT90USB_UHCON_SOFEN	(1 << 0) //!< Start of frame generation enable
#define AT90USB_UHCON_RESET	(1 << 1) //!< Send USB reset
#define AT90USB_UHCON_RESUME	(1 << 2) //!< Send USB resume
//@}

/** \name AT90USB USB host interrupt register */
//@{
#define AT90USB_UHINT_DCONNI	(1 << 0) //!< Device connection interrupt
#define AT90USB_UHINT_DDISCI	(1 << 1) //!< Device disconnection interrupt
#define AT90USB_UHINT_RSTI	(1 << 2) //!< USB reset sent interrupt
#define AT90USB_UHINT_RSMEDI	(1 << 3) //!< Downstream resume sent interrupt
#define AT90USB_UHINT_RXRSMI	(1 << 4) //!< Upstream resume received interrupt
#define AT90USB_UHINT_HSOFI	(1 << 5) //!< Host start of frame interrupt
#define AT90USB_UHINT_HWUPI	(1 << 6) //!< Host wake-up interrupt
//@}

/** \name AT90USB USB host interrupt enable register */
//@{
#define AT90USB_UHIEN_DCONNE	(1 << 0) //!< Device connection interrupt enable
#define AT90USB_UHIEN_DDISCE	(1 << 1) //!< Device disconnection interrupt enable
#define AT90USB_UHIEN_RSTE	(1 << 2) //!< USB reset sent interrupt enable
#define AT90USB_UHIEN_RSMEDE	(1 << 3) //!< Downstream resume sent interrupt enable
#define AT90USB_UHIEN_RXRSME	(1 << 4) //!< Upstream resume received interrupt enable
#define AT90USB_UHIEN_HSOFE	(1 << 5) //!< Host start of frame interrupt enable
#define AT90USB_UHIEN_HWUPE	(1 << 6) //!< Host wake-up interrupt enable
//@}

/** \name AT90USB USB host address register */
//@{
#define AT90USB_UHADDR_HADDR	(1 << 0) //!< USB host address
//@}

/** \name AT90USB USB host frame number high register */
//@{
#define AT90USB_UHFNUMH_FNUM	(1 << 0) //!< MSB frame number
//@}

/** \name AT90USB USB host frame number low register */
//@{
#define AT90USB_UHFNUML_FNUM	(1 << 0) //!< LSB frame number
//@}

/** \name AT90USB USB host frame length register */
//@{
#define AT90USB_UHFLEN_FLEN	(1 << 0) //!< Frame length
//@}

/*
 * AT90USB USB pipe registers
 */

/** \name AT90USB USB pipe number register */
//@{
#define AT90USB_UPNUM_PNUM(x)	((x) << 0) //!< Pipe number
//@}

/** \name AT90USB USB pipe reset register */
//@{
#define AT90USB_UPRST_PRST(x)	(1 << (x)) //!< Reset pipe at bit posistion N
//@}

/** \name AT90USB USB pipe control register */
//@{
#define AT90USB_UPCONX_PEN	(1 << 0) //!< Pipe enable
#define AT90USB_UPCONX_RSTDT	(1 << 3) //!< Reset data toggle
#define AT90USB_UPCONX_INMODE	(1 << 5) //!< IN request mode
#define AT90USB_UPCONX_PFREEZE	(1 << 6) //!< Pipe freeze
//@}

/** \name AT90USB USB pipe config 0 register */
//@{
#define AT90USB_UPCFG0X_PEPNUM(x)	((x) << 0) //!< Pipe endpoint number
#define AT90USB_UPCFG0X_PTOKEN(x)	((x) << 4) //!< Pipe token
# define AT90USB_UPCFG0X_PTOKEN_SETUP	(0 << 4)   //!< Setup pipe token
# define AT90USB_UPCFG0X_PTOKEN_IN	(1 << 4)   //!< In pipe token
# define AT90USB_UPCFG0X_PTOKEN_OUT	(2 << 4)   //!< Out pipe token
#define AT90USB_UPCFG0X_PTYPE(x)	((x) << 6) //!< Pipe type
# define AT90USB_UPCFG0X_PTYPE_CONTROL	(0 << 6)   //!< Control pipe
# define AT90USB_UPCFG0X_PTYPE_ISO	(1 << 6)   //!< Isochronous pipe
# define AT90USB_UPCFG0X_PTYPE_BULK	(2 << 6)   //!< Bulk pipe
# define AT90USB_UPCFG0X_PTYPE_INT	(3 << 6)   //!< Interrupt pipe
//@}

/** \name AT90USB USB pipe config 1 register */
//@{
#define AT90USB_UPCFG1X_ALLOC		(1 << 1)   //!< Configure pipe memory
#define AT90USB_UPCFG1X_PBK(x)		((x) << 2) //!< Pipe bank
#define AT90USB_UPCFG1X_PSIZE		((x) << 4) //!< Pipe size
# define AT90USB_UPCFG1X_PSIZE_8	(0 << 4)   //!<   8 byte pipe size
# define AT90USB_UPCFG1X_PSIZE_16	(1 << 4)   //!<  16 byte pipe size
# define AT90USB_UPCFG1X_PSIZE_32	(2 << 4)   //!<  32 byte pipe size
# define AT90USB_UPCFG1X_PSIZE_64	(3 << 4)   //!<  64 byte pipe size
# define AT90USB_UPCFG1X_PSIZE_128	(4 << 4)   //!< 128 byte pipe size
# define AT90USB_UPCFG1X_PSIZE_256	(5 << 4)   //!< 256 byte pipe size
//@}

/** \name AT90USB USB pipe config 2 register */
//@{
#define AT90USB_UPCFG2X_INTFRQ(x)	((x) << 0) //!< Interrupt pipe request frequency
//@}

/** \name AT90USB USB pipe status register */
//@{
#define AT90USB_UPSTAX_NBUSYBK	(1 << 0) //!< Busy bank flag
# define AT90USB_UPSTAX_GET_NBUSYBK(x)	((x) & 3) //!< # busy banks
#define AT90USB_UPSTAX_DTSEQ	(1 << 2) //!< Toggle sequencing flag
#define AT90USB_UPSTAX_UNDERFI	(1 << 5) //!< Underflow flag
#define AT90USB_UPSTAX_OVERFI	(1 << 6) //!< Overflow flag
#define AT90USB_UPSTAX_CFGOK	(1 << 7) //!< Configure pipe memory ok
//@}

/** \name AT90USB USB pipe IN reuqest number before freeze register */
//@{
#define AT90USB_UPINRQX_INRQ(x)	((x) << 0) //!< IN request number before freeze
//@}

/** \name AT90USB USB pipe error register */
//@{
#define AT90USB_UPERRX_DATATGL		(1 << 0) //!< Bad data toggle error
#define AT90USB_UPERRX_DATAPID		(1 << 1) //!< Data PID error
#define AT90USB_UPERRX_PID		(1 << 3) //!< PID error
#define AT90USB_UPERRX_TIMEOUT		(1 << 4) //!< Time out error
#define AT90USB_UPERRX_CRC16		(1 << 5) //!< CRC16 error
#define AT90USB_UPERRX_COUNTER		(1 << 6) //!< Error counter
# define AT90USB_UPERRX_GET_COUNTER(x)	(((x) & 0xc0) >> 6) //!< # errors
//@}

/** \name AT90USB USB pipe interrupt register */
//@{
#define AT90USB_UPINTX_RXINI	(1 << 0) //!< IN data received
#define AT90USB_UPINTX_RXSTALLI	(1 << 1) //!< STALL received / ISO CRC error
#define AT90USB_UPINTX_TXOUTI	(1 << 2) //!< OUT bank ready
#define AT90USB_UPINTX_TXSTPI	(1 << 3) //!< SETUP bank ready
#define AT90USB_UPINTX_PERRI	(1 << 4) //!< Pipe error
#define AT90USB_UPINTX_RWAL	(1 << 5) //!< Read/write allowed
#define AT90USB_UPINTX_NAKEDI	(1 << 6) //!< NAK handshake received
#define AT90USB_UPINTX_FIFOCON	(1 << 7) //!< FIFO control
//@}

/** \name AT90USB USB pipe interrupt enable register */
//@{
#define AT90USB_UPINTX_RXINE	(1 << 0) //!< IN data received enable
#define AT90USB_UPINTX_RXSTALLE	(1 << 1) //!< STALL received enable
#define AT90USB_UPINTX_TXOUTE	(1 << 2) //!< OUT bank ready enable
#define AT90USB_UPINTX_TXSTPE	(1 << 3) //!< SETUP bank ready enable
#define AT90USB_UPINTX_PERRE	(1 << 4) //!< Pipe error enable
#define AT90USB_UPINTX_NAKEDE	(1 << 6) //!< NAK handshake received enable
#define AT90USB_UPINTX_FLERRE	(1 << 7) //!< Flow error enable
//@}

/** \name AT90USB USB pipe data bits register */
//@{
#define AT90USB_UPDATX_PDAT	(1 << 0) //!< Pipe data bits
//@}

/** \name AT90USB USB pipe byte count high register */
//@{
#define AT90USB_UPBCHX_PBYCT	(1 << 0) //!< Byte count MSB bits
//@}

/** \name AT90USB USB pipe byte count low register */
//@{
#define AT90USB_UPBCLX_PBYCT	(1 << 0) //!< Byte count LSB bits
//@}

/** \name AT90USB USB pipe interrupts register */
//@{
#define AT90USB_UPINT_PINT(x)	(1 << (x)) //!< Pipe interrupts
//@}

#endif /* AT90USB_REGS_H_INCLUDED */
