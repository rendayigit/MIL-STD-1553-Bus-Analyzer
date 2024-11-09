/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Software Library
 *
 *          Copyright (c) 2006 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  Regmap.h (RTL register constants header)
 *
 *  Created 03/21/00 DL
 *
 */

/* MASK TO MODIFY ALL REGISTER BITS */
#define REG_MASK_SETALL     0xFFFF

/* USE THE FOLLOWING W/ MASKS TO CLEAR & SET RESPECTIVELY */
#define REG_CLEAR_BITS      0x0000
#define REG_SET_BITS        0xFFFF

#define REG32_MASK_SETALL   0xFFFFFFFF
#define REG32_CLEAR_BITS    0x00000000
#define REG32_SET_BITS      0xFFFFFFFF

/**************** ALL NON-TEST MODE REGISTERS **********************/
#define REG_IMR1_RW         0x0000 /* Interrupt Mask Reg #1 */
#define REG_CFG1_RW         0x0001 /* Config. Reg #1 */
#define REG_CFG2_RW         0x0002 /* Config. Reg #2 */

#define REG_SR_W            0x0003 /* Start/Reset Reg */
#define REG_BCRT_CSTK_R     0x0003 /* BC/RT Cmd Stk Pointer Reg */
#define REG_BC_CTRL_RW      0x0004 /* BC Control Word Reg */
#define REG_RT_SACTRL_RW    0x0004 /* RT Subaddr Control Word Reg */
#define REG_TT_RW           0x0005 /* Time Tag Reg */
#define REG_ISR1_R          0x0006 /* Interrupt Status Reg #1 */
#define REG_CFG3_RW         0x0007 /* Config. Reg #3 */
#define REG_CFG4_RW         0x0008 /* Config. Reg #4 */
#define REG_CFG5_RW         0x0009 /* Config. Reg #5 */
#define REG_RTMT_DSTK_RW    0x000A /* RT/MT Data Stk Address Reg */
#define REG_BC_FRMREMAIN_RW 0x000B /* BC Frame Time Remaining Reg */
#define REG_BC_MSGREMAIN_R  0x000C /* BC Message Time Remaining Reg */
#define REG_BC_FRMTIME_RW   0x000D /* BC Frame Time Reg */
#define REG_BC_INSTPTR_RW   0x000D /* BC Instruction list pointer Reg */
#define REG_RT_LASTCMD_RW   0x000D /* RT Last Command Reg */
#define REG_MT_TRIG_RW      0x000D /* MT Trigger Reg */
#define REG_RT_STATUS_R     0x000E /* RT Status Word Reg */
#define REG_RT_BIT_R        0x000F /* RT BIT Word Reg */
#define REG_CFG6_RW         0x0018 /* Config. Reg #6 */
#define REG_CFG7_RW         0x0019 /* Config. Reg #7 */
#define REG_BC_COND_R       0x001B /* BC Condition Code Reg */
#define REG_BC_GPF_W        0x001B /* BC General Purpose Flag Reg */
#define REG_BIT_TEST_R      0x001C /* BIT Test Reg */
#define REG_IMR2_RW         0x001D /* Interrupt Mask Reg #2 */
#define REG_ISR2_R          0x001E /* Interrupt Status Reg #2 */
#define REG_BC_GPQP_RW      0x001F /* BC General Purpose Queue Ptr Reg */
#define REG_RTMT_ISQP_RW    0x001F /* RT/MT Interrupt Status Queue Ptr */
#define REG_MEM_PAGE        0x0020 /* Current Memory Page (52/53 only) */
#define REG_CFG8_RW         0x0020 /* Config. Reg #8 */
#define REG_MT_DSTK_ENH_R   0x0021 /* Enhanced MT Data Stk Address Reg */
#define REG_MT_CSTK_ENH_R   0x0022 /* Enhanced MT CMD Stk Address Reg */
#define REG_CFG9_RW         0x0023 /* MultiRT Config Register */
#define REG_CFG10_RW        0x0024 /* Starting PCI Addr Low for DMA */
#define REG_CFG11_RW        0x0025 /* Starting PCI Addr High for DMA */
#define REG_CFG12_RW        0x0026 /* Starting Mem Addr Low for DMA */
#define REG_CFG13_RW        0x0027 /* Starting Mem Addr High for DMA */
#define REG_CFG14_RW        0x0028 /* DMA Transfer Size Register */
#define REG_TT3_RW          0x0029 /* Time Tag 32-47, addition to TT2TT 05h */
#define REG_BLD_TYPE_R      0x002A /* Flexcore Build Type register */

