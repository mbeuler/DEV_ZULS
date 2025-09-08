/*
 * icm20948reg.hpp
 *
 *  Created on: Mar 29, 2024
 *      Author: mbeuler
 */

#ifndef ZULS_INCLUDE_ICM20948REG_HPP_
#define ZULS_INCLUDE_ICM20948REG_HPP_


/* I2C address for the ICM20948 (AD0 = 0, see datasheet p. 14) */
constexpr uint8_t ICM20948_I2C_BASE_ADDR    {0x68};

/* Value of WHO_AM_I register */
constexpr uint8_t ICM20948_WHO_AM_I_VALUE        {0xEA}; // See datasheet p. 36

/*************************************************************************************
 * User bank 0 (ICM-20948 datasheet p. 32 f.)
 *************************************************************************************/
constexpr uint8_t ICM20948_WHO_AM_I              {0x00};    // Permission: R
constexpr uint8_t ICM20948_USER_CTRL             {0x03};    // Permission: R/W
constexpr uint8_t ICM20948_LP_CONFIG             {0x05};    // Permission: R/W
constexpr uint8_t ICM20948_PWR_MGMT_1            {0x06};    // Permission: R/W
constexpr uint8_t ICM20948_PWR_MGMT_2            {0x07};    // Permission: R/W
constexpr uint8_t ICM20948_INT_PIN_CFG           {0x0F};    // Permission: R/W
constexpr uint8_t ICM20948_INT_ENABLE            {0x10};    // Permission: R/W
constexpr uint8_t ICM20948_INT_ENABLE_1          {0x11};    // Permission: R/W
constexpr uint8_t ICM20948_INT_ENABLE_2          {0x12};    // Permission: R/W
constexpr uint8_t ICM20948_INT_ENABLE_3          {0x13};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_MST_STATUS        {0x17};    // Permission: R/C
constexpr uint8_t ICM20948_INT_STATUS            {0x19};    // Permission: R/C
constexpr uint8_t ICM20948_INT_STATUS_1          {0x1A};    // Permission: R/C
constexpr uint8_t ICM20948_INT_STATUS_2          {0x1B};    // Permission: R/C
constexpr uint8_t ICM20948_INT_STATUS_3          {0x1C};    // Permission: R/C
constexpr uint8_t ICM20948_DELAY_TIMEH           {0x28};    // Permission: R
constexpr uint8_t ICM20948_DELAY_TIMEL           {0x29};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_XOUT_H          {0x2D};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_XOUT_L          {0x2E};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_YOUT_H          {0x2F};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_YOUT_L          {0x30};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_ZOUT_H          {0x31};    // Permission: R
constexpr uint8_t ICM20948_ACCEL_ZOUT_L          {0x32};    // Permission: R
constexpr uint8_t ICM20948_GYRO_XOUT_H           {0x33};    // Permission: R
constexpr uint8_t ICM20948_GYRO_XOUT_L           {0x34};    // Permission: R
constexpr uint8_t ICM20948_GYRO_YOUT_H           {0x35};    // Permission: R
constexpr uint8_t ICM20948_GYRO_YOUT_L           {0x36};    // Permission: R
constexpr uint8_t ICM20948_GYRO_ZOUT_H           {0x37};    // Permission: R
constexpr uint8_t ICM20948_GYRO_ZOUT_L           {0x38};    // Permission: R
constexpr uint8_t ICM20948_TEMP_OUT_H            {0x39};    // Permission: R
constexpr uint8_t ICM20948_TEMP_OUT_L            {0x3A};    // Permission: R
constexpr uint8_t ICM20948_EXT_SLV_SENS_DATA_00  {0x3B};    // Permission: R
constexpr uint8_t ICM20948_EXT_SLV_SENS_DATA_01  {0x3C};    // Permission: R
constexpr uint8_t ICM20948_FIFO_EN_1             {0x66};    // Permission: R/W
constexpr uint8_t ICM20948_FIFO_EN_2             {0x67};    // Permission: R/W
constexpr uint8_t ICM20948_FIFO_RST              {0x68};    // Permission: R/W
constexpr uint8_t ICM20948_FIFO_MODE             {0x69};    // Permission: R/W
constexpr uint8_t ICM20948_FIFO_COUNTH           {0x70};    // Permission: R
constexpr uint8_t ICM20948_FIFO_COUNTL           {0x71};    // Permission: R
constexpr uint8_t ICM20948_FIFO_R_W              {0x72};    // Permission: R/W
constexpr uint8_t ICM20948_DATA_RDY_STATUS       {0x74};    // Permission: R/C
constexpr uint8_t ICM20948_FIFO_CFG              {0x76};    // Permission: R/W

/*************************************************************************************
 * User bank 1 (ICM-20948 datasheet p. 33 f.)
 *************************************************************************************/
