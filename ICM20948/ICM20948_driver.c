/*
 *  ICM_20948_driver.c
 *
 *  Created on: 2019年7月1日
 *      Author: zhangyongjie
 */


#include "ICM20948_driver.h"
#include "i2c_driver.h"
#include <Clock.h>


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#define     INFO        1
#define     ALWAYS      2

#define     Dprintf(level, LOG, arg...)      \
    do {            \
        if(level >= INFO)  \
            printf(LOG, ##arg); \
    }while(0)


static uint8_t SLAVE_ADDR = 0x68;

void set_SLAVE_ADDR(uint8_t addr)
{
    SLAVE_ADDR = addr;
}


/* FUNCTION: 获取ID号
 *
 * 参数说明：
 * @i2c_module   I2C设备
 *
 * return
 *          返回设备ID号，否则返回 -1
 *
*/
int16_t ICM20948_Get_WHO_AM_I(int i2c_module)
{
    uint8_t rxBuf = SELECT_USER_BANK0;
    if (writeI2C(i2c_module, SLAVE_ADDR, REG_BANK_SEL, &rxBuf, 1) == false)
        return -1;

    if (readI2C(i2c_module, SLAVE_ADDR, UB0_WHO_AM_I, &rxBuf, 1) == true)
        return rxBuf;
    else
        return -1;
}


/* FUNCTION: 切换bank内存分区
 *
 * 参数说明：
 * @i2c_module   I2C设备
 * @user_bank     bank分区编号
 *          SELECT_USER_BANK0
 *          SELECT_USER_BANK1
 *          SELECT_USER_BANK2
 *          SELECT_USER_BANK3
 * return
 *          成功返回true,否则返回false
 *
*/
static bool ICM20948_ChangeUserBank(int i2c_module, __REG_BANK_SEL_SELECT_USER_BANK user_bank)
{
    uint8_t txBuf = user_bank;
    if (writeI2C(i2c_module, SLAVE_ADDR, REG_BANK_SEL, &txBuf, 1) == false)
        return false;

    return true;

}

/*
 * FUNCTION: 设置芯片时钟源
 *
 * @i2c_module      I2C设备
 * @clock           时钟类型
 *
 *Ret:
 *     成功返回True，否则返回false
 * */

static bool ICM20948_SetClock(int i2c_module, UB0_PWR_MGMT_1_REG_VALUE clock)
{
    uint8_t txBuf = 0x00;

    txBuf = clock;
    if (writeI2C(i2c_module, SLAVE_ADDR, UB0_PWR_MGMT_1, &txBuf, 1) == false)
        return false;

    return true;
}

/*
 * FUNCTION: 重置芯片内存寄存器
 *
 * @i2c_module  i2c控制器
 *
 * */

bool ICM20948_Reset(i2c_module)
{
    uint8_t txBuf = UB0_PWR_MGMNT_1_DEV_RESET;
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }
    Clock_Delay1ms(10);

    if (writeI2C(i2c_module, SLAVE_ADDR, UB0_PWR_MGMT_1, &txBuf, 1) == false)
        return false;

    printf("ICM20948_Reset OK\n");
    return true;
}

/*
 *
 * function: 将传感器从睡眠模式唤醒
 *
 * @mode   唤醒模式
 *
 * */

bool ICM20948_PowerOn(int i2c_module, UB0_PWR_MGMT_1_REG_VALUE mode)
{
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }

    // set master clock and wake up
    if (ICM20948_SetClock(i2c_module, mode) == false)
        return false;

    return true;
}

bool ICM20948_TempConfigInit(int i2c_module, __UB2_TEMP_CONFIG_TEMP_DLPFCFG rate)
{
    uint8_t txBuf;

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK2) == false) {
        return false;
    }
    Clock_Delay1ms(10);

    txBuf = rate;

    if(writeI2C(i2c_module, SLAVE_ADDR, UB2_TEMP_CONFIG, &txBuf, 1) == false)
        return false;
    return true;

}


bool ICM20948_AccelGyroOnOrOff(int i2c_module, __UB0_PWR_MGMT_2_ACCEL_GYRO_MODE ub0_accel_gyro_mode)
{
    uint8_t txBuf;

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }
    Clock_Delay1ms(10);

    txBuf = ub0_accel_gyro_mode;
    if (writeI2C(i2c_module, SLAVE_ADDR, UB0_PWR_MGMT_2, &txBuf, 1) == false)
        return false;

    return true;
}