/* PC104+ and MultiIO board level register */
#define REG_FLASH_ADDR_RW                  0x400  /* Flash addr reg */
#define REG_FLASH_CNT_DIN_RW               0x401  /* Flash control reg / data in */
#define REG_FLASH_CNT_DOUT_RW              0x402  /* Flash comtrol reg / data out */
#define REG_FLASH_CFG_RW                   0x403  /* Flash status reg */
#define REG_CAP_R                          0x404  /* Capability Reg */
#define REG_FPGA_ARCH_R                    0x405  /* Xilinx FPGA Data Archive # */
#define REG_FPGA_REV_R                     0x406  /* Xilinx FPGA Revision */
#define REG_RESERVED_1                     0x407  /* Reserved */
#define REG_RESERVED_2                     0x408  /* Reserved */
#define REG_FLASH_START_R                  0x409  /* Reserved */
#define REG_FLASH_CLUSTERS_R               0x40A  /* Reserved */
#define REG_FLASH_CLUSTERS_SIZE_R          0x40B  /* Reserved */
#define REG_IRIG_CFG_RW                    0x40C  /* IRIG CFG Reg */
#define REG_IRIG_TEST_RW                   0x40D  /* IRIG Test Reg */
#define REG_IRIG_ENABLE_RW                 0x40E  /* IRIG Enable Reg */
#define REG_INT_STATUS                     0x414  /* Main Int Status Reg */
#define REG_INT_STATUS_2                   0x415  /* Secondary Int Status Reg */
#define REG_MTI_INT_CONFIG                 0x41B  /* DMA Configuration */
#define REG_DIO_CFG                        0x41C  /* Discrete IO Reg */
#define REG_AIO_CFG                        0x41D  /* Avionics IO Reg */

#define REG_IRIG_B_OUTPUT_1         0x4A0 /* IRIG-B Output - Days, hrs, mins, secs */
#define REG_IRIG_B_OUTPUT_2         0x4A1 /* IRIG-B Output - years, control */

/* MT-I Improvements - PC104+ and MultiIO board level register */
#define REG_MTI_CFG15_RW            0x002E /* MT-I New Cmd Stack? */
#define REG_MTI_IMR3_RW             0x002F /* MT-I New Interrupt Mask Register */
#define REG_MTI_IRQ_WORD_CNT_RW     0x0031 /* MT-I Interrupt # Words */
#define REG_MTI_IRQ_MSG_CNT_RW      0x0032 /* MT-I Interrupt # Messages */
#define REG_MTI_IRQ_TIME_IN_RW      0x0033 /* MT-I Interrupt Time In */
#define REG_MTI_IRQ_CTL_RW          0x0034 /* MT-I Interrupt Control Register */
#define REG_MTI_IRQ_FMSG_OUT_ADDR_R 0x0035 /* MT-I Interrupt First Message Out Address */
#define REG_MTI_IRQ_MSG_LEN_OUT_R   0x0036 /* MT-I Interrupt # Words written to CMD stack since last int */
#define REG_MTI_IRQ_MSG_CNT_OUT_R   0x0037 /* MT-I Interrupt # Msgs written to CMD stack since last int  */
#define REG_MTI_ISR3_R              0x0038 /* MT-I Interrupt Status Register  */
#define REG_MTI_Q_STAT_R            0x0039 /* MT-I Interrupt Q Status Register  */

/**************** BU-6558xCx Series Registers **********************/
#define REG_CANCTL0         0x0081 /* CAN bus control Reg EBR */
#define REG_CANCTL1         0x0091 /* CAN bus control Reg EBR */
#define REG_CANCTL2         0x00A1 /* CAN bus control Reg EBR */
#define REG_CANCTL3         0x00B1 /* CAN bus control Reg EBR */
#define REG_MSCI0           0x00C0 /* MSCI Serial Addr Reg EBR */
#define REG_MSCI1           0x00C1 /* MSCI Serial Addr Reg EBR */
#define REG_STORE0          0x00C2 /* Store Addr Reg EBR */
#define REG_STORE1          0x00C3 /* Store Addr Reg EBR */
#define REG_INT_80          0x00D2 /* 6558x Int Status Register */
#define REG_CAN_EBR_CTL     0x00D5 /* CANbus - EBR loopback ctrl */
#define REG_CAN_EBR_READ    0x00D6 /* CANbus - EBR loopback read */
#define REG_HUB_RW          0x00D9 /* 6558x Hub Port Register */

