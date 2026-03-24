#include <windows.h>
#include <d2d1.h>
#include <wrl/client.h> // For ComPtr

#pragma comment(lib, "d2d1.lib")

using Microsoft::WRL::ComPtr;

class D2DApp {
public:
	D2DApp() : m_hwnd(nullptr) {}

	// Initialize the window and Direct2D
	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = D2DApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"D2DAppClass";

		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"D2DAppClass", L"Direct2D Boilerplate",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
			NULL, NULL, hInstance, this
		);

		if (!m_hwnd) return E_FAIL;

		// Use the 4-argument version to avoid compiler ambiguity
		HRESULT hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory),
			nullptr,
			&m_pFactory
		);

		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, nCmdShow);
			UpdateWindow(m_hwnd);
		}

		return hr;
	}

	// Main Message Loop
	void RunMessageLoop() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

private:
	HWND m_hwnd;
	ComPtr<ID2D1Factory> m_pFactory;
	ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	ComPtr<ID2D1SolidColorBrush> m_pBrush;

	// Create resources that are tied to the graphics device (GPU)
	HRESULT CreateDeviceResources() {
		HRESULT hr = S_OK;

		if (!m_pRenderTarget) {
			RECT rc;
			GetClientRect(m_hwnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

			// Create Render Target
			hr = m_pFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_hwnd, size),
				&m_pRenderTarget
			);

			if (SUCCEEDED(hr)) {
				// Create a basic brush (Cornflower Blue)
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
					&m_pBrush
				);
			}
		}
		return hr;
	}

	// Discard resources if the device is lost
	void DiscardDeviceResources() {
		m_pRenderTarget.Reset();
		m_pBrush.Reset();
	}

	// The actual drawing code
	void OnRender() {
		if (SUCCEEDED(CreateDeviceResources())) {
			m_pRenderTarget->BeginDraw();
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			// Draw a rectangle in the middle
			D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
			m_pRenderTarget->FillRectangle(
				D2D1::RectF(rtSize.width / 4, rtSize.height / 4, 3 * rtSize.width / 4, 3 * rtSize.height / 4),
				m_pBrush.Get()
			);

			HRESULT hr = m_pRenderTarget->EndDraw();

			// If the device is lost (e.g. driver update), recreate it next time
			if (hr == D2DERR_RECREATE_TARGET) {
				DiscardDeviceResources();
			}
		}
	}

	void OnResize(UINT width, UINT height) {
		if (m_pRenderTarget) {
			m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		}
	}

	// Standard Win32 Window Procedure
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		D2DApp* pApp = nullptr;

		if (message == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pApp = reinterpret_cast<D2DApp*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
		}
		else {
			pApp = reinterpret_cast<D2DApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (pApp) {
			switch (message) {
			case WM_SIZE:
				pApp->OnResize(LOWORD(lParam), HIWORD(lParam));
				return 0;
			case WM_PAINT:
				pApp->OnRender();
				ValidateRect(hwnd, NULL);
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 1;
			}
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
};

// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		{
			D2DApp app;
			if (SUCCEEDED(app.Initialize(hInstance, nCmdShow))) {
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}