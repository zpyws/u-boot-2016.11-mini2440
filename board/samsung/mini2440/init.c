/*
 * init.c
 *
 *  Created on: 2020/03/28
 *      Author: AnKun
 */
 //ported by yangwensen@20201221

#define NFCONF (*(volatile unsigned int *)0x4E000000)
#define NFCONT (*(volatile unsigned int *)0x4E000004)
#define NFCMD  (*(volatile unsigned char *)0x4E000008)
#define NFADDR (*(volatile unsigned char *)0x4E00000C)
#define NFDATA (*(volatile unsigned char *)0x4E000010)
#define NFSTAT (*(volatile unsigned char *)0x4E000020)

#define GPBCON (*(volatile unsigned int *)0x56000010)
#define GPBDAT (*(volatile unsigned int *)0x56000014)
#define GPBUP  (*(volatile unsigned int *)0x56000018)

#define mini2440_nand_select()    do{NFCONT &= ~(1 << 1);}while(0);
#define mini2440_nand_deselect()  do{NFCONT |= (1 << 1);}while(0);

static int is_boot_from_nor_flash(void)
{
	int tmp = 0;
	volatile int* p = (volatile int *)0;

	tmp = *p;
	*p = 0xAAAAAAAA;

	if (*p == 0xAAAAAAAA)   /* boot from nand */
	{
		*p = tmp;
		return 0;
	}
	else                    /* boot from nor */
	{
		return 1;
	}
}

void mini2440_nand_init(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0

	NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
	NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

static void mini2440_nand_delay(void)
{
	int delay = 0;
	for (delay = 0; delay < 10; delay++);
}

static void mini2440_nand_write_cmd(unsigned char cmd)
{
	NFCMD = cmd;
	mini2440_nand_delay();
}

static void mini2440_nand_write_addr(unsigned int addr)
{
	unsigned int col  = addr % 2048;
	unsigned int page = addr / 2048;

	NFADDR = col & 0xFF;
	mini2440_nand_delay();

	NFADDR = (col >> 8) & 0xFF;
	mini2440_nand_delay();

	NFADDR = page & 0xFF;
	mini2440_nand_delay();

	NFADDR = (page >> 8) & 0xFF;
	mini2440_nand_delay();

	NFADDR = (page >> 16) & 0xFF;
	mini2440_nand_delay();
}

static void mini2440_nand_wait_ready(void)
{
	while (!(NFSTAT & 0x1)) {};
}

void mini2440_nand_read(unsigned int addr, void* buf, unsigned int len)
{
	int i = 0;
	int col = addr % 2048;
	unsigned char* p = (unsigned char *)buf;

	mini2440_nand_select();
	while (i < len)
	{
		mini2440_nand_write_cmd(0x00);
		mini2440_nand_write_addr(addr);
		mini2440_nand_write_cmd(0x30);
		mini2440_nand_wait_ready();
		for (; ((col < 2048)&&(i < len)); col++)
		{
			*p++ = (NFDATA & 0xFF);
			i++;
			addr++;
		}
		col = 0;
	}
	mini2440_nand_deselect();
}

int mymemcmp(const void* addr1, const void* addr2, unsigned int len)
{
	char* p1 = (char *)addr1;
	char* p2 = (char *)addr2;

	while (len--)
	{
		if (*p1 != *p2)
			return 1;
		p1++;
		p2++;
	}
	return 0;
}

//by yangwensen@20201225
#define MINI2440_UBOOT_COPYTO  0x33f00000        /* 拷贝到内存中的地址,此值与CONFIG_SYS_TEXT_BASE一致,将其定在64MB内存的最高512KB*/
#define MINI2440_UBOOT_OFFSET  (4*1024)          /* u-boot在NAND中的偏移地址(NAND中的前4KB存放的是u-boot-spl.bin) */
#define MINI2440_UBOOT_SIZE    ((512-4)*1024)    /* u-boot的大小现在设定不得超过508K */

void copy_uboot_to_sdram(void)
{
	void (*jump)(void) = (void *)MINI2440_UBOOT_COPYTO;

	GPBDAT &= ~(1ul << 5);		//led1 on

	if (is_boot_from_nor_flash())
	{
		int len = MINI2440_UBOOT_SIZE;
		char* s = (char *)MINI2440_UBOOT_OFFSET;
		char* d = (char *)MINI2440_UBOOT_COPYTO;
		GPBDAT &= ~(1ul << 6);	//led2 on
		while (len--)    
			*d++ = *s++;
		GPBDAT &= ~(1ul << 8);	//led4 on
	}
	else
	{
		mini2440_nand_init();
		GPBDAT &= ~(1ul << 6);	//led2 on
		mini2440_nand_read(MINI2440_UBOOT_OFFSET, (void *)MINI2440_UBOOT_COPYTO, MINI2440_UBOOT_SIZE);
		GPBDAT &= ~(1ul << 7);	//led3 on, to indicate booting from nand flash
		GPBDAT &= ~(1ul << 8);	//led4 on
	}

	jump();
}

//by yangwensen@20201224
static void mini2440_led_init(void)
{
	GPBCON &= ~(0xfful<<10);								//clear bit-field[10..17]
	GPBCON |= ((1ul<<10)|(1ul<<12)|(1ul<<14)|(1ul<<16));	//PB[5..8]
	GPBUP  |= ((1ul<<5)|(1ul<<6)|(1ul<<7)|(1ul<<8));		//disable pull-up resistor(PB[0..10])
	GPBDAT |= ((1ul<<5)|(1ul<<6)|(1ul<<7)|(1ul<<8));		//all leds off 	 
}

static void clear_bss(void)
{
	extern int __bss_start;
	extern int __bss_end;

	int* p = &__bss_start;

	while (p < &__bss_end)    
		*p++ = 0;
}

void mini2440_board_init_f(void)
{
	clear_bss();
	mini2440_led_init();
	copy_uboot_to_sdram();
}
