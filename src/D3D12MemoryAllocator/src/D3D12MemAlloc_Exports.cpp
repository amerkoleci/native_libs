// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.
#include "D3D12MemAlloc.h"
using namespace D3D12MA;

#define EXPORT extern "C" __declspec(dllexport)

/* Allocator */
EXPORT HRESULT CreateAllocator(const ALLOCATOR_DESC* pDesc, Allocator** ppAllocator)
{
    return D3D12MA::CreateAllocator(pDesc, ppAllocator);
}

EXPORT ULONG Allocator_AddRef(Allocator* allocator)
{
    return allocator->AddRef();
}

EXPORT ULONG Allocator_Release(Allocator* allocator)
{
    return allocator->Release();
}

EXPORT BOOL Allocator_IsUMA(Allocator* allocator)
{
    return allocator->IsUMA();
}

EXPORT BOOL Allocator_IsCacheCoherentUMA(Allocator* allocator)
{
    return allocator->IsCacheCoherentUMA();
}

EXPORT uint64_t Allocator_GetMemoryCapacity(Allocator* allocator, uint32_t memorySegmentGroup)
{
    return allocator->GetMemoryCapacity(memorySegmentGroup);
}

EXPORT HRESULT Allocator_CreateResource(Allocator* allocator,
    const ALLOCATION_DESC* pAllocDesc, const D3D12_RESOURCE_DESC* pResourceDesc,
    D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    Allocation** ppAllocation, REFIID riidResource, void** ppvResource)
{
    return allocator->CreateResource(pAllocDesc, pResourceDesc, InitialResourceState, pOptimizedClearValue, ppAllocation, riidResource, ppvResource);
}

#ifdef __ID3D12Device8_INTERFACE_DEFINED__
EXPORT HRESULT Allocator_CreateResource2(Allocator* allocator,
    const ALLOCATION_DESC* pAllocDesc, const D3D12_RESOURCE_DESC1* pResourceDesc,
    D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    Allocation** ppAllocation, REFIID riidResource, void** ppvResource)
{
    return allocator->CreateResource2(pAllocDesc, pResourceDesc, InitialResourceState, pOptimizedClearValue, ppAllocation, riidResource, ppvResource);
}
#endif // #ifdef __ID3D12Device8_INTERFACE_DEFINED__

#ifdef __ID3D12Device10_INTERFACE_DEFINED__
EXPORT HRESULT Allocator_CreateResource3(Allocator* allocator,
    const ALLOCATION_DESC* pAllocDesc, const D3D12_RESOURCE_DESC1* pResourceDesc,
    D3D12_BARRIER_LAYOUT InitialLayout,
    const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    UINT32 NumCastableFormats, DXGI_FORMAT* pCastableFormats,
    Allocation** ppAllocation, REFIID riidResource, void** ppvResource)
{
    return allocator->CreateResource3(pAllocDesc, pResourceDesc, InitialLayout, pOptimizedClearValue,
        NumCastableFormats, pCastableFormats,
        ppAllocation, riidResource, ppvResource);
}
#endif // #ifdef __ID3D12Device10_INTERFACE_DEFINED__

EXPORT HRESULT Allocator_AllocateMemory(Allocator* allocator,
    const ALLOCATION_DESC* pAllocDesc, const D3D12_RESOURCE_ALLOCATION_INFO* pAllocInfo, Allocation** ppAllocation)
{
    return allocator->AllocateMemory(pAllocDesc, pAllocInfo, ppAllocation);
}

EXPORT HRESULT Allocator_CreateAliasingResource1(Allocator* allocator,
    Allocation* pAllocation,
    UINT64 AllocationLocalOffset,
    const D3D12_RESOURCE_DESC1* pResourceDesc,
    D3D12_RESOURCE_STATES InitialResourceState,
    const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    REFIID riidResource, void** ppvResource)
{
    return allocator->CreateAliasingResource1(pAllocation, AllocationLocalOffset, pResourceDesc, InitialResourceState, pOptimizedClearValue, riidResource, ppvResource);
}