/**************** INTERRUPT MASK REGISTER #1 (IMR1) BITS ***********/
#define IMR1_ALL_EOM            0x0001
#define IMR1_BC_STATUS_SET      0x0002
#define IMR1_RT_MODE_CODE       0x0002
#define IMR1_MT_PATTERN_TRIG    0x0002
#define IMR1_ALL_FORMAT_ERR     0x0004
#define IMR1_BC_END_OF_FRM      0x0008
#define IMR1_BC_MSG_EOM         0x0010
#define IMR1_RT_SUBADDR_EOM     0x0010
#define IMR1_RT_CIRCBUF_ROVER   0x0020
#define IMR1_ALL_TT_ROVER       0x0040
#define IMR1_RT_ADDR_PAR_ERR    0x0080
#define IMR1_BC_RETRY           0x0100
#define IMR1_ALL_HSHAKE_FAIL    0x0200
#define IMR1_MT_DATASTK_ROVER   0x0400
#define IMR1_MT_CMDSTK_ROVER    0x0800
#define IMR1_BCRT_CMDSTK_ROVER  0x1000
#define IMR1_BCRT_TX_TIMEOUT    0x2000
#define IMR1_ALL_RAM_PAR_ERR    0x4000

/**************** CONFIGURATION REGISTER #1 (CFG1) BITS ************/
#define CFG1_FUNCTION           0xC000
#define CFG1_FUNCTION_BC        0x0000
#define CFG1_FUNCTION_RT        0x8000
#define CFG1_FUNCTION_MT        0x4000

/*-BC FUNCTION BITS-*/
#define CFG1_BC_MSG_IN_PROG     0x0001
#define CFG1_BC_FRM_IN_PROG     0x0002
#define CFG1_BC_ENABLED         0x0004
#define CFG1_BC_DOUBLE_RETRY    0x0008
#define CFG1_BC_RETRY_ENABLED   0x0010
#define CFG1_BC_GAP_T_ENABLED   0x0020
#define CFG1_BC_I_TRG_ENABLED   0x0040
#define CFG1_BC_E_TRG_ENABLED   0x0080
#define CFG1_BC_FRM_AUTOREPEAT  0x0100
#define CFG1_BC_STAT_STOP_FRM   0x0200
#define CFG1_BC_STAT_STOP_MSG   0x0400
#define CFG1_BC_ERR_STOP_FRM    0x0800
#define CFG1_BC_ERR_STOP_MSG    0x1000
#define CFG1_BC_CURRENT_AREA    0x2000

/*-RT FUNCTION BITS-*/
#define CFG1_RT_MSG_IN_PROG     0x0001
#define CFG1_RT_MMT_ENABLED     0x1000
#define CFG1_RT_CURRENT_AREA    0x2000

/*-RT (W/O ALT STATUS) FUNCTION BITS-*/
#define CFG1_RT_STS_MASK        0x0F80
#define CFG1_RT_FLAG            0x0080
#define CFG1_RT_SS_FLAG         0x0100
#define CFG1_RT_SERV_REQ        0x0200
#define CFG1_RT_BUSY            0x0400
#define CFG1_RT_DBCA            0x0800

/*-RT (WITH ALT STATUS) FUNCTION BITS-*/
#define CFG1_RT_S10_S0          0x0FFE

/*-MT FUNCTION BITS-*/
#define CFG1_MT_ACTIVE          0x0001
#define CFG1_MT_TRIGGERED       0x0002
#define CFG1_MT_ENABLED         0x0004
#define CFG1_MT_E_TRG_ENABLED   0x0080
#define CFG1_MT_STOP_ON_TRG     0x0200
#define CFG1_MT_START_ON_TRG    0x0400
#define CFG1_MT_TRG_ENA_WRD     0x0800
#define CFG1_MT_MMT_ENABLED     0x1000
#define CFG1_MT_CURRENT_AREA    0x2000

/**************** CONFIGURATION REGISTER #2 (CFG2) BITS ************/
#define CFG2_RT_SEP_BCST_DATA   0x0001
#define CFG2_RT_ENH_MEMMGR      0x0002
#define CFG2_RT_CLEAR_SERV_REQ  0x0004
#define CFG2_ALL_IRQ_LVL_PULSE  0x0008
#define CFG2_ALL_IRQ_AUTO_CLR   0x0010
#define CFG2_RT_LD_TT_ON_SYNC   0x0020
#define CFG2_RT_CLR_TT_ON_SYNC  0x0040
#define CFG2_BCRT_256WRD_BNDRY  0x0400
#define CFG2_RT_OVR_INV_DATA    0x0800
#define CFG2_RT_SA_DBUF_ENA     0x1000
#define CFG2_RT_BUSY_LOOKUP     0x2000
#define CFG2_ALL_RAM_PAR_ENA    0x4000
#define CFG2_ALL_ENH_IRQS       0x8000

/*-TIME TAG RES BITS-*/
#define CFG2_ALL_TT_RES_MASK    0x0380
#define CFG2_ALL_TT_64US        0x0000
#define CFG2_ALL_TT_32US        0x0080
#define CFG2_ALL_TT_16US        0x0100
#define CFG2_ALL_TT_8US         0x0180
#define CFG2_ALL_TT_4US         0x0200
#define CFG2_ALL_TT_2US         0x0280
#define CFG2_ALL_TT_TEST        0x0300
#define CFG2_ALL_TT_EXT_CLK     0x0380

