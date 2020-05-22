#pragma once

#include "Defs.h"
#include "MinHeap.h"

class Dispatcher
{

public:

	Dispatcher(int id, int num_servers, double load) : poisson_distribution_(load)
	{ 
		// dispatcher ID
		id_ = id;

		// number of servers
		num_servers_ = num_servers;

		// seed the random generator with ID
		main_generator_.seed(id);
	}

	~Dispatcher() 
	{
	}

	// arrivals at the dispatcher
	int get_arrivals();

	// random by default. Overridden by derived classes.
	virtual int get_destination();

protected:

	// dispatcher ID
	int id_;

	// number of servers
	int num_servers_;

	// Random numbers engine
	default_random_engine main_generator_;

	// Poisson arrivals
	poisson_distribution<int> poisson_distribution_;

};

class JSQ_Dispatcher : public Dispatcher
{

public:

	JSQ_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) 
	{ 
		servers_heap_ = new MinHeap(num_servers); 
	}

	~JSQ_Dispatcher() 
	{ 
		delete servers_heap_; 
	}

	int get_destination() override;

	void update_server(int i, int val);

private:

	MinHeap* servers_heap_;

};

class POC_Dispatcher : public Dispatcher
{

public:

	POC_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) {}
	
	int get_destination(vector<int> &q, vector<int> &v);

};

class JIQ_Dispatcher : public Dispatcher
{
private:
	vector<int> idle_queues_;
public:
	JIQ_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) {}
	int get_destination() override;
	void update_idle_queue(int i);
};

class LSQ_Dispatcher : public Dispatcher
{

private:

	MinHeap* local_servers_heap_;

public:

	LSQ_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load)
	{ 
		local_servers_heap_ = new MinHeap(num_servers);
	}

	~LSQ_Dispatcher()
	{
		delete local_servers_heap_;
	}

	int get_destination();

	void update_local_queues(vector<int> &q, vector<int> &v);

	int get_local_queue(int i);

};

class WR_Dispatcher : public Dispatcher
{

public:

	WR_Dispatcher(int id, int num_servers, double load, double* sr) : Dispatcher(id, num_servers, load)
	{
		server_rates_ = new double[num_servers];
		server_rates_sum_ = 0;
		for (int i = 0; i < num_servers; i++)
		{
			server_rates_[i] = ((1.0 - sr[i]) / sr[i]);
			server_rates_sum_ += server_rates_[i];
		}
	}

	~WR_Dispatcher()
	{
		delete[] server_rates_;
	}

	int get_destination() override;

private:

	double* server_rates_;
	double server_rates_sum_;

};

class RR_Dispatcher : public Dispatcher
{

public:

	RR_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load)
	{
		curr_ = rand() % num_servers;
	}

	int get_destination() override;

private:

	int curr_;

};

class WRR_Dispatcher : public Dispatcher
{

public:

	WRR_Dispatcher(int id, int num_servers, double load, double* sr) : Dispatcher(id, num_servers, load)
	{

		resolution_factor_ = 1000.0;
		resolution_distance_ = 10;

		server_rates_ = new int[num_servers];

		for (int i = 0; i < num_servers; i++)
		{
			server_rates_[i] = (int)(resolution_factor_*(1.0 - sr[i]) / sr[i]);
		}

		curr_ = rand() % num_servers;
		curr_sum_ = 0;

		step_ = *min_element(server_rates_, server_rates_ + num_servers);

	}

	~WRR_Dispatcher()
	{
		delete[] server_rates_;
	}

	int get_destination() override;

private:

	int gcd_(int a, int b);
	int array_gcd_(int arr[], int n);

	int* server_rates_;

	int curr_;
	int curr_sum_;

	int step_;

	double resolution_factor_;
	int resolution_distance_;

};

class POM_Dispatcher : public Dispatcher
{

public:

	POM_Dispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) 
	{
		m_shortest_ = rand() % num_servers;
	}

	int get_destination(vector<int> &q, vector<int> &v);

	int get_previous_shortest();

private:

	int m_shortest_;

};

