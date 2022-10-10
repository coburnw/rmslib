#include "rms_scalar.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

Scale::Scale(){
  scale_slope = 1.0;
  scale_offset = 0.0;
}

Scale::Scale(float slope, float offset){
  scale_slope = slope;
  scale_offset = offset;
}

float Scale::slope(){
  return scale_slope;
}

float Scale::offset(){
  return scale_offset;
}

float Scale::to_eng(float value){
  float val = value * scale_slope + scale_offset;
  return val;
}

float Scale::from_eng(float value){
  return (value - scale_offset) / scale_slope;
}

// **** Input ****
Input::Input(float slope, float offset)
{
  input_scale = Scale(slope, offset);
}

bool Input::update(int sample){
  _sample_value = sample;
  return false;
}

int Input::sample_value(){
  return _sample_value;
}

float Input::value(){
  return input_scale.to_eng(sample_value());  
}

// **** RmsScalar ****
RmsScalar::RmsScalar(float slope, float offset, const char* scalar_units):
  Input(slope, offset)
{
  units = scalar_units;

  callback = nullptr;
  
  freq_prev_val = 0;
  peak_max_val = 0;
  peak_min_val = 0;
  rms_acc = 0;
  rms_sample_count = 0;

  rms_mean_filter = new EwmaT<int64_t>{3, 10};
  
  ac_cycle_start_us = 0;
  ac_cycle_stop_us = 0;
  ac_samples_per_cycle = 0;

  ac_cycle_complete = false;
  cycle_complete_armed = false;
}

void RmsScalar::register_callback(RmsCallback* cb){
  callback = cb;
}

// loop processing
bool RmsScalar::process(){
  bool is_zero_crossing = ac_cycle_complete;
  if (ac_cycle_complete){
    process_zero_crossing();
  }

  return is_zero_crossing;
}

void RmsScalar::process_zero_crossing(){
  long rms_mean = ac_rms_acc / ac_rms_sample_count;
  rms_mean_filter->filter(rms_mean);

  ac_samples_per_cycle = ac_rms_sample_count;
  ac_cycle_complete = false;
  
  return;
}

bool RmsScalar::cycle_complete(){
  return ac_cycle_complete;
}

// isr processing
bool RmsScalar::update(int sample, unsigned long timestamp_us){
  Input::update(sample);
    
  bool cycle_complete = update_frequency(sample_value(), timestamp_us);
  update_peak(sample_value(), cycle_complete);
  update_rms(sample_value(), cycle_complete);

  if (cycle_complete){
    ac_cycle_complete = true;
  }

  if (callback){
    callback->update(sample_value());
  }
  
  return cycle_complete;
}

bool RmsScalar::update_frequency(int value, unsigned long timestamp_us){
  bool cycle_complete = false;
  
  if ((freq_prev_val < 0) && (value >= 0)) {
    cycle_complete_armed = true;
  }
  else if ((cycle_complete_armed) && (value >= 0)) { 
    ac_cycle_start_us = ac_cycle_stop_us;
    ac_cycle_stop_us = timestamp_us;

    cycle_complete_armed = false;
    cycle_complete = true;
  }
  else{
    cycle_complete_armed = false;
  }
  
  freq_prev_val = value;

  return cycle_complete;
}

void RmsScalar::update_peak(int value, bool cycle_complete){
  if (cycle_complete){
    ac_pos_peak = peak_max_val;
    ac_neg_peak = peak_min_val;
    peak_max_val = 0;
    peak_min_val = 0;
  }
  
  if (value > peak_max_val){
    peak_max_val = value;
  }
  else if (value < peak_min_val){
    peak_min_val = value;
  }
  return;
}

void RmsScalar::update_rms(int value, bool cycle_complete){
  if (cycle_complete){
    ac_rms_acc = rms_acc;
    ac_rms_sample_count = rms_sample_count;
    ac_samples_per_cycle = rms_sample_count;

    rms_acc = 0;
    rms_sample_count = 0;
  }

  rms_acc += long(value) * value;
  ++rms_sample_count;

  return;
}

long RmsScalar::mean_squared(){
  long ms = rms_mean_filter->output();
  return ms;
}

float RmsScalar::rms(){
  float rms_raw = sqrt(mean_squared());
  return input_scale.to_eng(rms_raw);
}

float RmsScalar::peak(){
  int pp = ac_pos_peak - ac_neg_peak;
  return input_scale.to_eng(pp/2);
}

// return start of cycle in microseconds
float RmsScalar::zero_crossing(){
  return ac_cycle_start_us;
}

// return period in seconds
float RmsScalar::period(){
  return (ac_cycle_stop_us - ac_cycle_start_us)/1e6;
}

int RmsScalar::samples_per_cycle(void){
  return ac_samples_per_cycle;
}

