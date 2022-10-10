#ifndef NCO_H
#define NCO_H

#include "Arduino.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

class PhaseAccum{
 public:
  PhaseAccum(int);
  bool update();
  float value(float);
  
 private:
  int n;
  int max_count;
  int phase_acc;
};


class Wave{
 public:
  Wave(int, float);
  bool update();
  float value(float);
  
 protected:
  PhaseAccum pa;
  float gain;
};

class SineWave: public Wave{
 public:
  SineWave(int, float);
  float value(float);
};

class SquareWave: public Wave{
 public:
  SquareWave(int, float);
  float value(float);
};

class TriangleWave: public Wave{
 public:
  TriangleWave(int, float);
  float value(float);
};

#endif
