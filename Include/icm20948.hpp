/*
 * icm20948.hpp
 *
 *  Created on: Mar 29, 2024
 *      Author: mbeuler
 */

#ifndef ZULS_INCLUDE_ICM20948_HPP_
#define ZULS_INCLUDE_ICM20948_HPP_

#include <stdlib.h>  // For abs() function


#if defined (STM32F411xE)
    #include "stm32f4xx_spi.hpp"
#elif defined (STM32H743xx)
    #include <stm32h7xx_spi.hpp>
#else
    #error "Please select first the target device used in your application (via preprocessor)"
#endif


#define IS_VALID_FULL_SCALE(FullScale1,FullScale2)    (FullScale1.ui8Selection == FullScale2.ui8Selection \
		                                            && FullScale1.fSensitivity == FullScale2.fSensitivity \
													&& FullScale1.ui16Range    == FullScale2.ui16Range)

#define IS_VALID_ACCEL_SAMPLE_RATE(SampleRate1,SampleRate2)    (SampleRate1.ui16Value == SampleRate2.ui16Value \
		                                                     && SampleRate1.boFCHOICE == SampleRate2.boFCHOICE \
														     && SampleRate1.ui16Div   == SampleRate2.ui16Div)

#define IS_VALID_GYRO_SAMPLE_RATE(SampleRate1,SampleRate2)    (SampleRate1.ui16Value == SampleRate2.ui16Value \
		                                                    && SampleRate1.boFCHOICE == SampleRate2.boFCHOICE \
														    && SampleRate1.ui8Div    == SampleRate2.ui8Div)

#define SAMPLES_MEAN_VALUE   1000
#define MAX_ITERATIONS       100     // This value MUST be less than 255

/* Default values of maximum calibration error */
constexpr int16_t ICM20948_ACCEL_PREC = 16; // 8;
constexpr int16_t ICM20948_GYRO_PREC  = 8; // 4;

typedef enum
{
	ICM20948_RET_OK     =  0,
	ICM20948_GEN_FAIL   = -1,
	ICM20948_INV_PARAM  = -2,
	ICM20948_INV_CONFIG = -3,
	ICM20948_NN         = -4
}ICM20948_RetCode_t;

typedef enum
{
	ICM20948_DLPF_0 = 0x00,
	ICM20948_DLPF_1 = 0x08,
	ICM20948_DLPF_2 = 0x10,
	ICM20948_DLPF_3 = 0x18,
	ICM20948_DLPF_4 = 0x20,
	ICM20948_DLPF_5 = 0x28,
	ICM20948_DLPF_6 = 0x30,
	ICM20948_DLPF_7 = 0x38
}ICM20948_DLPF_t;

typedef struct
{
	uint8_t  ui8Selection;
	float    fSensitivity;
	uint16_t ui16Range;
}ICM20948_FullScale_t;

typedef struct
{
	uint16_t ui16Value; // Accelerometer sample rate in Hz
	bool     boFCHOICE; // Enable accelerometer DLPF
	uint16_t ui16Div;   // Accelerometer sample rate divider
}ICM20948_AccelSampleRate_t;

typedef struct
{
	uint16_t ui16Value; // Gyro sample rate in Hz
	bool     boFCHOICE; // Enable gyro DLPF
	uint8_t  ui8Div;    // Gyro sample rate divider
}ICM20948_GyroSampleRate_t;

typedef struct
{
	int16_t i16XAxis;
	int16_t i16YAxis;
	int16_t i16ZAxis;
}ICM20948_i16Vector_t;

typedef struct
{
	int32_t i32XAxis;
	int32_t i32YAxis;
	int32_t i32ZAxis;
}ICM20948_i32Vector_t;

typedef struct
{
	float fXAxis;
	float fYAxis;
	float fZAxis;
}ICM20948_fVector_t;

typedef struct
{
	bool boStatusOK;
	bool boSleep;
	bool boUseSPI;
	ICM20948_FullScale_t AccelFullScale;
	ICM20948_AccelSampleRate_t AccelSampleRate;
	ICM20948_DLPF_t AccelDLPF;
	ICM20948_FullScale_t GyroFullScale;
	ICM20948_GyroSampleRate_t GyroSampleRate;
	ICM20948_DLPF_t GyroDLPF;
}ICM20948_SensorConfig_t;


