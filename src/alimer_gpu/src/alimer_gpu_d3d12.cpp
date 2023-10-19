// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_gpu_internal.h"

#if defined(ALIMER_D3D12)
#ifdef _GAMING_XBOX_SCARLETT
#   pragma warning(push)
#   pragma warning(disable: 5204 5249)
#   include <d3d12_xs.h>
#   pragma warning(pop)
#   include <d3dx12_xs.h>
#elif (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
#   pragma warning(push)
#   pragma warning(disable: 5204)
#   include <d3d12_x.h>
#   pragma warning(pop)
#else
#   include <directx/d3d12.h>
#   include <directx/d3d12video.h>
#   include <directx/d3dx12_resource_helpers.h>
#   include <directx/d3dx12_pipeline_state_stream.h>
#   include <directx/d3dx12_check_feature_support.h>
#   define PPV_ARGS(x) IID_PPV_ARGS(&x)
#endif

#include <dxgi1_6.h>
#include <wrl/client.h>

#if defined(_DEBUG) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   include <dxgidebug.h>
#endif

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include "third_party/D3D12MemAlloc.h"

#include <stdio.h>
#include <string.h>
#include <mutex>
#include <deque>

#define VHR(hr) if (FAILED(hr)) { ALIMER_ASSERT(0); }
#define SAFE_RELEASE(obj) if ((obj)) { (obj)->Release(); (obj) = nullptr; }

using Microsoft::WRL::ComPtr;

namespace
{
    static_assert(sizeof(GPUViewport) == sizeof(D3D12_VIEWPORT), "Viewport mismatch");
    static_assert(offsetof(GPUViewport, x) == offsetof(D3D12_VIEWPORT, TopLeftX), "Layout mismatch");
    static_assert(offsetof(GPUViewport, y) == offsetof(D3D12_VIEWPORT, TopLeftY), "Layout mismatch");
    static_assert(offsetof(GPUViewport, width) == offsetof(D3D12_VIEWPORT, Width), "Layout mismatch");
    static_assert(offsetof(GPUViewport, height) == offsetof(D3D12_VIEWPORT, Height), "Layout mismatch");
    static_assert(offsetof(GPUViewport, minDepth) == offsetof(D3D12_VIEWPORT, MinDepth), "Layout mismatch");
    static_assert(offsetof(GPUViewport, maxDepth) == offsetof(D3D12_VIEWPORT, MaxDepth), "Layout mismatch");

    static_assert(sizeof(GPUDispatchIndirectCommand) == sizeof(D3D12_DISPATCH_ARGUMENTS), "DispatchIndirectCommand mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, x) == offsetof(D3D12_DISPATCH_ARGUMENTS, ThreadGroupCountX), "Layout mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, y) == offsetof(D3D12_DISPATCH_ARGUMENTS, ThreadGroupCountY), "Layout mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, z) == offsetof(D3D12_DISPATCH_ARGUMENTS, ThreadGroupCountZ), "Layout mismatch");

    static_assert(sizeof(GPUDrawIndirectCommand) == sizeof(D3D12_DRAW_ARGUMENTS), "DrawIndirectCommand mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, vertexCount) == offsetof(D3D12_DRAW_ARGUMENTS, VertexCountPerInstance), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, instanceCount) == offsetof(D3D12_DRAW_ARGUMENTS, InstanceCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, firstVertex) == offsetof(D3D12_DRAW_ARGUMENTS, StartVertexLocation), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, firstInstance) == offsetof(D3D12_DRAW_ARGUMENTS, StartInstanceLocation), "Layout mismatch");

    static_assert(sizeof(GPUDrawIndexedIndirectCommand) == sizeof(D3D12_DRAW_INDEXED_ARGUMENTS), "DrawIndexedIndirectCommand mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, indexCount) == offsetof(D3D12_DRAW_INDEXED_ARGUMENTS, IndexCountPerInstance), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, instanceCount) == offsetof(D3D12_DRAW_INDEXED_ARGUMENTS, InstanceCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, firstIndex) == offsetof(D3D12_DRAW_INDEXED_ARGUMENTS, StartIndexLocation), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, baseVertex) == offsetof(D3D12_DRAW_INDEXED_ARGUMENTS, BaseVertexLocation), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, firstInstance) == offsetof(D3D12_DRAW_INDEXED_ARGUMENTS, StartInstanceLocation), "Layout mismatch");

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) 
    using PFN_CREATE_DXGI_FACTORY2 = decltype(&CreateDXGIFactory2);

    PFN_CREATE_DXGI_FACTORY2 Alimer_CreateDXGIFactory2 = nullptr;

    PFN_D3D12_CREATE_DEVICE Alimer_D3D12CreateDevice = nullptr;
    PFN_D3D12_GET_DEBUG_INTERFACE Alimer_D3D12GetDebugInterface = nullptr;
    PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE Alimer_D3D12SerializeVersionedRootSignature = nullptr;

#if defined(_DEBUG)
    using PFN_DXGI_GET_DEBUG_INTERFACE1 = decltype(&DXGIGetDebugInterface1);
    PFN_DXGI_GET_DEBUG_INTERFACE1 Alimer_DXGIGetDebugInterface1 = nullptr;

    // Declare debug guids to avoid linking with "dxguid.lib"
    static constexpr IID GPU_DXGI_DEBUG_ALL = { 0xe48ae283, 0xda80, 0x490b, {0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8} };
    static constexpr IID GPU_DXGI_DEBUG_DXGI = { 0x25cddaa4, 0xb1c6, 0x47e1, {0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a} };
#endif
#endif
}

static struct {
    GPUConfig config;

