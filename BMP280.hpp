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
} BMP280_status_t; 


class BMP280{
public: 
    BMP280_status_t init(int cs_pin); 

    // chip registers functions 
    
    BMP280_status_t readId(byte* id); 

    BMP280_status_t reset(); 

    BMP280_status_t readStatus(byte* status); 

    BMP280_status_t writeCtrlMeas(byte ctrl_meas); 

    BMP280_status_t writeConfig(byte config); 

    BMP280_status_t readPressData(byte* pressure); 

    BMP280_status_t readTempData(byte* temp); 

private: 
    // private variables
    int _cs_pin; 
    SPISettings _spi_settings = SPISettings(BMP280_SPI_FREQ, BMP280_SPI_DATA_ORDER, BMP280_SPI_MODE); 
    BMP280_status_t _status; 

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
    BMP280CalibData _calib_data; 


    


    // private functions 

    BMP280_status_t readCalibrationData(); 

    void select(){
        digitalWrite(_cs_pin, LOW); 
        SPI.beginTransaction(_spi_settings); 
    }; 

    void release(){
        digitalWrite(_cs_pin, HIGH); 
        SPI.endTransaction(); 
    }; 


}; 

#endif