#include "pch.h"

#include <upload_buffer.h>
#include <utils.h>

UploadBuffer* UploadBuffer_init(UINT elementByteSize, ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
{
	UploadBuffer* ub = (UploadBuffer*)malloc(sizeof(UploadBuffer));
	assert(ub != NULL);

	ub->_uploadBuffer = NULL;
	ub->_mappedData = NULL;
	ub->_elementByteSize = elementByteSize;
	ub->_isConstantBuffer = isConstantBuffer;
	
	if (isConstantBuffer)
		ub->_elementByteSize = Utils_CalcConstantBufferByteSize(elementByteSize);

	D3D12_HEAP_PROPERTIES heap_prop;
	heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_prop.CreationNodeMask = 1;
	heap_prop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resource_desc;
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resource_desc.Alignment = 0;
	resource_desc.Width = ub->_elementByteSize * elementCount;
	resource_desc.Height = 1;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.MipLevels = 1;
	resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(device->lpVtbl->CreateCommittedResource(
		device,
		&heap_prop,
		D3D12_HEAP_FLAG_NONE,
		&resource_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		&IID_ID3D12Resource, (void**)&ub->_uploadBuffer));

	ThrowIfFailed(ub->_uploadBuffer->lpVtbl->Map(ub->_uploadBuffer, 0, NULL, (void**)&ub->_mappedData));

	return ub;
}

void UploadBuffer_free(UploadBuffer* ub)
{
	if (ub->_uploadBuffer)
	{
		ub->_uploadBuffer->lpVtbl->Unmap(ub->_uploadBuffer, 0, NULL);
		ub->_uploadBuffer->lpVtbl->Release(ub->_uploadBuffer);
	}

	free(ub);
}

void UploadBuffer_CopyData(UploadBuffer* ub, int elementIndex, void* data, UINT size)
{
	memcpy(&ub->_mappedData[elementIndex * ub->_elementByteSize], data, size);
}
