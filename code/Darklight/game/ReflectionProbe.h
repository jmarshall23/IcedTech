// ReflectionProbe.h
//

//
// rvmReflectionProbe
//
class rvmReflectionProbe : public idEntity {
public:
	CLASS_PROTOTYPE(rvmReflectionProbe);

	virtual void Spawn(void);

	void Capture(void);
	CLASS_STATES_PROTOTYPE(rvmReflectionProbe);
private:
	int captureSize;
};