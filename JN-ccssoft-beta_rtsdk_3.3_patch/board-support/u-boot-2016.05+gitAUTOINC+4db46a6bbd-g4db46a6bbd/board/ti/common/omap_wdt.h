#ifndef OMAP_WDT_H
#define OMAP_WDT_H

#define OMAP_WDT_BASE       0x4AE14000
#define OMAP_WDT_WCLR       0x24
#define OMAP_WDT_WCRR       0x28
#define OMAP_WDT_WLDR       0x2C
#define OMAP_WDT_WTGR       0x30
#define OMAP_WDT_WWPS       0x34
#define OMAP_WDT_WDLY       0x44
#define OMAP_WDT_WSPR       0x48

#define WWPS_PEND_WCLR      BIT(0)
#define WWPS_PEND_WCRR      BIT(1)
#define WWPS_PEND_WLDR      BIT(2)
#define WWPS_PEND_WTGR      BIT(3)
#define WWPS_PEND_WSPR      BIT(4)
#define WWPS_PEND_WDLY      BIT(5)

#define TIMER_MARGIN_MAX        (24 * 60 * 60)	/* 1 day */
#define TIMER_MARGIN_DEFAULT        60	/* 60 secs */
#define TIMER_MARGIN_MIN        1

#define PTV     0
#define GET_WLDR_VAL(secs)      (0xffffffff - ((secs) * (32768/(1<<PTV))) + 1)
#define GET_WCCR_SECS(val)      ((0xffffffff - (val) + 1) / (32768/(1<<PTV)))

void omap_wdt_init(void);

#endif /* OMAP_WDT_H */
