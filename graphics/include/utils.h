#ifndef _UTILS_H_
#define _UTILS_H_

#define ThrowIfFailed(x)        \
{                               \
    HRESULT hr = (x);           \
    if (hr != S_OK)             \
    {                           \
        CHAR buf[1024];         \
        sprintf_s(buf, 1024, "%s %d", __FILE__, __LINE__);              \
        MessageBoxA(NULL, buf, "Error", MB_OK | MB_ICONERROR);          \
        ExitProcess(-1);                                                \
    }                                                                   \
}

inline UINT Utils_CalcConstantBufferByteSize(UINT byteSize)
{
    return (byteSize + 255) & ~255;
}

inline ID3DBlob* LoadBinary(const wchar_t* filename)
{
    ID3DBlob* blob = NULL;
    FILE* fin;

    errno_t e = _wfopen_s(&fin, filename, L"rb");
    if (fin == NULL)
    {
        return NULL;
    }

    fseek(fin, 0, SEEK_END);
    long size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    ThrowIfFailed(D3DCreateBlob(size, &blob));
    size_t r = fread(blob->lpVtbl->GetBufferPointer(blob), size, 1, fin);

    fclose(fin);
    return blob;
}

inline void Utils_MemcpySubresource(
    _In_ const D3D12_MEMCPY_DEST* pDest,
    _In_ const D3D12_SUBRESOURCE_DATA* pSrc,
    SIZE_T RowSizeInBytes,
    UINT NumRows,
    UINT NumSlices)
{
    for (UINT z = 0; z < NumSlices; ++z)
    {
        BYTE* pDestSlice = (BYTE*)(pDest->pData) + pDest->SlicePitch * z;
        const BYTE* pSrcSlice = (const BYTE*)(pSrc->pData) + pSrc->SlicePitch * z;
        for (UINT y = 0; y < NumRows; ++y)
        {
            memcpy(pDestSlice + pDest->RowPitch * y,
                pSrcSlice + pSrc->RowPitch * y,
                RowSizeInBytes);
        }
    }
}

inline UINT64 Utils_UpdateSubresources_1(
    ID3D12GraphicsCommandList* pCmdList,
    ID3D12Resource* pDestinationResource,
    ID3D12Resource* pIntermediate,
    UINT FirstSubresource,
    UINT NumSubresources,
    UINT64 RequiredSize,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
    const UINT* pNumRows,
    const UINT64* pRowSizesInBytes,
    const D3D12_SUBRESOURCE_DATA* pSrcData)
{
    // Minor validation
    D3D12_RESOURCE_DESC IntermediateDesc, DestinationDesc;
    pIntermediate->lpVtbl->GetDesc(pIntermediate, &IntermediateDesc);
    pDestinationResource->lpVtbl->GetDesc(pDestinationResource, &DestinationDesc);
    if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
        IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset ||
        RequiredSize >(size_t)(-1) ||
        (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
            (FirstSubresource != 0 || NumSubresources != 1)))
    {
        return 0;
    }

    BYTE* pData;
    HRESULT hr = pIntermediate->lpVtbl->Map(pIntermediate, 0, NULL, &pData);
    if (FAILED(hr))
    {
        return 0;
    }

    for (UINT i = 0; i < NumSubresources; ++i)
    {
        if (pRowSizesInBytes[i] > (size_t)(-1)) return 0;
        D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, (size_t)(pLayouts[i].Footprint.RowPitch) * (size_t)(pNumRows[i]) };
        Utils_MemcpySubresource(&DestData, &pSrcData[i], (size_t)(pRowSizesInBytes[i]), pNumRows[i], pLayouts[i].Footprint.Depth);
    }
    pIntermediate->lpVtbl->Unmap(pIntermediate, 0, NULL);

    if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    {
        pCmdList->lpVtbl->CopyBufferRegion(pCmdList,
            pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
    }
    else
    {
        for (UINT i = 0; i < NumSubresources; ++i)
        {
            D3D12_TEXTURE_COPY_LOCATION Dst = { pDestinationResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, i + FirstSubresource };
            D3D12_TEXTURE_COPY_LOCATION Src = { pIntermediate, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, pLayouts[i] };
            pCmdList->lpVtbl->CopyTextureRegion(pCmdList, &Dst, 0, 0, 0, &Src, NULL);
        }
    }
    return RequiredSize;
}

