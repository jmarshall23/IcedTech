// draw_skin.cpp
//

#include "engine_precompiled.h"
#include "../tr_local.h"

/*
======================
RB_BindJointBuffer
======================
*/
void RB_BindJointBuffer(idJointBuffer *jointBuffer, float * inverseJointPose, int numJoints, void*colorOffset, void*color2Offset) {
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR2);

	glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), colorOffset);
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), color2Offset);

	jointBuffer->Update(inverseJointPose, numJoints);

	const GLuint ubo = reinterpret_cast<GLuint>(jointBuffer->GetAPIObject());
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, jointBuffer->GetOffset(), jointBuffer->GetNumJoints() * sizeof(idJointMat));
}

/*
======================
RB_UnBindJointBuffer
======================
*/
void RB_UnBindJointBuffer(void) {
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR2);
}