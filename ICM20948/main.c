#include <ICM20948_driver.h>
#include <TSL2561_driver.h>
#include <stdio.h>
#include "msp.h"
#include "Clock.h"
#include "gpio.h"
#include "i2c.h"
#include "driverlib.h"
#include "rom_map.h"
#include "i2c_driver.h"
#include "Uart.h"

#define         I2C_MASTER_DEVICE       EUSCI_B3_BASE
double          lux = 0.0;
static          uint32_t                TSL2561_Int_Status = 0x01;

void main(void)
{
    uint8_t tsl2561_device_id = 0x00;
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    uint16_t magX, magY, magZ;


    Clock_Init48MHz();
    MAP_WDT_A_holdTimer();


    initI2C(I2C_MASTER_DEVICE);

#if 1
    ICM20948_PowerOn(I2C_MASTER_DEVICE, UB0_PWR_MGMNT_1_DEV_RESET);

    ICM20948_PowerOn(I2C_MASTER_DEVICE, UB0_PWR_MGMT_1_AUTO_CLOCK);

    ICM20948_GyroConfigInit(I2C_MASTER_DEVICE,
                                UB2_GYRO_CONFIG_1_FS_SEL_1000DPS,
                                UB2_GYRO_CONFIG_1_DLPFCFG_12HZ);


    ICM20948_AccelConfigInit(I2C_MASTER_DEVICE,
                                 UB2_ACCEL_CONFIG_FS_SEL_8G,
                                 UB2_ACCEL_CONFIG_DLPF_BANDWIDTH_111HZ);

    ICM20948_EnableI2cMaster(I2C_MASTER_DEVICE);

    ICM20948_AccelGyroOnOrOff(I2C_MASTER_DEVICE, UB0_PWR_MGMT2_ACCEL_GYRO_ON);

    ICM20948_InitMag(I2C_MASTER_DEVICE);

    while (1) {

        AK09916_GetMag(I2C_MASTER_DEVICE, &magX, &magY, &magZ);
        printf("MagX = %d  MagY = %d MagZ = %d\n", (int16_t)magX, (int16_t)magY, (int16_t)magZ);
//        ICM20948_GetAccel(I2C_MASTER_DEVICE, &accelX, &accelY, &accelZ);
//        printf("accelX = %f  accelY = %f accelZ = %f\n", accelX, accelY, accelZ);
//        ICM20948_GetGyro(I2C_MASTER_DEVICE, &gyroX, &gyroY, &gyroZ);
//        printf("gyroX = %f  gyroY = %f gyroZ = %f\n", gyroX, gyroY, gyroZ);

    }
#endif


}

/* GPIO ISR */
void PORT5_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    printf("%s run\n", __func__);
    TSL2561_Int_Status = 0x00;
}

