/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "CodalConfig.h"
#include "DataStream.h"

#ifndef LEVEL_DETECTOR_SPL_H
#define LEVEL_DETECTOR_SPL_H

/**
 * Status values
 */
#define LEVEL_DETECTOR_SPL_INITIALISED                       0x01
#define LEVEL_DETECTOR_SPL_HIGH_THRESHOLD_PASSED             0x02
#define LEVEL_DETECTOR_SPL_LOW_THRESHOLD_PASSED              0x04

/**
 * Default configuration values
 */
#define LEVEL_DETECTOR_SPL_DEFAULT_WINDOW_SIZE              128

#ifndef LEVEL_DETECTOR_SPL_NORMALIZE
#define LEVEL_DETECTOR_SPL_NORMALIZE    1
#endif

/**
 * Define the parameters for the dB->8bit translation function.
 */

// The level (in dB) that corresponds to an 8bit value of 0.
#ifndef LEVEL_DETECTOR_SPL_8BIT_000_POINT
#define LEVEL_DETECTOR_SPL_8BIT_000_POINT                   35.0f
#endif

// The level (in dB) that corresponds to an 8bit value of 255.
#ifndef LEVEL_DETECTOR_SPL_8BIT_255_POINT
#define LEVEL_DETECTOR_SPL_8BIT_255_POINT                   100.0f
#endif

#define LEVEL_DETECTOR_SPL_8BIT_CONVERSION                  (255.0f/(LEVEL_DETECTOR_SPL_8BIT_255_POINT-LEVEL_DETECTOR_SPL_8BIT_000_POINT))

/**
 * Level detetor unit enumeration.
 */
#define LEVEL_DETECTOR_SPL_DB                               1
#define LEVEL_DETECTOR_SPL_8BIT                             2


namespace codal{
    class LevelDetectorSPL : public CodalComponent, public DataSink
    {
    public:

        // The stream component that is serving our data
        DataSource      &upstream;          // The component producing data to process
        float           highThreshold;      // threshold at which a HIGH event is generated
        float           lowThreshold;       // threshold at which a LOW event is generated
        int             windowSize;         // The number of samples the make up a level detection window.
        float           level;              // The current, instantaneous level.
        int             sigma;              // Running total of the samples in the current window.
        float           gain;
        float           minValue;
        bool            activated;          // Has this component been connected yet
        bool            enabled;            // Is the component currently running
        int             unit;               // The units to be returned from this level detector (e.g. dB or linear 8bit)

        /**
          * Creates a component capable of measuring and thresholding stream data
          *
          * @param source a DataSource to measure the level of.
          * @param highThreshold the HIGH threshold at which a SPL_LEVEL_THRESHOLD_HIGH event will be generated
          * @param lowThreshold the HIGH threshold at which a SPL_LEVEL_THRESHOLD_LOW event will be generated
          * @param id The id to use for the message bus when transmitting events.
          * @param connectImmediately Should this component connect to upstream splitter when started
          */
        LevelDetectorSPL(DataSource &source, float highThreshold, float lowThreshold, float gain,
            float minValue = 52,
            uint16_t id = DEVICE_ID_SYSTEM_LEVEL_DETECTOR_SPL,
            bool connectImmediately  = true);

        /**
         * Callback provided when data is ready.
         */
    	virtual int pullRequest();

        /*
         * Determines the instantaneous value of the sensor, in SI units, and returns it.
         *
         * @return The current value of the sensor.
         */
        float getValue();

        /**
         * Disable component
         */
        void disable();

        /**
         * Set threshold to the given value. Events will be generated when these thresholds are crossed.
         *
         * @param value the LOW threshold at which a ANALOG_THRESHOLD_LOW will be generated.
         *
         * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if the request fails.
         */
        int setLowThreshold(float value);

        /**
         * Set threshold to the given value. Events will be generated when these thresholds are crossed.
         *
         * @param value the HIGH threshold at which a ANALOG_THRESHOLD_HIGH will be generated.
         *
         * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if the request fails.
         */
        int setHighThreshold(float value);

        /**
         * Determines the currently defined low threshold.
         *
         * @return The current low threshold. DEVICE_INVALID_PARAMETER if no threshold has been defined.
         */
        float getLowThreshold();

        /**
         * Determines the currently defined high threshold.
         *
         * @return The current high threshold. DEVICE_INVALID_PARAMETER if no threshold has been defined.
         */
        float getHighThreshold();

        /**
         * Set the window size to the given value. The window size defines the number of samples used to determine a sound level.
         * The higher the value, the more accurate the result will be. The lower the value, the more responsive the result will be.
         * Adjust this value to suit the requirements of your applicaiton.
         *
         * @param size The size of the window to use (number of samples).
         *
         * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if the request fails.
         */
        int setWindowSize(int size);

        int setGain(float gain);

        /**
         * Defines the units that will be returned by the getValue() function.
         *
         * @param unit Either LEVEL_DETECTOR_SPL_DB or LEVEL_DETECTOR_SPL_8BIT.
         * @return DEVICE_OK or DEVICE_INVALID_PARAMETER.
         */
         int setUnit(int unit);

        /**
         * Destructor.
         */
        ~LevelDetectorSPL();

        private:
        float splToUnit(float f);
        float unitToSpl(float f);
    };
}

#endif
