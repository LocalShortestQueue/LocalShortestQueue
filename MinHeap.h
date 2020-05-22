#pragma once
#include "Defs.h"

constexpr auto NIL_NODE = -1;

class MinHeap
{

private:

	struct heap_node
	{
		int left_;
		int right_;
		int parent_;

		double queue_size_;
	};

	heap_node* heap_;
	int min_index_;
	int num_servers_;

	void IncreaseKey_(int i);
	void DecreaseKey_(int i);
	void Heapify_(int i);
	void Swap_(int i, int j);

public:

	MinHeap(int n);
	~MinHeap();

	void UpdateKey(int i, int val);

	int GetMin();

	int GetVal(int i);

};