/**************** START RESET REGISTER (SR) BITS *******************/
#define SR_ALL_RESET            0x0001
#define SR_BCMT_START           0x0002
#define SR_ALL_IRQ_RESET        0x0004
#define SR_ALL_TT_RESET         0x0008
#define SR_ALL_TT_TEST_CLOCK    0x0010
#define SR_BC_STOP_ON_FRM       0x0020
#define SR_BCMT_STOP_ON_MSG     0x0040
#define SR_START_PROT_TEST      0x0080
#define SR_DMA_START            0x0100  /* Make sure Cfg#8 DMA Enabled */
#define SR_START_RAM_TEST       0x0200
#define SR_CLEAR_TEST_REG       0x0400
#define SR_CLEAR_RT_HALT        0x0800

/**************** INTERRUPT STATUS REGISTER #1 (ISR1) BITS **********/
#define ISR1_ALL_EOM            0x0001
#define ISR1_BC_STATUS_SET      0x0002
#define ISR1_RT_MODE_CODE       0x0002
#define ISR1_MT_PATTERN_TRIG    0x0002
#define ISR1_ALL_FORMAT_ERR     0x0004
#define ISR1_BC_END_OF_FRM      0x0008
#define ISR1_BC_MSG_EOM         0x0010
#define ISR1_RT_SUBADDR_EOM     0x0010
#define ISR1_RT_CIRCBUF_ROVER   0x0020
#define ISR1_ALL_TT_ROVER       0x0040
#define ISR1_RT_ADDR_PAR_ERR    0x0080
#define ISR1_BC_RETRY           0x0100
#define ISR1_ALL_HSHAKE_FAIL    0x0200
#define ISR1_MT_DATASTK_ROVER   0x0400
#define ISR1_MT_CMDSTK_ROVER    0x0800
#define ISR1_BCRT_CMDSTK_ROVER  0x1000
#define ISR1_BCRT_TX_TIMEOUT    0x2000
#define ISR1_ALL_RAM_PAR_ERR    0x4000
#define ISR1_ALL_MASTER_IRQ     0x8000

/**************** CONFIGURATION REGISTER #3 (CFG3) BITS *************/
#define CFG3_RT_ENH_MCODES      0x0001
#define CFG3_RTMT_1553A_MCODES  0x0002
#define CFG3_RT_FAILFLG_WRAP    0x0004
#define CFG3_RT_BUSY_RX_XFER    0x0008
#define CFG3_RT_ILL_RX_XFER     0x0010
#define CFG3_RT_ALT_STATUS_WRD  0x0020
#define CFG3_RT_OVR_MCODE_ERR   0x0040
#define CFG3_RT_ILL_DISABLED    0x0080
#define CFG3_ALL_ENH_MODE       0x8000

/*-MONITOR DATA STACK BITS-*/
#define CFG3_MT_DATASTK_SIZE    0x0700
#define CFG3_MT_DATASTK_64K     0x0000
#define CFG3_MT_DATASTK_32K     0x0100
#define CFG3_MT_DATASTK_16K     0x0200
#define CFG3_MT_DATASTK_8K      0x0300
#define CFG3_MT_DATASTK_4K      0x0400
#define CFG3_MT_DATASTK_2K      0x0500
#define CFG3_MT_DATASTK_1K      0x0600
#define CFG3_MT_DATASTK_512     0x0700

/*-MONITOR COMMAND STACK BITS-*/
#define CFG3_MT_CMDSTK_SIZE     0x1800
#define CFG3_MT_CMDSTK_256      0x0000
#define CFG3_MT_CMDSTK_1K       0x0800
#define CFG3_MT_CMDSTK_4K       0x1000
#define CFG3_MT_CMDSTK_16K      0x1800

/*-BC AND RT COMMAND STACK BITS-*/
#define CFG3_BCRT_CMDSTK_SIZE   0x6000
#define CFG3_BCRT_CMDSTK_256    0x0000
#define CFG3_BCRT_CMDSTK_512    0x2000
#define CFG3_BCRT_CMDSTK_1K     0x4000
#define CFG3_BCRT_CMDSTK_2K     0x6000

