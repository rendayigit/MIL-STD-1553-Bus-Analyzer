/*******************************************************************************
 * FILE: ddc_udl_security.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to verify that the user is using DDC hardware
 * with DDC software.
 *  
 ******************************************************************************
 * Non-Disclosure Statement
 * ------------------------
 * This software is the sole property of Data Device Corporation.  All
 * rights, title, ownership, or other interests in the software remain
 * the property of Data Device Corporation.  This software may be used
 * in accordance with applicable licenses.  Any unauthorized use,
 * duplication, transmission, distribution, or disclosure is expressly
 * forbidden.
 *
 * This non-disclosure statement may not be removed or modified without
 * prior written consent of Data Device Corporation.
 ******************************************************************************
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2014 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#if 0 /* TODO - not used yet */

#define SW_LOCK_NUM_CLOCKS      0x1000
#define LFSR_ALGORITHM_IN_PROG  0x20000

#define REG_CLOCK   0x0000
#define REG_SEED    0x0001
#define REG_RESULT  0x0002

/*-----------------------------------------------------------------------------
    Function:       ddcSfpMemRegConfig
    Description:    Calculate LFSR in hardware and software with common seed

    Parameters:
                pDeviceContext

    Returns:
                N/A

    History:
          10/08  SMG   -created
------------------------------------------------------------------------------*/
S16BIT ddcSfpMemRegConfig(PDEVICE_CONTEXT pDeviceContext)
{
    LARGE_INTEGER       Time;
    U32                 u32Result = 0, u32lfsrBit = 0, j, u32Clocks, u32Seed, u32CompBit = 0;
    U32                 u32Data, i;
    unsigned char       lfsrReg[17];

    /* Write Seed to first LFSR reg */
    u32Clocks = SW_LOCK_NUM_CLOCKS;
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_CLOCK), &u32Clocks, 0);

    /* Generate Random Seed */
    KeQuerySystemTime(&Time);
    srand(Time.LowPart);
    u32Seed = rand();

    /* Write Seed to register */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_SEED), &u32Seed, 0);

    /* Poll result bit for algorithm to complete */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_RESULT), &u32Data);
    while (u32Data & LFSR_ALGORITHM_IN_PROG)
    {
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_RESULT), &u32Data);
        u32CompBit++;
        if (u32CompBit == 10000)
        {
            break;
        }
    }

    /* Calculate Code */
    for (i = 0; i < 17; i++)
    {
        lfsrReg[i] = (unsigned char)(u32Seed & 0x00000001);
        u32Seed >>= 1;
    }
    for (i = 0; i < SW_LOCK_NUM_CLOCKS + 1; i++)
    {
        u32lfsrBit = lfsrReg[0] ^ lfsrReg[13] ^ lfsrReg[16];
        for (j = 16; j > 0; j--)
            lfsrReg[j] = lfsrReg[j - 1];
        lfsrReg[0] = (unsigned char)u32lfsrBit;
    }
    for (i = 0; i < 17; i++)
    {
        u32Result |= ((U32BIT)lfsrReg[i]) << i;
    }

    /* Read Result */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_RESULT), &u32Data);

    /* Check progress bit to make sure hardware has finished calculating algorithm */
    while (u32Data & LFSR_ALGORITHM_IN_PROG)
    {
        /* Read again */
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdMemConfig.pu32RegBA) + REG_RESULT), &u32Data);

        u32CompBit++;
        if (u32CompBit == 10000)
        {
            break;
        }
    }

    /* Check software calculation to hardware calculation */
    if (u32Data != u32Result)
    {
        return UM_ERROR_HDW_CONFIGURATION;
    }

    return STATUS_SUCCESS;
}

#endif /* TODO - not used yet */