    DWORD dxgiFactoryFlags{ 0 };
    IDXGIFactory6* dxgiFactory = nullptr;
    BOOL tearingSupported{ false };

} state;

static bool D3D12_IsSupported(void)
{
    static bool available_initialized = false;
    static bool available = false;

    if (available_initialized) {
        return available;
    }

    available_initialized = true;

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    HMODULE dxgiDLL = LoadLibraryExW(L"dxgi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE d3d12DLL = LoadLibraryExW(L"d3d12.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (dxgiDLL == nullptr ||
        d3d12DLL == nullptr)
    {
        return false;
    }

    Alimer_CreateDXGIFactory2 = (PFN_CREATE_DXGI_FACTORY2)GetProcAddress(dxgiDLL, "CreateDXGIFactory2");
    if (Alimer_CreateDXGIFactory2 == nullptr)
    {
        return false;
    }

#if defined(_DEBUG)
    Alimer_DXGIGetDebugInterface1 = (PFN_DXGI_GET_DEBUG_INTERFACE1)GetProcAddress(dxgiDLL, "DXGIGetDebugInterface1");
#endif

    Alimer_D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12DLL, "D3D12CreateDevice");
    if (!Alimer_D3D12CreateDevice)
    {
        return false;
    }

    Alimer_D3D12GetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(d3d12DLL, "D3D12GetDebugInterface");
    Alimer_D3D12SerializeVersionedRootSignature = (PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE)GetProcAddress(d3d12DLL, "D3D12SerializeVersionedRootSignature");
    if (!Alimer_D3D12SerializeVersionedRootSignature) {
        return false;
    }
#endif

    ComPtr<IDXGIFactory4> dxgiFactory;
    if (FAILED(Alimer_CreateDXGIFactory2(0, PPV_ARGS(dxgiFactory))))
    {
        return false;
    }

    ComPtr<IDXGIAdapter1> dxgiAdapter;
    bool foundCompatibleDevice = false;
    for (uint32_t i = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(i, dxgiAdapter.ReleaseAndGetAddressOf()); ++i)
    {
        DXGI_ADAPTER_DESC1 adapterDesc;
        VHR(dxgiAdapter->GetDesc1(&adapterDesc));

        // Don't select the Basic Render Driver adapter.
        if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the actual device.
        if (SUCCEEDED(Alimer_D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
        {
            foundCompatibleDevice = true;
            break;
        }
    }

    if (foundCompatibleDevice)
    {
        available = true;
        return true;
    }

    available = true;
    return true;
}

static bool D3D12_Init(const GPUConfig* config)
{
    state.config = *config;

    if (config->validationMode != GPUValidationMode_Disabled)
    {
        state.dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(Alimer_D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();

            if (config->validationMode == GPUValidationMode_GPU)
            {
                ComPtr<ID3D12Debug1> debugController1;
                if (SUCCEEDED(debugController.As(&debugController1)))
                {
                    debugController1->SetEnableGPUBasedValidation(TRUE);
                    debugController1->SetEnableSynchronizedCommandQueueValidation(TRUE);
                }

                ID3D12Debug2* debugController2;
                if (SUCCEEDED(debugController->QueryInterface(&debugController2)))
                {
                    const bool g_D3D12DebugLayer_GPUBasedValidation_StateTracking_Enabled = true;

                    if (g_D3D12DebugLayer_GPUBasedValidation_StateTracking_Enabled)
                        debugController2->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_DISABLE_STATE_TRACKING);
                    else
                        debugController2->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);
                    debugController2->Release();
                }
            }

            // DRED
            ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> pDredSettings;
            if (SUCCEEDED(Alimer_D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings))))
            {
                // Turn on auto-breadcrumbs and page fault reporting.
                pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
                pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
                pDredSettings->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
            }

#if defined(_DEBUG) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
            IDXGIInfoQueue* dxgiInfoQueue = nullptr;
            if (SUCCEEDED(Alimer_DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
            {
                state.dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

                dxgiInfoQueue->SetBreakOnSeverity(GPU_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
                dxgiInfoQueue->SetBreakOnSeverity(GPU_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

                DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
                {
                    80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
                };
                DXGI_INFO_QUEUE_FILTER filter = {};
                filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
                filter.DenyList.pIDList = hide;
                dxgiInfoQueue->AddStorageFilterEntries(GPU_DXGI_DEBUG_DXGI, &filter);
                dxgiInfoQueue->Release();
            }
#endif
        }
        else
        {
            OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
        }
    }

    // Create factory and determines whether tearing support is available for fullscreen borderless windows.
    {
        VHR(Alimer_CreateDXGIFactory2(state.dxgiFactoryFlags, IID_PPV_ARGS(&state.dxgiFactory)));

        state.tearingSupported = TRUE;
        if (FAILED(state.dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &state.tearingSupported, sizeof(state.tearingSupported))))
        {
            state.tearingSupported = FALSE;
        }
    }

    return true;
}

static void D3D12_Shutdown()
{
    SAFE_RELEASE(state.dxgiFactory);

#if defined(_DEBUG) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    {
        IDXGIDebug1* dxgiDebug;
        if (SUCCEEDED(Alimer_DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        {
            dxgiDebug->ReportLiveObjects(GPU_DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
            dxgiDebug->Release();
        }
    }
#endif

    memset(&state, 0, sizeof(state));
}

static bool D3D12_InitSurface(GPUSurface surface)
{
    return true;
}

static GPUDevice D3D12_CreateDevice(/*const VGPUDeviceDescriptor* info*/)
{
    return nullptr;
}

GPUDriver D3D12_Driver = {
    GPUBackendType_D3D12,
    D3D12_IsSupported,
    D3D12_Init,
    D3D12_Shutdown,
    D3D12_InitSurface,
    D3D12_CreateDevice
};

#undef VHR
#undef SAFE_RELEASE

#endif /* ALIMER_D3D12 */
