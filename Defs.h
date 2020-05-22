#ifndef DEFS_H_
#define DEFS_H_

#include <stdlib.h>
#include <iostream>
#include <random>
#include <queue>
#include <algorithm>
#include <fstream>
#include <assert.h>
#include <math.h>
#include <string>
#include <unordered_map>

using std::cout;
using std::cin;
using std::endl;

using std::find;
using std::abs;
using std::to_string;
using std::min_element;

using std::unordered_map;
using std::queue;
using std::vector;
using std::string;
using std::ofstream;

using std::default_random_engine;

using std::poisson_distribution;
using std::geometric_distribution;
using std::bernoulli_distribution;

// Use queues?
#define QUEUE_SIM

// Record job completion times?
#define JOB_COMPLETION_TIME

// Record incast?
#define INCAST

// Record jobs distribution data?
//#define ANIMATION_DATA

#endif 


