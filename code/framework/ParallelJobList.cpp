/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#include "Engine_precompiled.h"
#include "ParallelJobListLocal.h"

idCVar jobs_prioritize("jobs_prioritize", "1", CVAR_BOOL | CVAR_NOCHEAT, "prioritize job lists");

/*
================================================================================================

	Job and Job-List names

================================================================================================
*/

const char * jobNames[] = {
	"Renderer",
	"Game",
	"Utility",	
};

static const int MAX_REGISTERED_JOBS = 128;
struct registeredJob {
	jobRun_t		function;
	const char *	name;
} registeredJobs[MAX_REGISTERED_JOBS];
static int numRegisteredJobs;

const char * GetJobListName( jobListId_t id ) {
	return jobNames[id];
}

/*
========================
IsRegisteredJob
========================
*/
static bool IsRegisteredJob( jobRun_t function ) {
	for ( int i = 0; i < numRegisteredJobs; i++ ) {
		if ( registeredJobs[i].function == function ) {
			return true;
		}
	}
	return false;
}

/*
========================
idParallelJobManagerLocal::RegisterJob
========================
*/
void idParallelJobManagerLocal::RegisterJob( jobRun_t function, const char * name ) {
	if ( IsRegisteredJob( function ) ) {
		return;
	}
	registeredJobs[numRegisteredJobs].function = function;
	registeredJobs[numRegisteredJobs].name = name;
	numRegisteredJobs++;
}

/*
========================
GetJobName
========================
*/
const char * GetJobName( jobRun_t function ) {
	for ( int i = 0; i < numRegisteredJobs; i++ ) {
		if ( registeredJobs[i].function == function ) {
			return registeredJobs[i].name;
		}
	}
	return "unknown";
}

int globalSpuLocalStoreActive;
void * globalSpuLocalStoreStart;
void * globalSpuLocalStoreEnd;
idSysMutex globalSpuLocalStoreMutex;

/*
================================================================================================

	PS3

================================================================================================
*/


/*
================================================================================================

idParallelJobList_Threads

================================================================================================
*/

static idCVar jobs_longJobMicroSec( "jobs_longJobMicroSec", "10000", CVAR_INTEGER, "print a warning for jobs that take more than this number of microseconds" );


const static int		MAX_THREADS	= 32;

struct threadJobListState_t {
								threadJobListState_t() :
									jobList( NULL ),
									version( 0xFFFFFFFF ),
									signalIndex( 0 ),
									lastJobIndex( 0 ),
									nextJobIndex( -1 ) {}
								threadJobListState_t( int _version ) :
									jobList( NULL ),
									version( _version ),
									signalIndex( 0 ),
									lastJobIndex( 0 ),
									nextJobIndex( -1 ) {}
	idParallelJobList_Threads *	jobList;
	int							version;
	int							signalIndex;
	int							lastJobIndex;
	int							nextJobIndex;
};

struct threadStats_t {
	unsigned int	numExecutedJobs;
	unsigned int	numExecutedSyncs;
	uint64_t			submitTime;
	uint64_t			startTime;
	uint64_t			endTime;
	uint64_t			waitTime;
	uint64_t			threadExecTime[MAX_THREADS];
	uint64_t			threadTotalTime[MAX_THREADS];
};

class idParallelJobList_Threads {
public:
							idParallelJobList_Threads( jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs );
							~idParallelJobList_Threads();

	//------------------------
	// These are called from the one thread that manages this list.
	//------------------------
	ID_INLINE void			AddJob( jobRun_t function, void * data );
	ID_INLINE void			InsertSyncPoint( jobSyncType_t syncType );
	void					Submit( idParallelJobList_Threads * waitForJobList_, int parallelism );
	void					Wait();
	bool					TryWait();
	bool					IsSubmitted() const;

	unsigned int			GetNumExecutedJobs() const { return threadStats.numExecutedJobs; }
	unsigned int			GetNumSyncs() const { return threadStats.numExecutedSyncs; }
	uint64_t					GetSubmitTimeMicroSec() const { return threadStats.submitTime; }
	uint64_t					GetStartTimeMicroSec() const { return threadStats.startTime; }
	uint64_t					GetFinishTimeMicroSec() const { return threadStats.endTime; }
	uint64_t					GetWaitTimeMicroSec() const { return threadStats.waitTime; }
	uint64_t					GetTotalProcessingTimeMicroSec() const;
	uint64_t					GetTotalWastedTimeMicroSec() const;
	uint64_t					GetUnitProcessingTimeMicroSec( int unit ) const;
	uint64_t					GetUnitWastedTimeMicroSec( int unit ) const;

