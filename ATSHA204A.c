#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define	ATSHA204A_WAKE		0x11
#define	ATSHA204A_AWAKE_OK			0
#define	ATSHA204A_AWAKE_ERR			-1
#define	ATSHA204A_MAC_MODE			0x00
#define	ATSHA204A_MAC_SLOTID		0x00
#define	ATSHA204A_MATCH_KEY_OK		0x00
#define	ATSHA204A_MATCH_KEY_ERR		0x01


#define	ARRARY_SIZE(arr)		sizeof(arr)	/ sizeof(arr[0])
#define DEVICE_I2C_ADDR   "/dev/i2c-0"
#define DEVICE_ADDR 0x64

static int i2c_read(int fd, unsigned char *rbuf, int len)
{
	uint8_t buf[1] = {0x00};
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data i2c_message;

	i2c_message.msgs = msgs;
	i2c_message.nmsgs = 1;

	msgs[0].addr = DEVICE_ADDR;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = buf;
	i2c_message.nmsgs = 2;
	msgs[1].addr = DEVICE_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = len;
	msgs[1].buf = rbuf;

	int ret = 0;
	ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_message);
	return ret;
	//return read(fd, rbuf, len);
}
static int i2c_write(int fd, unsigned char *buf, int len)
{
	int status = -1;
	struct i2c_msg msgs[1];
	struct i2c_rdwr_ioctl_data i2c_message;

	i2c_message.nmsgs = 1;
	i2c_message.msgs = msgs;

	i2c_message.msgs[0].addr = DEVICE_ADDR;
	i2c_message.msgs[0].flags = 0;
	i2c_message.msgs[0].buf = buf;
	i2c_message.msgs[0].len = len;

	status = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_message);
	return status;
	//return write(fd, rbuf, len);
}

static void sha204c_calculate_crc( unsigned char  length, \
		unsigned char  *command_buf, unsigned char  *crc)
{
	unsigned char counter;
	unsigned short crc_register = 0;
	unsigned short polynom = 0x8005;
	unsigned char  shift_register;
	unsigned char  command_buf_bit, crc_bit;

	for (counter = 0; counter < length; counter++) {
		for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
			command_buf_bit = (command_buf[counter] & shift_register) ? 1 : 0;
			crc_bit = crc_register >> 15;
			crc_register <<= 1;
			if (command_buf_bit != crc_bit)
				crc_register ^= polynom;
		}
	}
	crc[0] = (unsigned char)(crc_register & 0x00FF);
	crc[1] = (unsigned char)(crc_register >> 8);
}


void print_error(uint8_t value)
{
	switch (value) {
		case 0x00:
			printf("\nSuccessful Command Execution\n");
			break;
		case 0x01:
			printf("\nCheckMac Miscompare\n");
			break;
		case 0x03:
			printf("\nParse Error\n");
			break;
		case 0x0F:
			printf("\nExecution Error\n");
			break;
		case 0x11:
			printf("\nAfter Wake,Prior to First Command\n");
			break;
		case 0xFF:
			printf("\nCRC or other Communications Error\n");
			break;
	}
	return ;
}

int devrev_comm(int fd, uint8_t *ret_value, int *ret_len)
{
	int ret;
	uint8_t reg[8] = {0x03, 0x07, 0x30, 0x00, 0x00, 0x00};
	*ret_len = 4 + 3;

	sha204c_calculate_crc(5, &reg[1], &reg[6]);
	ret = i2c_write(fd, reg, 8);
	usleep(10000);	

	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

/*
   COMM LEN 0x17 Param1 Param2

Param1: 1
Bit 0:  Zero for Configuration zone, one for Data and OTP zones.
Bits 1-6: Must be zero.
Bit 7:  If one, the check of the zone CRC is ignored and the zone is locked,
regardless of the state of the memory. Atmel does not recommend using this
mode.
Param2: 2
Summary of the designated zones, or should be 0x0000 if Zone[7] is set.
 */

int lock_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t zone)
{
	int ret;
	uint8_t reg[8] = {0x03, 0x07, 0x17, 0x00, 0x00, 0x00};
	reg[3] = zone;
	*ret_len = 1 + 3;

	sha204c_calculate_crc(5, &reg[1], &reg[6]);
	ret = i2c_write(fd, reg, 8);
	usleep(10000);	

	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

/*
   COMM, LEN, OPCODE, PARAM1 PARAM2 CRC

PARAM1:	
Bit 7: If one, 32 bytes are read; otherwise four bytes are read. Must be zero if
reading from OTP zone.
 */
int read_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t zone, uint8_t addr)
{
	int ret;
	uint8_t reg[8] = {0x03, 0x07, 0x02, 0x00, 0x00, 0x00};
	reg[3] = zone;
	reg[4] = addr;

	if ( zone >> 7 ) {
		*ret_len = 32 + 3;
	} else {
		*ret_len = 4 + 3;
	}
	printf("ret_len = %d\n", *ret_len);
	sha204c_calculate_crc(5, &reg[1], &reg[6]);
	ret = i2c_write(fd, reg, 8);
	usleep(10000);	

	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

int write_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t zone, \
		uint8_t addr, uint8_t *data1,uint8_t *data2)
{
	int ret;
	int write_len = 6;
	uint8_t reg[100] = {0x03, 0x07, 0x12, 0x00, 0x00, 0x00};
	reg[3] = zone;
	reg[4] = addr;
	*ret_len = 4; 
	if (zone & (0x1<<7)) {
		write_len  += 32;
		memcpy(&reg[6], data1, 32);
	} else {
		write_len += 4;
		memcpy(&reg[6], data1, 4);
	}
	write_len += 2;
	reg[1] = write_len -1;
	sha204c_calculate_crc(write_len -1 - 2, &reg[1], &reg[write_len - 2]);
	ret = i2c_write(fd, reg, write_len);
	usleep(10000);	

	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;

	return 0;
}


