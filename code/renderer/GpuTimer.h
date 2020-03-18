// GpuTimer.h
//

class rvmGpuTimer {
public:
	rvmGpuTimer();
	~rvmGpuTimer();

	void Start(void);
	void End(void);

	int Length();
private:
	GLuint64 startTime;
	GLuint64 stopTime;
	unsigned int queryID[2];
};