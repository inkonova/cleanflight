/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>

#include <limits.h>

#define BARO

extern "C" {
    #include "common/axis.h"
    #include "common/maths.h"

    #include "sensors/sensors.h"
    #include "drivers/sensor.h"
    #include "drivers/accgyro.h"
    #include "drivers/compass.h"
    #include "sensors/gyro.h"
    #include "sensors/compass.h"
    #include "sensors/acceleration.h"
    #include "sensors/barometer.h"

    #include "config/runtime_config.h"

    #include "flight/mixer.h"
    #include "flight/pid.h"
    #include "flight/imu.h"
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

#define DOWNWARDS_THRUST true
#define UPWARDS_THRUST false


TEST(FlightImuTest, TestCalculateHeading)
{
    //TODO: Add test cases using the Z dimension.
    t_fp_vector north = {.A={1.0f, 0.0f, 0.0f}};
    EXPECT_EQ(imuCalculateHeading(&north), 0);

    t_fp_vector east = {.A={0.0f, 1.0f, 0.0f}};
    EXPECT_EQ(imuCalculateHeading(&east), 90);

    t_fp_vector south = {.A={-1.0f, 0.0f, 0.0f}};
    EXPECT_EQ(imuCalculateHeading(&south), 180);

    t_fp_vector west = {.A={0.0f, -1.0f, 0.0f}};
    EXPECT_EQ(imuCalculateHeading(&west), 270);

    t_fp_vector north_east = {.A={1.0f, 1.0f, 0.0f}};
    EXPECT_EQ(imuCalculateHeading(&north_east), 45);
}

extern float anglerad[];

TEST(FlightImuTest, TestCalculateThrottleAngleCorrectionAxis)
{
    const uint16_t TRUST = 3;
    float testAnglesAndResults[][2] = {
        {0, 0}, //at 0 deg, no compensation

        {M_PIf/6, 5},            //at 30 deg, compensantion sould be about 0.464101791
        {M_PIf/4, 12},             //at 45 deg compesantion shuld be about 1.242640972
        {M_PIf/2-M_PIf/6, 30.00000477f},    //at 60 deg compesantion shuld be about 3.000000477
        {M_PIf/2, 1000},                    //at 90 deg compesantion shuld be 1000 becuse function should limit to that

        {-M_PIf/6, 5},           //at -30 deg, compensantion sould be about 0.464101791
        {-M_PIf/4, 12},            //at -45 deg compesantion shuld be about 1.242640972
        {-M_PIf/2+M_PIf/6, 30.00000477f},   //at -60 deg compesantion shuld be about 3.000000477
        {-M_PIf/2, 1000},                   //at -90 deg compesantion shuld be 1000 becuse function should limit to that

    }; //test value

    const float MAX_ERROR = 0.000001f;

    const size_t SIZE_OF_TEST_ARRAY = sizeof(testAnglesAndResults)/sizeof(testAnglesAndResults[0]);

    size_t test_angle = 0;
    int16_t result;
    for (size_t i=0; i < SIZE_OF_TEST_ARRAY; i++){
        anglerad[test_angle] = testAnglesAndResults[i][0];
        result = calculateThrottleAngleCorrectionAxis(TRUST, test_angle);

        EXPECT_LT( ABS(result), testAnglesAndResults[i][1]+MAX_ERROR );
    }
}


// STUBS

extern "C" {
uint32_t rcModeActivationMask;
int16_t rcCommand[4];

uint16_t acc_1G;
int16_t heading;
gyro_t gyro;
int16_t magADC[XYZ_AXIS_COUNT];
int32_t BaroAlt;
int16_t debug[4];

uint8_t stateFlags;
uint16_t flightModeFlags;
uint8_t armingFlags;

int32_t sonarAlt;
int16_t accADC[XYZ_AXIS_COUNT];
int16_t gyroADC[XYZ_AXIS_COUNT];



void gyroUpdate(void) {};
bool sensors(uint32_t mask)
{
    UNUSED(mask);
    return false;
};
void updateAccelerationReadings(rollAndPitchTrims_t *rollAndPitchTrims)
{
    UNUSED(rollAndPitchTrims);
}

uint32_t micros(void) { return 0; }
bool isBaroCalibrationComplete(void) { return true; }
void performBaroCalibrationCycle(void) {}
int32_t baroCalculateAltitude(void) { return 0; }
}