int nonce_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t mode,\
		uint8_t *rand)
{
	int ret;
	int write_len = 6;
	uint8_t reg[100] = {0x03, 0x00, 0x16, 0x03, 0x00, 0x00};
	reg[3] = mode;

	switch (mode) {
		case 0x00:
		case 0x01:
			memcpy(&reg[6], rand, 32);
			write_len += (32 + 2);
			reg[1] = write_len -1;
			*ret_len = 32 + 3;
			break;
		case 0x03:
			memcpy(&reg[6], rand, 32);
			write_len += (32 + 2);
			reg[1] = write_len -1;
			*ret_len = 1 + 3;
			break;
		default :
			return -1;
			break;
	}
	sha204c_calculate_crc(write_len - 1 - 2, &reg[1], &reg[write_len - 2]);
	ret = i2c_write(fd, reg, write_len);
	if ( ret < 0 ) {
		return -1;
	}
	usleep(100000);	
	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

int gendig_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t zone, \
		uint8_t slot, uint8_t *other_data)
{
	int ret;
	int write_len = 6;
	uint8_t reg[30] = {0x03, 0x07, 0x15, 0x00, 0x00, 0x00};
	reg[3] = zone;
	reg[4] = slot;

	sha204c_calculate_crc(5, &reg[1], &reg[6]);
	ret = i2c_write(fd, reg, 8);
	usleep(100000);	

	*ret_len = 4;
	ret = i2c_read(fd, ret_value, 4);
	return ret;
}
/*
is_update_eeprom:
If zero, then it will automatically update EEPROM seed only 
if necessary prior to random number generation.Recommended for highest security.
If one, then it will generate a random number using existing EEPROM seed; 
do not update EEPROM seed.
 */

int random_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t is_update_eeprom)
{
	int ret;	
	uint8_t reg[8] = {0x03, 0x07, 0x1B, 0x00, 0x00, 0x00, 0x24, 0xCD};
	reg[3] &= is_update_eeprom;
	sha204c_calculate_crc(5, &reg[1], &reg[6]);
	ret = i2c_write(fd, reg, 8);
	if ( ret < 0 ) {
		return -1;
	}

	usleep(400000);
	*ret_len = 32 + 3;
	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

int mac_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t mode, uint8_t slot, uint8_t *data)
{
	int ret;
	int write_len = 6;
	uint8_t reg[100] = {0x03, 0x00, 0x08, 0x00, 0x00, 0x00};
	reg[3] = mode;
	reg[4] = slot;
	if (mode & 0x1 ) {
		write_len += 2;	
	} else {
		write_len += (32 + 2);
		memcpy(&reg[6], data, 32);
	}
	reg[1] = write_len -1;
	sha204c_calculate_crc(write_len -1 -2, &reg[1], &reg[write_len -2]);

	ret = i2c_write(fd, reg, write_len);
	if ( ret < 0 )
		return -1;

	usleep(100000);	
	*ret_len = 32 + 3;
	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}
int get_checkmac_otherdata(int fd, uint8_t *value)
{
	int ret;
	char buf[10] = { 0 };
	memset(buf, 0x00, sizeof(buf));
	ret = read_comm(fd, buf, &ret, 0x00, 0x02);   //get the config zone SN[4:7]
	if ( ret < 0 )
		return -1;
	memcpy(value, &buf[1], 4);

	memset(buf, 0x00, sizeof(buf));
	ret = read_comm(fd, buf, &ret, 0x00, 0x00);   //get the config zone SN[2:3]
	if ( ret < 0 )
		return -1;
	memcpy(&value[4], &buf[3], 2);

	return 0;
}

/*
   0x03 comm_len 0x54 mode		slot		client_chal client_resp			other_data   crc
   1		2				32           32					13        2
   MacCommand_slot   TempKey	MacCommandReceived   
other_data:
0x08 MacCommandMode	MacCommandSlotID  OTP[8:10] SN[4:7]  SN[2:3]
0000	0000		00
 */

