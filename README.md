# Adaptive-solutions experimentation runtime (ASER)

[![Build Status](https://travis-ci.org/betabandido/ASER.svg?branch=master)](https://travis-ci.org/betabandido/ASER)

ASER is a runtime to do research on adaptive solutions for optimizing the usage of shared resources in a computing system. The runtime is capable of managing the execution of multiple processes, while monitoring the execution and taking decisions to optimize a certain metric of interest.

Execution managers and execution monitors are the two main constructs in ASER. Execution managers are responsible of managing all the life-cycle of the processes belonging to the workload to be monitored---from the creation of these processes to detecting when they have finished. Execution monitors are used to perform a diverse set of tasks that monitor and control the processes being executed. Users can extend the base execution monitor to implement their custom optimization policies.

## Minimalistic Example

```
pt::ptree properties;
pt::read_json("config.json", properties);

std::vector<simple_manager::benchmark> benchs = {
  {0, {"/usr/bin/env", "sleep", "5"}},
  {1, {"/usr/bin/env", "sleep", "2"}}
};

simple_manager exec_mgr(properties, benchs);
exec_mgr.start();
```

This code snippet uses an execution manager (`simple_manager`) to execute two processes that will run the command sleep---with different arguments. Once the first process finishes, the execution manager will kill the remaining process and the execution will end.

The contents of the configuration file (config.json) are:

```
{
  "exec_monitor": {
    "type": "simple",
    "sampling_length": 500
  }
}
```

ASER will create an execution monitor belonging to type "simple", with a sampling period of 500 ms. This simplistic monitor does nothing but run a loop, sleeping for 500 ms on each iteration.