/**************** CONFIGURATION REGISTER #4 (CFG4) BITS ************/
#define CFG4_RT_LATCH_ADDR      0x0008
#define CFG4_MT_TAG_GAP_OPT     0x0010
#define CFG4_BC_VALBUSY_NODATA  0x0020
#define CFG4_BC_VALME_NODATA    0x0040
#define CFG4_BC_RETRY2_ALT      0x0080
#define CFG4_BC_RETRY1_ALT      0x0100
#define CFG4_BC_RETRY_IF_STATUS 0x0200
#define CFG4_BC_RETRY_IF_A_ME   0x0400
#define CFG4_BC_BCST_MASK_XOR   0x0800
#define CFG4_BC_ENH_CTRL_WORD   0x1000
#define CFG4_RT_MCODE_OVR_BUSY  0x2000
#define CFG4_RT_INH_BIT_IF_BUSY 0x4000
#define CFG4_RT_EXT_BIT_ENABLE  0x8000

/*-ACE TEST MODE BITS-*/
#define CFG4_ALL_TESTM_MASK     0x0007
#define CFG4_ALL_TESTM_NORM     0x0000
#define CFG4_ALL_TESTM_DECODER  0x0001
#define CFG4_ALL_TESTM_ENCODER  0x0002
#define CFG4_ALL_TESTM_PROTOCOL 0x0003
#define CFG4_ALL_TESTM_TIMER    0x0004
#define CFG4_ALL_TESTM_REGISTER 0x0005
#define CFG4_ALL_TESTM_RESRVD1  0x0006
#define CFG4_ALL_TESTM_RESRVD2  0x0007

/**************** CONFIGURATION REGISTER #5 (CFG5) BITS ************/
#define CFG5_RT_ADDR_PAR        0x0001
#define CFG5_RT_ADDR_MASK       0x003E
#define CFG5_RT_ADDR_LATCH      0x0040
#define CFG5_RTMT_BCST_DISABLE  0x0080
#define CFG5_ALL_GAP_CHECK_ENA  0x0100
#define CFG5_ALL_EXP_XING_ENA   0x0800
#define CFG5_BCRT_TXB_INH       0x1000
#define CFG5_BCRT_TXA_INH       0x2000
#define CFG5_ALL_SINGLE_ENDED   0x4000
#define CFG5_ALL_CLK_12_16      0x8000

/*-RESPONSE TIMEOUT SELECT BITS-*/
#define CFG5_ALL_RESPTIME_MASK  0x0600
#define CFG5_ALL_RESPTIME_18US  0x0000
#define CFG5_ALL_RESPTIME_22US  0x0200
#define CFG5_ALL_RESPTIME_50US  0x0400
#define CFG5_ALL_RESPTIME_130US 0x0600

/**************** CONFIGURATION REGISTER #6 (CFG6) BITS ************/
#define CFG6_ALL_64WRD_REGSPC   0x0004
#define CFG6_MT_ENH_MT          0x0010
#define CFG6_RT_ADDR_SRC        0x0020
#define CFG6_RTMT_IRQ_Q_ENA     0x0040
#define CFG6_RTMT_VALMSG_TO_Q   0x0080
#define CFG6_RTMT_INVALMSG_TO_Q 0x0100
#define CFG6_RT_GCIRC_ENA       0x1000
#define CFG6_RTMT_CMDP_INC_EOM  0x2000
#define CFG6_ALL_ENH_CPU_ACCSS  0x4000
#define CFG6_BC_ENH_BC          0x8000
#define CFG6_GCIRC_ENA_MASK     0x1000

/*-CLOCK SELECT BITS-*/
#define CFG6_ALL_CLKSEL_MASK    0x0003
#define CFG6_ALL_CLKSEL_16MHZ   0x0000
#define CFG6_ALL_CLKSEL_12MHZ   0x0001
#define CFG6_ALL_CLKSEL_20MHZ   0x0002
#define CFG6_ALL_CLKSEL_10MHZ   0x0003

/*-GLOBAL CIRCULAR BUFFER SIZE-*/
#define CFG6_RT_GCIRC_SZ_MASK   0x0E00
#define CFG6_RT_GCIRC_SZ_SNGLE  0x0000
#define CFG6_RT_GCIRC_SZ_128    0x0200
#define CFG6_RT_GCIRC_SZ_256    0x0400
#define CFG6_RT_GCIRC_SZ_512    0x0600
#define CFG6_RT_GCIRC_SZ_1K     0x0800
#define CFG6_RT_GCIRC_SZ_2K     0x0A00
#define CFG6_RT_GCIRC_SZ_4K     0x0C00
#define CFG6_RT_GCIRC_SZ_8K     0x0E00

/**************** CONFIGURATION REGISTER #7 (CFG7) BITS ************/
#define CFG7_ALL_MCRST_INCMD    0x0001
#define CFG7_BC_WDTIMER_ENA     0x0002
#define CFG7_BCRT_ENH_TT_SYNC   0x0004
#define CFG7_ALL_1553B_RESPTIME 0x0008
#define CFG7_RT_HALT            0x0010