constexpr uint8_t ICM20948_SELF_TEST_X_GYRO      {0x02};    // Permission: R/W
constexpr uint8_t ICM20948_SELF_TEST_Y_GYRO      {0x03};    // Permission: R/W
constexpr uint8_t ICM20948_SELF_TEST_Z_GYRO      {0x04};    // Permission: R/W
constexpr uint8_t ICM20948_SELF_TEST_X_ACCEL     {0x0E};    // Permission: R/W
constexpr uint8_t ICM20948_SELF_TEST_Y_ACCEL     {0x0F};    // Permission: R/W
constexpr uint8_t ICM20948_SELF_TEST_Z_ACCEL     {0x10};    // Permission: R/W
constexpr uint8_t ICM20948_XA_OFFS_H             {0x14};    // Permission: R/W
constexpr uint8_t ICM20948_XA_OFFS_L             {0x15};    // Permission: R/W
constexpr uint8_t ICM20948_YA_OFFS_H             {0x17};    // Permission: R/W
constexpr uint8_t ICM20948_YA_OFFS_L             {0x18};    // Permission: R/W
constexpr uint8_t ICM20948_ZA_OFFS_H             {0x1A};    // Permission: R/W
constexpr uint8_t ICM20948_ZA_OFFS_L             {0x1B};    // Permission: R/W
constexpr uint8_t ICM20948_TIMEBASE_CORR_PLL     {0x28};    // Permission: R/W

/*************************************************************************************
 * User bank 2 (ICM-20948 datasheet p. 34)
 *************************************************************************************/
constexpr uint8_t ICM20948_GYRO_SMPLRT_DIV       {0x00};    // Permission: R/W
constexpr uint8_t ICM20948_GYRO_CONFIG_1         {0x01};    // Permission: R/W
constexpr uint8_t ICM20948_GYRO_CONFIG_2         {0x02};    // Permission: R/W
constexpr uint8_t ICM20948_XG_OFFS_USRH          {0x03};    // Permission: R/W
constexpr uint8_t ICM20948_XG_OFFS_USRL          {0x04};    // Permission: R/W
constexpr uint8_t ICM20948_YG_OFFS_USRH          {0x05};    // Permission: R/W
constexpr uint8_t ICM20948_YG_OFFS_USRL          {0x06};    // Permission: R/W
constexpr uint8_t ICM20948_ZG_OFFS_USRH          {0x07};    // Permission: R/W
constexpr uint8_t ICM20948_ZG_OFFS_USRL          {0x08};    // Permission: R/W
constexpr uint8_t ICM20948_ODR_ALIGN_EN          {0x09};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_SMPLRT_DIV_1    {0x10};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_SMPLRT_DIV_2    {0x11};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_INTEL_CTRL      {0x12};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_WOM_THR         {0x13};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_CONFIG          {0x14};    // Permission: R/W
constexpr uint8_t ICM20948_ACCEL_CONFIG_2        {0x15};    // Permission: R/W
constexpr uint8_t ICM20948_FSYNC_CONFIG          {0x52};    // Permission: R/W
constexpr uint8_t ICM20948_TEMP_CONFIG           {0x53};    // Permission: R/W
constexpr uint8_t ICM20948_MOD_CTRL_USR          {0x54};    // Permission: R/W

/*************************************************************************************
 * User bank 3 (ICM-20948 datasheet p. 34 f.)
 *************************************************************************************/
constexpr uint8_t ICM20948_I2C_MST_ODR_CFG       {0x00};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_MST_CTRL          {0x01};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_MST_DELAY_CTRL    {0x02};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV0_ADDR         {0x03};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV0_REG          {0x04};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV0_CTRL         {0x05};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV0_DO           {0x06};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV1_ADDR         {0x07};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV1_REG          {0x08};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV1_CTRL         {0x09};    // Permission: R/W
constexpr uint8_t ICM20948_I2C_SLV1_DO           {0x0A};    // Permission: R/W

/*************************************************************************************
 * All banks (ICM-20948 datasheet p. 33 ff.)
 *************************************************************************************/
constexpr uint8_t ICM20948_REG_BANK_SEL          {0x7F};    // Permission: R/W

/*************************************************************************************
 * Register bits
 *************************************************************************************/
constexpr uint8_t ICM20948_I2C_IF_DIS            {0x10};    // ICM20948_USER_CTRL (datasheet p. 36)

constexpr uint8_t ICM20948_DEVICE_RESET          {0x80};    // ICM20948_PWR_MGMT_1 (datasheet p. 37)
constexpr uint8_t ICM20948_SLEEP                 {0x40};    // ICM20948_PWR_MGMT_1

constexpr uint8_t ICM20948_GYRO_DLPFCFG          {0x38};    // ICM20948_GYRO_CONFIG_1 (datasheet p. 59)
constexpr uint8_t ICM20948_GYRO_FS_SEL           {0x06};    // ICM20948_GYRO_CONFIG_1
constexpr uint8_t ICM20948_GYRO_FCHOICE          {0x01};    // ICM20948_GYRO_CONFIG_1

constexpr uint8_t ICM20948_ACCEL_DLPFCFG         {0x38};    // ICM20948_ACCEL_CONFIG (datasheet p. 64)
constexpr uint8_t ICM20948_ACCEL_FS_SEL          {0x06};    // ICM20948_ACCEL_CONFIG
constexpr uint8_t ICM20948_ACCEL_FCHOICE         {0x01};    // ICM20948_ACCEL_CONFIG


#endif /* ZULS_INCLUDE_ICM20948REG_HPP_ */