/* Constants for Accelerometer sample rate, low pass filter and full scale (ICM-20948 datasheet, p. 63 ff.)
 * Accelerometer Sample Rate = 4500 [Hz]                           when DLPF is disabled (ACCEL_FCHOICE = 0)
 *                           = 1125 / (1 + ACCEL_SMPLRT_DIV) [Hz]  when DLPF is enabled  (ACCEL_FCHOICE = 1) */
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_4500_HZ  = {4500, 0, 0x0000};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_1125_HZ  = {1125, 1, 0x0000};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_562_5_HZ = { 563, 1, 0x0001};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_375_HZ   = { 375, 1, 0x0002};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_281_3_HZ = { 281, 1, 0x0003};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_225_HZ   = { 225, 1, 0x0004};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_187_5_HZ = { 188, 1, 0x0005};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_140_6_HZ = { 141, 1, 0x0007};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_125_HZ   = { 125, 1, 0x0008};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_102_3_HZ = { 102, 1, 0x000A};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_70_3_HZ  = {  70, 1, 0x000F};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_66_2_HZ  = {  66, 1, 0x0010};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_48_9_HZ  = {  22, 1, 0x0016};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_34_1_HZ  = {  34, 1, 0x0020};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_17_3_HZ  = {  17, 1, 0x0040};
constexpr ICM20948_AccelSampleRate_t ACCEL_SR_4_4_HZ   = {   4, 1, 0x00FF};

/* See also datasheet p. 12 */
constexpr ICM20948_FullScale_t ACCEL_FS_2G  = {0x00, 0.000061,  2}; /* 1 / 16384 = 0.000061 */
constexpr ICM20948_FullScale_t ACCEL_FS_4G  = {0x02, 0.000122,  4}; /* 1 /  8192 = 0.000122 */
constexpr ICM20948_FullScale_t ACCEL_FS_8G  = {0x04, 0.000244,  8}; /* 1 /  4096 = 0.000244 */
constexpr ICM20948_FullScale_t ACCEL_FS_16G = {0x06, 0.000488, 16}; /* 1 /  2048 = 0.000488 */


/* Constants for Gyroscope sample rate, low pass filter and full scale (ICM-20948 datasheet p. 59 ff.)
 * Gyroscope Sample Rate = 9000 [Hz]                          when DLPF is disabled (GYRO_FCHOICE = 0)
 *                       = 1125 / (1 + GYRO_SMPLRT_DIV) [Hz]  when DLPF is enabled  (GYRO_FCHOICE = 1) */
constexpr ICM20948_GyroSampleRate_t GYRO_SR_9000_HZ  = {9000, 0, 0x00};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_1125_HZ  = {1125, 1, 0x00};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_562_5_HZ = { 563, 1, 0x01};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_375_HZ   = { 375, 1, 0x02};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_281_3_HZ = { 281, 1, 0x03};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_225_HZ   = { 225, 1, 0x04};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_187_5_HZ = { 188, 1, 0x05};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_140_6_HZ = { 141, 1, 0x07};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_125_HZ   = { 125, 1, 0x08};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_102_3_HZ = { 102, 1, 0x0A};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_70_3_HZ  = {  70, 1, 0x0F};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_66_2_HZ  = {  66, 1, 0x10};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_48_9_HZ  = {  22, 1, 0x16};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_34_1_HZ  = {  34, 1, 0x20};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_17_3_HZ  = {  17, 1, 0x40};
constexpr ICM20948_GyroSampleRate_t GYRO_SR_4_4_HZ   = {   4, 1, 0xFF};

/* See also datasheet p. 11 */
constexpr ICM20948_FullScale_t GYRO_FS_250DPS  = {0x00, 0.007633,  250}; /* 1 / 131.0 = 0.007633 */
constexpr ICM20948_FullScale_t GYRO_FS_500DPS  = {0x02, 0.015267,  500}; /* 1 /  65.5 = 0.015267 */
constexpr ICM20948_FullScale_t GYRO_FS_1000DPS = {0x04, 0.030487, 1000}; /* 1 /  32.8 = 0.030487 */
constexpr ICM20948_FullScale_t GYRO_FS_2000DPS = {0x06, 0.060975, 2000}; /* 1 /  16.4 = 0.060975 */


class ICM20948
{
public:
	/* Constructor, Destructor */
	ICM20948(SPI *pSPI, ICM20948_FullScale_t ACCEL_FS, ICM20948_FullScale_t GYRO_FS,
			 ICM20948_AccelSampleRate_t ACCEL_SR, ICM20948_GyroSampleRate_t GYRO_SR, ICM20948_DLPF_t DLPF);
	~ICM20948(void);

