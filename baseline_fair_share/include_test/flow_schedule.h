#include <vector>
#include "../include_test/task.h"

#ifndef _FLOW_SCHEDULE_H_
#define _FLOW_SCHEDULE_H_


vector<struct Sched_task_elem> task_schedule(vector<struct Task_elem> &task);

vector<struct Task_elem> generate_task();

#endif