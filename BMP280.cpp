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
    SPI.transfer(BMP280_PRESS_MSB|1<<7); 
    *pressure = SPI.transfer(0); 
    *(pressure + 1) = SPI.transfer(0); 
    *(pressure + 2) = SPI.transfer(0); 
    release(); 
    return BMP280_OK; // TODO implement check update thing 
}

BMP280_status_t BMP280::readTempData(byte* temp){
    // read the temperature data 
    // does not run the compensation routine
    select(); 
    SPI.transfer(BMP280_TEMP_MSB|1<<7); 
    *temp = SPI.transfer(0); 
    *(temp + 1) = SPI.transfer(0); 
    *(temp + 2) = SPI.transfer(0); 
    release(); 
    return BMP280_OK; 
}

BMP280_status_t BMP280::readPressTempData(byte* pressure, byte* temp){
    // perform a burst read of the temperature & pressure data
    select(); 
    SPI.transfer(BMP280_PRESS_MSB|1<<7); 
    *pressure = SPI.transfer(0); 
    *(pressure + 1) = SPI.transfer(0); 
    *(pressure + 2) = SPI.transfer(0); 
    *temp = SPI.transfer(0); 
    *(temp + 1) = SPI.transfer(0); 
    *(temp + 2) = SPI.transfer(0); 
    release(); 
    return BMP280_OK; 
}

float BMP280::compensatePressure(signed int pressure_data){ 
    // compensate the pressure adc measurement 
    // uses Bosch's method described in the datasheet
    // requires the temperature compensation to be run prior
    // TODO verify signed long is a 64 bit value 
    signed long var1, var2, p; 
    var1 = ((signed long)_t_fine) - 128000; 
    var2 = var1 * var1 * (signed long)_calib_data.dig_P6; 
    var2 = var2 + ((var1*(signed long)_calib_data.dig_P5)<<17); 
    var2 = var2 + ((signed long)_calib_data.dig_P4<<35);
    var1 = ((var1 * var1 * (signed long)_calib_data.dig_P3)>>8) + ((var1 * (signed long)_calib_data.dig_P2)<<12); 
    var1 = (((((signed long)1)<<47)+var1))*((signed long)_calib_data.dig_P1)>>33; 
    if(var1 == 0){
        return 0; // div by 0 catch 
    }
    p = 1048576 - pressure_data; 
    p = (((p<<31)-var2)*3125)/var1; 
    var1 = (((signed long)_calib_data.dig_P9) * (p>>13) * (p>>13)) >> 25; 
    var2 = (((signed long)_calib_data.dig_P8) *p) >> 19; 
    p = ((p + var1 + var2) >> 8) + (((signed long)_calib_data.dig_P7)<<4); 
    return (float)(p/256); 
}

float BMP280::compensateTemperature(signed int temperature_data){
    // compensate the temperature adc measurement 
    // uses Bosch's method described in the datasheet 
    // TODO verify data type sizes 
    signed int var1, var2, T; 
    var1 = ((((temperature_data>>3) - ((signed int)_calib_data.dig_T1<<1))) * ((signed int)_calib_data.dig_T2)) >> 11; 
    var2 = (((((temperature_data>>4) - ((signed int)_calib_data.dig_T1)) * ((temperature_data>>4) - ((signed int)_calib_data.dig_T1))) >> 12) * ((signed int)_calib_data.dig_T3)) >> 14; 
    _t_fine = var1 + var2; 
    T = (_t_fine * 5 + 128) >> 8; 
    return (float)(T/100.0); 
}

float BMP280::getTemperature(){
    // get the adc data then compensate it 
    byte adc_temp[3]; 
    _status = readTempData(adc_temp); 
    // convert to a signed int ? TODO verify 
    signed int temp_data = adc_temp[0] << 12 | adc_temp[1] << 4 | adc_temp[2] >> 4; 
    return compensateTemperature(temp_data); 
}

float BMP280::getPressure(){
    // because the pressure compensation depends on temperature data, both pressure and data need 
    //  to be read. 
    float press, temp; 
    getPressureTemperature(&press, &temp); 
    return press; 
}

BMP280_status_t BMP280::getPressureTemperature(float* pressure, float* temperature){
    // read all the data 
    // this is done so a continual burst read of all available data is performed 
    byte temp_adc[3]; 
    byte press_adc[3]; 
    _status = readPressTempData(press_adc, temp_adc); 
    // combine into signed data 
    signed int temp_data = temp_adc[0] << 12 | temp_adc[1] << 4 | temp_adc[2] >> 4;
    signed int press_data = press_adc[0] << 12 | press_adc[1] << 4 | press_adc[2] >> 4;
    *temperature = compensateTemperature(temp_data); 
    *pressure = compensatePressure(press_data); 
    return _status; 
}

BMP280_status_t BMP280::readCalibrationData(){ 
    // read in the calibration data 
    int *ptr = (int*)&_calib_data; 
    // struct is set up to do a straight read 
    select(); 
    // start reading at the starting address of the calib data 
    SPI.transfer(BMP280_CALIB_25|1<<7); 
    for(unsigned int i = 0; i < sizeof(_calib_data); i ++){
        *(ptr + i) = SPI.transfer(0); 
    }
    release(); 
    return BMP280_OK;
}