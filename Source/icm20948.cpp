/*
 * icm20948.cpp
 *
 *  Created on: Mar 29, 2024
 *      Author: mbeuler
 */

#include "icm20948.hpp"
#include "icm20948reg.hpp"


extern "C" uint32_t get_Ticks(void);


/* ICM20948 class */
ICM20948::ICM20948(SPI *pSPI, ICM20948_FullScale_t ACCEL_FS, ICM20948_FullScale_t GYRO_FS,
		           ICM20948_AccelSampleRate_t ACCEL_SR, ICM20948_GyroSampleRate_t GYRO_SR, ICM20948_DLPF_t DLPF)
{
	this->pSPI = pSPI;

	ICM20948_SensorConfig.boUseSPI = true;

	init(ACCEL_FS, GYRO_FS, ACCEL_SR, GYRO_SR, DLPF);
}


/* Public methods */
ICM20948_SensorConfig_t ICM20948::getSensorConfig(void)
{
	return ICM20948_SensorConfig;
}


int16_t ICM20948::sleep(bool boValue)
{
	if (boValue)
	{
		if (setRegister8Bit(0, ICM20948_PWR_MGMT_1, ICM20948_SLEEP) != 0) {return -1;}
	}
	else
	{
		if (clearRegister8Bit(0, ICM20948_PWR_MGMT_1, ICM20948_SLEEP) != 0) {return -1;}
	}

	ICM20948_SensorConfig.boSleep = boValue;

	return 0;
}


int16_t ICM20948::setAccelFullScale(ICM20948_FullScale_t FullScale)
{
	/* Check argument FullScale */
	if (!isValidAccelFullScale(FullScale)) {return -1;}

	if (changeRegister8(2, ICM20948_ACCEL_CONFIG, ICM20948_ACCEL_FS_SEL, FullScale.ui8Selection) != 0) {return -1;}
	ICM20948_SensorConfig.AccelFullScale = FullScale;

	return 0;
}


int16_t ICM20948::getAccelFullScale(ICM20948_FullScale_t *FullScale)
{
	uint8_t ui8Data;

	if (readRegister8(2, ICM20948_ACCEL_CONFIG, &ui8Data) != 0) {return -1;}
	ui8Data &= ICM20948_ACCEL_FS_SEL;

	if      (ui8Data == ACCEL_FS_2G.ui8Selection) {*FullScale = ACCEL_FS_2G;}
	else if (ui8Data == ACCEL_FS_4G.ui8Selection) {*FullScale = ACCEL_FS_4G;}
	else if (ui8Data == ACCEL_FS_8G.ui8Selection) {*FullScale = ACCEL_FS_8G;}
	else                                          {*FullScale = ACCEL_FS_16G;}

	return 0;
}


int16_t ICM20948::setGyroFullScale(ICM20948_FullScale_t FullScale)
{
	/* Check argument FullScale */
	if (!isValidGyroFullScale(FullScale)) {return -1;}

	if (changeRegister8(2, ICM20948_GYRO_CONFIG_1, ICM20948_GYRO_FS_SEL, FullScale.ui8Selection) != 0) {return -1;}
	ICM20948_SensorConfig.GyroFullScale = FullScale;

	return 0;
}


int16_t ICM20948::getGyroFullScale(ICM20948_FullScale_t *FullScale)
{
	uint8_t ui8Data;

	if (readRegister8(2, ICM20948_GYRO_CONFIG_1, &ui8Data) != 0) {return -1;}
	ui8Data &= ICM20948_GYRO_FS_SEL;

	if      (ui8Data == GYRO_FS_250DPS.ui8Selection)  {*FullScale = GYRO_FS_250DPS;}
	else if (ui8Data == GYRO_FS_500DPS.ui8Selection)  {*FullScale = GYRO_FS_500DPS;}
	else if (ui8Data == GYRO_FS_1000DPS.ui8Selection) {*FullScale = GYRO_FS_1000DPS;}
	else                                              {*FullScale = GYRO_FS_2000DPS;}

	return 0;
}


int16_t ICM20948::setAccelSampleRate(ICM20948_AccelSampleRate_t SampleRate)
{
	/* Check argument SampleRate */
	if (!isValidAccelSampleRate(SampleRate)) {return -1;}

	if (writeRegister16(2, ICM20948_ACCEL_SMPLRT_DIV_1, SampleRate.ui16Div) != 0) {return -1;}

	/* Sample rate <= 1125Hz --> DLPF is enabled  (FCHOICE = true)
	 * Sample rate  = 4500Hz --> DLPF is disabled (FCHOICE = false) */
	if (changeRegister8(2, ICM20948_ACCEL_CONFIG, ICM20948_ACCEL_FCHOICE, (uint8_t)SampleRate.boFCHOICE) != 0) {return -1;}

//	if (SampleRate.boFCHOICE)
//	{
//		/* DLPF is enabled --> Sample rate <= 1125Hz */
//		if (setRegister8Bit(2, ICM20948_ACCEL_CONFIG, ICM20948_ACCEL_FCHOICE) != 0) {return -1;}
//	}
//	else
//	{
//		/* DLPF is disabled --> Sample rate = 4500Hz */
//		if (clearRegister8Bit(2, ICM20948_ACCEL_CONFIG, ICM20948_ACCEL_FCHOICE) != 0) {return -1;}
//	}

	ICM20948_SensorConfig.AccelSampleRate = SampleRate;

	return 0;
}


