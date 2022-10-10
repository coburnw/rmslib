#include "nco.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

PhaseAccum::PhaseAccum(int numb_of_bits){
  n = numb_of_bits;
  max_count = pow(2, n) - 1;
  phase_acc = 0;
}

bool PhaseAccum::update(){
  bool start_of_cycle = false;
  if (++phase_acc > max_count){
    phase_acc = 0;
    start_of_cycle = true;
  }

  return start_of_cycle;
}

float PhaseAccum::value(float offset_in_radians){
  int offset = offset_in_radians / (2 * M_PI) * (max_count+1);

  int phase = (phase_acc + offset) & max_count;
  float radians = float(phase)/(max_count+1) * 2 * M_PI;
  
  return radians;
}

Wave::Wave(int numb_of_bits, float amp_gain=1) : pa(numb_of_bits){
  gain = amp_gain;
  return;
}

bool Wave::update(){
  bool start_of_cycle = pa.update();
  return start_of_cycle;
}

float Wave::value(float offset_in_radians){
  return gain * pa.value(offset_in_radians);
}

// ###################
SineWave::SineWave(int numb_of_bits, float amp_gain=1) : Wave(numb_of_bits, amp_gain){
  return;
}

float SineWave::value(float offset_in_radians){
  return gain * sin(pa.value(offset_in_radians));
}

// ###################
SquareWave::SquareWave(int numb_of_bits, float amp_gain=1) : Wave(numb_of_bits, amp_gain){
  return;
}

float SquareWave::value(float offset_in_radians){
  float amplitude = 1.0 * gain;
  if (pa.value(offset_in_radians) < M_PI)
    amplitude *= -1.0;
  return amplitude;
}

// ###################
// http://www.rfcafe.com/references/electrical/triangle-wave-voltage-conversion.htm
TriangleWave::TriangleWave(int numb_of_bits, float amp_gain=1) : Wave(numb_of_bits, amp_gain){
  return;
}

float TriangleWave::value(float offset_in_radians){
  // https://stackoverflow.com/a/19374586
  //(2 * amplitudeConstant / Math.PI) * Math.Asin(Math.Sin(2 * (Math.PI / periodConstant) * Convert.ToDouble(variableX)))
  float amplitude = (2 * gain / M_PI) * asin(sin(2 * (M_PI / (2*M_PI)) * pa.value(offset_in_radians)));
  return amplitude;

}
