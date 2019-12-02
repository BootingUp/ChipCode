/*
 *  ICM_20948_driver.h
 *
 *  Created on: 2019年7月1日
 *      Author: zhangyongjie
 */

#ifndef ICM_20948_DRIVER_H_
#define ICM_20948_DRIVER_H_
#include <stdint.h>
#include <stdbool.h>

typedef enum REG_BANK_SEL_SELECT_USER_BANK{
    SELECT_USER_BANK0   =   (0x00 << 4),
    SELECT_USER_BANK1   =   (0x01 << 4),
    SELECT_USER_BANK2   =   (0x02 << 4),
    SELECT_USER_BANK3   =   (0x03 << 4),
}__REG_BANK_SEL_SELECT_USER_BANK;

//通用寄存器
#define     REG_BANK_SEL            0x7F


typedef enum MSG_CNTL2_CONTROL2_OPS_MODE {
    MSG_CNTL2_POWER_DOWN_MODE           = 0x00,     // Power-down mode
    MSG_CNTL2_SINGLE_MEAS_MODE          = 0x01,     //Single measurement mode
    MSG_CNTL2_CONTIN_MEAS_MODE1         = 0x01 << 1, //Continuous measurement mode 1
    MSG_CNTL2_CONTIN_MEAS_MODE2         = 0x01 << 2, //Continuous measurement mode 2
    MSG_CNTL2_CONTIN_MEAS_MODE3         = 0x03 << 1, //Continuous measurement mode 3
    MSG_CNTL2_CONTIN_MEAS_MODE4         = 0x01 << 3, //Continuous measurement mode 4
    MSG_CNTL2_SELF_TEST_MODE            = 0x01 << 4,   //Self-test mode

}__MSG_CNTL2_CONTROL2_OPS_MODE;

#define MAG_CNTL3_RESET     0x01

//磁力计寄存器的详细说明
typedef enum MAGNETOMETER_REG{
    MAG_WIA_DEVICE_ID                   = 0x01,
    MAG_ST1_STATUS1                     = 0x10,
    MAG_HXL_TO_HZHMEASUREMENT_DATA      = 0x11,     //0x11 ~ 0x16  HXL、HXH、HYL、HYH、HZL、HZH data lower 8bit
    MAG_ST2_STATUS2                     = 0x18,
    MAG_CNTL2_CONTROL2                  = 0x31,
    MAG_CNTL3_CONTROL3                  = 0x32,
    MAG_TS1_TEST1                       = 0x33,
    MAG_TS2_TS2_TEST2                   = 0x34,

}__MAGNETOMETER_REG;



typedef enum UB0_PWR_MGMT_1_REG_VALUE {
    UB0_PWR_MGMT_1_AUTO_CLOCK                   = 0x01, // no low power
    UB0_PWR_MGMT_1_AUTO_CLOCK_20MHz             = 0x06,
    UB0_PWR_MGMT_1_STOP_CLOCK_AND_KEEP_TIMING   = 0x07,
    UB0_PWR_MGMNT_1_DEV_RESET                   = 0x80,
    UB0_PWR_MGMNT_1_SLEEP                       = 0x01 << 6,
}UB0_PWR_MGMT_1_REG_VALUE;


typedef enum UB0_PWR_MGMT_2_ACCEL_GYRO_MODE {
    UB0_PWR_MGMT2_ACCEL_GYRO_ON                  = 0X00,
    UB0_PWR_MGMT2_ACCEL_GYRO_OFF                 = 0X3F,
}__UB0_PWR_MGMT_2_ACCEL_GYRO_MODE;

#define UB0_USER_CTRL_I2C_MST_EN        0x20
#define UB0_PWR_MGMT_2_SEN_ENABLE      0x00

