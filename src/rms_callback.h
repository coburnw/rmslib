#ifndef RMS_CALLBACK_H
#define RMS_CALLBACK_H

// GNU Affero General Public License v3.0
// Copyright (c) 2022 Coburn Wightman

class RmsCallback{
 public:
  typedef void (*function_ptr)(void*, int);

  RmsCallback(void*, function_ptr);
  void update(int x);

 private:
  function_ptr _function = nullptr;
  void* _instance = nullptr;
};

#endif
