
// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include "rms_power.h"

RmsPower::RmsPower(RmsScalar* volts, RmsScalar* amps):
  RmsProduct(volts, amps){
}

float RmsPower::voltage(){
  return scalar_a->rms();
}

float RmsPower::amperage(){
  return scalar_b->rms();
}

float RmsPower::apparent_power(){
  return apparent_product();
}

float RmsPower::real_power(){
  return real_product();
}

float RmsPower::power_factor(){
  return product_factor();
}

