#include "MPU6050.h"

I2C_HandleTypeDef *mpui2c;
struct MPU6050 *mpu;

uint32_t MPU6050Init(I2C_HandleTypeDef *i2cP, struct MPU6050 *mpuP, uint8_t dlpf_cfg, uint8_t fs_sel, uint8_t afs_sel){
	mpui2c = i2cP;
	mpu = mpuP;
	mpu->address = MPU6050_ADDRESS;
	mpu->DLPF_CFG = dlpf_cfg;
	mpu->FS_SEL = fs_sel;
	mpu->AFS_SEL = afs_sel;
	mpu->check = 0;
	mpu->accelX = 0;
	mpu->accelY = 0;
	mpu->accelZ = 0;
	mpu->gyroX = 0;
	mpu->gyroY = 0;
	mpu->gyroZ = 0;
	mpu->temp = 0.0;
	MPU6050BufferReset();
	MPU6050Start();
	MPU6050Check();
	return 0;
}

void MPU6050Start(void){
	MPU6050WriteRegister(MPU6050_CONFIG, mpu->DLPF_CFG);
	MPU6050WriteRegister(MPU6050_GYRO_CONFIG, mpu->FS_SEL << 3);
	MPU6050WriteRegister(MPU6050_ACCEL_CONFIG, mpu->AFS_SEL << 3);
	MPU6050WriteRegister(MPU6050_PWR_MGMT_1, 0);
}

void MPU6050ReadAccel(void){
	MPU6050ReadRegister(MPU6050_ACCEL_XOUT_H, 6);
	mpu->accelX = mpu->buff[0] << 8 | mpu->buff[1];
	mpu->accelY = mpu->buff[2] << 8 | mpu->buff[3];
	mpu->accelZ = mpu->buff[4] << 8 | mpu->buff[5];
	MPU6050BufferReset();
}

void MPU6050ReadTemp(void){
	MPU6050ReadRegister(MPU6050_TEMP_OUT_H, 2);
	mpu->temp = ((((float)(mpu->buff[0] << 8 | mpu->buff[1])) / 340) + 36.53) / 10;
	MPU6050BufferReset();
}

void MPU6050ReadGyro(void){
	MPU6050ReadRegister(MPU6050_GYRO_XOUT_H, 6);
	mpu->gyroX = mpu->buff[0] << 8 | mpu->buff[1];
	mpu->gyroY = mpu->buff[2] << 8 | mpu->buff[3];
	mpu->gyroZ = mpu->buff[4] << 8 | mpu->buff[5];
	MPU6050BufferReset();
}

void MPU6050WriteRegister(uint8_t address, uint8_t byte){
	mpu->buff[0] = address;
	mpu->buff[1] = byte;
	HAL_I2C_Master_Transmit(mpui2c, mpu->address, mpu->buff, 2, 100);
	MPU6050BufferReset();
}

void MPU6050ReadRegister(uint8_t address, uint16_t val){
	mpu->buff[0] = address;
	HAL_I2C_Master_Transmit(mpui2c, mpu->address, mpu->buff, 1, 100);
	HAL_I2C_Master_Receive(mpui2c, mpu->address, mpu->buff, val, 100);
}

void MPU6050BufferReset(void){
	for(int i = 0; i < MPU6050_STRUCT_BUFFER_LEN; i++){
		mpu->buff[i] = 0;
	}
}

void MPU6050Check(void){
	mpu->buff[0] = MPU6050_WHO_AM_I;
	HAL_I2C_Master_Transmit(mpui2c, mpu->address, mpu->buff, 1, 100);
	HAL_I2C_Master_Receive(mpui2c, mpu->address, mpu->buff, 1, 100);
	mpu->check = mpu->buff[0];
	MPU6050BufferReset();
}