bool ICM20948_GyroConfigInit(int i2c_module,
                       __UB2_GYRO_CONFIG_1_FS_SEL ub2_fs_sel,
                       __UB2_GYRO_CONFIG_1_DLPFCFG ub2_dlpfcfg)
{
    uint8_t txBuf = 0x00;
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK2) == false) {
        return false;
    }

    txBuf = 0x07;
    if (writeI2C(i2c_module, SLAVE_ADDR, UB2_GYRO_SMPLRT_DIV, &txBuf, 1) == false)
        return false;

    txBuf = ub2_fs_sel | ub2_dlpfcfg | 0x30;

    if(writeI2C(i2c_module, SLAVE_ADDR, UB2_GYRO_CONFIG_1, &txBuf, 1) == false)
        return false;

    Dprintf(ALWAYS, "function:%s   ub2 reg = %x  regValue = %x\n", __func__, UB2_GYRO_CONFIG_1, txBuf);

    return true;
}

bool ICM20948_AccelConfigInit(int i2c_module,
                              __UB2_ACCEL_CONFIG_FS_SEL ub2_fs_sel,
                              __UB2_ACCEL_CONFIG_DLPFCFG_BANDWIDTH ub2_dlpfcfg)
{
    uint8_t txBuf = 0x00;
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK2) == false) {
        return false;
    }

    txBuf = 0x00;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB2_ACCEL_SMPLRT_DIV_1, &txBuf, 1) == false)
        return false;
    Dprintf(ALWAYS, "function:%s reg = %x ub2 regValue = %x\n", __func__, UB2_ACCEL_SMPLRT_DIV_1, txBuf);

    txBuf = 0x07;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB2_ACCEL_SMPLRT_DIV_2, &txBuf, 1) == false)
        return false;
    Dprintf(ALWAYS, "function:%s reg = %x ub2 regValue = %x\n", __func__, UB2_ACCEL_SMPLRT_DIV_2, txBuf);

    txBuf = ub2_fs_sel | ub2_dlpfcfg | 0x30;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB2_ACCEL_CONFIG, &txBuf, 1) == false)
        return false;
    Dprintf(ALWAYS, "function:%s reg = %x ub2 regValue = %x\n", __func__, UB2_ACCEL_CONFIG, txBuf);

    return true;
}

bool ICM20948_EnableI2cMaster(int i2c_module)
{
    uint8_t txBuf = 0x00;
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }

    //enable I2c Master mode 、TEMP_EN
    txBuf = UB0_USER_CTRL_I2C_MST_EN;
    if (writeI2C(i2c_module, SLAVE_ADDR, UB0_USER_CTRL, &txBuf, 1) == false) {
        Dprintf(ALWAYS, "UB0 function:%s is ERROR reg = %x  regValue= %x \n",
                __func__, UB0_USER_CTRL, txBuf);
        return false;
    }

//    txBuf = 0x70;
//    writeI2C(i2c_module, SLAVE_ADDR, UB0_LP_CONFIG, &txBuf, 1);

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
         return false;
    }


    txBuf = UB3_I2C_MST_CTRL_CLK_400KHZ | UB3_I2C_MST_CTRL_I2C_MST_P_NSR_START | 0x01 << 7;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_MST_CTRL, &txBuf, 1) == false)
        return false;
    Dprintf(ALWAYS, "function:%s  ub3 reg = %x  regValue= %x \n",
            __func__, UB3_I2C_MST_CTRL, txBuf);


    txBuf = UB3_I2C_MST_DELAY_CTRL_I2C_SLV0_DELAY_EN;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_MST_DELAY_CTRL, &txBuf, 1) == false)
        return false;
    Dprintf(ALWAYS, "function:%s  ub3 reg = %x  regValue= %x \n",
            __func__, UB3_I2C_MST_DELAY_CTRL, txBuf);

    return true;
}