#ifdef __ID3D12Device10_INTERFACE_DEFINED__
EXPORT HRESULT Allocator_CreateAliasingResource2(Allocator* allocator,
    Allocation* pAllocation,
    UINT64 AllocationLocalOffset,
    const D3D12_RESOURCE_DESC1* pResourceDesc,
    D3D12_BARRIER_LAYOUT InitialLayout,
    const D3D12_CLEAR_VALUE* pOptimizedClearValue,
    UINT32 NumCastableFormats, DXGI_FORMAT* pCastableFormats,
    REFIID riidResource, void** ppvResource)
{
    return allocator->CreateAliasingResource2(pAllocation, AllocationLocalOffset, pResourceDesc, InitialLayout, pOptimizedClearValue,
        NumCastableFormats, pCastableFormats,
        riidResource, ppvResource);
}
#endif  // #ifdef __ID3D12Device10_INTERFACE_DEFINED__

EXPORT HRESULT Allocator_CreatePool(Allocator* allocator, const POOL_DESC* pPoolDesc, Pool** ppPool)
{
    return allocator->CreatePool(pPoolDesc, ppPool);
}

EXPORT void Allocator_SetCurrentFrameIndex(Allocator* allocator, uint32_t frameIndex)
{
    allocator->SetCurrentFrameIndex(frameIndex);
}

EXPORT void Allocator_GetBudget(Allocator* allocator, Budget* pLocalBudget, Budget* pNonLocalBudget)
{
    allocator->GetBudget(pLocalBudget, pNonLocalBudget);
}

EXPORT void Allocator_CalculateStatistics(Allocator* allocator, TotalStatistics* pStats)
{
    allocator->CalculateStatistics(pStats);
}

/* Pool */
EXPORT ULONG Pool_AddRef(Pool* pool)
{
    return pool->AddRef();
}

EXPORT ULONG Pool_Release(Pool* pool)
{
    return pool->Release();
}

EXPORT void Pool_GetDesc(Pool* pool, POOL_DESC* pDesc)
{
    *pDesc = pool->GetDesc();
}

EXPORT void Pool_GetStatistics(Pool* pool, Statistics* pStats)
{
    pool->GetStatistics(pStats);
}

EXPORT void Pool_CalculateStatistics(Pool* pool, DetailedStatistics* pStats)
{
    pool->CalculateStatistics(pStats);
}

/* VirtualBlock */
EXPORT HRESULT CreateVirtualBlock(const VIRTUAL_BLOCK_DESC* pDesc, VirtualBlock** ppVirtualBlock)
{
    return D3D12MA::CreateVirtualBlock(pDesc, ppVirtualBlock);
}

EXPORT ULONG VirtualBlock_AddRef(VirtualBlock* block)
{
    return block->AddRef();
}

EXPORT ULONG VirtualBlock_Release(VirtualBlock* block)
{
    return block->Release();
}

EXPORT void VirtualBlock_Clear(VirtualBlock* block)
{
    block->Clear();
}

EXPORT HRESULT VirtualBlock_Allocate(VirtualBlock* block, VIRTUAL_ALLOCATION_DESC* pDesc, VirtualAllocation* pAllocation, UINT64* pOffset)
{
    return block->Allocate(pDesc, pAllocation, pOffset);
}

EXPORT void VirtualBlock_FreeAllocation(VirtualBlock* block, VirtualAllocation* pAllocation)
{
    block->FreeAllocation(*pAllocation);
}

EXPORT BOOL VirtualBlock_IsEmpty(VirtualBlock* block)
{
    return block->IsEmpty();
}

EXPORT void VirtualBlock_GetAllocationInfo(VirtualBlock* block, VirtualAllocation* pAllocation, VIRTUAL_ALLOCATION_INFO* pAllocationInfo)
{
    block->GetAllocationInfo(*pAllocation, pAllocationInfo);
}
