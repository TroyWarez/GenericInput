#pragma once
#include "framework.h"

const UINT FrameCount = 2;

using Microsoft::WRL::ComPtr;

class Renderer
{
public:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> sync_fence;

	UINT rtvDescriptorSize;
	UINT frameIndex;
	HANDLE fenceEvent;
	UINT64 fenceValue;

	ComPtr<IDXGIFactory4> factory;
	void InitializeD3D12(HWND hwnd);
	void Render();
	void WaitForPreviousFrame();
	Renderer();
	~Renderer();
};

