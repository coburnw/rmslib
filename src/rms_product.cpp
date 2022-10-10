#include "rms_product.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

RmsProduct::RmsProduct(RmsScalar* vac_input, RmsScalar* iac_input){
  v_in = vac_input;
  i_in = iac_input;

  real_power_slope = v_in->input_scale.slope() * i_in->input_scale.slope();
  
  real_power_acc = 0;
  real_power_acc_count = 0;
  real_power_filter = new EwmaT<int64_t>(3, 10);

  full_cycle_sample_count = 0;

  callback = new RmsCallback(this, callback_wrapper);
  i_in->register_callback(callback);
  return;
}

// explore move wrapper into callback class as a template function?
void RmsProduct::callback_wrapper(void* instance, int raw_val){
  RmsProduct* self = (RmsProduct*) instance;
  self->callback_update(raw_val);
  return;
}

void RmsProduct::callback_update(int raw_value){
  if (real_power_reset_flag){
    real_power_reset_flag = false;
    real_power_acc = 0.0;
    real_power_acc_count = 0;
  }
  raw_power_product = (int32_t)v_in->sample_value() * i_in->sample_value();
  real_power_acc += raw_power_product;
  ++real_power_acc_count;
  
  return;
}

bool RmsProduct::process(){
  bool is_epoch = v_in->process();
  process(is_epoch);
  
  return is_epoch;
}

bool RmsProduct::process(bool is_epoch){
  i_in->process();
  if (is_epoch){
    process_epoch();
  }
  
  return is_epoch;
}

void RmsProduct::process_epoch(){
  int64_t real_mean = real_power_acc / real_power_acc_count;
  real_power_acc_debug = real_mean;
  real_power_filter->filter(real_mean);  // rename filter to real mean?
  
  // use to debug disparity with samples_per_cycle()
  full_cycle_sample_count = real_power_acc_count;

  real_power_reset_flag = true;
  
  return;
}

bool RmsProduct::cycle_complete(){
  return v_in->cycle_complete();
}

int RmsProduct::raw_voltage(){
  return v_in->sample_value();
}

int RmsProduct::raw_amperage(){
  return i_in->sample_value();
}

int32_t RmsProduct::raw_product(){
  return raw_power_product;
}

float RmsProduct::voltage(){
  return v_in->rms();
}

float RmsProduct::amperage(){
  return i_in->rms();
}

float RmsProduct::apparent_power(){
  return v_in->rms() * i_in->rms();
}

float RmsProduct::real_power(){
  return real_power_filter->output() * real_power_slope;
}

float RmsProduct::frequency(){
  return 1 / v_in->period();
}

float RmsProduct::power_factor(){
  return real_power() / apparent_power();
}

float RmsProduct::phase_angle(){
  return 0.0;
}

// number of a/d samples per cycle
int RmsProduct::samples_per_cycle(){
  return i_in->samples_per_cycle();
}
