/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

#if (FCLK_SPEED == 0)		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif (FCLK_SPEED == 1)		//FCLK = 400 MHz, HCLK = 100 MHz, PCLK = 50 MH
#define M_MDIV	92
#define M_PDIV	0x1
#define M_SDIV	0x1
#endif

#define USB_CLOCK 1

#if (USB_CLOCK == 0)
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif (USB_CLOCK == 1)		//48MHz
#define U_M_MDIV	56
#define U_M_PDIV	0x2
#define U_M_SDIV	0x2
#endif

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b" : "=r" (loops) : "0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */
//by yangwensen@20200908
int board_early_init_f(void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	writel(0xFFFFFF, &clk_power->locktime);

	writel(0, &clk_power->camdivn);		//by yangwensen@20200907

	//FCLK:HCLK:PCLK = 1:4:8
	writel((2<<1)|(1<<0), &clk_power->clkdivn);		//fclk=400mhz, fclk=100mhz, pclk=50mhz, by yangwensen@20200907

	__asm__ volatile(
		"MRC p15, 0, r0, c1, c0, 0\n"
		"ORR r0, r0, #0xc0000000\n"
		"MCR p15, 0, r0, c1, c0, 0\n"
		: : :"r0"
	);

	//When you set MPLL&UPLL values, you have to set the UPLL value first and then the MPLL value. (Needs intervals approximately 7 NOP)
	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure MPLL */
	writel((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV,
	       &clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
	writel(0x00044555, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbup);
	writel(0xAAAAAAAA, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcup);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgup);
	writel(0x002AFAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

int board_init(void)
{
	/* arch number of mini2440-Board */
	gd->bd->bi_arch_number = MACH_TYPE_MINI2440;		//by yangwensen@20201123

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
}
