#include "Arduino.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

#include <rmslib.h>
#include "nco.h"

// for a triangle wave, the expected Vpk/Vrms result is 0.577
// http://www.rfcafe.com/references/electrical/triangle-wave-voltage-conversion.htm
//TriangleWave wave{ TriangleWave{5, 1.0*1000} };


// for a sine wave the expected Vpk/Vrms result is 0.707
// http://www.rfcafe.com/references/electrical/sinewave-voltage-conversion.htm
const float v_nom = 120.0;
const float v_fs = 1.5 * v_nom * 1.414;

const float i_nom = 20.0;
const float i_fs = 1.5 * i_nom * 1.414;

const int adc_max_count = 4096-1;
SineWave wave{ SineWave{4, adc_max_count} };  

// for a square wave, the expected Vpk/Vrms result is 1.0
// http://www.rfcafe.com/references/electrical/square-wave-voltage-conversion.htm
//SquareWave wave{ SquareWave{4, 1.0*1000} };

bool ref_soc;

//Scale v_scale{ Scale{v_fs/adc_max_count,0} };
RmsScalar v_ref{ RmsScalar{v_fs/adc_max_count, 0.0} };

//Scale i_scale{ Scale{i_fs/adc_max_count,0} };
RmsScalar amps{ RmsScalar{i_fs/adc_max_count,0.0} };

RmsProduct zone{ RmsProduct{&v_ref, &amps} };
float phase_degrees = 0.0;
float phase_radians = phase_degrees/360.0 * 2 * M_PI;

void setup() {
  pinMode(PIN_PA7, OUTPUT);
  
  Serial.swap(1);
  Serial.begin(115200, SERIAL_8N1);
  
  return;
}

void loop() {
  bool soc = wave.update();
  int v_raw = 120.0*1.414/v_fs * wave.value(0);  
  int i_raw = 2.0*1.414/i_fs * wave.value(phase_radians);
  
  ref_soc = v_ref.update(v_raw);
  v_ref.process();

  amps.update(i_raw);
  zone.process(ref_soc);

  Serial.print(zone.raw_voltage());
  Serial.print(", ");
  Serial.print(zone.raw_amperage());
  Serial.print(", ");
  Serial.print(zone.raw_product());

  if (soc){
    Serial.print(" : ");
    Serial.print(phase_degrees);
    Serial.print("*, ");
    Serial.print(zone.voltage());
    Serial.print("v, ");
    Serial.print(zone.amperage());
    Serial.print("a, ");
    Serial.print(zone.real_power());
    Serial.print("pr, ");
    Serial.print(zone.apparent_power());
    Serial.print("pa, ");
    Serial.print(zone.power_factor());
  }
  Serial.print("\n");
  
  delay(10);
  
}
