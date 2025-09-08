/*
 * convert.hpp
 *
 *  Created on: 07.01.2023
 *      Author: marcel.beuler
 */

#ifndef ZULS_INCLUDE_CONVERT_HPP_
#define ZULS_INCLUDE_CONVERT_HPP_

#include <stdint.h>
#include <cmath>
#include <string>


class CONVERT
{
public:
	/* Methods */
	std::string convIntToStr(int iValue, int base = 10);
	std::string convFloatToStr(float fValue, uint8_t ui8TotalDigits = 6);

	float convUintToFloat(uint32_t ui32Data);


private:
	/* Methods */
	void reverse(char str[], int length);
};


#endif /* ZULS_INCLUDE_CONVERT_HPP_ */