int16_t ICM20948::getAccelSampleRate(ICM20948_AccelSampleRate_t *SampleRate)
{
	uint16_t ui16Div;
	uint8_t ui8Aux;

	bool boFCHOICE = false;

	if (readRegister16(2, ICM20948_ACCEL_SMPLRT_DIV_1, &ui16Div) != 0) {return -1;}
	if (readRegister8(2, ICM20948_ACCEL_CONFIG, &ui8Aux) != 0) {return -1;}

	if (ui8Aux & ICM20948_ACCEL_FCHOICE) {boFCHOICE = true;}

	if (!boFCHOICE)
	{
		if (ui16Div == ACCEL_SR_4500_HZ.ui16Div) {*SampleRate = ACCEL_SR_4500_HZ;}
		else                                     {return -1;}
	}
	else
	{
		if      (ui16Div ==  ACCEL_SR_1125_HZ.ui16Div) {*SampleRate =  ACCEL_SR_1125_HZ;}
		else if (ui16Div == ACCEL_SR_562_5_HZ.ui16Div) {*SampleRate = ACCEL_SR_562_5_HZ;}
		else if (ui16Div ==   ACCEL_SR_375_HZ.ui16Div) {*SampleRate =   ACCEL_SR_375_HZ;}
		else if (ui16Div == ACCEL_SR_281_3_HZ.ui16Div) {*SampleRate = ACCEL_SR_281_3_HZ;}
		else if (ui16Div ==   ACCEL_SR_225_HZ.ui16Div) {*SampleRate =   ACCEL_SR_225_HZ;}
		else if (ui16Div == ACCEL_SR_187_5_HZ.ui16Div) {*SampleRate = ACCEL_SR_187_5_HZ;}
		else if (ui16Div == ACCEL_SR_140_6_HZ.ui16Div) {*SampleRate = ACCEL_SR_140_6_HZ;}
		else if (ui16Div ==   ACCEL_SR_125_HZ.ui16Div) {*SampleRate =   ACCEL_SR_125_HZ;}
		else if (ui16Div == ACCEL_SR_102_3_HZ.ui16Div) {*SampleRate = ACCEL_SR_102_3_HZ;}
		else if (ui16Div ==  ACCEL_SR_70_3_HZ.ui16Div) {*SampleRate =  ACCEL_SR_70_3_HZ;}
		else if (ui16Div ==  ACCEL_SR_66_2_HZ.ui16Div) {*SampleRate =  ACCEL_SR_66_2_HZ;}
		else if (ui16Div ==  ACCEL_SR_48_9_HZ.ui16Div) {*SampleRate =  ACCEL_SR_48_9_HZ;}
		else if (ui16Div ==  ACCEL_SR_34_1_HZ.ui16Div) {*SampleRate =  ACCEL_SR_34_1_HZ;}
		else if (ui16Div ==  ACCEL_SR_17_3_HZ.ui16Div) {*SampleRate =  ACCEL_SR_17_3_HZ;}
		else if (ui16Div ==   ACCEL_SR_4_4_HZ.ui16Div) {*SampleRate =   ACCEL_SR_4_4_HZ;}
		else                                           {return -1;}
	}

	return 0;
}


int16_t ICM20948::setGyroSampleRate(ICM20948_GyroSampleRate_t SampleRate)
{
	/* Check argument SampleRate */
	if (!isValidGyroSampleRate(SampleRate)) {return -1;}

	if (writeRegister8(2, ICM20948_GYRO_SMPLRT_DIV, SampleRate.ui8Div) != 0) {return -1;}

	/* Sample rate <= 1125Hz --> DLPF is enabled  (FCHOICE = true)
	 * Sample rate  = 4500Hz --> DLPF is disabled (FCHOICE = false) */
	if (changeRegister8(2, ICM20948_GYRO_CONFIG_1, ICM20948_GYRO_FCHOICE, (uint8_t)SampleRate.boFCHOICE) != 0) {return -1;}

//	if (SampleRate.boFCHOICE)
//	{
//		/* DLPF is enabled --> Sample rate <= 1125Hz */
//		if (setRegister8Bit(2, ICM20948_GYRO_CONFIG_1, ICM20948_GYRO_FCHOICE) != 0) {return -1;}
//	}
//	else
//	{
//		/* DLPF is disabled --> Sample rate = 9000Hz */
//		if (clearRegister8Bit(2, ICM20948_GYRO_CONFIG_1, ICM20948_GYRO_FCHOICE) != 0) {return -1;}
//	}

	ICM20948_SensorConfig.GyroSampleRate = SampleRate;

	return 0;
}


int16_t ICM20948::getGyroSampleRate(ICM20948_GyroSampleRate_t *SampleRate)
{
	uint8_t ui8Div;
	uint8_t ui8Aux;

	bool boFCHOICE = false;

	if (readRegister8(2, ICM20948_GYRO_SMPLRT_DIV, &ui8Div) != 0) {return -1;}
	if (readRegister8(2, ICM20948_GYRO_CONFIG_1, &ui8Aux) != 0) {return -1;}

	if (ui8Aux & ICM20948_GYRO_FCHOICE) {boFCHOICE = true;}

	if (!boFCHOICE)
	{
		if (ui8Div == GYRO_SR_9000_HZ.ui8Div) {*SampleRate = GYRO_SR_9000_HZ;}
		else                                  {return -1;}
	}
	else
	{
		if      (ui8Div ==  GYRO_SR_1125_HZ.ui8Div) {*SampleRate =  GYRO_SR_1125_HZ;}
		else if (ui8Div == GYRO_SR_562_5_HZ.ui8Div) {*SampleRate = GYRO_SR_562_5_HZ;}
		else if (ui8Div ==   GYRO_SR_375_HZ.ui8Div) {*SampleRate =   GYRO_SR_375_HZ;}
		else if (ui8Div == GYRO_SR_281_3_HZ.ui8Div) {*SampleRate = GYRO_SR_281_3_HZ;}
		else if (ui8Div ==   GYRO_SR_225_HZ.ui8Div) {*SampleRate =   GYRO_SR_225_HZ;}
		else if (ui8Div == GYRO_SR_187_5_HZ.ui8Div) {*SampleRate = GYRO_SR_187_5_HZ;}
		else if (ui8Div == GYRO_SR_140_6_HZ.ui8Div) {*SampleRate = GYRO_SR_140_6_HZ;}
		else if (ui8Div ==   GYRO_SR_125_HZ.ui8Div) {*SampleRate =   GYRO_SR_125_HZ;}
		else if (ui8Div == GYRO_SR_102_3_HZ.ui8Div) {*SampleRate = GYRO_SR_102_3_HZ;}
		else if (ui8Div ==  GYRO_SR_70_3_HZ.ui8Div) {*SampleRate =  GYRO_SR_70_3_HZ;}
		else if (ui8Div ==  GYRO_SR_66_2_HZ.ui8Div) {*SampleRate =  GYRO_SR_66_2_HZ;}
		else if (ui8Div ==  GYRO_SR_48_9_HZ.ui8Div) {*SampleRate =  GYRO_SR_48_9_HZ;}
		else if (ui8Div ==  GYRO_SR_34_1_HZ.ui8Div) {*SampleRate =  GYRO_SR_34_1_HZ;}
		else if (ui8Div ==  GYRO_SR_17_3_HZ.ui8Div) {*SampleRate =  GYRO_SR_17_3_HZ;}
		else if (ui8Div ==   GYRO_SR_4_4_HZ.ui8Div) {*SampleRate =   GYRO_SR_4_4_HZ;}
		else                                        {return -1;}
	}

	return 0;
}


