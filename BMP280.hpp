/**
 * @file BMP280.hpp
 * @author Jeremy Dunne 
 * @brief Interface library for the BMP280 barometer 
 * @version 0.1
 * @date 2022-12-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _BMP_280_HPP_
#define _BMP_280_HPP_

// includes 
#include <Arduino.h> 
#include <SPI.h> 


// registers 
#define BMP280_TEMP_XLSB                0xFC 
#define BMP280_TEMP_LSB                 0xFB 
#define BMP280_TEMP_MSB                 0xFA 
#define BMP280_PRESS_XLSB               0xF9 
#define BMP280_PRESS_LSB                0xF8 
#define BMP280_PRESS_MSB                0xF7 
#define BMP280_CONFIG                   0xF5
#define BMP280_CTRL_MEAS                0xF4 
#define BMP280_STATUS                   0xF3 
#define BMP280_RESET                    0xE0 
#define BMP280_ID                       0xD0 
#define BMP280_CALIB_25                 0x88 

#define BMP280_RESET_COMMAND            0x58 

// pre definitions & settings
#define BMP280_ID_RESP                  0x58 



// ctrl_meas settings 
#define BMP280_TEMP_OVERSAMPLE_SKIPPED  0b00000000
#define BMP280_TEMP_OVERSAMPLE_1        0b00100000
#define BMP280_TEMP_OVERSAMPLE_2        0b01000000
#define BMP280_TEMP_OVERSAMPLE_4        0b01100000
#define BMP280_TEMP_OVERSAMPLE_8        0b10000000
#define BMP280_TEMP_OVERSAMPLE_16       0b10100000

#define BMP280_PRES_OVERSAMPLE_SKIPPED  0b00000000
#define BMP280_PRES_OVERSAMPLE_1        0b00000100
#define BMP280_PRES_OVERSAMPLE_2        0b00001000
#define BMP280_PRES_OVERSAMPLE_4        0b00001100
#define BMP280_PRES_OVERSAMPLE_8        0b00010000
#define BMP280_PRES_OVERSAMPLE_16       0b00010100

#define BMP280_MEAS_SLEEP_MODE          0b00000000
#define BMP280_MEAS_FORCED_MODE         0b00000001
#define BMP280_MEAS_NORMAL_MODE         0b00000011

// config settings 
#define BMP280_T_STDBY_00005            0b00000000 
#define BMP280_T_STDBY_00625            0b00100000 
#define BMP280_T_STDBY_01250            0b01000000 
#define BMP280_T_STDBY_02500            0b01100000 
#define BMP280_T_STDBY_05000            0b10000000 
#define BMP280_T_STDBY_10000            0b10100000 
#define BMP280_T_STDBY_20000            0b11000000 
#define BMP280_T_STDBY_40000            0b11100000 

#define BMP280_FILTER_0                 0b00000000
#define BMP280_FILTER_2                 0b00000100
#define BMP280_FILTER_4                 0b00001000
#define BMP280_FILTER_8                 0b00001100
#define BMP280_FILTER_16                0b00010000

// SPI Settings 
#define BMP280_SPI_FREQ         10000000    // 10 MHz max speed 
#define BMP280_SPI_MODE         SPI_MODE_0  
#define BMP280_SPI_DATA_ORDER   MSBFIRST 



// Return Type 
typedef enum{
    BMP280_OK = 0, 
    BMP280_UNKNOWN_ID, 
    BMP280_COMM_FAIL
} BMP280_status_t; ///< standard return type for the BMP280 class 

/**
 * @brief interface library for the BMP280 barometer 
 * 
 */
class BMP280{
public: 
    /**
     * @brief initialize the barometer 
     * 
     * Begins communication with the barometer, checks the device ID, and gets the calibration 
     *  data from the chip 
     * 
     * @param cs_pin chip select pin 
     * @return BMP280_status_t 
     */
    BMP280_status_t init(int cs_pin); 

    // chip registers functions 
    
    /**
     * @brief read the ID of the chip 
     * 
     * @param id byte to store the ID in 
     * @return BMP280_status_t 
     */
    BMP280_status_t readId(byte* id); 

    /**
     * @brief reset the barometer 
     * 
     * @return BMP280_status_t 
     */
    BMP280_status_t reset(); 

    /**
     * @brief read the status byte 
     * 
     * @param status byte to read the status into 
     * @return BMP280_status_t 
     */
    BMP280_status_t readStatus(byte* status); 

    /**
     * @brief write the ctrl measurement register 
     * 
     * @param ctrl_meas data to write into the ctrl register 
     * @return BMP280_status_t 
     */
    BMP280_status_t writeCtrlMeas(byte ctrl_meas); 

