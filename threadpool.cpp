#include "threadpool.h"
#include <thread>
#include <queue>
#include <vector>
#include <iostream>
using namespace std;



Worker::Worker(CombinationGenerator gen) {
  generator = gen;
}

void Worker::operator()() {
  function<void()> task;
  string magpair;
  StarGroup current_group;

  while (! generator.done) {
    current_group = StarGroup(apply_indices(probestars, generator.next()));
        
    magpair = current_group.magpair();
    if ( current_group.valid() ) {
      if ( ! has_collisions(current_group, probes) ) {
        magpair = current_group.magpair();
        valid_mags_map[magpair] = true;

        for ( string dimmer_pair : dimmer_wfs(magpair) ) { valid_mags_map[dimmer_pair] = true; }
        for ( string dimmer_pair : dimmer_gdr(magpair) ) { valid_mags_map[dimmer_pair] = true; }
      }
    }
  }
}

int ThreadPool::numtasks() {
  return tasks.size();
}

ThreadPool::ThreadPool(vector<CombinationGenerator> generators)
  : stop(false)
{
  for (int i=0; i<_nthreads; i++) {
    workers.push_back(thread(Worker(*this)));
  }
}

void ThreadPool::joinall() {
  for (int i=0; i<workers.size(); i++) {
    workers[i].join();
  }
}

ThreadPool::~ThreadPool() {
  stop = true;
  condition.notify_all();

  for (int i=0; i<workers.size(); i++) {
    workers[i].join();
  }
}

template<class F>
void ThreadPool::enqueue(F f) {
  {
    unique_lock<mutex> lock(queue_mutex);
    tasks.push_back(function<void()>(f));
  }

  condition.notify_one();
}
