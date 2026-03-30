#include "Renderer.h"
#include <d3d12.h>
Renderer::Renderer()
{
	rtvDescriptorSize = 0;
	frameIndex = 0;
	fenceEvent = nullptr;
	fenceValue = 0;

}
Renderer::~Renderer()
{
	if (fenceEvent)
	{
		CloseHandle(fenceEvent);
	}
}
void Renderer::InitializeD3D12(HWND hwnd)
{
	if (IsWindow(hwnd) && hwnd != nullptr)
	{
		CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device));

		// Create Command Queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

		// Create Swap Chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.Width = WND_WIDTH;
		swapChainDesc.Height = WND_HEIGHT;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain1;
		factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
		HRESULT hr = swapChain1.As(&swapChain);
		if (FAILED(hr)) {
			// Handle error: This might fail if the OS is older than Windows 10
			MessageBox(hwnd, L"Failed to create swap chain. Ensure you are running on Windows 10 or later.", L"Error", MB_OK | MB_ICONERROR);
		}
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		// Create Descriptor Heaps (For Render Target Views)
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create Frame Resources (RTVs)
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < FrameCount; n++) {
			swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
			device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvDescriptorSize;
		}

		// Create Command Allocator and List
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		commandList->Close(); // Lists are created in recording state; close it for now.

		// Create Synchronization Objects
		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&sync_fence));
		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}
	else
	{
		MessageBox(hwnd, L"Invalid window handle provided for D3D12 initialization.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	
}
void Renderer::Render() {
	// Reset allocator and list
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	// Transition the back buffer from 'Present' to 'Render Target'
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderTargets[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.ptr += (frameIndex * rtvDescriptorSize);

	// Clear the screen (Cornflower Blue)
	const float clearColor[] = { 0.39f, 0.58f, 0.93f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Transition back to 'Present'
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	// Execute the commands
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame
	swapChain->Present(1, 0);

	WaitForPreviousFrame();
}

void Renderer::WaitForPreviousFrame() {
	const UINT64 fence = fenceValue;
	commandQueue->Signal(sync_fence.Get(), fence);
	fenceValue++;

	if (sync_fence->GetCompletedValue() < fence) {
		sync_fence->SetEventOnCompletion(fence, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}