	jobListId_t				GetId() const { return listId; }
	jobListPriority_t		GetPriority() const { return listPriority; }
	int						GetVersion() { return version.GetValue(); }

	bool					WaitForOtherJobList();

	//------------------------
	// This is thread safe and called from the job threads.
	//------------------------
	enum runResult_t {
		RUN_OK			= 0,
		RUN_PROGRESS	= BIT( 0 ),
		RUN_DONE		= BIT( 1 ),
		RUN_STALLED		= BIT( 2 )
	};

	int						RunJobs( unsigned int threadNum, threadJobListState_t & state, bool singleJob );

private:
	static const int		NUM_DONE_GUARDS = 4;	// cycle through 4 guards so we can cyclicly chain job lists

	bool					threaded;
	bool					done;
	bool					hasSignal;
	jobListId_t				listId;
	jobListPriority_t		listPriority;
	unsigned int			maxJobs;
	unsigned int			maxSyncs;
	unsigned int			numSyncs;
	int						lastSignalJob;
	idSysInterlockedInteger * waitForGuard;
	idSysInterlockedInteger doneGuards[NUM_DONE_GUARDS];
	int						currentDoneGuard;
	idSysInterlockedInteger	version;
	struct job_t {
		jobRun_t	function;
		void *		data;
		int			executed;
	};
	idList< job_t >		jobList;
	idList< idSysInterlockedInteger >	signalJobCount;
	idSysInterlockedInteger				currentJob;
	idSysInterlockedInteger				fetchLock;
	idSysInterlockedInteger				numThreadsExecuting;

	threadStats_t						deferredThreadStats;
	threadStats_t						threadStats;

	int						RunJobsInternal( unsigned int threadNum, threadJobListState_t & state, bool singleJob );

	static void				Nop( void * data ) {}

	static int				JOB_SIGNAL;
	static int				JOB_SYNCHRONIZE;
	static int				JOB_LIST_DONE;
};

int idParallelJobList_Threads::JOB_SIGNAL;
int idParallelJobList_Threads::JOB_SYNCHRONIZE;
int idParallelJobList_Threads::JOB_LIST_DONE;

/*
========================
idParallelJobList_Threads::idParallelJobList_Threads
========================
*/
idParallelJobList_Threads::idParallelJobList_Threads( jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs ) :
	threaded( true ),
	done( true ),
	hasSignal( false ),
	listId( id ),
	listPriority( priority ),
	numSyncs( 0 ),
	lastSignalJob( 0 ),
	waitForGuard( NULL ),
	currentDoneGuard( 0 ),
	jobList() {

	assert( listPriority != JOBLIST_PRIORITY_NONE );

	this->maxJobs = maxJobs;
	this->maxSyncs = maxSyncs;
	jobList.AssureSize( maxJobs + maxSyncs * 2 + 1 );	// syncs go in as dummy jobs and one more to update the doneCount
	jobList.SetNum( 0 );
	signalJobCount.AssureSize( maxSyncs + 1 );			// need one extra for submit
	signalJobCount.SetNum( 0 );

	memset( &deferredThreadStats, 0, sizeof( threadStats_t ) );
	memset( &threadStats, 0, sizeof( threadStats_t ) );
}

/*
========================
idParallelJobList_Threads::~idParallelJobList_Threads
========================
*/
idParallelJobList_Threads::~idParallelJobList_Threads() {
	Wait();
}

/*
========================
idParallelJobList_Threads::AddJob
========================
*/
ID_INLINE void idParallelJobList_Threads::AddJob( jobRun_t function, void * data ) {
	assert( done );
#if defined( _DEBUG )
	// make sure there isn't already a job with the same function and data in the list
	if ( jobList.Num() < 1000 ) {	// don't do this N^2 slow check on big lists
		for ( int i = 0; i < jobList.Num(); i++ ) {
			assert( jobList[i].function != function || jobList[i].data != data );
		}
	}
#endif
	if ( 1 ) { // JDC: this never worked in tech5!  !jobList.IsFull() ) {
		job_t & job = jobList.Alloc();
		job.function = function;
		job.data = data;
		job.executed = 0;
	} else {
		// debug output to show us what is overflowing
		int currentJobCount[MAX_REGISTERED_JOBS] = {};
		
		for ( int i = 0; i < jobList.Num(); ++i ) {
			const char * jobName = GetJobName( jobList[ i ].function );
			for ( int j = 0; j < numRegisteredJobs; ++j ) {
				if ( jobName == registeredJobs[ j ].name ) {
					currentJobCount[ j ]++;
					break;
				}
			}
		}

		// print the quantity of each job type
		for ( int i = 0; i < numRegisteredJobs; ++i ) {
			if ( currentJobCount[ i ] > 0 ) {
				common->Printf( "Job: %s, # %d", registeredJobs[ i ].name, currentJobCount[ i ] );
			}
		}
		common->Error( "Can't add job '%s', too many jobs %d", GetJobName( function ), jobList.Num() );
	}
}

