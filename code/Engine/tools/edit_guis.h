// edit_guis.h
//

//
// rvmToolShowFrameRate
//
class rvmToolShowFrameRate : public rvmToolGui {
public:
	virtual const char* Name(void);
	virtual void Render(void);
};

extern rvmToolShowFrameRate displayFrameRateTool;