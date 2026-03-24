#include <windows.h>
#include <d2d1.h>
#include <wrl/client.h>
#include <Xinput.h> // 1. Include XInput

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Xinput.lib") // 2. Link XInput

using Microsoft::WRL::ComPtr;

class D2DApp {
public:
	D2DApp() : m_hwnd(nullptr), m_posX(400.0f), m_posY(300.0f) {}

	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = D2DApp::WndProc;
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"D2DAppClass";
		wcex.cbWndExtra = sizeof(LONG_PTR);

		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(L"D2DAppClass", L"Direct2D + XInput", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, this);

		if (!m_hwnd) return E_FAIL;

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, &m_pFactory);

		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, nCmdShow);
			UpdateWindow(m_hwnd);
		}
		return hr;
	}

	// 3. Updated Loop: Uses PeekMessage so we can poll the controller every frame
	void RunMessageLoop() {
		MSG msg = { 0 };
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				UpdateController(); // Check XInput
				OnRender();         // Draw
			}
		}
	}

private:
	HWND m_hwnd;
	ComPtr<ID2D1Factory> m_pFactory;
	ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	ComPtr<ID2D1SolidColorBrush> m_pBrush;

	// Position for our rectangle
	float m_posX;
	float m_posY;

	// 4. Polling the Controller
	void UpdateController() {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Get state of controller 0
		DWORD dwResult = XInputGetState(0, &state);

		if (dwResult == ERROR_SUCCESS) {
			// Left Stick X/Y are between -32768 and 32767
			float stickX = state.Gamepad.sThumbLX;
			float stickY = state.Gamepad.sThumbLY;

			// Simple Deadzone (ignore small movements)
			float deadzone = 8000;
			float speed = 0.005f;

		}
	}

	HRESULT CreateDeviceResources() {
		if (!m_pRenderTarget) {
			RECT rc;
			GetClientRect(m_hwnd, &rc);
			// Cast to UINT32 explicitly
			D2D1_SIZE_U size = D2D1::SizeU(
				static_cast<UINT32>(rc.right - rc.left),
				static_cast<UINT32>(rc.bottom - rc.top)
			);
			HRESULT hr = m_pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);
			if (SUCCEEDED(hr)) {
				m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &m_pBrush);
			}
			return hr;
		}
		return S_OK;
	}

	void OnRender() {
		if (SUCCEEDED(CreateDeviceResources())) {
			m_pRenderTarget->BeginDraw();

			// Use the Enum explicitly to avoid ambiguity
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			// 1. Use .0f on all numbers
			// 2. Pass the rect DIRECTLY (no &) to use the helper overload
			m_pRenderTarget->FillRectangle(
				D2D1::RectF(m_posX - 25.0f, m_posY - 25.0f, m_posX + 25.0f, m_posY + 25.0f),
				m_pBrush.Get()
			);

			if (m_pRenderTarget->EndDraw() == D2DERR_RECREATE_TARGET) {
				m_pRenderTarget.Reset();
				m_pBrush.Reset();
			}
		}
	}

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		if (message == WM_CREATE) {
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			D2DApp* pApp = (D2DApp*)pcs->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pApp);
			return 0;
		}
		D2DApp* pApp = (D2DApp*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (pApp) {
			switch (message) {
			case WM_SIZE:
				if (pApp->m_pRenderTarget) {
					UINT32 width = static_cast<UINT32>(LOWORD(lParam));
					UINT32 height = static_cast<UINT32>(HIWORD(lParam));
					pApp->m_pRenderTarget->Resize(D2D1::SizeU(width, height));
				}
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 1;
			}
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
};

int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int n) {
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	D2DApp app;
	if (SUCCEEDED(app.Initialize(h, n))) {
		app.RunMessageLoop();
	}
	CoUninitialize();
	return 0;
}