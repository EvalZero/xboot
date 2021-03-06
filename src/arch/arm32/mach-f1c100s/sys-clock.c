/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <f1c100s/reg-ccu.h>

static void wait_pll_stable(u32_t base)
{
	u32_t rval = 0;
	u32_t time = 0xfff;

	do {
		rval = read32(base);
		time--;
	} while(time && !(rval & (1 << 28)));
}

static void clock_set_pll_cpu(u32_t clk)
{
	u32_t n, k, m, p;
	u32_t rval = 0;
	u32_t div = 0;

	if(clk > 720000000)
		clk = 720000000;

	if((clk % 24000000) == 0)
	{
		div = clk / 24000000;
		n = div - 1;
		k = 0;
		m = 0;
		p = 0;
	}
	else if((clk % 12000000) == 0)
	{
		m = 1;
		div = clk / 12000000;
		if((div % 3) == 0)
			k = 2;
		else if((div % 4) == 0)
			k = 3;
		else
			k = 1;
		n = (div / (k + 1)) - 1;
		p = 0;
	}
	else
	{
		div = clk / 24000000;
		n = div - 1;
		k = 0;
		m = 0;
		p = 0;
	}

	rval = read32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
	rval &= ~((0x3 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	rval |= (1U << 31) | (p << 16) | (n << 8) | (k << 4) | m;
	write32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL, rval);
	wait_pll_stable(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
}

void sys_clock_init(void)
{
	write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME0, 0x1ff);
	write32(F1C100S_CCU_BASE + CCU_PLL_STABLE_TIME1, 0x1ff);
	write32(F1C100S_CCU_BASE + CCU_CPU_CFG, 0x00020000);
	write32(F1C100S_CCU_BASE + CCU_AHB_APB_CFG, 0x00012110);

	clock_set_pll_cpu(408000000);
}
