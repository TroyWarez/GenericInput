#include "framework.h"
#include "http.h"
#ifdef WINDOWS
#pragma warning( push )
#pragma warning( disable : 4711)
HINTERNET hSession = nullptr;
HINTERNET hConnect = nullptr;
HINTERNET hRequest = nullptr;

#endif // WINDOWS

#ifdef UNICODE
bool HTTP::DownloadSDLFileW(std::wstring SDL_HOST, std::wstring SDL_PATH, std::string& SDL_Database)
{
	if (hRequest != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringW(L"The request handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hRequest) == FALSE)
		{
			hRequest = nullptr;
#ifdef _DEBUG
			OutputDebugStringW(L"Could not close the request handle!\n");
#endif
			return false;
		}
	}
	if (hConnect != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringW(L"The connect handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hConnect) == FALSE)
		{
			hConnect = nullptr;
#ifdef _DEBUG
			OutputDebugStringW(L"Could not close the connect handle!\n");
#endif
			return false;
		}
	}
	if (hSession != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringW(L"The session handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hRequest) == FALSE)
		{
			hRequest = nullptr;
#ifdef _DEBUG
			OutputDebugStringW(L"Could not close the session handle!\n");
#endif
			return false;
		}
	}

	std::vector<BYTE> vData;
	DWORD dwSize = 0;

#ifndef UNICODE
	std::wstring wSDL_HOST = string::ConvertAnsiToWide(SDL_HOST);
	std::wstring wSDL_PATH = string::ConvertAnsiToWide(SDL_PATH);
#endif

	hSession = WinHttpOpen(USER_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringW(L"The request handle was not zero!\n");
#endif
		if (hSession == NULL) {
#ifdef _DEBUG
			OutputDebugStringW(L"Could not start http client!\n");
#endif
			goto clean_up;
		}

		hConnect = WinHttpConnect(hSession, SDL_HOST.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);

		if (hConnect == NULL) {
#ifdef _DEBUG
			OutputDebugStringW(L"Could not connect to host!\n");
#endif
			goto clean_up;
		}

		hRequest = WinHttpOpenRequest(hConnect, L"GET", SDL_PATH.c_str(), NULL, WINHTTP_NO_REFERER, NULL, NULL);
		if (hRequest == NULL) {
#ifdef _DEBUG
			OutputDebugStringW(L"Could not open request!\n");
#endif
			goto clean_up;
		}
		if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringW(L"Failed to send request!\n");
#endif
			goto clean_up;
		}
		if (WinHttpReceiveResponse(hRequest, NULL) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringW(L"Failed to receive request!\n");
#endif
			goto clean_up;
		}

		do {
			dwSize = 0;
			if (WinHttpQueryDataAvailable(hRequest, &dwSize) == FALSE)
			{
#ifdef _DEBUG
				OutputDebugStringW(L"Failed to query data!\n");
#endif
				goto clean_up;
			}
			try {
				vData.resize(dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)vData.data(), dwSize, NULL))
				{
#ifdef _DEBUG
					OutputDebugStringW(L"Failed to read data!\n");
#endif
				}
				SDL_Database.append((CHAR*)vData.data());
				vData.clear();
			}
			catch (std::bad_alloc)
			{
#ifdef _DEBUG
				OutputDebugStringW(L"Caught a bad allocation!\n");
#endif
				goto clean_up;
			}
		} while (dwSize > 0);

	clean_up:
		if (hRequest)
		{
			WinHttpCloseHandle(hRequest);
			hRequest = nullptr;
		}
		if (hConnect)
		{
			WinHttpCloseHandle(hConnect);
			hConnect = nullptr;
		}
		if (hSession)
		{
			WinHttpCloseHandle(hSession);
			hSession = nullptr;
		}
		if (SDL_Database.size() == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

#ifdef _DEBUG
	OutputDebugStringW(L"The request handle was not zero!\n");
#endif
	return false;
}
#else
bool HTTP::DownloadSDLFileA(std::string SDL_HOST, std::string SDL_PATH, std::string& SDL_Database)
{
	if (hRequest != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringA("The request handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hRequest) == FALSE)
		{
			hRequest = nullptr;
#ifdef _DEBUG
			OutputDebugStringA("Could not close the request handle!\n");
#endif
			return false;
		}
	}
	if (hConnect != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringA("The connect handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hConnect) == FALSE)
		{
			hConnect = nullptr;
#ifdef _DEBUG
			OutputDebugStringA("Could not close the connect handle!\n");
#endif
			return false;
		}
	}
	if (hSession != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringA("The session handle was not zero!\n");
#endif
		if (WinHttpCloseHandle(hRequest) == FALSE)
		{
			hRequest = nullptr;
#ifdef _DEBUG
			OutputDebugStringA("Could not close the session handle!\n");
#endif
			return false;
		}
	}

	std::vector<BYTE> vData;
	DWORD dwSize = 0;

#ifndef UNICODE
	std::wstring wSDL_HOST = string::ConvertAnsiToWide(SDL_HOST);
	std::wstring wSDL_PATH = string::ConvertAnsiToWide(SDL_PATH);
#endif

	hSession = WinHttpOpen(USER_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession != nullptr)
	{
#ifdef _DEBUG
		OutputDebugStringA("The request handle was not zero!\n");
#endif
		if (hSession == NULL) {
#ifdef _DEBUG
			OutputDebugStringA("Could not start http client!\n");
#endif
			goto clean_up;
		}

		hConnect = WinHttpConnect(hSession, wSDL_HOST.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);

		if (hConnect == NULL) {
#ifdef _DEBUG
			OutputDebugStringA("Could not connect to host!\n");
#endif
			goto clean_up;
		}

		hRequest = WinHttpOpenRequest(hConnect, L"GET", wSDL_PATH.c_str(), NULL, WINHTTP_NO_REFERER, NULL, NULL);
		if (hRequest == NULL) {
#ifdef _DEBUG
			OutputDebugStringA("Could not open request!\n");
#endif
			goto clean_up;
		}
		if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringA("Failed to send request!\n");
#endif
			goto clean_up;
		}
		if (WinHttpReceiveResponse(hRequest, NULL) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringA("Failed to receive request!\n");
#endif
			goto clean_up;
		}

		do {
			dwSize = 0;
			if (WinHttpQueryDataAvailable(hRequest, &dwSize) == FALSE)
			{
#ifdef _DEBUG
				OutputDebugStringA("Failed to query data!\n");
#endif
				goto clean_up;
			}
			try {
				vData.resize(dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)vData.data(), dwSize, NULL))
				{
#ifdef _DEBUG
					OutputDebugStringA("Failed to read data!\n");
#endif
				}
				SDL_Database.append((CHAR*)vData.data());
				vData.clear();
			}
			catch (std::bad_alloc)
			{
#ifdef _DEBUG
				OutputDebugStringA("Caught a bad allocation!\n");
#endif
				goto clean_up;
			}
		} while (dwSize > 0);

	clean_up:
		if (hRequest)
		{
			WinHttpCloseHandle(hRequest);
		}
		if (hConnect)
		{
			WinHttpCloseHandle(hConnect);
		}
		if (hSession)
		{
			WinHttpCloseHandle(hSession);
		}
		if (SDL_Database.size() == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

#ifdef _DEBUG
	OutputDebugStringA("The request handle was not zero!\n");
#endif
	return false;
}
#endif
#pragma warning( pop )