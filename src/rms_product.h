#ifndef RMS_PRODUCT_H
#define RMS_PRODUCT_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <stdint.h>

#include "rms_scalar.h"
#include "rms_callback.h"

class RmsProduct{
public:
  RmsProduct();
  RmsProduct(RmsScalar*, RmsScalar*);

  static void callback_wrapper(void*, int);
  void callback_update(int);
  
  bool process();
  bool process(bool);
  void process_epoch();
  bool cycle_complete();
  
  int32_t apparent_product();
  int32_t real_product();
  int16_t product_factor();
  int16_t phase_angle();

  long period_us();
  int samples_per_cycle();

  // instantaneous product sample
  int32_t sample();

protected:
  RmsScalar* scalar_a;
  RmsScalar* scalar_b;

private:
  RmsCallback* callback;

  int64_t real_product_acc;
  int real_product_acc_count;
  int32_t real_product_value;
  int32_t sample_product_value;
  bool real_product_reset_flag;
};


#endif
