/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>

#include "MAX30100_SpO2Calculator.h"

// SaO2 Look-up Table
// http://www.ti.com/lit/an/slaa274b/slaa274b.pdf
uint8_t spO2LUT[43]/*SpO2Calculator::spO2LUT[43]*/ = {100,100,100,100,99,99,99,99,99,99,98,98,98,98,
                                                      98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,
                                                      95,95,95,95,94,94,94,94,94,93,93,93,93,93};


float irACValueSqSum;
float redACValueSqSum;
uint8_t beatsDetectedNum;
uint32_t samplesRecorded;
uint8_t spO2;

void SpO2Calculator()
{
    irACValueSqSum=0;
    redACValueSqSum=0;
    beatsDetectedNum=0;
    samplesRecorded=0;
    spO2=0;
}

void SPO2update(float irACValue, float redACValue, u8 beatDetected)//SpO2Calculator::update(float irACValue, float redACValue, bool beatDetected)
{
    irACValueSqSum += irACValue * irACValue;
    redACValueSqSum += redACValue * redACValue;
    ++samplesRecorded;

    if (1) {
        ++beatsDetectedNum;
        if (1) {
            float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);
            uint8_t index = 0;

            if (acSqRatio > 66) {
                index = (uint8_t)acSqRatio - 66;
            } else if (acSqRatio > 50) {
                index = (uint8_t)acSqRatio - 50;
            }
            //reset();

            spO2 = spO2LUT[index];
        }
    }
}

void reset()//SpO2Calculator::reset()
{
    samplesRecorded = 0;
    redACValueSqSum = 0;
    irACValueSqSum = 0;
    beatsDetectedNum = 0;
    spO2 = 0;
}

uint8_t getSpO2()//SpO2Calculator::getSpO2()
{
    return spO2;
}