int checkmac_comm(int fd, uint8_t *ret_value, int *ret_len, uint8_t mode, \
		uint8_t slot, uint8_t *client_chal, uint8_t *client_resp, uint8_t *other_data)
{
	int ret;
	int write_len = 6;
	uint8_t reg[100] = {0x03, 0x00, 0x28, 0x00, 0x00, 0x00};
	reg[3] = mode;
	reg[4] = slot;

	memcpy(&reg[6], client_chal, 32);
	memcpy(&reg[6 + 32], client_resp, 32);
	memcpy(&reg[6 + 64], other_data, 13);

	write_len += (32 + 32 + 13 + 2);
	reg[1] = write_len - 1;

	sha204c_calculate_crc(write_len -1 -2, &reg[1], &reg[write_len -2]);

	ret = i2c_write(fd, reg, write_len);
	usleep(200000);	

	*ret_len = 1 + 3;
	ret = i2c_read(fd, ret_value, *ret_len);
	return ret;
}

int dev_awake(int fd)
{
	uint8_t reset[] = {0x00};
	uint8_t ret_data[4];

	ioctl(fd, I2C_SLAVE_FORCE, (unsigned long)DEVICE_ADDR);

	ioctl(fd, I2C_TIMEOUT, 10);     
	ioctl(fd, I2C_RETRIES, 2);
	ioctl(fd, I2C_TENBIT, 0);

	i2c_write(fd, reset, 1);
	usleep(200000);
	memset(ret_data, 0x00, sizeof(ret_data));
	i2c_read(fd, ret_data, 4);
	if (ret_data[1] == ATSHA204A_WAKE) {
		return ATSHA204A_AWAKE_OK;
	} else {
		return ATSHA204A_AWAKE_ERR;
	}
}


int open_dev(const char *devname)
{
	int ret = 0;
	int fd = 0;
	fd = open(devname, O_RDWR);
	if (fd < 0) {
		return -1;
	}
	ret = dev_awake(fd);
	if ( ret == ATSHA204A_AWAKE_OK ) {
		return fd;
	} else {
		return ATSHA204A_AWAKE_ERR;
	}
}

int create_key(int fd, uint8_t *key)
{
	uint8_t ret_data[40];
	int ret_len = 0 ;
	nonce_comm(fd, ret_data, &ret_len, 0x00, key);
	if ( ret_data[0] == 35 ) 
		return 0;
	else
		return -1;
}

int check_key(int fd, uint8_t *key)
{
	int ret = 0;
	int ret_len = 0;
	uint8_t ret_data[100];
	uint8_t mac_received[32] = {0};
	uint8_t other_data[13] = {0};

	memset(ret_data, 0x00, sizeof(ret_data));
	ret = mac_comm(fd, ret_data, &ret_len, ATSHA204A_MAC_MODE, ATSHA204A_MAC_SLOTID, key);
	if ( ret < 0 )
		return -1;

	memcpy(mac_received, &ret_data[1], 32);

	memset(other_data, 0x00, sizeof(other_data));
	other_data[0] = 0x08;
	other_data[1] = ATSHA204A_MAC_MODE;
	other_data[2] = ATSHA204A_MAC_SLOTID;

	memset(ret_data, 0x00, sizeof(ret_data));
	checkmac_comm(fd, ret_data, &ret_len, 0x00, \
			ATSHA204A_MAC_SLOTID, key, mac_received, other_data);
	if ( ret_data[1] == ATSHA204A_MATCH_KEY_OK) {
		//printf("through \n");
		return ATSHA204A_MATCH_KEY_OK;
	} else {
		//printf("gome over \n");
		return ATSHA204A_MATCH_KEY_ERR;
	}
}


int random_key(int fd, uint8_t *ret_key)
{
	int ret = 0;
	int ret_len = 0;
	uint8_t ret_data[40];
	ret = random_comm(fd, ret_data, &ret_len, 0x00);
	if ( ret < 0 ) {
		return -1;
	} else {
		memcpy(ret_key, &ret_data[1], 32);
		return 0;
	}
}

int close_dev(int fd)
{
	return close(fd);
}

int main(int argc, char **argv)
{
	int ret = 0;
	int fd = 0;
	fd = open_dev(DEVICE_I2C_ADDR);
	if ( fd < 0 ) {
		goto err;
	}
	uint8_t key[32] = {0xB7, 0x58, 0x66, 0xB3, 0x4E, 0xC8, 0x18, 0x60,\
		0x89, 0xAF, 0x84, 0x55, 0xA2, 0x77, 0x04, 0xBC, 0x08, \
			0xAF, 0xE4, 0x68, 0x9C, 0x6A, 0x2B, 0x64, 0xEA, 0xD9, 0xB0, 0x8A,\
			0x16, 0xBA, 0x6E, 0xBD};

	/*
	ret = random_key(fd, key);
	if ( ret == 0 ) {
		int i = 0;
		while ( i < 32 ) {
			printf("%02x ", key[i++]);
		}
	}
	printf("\n");
	*/
	ret = create_key(fd, key);
	if ( ret < 0 ) {
		goto create_key_err;
	}

	ret = check_key(fd, key);
	if ( ret == ATSHA204A_MATCH_KEY_OK ) {
		printf("match OK\n");
	} else {
		printf("match failure! \n");
	}

	close_dev(fd);
	return 0;

check_key_err:

create_key_err:
	close_dev(fd);
err:
	return -1;
}
