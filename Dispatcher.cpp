#include "Dispatcher.h"

int Dispatcher::get_destination() 
{
	return rand() % num_servers_;
}

int Dispatcher::get_arrivals()
{
	return poisson_distribution_(main_generator_);
}

int JSQ_Dispatcher::get_destination()
{
	return servers_heap_->GetMin();
}

void JSQ_Dispatcher::update_server(int i, int val)
{
	servers_heap_->UpdateKey(i, val);
}

int POC_Dispatcher::get_destination(vector<int> &q, vector<int> &v)
{

	// Choose the shortest among these
	int shortest_queue = v[0];
	for (int i = 0; i < v.size(); i++)
	{
		if (shortest_queue > v[i])
		{
			shortest_queue = v[i];
		}
	}

	// find all shortest queue indeces
	vector<int> min_queues;
	for (int i = 0; i < v.size(); i++)
	{
		if (shortest_queue == v[i]) {
			min_queues.push_back(q[i]);
		}
	}

	// return an index of a random shortest queue
	return min_queues[rand() % min_queues.size()];

}

int JIQ_Dispatcher::get_destination()
{

	if (idle_queues_.size() == 0)
	{
		// Idle queue pool is empty? choose randomy
		return rand() % num_servers_;
	}
	else
	{
		// Choose random idle queue
		int idle_pool_index = rand() % idle_queues_.size();
		int chosen_idle_queue = idle_queues_[idle_pool_index];
		idle_queues_.erase(idle_queues_.begin() + idle_pool_index);
		return chosen_idle_queue;
	}
}

void JIQ_Dispatcher::update_idle_queue(int i)
{
	// insert queue to the pool of idle queues
	idle_queues_.push_back(i);
}

int LSQ_Dispatcher::get_destination()
{
	return local_servers_heap_->GetMin();
}

void LSQ_Dispatcher::update_local_queues(vector<int> &q, vector<int> &v)
{
	for (int i = 0; i < v.size(); i++)
	{
		local_servers_heap_->UpdateKey(q[i], v[i]);
	}
}

int LSQ_Dispatcher::get_local_queue(int i)
{
	return local_servers_heap_->GetVal(i);
}

int WR_Dispatcher::get_destination()
{

	double rnd = ((double)rand() / RAND_MAX) * server_rates_sum_ - 0.00001;
	
	for (int i = 0; i < num_servers_; i++) {
		if (rnd <= server_rates_[i])
			return i;
		rnd -= server_rates_[i];
	}

	assert(!"should never get here");
	return -1;

}

int RR_Dispatcher::get_destination()
{
	return (curr_++) % num_servers_;
}

int WRR_Dispatcher::gcd_(int a, int b)
{
	if (a == 0)
		return b;
	return gcd_(b % a, a);
}

int WRR_Dispatcher::array_gcd_(int arr[], int n)
{
	int result = arr[0];
	for (int i = 1; i < n; i++)
		result = gcd_(arr[i], result);

	return result;
}

int WRR_Dispatcher::get_destination()
{

	curr_sum_ += step_;

	if (server_rates_[curr_] - curr_sum_ < resolution_distance_)
	{
		curr_sum_ = 0;
		curr_ = (curr_ + 1) % num_servers_;
	}

	return curr_;

}

int POM_Dispatcher::get_destination(vector<int>& q, vector<int>& v)
{

	// Choose the shortest among these
	int shortest_queue = v[0];
	for (int i = 0; i < v.size(); i++)
	{
		if (shortest_queue > v[i])
		{
			shortest_queue = v[i];
		}
	}

	// find all shortest queue indeces
	vector<int> min_queues;
	for (int i = 0; i < v.size(); i++)
	{
		if (shortest_queue == v[i]) {
			min_queues.push_back(q[i]);
		}
	}

	int index = rand() % min_queues.size();

	// remember the shortest
	m_shortest_ = min_queues[rand() % min_queues.size()];

	// return an index of a random shortest queue
	return m_shortest_;

}

int POM_Dispatcher::get_previous_shortest()
{
	return m_shortest_;
}