int16_t ICM20948::setAccelDLPF(ICM20948_DLPF_t DLPF)
{
	/* Check argument DLPF */
	if (!isValidDLPF(DLPF)) {return -1;}

	if (changeRegister8(2, ICM20948_ACCEL_CONFIG, ICM20948_ACCEL_DLPFCFG, DLPF) != 0) {return -1;}
	ICM20948_SensorConfig.AccelDLPF = DLPF;

	return 0;
}


int16_t ICM20948::getAccelDLPF(ICM20948_DLPF_t *pDLPF)
{
	uint8_t ui8Data;

	if (readRegister8(2, ICM20948_ACCEL_CONFIG, &ui8Data) != 0) {return -1;}
	ui8Data &= ICM20948_ACCEL_DLPFCFG;

	if      (ui8Data == ICM20948_DLPF_0) {*pDLPF = ICM20948_DLPF_0;}
	else if (ui8Data == ICM20948_DLPF_1) {*pDLPF = ICM20948_DLPF_1;}
	else if (ui8Data == ICM20948_DLPF_2) {*pDLPF = ICM20948_DLPF_2;}
	else if (ui8Data == ICM20948_DLPF_3) {*pDLPF = ICM20948_DLPF_3;}
	else if (ui8Data == ICM20948_DLPF_4) {*pDLPF = ICM20948_DLPF_4;}
	else if (ui8Data == ICM20948_DLPF_5) {*pDLPF = ICM20948_DLPF_5;}
	else if (ui8Data == ICM20948_DLPF_6) {*pDLPF = ICM20948_DLPF_6;}
	else                                 {*pDLPF = ICM20948_DLPF_7;}

	return 0;
}


int16_t ICM20948::setGyroDLPF(ICM20948_DLPF_t DLPF)
{
	/* Check argument DLPF */
	if (!isValidDLPF(DLPF)) {return -1;}

	if (changeRegister8(2, ICM20948_GYRO_CONFIG_1, ICM20948_GYRO_DLPFCFG, DLPF) != 0) {return -1;}
	ICM20948_SensorConfig.GyroDLPF = DLPF;

	return 0;
}


int16_t ICM20948::getGyroDLPF(ICM20948_DLPF_t *pDLPF)
{
	uint8_t ui8Data;

	if (readRegister8(2, ICM20948_GYRO_CONFIG_1, &ui8Data) != 0) {return -1;}
	ui8Data &= ICM20948_GYRO_DLPFCFG;

	if      (ui8Data == ICM20948_DLPF_0) {*pDLPF = ICM20948_DLPF_0;}
	else if (ui8Data == ICM20948_DLPF_1) {*pDLPF = ICM20948_DLPF_1;}
	else if (ui8Data == ICM20948_DLPF_2) {*pDLPF = ICM20948_DLPF_2;}
	else if (ui8Data == ICM20948_DLPF_3) {*pDLPF = ICM20948_DLPF_3;}
	else if (ui8Data == ICM20948_DLPF_4) {*pDLPF = ICM20948_DLPF_4;}
	else if (ui8Data == ICM20948_DLPF_5) {*pDLPF = ICM20948_DLPF_5;}
	else if (ui8Data == ICM20948_DLPF_6) {*pDLPF = ICM20948_DLPF_6;}
	else                                 {*pDLPF = ICM20948_DLPF_7;}

	return 0;
}


void ICM20948::setAccelOffset(ICM20948_i16Vector_t Offset)
{
	AccelOffset.i16XAxis = Offset.i16XAxis;
	AccelOffset.i16YAxis = Offset.i16YAxis;
	AccelOffset.i16ZAxis = Offset.i16ZAxis;
}


void ICM20948::getAccelOffset(ICM20948_i16Vector_t *pOffset)
{
	pOffset->i16XAxis = AccelOffset.i16XAxis;
	pOffset->i16YAxis = AccelOffset.i16YAxis;
	pOffset->i16ZAxis = AccelOffset.i16ZAxis;
}


void ICM20948::resetAccelOffset(void)
{
	AccelOffset.i16XAxis = 0x0000;
	AccelOffset.i16YAxis = 0x0000;
	AccelOffset.i16ZAxis = 0x0000;
}


void ICM20948::setGyroOffset(ICM20948_i16Vector_t Offset)
{
	GyroOffset.i16XAxis = Offset.i16XAxis;
	GyroOffset.i16YAxis = Offset.i16YAxis;
	GyroOffset.i16ZAxis = Offset.i16ZAxis;
}


