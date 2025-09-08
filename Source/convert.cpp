/*
 * convert.cpp
 *
 *  Created on: 07.01.2023
 *      Author: marcel.beuler
 */

#include "convert.hpp"
#include <stdint.h>



/* Public methods */
/**
  @brief  This function replaces itoa. In contrast to itoa, it returns a string.
  @param  None
  @return xxx
**/
std::string CONVERT::convIntToStr(int iValue, int base)
{
	int iAux;
	bool isNegative = false;

	std::string str;

	/* Handle 0 explicitly, otherwise empty string is returned for 0 */
	if (iValue == 0) {return str = '0';}

	/* Negative numbers are only handled with base 10, otherwise they are considered unsigned */
	if (iValue < 0)
	{
		iValue = -iValue;
		if (base == 10) {isNegative = true;}
	}

	/* Process individual digits */
	while(iValue != 0)
	{
		iAux   = iValue % base;
		iValue = iValue / base;

		if (iAux > 9) {str += (iAux - 10) + 'A';}
		else          {str += iAux + '0';}
	}

	if (isNegative) {str += '-';}

	reverse(&str[0], str.length());

	return str;
}


/* The function handles floats with an integer component up to 4,294,967,295 in size.
 * It replaces ftoa. In contrast to ftoa, it returns a string. */
std::string CONVERT::convFloatToStr(float fValue, uint8_t ui8TotalDigits)
{
    uint8_t i, j, divisor = 1;
    uint8_t ui8IntDigits; // integer digits (before the decimal)
    uint32_t ui32Value;
    float fDecimals;

    std::string strFloat;
    std::string strAux;

    if (fValue < 0) // if a negative number
    {
        fValue = -fValue; // store its positive absolute value
        strFloat = '-';
    }

    ui32Value = (int)fValue; // store the integer value

    ui8IntDigits = ceil(std::log10(fValue));  // get integer digits (before the decimal)
    ui8TotalDigits = ui8IntDigits + 2;  // Additional code line, M. Beuler (2023-08-05)
    if (ui8IntDigits > 0) // ui8IntDigits > 0 indicates an fValue > 1
    {
        if (ui8IntDigits == ui8TotalDigits)     // if integer digits = total digits
        {
            fValue += 0.5;                      // add 0.5 to round up decimals >= 0.5
            strAux = convIntToStr((int)fValue);
            strFloat += strAux;
        }
        else if ((ui8IntDigits - ui8TotalDigits) > 0) // if more integer digits than total digits
        {
            i = ui8IntDigits - ui8TotalDigits;        // count digits to discard
            divisor = 10;
            for (j = 0; j < i; j++) {divisor *= 10;}  // divisor isolates our desired integer digits
            fValue /= divisor;                        // divide
            fValue += 0.5;                            // round when converting to int
            ui32Value = (int)fValue;
            ui32Value *= divisor;                     // and multiply back to the adjusted value
            strAux = convIntToStr(ui32Value);
            strFloat += strAux;
        }
        else // if more total number of digits specified than integer digits
        {
            strAux = convIntToStr(ui32Value); // convert
            strFloat += strAux;               // and append
        }
    }
    else // decimal fractions between 0 and 1: leading 0
    {
        strAux = '0';
        strFloat += strAux;
    }

    if (ui8IntDigits < ui8TotalDigits)    // if total digits exceeds integer digits
    {
        fDecimals = fValue - (int)fValue; // get decimal value as float
        strAux = '.';                     // append decimal point
        strFloat += strAux;

        i = ui8TotalDigits - ui8IntDigits;         // number of decimals to read
        for (j = 0; j < i; j++)                    // for each
        {
            fDecimals *= 10;                       // multiply decimals by 10
            if (j == (i-1)) {fDecimals += 0.5;}    // and if it's the last, add 0.5 to round it
            strAux = convIntToStr((int)fDecimals); // convert as integer
            strFloat += strAux;                    // and append

            fDecimals -= (int)fDecimals;           // and remove, moving to the next
        }
    }

    return strFloat;
}


/**
  @brief  xxx
  @param  None
  @return xxx
**/
float CONVERT::convUintToFloat(uint32_t ui32Data)
{
	union
	{
		/* Data in a union is not stored sequentially, but in the same memory area */
		float f;
		uint32_t i;
	}Union;

	Union.i = ui32Data;

	return Union.f;
}


/* Private methods */
/**
  @brief  xxx
  @param  None
  @return xxx
**/
void CONVERT::reverse(char Array[], int length)
{
	int i = 0, j = length - 1, temp;

	while(i < j)
	{
		temp     = Array[i];
		Array[i] = Array[j];
		Array[j] = temp;
		i++;
		j--;
	}
}