/*-MEMORY MANAGEMENT BASE ADDRESS-*/
#define CFG7_ALL_MM_BASE_MASK   0xFC00

/**************** CONFIGURATION REGISTER #8 (CFG8) BITS ************/
#define CFG8_MON_OWN_RT_ADDR    0x00000001
#define CFG8_DMA_ENA            0x00000002
#define CFG8_EOM_INT_COUNT_ENA  0x00000004
#define CFG8_EOM_INT_COUNT_1    0x00000008
#define CFG8_EOM_INT_COUNT_2    0x00000010
#define CFG8_EOM_INT_COUNT_4    0x00000020
#define CFG8_EOM_INT_COUNT_8    0x00000040
#define CFG8_EOM_INT_COUNT_16   0x00000080
#define CFG8_EOM_INT_COUNT_32   0x00000100
#define CFG8_TT_AT_EOM          0x00000200
#define CFG8_BUSY_FLAG          0x00000400
#define CFG8_TT_1US_RESOLUTION  0x00000800
#define CFG8_ALT_BC_MSG_TIMER   0x00001000
#define CFG8_ALT_TT_INIT_LOAD   0x00002000
#define CFG8_EXT_TT_LOAD_CLEAR  0x00004000
#define CFG8_DMA_BUSY           0x00008000
#define CFG8_PRG_TT_ROVR_CODE_0 0x00010000
#define CFG8_PRG_TT_ROVR_CODE_1 0x00020000
#define CFG8_PRG_TT_ROVR_CODE_2 0x00040000

#define CFG8_RT_ONLY            0x0008  /* Bit 3: 0-BC/MT enabled; 1-RT only        */

/**************** CONFIGURATION REGISTER #15 (CFG15) BITS ************/
#define CFG15_NEW_CMD_STACK    0x00000001
#define CFG15_EXT_INT_RESET    0x00000002

/*********** BC GENERAL PURPOSE FLAG REGISTER (BCGPF) BITS *********/
/*-GENERAL PURPOSE FLAG 0-*/
#define BCGPF_BC_GPF0_MASK      0x0101
#define BCGPF_BC_GPF0_LEAVE     0x0000
#define BCGPF_BC_GPF0_SET       0x0001
#define BCGPF_BC_GPF0_CLEAR     0x0100
#define BCGPF_BC_GPF0_TOGGLE    0x0101

/*-GENERAL PURPOSE FLAG 1-*/
#define BCGPF_BC_GPF1_MASK      0x0202
#define BCGPF_BC_GPF1_LEAVE     0x0000
#define BCGPF_BC_GPF1_SET       0x0002
#define BCGPF_BC_GPF1_CLEAR     0x0200
#define BCGPF_BC_GPF1_TOGGLE    0x0202

/*-GENERAL PURPOSE FLAG 2-*/
#define BCGPF_BC_GPF2_MASK      0x0404
#define BCGPF_BC_GPF2_LEAVE     0x0000
#define BCGPF_BC_GPF2_SET       0x0004
#define BCGPF_BC_GPF2_CLEAR     0x0400
#define BCGPF_BC_GPF2_TOGGLE    0x0404

/*-GENERAL PURPOSE FLAG 3-*/
#define BCGPF_BC_GPF3_MASK      0x0808
#define BCGPF_BC_GPF3_LEAVE     0x0000
#define BCGPF_BC_GPF3_SET       0x0008
#define BCGPF_BC_GPF3_CLEAR     0x0800
#define BCGPF_BC_GPF3_TOGGLE    0x0808

/*-GENERAL PURPOSE FLAG 4-*/
#define BCGPF_BC_GPF4_MASK      0x1010
#define BCGPF_BC_GPF4_LEAVE     0x0000
#define BCGPF_BC_GPF4_SET       0x0010
#define BCGPF_BC_GPF4_CLEAR     0x1000
#define BCGPF_BC_GPF4_TOGGLE    0x1010

/*-GENERAL PURPOSE FLAG 5-*/
#define BCGPF_BC_GPF5_MASK      0x2020
#define BCGPF_BC_GPF5_LEAVE     0x0000
#define BCGPF_BC_GPF5_SET       0x0020
#define BCGPF_BC_GPF5_CLEAR     0x2000
#define BCGPF_BC_GPF5_TOGGLE    0x2020

/*-GENERAL PURPOSE FLAG 6-*/
#define BCGPF_BC_GPF6_MASK      0x4040
#define BCGPF_BC_GPF6_LEAVE     0x0000
#define BCGPF_BC_GPF6_SET       0x0040
#define BCGPF_BC_GPF6_CLEAR     0x4000
#define BCGPF_BC_GPF6_TOGGLE    0x4040