bool ICM20948_InitMag(int i2c_module)
{

    uint8_t txBuf = 0x00;
    int i = 0;
    uint8_t rxBuf[10] = { 0 };

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
        return false;
    }

    txBuf = UB3_I2C_SLV_ADDR_W | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = MAG_CNTL3_CONTROL3;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = 0x00;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_DO, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = 0x81;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    Clock_Delay1ms(100);
    txBuf = 0x00;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }


    txBuf = UB3_I2C_SLV_ADDR_W | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = MAG_CNTL2_CONTROL2;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = MSG_CNTL2_CONTIN_MEAS_MODE2;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_DO, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = 0x81;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    Clock_Delay1ms(100);
    txBuf = 0x00;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    ///////////////////////////////////////
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
        return false;
    }

    txBuf = UB3_I2C_SLV_ADDR_R | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_ADDR is Error\n");
        return false;
    }

    txBuf = 0x01;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_REG is Error\n");
        return false;
    }

    txBuf = 0x80 | 0x01;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_CTRL is Error\n");
        return false;
    }
    Clock_Delay1ms(100);

    uint8_t temp = 0x00;
    writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &temp, 1);


    ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0);

    if(readI2C(i2c_module, SLAVE_ADDR, UB0_EXT_SLV_SENS_DATA_00, rxBuf, 1) == false)
        return false;

    for (i = 0; i < 1; i++) {
        Dprintf(ALWAYS, "function: %s reg = %x  device ID = %x\n",
                __func__, UB0_EXT_SLV_SENS_DATA_00 + i, rxBuf[i]);
    }

    return true;
}


bool AK09916_GetMag(int i2c_module, uint16_t *magX, uint16_t *magY, uint16_t *magZ)
{
    int i = 0;
    uint8_t rxBuf[24] = { 0x00 };
    uint8_t txBuf = 0x00;

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
        return false;
    }
    txBuf = UB3_I2C_SLV_ADDR_R | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = MAG_HXL_TO_HZHMEASUREMENT_DATA;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }


    txBuf = 0x80 | 0x06;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    Clock_Delay1us(1000);


    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
           return false;
    }

    if(readI2C(i2c_module, SLAVE_ADDR, UB0_EXT_SLV_SENS_DATA_00, rxBuf, 6) == false)
        return false;
#if 1
    for (i = 0; i < 6; i++) {
        Dprintf(ALWAYS, "function: %s reg = %x  regValue = %x\n",
                __func__, UB0_EXT_SLV_SENS_DATA_00 + i, rxBuf[i]);
    }
#endif


    *magX = rxBuf[1] << 8 | rxBuf[0];
    *magY = rxBuf[3] << 8 | rxBuf[2];
    *magZ = rxBuf[5] << 8 | rxBuf[4];

    //读取磁力数据之后，必须读取ST2寄存器值
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
        return false;
    }
    txBuf = UB3_I2C_SLV_ADDR_R | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    txBuf = MAG_ST2_STATUS2;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }


    txBuf = 0x80 | 0x01;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    Clock_Delay1us(1000);


    txBuf = 0x00;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 reg is Error\n");
        return false;
    }

    //读取ST2值
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
           return false;
    }

    if(readI2C(i2c_module, SLAVE_ADDR, UB0_EXT_SLV_SENS_DATA_00, rxBuf, 6) == false)
        return false;
#if 1
    for (i = 0; i < 1; i++) {
        Dprintf(ALWAYS, "function: %s reg = %x  ST2 regValue = %x\n",
                __func__, UB0_EXT_SLV_SENS_DATA_00 + i, rxBuf[i]);
    }
#endif

    return true;
}


bool ICM20948_GetTemp(int i2c_module, float *temp, float RoomTemp_Offset)
{
    uint8_t rxBuf[2] = { 0x00 };
    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }

    if (readI2C(i2c_module, SLAVE_ADDR, UB0_TEMP_OUT_H, rxBuf, 2) == false)
        return false;

    //TEMP_degC = ((TEMP_OUT – RoomTemp_Offset)/Temp_Sensitivity) + 21degC
    *temp = ((rxBuf[0] << 8 | rxBuf[1]) - RoomTemp_Offset) / 333.87f + 21.0f;

    Dprintf(ALWAYS, "%s %02x %02x\n", __func__, rxBuf[0], rxBuf[1]);
    return true;
}

