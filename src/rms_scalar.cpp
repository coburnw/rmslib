// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include "rms_scalar.h"

RmsScalar::RmsScalar()
{
  callback = nullptr;
  
  freq_prev_val = 0;
  peak_max_val = 0;
  peak_min_val = 0;
  mean_squared_acc = 0;
  mean_squared_acc_count = 0;

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
  ac_mean_squared = ac_mean_squared_acc / ac_mean_squared_acc_count;

  ac_samples_per_cycle = ac_mean_squared_acc_count;
  ac_cycle_complete = false;
  
  return;
}

bool RmsScalar::cycle_complete(){
  return ac_cycle_complete;
}

// isr processing
bool RmsScalar::update(int sample, unsigned long timestamp_us){
  sample_value = sample;
    
  bool cycle_complete = update_frequency(sample_value, timestamp_us);
  update_peak(sample_value, cycle_complete);
  update_rms(sample_value, cycle_complete);

  if (cycle_complete){
    ac_cycle_complete = true;
  }

  if (callback){
    callback->update(sample_value);
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
    ac_mean_squared_acc = mean_squared_acc;
    ac_mean_squared_acc_count = mean_squared_acc_count;
    ac_samples_per_cycle = mean_squared_acc_count;

    mean_squared_acc = 0;
    mean_squared_acc_count = 0;
  }

  mean_squared_acc += long(value) * value;
  ++mean_squared_acc_count;

  return;
}

int RmsScalar::sample(){
  return sample_value;
}

int RmsScalar::rms(){
  return sqrt(ac_mean_squared);
}

int RmsScalar::peak(){
  int pp = ac_pos_peak - ac_neg_peak;
  return (pp / 2);
}

// return start of cycle in microseconds
long RmsScalar::zero_crossing(){
  return ac_cycle_start_us;
}

// return period in microseconds
long RmsScalar::period(){
  return (ac_cycle_stop_us - ac_cycle_start_us);
}

int RmsScalar::samples_per_cycle(void){
  return ac_samples_per_cycle;
}