inline UINT64 Utils_UpdateSubresources(
    ID3D12GraphicsCommandList* pCmdList,
    ID3D12Resource* pDestinationResource,
    ID3D12Resource* pIntermediate,
    UINT64 IntermediateOffset,
    UINT FirstSubresource,
    UINT NumSubresources,
    D3D12_SUBRESOURCE_DATA* pSrcData)
{
    UINT64 RequiredSize = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[1];
    UINT NumRows[1];
    UINT64 RowSizesInBytes[1];

    D3D12_RESOURCE_DESC Desc;
    pDestinationResource->lpVtbl->GetDesc(pDestinationResource, &Desc);
    ID3D12Device* pDevice = NULL;
    pDestinationResource->lpVtbl->GetDevice(pDestinationResource, &IID_ID3D12Device, &pDevice);
    pDevice->lpVtbl->GetCopyableFootprints(pDevice, &Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
    pDevice->lpVtbl->Release(pDevice);

    return Utils_UpdateSubresources_1(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, Layouts, NumRows, RowSizesInBytes, pSrcData);
}

inline ID3D12Resource* Utils_CreateDefaultBuffer(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* initData,
    const UINT64 byteSize,
    ID3D12Resource ** defaultBuffer)
{
    D3D12_HEAP_PROPERTIES heapProperties1 =
    {
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        D3D12_MEMORY_POOL_UNKNOWN,
        1,
        1
    };

    D3D12_RESOURCE_DESC bufferDesc1 =
    {
        D3D12_RESOURCE_DIMENSION_BUFFER,			// type
        0,											// alignment
        byteSize,                               	// size in bytes
        1,											// height
        1,											// depthOrArraySize
        1,											// mip levels
        DXGI_FORMAT_UNKNOWN,						// format
        { 1, 0 },									// sample description
        D3D12_TEXTURE_LAYOUT_ROW_MAJOR,				// layout
        D3D12_RESOURCE_FLAG_NONE					// flags
    };

    ThrowIfFailed(device->lpVtbl->CreateCommittedResource(device,
        &heapProperties1,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc1,
        D3D12_RESOURCE_STATE_COMMON,
        NULL,
        &IID_ID3D12Resource, defaultBuffer));

    ID3D12Resource* uploadBuffer;

    D3D12_HEAP_PROPERTIES heapProperties2 =
    {
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        D3D12_MEMORY_POOL_UNKNOWN,
        1,
        1
    };

    D3D12_RESOURCE_DESC bufferDesc2 =
    {
        D3D12_RESOURCE_DIMENSION_BUFFER,			// type
        0,											// alignment
        byteSize,                               	// size in bytes
        1,											// height
        1,											// depthOrArraySize
        1,											// mip levels
        DXGI_FORMAT_UNKNOWN,						// format
        { 1, 0 },									// sample description
        D3D12_TEXTURE_LAYOUT_ROW_MAJOR,				// layout
        D3D12_RESOURCE_FLAG_NONE					// flags
    };

    ThrowIfFailed(device->lpVtbl->CreateCommittedResource(device,
        &heapProperties2,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc2,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,    // Clear value
        (REFIID)&IID_ID3D12Resource, (LPVOID*)(&uploadBuffer)));

    D3D12_SUBRESOURCE_DATA subResourceData = { 0 };
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    D3D12_RESOURCE_BARRIER barrier1 =
    {
        D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        D3D12_RESOURCE_BARRIER_FLAG_NONE,
        { *defaultBuffer, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST }
    };
    cmdList->lpVtbl->ResourceBarrier(cmdList, 1, &barrier1);
    

    Utils_UpdateSubresources(cmdList, *defaultBuffer, uploadBuffer, 0, 0, 1, &subResourceData);

    D3D12_RESOURCE_BARRIER barrier2 =
    {
        D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        D3D12_RESOURCE_BARRIER_FLAG_NONE,
        { *defaultBuffer, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ }
    };
    cmdList->lpVtbl->ResourceBarrier(cmdList, 1, &barrier2);
    
    return uploadBuffer;
}


#endif /* _UTILS_H_ */