/*
========================
idParallelJobList_Threads::InsertSyncPoint
========================
*/
ID_INLINE void idParallelJobList_Threads::InsertSyncPoint( jobSyncType_t syncType ) {
	assert( done );
	switch( syncType ) {
		case SYNC_SIGNAL: {
			assert( !hasSignal );
			if ( jobList.Num() ) {
				assert( !hasSignal );
				signalJobCount.Alloc();
				signalJobCount[signalJobCount.Num() - 1].SetValue( jobList.Num() - lastSignalJob );
				lastSignalJob = jobList.Num();
				job_t & job = jobList.Alloc();
				job.function = Nop;
				job.data = & JOB_SIGNAL;
				hasSignal = true;
			}
			break;
		}
		case SYNC_SYNCHRONIZE: {
			if ( hasSignal ) {
				job_t & job = jobList.Alloc();
				job.function = Nop;
				job.data = & JOB_SYNCHRONIZE;
				hasSignal = false;
				numSyncs++;
			}
			break;
		}
	}
}

/*
========================
idParallelJobList_Threads::Submit
========================
*/
void idParallelJobList_Threads::Submit( idParallelJobList_Threads * waitForJobList, int parallelism ) {
	assert( done );
	assert( numSyncs <= maxSyncs );
	assert( (unsigned int) jobList.Num() <= maxJobs + numSyncs * 2 );
	assert( fetchLock.GetValue() == 0 );

	done = false;
	currentJob.SetValue( 0 );

	memset( &deferredThreadStats, 0, sizeof( deferredThreadStats ) );
	deferredThreadStats.numExecutedJobs = jobList.Num() - numSyncs * 2;
	deferredThreadStats.numExecutedSyncs = numSyncs;
	deferredThreadStats.submitTime = Sys_Microseconds();
	deferredThreadStats.startTime = 0;
	deferredThreadStats.endTime = 0;
	deferredThreadStats.waitTime = 0;

	if ( jobList.Num() == 0 ) {
		return;
	}

	if ( waitForJobList != NULL ) {
		waitForGuard = & waitForJobList->doneGuards[waitForJobList->currentDoneGuard];
	} else {
		waitForGuard = NULL;
	}

	currentDoneGuard = ( currentDoneGuard + 1 ) & ( NUM_DONE_GUARDS - 1 );
	doneGuards[currentDoneGuard].SetValue( 1 );

	signalJobCount.Alloc();
	signalJobCount[signalJobCount.Num() - 1].SetValue( jobList.Num() - lastSignalJob );

	job_t & job = jobList.Alloc();
	job.function = Nop;
	job.data = & JOB_LIST_DONE;

	if ( threaded ) {
		// hand over to the manager
		void SubmitJobList( idParallelJobList_Threads * jobList, int parallelism );
		SubmitJobList( this, parallelism );
	} else {
		// run all the jobs right here
		threadJobListState_t state( GetVersion() );
		RunJobs( 0, state, false );
	}
}

/*
========================
idParallelJobList_Threads::Wait
========================
*/
#define verify( x )		( ( x ) ? true : false )

void idParallelJobList_Threads::Wait() {
	if ( jobList.Num() > 0 ) {
		// don't lock up but return if the job list was never properly submitted
		if ( !verify( !done && signalJobCount.Num() > 0 ) ) {
			return;
		}

		bool waited = false;
		uint64_t waitStart = Sys_Microseconds();

		while ( signalJobCount[signalJobCount.Num() - 1].GetValue() > 0 ) {
			Sys_Yield();
			waited = true;
		}
		version.Increment();
		while ( numThreadsExecuting.GetValue() > 0 ) {
			Sys_Yield();
			waited = true;
		}

		jobList.SetNum( 0 );
		signalJobCount.SetNum( 0 );
		numSyncs = 0;
		lastSignalJob = 0;

		uint64_t waitEnd = Sys_Microseconds();
		deferredThreadStats.waitTime = waited ? ( waitEnd - waitStart ) : 0;
	}
	memcpy( & threadStats, & deferredThreadStats, sizeof( threadStats ) );
	done = true;
}

