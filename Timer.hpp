#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
using namespace std::chrono;
class Timer{
private:
  time_point<high_resolution_clock> start;

public:
  Timer(){
    update();
  }

  void update(){
    start=high_resolution_clock::now();
  }

  long long  getElapsedTimeInMicroSec(){
    return duration_cast<microseconds>(high_resolution_clock::now()-start).count();
  }

  double getElapsedTimeInMilliSec(){
    return getElapsedTimeInMicroSec()*0.001;
  }

  double getElapsedTimeInSec(){
    return getElapsedTimeInMicroSec()*0.000001;
  }

};

#endif
