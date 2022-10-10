#ifndef RMS_SCALAR_H
#define RMS_SCALAR_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <stdint.h>
#include <math.h>

#include "EwmaT.h"
#include "rms_callback.h"

class Scale{
public:
  Scale();
  Scale(float, float);
  
  float to_eng(float);
  float from_eng(float);
  float slope();
  float offset();
  
private:
  float scale_slope;
  float scale_offset;
};

class Input{
public:
  Input(float=1.0, float=0.0);
  bool update(int);     // isr action
  void process();       // background processing
  int sample_value();
  float value();

  Scale input_scale;

protected:
  int _sample_value;
  int debug_reg;
};
  
class RmsScalar: public Input{
public:
  RmsScalar(float=1.0, float=0.0, const char* scalar_units="");

  void register_callback(RmsCallback*);

  bool update(int, unsigned long=0);
  bool process();
  //bool process(bool);
  bool cycle_complete();

  float rms();
  float peak();
  float period();
  float zero_crossing();
  int samples_per_cycle();

  const char* units;

protected:
private:
  RmsCallback* callback;
  
  long mean_squared();
  
  inline bool update_frequency(int, unsigned long);
  inline void update_peak(int, bool);
  inline void update_rms(int, bool);
  inline void update_end_of_cycle(int);
  void process_zero_crossing();

  EwmaT<int64_t>* rms_mean_filter;

  // working variables
  int freq_prev_val;
  int peak_max_val;
  int peak_min_val;

  int rms_sample_count;
  long long rms_acc;
  bool cycle_complete_armed;
  bool ac_cycle_complete;
  
  int ac_rms_sample_count;
  int ac_samples_per_cycle;
  long long ac_rms_acc;
  
  // finished product for post processing  
  unsigned long ac_cycle_start_us;
  unsigned long ac_cycle_stop_us;
  int ac_pos_peak;
  int ac_neg_peak;
  
  //int dbg_pin;
};

#endif
