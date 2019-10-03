// draw_occlusion.cpp
//

#include "precompiled.h"
#include "tr_local.h"

enum rvnmOcclusionQueryState
{
	OCCLUSION_QUERY_STATE_HIDDEN,
	OCCLUSION_QUERY_STATE_VISIBLE,
	OCCLUSION_QUERY_STATE_WAITING
};

class rmvOcclusionQuery {
public:
	rmvOcclusionQuery()
	{
		glGenQueries(1, &id);
		queryState = OCCLUSION_QUERY_STATE_HIDDEN;
	}

	void RunQuery(viewLight_s *light)
	{
		if (queryState != OCCLUSION_QUERY_STATE_WAITING)
		{
			queryState = OCCLUSION_QUERY_STATE_WAITING;

			glBeginQuery(GL_ANY_SAMPLES_PASSED, id);
			RB_DrawElementsImmediate(light->frustumTris);
			glEndQuery(GL_ANY_SAMPLES_PASSED);
		}
	}
private:
	GLuint id;
	rvnmOcclusionQueryState queryState;
};

void RB_Draw_LightOcclusion(void) {
	viewLight_t		*vLight;

	for (vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next) {

	}
}