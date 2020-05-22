#include "Defs.h"
#include "Server.h"
#include "Dispatcher.h"

int max_repeating(int* destination_arr, int m, int n)
{

	int* server_arr = new int[n];
	for (int i = 0; i < n; i++)
		server_arr[i] = 0;

	for (int i = 0; i < m; i++)
	{
		server_arr[destination_arr[i]] += 1;
	}

	int result = 0;
	for (int i = 0; i < n; i++)
	{
		if (server_arr[i] > result)
		{
			result = server_arr[i];
		}
	}

	delete[] server_arr;

	return result;
}


void rnd_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	Dispatcher** dispatchers = new Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << 0 << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << 0;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map) 
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}
			
	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void rr_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	RR_Dispatcher** dispatchers = new RR_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new RR_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << 0 << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << 0;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void wrnd_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	WR_Dispatcher** dispatchers = new WR_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new WR_Dispatcher(i, n, load, sr);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << 0 << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << 0;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void wrr_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	WRR_Dispatcher** dispatchers = new WRR_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new WRR_Dispatcher(i, n, load, sr);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << 0 << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << 0;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void jsq_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	JSQ_Dispatcher** dispatchers = new JSQ_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new JSQ_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA
			
			if (arrived_jobs[i] > 0)
			{
				servers[destinations[i]]->add_jobs(arrived_jobs[i], t);
				message_count = message_count + n;

				dispatchers[i]->update_server(destinations[i], servers[destinations[i]]->get_size());
			}
		}

		// Departures
		for (int i = 0; i < n; i++)
		{
			servers[i]->complete_jobs(t);

			int curr_qsize = servers[i]->get_size();
			for (int j = 0; j < m; j++) 
			{
				dispatchers[j]->update_server(i, curr_qsize);
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string in_fn;
	in_fn.append(fn);
	in_fn.append("_incast.txt");

	incast_data_file.open(in_fn, ofstream::out | ofstream::app);

	for (int i = 0; i < m + 1; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}

	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void poc_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA


	// What is p?
	int p = 2;

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	POC_Dispatcher** dispatchers = new POC_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new POC_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			
			// sample p different servers
			vector<int> sampled_queues_id;
			vector<int> sampled_queues_val;

			while (sampled_queues_id.size() < p)
			{

				int random_queue = rand() % n;

				if (find(sampled_queues_id.begin(), sampled_queues_id.end(), random_queue) == sampled_queues_id.end()) {

					sampled_queues_id.push_back(random_queue);
					sampled_queues_val.push_back(servers[random_queue]->get_size());

				}

			}
			
			destinations[i] = dispatchers[i]->get_destination(sampled_queues_id, sampled_queues_val);
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);
			if (arrived_jobs[i] > 0)
			{
				message_count = message_count + p;
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{
			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void pom_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// What is p? ... m = 1.
	int p = 2;

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	POM_Dispatcher** dispatchers = new POM_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new POM_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{

			// sample p different servers
			vector<int> sampled_queues_id;
			vector<int> sampled_queues_val;

			int previous_shortest = dispatchers[i]->get_previous_shortest();

			sampled_queues_id.push_back(previous_shortest);
			sampled_queues_val.push_back(servers[previous_shortest]->get_size());

			while (sampled_queues_id.size() < p)
			{

				int random_queue = rand() % n;

				if (find(sampled_queues_id.begin(), sampled_queues_id.end(), random_queue) == sampled_queues_id.end()) {

					sampled_queues_id.push_back(random_queue);
					sampled_queues_val.push_back(servers[random_queue]->get_size());

				}

			}

			destinations[i] = dispatchers[i]->get_destination(sampled_queues_id, sampled_queues_val);
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);
			if (arrived_jobs[i] > 0)
			{
				message_count = message_count + p;
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void jiq_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// Servers initialization
	JIQ_Server** servers = new JIQ_Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new JIQ_Server(i, sr[i]);
	}

	// Dispatchers initialization
	JIQ_Dispatcher** dispatchers = new JIQ_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new JIQ_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			bool got_idle = servers[i]->complete_jobs(t);
			if (got_idle)
			{
				dispatchers[rand()%m]->update_idle_queue(i);
				message_count = message_count + 1;
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;

		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string I_fn;
	I_fn.append(fn);
	I_fn.append("_incast.txt");

	incast_data_file.open(I_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < m; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}
	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void lsq_sample_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

	ofstream approx_animation_data_file;

	string approx_a_fn;
	approx_a_fn.append(fn);
	approx_a_fn.append("_approx_amination.txt");
	approx_animation_data_file.open(approx_a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// what is p?
	int p = 2;

	// Servers initialization
	Server** servers = new Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Server(i, sr[i]);
	}

	// Dispatchers initialization
	LSQ_Dispatcher** dispatchers = new LSQ_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new LSQ_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{

			// sample only if new jobs arrive
			if (arrived_jobs[i] > 0) {

				// sample p different servers
				vector<int> sampled_queues_id;
				vector<int> sampled_queues_val;

				while (sampled_queues_id.size() < p)
				{

					int random_queue = rand() % n;

					if (find(sampled_queues_id.begin(), sampled_queues_id.end(), random_queue) == sampled_queues_id.end()) {

						sampled_queues_id.push_back(random_queue);
						sampled_queues_val.push_back(servers[random_queue]->get_size());

					}

				}

				dispatchers[i]->update_local_queues(sampled_queues_id, sampled_queues_val);
				message_count = message_count + p;

			}

			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Update local views of selected destinations
		for (int i = 0; i < m; i++)
		{
			if (arrived_jobs[i] > 0)
			{
				// add to local view
				vector<int> sampled_queues_id;
				vector<int> sampled_queues_val;

				sampled_queues_id.push_back(destinations[i]);
				sampled_queues_val.push_back(servers[destinations[i]]->get_size() + arrived_jobs[i]);

				dispatchers[i]->update_local_queues(sampled_queues_id, sampled_queues_val);
			}
		}

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			servers[i]->complete_jobs(t);

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
			approx_animation_data_file << "Server " << i << " Jobs " << dispatchers[0]->get_local_queue(i) << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;
		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string in_fn;
	in_fn.append(fn);
	in_fn.append("_incast.txt");

	incast_data_file.open(in_fn, ofstream::out | ofstream::app);

	for (int i = 0; i < m + 1; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}

	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
	approx_animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void lsq_update_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

	ofstream approx_animation_data_file;

	string approx_a_fn;
	approx_a_fn.append(fn);
	approx_a_fn.append("_approx_amination.txt");
	approx_animation_data_file.open(approx_a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// what is p? we send message w.p. m*p/n.
	int p = 2;

	// Servers initialization
	JIQ_Server** servers = new JIQ_Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new JIQ_Server(i, sr[i]);
	}

	// Dispatchers initialization
	LSQ_Dispatcher** dispatchers = new LSQ_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new LSQ_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Update local views of selected destinations
		for (int i = 0; i < m; i++)
		{
			if (arrived_jobs[i] > 0)
			{
				// add to local view
				vector<int> sampled_queues_id;
				vector<int> sampled_queues_val;

				sampled_queues_id.push_back(destinations[i]);
				sampled_queues_val.push_back(servers[destinations[i]]->get_size() + arrived_jobs[i]);

				dispatchers[i]->update_local_queues(sampled_queues_id, sampled_queues_val);
			}
		}

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures
		for (int i = 0; i < n; i++)
		{

			bool got_idle = servers[i]->complete_jobs(t);
			if (got_idle)
			{
				// send message w.p. 1
				vector<int> sampled_queues_id;
				vector<int> sampled_queues_val;

				sampled_queues_id.push_back(i);
				sampled_queues_val.push_back(servers[i]->get_size());

				dispatchers[rand() % m]->update_local_queues(sampled_queues_id, sampled_queues_val);

				message_count = message_count + 1;
			}
			else {
				// send message w.p. (m*p)/n
				if (rand() % n < m*p) {

					vector<int> sampled_queues_id;
					vector<int> sampled_queues_val;

					sampled_queues_id.push_back(i);
					sampled_queues_val.push_back(servers[i]->get_size());

					dispatchers[rand() % m]->update_local_queues(sampled_queues_id, sampled_queues_val);
					
					message_count = message_count + 1;
				}
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
			approx_animation_data_file << "Server " << i << " Jobs " << dispatchers[0]->get_local_queue(i) << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;

		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string in_fn;
	in_fn.append(fn);
	in_fn.append("_incast.txt");

	incast_data_file.open(in_fn, ofstream::out | ofstream::app);

	for (int i = 0; i < m + 1; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}

	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
	approx_animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


void lsq_smart_sim(int n, int m, double load, double* sr, int time, string fn)
{

#ifdef ANIMATION_DATA

	ofstream animation_data_file;

	string a_fn;
	a_fn.append(fn);
	a_fn.append("_amination.txt");
	animation_data_file.open(a_fn, ofstream::out | ofstream::app);

	ofstream approx_animation_data_file;

	string approx_a_fn;
	approx_a_fn.append(fn);
	approx_a_fn.append("_approx_amination.txt");
	approx_animation_data_file.open(approx_a_fn, ofstream::out | ofstream::app);

#endif // ANIMATION_DATA

	// what is p? we send message w.p. of a least p*m/n.
	double p = 2.0;

	// Random numbers engine
	default_random_engine main_generator;

	// Servers initialization
	Smart_Server** servers = new Smart_Server*[n];
	for (int i = 0; i < n; i++)
	{
		servers[i] = new Smart_Server(i, sr[i]);
	}

	// Dispatchers initialization
	LSQ_Dispatcher** dispatchers = new LSQ_Dispatcher*[m];
	for (int i = 0; i < m; i++)
	{
		dispatchers[i] = new LSQ_Dispatcher(i, n, load);
	}

	// Auxiliary arrays
	int* arrived_jobs = new int[m];
	int* destinations = new int[m];

#ifdef INCAST
	int* incast = new int[m + 1]();
#endif // Incast

	// Count number of exchanged server-dispatcher messages
	double message_count = 0;

	// Main loop
	for (int t = 0; t < time; t++)
	{

		// Arrivals		
		for (int i = 0; i < m; i++)
		{
			arrived_jobs[i] = dispatchers[i]->get_arrivals();
		}

		// Destinations
		for (int i = 0; i < m; i++)
		{
			destinations[i] = dispatchers[i]->get_destination();
		}

#ifdef INCAST		
		incast[max_repeating(destinations, m, n)] += 1;
#endif // Incast

		// Update local views of selected destinations of selected destinations
		for (int i = 0; i < m; i++)
		{
			if (arrived_jobs[i] > 0)
			{
				// add to local view
				vector<int> sampled_queues_id;
				vector<int> sampled_queues_val;

				sampled_queues_id.push_back(destinations[i]);
				sampled_queues_val.push_back(servers[destinations[i]]->get_size() + arrived_jobs[i]);

				dispatchers[i]->update_local_queues(sampled_queues_id, sampled_queues_val);
			}
		}

		// Admit jobs to servers
		for (int i = 0; i < m; i++)
		{

			servers[destinations[i]]->add_jobs(arrived_jobs[i], t);

#ifdef ANIMATION_DATA
			animation_data_file << "Dispatcher " << i << " Destination " << destinations[i] << endl;
#endif // ANIMATION_DATA

		}

		// Departures - here is all the "smart communication" logic
		for (int i = 0; i < n; i++)
		{

			// departures
			bool completed_jobs = servers[i]->complete_jobs(t);

			// completed jobs?
			if (completed_jobs)
			{
				// get the real queue length
				int global_queue_size = servers[i]->get_size();

				// search for the max-gap queue value
				int mgap_value = 0;

				for (int j = 0; j < m; j++)
				{
					// local view of server i by dispatcher j
					int local_value = dispatchers[j]->get_local_queue(i);

					if (abs(global_queue_size - local_value) > mgap_value)
					{
						mgap_value = abs(global_queue_size - local_value);
					}
				}

				double zq = 0;
				if ((mgap_value > global_queue_size) || (global_queue_size == 0))
				{
					zq = 1;
				}
				double bd_p = (m * p) / n + zq * (1.0 - (m * p) / n);
				bernoulli_distribution bd(bd_p);
				bool send_message = bd(main_generator);

				if (send_message)
				{

					// update message count
					message_count = message_count + 1;

					// find all mgap queues
					vector<int> mgap_queues;
					for (int j = 0; j < m; j++)
					{
						if (mgap_value == abs(global_queue_size - dispatchers[j]->get_local_queue(i))) {
							mgap_queues.push_back(j);
						}
					}

					// update a random mgap queue
					int rand_mgap_queue = rand() % mgap_queues.size();

					vector<int> sampled_queues_id;
					vector<int> sampled_queues_val;

					sampled_queues_id.push_back(i);
					sampled_queues_val.push_back(servers[i]->get_size());

					dispatchers[mgap_queues[rand_mgap_queue]]->update_local_queues(sampled_queues_id, sampled_queues_val);
				}
			}

#ifdef ANIMATION_DATA
			animation_data_file << "Server " << i << " Jobs " << servers[i]->get_size() << endl;
			approx_animation_data_file << "Server " << i << " Jobs " << dispatchers[0]->get_local_queue(i) << endl;
#endif // ANIMATION_DATA

		}

		// Collect stats and print
		if (t % 100000 == 100000 - 1) {

			double total_system_jobs = 0;
			double total_commulative_delays = 0;
			double total_departures = 0;

			for (int i = 0; i < n; i++)
			{
				total_system_jobs += servers[i]->get_commulative_size();
				total_commulative_delays += servers[i]->get_commulative_job_delays();
				total_departures += servers[i]->get_commulative_departures();
			}
			cout <<
				"Time: " << t <<
				" Jobs: " << total_system_jobs / t <<
				" Delay: " << total_commulative_delays / total_departures <<
				" Message rate: " << message_count / t << endl;

		}

	}

	// Collect final results
	double total_system_jobs = 0;
	double total_commulative_delays = 0;
	double total_departures = 0;

	for (int i = 0; i < n; i++)
	{
		total_system_jobs += servers[i]->get_commulative_size();
		total_commulative_delays += servers[i]->get_commulative_job_delays();
		total_departures += servers[i]->get_commulative_departures();
	}

	// Print results to a file
	ofstream data_file;

	string g_fn;
	g_fn.append(fn);
	g_fn.append(".txt");

	data_file.open(g_fn, ofstream::out | ofstream::app);
	data_file << "Load\t" << load << "\tJobs\t" << total_system_jobs / time;
	data_file << "\tDelays\t" << total_commulative_delays / total_departures;
	data_file << "\tMrate\t" << message_count / time;
	data_file << "\n";

	data_file.close();

#ifdef JOB_COMPLETION_TIME

	// Collect job completion time results
	ofstream jct_data_file;

	string d_fn;
	d_fn.append(fn);
	d_fn.append("_load_");
	d_fn.append(to_string(load));
	d_fn.append("_jct.txt");

	jct_data_file.open(d_fn, ofstream::out | ofstream::app);
	for (int i = 0; i < n; i++) {
		unordered_map<int, int>* jct_map = servers[i]->get_jct_map();
		for (auto& t : *jct_map)
		{
			jct_data_file << t.first << " " << t.second << "\n";
		}

	}
	jct_data_file.close();

#endif

#ifdef INCAST

	// Collect job completion time results
	ofstream incast_data_file;

	string in_fn;
	in_fn.append(fn);
	in_fn.append("_incast.txt");

	incast_data_file.open(in_fn, ofstream::out | ofstream::app);

	for (int i = 0; i < m + 1; i++)
	{
		incast_data_file << i << " " << incast[i] << " " << "\n";
	}

	incast_data_file.close();

#endif

#ifdef ANIMATION_DATA
	animation_data_file.close();
	approx_animation_data_file.close();
#endif // ANIMATION_DATA

	// Free allocated memory
	delete[] arrived_jobs;
	delete[] destinations;
#ifdef INCAST
	delete[] incast;
#endif
	for (int i = 0; i < n; i++)
	{
		delete servers[i];
	}
	delete[] servers;
	for (int i = 0; i < m; i++)
	{
		delete dispatchers[i];
	}
	delete[] dispatchers;
}


double calculate_rates(int n, int m, int rate_ratio, int server_skew, double* mu, double load)
{

	// We are interested in server ratios: 10%:90%, 50%:50%, 90%:10%.
	// We are intersted in skewes: 1:1, 1:2, 1:10.

	double nominal_rate = 100.0;

	// number of slow servers is n*ratio
	double ratios[3] = { 0.1, 0.5, 0.9 };

	// server rates ratio
	double skewes[3] = { 1.0, 2.0, 10.0 };

	double n_slow = n * ratios[rate_ratio];
	double n_fast = n - n * ratios[rate_ratio];

	// asserts
	assert(n_slow + n_fast == n && "n_slow + n_fast is not equal to n");
	assert(n_slow > 0 && "n_slow is not positive");
	assert(n_fast > 0 && "n_fast is not positive");

	double mu_fast_zero = (n_slow / skewes[server_skew] + n_fast) / nominal_rate;
	double mu_fast = mu_fast_zero / (mu_fast_zero + 1);
	double mu_slow_zero = mu_fast_zero * skewes[server_skew];
	double mu_slow = mu_slow_zero / (mu_slow_zero + 1);

	// asserts
	double total_service_rate = n_slow * (1 - mu_slow) / mu_slow + n_fast * (1 - mu_fast) / mu_fast;
	assert((mu_fast > 0 && mu_fast < 1) && "mu_fast not in (0,1)");
	assert((mu_slow > 0 && mu_slow < 1) && "mu_slow not in (0,1)");
	assert(round(total_service_rate) == nominal_rate && "total rate does not equal nominal rate");

	for (int i = 0; i < n_slow; i++)
	{
		mu[i] = mu_slow;
	}
	for (int i = (int)n_slow; i < n; i++)
	{
		mu[i] = mu_fast;
	}

	double lambda = nominal_rate * load / m;

	cout << "lambda is " << lambda * m << " with a rate of " << lambda << " per each of the " << m << " dispatchers" << endl;
	cout << n_fast << " fast servers with rate " << (1 - mu_fast) / mu_fast << endl;
	cout << n_slow << " slow servers with rate " << (1 - mu_slow) / mu_slow << endl;

	return lambda;

}


int main(int argc, char *argv[])
{

	if (argc < 8) {
		cout << "Usage Error:\n";
		cout << "argv[1]: int number of servers\n";
		cout << "argv[2]: int number of dispatcher\n";
		cout << "argv[3]: double load\n";
		cout << "argv[4]: int time\n";
		cout << "argv[5]: int algorithm\n";
		cout << "argv[6]: int rate ratio\n";
		cout << "argv[7]: int server skew\n";
		cin.get();
		return 0;
	}

	// Input arguments
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	double load = atof(argv[3]);
	int time = atoi(argv[4]);
	int alg = atoi(argv[5]);
	int rate_ratio = atoi(argv[6]);
	int server_skew = atoi(argv[7]);

	// Set arrival and departure rates
	double* mu = new double[n];
	double lambda = calculate_rates(n, m, rate_ratio, server_skew, mu, load);

	string fn_prefix;

	// number of slow servers is n*ratio
	string ratios[3] = { "0.1", "0.5", "0.9" };

	// server rates ratio - "1.0" is homogenous
	string skewes[3] = { "1.0", "2.0", "10.0" };

	fn_prefix.append("n_");
	fn_prefix.append(argv[1]);
	fn_prefix.append("_m_");
	fn_prefix.append(argv[2]);
	fn_prefix.append("_r_");
	fn_prefix.append(ratios[rate_ratio]);
	fn_prefix.append("_s_");
	fn_prefix.append(skewes[server_skew]);
	fn_prefix.append("_");

	// choose algorithm
	switch (alg) {
	case 1: rnd_sim(n, m, lambda, mu, time, fn_prefix.append("random"));
		break;   
	case 2: jsq_sim(n, m, lambda, mu, time, fn_prefix.append("jsq"));
		break;
	case 3: poc_sim(n, m, lambda, mu, time, fn_prefix.append("poc"));
		break;
	case 4: jiq_sim(n, m, lambda, mu, time, fn_prefix.append("jiq"));
		break;
	case 5: lsq_sample_sim(n, m, lambda, mu, time, fn_prefix.append("lsq_sample"));
		break;
	case 6: lsq_update_sim(n, m, lambda, mu, time, fn_prefix.append("lsq_update"));
		break;
	case 7: lsq_smart_sim(n, m, lambda, mu, time, fn_prefix.append("lsq_smart"));
		break;
	case 8: wrnd_sim(n, m, lambda, mu, time, fn_prefix.append("weighted_random"));
		break;
	case 9: rr_sim(n, m, lambda, mu, time, fn_prefix.append("round_robin"));
		break;
	case 10: wrr_sim(n, m, lambda, mu, time, fn_prefix.append("weighted_round_robin"));
		break;
	case 11: pom_sim(n, m, lambda, mu, time, fn_prefix.append("pom"));
		break;
	}

	delete[] mu;

	return 0;

}
