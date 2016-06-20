#include <vector>
#include <thread>
#include <queue>
#include <map>
using namespace std;


#ifndef _THREADPOOL_H
#define _THREADPOOL_H


class ThreadPool;

class Worker {
 public:
  Worker(ThreadPool &tp) { }
  Worker(CombinationGenerator gen, vector<Probe> _probes, vector< vector<Star> > _probestars);
  void operator()();
  CombinationGenerator generator;
  map<string, bool> valid_mags_map;
  vector< vector<Star> > probestars;
  vector<Probe> probes;
};

class ThreadPool {
 public:
  ThreadPool(vector<CombinationGenerator> generators, vector<Probe> _probes, vector< vector<Star> > _probestars);
  ThreadPool();
  ~ThreadPool();
  void joinall();

 private:
  friend class Worker;
  
  vector<thread> workers;
  deque< function<void()> > tasks;

  mutex queue_mutex;
  condition_variable condition;
};


#endif
