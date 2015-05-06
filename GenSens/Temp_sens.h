#ifndef __TEMP_SENS__
#define __TEMP_SENS__

#include <Arduino.h>

#include "GenSens.h"

class Temp_sens: public GenSens{
 private:
  const char *sensor_type;
 public:
  Temp_sens(){};
  ~Temp_sens();
  Temp_sens(const char *sensor_type, uint8_t id_node, uint8_t frequency, const char *encryptK, uint8_t netw_id, boolean mote_type);
  const char *get_sensor_type();

};

#endif
