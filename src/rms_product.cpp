#include "rms_product.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

RmsProduct::RmsProduct(RmsScalar* a, RmsScalar* b){
  scalar_a = a;
  scalar_b = b;

  real_product_acc = 0;
  real_product_acc_count = 0;
  real_product_value = 0;
  sample_product_value = 0;
  real_product_reset_flag = false;
  
  callback = new RmsCallback(this, callback_wrapper);
  scalar_b->register_callback(callback);
  return;
}

// static function to route callback to our instance function
void RmsProduct::callback_wrapper(void* instance, int raw_val){
  RmsProduct* self = (RmsProduct*) instance;
  self->callback_update(raw_val);
  return;
}

void RmsProduct::callback_update(int raw_value){
  if (real_product_reset_flag){
    real_product_reset_flag = false;
    real_product_acc = 0;
    real_product_acc_count = 0;
  }

  sample_product_value = (int32_t)scalar_a->sample() * scalar_b->sample();
  real_product_acc += sample_product_value;
  ++real_product_acc_count;
  
  return;
}

bool RmsProduct::process(){
  bool is_epoch = scalar_a->process();
  process(is_epoch);
  
  return is_epoch;
}

bool RmsProduct::process(bool is_epoch){
  scalar_b->process();
  if (is_epoch){
    process_epoch();
  }
  
  return is_epoch;
}

void RmsProduct::process_epoch(){
  real_product_value = real_product_acc / real_product_acc_count;  
  real_product_reset_flag = true;
  
  return;
}

bool RmsProduct::cycle_complete(){
  return scalar_a->cycle_complete();
}

int32_t RmsProduct::sample(){
  return sample_product_value;  // sample_product snapshot
}

int32_t RmsProduct::apparent_product(){
  return scalar_a->rms() * scalar_b->rms();
}

int32_t RmsProduct::real_product(){
  return real_product_value;
}

int16_t RmsProduct::product_factor(){
  return real_product() / apparent_product();
}

int16_t RmsProduct::phase_angle(){
  return 0;
}

long RmsProduct::period_us(){
  return scalar_a->period();
}

// number of a/d samples per cycle
int RmsProduct::samples_per_cycle(){
  return scalar_b->samples_per_cycle();
}