/*
========================
idParallelJobList_Threads::TryWait
========================
*/
bool idParallelJobList_Threads::TryWait() {
	if ( jobList.Num() == 0 || signalJobCount[signalJobCount.Num() - 1].GetValue() <= 0 ) {
		Wait();
		return true;
	}
	return false;
}

/*
========================
idParallelJobList_Threads::IsSubmitted
========================
*/
bool idParallelJobList_Threads::IsSubmitted() const {
	return !done;
}

/*
========================
idParallelJobList_Threads::GetTotalProcessingTimeMicroSec
========================
*/
uint64_t idParallelJobList_Threads::GetTotalProcessingTimeMicroSec() const {
	uint64_t total = 0;
	for ( int unit = 0; unit < MAX_THREADS; unit++ ) {
		total += threadStats.threadExecTime[unit];
	}
	return total;
}

/*
========================
idParallelJobList_Threads::GetTotalWastedTimeMicroSec
========================
*/
uint64_t idParallelJobList_Threads::GetTotalWastedTimeMicroSec() const {
	uint64_t total = 0;
	for ( int unit = 0; unit < MAX_THREADS; unit++ ) {
		total += threadStats.threadTotalTime[unit] - threadStats.threadExecTime[unit];
	}
	return total;
}

/*
========================
idParallelJobList_Threads::GetUnitProcessingTimeMicroSec
========================
*/
uint64_t idParallelJobList_Threads::GetUnitProcessingTimeMicroSec( int unit ) const {
	if ( unit < 0 || unit >= MAX_THREADS ) {
		return 0;
	}
	return threadStats.threadExecTime[unit];
}

/*
========================
idParallelJobList_Threads::GetUnitWastedTimeMicroSec
========================
*/
uint64_t idParallelJobList_Threads::GetUnitWastedTimeMicroSec( int unit ) const {
	if ( unit < 0 || unit >= MAX_THREADS ) {
		return 0;
	}
	return threadStats.threadTotalTime[unit] - threadStats.threadExecTime[unit];
}

#ifndef _DEBUG
volatile float longJobTime;
volatile jobRun_t longJobFunc;
volatile void * longJobData;
#endif

