#ifndef		__TSL2561_DRIVER__H__
#define		__TSL2561_DRIVER__H__

#include <linux/ioctl.h>

#define		TSL_EN_INT			_IO('t', 0x03)
#define		TSL_DIS_INT			_IO('t', 0x04)
#define		TSL_SET_THR			_IO('t', 0x05)
#define		TSL_GET_THR			_IO('t', 0x06)

typedef struct __tsl2561_thr_value
{
	unsigned char	data[4];
}tsl2561_thr_value;

#endif


