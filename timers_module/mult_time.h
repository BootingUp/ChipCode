/**================================================================
 *   Copyright (C) 2020  ZhangYiFei. All rights reserved.
 *   
 *   文件名称：mult_time.h
 *   创 建 者：ZhangYiFei
 *   创建日期：2020年01月13日
 *   描    述：
 *
 ================================================================**/

#ifndef		__MULT_TIME_H__
#define		__MULT_TIME_H__


#include <linux/ioctl.h>

#ifdef		__cplusplus
extern "C" {
#endif
	
	typedef struct mult_time {
		int timeout_s;
		int time_id;
	}mult_time_t;


#define		MULT_TIME_START			_IO('t', 0x03)
#define		MULT_TIME_STOP			_IO('t', 0x04)

#ifdef		__cplusplus
}
#endif


#endif
