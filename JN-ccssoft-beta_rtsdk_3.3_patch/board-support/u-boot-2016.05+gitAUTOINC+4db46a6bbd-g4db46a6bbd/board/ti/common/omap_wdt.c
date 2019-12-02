#include <common.h>
#include <watchdog.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/arch/cpu.h>
#include "omap_wdt.h"

/* Hardware timeout in seconds */
#define OMAP_WDT_TIMEOUT        180

static unsigned int ttgr_value = 0x1234;
void omap_wdt_reload(void)
{
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WTGR)
        ;
    ttgr_value = ~ttgr_value;
    writel(ttgr_value, OMAP_WDT_BASE + OMAP_WDT_WTGR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WTGR)
        ;
    mdelay(10);
}

void omap_wdt_disable(void)
{
    writel(0xAAAA, OMAP_WDT_BASE + OMAP_WDT_WSPR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WSPR)
        ;

    writel(0x5555, OMAP_WDT_BASE + OMAP_WDT_WSPR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WSPR)
        ;
    mdelay(10);
}

void omap_wdt_enable(void)
{
    writel(0xBBBB, OMAP_WDT_BASE + OMAP_WDT_WSPR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WSPR)
        ;

    writel(0x4444, OMAP_WDT_BASE + OMAP_WDT_WSPR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WSPR)
        ;
    mdelay(10);
}

void omap_wdt_set_prescaler(void)
{
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WCLR)
        ;
    writel(BIT(5) | (PTV<<2), OMAP_WDT_BASE + OMAP_WDT_WCLR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WCLR)
        ;
    mdelay(10);
}

static int omap_wdt_set_timeout(unsigned int timeout)
{
    u32 timer_load = GET_WLDR_VAL(timeout);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WLDR)
        ;
    writel(timer_load, OMAP_WDT_BASE + OMAP_WDT_WLDR);
    while ((readl(OMAP_WDT_BASE + OMAP_WDT_WWPS)) & WWPS_PEND_WLDR)
        ;
    mdelay(10);

    return 0;
}

void omap_wdt_init(void)
{
    omap_wdt_disable();
    omap_wdt_set_prescaler();
    omap_wdt_set_timeout(OMAP_WDT_TIMEOUT);
    omap_wdt_reload();
    omap_wdt_enable();
}

