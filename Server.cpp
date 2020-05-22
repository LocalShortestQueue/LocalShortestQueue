#include "Server.h"

Server::~Server()
{
}

int Server::get_id()
{
	return id_;
}

int Server::get_size()
{

#ifdef QUEUE_SIM
	assert(jobs_queue_size_ == (int)jobs_queue_.size() && "queue size missmatch");
#endif

	return jobs_queue_size_;

}

void Server::add_jobs(int arrived_jobs, int arrival_time)
{

#ifdef QUEUE_SIM
	for (int i = 0; i < arrived_jobs; i++)
	{
		Job job{ arrival_time };
		jobs_queue_.push(job);
	}
#endif

	jobs_queue_size_ += arrived_jobs;

}

bool Server::complete_jobs(int departure_time)
{

	int offered_service = completed_jobs_(generator_);
	int num_completed = std::min(jobs_queue_size_, offered_service);

#ifdef QUEUE_SIM	
	for (int i = 0; i < num_completed; i++)
	{
		int jct = departure_time - jobs_queue_.front().get_time() + 1;

#ifdef JOB_COMPLETION_TIME
		if (jct_map_.find(jct) == jct_map_.end()) {
			// not found
			jct_map_[jct] = 1;
	}
		else {
			// found
			jct_map_[jct] += 1;
		}
#endif // JOB_COMPLETION_TIME
 
		job_delays_ += jct;
		jobs_queue_.pop();

	}
#endif

	jobs_queue_size_ -= num_completed;
	job_commulative_ += jobs_queue_size_;
	job_departures_ += num_completed;

	return true;

}

double Server::get_commulative_job_delays()
{
	return job_delays_;
}

double Server::get_commulative_size()
{
	return job_commulative_;
}

double Server::get_commulative_departures()
{
	return job_departures_;
}

#ifdef JOB_COMPLETION_TIME
unordered_map<int, int>* Server::get_jct_map()
{
	return &jct_map_;
}
#endif

bool JIQ_Server::complete_jobs(int departure_time)
{

	int offered_service = completed_jobs_(generator_);
	int num_completed = std::min(jobs_queue_size_, offered_service);

#ifdef QUEUE_SIM	
	for (int i = 0; i < num_completed; i++)
	{
		int jct = departure_time - jobs_queue_.front().get_time() + 1;

#ifdef JOB_COMPLETION_TIME
		if (jct_map_.find(jct) == jct_map_.end()) {
			// not found
			jct_map_[jct] = 1;
		}
		else {
			// found
			jct_map_[jct] += 1;
		}
#endif // JOB_COMPLETION_TIME

		job_delays_ += jct;
		jobs_queue_.pop();
	}
#endif

	jobs_queue_size_ -= num_completed;
	job_commulative_ += jobs_queue_size_;
	job_departures_ += num_completed;

	if (num_completed > 0 && jobs_queue_size_ == 0)
	{
		return true;
	}
	return false;

}

bool Smart_Server::complete_jobs(int departure_time)
{

	int offered_service = completed_jobs_(generator_);
	int num_completed = std::min(jobs_queue_size_, offered_service);

#ifdef QUEUE_SIM
	for (int i = 0; i < num_completed; i++)
	{
		int jct = departure_time - jobs_queue_.front().get_time() + 1;

#ifdef JOB_COMPLETION_TIME
		if (jct_map_.find(jct) == jct_map_.end()) {
			// not found
			jct_map_[jct] = 1;
		}
		else {
			// found
			jct_map_[jct] += 1;
		}
#endif // JOB_COMPLETION_TIME

		job_delays_ += jct;
		jobs_queue_.pop();
	}
#endif

	jobs_queue_size_ -= num_completed;
	job_commulative_ += jobs_queue_size_;
	job_departures_ += num_completed;
	
	// consider sending a message only if jobs were completed
	if (num_completed > 0)
	{
		return true;
	}
	return false;

}

double Smart_Server::get_expected_delay()
{
	return (double)jobs_queue_size_ / ((1.0 - mu_) / mu_);
}
