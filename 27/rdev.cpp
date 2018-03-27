#include <unistd.h>
#include <random>
#include <functional>
#include <chrono>
#include <thread>
#include "rdev.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

class backoff_time_t {
public:
  random_device                      rd;
  mt19937                            mt;
  uniform_real_distribution<double>  dist;
  backoff_time_t() : rd{}, mt{rd()}, dist{0.0, 1.0} {}

  double rand() {
    return dist(mt);
  }
};

thread_local backoff_time_t backoff_time;

int RangedRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 zGenerator(rd());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(zGenerator); 
} 