#include "MinHeap.h"

MinHeap::MinHeap(int n)
{

	num_servers_ = n;

	heap_ = new heap_node[n];

	min_index_ = 0;

	for (int i = 0; i < n; i++)
	{

		heap_[i].queue_size_ = 0;

		heap_[i].left_ = ((2 * i + 1 < n) ? 2 * i + 1 : NIL_NODE);
		heap_[i].right_ = ((2 * i + 2 < n) ? 2 * i + 2 : NIL_NODE);
		heap_[i].parent_ = ((i == 0) ? NIL_NODE : (i - 1) / 2);

	}

}

MinHeap::~MinHeap()
{
	delete[] heap_;
}

void MinHeap::IncreaseKey_(int i)
{
	Heapify_(i);
}

void MinHeap::DecreaseKey_(int i)
{
	while (heap_[i].parent_ != NIL_NODE && heap_[i].queue_size_ < heap_[heap_[i].parent_].queue_size_)
	{
		Swap_(heap_[i].parent_, i);
	}
}

void MinHeap::Swap_(int parent_node, int child_node)
{

	// swapping finished?
	if (parent_node == NIL_NODE || child_node == NIL_NODE)
	{
		return;
	}

	// update new min?
	if (heap_[parent_node].parent_ == NIL_NODE)
	{
		min_index_ = child_node;
	}

	// save all info
	int grandpa_node = heap_[parent_node].parent_;

	int child_r_child_node = heap_[child_node].right_;
	int child_l_child_node = heap_[child_node].left_;

	int parent_l_child_node = heap_[parent_node].left_;
	int parent_r_child_node = heap_[parent_node].right_;

	// grandpa is not NIL? then, update its changed child ...
	if (grandpa_node != NIL_NODE)
	{
		if (heap_[grandpa_node].left_ == parent_node)
		{
			heap_[grandpa_node].left_ = child_node;
		}
		else
		{
			heap_[grandpa_node].right_ = child_node;
		}
	}

	// update parent node
	heap_[parent_node].parent_ = child_node;
	heap_[parent_node].left_ = child_l_child_node;
	heap_[parent_node].right_ = child_r_child_node;

	// update child node
	heap_[child_node].parent_ = grandpa_node;

	if (parent_l_child_node == child_node) // is it a left child?
	{

		heap_[child_node].left_ = parent_node;
		heap_[child_node].right_ = parent_r_child_node;

		// update cousin
		if (parent_r_child_node != NIL_NODE)
		{
			heap_[parent_r_child_node].parent_ = child_node;
		}

	}
	else // is it a right child?
	{

		heap_[child_node].left_ = parent_l_child_node;
		heap_[child_node].right_ = parent_node;

		// update cousin
		if (parent_l_child_node != NIL_NODE)
		{
			heap_[parent_l_child_node].parent_ = child_node;
		}

	}

	// update grandchilden
	if (child_r_child_node != NIL_NODE)
	{
		heap_[child_r_child_node].parent_ = parent_node;
	}
	if (child_l_child_node != NIL_NODE)
	{
		heap_[child_l_child_node].parent_ = parent_node;
	}

}

void MinHeap::UpdateKey(int i, int val)
{

	double old_val = heap_[i].queue_size_;

	if (old_val == val)
	{
		return;
	}

	double random_tie_break = ((double)rand()) / ((double)(RAND_MAX)+1);

	heap_[i].queue_size_ = val + random_tie_break;

	if (old_val < heap_[i].queue_size_)
	{
		IncreaseKey_(i);
	}

	else
	{
		DecreaseKey_(i);
	}

	return;

}

int MinHeap::GetMin()
{
	return min_index_;
}

int MinHeap::GetVal(int i)
{
	return (int)heap_[i].queue_size_;
}

// A recursive method to heapify a subtree with the root at given index 
// This method assumes that the subtrees are already heapified 
void MinHeap::Heapify_(int i)
{

	int l = heap_[i].left_;
	int r = heap_[i].right_;
	int smallest = i;

	if (l != NIL_NODE && heap_[l].queue_size_ < heap_[i].queue_size_)
		smallest = l;
	if (r != NIL_NODE && heap_[r].queue_size_ < heap_[smallest].queue_size_)
		smallest = r;

	if (smallest != i)
	{
		Swap_(i, smallest);
		Heapify_(i);
	}

}



/*

int main()
{

	int n = 100;
	int t = 1000000;

	MinHeap queues(n);

	cout << "Min Queue: " << queues.GetMin() << endl;

	for (int i = 0; i < t; i++)
	{
		queues.UpdateKey(rand() % n, rand() % 10000);

		if (i % 100000 == 0)
		{
			cout << "Min Queue: " << queues.GetMin() << endl;
		}
	}

	cin.get();
	return 0;
}

*/

