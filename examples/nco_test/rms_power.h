#ifndef RMS_POWER_H
#define RMS_POWER_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <rmslib.h>

class RmsPower: public RmsProduct{
public:
  RmsPower(RmsScalar*, RmsScalar*);
  
  float voltage();
  float amperage();
  float apparent_power();
  float real_power();
  float power_factor();
};

#endif
