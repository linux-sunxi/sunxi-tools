void _start(void)
{
	clock_init();
	gpio_init();
	uart_init();
	s_init();
}

void dcache_enable(void)
{
}

void sunxi_wemac_initialize(void)
{
}

void *gd;
int gdata;

void preloader_console_init(void)
{
}

void hang(void)
{
	printf("Please reset the board!");
}

void udelay(unsigned long usec)
{
	__udelay(usec);
}

void sunxi_board_init(void)
{
	int power_failed = 1;
	int ramsize;

	timer_init();

	printf("DRAM:");
	ramsize = sunxi_dram_init();
	if (!ramsize) {
		printf(" ?");
		ramsize = sunxi_dram_init();
	}
	if (!ramsize) {
		printf(" ?");
		ramsize = sunxi_dram_init();
	}
	printf(" %dMB\n", ramsize>>20);
	if (!ramsize)
		hang();

#ifdef CONFIG_AXP209_POWER
	power_failed |= axp209_init();
	power_failed |= axp209_set_dcdc2(1400);
	power_failed |= axp209_set_dcdc3(1250);
	power_failed |= axp209_set_ldo2(3000);
	power_failed |= axp209_set_ldo3(2800);
	power_failed |= axp209_set_ldo4(2800);
#endif

	/*
	 * Only clock up the CPU to full speed if we are reasonably
	 * assured it's being powered with suitable core voltage
	 */
	if (!power_failed)
		clock_set_pll1(1008000000);
}