    /**
     * @brief write to the config register 
     * 
     * @param config data to write into the config register 
     * @return BMP280_status_t 
     */
    BMP280_status_t writeConfig(byte config); 

    /**
     * @brief get the temperature
     * 
     * reads temperature data from the barometer and runs the compensation routine 
     * 
     * @return float temperature in celcius 
     */
    float getTemperature(); 

    /**
     * @brief get the pressure 
     * 
     * reads pressure data from the barometer and runs the compensation routine. Reads both the temperature and 
     *  the pressure as data is required from the temperature to complete the pressure compensation
     * 
     * @return float pressure in pascals
     */
    float getPressure(); 

    /**
     * @brief get the pressure and temperature 
     * 
     * reads pressure and temperature data from the barometer and runs the compensation routines 
     * 
     * @param pressure pointer to store the pressure in, in Pascals 
     * @param temperature pointer to store the temperature in, in celcius
     * @return BMP280_status_t 
     */
    BMP280_status_t getPressureTemperature(float* pressure, float* temperature); 

private: 
    // private variables
    int _cs_pin; 
    SPISettings _spi_settings = SPISettings(BMP280_SPI_FREQ, BMP280_SPI_DATA_ORDER, BMP280_SPI_MODE); 
    BMP280_status_t _status; 
    signed int _t_fine; 

    // calibration data 
    struct BMP280CalibData{
        unsigned short dig_T1; 
        signed short dig_T2; 
        signed short dig_T3; 
        unsigned short dig_P1; 
        signed short dig_P2; 
        signed short dig_P3; 
        signed short dig_P4; 
        signed short dig_P5; 
        signed short dig_P6; 
        signed short dig_P7; 
        signed short dig_P8; 
        signed short dig_P9; 
    }; 
    BMP280CalibData _calib_data; ///< structure to store the calibration data in 

    // private functions 
    /**
     * @brief read the pressure data from the barometer 
     * 
     * reads the 3 bytes storing the applicable 20 bits of pressure data. Data is just adc data, must be compensated. 
     * 
     * @param pressure buffer of size 3 to store the pressure data in. Data is 20 bits long, should be shifted the last 4 bits 
     * @return BMP280_status_t 
     */
    BMP280_status_t readPressData(byte* pressure); 

    /**
     * @brief read the temperature data from the barometer 
     * 
     * reads the 3 bytes storing the applicable 20 bits of temperature data. Data is just adc data, must be compensated.
     * 
     * @param temp buffer of size 3 to store temperature data in. Data is 20 bits long, should be shifted by 4 bits 
     * @return BMP280_status_t 
     */
    BMP280_status_t readTempData(byte* temp); 

    /**
     * @brief read the pressure and temperature data 
     * 
     * Performs a single burst read to get both the pressure and temperature data. Reads 6 bytes in total. 
     * 
     * @param pressure buffer of size 3 to store the pressure data in. Data is 20 bits long, should be shifted the last 4 bits 
     * @param temp uffer of size 3 to store temperature data in. Data is 20 bits long, should be shifted by 4 bits 
     * @return BMP280_status_t 
     */
    BMP280_status_t readPressTempData(byte* pressure, byte* temp); 

    /**
     * @brief compensates the raw pressure data 
     * 
     * uses the recommended procedure to compensate the pressure data to convert adc data into actual measurements. 
     * requires the temperature compensation routine to be run first. 
     * 
     * @param pressure_data pressure adc data from the barometer 
     * @return float pressure in pascals 
     */
    float compensatePressure(signed int pressure_data); 

    /**
     * @brief compensates the raw temperature data 
     * 
     * uses the recommended procedure to compensate the temperature data to convert adc data into measurements. 
     * 
     * @param temperature_data temperature adc data from the barometer 
     * @return float temperature in celcius
     */
    float compensateTemperature(signed int temperature_data); 

    BMP280_status_t readCalibrationData(); 

    /**
     * @brief select the device on the SPI bus 
     * 
     * selects the barometer and starts the SPI transaction with correct chip settings 
     * 
     */
    void select(){
        digitalWrite(_cs_pin, LOW); 
        SPI.beginTransaction(_spi_settings); 
    }; 

    /**
     * @brief releases the device on the SPI bus 
     * 
     * releases the barometer and ends the SPI transaction 
     * 
     */
    void release(){
        digitalWrite(_cs_pin, HIGH); 
        SPI.endTransaction(); 
    }; 
}; 

#endif