#ifndef RMS_SCALAR_H
#define RMS_SCALAR_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <stdint.h>
#include <math.h>

#include "rms_callback.h"
 
class RmsScalar{
public:
  RmsScalar();
  
  void register_callback(RmsCallback*);

  bool update(int, unsigned long=0);
  bool process();

  int sample();
  int rms();
  int peak();
  long period();
  long zero_crossing();
  bool cycle_complete();
  
  int samples_per_cycle();

protected:
private:
  RmsCallback* callback;
  
  inline bool update_frequency(int, unsigned long);
  inline void update_peak(int, bool);
  inline void update_rms(int, bool);
  inline void update_end_of_cycle(int);
  void process_zero_crossing();

  // working variables
  int sample_value;
  int peak_max_val;
  int peak_min_val;

  // active accumulator
  int mean_squared_acc_count;
  long long mean_squared_acc;

  // holding registers for post processing
  int ac_mean_squared_acc_count;
  long long ac_mean_squared_acc;
  long long ac_mean_squared;

  // cycle complete detection
  int freq_prev_val;
  bool cycle_complete_armed;

  // state variables
  bool ac_cycle_complete;
  int ac_samples_per_cycle;
  
  // finished product ready for client 
  unsigned long ac_cycle_start_us;
  unsigned long ac_cycle_stop_us;
  int ac_pos_peak;
  int ac_neg_peak;
  
};

#endif
