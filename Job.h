#pragma once
#include "Defs.h"

class Job
{

public:

	// constructor
	Job(int time);

	// destructor
	~Job();

	// get job creation time (the time it entered the system)
	int get_time();

private:

	// job creation time
	int creation_time_;

};