//bank0 寄存器
typedef enum USER_BANK0 {
    UB0_WHO_AM_I                = 0x00,
    UB0_USER_CTRL               = 0x03,
    UB0_LP_CONFIG               = 0x05,
    UB0_PWR_MGMT_1              = 0x06,
    UB0_PWR_MGMT_2              = 0x07,
    UB0_INT_PIN_CFG             = 0x0F,
    UB0_INT_ENABLE              = 0x10,
    UB0_INT_ENABLE_1            = 0x11,
    UB0_INT_ENABLE_2            = 0x12,
    UB0_INT_ENABLE_3            = 0x13,
    UB0_I2C_MST_STATUS          = 0x17,
    UB0_INT_STATUS              = 0x19,
    UB0_INT_STATUS_1            = 0x1A,
    UB0_INT_STATUS_2            = 0x1B,
    UB0_INT_STATUS_3            = 0x1C,
    UB0_DELAY_TIMEH             = 0x28,
    UB0_DELAY_TIMEL             = 0x29,
    UB0_ACCEL_XOUT_H            = 0x2D,
    UB0_ACCEL_XOUT_L            = 0x2E,
    UB0_ACCEL_YOUT_H            = 0x2F,
    UB0_ACCEL_YOUT_L            = 0x30,
    UB0_ACCEL_ZOUT_H            = 0x31,
    UB0_ACCEL_ZOUT_L            = 0x32,
    UB0_GYRO_XOUT_H             = 0x33,
    UB0_GYRO_XOUT_L             = 0x34,
    UB0_GYRO_YOUT_H             = 0x35,
    UB0_GYRO_YOUT_L             = 0x36,
    UB0_GYRO_ZOUT_H             = 0x37,
    UB0_GYRO_ZOUT_L             = 0x38,
    UB0_TEMP_OUT_H              = 0x39,
    UB0_TEMP_OUT_L              = 0x3A,
    UB0_EXT_SLV_SENS_DATA_00    = 0x3B, //DATA_00  ~ DATA_23 (0x52)
    UB0_FIFO_EN_1               = 0x66,
    UB0_FIFO_EN_2               = 0x67,
    UB0_FIFO_RST                = 0x68,
    UB0_FIFO_MODE               = 0x69,
    UB0_FIFO_COUNTH             = 0x70,
    UB0_FIFO_COUNTL             = 0x71,
    UB0_FIFO_R_W                = 0x72,
    UB0_DATA_RDY_STATUS         = 0x74,
    UB0_FIFO_CFG                = 0x76,
}__USER_BANK0;


//bank1 USER_BANK1

typedef enum USER_BANK1 {
    UB1_SELF_TEST_X_GYRO        = 0x02,
    UB1_SELF_TEST_Y_GYRO        = 0x03,
    UB1_SELF_TEST_Z_GYRO        = 0x04,
    UB1_SELF_TEST_X_ACCEL       = 0x0E,
    UB1_SELF_TEST_Y_ACCEL       = 0x0F,
    UB1_SELF_TEST_Z_ACCEL       = 0x10,
    UB1_XA_OFFS_H               = 0x14,
    UB1_XA_OFFS_L               = 0x15,
    UB1_YA_OFFS_H               = 0x17,
    UB1_YA_OFFS_L               = 0x18,
    UB1_ZA_OFFS_H               = 0x1A,
    UB1_ZA_OFFS_L               = 0x1B,
    UB1_TIMEBASE_CORRECTION_PLL = 0x28,
}__USER_BANK1;


#define     UB2_ACCEL_CONFIG_FCHOICE                (0x01)

typedef enum UB2_ACCEL_CONFIG_FS_SEL
{
    UB2_ACCEL_CONFIG_FS_SEL_2G              = 0x00 << 1,
    UB2_ACCEL_CONFIG_FS_SEL_4G              = 0x01 << 1,
    UB2_ACCEL_CONFIG_FS_SEL_8G              = 0x02 << 1,
    UB2_ACCEL_CONFIG_FS_SEL_16G             = 0x03 << 1,
}__UB2_ACCEL_CONFIG_FS_SEL;