/*-GENERAL PURPOSE FLAG 7-*/
#define BCGPF_BC_GPF7_MASK      0x8080
#define BCGPF_BC_GPF7_LEAVE     0x0000
#define BCGPF_BC_GPF7_SET       0x0080
#define BCGPF_BC_GPF7_CLEAR     0x8000
#define BCGPF_BC_GPF7_TOGGLE    0x8080

/*-GENERAL PURPOSE FLAG 8-*/
#define BCGPF_BC_GPF8_MASK      0x01010000
#define BCGPF_BC_GPF8_LEAVE     0x00000000
#define BCGPF_BC_GPF8_SET       0x00010000
#define BCGPF_BC_GPF8_CLEAR     0x01000000
#define BCGPF_BC_GPF8_TOGGLE    0x01010000

/*-GENERAL PURPOSE FLAG 9-*/
#define BCGPF_BC_GPF9_MASK      0x02020000
#define BCGPF_BC_GPF9_LEAVE     0x00000000
#define BCGPF_BC_GPF9_SET       0x00020000
#define BCGPF_BC_GPF9_CLEAR     0x02000000
#define BCGPF_BC_GPF9_TOGGLE    0x02020000

/*-GENERAL PURPOSE FLAG 10*/
#define BCGPF_BC_GPF10_MASK     0x04040000
#define BCGPF_BC_GPF10_LEAVE    0x00000000
#define BCGPF_BC_GPF10_SET      0x00040000
#define BCGPF_BC_GPF10_CLEAR    0x04000000
#define BCGPF_BC_GPF10_TOGGLE   0x04040000

/*-GENERAL PURPOSE FLAG 11*/
#define BCGPF_BC_GPF11_MASK     0x08080000
#define BCGPF_BC_GPF11_LEAVE    0x00000000
#define BCGPF_BC_GPF11_SET      0x00080000
#define BCGPF_BC_GPF11_CLEAR    0x08000000
#define BCGPF_BC_GPF11_TOGGLE   0x08080000

/*-GENERAL PURPOSE FLAG 12*/
#define BCGPF_BC_GPF12_MASK     0x10100000
#define BCGPF_BC_GPF12_LEAVE    0x00000000
#define BCGPF_BC_GPF12_SET      0x00100000
#define BCGPF_BC_GPF12_CLEAR    0x10000000
#define BCGPF_BC_GPF12_TOGGLE   0x10100000

/*-GENERAL PURPOSE FLAG 13*/
#define BCGPF_BC_GPF13_MASK     0x20200000
#define BCGPF_BC_GPF13_LEAVE    0x00000000
#define BCGPF_BC_GPF13_SET      0x00200000
#define BCGPF_BC_GPF13_CLEAR    0x20000000
#define BCGPF_BC_GPF13_TOGGLE   0x20200000

/*-GENERAL PURPOSE FLAG 14*/
#define BCGPF_BC_GPF14_MASK     0x40400000
#define BCGPF_BC_GPF14_LEAVE    0x00000000
#define BCGPF_BC_GPF14_SET      0x00400000
#define BCGPF_BC_GPF14_CLEAR    0x40000000
#define BCGPF_BC_GPF14_TOGGLE   0x40400000

/*-GENERAL PURPOSE FLAG 15*/
#define BCGPF_BC_GPF15_MASK     0x80800000
#define BCGPF_BC_GPF15_LEAVE    0x00000000
#define BCGPF_BC_GPF15_SET      0x00800000
#define BCGPF_BC_GPF15_CLEAR    0x80000000
#define BCGPF_BC_GPF15_TOGGLE   0x80800000


/**************** INTERRUPT MASK REGISTER #2 (IMR2) BITS ***********/
#define IMR2_FLEX_DMA_ENABLE    0x0002  /* FLEXCORE DMA */
#define IMR2_ALL_BIT_COMPLETE   0x0002
#define IMR2_BC_UIRQ0           0x0004
#define IMR2_BC_UIRQ1           0x0008
#define IMR2_BC_UIRQ2           0x0010
#define IMR2_BC_UIRQ3           0x0020
#define IMR2_MT_DSTK_50P_ROVER  0x0040
#define IMR2_MT_CSTK_50P_ROVER  0x0080
#define IMR2_RT_CIRC_50P_ROVER  0x0100
#define IMR2_RT_CSTK_50P_ROVER  0x0200
#define IMR2_BC_TRAP            0x0400
#define IMR2_BC_CALLSTK_ERR     0x0800
#define IMR2_GPQ_ISQ_ROVER      0x1000
#define IMR2_RT_ILL_CMD         0x2000
#define IMR2_BC_OPCODE_PARITY   0x4000