/*
========================
idParallelJobList_Threads::RunJobsInternal
========================
*/
int idParallelJobList_Threads::RunJobsInternal( unsigned int threadNum, threadJobListState_t & state, bool singleJob ) {
	if ( state.version != version.GetValue() ) {
		// trying to run an old version of this list that is already done
		return RUN_DONE;
	}

	assert( threadNum < MAX_THREADS );

	if ( deferredThreadStats.startTime == 0 ) {
		deferredThreadStats.startTime = Sys_Microseconds();	// first time any thread is running jobs from this list
	}

	int result = RUN_OK;

	do {

		// run through all signals and syncs before the last job that has been or is being executed
		// this loop is really an optimization to minimize the time spent in the fetchLock section below
		for ( ; state.lastJobIndex < (int) currentJob.GetValue() && state.lastJobIndex < jobList.Num(); state.lastJobIndex++ ) {
			if ( jobList[state.lastJobIndex].data == & JOB_SIGNAL ) {
				state.signalIndex++;
				assert( state.signalIndex < signalJobCount.Num() );
			} else if ( jobList[state.lastJobIndex].data == & JOB_SYNCHRONIZE ) {
				assert( state.signalIndex > 0 );
				if ( signalJobCount[state.signalIndex - 1].GetValue() > 0 ) {
					// stalled on a synchronization point
					return ( result | RUN_STALLED );
				}
			} else if ( jobList[state.lastJobIndex].data == & JOB_LIST_DONE ) {
				if ( signalJobCount[signalJobCount.Num() - 1].GetValue() > 0 ) {
					// stalled on a synchronization point
					return ( result | RUN_STALLED );
				}
			}
		}

		// try to lock to fetch a new job
		if ( fetchLock.Increment() == 1 ) {

			// grab a new job
			state.nextJobIndex = currentJob.Increment() - 1;

			// run through any remaining signals and syncs (this should rarely iterate more than once)
			for ( ; state.lastJobIndex <= state.nextJobIndex && state.lastJobIndex < jobList.Num(); state.lastJobIndex++ ) {
				if ( jobList[state.lastJobIndex].data == & JOB_SIGNAL ) {
					state.signalIndex++;
					assert( state.signalIndex < signalJobCount.Num() );
				} else if ( jobList[state.lastJobIndex].data == & JOB_SYNCHRONIZE ) {
					assert( state.signalIndex > 0 );
					if ( signalJobCount[state.signalIndex - 1].GetValue() > 0 ) {
						// return this job to the list
						currentJob.Decrement();
						// release the fetch lock
						fetchLock.Decrement();
						// stalled on a synchronization point
						return ( result | RUN_STALLED );
					}
				} else if ( jobList[state.lastJobIndex].data == & JOB_LIST_DONE ) {
					if ( signalJobCount[signalJobCount.Num() - 1].GetValue() > 0 ) {
						// return this job to the list
						currentJob.Decrement();
						// release the fetch lock
						fetchLock.Decrement();
						// stalled on a synchronization point
						return ( result | RUN_STALLED );
					}
					// decrement the done count
					doneGuards[currentDoneGuard].Decrement();
				}
			}
			// release the fetch lock
			fetchLock.Decrement();
		} else {
			// release the fetch lock
			fetchLock.Decrement();
			// another thread is fetching right now so consider stalled
			return ( result | RUN_STALLED );
		}

		// if at the end of the job list we're done
		if ( state.nextJobIndex >= jobList.Num() ) {
			return ( result | RUN_DONE );
		}

		// execute the next job
		{
			uint64_t jobStart = Sys_Microseconds();

			jobList[state.nextJobIndex].function( jobList[state.nextJobIndex].data );
			jobList[state.nextJobIndex].executed = 1;

			uint64_t jobEnd = Sys_Microseconds();
			deferredThreadStats.threadExecTime[threadNum] += jobEnd - jobStart;

#ifndef _DEBUG
			if ( jobs_longJobMicroSec.GetInteger() > 0 ) {
				if ( jobEnd - jobStart > jobs_longJobMicroSec.GetInteger()
					&& GetId() != JOBLIST_UTILITY ) {
					longJobTime = ( jobEnd - jobStart ) * ( 1.0f / 1000.0f );
					longJobFunc = jobList[state.nextJobIndex].function;
					longJobData = jobList[state.nextJobIndex].data;
					const char * jobName = GetJobName( jobList[state.nextJobIndex].function );
					const char * jobListName = GetJobListName( GetId() );
					common->Printf( "%1.1f milliseconds for a single '%s' job from job list %s on thread %d\n", longJobTime, jobName, jobListName, threadNum );
				}
			}
#endif
		}

		result |= RUN_PROGRESS;

		// decrease the job count for the current signal
		if ( signalJobCount[state.signalIndex].Decrement() == 0 ) {
			// if this was the very last job of the job list
			if ( state.signalIndex == signalJobCount.Num() - 1 ) {
				deferredThreadStats.endTime = Sys_Microseconds();
				return ( result | RUN_DONE );
			}
		}

	} while( ! singleJob );

	return result;
}

/*
========================
idParallelJobList_Threads::RunJobs
========================
*/
int idParallelJobList_Threads::RunJobs( unsigned int threadNum, threadJobListState_t & state, bool singleJob ) {
	uint64_t start = Sys_Microseconds();

	numThreadsExecuting.Increment();

	int result = RunJobsInternal( threadNum, state, singleJob );

	numThreadsExecuting.Decrement();

	deferredThreadStats.threadTotalTime[threadNum] += Sys_Microseconds() - start;

	return result;
}

/*
========================
idParallelJobList_Threads::WaitForOtherJobList
========================
*/
bool idParallelJobList_Threads::WaitForOtherJobList() {
	if ( waitForGuard != NULL ) {
		if ( waitForGuard->GetValue() > 0 ) {
			return true;
		}
	}
	return false;
}

/*
================================================================================================

idParallelJobListLocal

================================================================================================
*/

/*
========================
idParallelJobListLocal::idParallelJobListLocal
========================
*/
idParallelJobListLocal::idParallelJobListLocal( jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs, const idColor * color ) {
	assert( priority > JOBLIST_PRIORITY_NONE );
	this->jobListThreads = new idParallelJobList_Threads( id, priority, maxJobs, maxSyncs );
	this->color = color;
}

/*
========================
idParallelJobListLocal::~idParallelJobListLocal
========================
*/
idParallelJobListLocal::~idParallelJobListLocal() {
	delete jobListThreads;
}