	/* Methods */
	ICM20948_SensorConfig_t getSensorConfig(void);
	int16_t sleep(bool boSleep);

	int16_t setAccelFullScale(ICM20948_FullScale_t FullScale);
	int16_t getAccelFullScale(ICM20948_FullScale_t *FullScale);
	int16_t setGyroFullScale(ICM20948_FullScale_t FullScale);
	int16_t getGyroFullScale(ICM20948_FullScale_t *FullScale);

	int16_t setAccelSampleRate(ICM20948_AccelSampleRate_t SampleRate);
	int16_t getAccelSampleRate(ICM20948_AccelSampleRate_t *SampleRate);
	int16_t setGyroSampleRate(ICM20948_GyroSampleRate_t SampleRate);
	int16_t getGyroSampleRate(ICM20948_GyroSampleRate_t *SampleRate);

	int16_t setAccelDLPF(ICM20948_DLPF_t DLPF);
	int16_t getAccelDLPF(ICM20948_DLPF_t *pDLPF);
	int16_t setGyroDLPF(ICM20948_DLPF_t DLPF);
	int16_t getGyroDLPF(ICM20948_DLPF_t *pDLPF);

	void setAccelOffset(ICM20948_i16Vector_t Offset);
	void getAccelOffset(ICM20948_i16Vector_t *pOffset);
	void resetAccelOffset(void);
	void setGyroOffset(ICM20948_i16Vector_t Offset);
	void getGyroOffset(ICM20948_i16Vector_t *pOffset);
	void resetGyroOffset(void);

	int16_t readAllDataRaw(void); // Accel + Gyro + Temp (14 bytes)

	ICM20948_i16Vector_t getAccelRaw(void);
	ICM20948_i16Vector_t getCorrectedAccelRaw(void);
	ICM20948_i16Vector_t getGyroRaw(void);
	ICM20948_i16Vector_t getCorrectedGyroRaw(void);

	int16_t calculateMeanValues(void);
	int16_t exeCalibration(void);
	int16_t exeCalibrationSingleIteration(uint8_t ui8Iteration, uint8_t *pReady);

	int16_t setDebugFunction8(uint8_t ui8Data);

	int16_t setDebugFunction16(uint16_t ui16Data);
	int16_t getDebugFunction16(uint16_t *pData);


private:
	/* Variables */
	SPI *pSPI;
	uint8_t ICM20948_I2C_ADDR;
	uint8_t ui8CurrentBank;
	ICM20948_SensorConfig_t ICM20948_SensorConfig;

	uint8_t ui8DataArray[14]; /* 0...5 : Accelerometer
	                             6...11: Gyroscope
	                            12...13: Temperature  */

	ICM20948_i16Vector_t AccelOffset;
	ICM20948_i16Vector_t GyroOffset;
	ICM20948_i16Vector_t CorrectedAccelMean;
	ICM20948_i16Vector_t CorrectedGyroMean;

	int16_t i16AccelPrec;
	int16_t i16GyroPrec;

	/* Methods */
	ICM20948_RetCode_t init(ICM20948_FullScale_t ACCEL_FS, ICM20948_FullScale_t GYRO_FS,
			                ICM20948_AccelSampleRate_t ACCEL_SR, ICM20948_GyroSampleRate_t GYRO_SR, ICM20948_DLPF_t DLPF);
	int16_t resetBank(void);
	int16_t switchBank(uint8_t ui8NewBank);
	int16_t writeRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Data);
	int16_t readRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t *pData);
	int16_t writeRegister16(uint8_t ui8Bank, uint8_t ui8RegAddrH, uint16_t ui16Data);
	int16_t readRegister16(uint8_t ui8Bank, uint8_t ui8RegAddrH, uint16_t *pData);
	int16_t setRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos);
	int16_t clearRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos);
	int16_t getRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos, bool *pValue);
	int16_t changeRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Msk, uint8_t ui8Value);

	inline bool isValidPos(uint8_t ui8Pos);
	inline bool isValidAccelFullScale(ICM20948_FullScale_t FullScale);
	inline bool isValidGyroFullScale(ICM20948_FullScale_t FullScale);
	inline bool isValidAccelSampleRate(ICM20948_AccelSampleRate_t SampleRate);
	inline bool isValidGyroSampleRate(ICM20948_GyroSampleRate_t SampleRate);
	inline bool isValidDLPF(ICM20948_DLPF_t DLPF);
};


#endif /* ZULS_INCLUDE_ICM20948_HPP_ */