/**************** INTERRUPT STATUS REGISTER #2 (ISR2) BITS *********/
#define ISR2_ALL_IRQ_CHAIN      0x0001
#define ISR2_FLEX_DMA_DONE      0x0002  /* FLEXCORE DMA DONE/Complete bit */
#define ISR2_ALL_BIT_COMPLETE   0x0002
#define ISR2_BC_UIRQ0           0x0004
#define ISR2_BC_UIRQ1           0x0008
#define ISR2_BC_UIRQ2           0x0010
#define ISR2_BC_UIRQ3           0x0020
#define ISR2_MT_DSTK_50P_ROVER  0x0040
#define ISR2_MT_CSTK_50P_ROVER  0x0080
#define ISR2_RT_CIRC_50P_ROVER  0x0100
#define ISR2_RT_CSTK_50P_ROVER  0x0200
#define ISR2_BC_TRAP            0x0400
#define ISR2_BC_CALLSTK_ERR     0x0800
#define ISR2_GPQ_ISQ_ROVER      0x1000
#define ISR2_RT_ILL_CMD         0x2000
#define ISR2_BC_OPCODE_PARITY   0x4000
#define ISR2_ALL_MASTER         0x8000

/**************** CONFIGURATION REGISTER #8 (CFG8) BITS ************/
#define CFG8_MT_OWN_ADDR        0x00000001
#define CFG8_DMA_ENA            0x00000002
#define CFG8_EOM_INT_CNT_ENA    0x00000004
#define CFG8_EOM_INT_CNT_1      0x00000008
#define CFG8_EOM_INT_CNT_2      0x00000010
#define CFG8_EOM_INT_CNT_4      0x00000020
#define CFG8_EOM_INT_CNT_8      0x00000040
#define CFG8_EOM_INT_CNT_16     0x00000080
#define CFG8_EOM_INT_CNT_32     0x00000100
#define CFG8_TT_AT_EOM          0x00000200
#define CFG8_RT_BUSY            0x00000400
#define CFG8_TT_1US_RES         0x00000800
#define CFG8_ALT_BC_MSG_TIMER   0x00001000
#define CFG8_ALT_TT_INIT_LOAD   0x00002000
#define CFG8_EXT_TT_LOAD_CLEAR  0x00004000
#define CFG8_DMA_BUSY           0x00008000

/**************** PC104+ and Multi-IO IRIG-B REGISTER  BITS ************/
#define IRIG_DISABLE            0x00000000  /* use Flexcore's internal TT */
#define IRIG_FMT_A              0x00000003  /* use IRIG Format-A  */
#define IRIG_FMT_B              0x00000005  /* use IRIG Format-B  */
#define IRIG_FMT_C              0x00000007  /* use IRIG Format-C  */
#define IRIG_FMT_D              0x00000009  /* use IRIG Format-D  */

/*****************MT-I Register Bits**********************************/

/**************** INTERRUPT MASK REGISTER #3 (IMR3) BITS ***********/
#define IMR3_MTI_NUM_WORDS      0x0001
#define IMR3_MTI_NUM_MSGS       0x0002
#define IMR3_MTI_TIME           0x0004
#define IMR3_MTI_MSG_TIME       0x0008
#define IMR3_MTI_HOST           0x0010
#define IMR3_MTI_OVR            0x0020

/**************** MTI INT CTL REGISTER  BITS ***********/
#define IMR3_MTI_CTL_HOST_TRIG  0x0001
#define IMR3_MTI_CTL_OVR_CLR    0x0002

/**************** INTERRUPT STATUS REGISTER #3 (ISR3) BITS *********/
#define ISR3_MTI_NUM_WORDS      0x0001
#define ISR3_MTI_NUM_MSGS       0x0002
#define ISR3_MTI_TIME           0x0004
#define ISR3_MTI_MSG_TIME       0x0008
#define ISR3_MTI_HOST           0x0010
#define ISR3_MTI_OVR            0x0020

/**************** MTI Q STATUS REGISTER  BITS ***********/
#define QSTAT_MTI_Q_EMPTY       0x0001
#define QSTAT_MTI_Q_FULL        0x0002

/***************** General Purpose Board Registers ******************/
#define IRIG_CONFIG             0x040CL
#define IRIG_TEST_VALUE         0x040DL
#define IRIG_ENABLE             0x040EL

/* External trigger registers and bit mask */
#define EXT_TRIGGET_DIO_SEL     0x041EL /* bits 0-3: 0 - Internal trigger; 1 - DIO trigger */
#define INT_TRIGGER_VALUE       0x041FL /* Internal trigger data */

#define EXT_TRIGGET_DIO_BITS    0x000F

#define LOOP_BACK_TEST_CONFIG   0x0427L  /* Loopback Test */

/************************** EBR **************************/
#define EBR_HUB_CTRL_REG        0x00DC