typedef enum UB2_ACCEL_CONFIG_DLPFCFG_BANDWIDTH
{
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_1209HZ      = 0x00,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_246HZ       = (0x00 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_111HZ       = (0x02 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_50HZ        = (0x03 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_24HZ        = (0x04 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_12HZ        = (0x05 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_6HZ         = (0x06 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
    UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_473HZ       = (0x07 << 3) | UB2_ACCEL_CONFIG_FCHOICE,
}__UB2_ACCEL_CONFIG_DLPFCFG_BANDWIDTH;

#define     USB2_GYRO_CONFIG_1_FCHOICE              (0x01)

typedef enum UB2_GYRO_CONFIG_1_FS_SEL {
    UB2_GYRO_CONFIG_1_FS_SEL_250DPS     = 0x00 << 1,
    UB2_GYRO_CONFIG_1_FS_SEL_500DPS     = 0x01 << 1,
    UB2_GYRO_CONFIG_1_FS_SEL_1000DPS    = 0x02 << 1,
    UB2_GYRO_CONFIG_1_FS_SEL_2000DPS    = 0x03 << 1,
}__UB2_GYRO_CONFIG_1_FS_SEL;


typedef enum UB2_GYRO_CONFIG_1_DLPFCFG {
    UB2_GYRO_CONFIG_1_DLPFCFG_12106HZ       = 0x00,
    UB2_GYRO_CONFIG_1_DLPFCFG_197HZ         = (0x00 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_152HZ         = (0x01 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_120HZ         = (0x02 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_51HZ          = (0x03 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_24HZ          = (0x04 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_12HZ          = (0x05 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_6HZ           = (0x06 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
    UB2_GYRO_CONFIG_1_DLPFCFG_361HZ         = (0x07 << 3) | USB2_GYRO_CONFIG_1_FCHOICE,
}__UB2_GYRO_CONFIG_1_DLPFCFG;

typedef enum UB2_TEMP_CONFIG_TEMP_DLPFCFG {
    UB2_TEMP_CONFIG_TEMP_RATE_79320HZ               = 0x00,
    UB2_TEMP_CONFIG_TEMP_RATE_2179HZ                = 0x01,
    UB2_TEMP_CONFIG_TEMP_RATE_1235HZ                = 0x02,
    UB2_TEMP_CONFIG_TEMP_RATE_659HZ                 = 0x03,
    UB2_TEMP_CONFIG_TEMP_RATE_341HZ                 = 0x04,
    UB2_TEMP_CONFIG_TEMP_RATE_173HZ                 = 0x05,
    UB2_TEMP_CONFIG_TEMP_RATE_88HZ                  = 0x06,
    UB2_TEMP_CONFIG_TEMP_RATE_79320HZ_REP           = 0x07,
}__UB2_TEMP_CONFIG_TEMP_DLPFCFG;


//bank2
typedef enum USER_BANK2 {
    UB2_GYRO_SMPLRT_DIV         = 0x00,
    UB2_GYRO_CONFIG_1           = 0x01,
    UB2_GYRO_CONFIG_2           = 0x02,
    UB2_XG_OFFS_USRH            = 0x03,
    UB2_XG_OFFS_USRL            = 0x04,
    UB2_YG_OFFS_USRH            = 0x05,
    UB2_YG_OFFS_USRL            = 0x06,
    UB2_ZG_OFFS_USRH            = 0x07,
    UB2_ZG_OFFS_USRL            = 0x08,
    UB2_ODR_ALIGN_EN            = 0x09,
    UB2_ACCEL_SMPLRT_DIV_1      = 0x10,
    UB2_ACCEL_SMPLRT_DIV_2      = 0x11,
    UB2_ACCEL_INTEL_CTRL        = 0x12,
    UB2_ACCEL_WOM_THR           = 0x13,
    UB2_ACCEL_CONFIG            = 0x14,
    UB2_ACCEL_CONFIG_2          = 0x15,
    UB2_FSYNC_CONFIG            = 0x52,
    UB2_TEMP_CONFIG             = 0x53,
    UB2_MOD_CTRL_USR            = 0x54,
}__USER_BANK2;


//bank3

/*
 * I2C主时钟频率可通过寄存器I2C_MST_CLK设置。
 * 由于温度变化以及不同功耗模式下系统时钟频率的部件间差异，
 * 应设置I2C_MST_CLK，使得在所有条件下时钟频率都不会 超过从设备可以支持的内容。
 * 要实现400 kHz，MAX的目标时钟频率，
 * 建议设置I2C_MST_CLK = 7（占空比为345.6 kHz / 46.67％）。
 * */

typedef enum UB3_I2C_MST_CTRL_CLK {
    UB3_I2C_MST_CTRL_CLK_370_29KHZ      = 0x02, //370.29 / 50.00%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_432_00KHZ      = 0x03, //432.00  / 50.00%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_370_29KHZ_42   = 0x04, //370.29  / 42.86%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_370_29KHZ_50   = 0x05, //370.29  / 50.00%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_345_60KHZ_40   = 0x06, //345.60  / 40.00%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_400KHZ         = 0x07, //345.60  / 46.67%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_304_94KHZ      = 0x08, //304.94  / 47.06%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_432_00KHZ_50   = 0x09, //432.00  / 50.00%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_432_00KHZ_41   = 0x0A, //432.00  / 41.67%%(DUTY CYCLE)
    UB3_I2C_MST_CTRL_CLK_471_27KHZ      = 0x0C, //471.27  / 45.45%
    UB3_I2C_MST_CTRL_CLK_345_60KHZ_46   = 0x0E, //345.60 / 46.67%
}__UB3_I2C_MST_CTRL_CLK;

#define       UB3_I2C_MST_CTRL_I2C_MST_P_NSR_RESTART    (0x00 << 4)
#define       UB3_I2C_MST_CTRL_I2C_MST_P_NSR_START      (0x01 << 4)
#define       UB3_I2C_MST_CTRL_MULT_MST_EN              (0x01 << 7)

#define       UB3_I2C_MST_DELAY_CTRL_I2C_SLV0_DELAY_EN  (0x01)

typedef enum UB3_I2C_SLV0_RW {
    UB3_I2C_SLV_ADDR_R         = 0x80,
    UB3_I2C_SLV_ADDR_W         = 0x00,
}__UB3_I2C_SLV0_RW;

#define         AK09916_SLAVE_ADDR      (0x0c)

typedef enum USER_BANK3 {
    UB3_I2C_MST_ODR_CONFIG      = 0x00,
    UB3_I2C_MST_CTRL            = 0x01,
    UB3_I2C_MST_DELAY_CTRL      = 0x02,
    UB3_I2C_SLV0_ADDR           = 0x03,
    UB3_I2C_SLV0_REG            = 0x04,
    UB3_I2C_SLV0_CTRL           = 0x05,
    UB3_I2C_SLV0_DO             = 0x06,
    UB3_I2C_SLV1_ADDR           = 0x07,
    UB3_I2C_SLV1_REG            = 0x08,
    UB3_I2C_SLV1_CTRL           = 0x09,
    UB3_I2C_SLV1_DO             = 0x0A,
    UB3_I2C_SLV2_ADDR           = 0x0B,
    UB3_I2C_SLV2_REG            = 0x0C,
    UB3_I2C_SLV2_CTRL           = 0x0D,
    UB3_I2C_SLV2_DO             = 0x0E,
    UB3_I2C_SLV3_ADDR           = 0x0F,
    UB3_I2C_SLV3_REG            = 0x10,
    UB3_I2C_SLV3_CTRL           = 0x11,
    UB3_I2C_SLV3_DO             = 0x12,
    UB3_I2C_SLV4_ADDR           = 0x13,
    UB3_I2C_SLV4_REG            = 0x14,
    UB3_I2C_SLV4_CTRL           = 0x15,
    UB3_I2C_SLV4_DO             = 0x16,
    UB3_I2C_SLV4_DI             = 0x17,
}__USER_BANK3;

bool ICM20948_PowerOn(int i2c_module, UB0_PWR_MGMT_1_REG_VALUE mode);

bool ICM20948_GyroConfigInit(int i2c_module,
                             __UB2_GYRO_CONFIG_1_FS_SEL ub2_fs_sel,
                             __UB2_GYRO_CONFIG_1_DLPFCFG ub2_dlpfcfg);
bool ICM20948_AccelConfigInit(int i2c_module,
                              __UB2_ACCEL_CONFIG_FS_SEL ub2_fs_sel,
                              __UB2_ACCEL_CONFIG_DLPFCFG_BANDWIDTH ub2_dlpfcfg);
bool ICM20948_EnableI2cMaster(int i2c_module);
bool AK09916_GetDeviceId(int i2c_module);
bool ICM20948_AccelGyroOnOrOff(int i2c_module, __UB0_PWR_MGMT_2_ACCEL_GYRO_MODE ub0_accel_gyro_mode);
bool AK09916_InitMag(int i2c_module);
bool ICM20948_GetMag(int i2c_module, uint16_t *magX, uint16_t *magY, uint16_t *magZ);
bool ICM20948_GetAccel(int i2c_module, float *accelX, float *accelY, float *accelZ);
bool ICM20948_GetGyro(int i2c_module, float *gyroX, float *gyroY, float *gyroZ);

#endif /* ICM_20948_DRIVER_H_ */
