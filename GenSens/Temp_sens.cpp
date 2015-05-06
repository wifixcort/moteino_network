#include "Temp_sens.h"


 

Temp_sens::Temp_sens(const char *sensor_type, uint8_t id_node, uint8_t frequency, const char *encryptK, uint8_t netw_id, boolean mote_type):GenSens(id_node, frequency, encryptK, netw_id, mote_type){
  sensor_type = sensor_type;
}//end constructor

const char *Temp_sens::get_sensor_type(){
  return sensor_type;
}//end get_sensor_type

