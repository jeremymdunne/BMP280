/**
 * @file BMP280.cpp
 * @author Jeremy Dunne 
 * @brief Implementation for the BMP280 interface library 
 * @version 0.1
 * @date 2022-12-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "BMP280.hpp"

BMP280_status_t BMP280::init(int cs_pin){
    // init 
    _cs_pin = cs_pin; 
    pinMode(_cs_pin, OUTPUT); 
    digitalWrite(_cs_pin, HIGH); 
    // check id 
    byte id; 
    _status = readId(&id); 
    if(_status != BMP280_OK){
        return _status; 
    }
    if(id != BMP280_ID_RESP){
        //Serial.println(id); 
        //Serial.println(BMP280_ID_RESP);
        return BMP280_UNKNOWN_ID; 
    }
    return BMP280_OK; 
}

BMP280_status_t BMP280::readId(byte* id){
    // read out the id 
    // read commands need a 1 in the RW bit 
    select(); 
    SPI.transfer(BMP280_ID|1<<7);  
    *id = SPI.transfer(0); 
    release(); 
    return BMP280_OK; 
}

BMP280_status_t BMP280::reset(){
    // reset 
    select(); 
    SPI.transfer(BMP280_RESET);
    SPI.transfer(BMP280_RESET_COMMAND);
    release(); 
    return BMP280_OK; 
}

BMP280_status_t BMP280::readStatus(byte* status){
    // read out the status register 
    select(); 
    SPI.transfer(BMP280_STATUS|1<<7); 
    *status = SPI.transfer(0); 
    release(); 
    return BMP280_OK; 
}

BMP280_status_t BMP280::writeCtrlMeas(byte ctrl_meas){
    // write the ctrl meas register 
    select(); 
    SPI.transfer(BMP280_CTRL_MEAS); 
    SPI.transfer(ctrl_meas); 
    release(); 
    return BMP280_OK; 
}

BMP280_status_t BMP280::writeConfig(byte config){
    select(); 
    SPI.transfer(BMP280_CONFIG);
    SPI.transfer(config); 
    release();
    return BMP280_OK; 
}

BMP280_status_t BMP280::readPressData(byte* pressure){
    // read the pressure data 
    // does not run the compensation routine 
    select(); 
    SPI.transfer(BMP280_PRESS_MSB); 
    *pressure = SPI.transfer(0); 
    *(pressure + 1) = SPI.transfer(0); 
    *(pressure + 2) = SPI.transfer(0); 
    release(); 
    return BMP280_OK; // TODO implement check update thing 
}