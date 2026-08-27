#ifndef _FRAME_RESOURCE_H_
#define _FRAME_RESOURCE_H_

typedef struct _UploadBuffer UploadBuffer;

typedef struct _FrameResource
{
	UploadBuffer* _passCB;
	UploadBuffer* _objectCB;
} FrameResource;

FrameResource* FrameResource_init(ID3D12Device* device, UINT passCount, UINT objCount);
void FrameResource_free(FrameResource* fr);



#endif /* _FRAME_RESOURCE_H_ */