bool ICM20948_GetAccel(int i2c_module, float *accelX, float *accelY, float *accelZ)
{
    int i = 0;
    uint8_t rxBuf[6] = { 0x00 };
    uint8_t accelMeter = 0x00;

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK2) == false) {
         return false;
    }

    if(readI2C(i2c_module, SLAVE_ADDR, UB2_ACCEL_CONFIG, rxBuf, 1) == false) {
        return false;
    }

    switch (rxBuf[0] & (03 << 1)) {
    case UB2_ACCEL_CONFIG_FS_SEL_2G:
        accelMeter = 2.0f;
        break;
    case UB2_ACCEL_CONFIG_FS_SEL_4G:
        accelMeter = 4.0f;
        break;
    case UB2_ACCEL_CONFIG_FS_SEL_8G:
        accelMeter = 8.0f;
        break;
    case UB2_ACCEL_CONFIG_FS_SEL_16G:
        accelMeter = 16.0f;
        break;

    }

    printf("accelMeter = %d\n", accelMeter);

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }

    if (readI2C(i2c_module, SLAVE_ADDR, UB0_ACCEL_XOUT_H, rxBuf, 6) == false)
        return false;

    *accelX = (rxBuf[0] << 8 | rxBuf[1]) / accelMeter; //rxBuf[1] >> (rxBuf[0] & (03 << 1))
    *accelY = (rxBuf[2] << 8 | rxBuf[3]) / accelMeter; //rxBuf[1] >> (rxBuf[0] & (03 << 1))
    *accelZ = (rxBuf[4] << 8 | rxBuf[5]) / accelMeter; //rxBuf[1] >> (rxBuf[0] & (03 << 1))

#if 0
    for (i = 0; i < sizeof(rxBuf); i++ ) {
        Dprintf(ALWAYS, "%s reg = %02x %02x\n", __func__, UB0_ACCEL_XOUT_H + i, rxBuf[i]);
    }
#endif

    return true;
}

bool ICM20948_GetGyro(int i2c_module, float *gyroX, float *gyroY, float *gyroZ)
{
    int i = 0;
    uint8_t rxBuf[6] = { 0x00 };
    uint16_t gyroMeter = 0x0000;

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK2) == false) {
        return false;
    }

    if(readI2C(i2c_module, SLAVE_ADDR, UB2_GYRO_CONFIG_1, rxBuf, 1) == false) {
        return false;
    }

    switch (rxBuf[0] & (03 << 1)) {
    case UB2_GYRO_CONFIG_1_FS_SEL_250DPS:
        gyroMeter = 250.0f;
        break;
    case UB2_GYRO_CONFIG_1_FS_SEL_500DPS:
        gyroMeter = 500.0f;
        break;
    case UB2_GYRO_CONFIG_1_FS_SEL_1000DPS:
        gyroMeter = 1000.0f;
        break;
    case UB2_GYRO_CONFIG_1_FS_SEL_2000DPS:
        gyroMeter = 2000.0f;
        break;
    }

    printf("gyroMeter = %d\n", gyroMeter);

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK0) == false) {
        return false;
    }

    if (readI2C(i2c_module, SLAVE_ADDR, UB0_GYRO_XOUT_H, rxBuf, 6) == false)
        return false;

    *gyroX = (rxBuf[0] << 8 | rxBuf[1]) / gyroMeter;
    *gyroY = (rxBuf[2] << 8 | rxBuf[3]) / gyroMeter;
    *gyroZ = (rxBuf[4] << 8 | rxBuf[5]) / gyroMeter;

#if 1
    for (i = 0; i < sizeof(rxBuf); i++ ) {
        Dprintf(ALWAYS, "%s reg = %02x %02x\n", __func__, UB0_GYRO_XOUT_H + i, rxBuf[i]);
    }
#endif

    return true;
}



bool AK09916_GetDeviceId(int i2c_module)
{
    int i = 0;
    uint8_t temp = 0x00;
    uint8_t txBuf = 0x00;
    uint8_t rxBuf[24] = { 0x00 };

    if (ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3) == false) {
        return false;
    }

    txBuf = UB3_I2C_SLV_ADDR_R | AK09916_SLAVE_ADDR;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_ADDR, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_ADDR is Error\n");
        return false;
    }

    txBuf = MAG_WIA_DEVICE_ID;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_REG, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_REG is Error\n");
        return false;
    }

    txBuf = 0x80 | 0x01;
    if(writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &txBuf, 1) == false) {
        printf("Set AK09916 UB3_I2C_SLV0_CTRL is Error\n");
        return false;
    }
    Clock_Delay1us(10);

    if(readI2C(i2c_module, SLAVE_ADDR, UB0_EXT_SLV_SENS_DATA_00, rxBuf, 1) == false)
        return false;

    for (i = 0; i < 1; i++) {
        Dprintf(ALWAYS, "function: %s reg = %x  regValue = %x\n",
                __func__, UB0_EXT_SLV_SENS_DATA_00 + i, rxBuf[i]);
    }

    ICM20948_ChangeUserBank(i2c_module, SELECT_USER_BANK3);

    temp = 0x00;
    writeI2C(i2c_module, SLAVE_ADDR, UB3_I2C_SLV0_CTRL, &temp, 1);

    return true;
}

