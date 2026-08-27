/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"

#include <general.h>
#include <upload_buffer.h>
#include <framerc.h>

FrameResource* FrameResource_init(ID3D12Device* device, UINT passCount, UINT objCount)
{
	FrameResource* fr = (FrameResource*)malloc(sizeof(FrameResource));
	assert(fr);

	memset(fr, 0, sizeof(FrameResource));

	if (passCount != 0)
		fr->_passCB = UploadBuffer_init(sizeof(PassConstants), device, passCount, true);
	
	if (objCount != 0)
		fr->_objectCB = UploadBuffer_init(sizeof(ObjectConstants), device, objCount, true);
	
	return fr;
}

void FrameResource_free(FrameResource* fr)
{
	if (fr->_passCB)
		UploadBuffer_free(fr->_passCB);

	if (fr->_objectCB)
		UploadBuffer_free(fr->_objectCB);

	free(fr);
}
