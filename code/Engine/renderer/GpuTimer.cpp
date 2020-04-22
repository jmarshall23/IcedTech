// GpuTimer.cpp
//

#include "Engine_precompiled.h"
#include "tr_local.h"

/*
=====================
rvmGpuTimer::rvmGpuTimer
=====================
*/
rvmGpuTimer::rvmGpuTimer() {
	glGenQueries(2, queryID);
}

/*
=====================
rvmGpuTimer::~rvmGpuTimer
=====================
*/
rvmGpuTimer::~rvmGpuTimer() {
	glDeleteQueries(2, queryID);
}

/*
=====================
rvmGpuTimer::Start
=====================
*/
void rvmGpuTimer::Start(void) {
	glQueryCounter(queryID[0], GL_TIMESTAMP);
}

/*
=====================
rvmGpuTimer::End
=====================
*/
void rvmGpuTimer::End(void) {
	glQueryCounter(queryID[1], GL_TIMESTAMP);



	glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);
}

/*
=====================
rvmGpuTimer::Length
=====================
*/
int rvmGpuTimer::Length() { 
	return ((double)(stopTime - startTime)) / 1000000.0; 
}