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
private:
	int captureSize;
};