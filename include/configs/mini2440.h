/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#if defined(CONFIG_SPL) && !defined(CONFIG_SPL_BUILD)
	#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_S3C24X0		/* This is a SAMSUNG S3C24x0-type SoC */
#define CONFIG_S3C2440		/* specifically a SAMSUNG S3C2440 SoC */
#define CONFIG_MINI2440		/* on a FriendlyARM MINI2440 Board */

#if defined(CONFIG_SPL) && !defined(CONFIG_SPL_BUILD)
	#define CONFIG_SYS_TEXT_BASE	0x33f00000	//the top most 512KB of the 64MB sdram,最高512KB内存用于放uboot,by yangwensen@20201225
#else
	#define CONFIG_SYS_TEXT_BASE	0x0
#endif

#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the SMDK2410 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ	12000000

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_BASE 0x20000000
#define DM9000_IO CONFIG_DM9000_BASE
#define DM9000_DATA (CONFIG_DM9000_BASE+4)
#define CONFIG_DM9000_USE_16BIT
#define CONFIG_DM9000_NO_SROM
#define CONFIG_NET_RETRY_COUNT		20

/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on SMDK2410 */

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#if 0
#define CONFIG_USB_OHCI
#define CONFIG_USB_OHCI_S3C24XX
#define CONFIG_DOS_PARTITION
#endif
/************************************************************
 * RTC
 ************************************************************/
#if 0
#define CONFIG_RTC_S3C24X0
#endif

#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#if 0
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#endif

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BSP
#if 0
#define CONFIG_CMD_DATE
#endif
#define CONFIG_CMD_NAND
#define CONFIG_CMD_REGINFO

#define CONFIG_CMDLINE_EDITING

/* autoboot */
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY

#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.1.111
#define CONFIG_SERVERIP		192.168.1.100
#define CONFIG_GATEWAYIP	192.168.1.1
#define CONFIG_HOSTNAME		"mini2440"
#define CONFIG_ETHADDR		D0:50:99:8E:66:F0
#define CONFIG_BOOTARGS		"console=ttySAC0,115200 root=/dev/mtdblock3"
#define CONFIG_BOOTCOMMAND  "nand read ${kernel_addr} kernel;bootm ${kernel_addr}"

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#endif

/* by yangwensen */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"netdev=eth0\0" \
	"nandargs=setenv bootargs console=ttySAC0,115200 root=/dev/mtdblock3 \0" \
	"nfsargs=setenv bootargs console=ttySAC0,115200 root=/dev/nfs rw " \
		"nfsroot=${serverip}:${rootfspath}\0" \
	"ramargs=setenv bootargs root=/dev/ram rw\0" \
	"addip=setenv bootargs ${bootargs} " \
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"  \
		":${hostname}:${netdev}:off panic=1\0" \
	"nfs_load_kernel=nfs ${kernel_addr} ${serverip}:${kernel_file_path}\0" \
	"nand_load_kernel=nand read ${kernel_addr} kernel\0" \
	"boot_rootfs_nfs=run nfsargs addip;boot\0" \
	"boot_rootfs_nand=run nandargs;boot\0" \
	"rootfspath=/home/yws/code/mini2440/code/rootfs/nfsroot/busybox-1.20.2-gcc4.4.3\0" \
	"hostname=mini2440\0" \
	"kernel_file_path=/home/yws/code/mini2440/code/linux/linux-5.10.2/arch/arm/boot/uImage\0" \
	"fdt_addr=40040000\0" \
	"kernel_addr=30008000\0" \
	"ramdisk_addr=40200000\0" \
	"u-boot=TQM862L/u-image.bin\0" \
	"load=tftp 200000 ${u-boot}\0" \
	""

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_CBSIZE	256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30800000

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* 64 MB */

#define PHYS_FLASH_1		0x00000000 /* Flash Bank #0 */

#define CONFIG_SYS_FLASH_BASE	PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_FLASH_LEGACY_1Mx16
#define CONFIG_FLASH_SHOW_PROGRESS	45

#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_BANKS_LIST     { CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT	(512)

#if 0
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE + 0x070000)
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SIZE			0x10000
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#endif
/* by yangwensen */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET				0x80000
#define CONFIG_ENV_SIZE					0x20000
#define CONFIG_ENV_OVERWRITE


/* by yangwensen */
#define MTDIDS_DEFAULT    "nand0=nand"
#define MTDPARTS_DEFAULT  "mtdparts=nand:512k(uboot),128k(env),4m(kernel),-(rootfs)"

/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN	(448 * 1024)
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_S3C2440
#define CONFIG_SYS_S3C2440_NAND_HWECC
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x4E000000
#define CONFIG_S3C24XX_CUSTOM_NAND_TIMING
#define CONFIG_S3C24XX_TACLS		2		//tCls>12nS	
#define CONFIG_S3C24XX_TWRPH0		1		//tWp>12nS
#define CONFIG_S3C24XX_TWRPH1		0		//tClh>5nS
#endif

#ifdef CONFIG_S3C2440_NAND_HWECC
#define CONFIG_SYS_NAND_PAGE_SIZE   2048
#define CONFIG_SYS_NAND_ECCBYTES    4
#define CONFIG_SYS_NAND_ECCSIZE     CONFIG_SYS_NAND_PAGE_SIZE
#endif

/*
 * File system
 */
#if 0
#define CONFIG_CMD_UBIFS
#endif
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#if 0
#define CONFIG_YAFFS2
#define CONFIG_RBTREE
#endif

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#endif /* __CONFIG_H */
