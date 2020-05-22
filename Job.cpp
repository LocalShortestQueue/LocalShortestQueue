#include "Job.h"

Job::Job(int time)
{
	creation_time_ = time;
}

Job::~Job()
{
}

int Job::get_time()
{
	return creation_time_;
}