void ICM20948::getGyroOffset(ICM20948_i16Vector_t *pOffset)
{
	pOffset->i16XAxis = GyroOffset.i16XAxis;
	pOffset->i16YAxis = GyroOffset.i16YAxis;
	pOffset->i16ZAxis = GyroOffset.i16ZAxis;
}


void ICM20948::resetGyroOffset(void)
{
	GyroOffset.i16XAxis = 0x0000;
	GyroOffset.i16YAxis = 0x0000;
	GyroOffset.i16ZAxis = 0x0000;
}


int16_t ICM20948::readAllDataRaw(void)
{
	/* Byte includes R/W-bit (read = 1) and 7-bit memory/register address */
	uint8_t ui8Data = 0x80 | ICM20948_ACCEL_XOUT_H;

	if (switchBank(0) != 0) {return -1;}

	pSPI->enableNSS();
	if (pSPI->transmitSPI(&ui8Data, 1) != 0)     {pSPI->disableNSS(); return -1;}
	if (pSPI->receiveSPI(ui8DataArray, 14) != 0) {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

	//if (pSPI->readBurst(ICM20948_ACCEL_XOUT_H, 14, ui8DataArray) != 0) {return -1;}

	return 0;
}


ICM20948_i16Vector_t ICM20948::getAccelRaw(void)
{
	ICM20948_i16Vector_t AccelRaw;

	AccelRaw.i16XAxis = (ui8DataArray[0] << 8) | ui8DataArray[1];
	AccelRaw.i16YAxis = (ui8DataArray[2] << 8) | ui8DataArray[3];
	AccelRaw.i16ZAxis = (ui8DataArray[4] << 8) | ui8DataArray[5];

	return AccelRaw;
}


ICM20948_i16Vector_t ICM20948::getCorrectedAccelRaw(void)
{
	ICM20948_i16Vector_t CorrectedAccelRaw;

	CorrectedAccelRaw.i16XAxis = ((ui8DataArray[0] << 8) | ui8DataArray[1]) + AccelOffset.i16XAxis;
	CorrectedAccelRaw.i16YAxis = ((ui8DataArray[2] << 8) | ui8DataArray[3]) + AccelOffset.i16YAxis;
	CorrectedAccelRaw.i16ZAxis = ((ui8DataArray[4] << 8) | ui8DataArray[5]) + AccelOffset.i16ZAxis;

	return CorrectedAccelRaw;
}


ICM20948_i16Vector_t ICM20948::getGyroRaw(void)
{
	ICM20948_i16Vector_t GyroRaw;

	GyroRaw.i16XAxis = (ui8DataArray[ 6] << 8) | ui8DataArray[ 7];
	GyroRaw.i16YAxis = (ui8DataArray[ 8] << 8) | ui8DataArray[ 9];
	GyroRaw.i16ZAxis = (ui8DataArray[10] << 8) | ui8DataArray[11];

	return GyroRaw;
}


ICM20948_i16Vector_t ICM20948::getCorrectedGyroRaw(void)
{
	ICM20948_i16Vector_t CorrectedGyroRaw;

	CorrectedGyroRaw.i16XAxis = ((ui8DataArray[ 6] << 8) | ui8DataArray[ 7]) + GyroOffset.i16XAxis;
	CorrectedGyroRaw.i16YAxis = ((ui8DataArray[ 8] << 8) | ui8DataArray[ 9]) + GyroOffset.i16YAxis;
	CorrectedGyroRaw.i16ZAxis = ((ui8DataArray[10] << 8) | ui8DataArray[11]) + GyroOffset.i16ZAxis;

	return CorrectedGyroRaw;
}


int16_t ICM20948::calculateMeanValues(void)
{
	ICM20948_i16Vector_t CorrectedAccelRaw; // Corrected raw measurement data from accelerometer
	ICM20948_i16Vector_t CorrectedGyroRaw;  // Corrected raw measurement data from gyroscope

	ICM20948_i32Vector_t CorrectedAccelRawSum;
	ICM20948_i32Vector_t CorrectedGyroRawSum;

	uint32_t ui32Ticks;

	/* Initialization */
	CorrectedAccelRawSum.i32XAxis = 0;
	CorrectedAccelRawSum.i32YAxis = 0;
	CorrectedAccelRawSum.i32ZAxis = 0;
	CorrectedGyroRawSum.i32XAxis  = 0;
	CorrectedGyroRawSum.i32YAxis  = 0;
	CorrectedGyroRawSum.i32ZAxis  = 0;

	ui32Ticks = get_Ticks();

	while(get_Ticks() < (ui32Ticks + 1));
	ui32Ticks = get_Ticks(); // Update ticks

	for (uint16_t i = 0; i < 100 + SAMPLES_MEAN_VALUE; i++)
	{
		if (readAllDataRaw() != 0) {return -1;}

		if (i >= 100)
		{
			CorrectedAccelRaw = getCorrectedAccelRaw();
			CorrectedGyroRaw  = getCorrectedGyroRaw();

			CorrectedAccelRawSum.i32XAxis += CorrectedAccelRaw.i16XAxis;
			CorrectedAccelRawSum.i32YAxis += CorrectedAccelRaw.i16YAxis;
			CorrectedAccelRawSum.i32ZAxis += CorrectedAccelRaw.i16ZAxis;
			CorrectedGyroRawSum.i32XAxis  += CorrectedGyroRaw.i16XAxis;
			CorrectedGyroRawSum.i32YAxis  += CorrectedGyroRaw.i16YAxis;
			CorrectedGyroRawSum.i32ZAxis  += CorrectedGyroRaw.i16ZAxis;
		}

		/* We need a delay of 1ms to get a process sample rate of 1000Hz
		 * (both accelerometer and gyroscope sample rate must be 1125Hz) */
		while(get_Ticks() < (ui32Ticks + 1));
		ui32Ticks = get_Ticks(); // Update ticks
	}

	CorrectedAccelMean.i16XAxis = CorrectedAccelRawSum.i32XAxis / SAMPLES_MEAN_VALUE;
	CorrectedAccelMean.i16YAxis = CorrectedAccelRawSum.i32YAxis / SAMPLES_MEAN_VALUE;
	CorrectedAccelMean.i16ZAxis = CorrectedAccelRawSum.i32ZAxis / SAMPLES_MEAN_VALUE;
	CorrectedGyroMean.i16XAxis  = CorrectedGyroRawSum.i32XAxis  / SAMPLES_MEAN_VALUE;
	CorrectedGyroMean.i16YAxis  = CorrectedGyroRawSum.i32YAxis  / SAMPLES_MEAN_VALUE;
	CorrectedGyroMean.i16ZAxis  = CorrectedGyroRawSum.i32ZAxis  / SAMPLES_MEAN_VALUE;

	return 0;
}


int16_t ICM20948::exeCalibration(void)
{
	int16_t i16Aux;

	uint8_t ui8Ready; /* Variable counts the calibrated sensor axes
	                     (its value is 6 if all accelerometer and gyroscope axes are calibrated) */

	uint32_t ui32Debug;

	if      (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_2G.ui8Selection) {i16Aux = 16384;}
	else if (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_4G.ui8Selection) {i16Aux =  8192;}
	else if (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_8G.ui8Selection) {i16Aux =  4096;}
	else                                                                                    {i16Aux =  2048;}

	/* Calculate the CorrectedAccelMean and CorrectedGyroMean values */
	if (calculateMeanValues() != 0) {return -1;}

	/* Calculate the new AccelOffset and GyroOffset values which are used in the first iteration (i = 0) */
	AccelOffset.i16XAxis = -CorrectedAccelMean.i16XAxis / i16AccelPrec;
	AccelOffset.i16YAxis = -CorrectedAccelMean.i16YAxis / i16AccelPrec;
	AccelOffset.i16ZAxis = (i16Aux - CorrectedAccelMean.i16ZAxis) / i16AccelPrec;
	GyroOffset.i16XAxis  = -CorrectedGyroMean.i16XAxis / i16GyroPrec;
	GyroOffset.i16YAxis  = -CorrectedGyroMean.i16YAxis / i16GyroPrec;
	GyroOffset.i16ZAxis  = -CorrectedGyroMean.i16ZAxis / i16GyroPrec;

	for (uint8_t i = 0; i < MAX_ITERATIONS; i++)
	{
		ui8Ready = 0;

		if (calculateMeanValues() != 0) {return -1;}

		if (abs(CorrectedAccelMean.i16XAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16XAxis = AccelOffset.i16XAxis - CorrectedAccelMean.i16XAxis / i16AccelPrec;}

		if (abs(CorrectedAccelMean.i16YAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16YAxis = AccelOffset.i16YAxis - CorrectedAccelMean.i16YAxis / i16AccelPrec;}

		if (abs(i16Aux - CorrectedAccelMean.i16ZAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16ZAxis = AccelOffset.i16ZAxis + (i16Aux - CorrectedAccelMean.i16ZAxis) / i16AccelPrec;}

		if (abs(CorrectedGyroMean.i16XAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16XAxis = GyroOffset.i16XAxis - CorrectedGyroMean.i16XAxis / i16GyroPrec;}

		if (abs(CorrectedGyroMean.i16YAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16YAxis = GyroOffset.i16YAxis - CorrectedGyroMean.i16YAxis / i16GyroPrec;}

		if (abs(CorrectedGyroMean.i16ZAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16ZAxis = GyroOffset.i16ZAxis - CorrectedGyroMean.i16ZAxis / i16GyroPrec;}

		if (ui8Ready == 6) {
			ui32Debug = 0;
			return 0;
		}
		ui32Debug = 0;
	}

	/* If we get here, the calibration process was not successful */
	ui32Debug = 0;

	return -2;
}


/**
  @brief  Calculates a single iteration of the calibration procedure
  @param  ui8Iteration (between 0 and MAX_ITERATIONS-1)
  @retval  0: All 6 axes are calibrated
           1: Less than 6 axes are calibrated, but ui8Iteration < MAX_ITERATIONS
          -1: An error during the function call 'calculateMeanValues' occurred (should not happen)
          -2: ui8Iteration >= MAX_ITERATIONS --> Calibration not succeeded
**/
int16_t ICM20948::exeCalibrationSingleIteration(uint8_t ui8Iteration, uint8_t *pReady)
{
	int16_t i16Aux;

	uint8_t ui8Ready = 0; /* Variable counts the calibrated sensor axes
	                         (its value is 6 if all accelerometer and gyroscope axes are calibrated) */
	*pReady = ui8Ready;

	if      (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_2G.ui8Selection) {i16Aux = 16384;}
	else if (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_4G.ui8Selection) {i16Aux =  8192;}
	else if (ICM20948_SensorConfig.AccelFullScale.ui8Selection == ACCEL_FS_8G.ui8Selection) {i16Aux =  4096;}
	else                                                                                    {i16Aux =  2048;}

	if (ui8Iteration == 0)
	{
		/* Calculate the CorrectedAccelMean and CorrectedGyroMean values */
		if (calculateMeanValues() != 0) {return -1;}

		/* Calculate the new AccelOffset and GyroOffset values which are used in the first iteration (i = 0) */
		AccelOffset.i16XAxis = -CorrectedAccelMean.i16XAxis / i16AccelPrec;
		AccelOffset.i16YAxis = -CorrectedAccelMean.i16YAxis / i16AccelPrec;
		AccelOffset.i16ZAxis = (i16Aux - CorrectedAccelMean.i16ZAxis) / i16AccelPrec;
		GyroOffset.i16XAxis  = -CorrectedGyroMean.i16XAxis / i16GyroPrec;
		GyroOffset.i16YAxis  = -CorrectedGyroMean.i16YAxis / i16GyroPrec;
		GyroOffset.i16ZAxis  = -CorrectedGyroMean.i16ZAxis / i16GyroPrec;
	}

	if (ui8Iteration < MAX_ITERATIONS)
	{
		if (calculateMeanValues() != 0) {return -1;}

		if (abs(CorrectedAccelMean.i16XAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16XAxis = AccelOffset.i16XAxis - CorrectedAccelMean.i16XAxis / i16AccelPrec;}

		if (abs(CorrectedAccelMean.i16YAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16YAxis = AccelOffset.i16YAxis - CorrectedAccelMean.i16YAxis / i16AccelPrec;}

		if (abs(i16Aux - CorrectedAccelMean.i16ZAxis) <= i16AccelPrec) {ui8Ready++;}
		else {AccelOffset.i16ZAxis = AccelOffset.i16ZAxis + (i16Aux - CorrectedAccelMean.i16ZAxis) / i16AccelPrec;}

		if (abs(CorrectedGyroMean.i16XAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16XAxis = GyroOffset.i16XAxis - CorrectedGyroMean.i16XAxis / i16GyroPrec;}

		if (abs(CorrectedGyroMean.i16YAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16YAxis = GyroOffset.i16YAxis - CorrectedGyroMean.i16YAxis / i16GyroPrec;}

		if (abs(CorrectedGyroMean.i16ZAxis) <= i16GyroPrec) {ui8Ready++;}
		else {GyroOffset.i16ZAxis = GyroOffset.i16ZAxis - CorrectedGyroMean.i16ZAxis / i16GyroPrec;}

		*pReady = ui8Ready;

		if (ui8Ready == 6) {return 0;}
		else               {return 1;}
	}

	return -2;
}


// Debug methods
int16_t ICM20948::setDebugFunction8(uint8_t ui8Data)
{
	if (writeRegister8(1, ICM20948_XA_OFFS_H, ui8Data) != 0) {return -1;}

	return 0;
}


int16_t ICM20948::setDebugFunction16(uint16_t ui16Data)
{
	if (writeRegister16(1, ICM20948_XA_OFFS_H, ui16Data) != 0) {return -1;}

	return 0;
}


int16_t ICM20948::getDebugFunction16(uint16_t *pData)
{
	if (readRegister16(1, ICM20948_XA_OFFS_H, pData) != 0) {return -1;}

	return 0;
}


/* Private methods */
ICM20948_RetCode_t ICM20948::init(ICM20948_FullScale_t ACCEL_FS, ICM20948_FullScale_t GYRO_FS,
		                          ICM20948_AccelSampleRate_t ACCEL_SR, ICM20948_GyroSampleRate_t GYRO_SR,
								  ICM20948_DLPF_t DLPF)
{
	uint8_t ui8Data;
	int16_t i16RetValue;

	uint32_t ui32StartTicks;

	ICM20948_SensorConfig.boStatusOK = false;
	ICM20948_SensorConfig.boSleep    = true;

	/* In cases where the sensor is already used and a controller reset occurs, the currently selected
	 * USER_BANK[1:0] in register ICM20948_REG_BANK_SEL is unknown.
	 * Therefore, we reset USER_BANK[1:0] in register ICM20948_REG_BANK_SEL --> ui8CurrentBank = 0 */
	if (resetBank() != 0) {return ICM20948_GEN_FAIL;}

	/* Check ICM20948 WHO_AM_I register */
	i16RetValue = readRegister8(0, ICM20948_WHO_AM_I, &ui8Data);
	if (i16RetValue != 0 || ui8Data != ICM20948_WHO_AM_I_VALUE) {return ICM20948_GEN_FAIL;}

	/* Reset ICM20948 device (see datasheet p. 37)
	 * - Write a 1 to DEVICE_RESET bit to set the reset, the bit will auto clear
	 * - SLEEP = 1       --> Chip is in sleep mode
	 * - CLKSEL[2:0] = 1 --> Auto select the best available clock source - PLL if ready, else use the internal oscillator */
	if (writeRegister8(0, ICM20948_PWR_MGMT_1, ICM20948_DEVICE_RESET) != 0) {return ICM20948_GEN_FAIL;}

	/* It is not specified in datasheet how long to wait after reset, but we wait 10ms and check DEVICE_RESET bit */
	ui32StartTicks = get_Ticks();
	while(get_Ticks() < (ui32StartTicks + 10)); // Delay of approximately 10ms (9...11ms)

	/* Check if DEVICE_RESET bit is cleared */
	if (readRegister8(0, ICM20948_PWR_MGMT_1, &ui8Data) != 0) {return ICM20948_GEN_FAIL;}
	if ((ui8Data & ICM20948_DEVICE_RESET) != 0) {return ICM20948_GEN_FAIL;}

	/* Initialization of ui8DataArray[] */
	for (uint8_t i = 0; i < 14; i++)
	{
		ui8DataArray[i] = 0x00;
	}

	/* Initialization of AccelOffset and GyroOffset */
	resetAccelOffset();
	resetGyroOffset();

	/* Default SensorConfig values after reset */
	ICM20948_SensorConfig.AccelFullScale  = ACCEL_FS_2G;
	ICM20948_SensorConfig.AccelSampleRate = ACCEL_SR_1125_HZ;
	ICM20948_SensorConfig.AccelDLPF       = ICM20948_DLPF_0;
	ICM20948_SensorConfig.GyroFullScale   = GYRO_FS_250DPS;
	ICM20948_SensorConfig.GyroSampleRate  = GYRO_SR_1125_HZ;
	ICM20948_SensorConfig.GyroDLPF        = ICM20948_DLPF_0;

	/* Clear SLEEP bit to wake up the chip from sleep mode */
	if (sleep(false) != 0) {return ICM20948_GEN_FAIL;}

	/* We wait a few milliseconds, otherwise the I2C_IF_DIS bit was not set in SPI mode */
	ui32StartTicks = get_Ticks();
	while(get_Ticks() < (ui32StartTicks + 4)); // Delay of approximately 4ms (3...5ms)
	if (ICM20948_SensorConfig.boUseSPI)
	{
		/* Set I2C_IF_DIS bit to prevent switching into I2C mode when using SPI (datasheet p. 28) */
		if (setRegister8Bit(0, ICM20948_USER_CTRL, ICM20948_I2C_IF_DIS) != 0) {return ICM20948_GEN_FAIL;}
		if (readRegister8(0, ICM20948_USER_CTRL, &ui8Data) != 0) {return ICM20948_GEN_FAIL;}
		if ((ui8Data & ICM20948_I2C_IF_DIS) != ICM20948_I2C_IF_DIS) {return ICM20948_GEN_FAIL;}
	}

	/* Set full scale range of accelerometer and gyroscope */
	if (setAccelFullScale(ACCEL_FS) != 0) {return ICM20948_GEN_FAIL;}
	if (setGyroFullScale(GYRO_FS) != 0) {return ICM20948_GEN_FAIL;}

	/* Set sample rate */
	if (setAccelSampleRate(ACCEL_SR) != 0) {return ICM20948_GEN_FAIL;}
	if (setGyroSampleRate(GYRO_SR) != 0) {return ICM20948_GEN_FAIL;}

	/* Set DLPF */
	if (setAccelDLPF(DLPF) != 0) {return ICM20948_GEN_FAIL;}
	if (setGyroDLPF(DLPF) != 0) {return ICM20948_GEN_FAIL;}

	/* Use default value for maximum calibration error */
	i16AccelPrec = ICM20948_ACCEL_PREC;
	i16GyroPrec  = ICM20948_GYRO_PREC;

	/* No error occurred */
	ICM20948_SensorConfig.boStatusOK = true;

	return ICM20948_RET_OK;
}


int16_t ICM20948::resetBank(void)
{
	//if (pSPI->writeByte(ICM20948_REG_BANK_SEL, 0 << 4) != 0) {return -1;}

	/* First byte includes R/W-bit (write = 0) and 7-bit memory address */
	uint8_t ui8Array[] = {0x7F & ICM20948_REG_BANK_SEL, uint8_t(0 << 4)};

	pSPI->enableNSS();
	if (pSPI->transmitSPI(ui8Array, 2) != 0) {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

	ui8CurrentBank = 0;
	return 0;
}


int16_t ICM20948::switchBank(uint8_t ui8NewBank)
{
	/* First byte includes R/W-bit (write = 0) and 7-bit memory/register address */
	uint8_t ui8Array[] = {uint8_t(0x7F & ICM20948_REG_BANK_SEL), uint8_t(ui8NewBank << 4)};

	if (ui8NewBank != ui8CurrentBank)
	{
		//if (pSPI->writeByte(ICM20948_REG_BANK_SEL, ui8NewBank << 4) != 0) {return -1;}

		pSPI->enableNSS();
		if (pSPI->transmitSPI(ui8Array, 2) != 0) {pSPI->disableNSS(); return -1;}
		pSPI->disableNSS();

		ui8CurrentBank = ui8NewBank;
	}
	return 0;
}


int16_t ICM20948::writeRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Data)
{
	/* First byte includes R/W-bit (write = 0) and 7-bit memory/register address */
	uint8_t ui8Array[] = {uint8_t(0x7F & ui8RegAddr), ui8Data};

	if (switchBank(ui8Bank) != 0) {return -1;}

	//if (pSPI->writeByte(ui8RegAddr, ui8Data) != 0) {return -1;}

	pSPI->enableNSS();
	if (pSPI->transmitSPI(ui8Array, 2) != 0) {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

	return 0;
}


int16_t ICM20948::readRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t *pData)
{
	/* Byte includes R/W-bit (read = 1) and 7-bit memory/register address */
	uint8_t ui8Data = 0x80 | ui8RegAddr;

	if (switchBank(ui8Bank) != 0) {return -1;}

	//if (pSPI->readByte(ui8RegAddr, pData) != 0) {return -1;}

	pSPI->enableNSS();
	if (pSPI->transmitSPI(&ui8Data, 1) != 0) {pSPI->disableNSS(); return -1;}
	if (pSPI->receiveSPI(pData, 1) != 0)     {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

	return 0;
}


int16_t ICM20948::writeRegister16(uint8_t ui8Bank, uint8_t ui8RegAddrH, uint16_t ui16Data)
{
	uint8_t ui8Array[3];

	ui8Array[0] = 0x7F & ui8RegAddrH;
	ui8Array[1] = ui16Data >> 8; // High byte is transmitted first
	ui8Array[2] = ui16Data;      // Low byte

//	uint8_t ui8Data[2];

//	ui8Data[0] = ui16Data >> 8; // High byte is transmitted first
//	ui8Data[1] = ui16Data;      // Low byte

	if (switchBank(ui8Bank) != 0) {return -1;}

	pSPI->enableNSS();
	if (pSPI->transmitSPI(ui8Array, 3) != 0) {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

//	if (pSPI->writeBurst(ui8RegAddrH, 2, ui8Data) != 0) {return -1;}

	return 0;
}


int16_t ICM20948::readRegister16(uint8_t ui8Bank, uint8_t ui8RegAddrH, uint16_t *pData)
{
	/* Byte includes R/W-bit (read = 1) and 7-bit memory/register address */
	uint8_t ui8Data = 0x80 | ui8RegAddrH;

	uint8_t ui8Array[2];

	if (switchBank(ui8Bank) != 0) {return -1;}

	//if (pSPI->readBurst(ui8RegAddrH, 2, ui8Data) != 0) {return -1;}
	//*pData = (ui8Data[0] << 8) | ui8Data[1];

	pSPI->enableNSS();
	if (pSPI->transmitSPI(&ui8Data, 1) != 0) {pSPI->disableNSS(); return -1;}
	if (pSPI->receiveSPI(ui8Array, 2) != 0)      {pSPI->disableNSS(); return -1;}
	pSPI->disableNSS();

	*pData = (ui8Array[0] << 8) | ui8Array[1];

	return 0;
}


int16_t ICM20948::setRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos)
{
	uint8_t ui8Data;

	/* Check argument ui8Pos */
	if (isValidPos(ui8Pos) == 0) {return -1;}

	if (readRegister8(ui8Bank, ui8RegAddr, &ui8Data) != 0) {return -1;}

	ui8Data |= ui8Pos; // Set bit
	if (writeRegister8(ui8Bank, ui8RegAddr, ui8Data) != 0) {return -1;}

	return 0;
}


int16_t ICM20948::clearRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos)
{
	uint8_t ui8Data;

	/* Check argument ui8Pos */
	if (isValidPos(ui8Pos) == 0) {return -1;}

	if (readRegister8(ui8Bank, ui8RegAddr, &ui8Data) != 0) {return -1;}

	ui8Data &= ~ui8Pos; // Clear bit
	if (writeRegister8(ui8Bank, ui8RegAddr, ui8Data) != 0) {return -1;}

	return 0;
}


int16_t ICM20948::getRegister8Bit(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Pos, bool *pValue)
{
	uint8_t ui8Data;

	/* Check argument ui8Pos */
	if (isValidPos(ui8Pos) == 0) {return -1;}

	if (readRegister8(ui8Bank, ui8RegAddr, &ui8Data) != 0) {return -1;}

	ui8Data &= ui8Pos;

	if (ui8Data) {*pValue = true;}
	else         {*pValue = false;}

	return 0;
}


int16_t ICM20948::changeRegister8(uint8_t ui8Bank, uint8_t ui8RegAddr, uint8_t ui8Msk, uint8_t ui8Value)
{
	uint8_t ui8Data;

	if (readRegister8(ui8Bank, ui8RegAddr, &ui8Data) != 0) {return -1;}

	ui8Data  &= ~ui8Msk;  // Clear masked bits
	ui8Value &= ui8Msk;   // Only masked bits can be changed by ui8Value
	ui8Data  |= ui8Value;

	if (writeRegister8(ui8Bank, ui8RegAddr, ui8Data) != 0) {return -1;}

	return 0;
}


inline bool ICM20948::isValidPos(uint8_t ui8Pos)
{
	return ((ui8Pos == 0x01) || (ui8Pos == 0x02) || (ui8Pos == 0x04) || (ui8Pos == 0x08) ||
			(ui8Pos == 0x10) || (ui8Pos == 0x20) || (ui8Pos == 0x40) || (ui8Pos == 0x80));
}


inline bool ICM20948::isValidAccelFullScale(ICM20948_FullScale_t FullScale)
{
	return (IS_VALID_FULL_SCALE(FullScale,ACCEL_FS_2G) || IS_VALID_FULL_SCALE(FullScale,ACCEL_FS_4G) ||
			IS_VALID_FULL_SCALE(FullScale,ACCEL_FS_8G) || IS_VALID_FULL_SCALE(FullScale,ACCEL_FS_16G));
}


inline bool ICM20948::isValidGyroFullScale(ICM20948_FullScale_t FullScale)
{
	return (IS_VALID_FULL_SCALE(FullScale,GYRO_FS_250DPS)  || IS_VALID_FULL_SCALE(FullScale,GYRO_FS_500DPS) ||
			IS_VALID_FULL_SCALE(FullScale,GYRO_FS_1000DPS) || IS_VALID_FULL_SCALE(FullScale,GYRO_FS_2000DPS));
}


inline bool ICM20948::isValidAccelSampleRate(ICM20948_AccelSampleRate_t SampleRate)
{
	return (IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_4500_HZ)  || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_1125_HZ)  ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_562_5_HZ) || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_375_HZ)   ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_281_3_HZ) || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_225_HZ)   ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_187_5_HZ) || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_140_6_HZ) ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_125_HZ)   || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_102_3_HZ) ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_70_3_HZ)  || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_66_2_HZ)  ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_48_9_HZ)  || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_34_1_HZ)  ||
			IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_17_3_HZ)  || IS_VALID_ACCEL_SAMPLE_RATE(SampleRate,ACCEL_SR_4_4_HZ));
}


inline bool ICM20948::isValidGyroSampleRate(ICM20948_GyroSampleRate_t SampleRate)
{
	return (IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_9000_HZ)  || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_1125_HZ)  ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_562_5_HZ) || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_375_HZ)   ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_281_3_HZ) || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_225_HZ)   ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_187_5_HZ) || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_140_6_HZ) ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_125_HZ)   || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_102_3_HZ) ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_70_3_HZ)  || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_66_2_HZ)  ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_48_9_HZ)  || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_34_1_HZ)  ||
			IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_17_3_HZ)  || IS_VALID_GYRO_SAMPLE_RATE(SampleRate,GYRO_SR_4_4_HZ));
}


inline bool ICM20948::isValidDLPF(ICM20948_DLPF_t DLPF)
{
	return ((DLPF == ICM20948_DLPF_0) || (DLPF == ICM20948_DLPF_1) || (DLPF == ICM20948_DLPF_2) || (DLPF == ICM20948_DLPF_3) ||
			(DLPF == ICM20948_DLPF_4) || (DLPF == ICM20948_DLPF_5) || (DLPF == ICM20948_DLPF_6) || (DLPF == ICM20948_DLPF_7));
}