/*
========================
idParallelJobListLocal::AddJob
========================
*/
void idParallelJobListLocal::AddJob( jobRun_t function, void * data ) {
	assert( IsRegisteredJob( function ) );
	jobListThreads->AddJob( function, data );
}

/*
========================
idParallelJobListLocal::AddJobSPURS
========================
*/
CellSpursJob128 * idParallelJobListLocal::AddJobSPURS() {
	return NULL;
}

/*
========================
idParallelJobListLocal::InsertSyncPoint
========================
*/
void idParallelJobListLocal::InsertSyncPoint( jobSyncType_t syncType ) {
	jobListThreads->InsertSyncPoint( syncType );
}

/*
========================
idParallelJobListLocal::Wait
========================
*/
void idParallelJobListLocal::Wait() {
	if ( jobListThreads != NULL ) {
		jobListThreads->Wait();
	}
}

/*
========================
idParallelJobListLocal::TryWait
========================
*/
bool idParallelJobListLocal::TryWait() {
	bool done = true;
	if ( jobListThreads != NULL ) {
		done &= jobListThreads->TryWait();
	}
	return done;
}

/*
========================
idParallelJobListLocal::Submit
========================
*/
void idParallelJobListLocal::Submit( idParallelJobList * waitForJobList, int parallelism ) {
	idParallelJobListLocal *waitForJobListLocal = (idParallelJobListLocal *)waitForJobList;

	assert(waitForJobListLocal != this );
	jobListThreads->Submit( (waitForJobListLocal != NULL ) ? waitForJobListLocal->jobListThreads : NULL, parallelism );
} 

/*
========================
idParallelJobListLocal::IsSubmitted
========================
*/
bool idParallelJobListLocal::IsSubmitted() const {
	return jobListThreads->IsSubmitted();
}

/*
========================
idParallelJobListLocal::GetNumExecutedJobs
========================
*/
unsigned int idParallelJobListLocal::GetNumExecutedJobs() const {
	return jobListThreads->GetNumExecutedJobs();
}

/*
========================
idParallelJobListLocal::GetNumSyncs
========================
*/
unsigned int idParallelJobListLocal::GetNumSyncs() const {
	return jobListThreads->GetNumSyncs();
}

/*
========================
idParallelJobListLocal::GetSubmitTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetSubmitTimeMicroSec() const {
	return jobListThreads->GetSubmitTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetStartTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetStartTimeMicroSec() const {
	return jobListThreads->GetStartTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetFinishTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetFinishTimeMicroSec() const {
	return jobListThreads->GetFinishTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetWaitTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetWaitTimeMicroSec() const {
	return jobListThreads->GetWaitTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetTotalProcessingTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetTotalProcessingTimeMicroSec() const {
	return jobListThreads->GetTotalProcessingTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetTotalWastedTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetTotalWastedTimeMicroSec() const {
	return jobListThreads->GetTotalWastedTimeMicroSec();
}

/*
========================
idParallelJobListLocal::GetUnitProcessingTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetUnitProcessingTimeMicroSec( int unit ) const {
	return jobListThreads->GetUnitProcessingTimeMicroSec( unit );
}

/*
========================
idParallelJobListLocal::GetUnitWastedTimeMicroSec
========================
*/
uint64_t idParallelJobListLocal::GetUnitWastedTimeMicroSec( int unit ) const {
	return jobListThreads->GetUnitWastedTimeMicroSec( unit );
}

/*
========================
idParallelJobListLocal::GetId
========================
*/
jobListId_t idParallelJobListLocal::GetId() const {
	return jobListThreads->GetId();
}

/*
========================
idJobThread::idJobThread
========================
*/
idJobThread::idJobThread() :
		firstJobList( 0 ),
		lastJobList( 0 ),
		threadNum( 0 ) {
}

/*
========================
idJobThread::~idJobThread
========================
*/
idJobThread::~idJobThread() {
}

/*
========================
idJobThread::Start
========================
*/
void idJobThread::Start( core_t core, unsigned int threadNum ) {
	this->threadNum = threadNum;
	StartWorkerThread( va( "JobListProcessor_%d", threadNum ), core, THREAD_NORMAL, JOB_THREAD_STACK_SIZE );
}

