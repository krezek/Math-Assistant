#ifndef _UPLOAD_BUFFER_H_
#define _UPLOAD_BUFFER_H_

#include <d3d12.h>

typedef struct _UploadBuffer
{
    ID3D12Resource* _uploadBuffer;
    BYTE* _mappedData;

    UINT _elementByteSize;
    bool _isConstantBuffer;
} UploadBuffer;

UploadBuffer* UploadBuffer_init(UINT size, ID3D12Device* device, UINT elementCount, bool isConstantBuffer);
void UploadBuffer_free(UploadBuffer* ub);

void UploadBuffer_CopyData(UploadBuffer* ub, int elementIndex, void* data, UINT size);

#endif /* _UPLOAD_BUFFER_H_ */
