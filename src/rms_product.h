#ifndef RMS_PRODUCT_H
#define RMS_PRODUCT_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <stdint.h>

#include "EwmaT.h"
#include "rms_scalar.h"
#include "rms_callback.h"

class RmsProduct{
public:
  RmsProduct();
  RmsProduct(RmsScalar*, RmsScalar*);

  static void callback_wrapper(void*, int);
  void callback_update(int);
  
  //void update(int);
  bool process();
  bool process(bool);
  void process_epoch();
  
  bool cycle_complete();
  float voltage();
  float amperage();
  float apparent_power();
  float real_power();
  float frequency();
  float power_factor();
  float phase_angle();
  int samples_per_cycle();

  int raw_voltage();
  int raw_amperage();
  int32_t raw_product();
  
private:
  RmsScalar* v_in;
  RmsScalar* i_in;
  RmsCallback* callback;

  // https://github.com/jonnieZG/EWMA
  EwmaT <int64_t>* real_power_filter;
  float real_power_slope;

  int64_t real_power_acc;
  int real_power_acc_count;
  bool real_power_reset_flag;

  // debug register
  int full_cycle_sample_count;
  int32_t raw_power_product;
  int64_t real_power_acc_debug; //debug snapshot
};

#endif