/*
========================
idJobThread::AddJobList
========================
*/
void idJobThread::AddJobList( idParallelJobList_Threads * jobList ) {
	// must lock because multiple threads may try to add new job lists at the same time
	addJobMutex.Lock();
	// wait until there is space available because in rare cases multiple versions of the same job lists may still be queued 
	while( lastJobList - firstJobList >= MAX_JOBLISTS ) {
		Sys_Yield();
	}
	assert( lastJobList - firstJobList < MAX_JOBLISTS );
	jobLists[lastJobList & ( MAX_JOBLISTS - 1 )].jobList = jobList;
	jobLists[lastJobList & ( MAX_JOBLISTS - 1 )].version = jobList->GetVersion();
	lastJobList++;
	addJobMutex.Unlock();
}

/*
========================
idJobThread::Run
========================
*/
int idJobThread::Run() {
	threadJobListState_t threadJobListState[MAX_JOBLISTS];
	int numJobLists = 0;
	int lastStalledJobList = -1;

	while ( !IsTerminating() ) {

		// fetch any new job lists and add them to the local list
		if ( numJobLists < MAX_JOBLISTS && firstJobList < lastJobList ) {
			threadJobListState[numJobLists].jobList = jobLists[firstJobList & ( MAX_JOBLISTS - 1 )].jobList;
			threadJobListState[numJobLists].version = jobLists[firstJobList & ( MAX_JOBLISTS - 1 )].version;
			threadJobListState[numJobLists].signalIndex = 0;
			threadJobListState[numJobLists].lastJobIndex = 0;
			threadJobListState[numJobLists].nextJobIndex = -1;
			numJobLists++;
			firstJobList++;
		}
		if ( numJobLists == 0 ) {
			break;
		}

		int currentJobList = 0;
		jobListPriority_t priority = JOBLIST_PRIORITY_NONE;
		if ( lastStalledJobList < 0 ) {
			// find the job list with the highest priority
			for ( int i = 0; i < numJobLists; i++ ) {
				if ( threadJobListState[i].jobList->GetPriority() > priority && !threadJobListState[i].jobList->WaitForOtherJobList() ) {
					priority = threadJobListState[i].jobList->GetPriority();
					currentJobList = i;
				}
			}
		} else {
			// try to hide the stall with a job from a list that has equal or higher priority
			currentJobList = lastStalledJobList;
			priority = threadJobListState[lastStalledJobList].jobList->GetPriority();
			for ( int i = 0; i < numJobLists; i++ ) {
				if ( i != lastStalledJobList && threadJobListState[i].jobList->GetPriority() >= priority && !threadJobListState[i].jobList->WaitForOtherJobList() ) {
					priority = threadJobListState[i].jobList->GetPriority();
					currentJobList = i;
				}
			}
		}

		// if the priority is high then try to run through the whole list to reduce the overhead
		// otherwise run a single job and re-evaluate priorities for the next job
		bool singleJob = ( priority == JOBLIST_PRIORITY_HIGH ) ? false : jobs_prioritize.GetBool();

		// try running one or more jobs from the current job list
		int result = threadJobListState[currentJobList].jobList->RunJobs( threadNum, threadJobListState[currentJobList], singleJob );

		if ( ( result & idParallelJobList_Threads::RUN_DONE ) != 0 ) {
			// done with this job list so remove it from the local list
			for ( int i = currentJobList; i < numJobLists - 1; i++ ) {
				threadJobListState[i] = threadJobListState[i + 1];
			}
			numJobLists--;
			lastStalledJobList = -1;
		} else if ( ( result & idParallelJobList_Threads::RUN_STALLED ) != 0 ) {
			// yield when stalled on the same job list again without making any progress
			if ( currentJobList == lastStalledJobList ) {
				if ( ( result & idParallelJobList_Threads::RUN_PROGRESS ) == 0 ) {
					Sys_Yield();
				}
			}
			lastStalledJobList = currentJobList;
		} else {
			lastStalledJobList = -1;
		}
	}
	return 0;
}

/*
================================================================================================

idParallelJobManagerLocal

================================================================================================
*/

extern void Sys_CPUCount( int & logicalNum, int & coreNum, int & packageNum );

idParallelJobManagerLocal parallelJobManagerLocal;
idParallelJobManager * parallelJobManager = &parallelJobManagerLocal;

/*
========================
SubmitJobList
========================
*/
void SubmitJobList( idParallelJobList_Threads * jobList, int parallelism ) {
	parallelJobManagerLocal.Submit( jobList, parallelism );
}

