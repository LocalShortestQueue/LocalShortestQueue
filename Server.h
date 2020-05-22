#pragma once

#include "Defs.h"
#include "Job.h"

class Server
{

public:

	// constructor
	Server(int id, double mu) : id_(id), completed_jobs_(mu) 
	{ 

		job_delays_ = 0;
		job_commulative_ = 0;
		job_departures_= 0 ;

		jobs_queue_size_ = 0;

		mu_ = mu;

		generator_.seed(id);

	}

	// destructor
	~Server();

	// get server id
	int get_id();

	// get queue size
	int get_size();

	// add jobs to the queue
	void add_jobs(int arrived_jobs, int arrival_time);

	// complete jobs
	virtual bool complete_jobs(int departure_time);

	// get commulative job delays
	double get_commulative_job_delays();

	// get commulative system size
	double get_commulative_size();

	// get commulative departures
	double get_commulative_departures();

#ifdef JOB_COMPLETION_TIME
	// get the jct map
	unordered_map<int, int>* get_jct_map();
#endif

protected:

	// server id
	int id_;

	// number of jobs in queue
	int jobs_queue_size_;

	// server rate
	double mu_;

	// count commulative jobs delays
	double job_delays_;

	// count commulative system jobs
	double job_commulative_;

	// count commulative departures
	double job_departures_;

	// random numbers engine
	default_random_engine generator_;

	// generate the number of jobs completed at each ts.
	geometric_distribution<int> completed_jobs_;

#ifdef QUEUE_SIM
	// queue of jobs
	queue<Job> jobs_queue_;
#endif

#ifdef JOB_COMPLETION_TIME
	// map for holding job completion times
	unordered_map<int, int> jct_map_;
#endif

};

class JIQ_Server : public Server
{
public:
	JIQ_Server(int id, double mu) : Server(id, mu) {}

	// JIQ customized complete jobs
	bool complete_jobs(int departure_time) override;
};

class Smart_Server : public Server
{
public:
	Smart_Server(int id, double mu) : Server(id, mu) {}

	// LSQ customized complete jobs
	bool complete_jobs(int departure_time) override;

	// Get expected queue completion time
	double get_expected_delay();
};

