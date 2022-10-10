#include "rms_callback.h"

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

// http://www.newty.de/fpt/callback.html#member
// https://stackoverflow.com/questions/2298242/callback-functions-in-c
//https://forum.arduino.cc/t/callback-function/472196/7

RmsCallback::RmsCallback(void* instance, function_ptr function){
  _instance = instance;
  _function = function;
}

void RmsCallback::update(int x){
  if (_instance){
    _function(_instance, x);
  }

  return;
}