/*
========================
idParallelJobManagerLocal::Init
========================
*/
void idParallelJobManagerLocal::Init() {
	// on consoles this will have specific cores for the threads, but on PC they will all be CORE_ANY
	core_t cores[] = JOB_THREAD_CORES;
	assert( sizeof( cores ) / sizeof( cores[0] ) >= MAX_JOB_THREADS );

	for ( int i = 0; i < MAX_JOB_THREADS; i++ ) {
		threads[i].Start( cores[i], i );
	}
	maxThreads = jobs_numThreads.GetInteger();

	Sys_CPUCount( numPhysicalCpuCores, numLogicalCpuCores, numCpuPackages );
}

/*
========================
idParallelJobManagerLocal::Shutdown
========================
*/
void idParallelJobManagerLocal::Shutdown() {
	for ( int i = 0; i < MAX_JOB_THREADS; i++ ) {
		threads[i].StopThread();
	}
}

/*
========================
idParallelJobManagerLocal::AllocJobList
========================
*/
idParallelJobList * idParallelJobManagerLocal::AllocJobList( jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs, const idColor * color ) {
	for ( int i = 0; i < jobLists.Num(); i++ ) {
		if ( jobLists[i]->GetId() == id ) {
			// idStudio may cause job lists to be allocated multiple times
		}
	}
	idParallelJobListLocal * jobList = new idParallelJobListLocal( id, priority, maxJobs, maxSyncs, color );
	jobLists.Append( jobList );
	return jobList;
}

/*
========================
idParallelJobManagerLocal::FreeJobList
========================
*/
void idParallelJobManagerLocal::FreeJobList( idParallelJobList * jobList ) {
	if ( jobList == NULL ) {
		return;
	}
	// wait for all job threads to finish because job list deletion is not thread safe
	for ( unsigned int i = 0; i < maxThreads; i++ ) {
		threads[i].WaitForThread();
	}
	int index = jobLists.FindIndex( (idParallelJobListLocal *)jobList );
	assert( index >= 0 && jobLists[index] == jobList );
	jobLists[index]->Wait();
	delete jobLists[index];
	jobLists.RemoveIndexFast( index );
}

/*
========================
idParallelJobManagerLocal::GetNumJobLists
========================
*/
int idParallelJobManagerLocal::GetNumJobLists() const {
	return jobLists.Num();
}

/*
========================
idParallelJobManagerLocal::GetNumFreeJobLists
========================
*/
int idParallelJobManagerLocal::GetNumFreeJobLists() const {
	return MAX_JOBLISTS - jobLists.Num();
}

/*
========================
idParallelJobManagerLocal::GetJobList
========================
*/
idParallelJobList * idParallelJobManagerLocal::GetJobList( int index ) {
	return jobLists[index];
}

/*
========================
idParallelJobManagerLocal::GetNumProcessingUnits
========================
*/
int idParallelJobManagerLocal::GetNumProcessingUnits() {
	return maxThreads;
}

/*
========================
idParallelJobManagerLocal::WaitForAllJobLists
========================
*/
void idParallelJobManagerLocal::WaitForAllJobLists() {
	// wait for all job lists to complete
	for ( int i = 0; i < jobLists.Num(); i++ ) {
		jobLists[i]->Wait();
	}
}

/*
========================
idParallelJobManagerLocal::Submit
========================
*/
void idParallelJobManagerLocal::Submit( idParallelJobList_Threads * jobList, int parallelism ) {
	if ( jobs_numThreads.IsModified() ) {
		maxThreads = idMath::ClampInt( 0, MAX_JOB_THREADS, jobs_numThreads.GetInteger() );
		jobs_numThreads.ClearModified();
	}

	// determine the number of threads to use
	int numThreads = maxThreads;
	if ( parallelism == JOBLIST_PARALLELISM_DEFAULT ) {
		numThreads = maxThreads;
	} else if ( parallelism == JOBLIST_PARALLELISM_MAX_CORES ) {
		numThreads = numLogicalCpuCores;
	} else if ( parallelism == JOBLIST_PARALLELISM_MAX_THREADS ) {
		numThreads = MAX_JOB_THREADS;
	} else if ( parallelism > MAX_JOB_THREADS ) {
		numThreads = MAX_JOB_THREADS;
	} else {
		numThreads = parallelism;
	}

	if ( numThreads <= 0 ) {
		threadJobListState_t state( jobList->GetVersion() );
		jobList->RunJobs( 0, state, false );
		return;
	}

	for ( int i = 0; i < numThreads; i++ ) {
		threads[i].AddJobList( jobList );
		threads[i].SignalWork();
	}
}