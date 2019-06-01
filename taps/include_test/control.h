#ifndef _CONTROL_H_
#define _CONTROL_H_

#define SERVER_NUM 4
#define FLOW_NUM 100
#define TASK_NUM 100  //for each server

//#define FAIR_SHARING 1

#define TAPS 1
//#define PDQ 1

#ifdef FAIR_SHARING
#define THRESHOLD 40
#endif

#ifdef PDQ
#define THRESHOLD 60
#endif

#ifdef TAPS
#define THRESHOLD 85
#endif

#endif
