#pragma once

struct P_CURL_MOD_MEMORY
{
	char* Memory;
	size_t Size;
	void* Callback;
	long CallbackData;
};

class CPugCurl
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void StartFrame();
	void PostJSON(const char* url, long Timeout, std::string BearerToken, std::string PostData, void* FunctionCallback, long CallbackData);
	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

private:
	bool m_ServerActive = false;
	CURLM* m_MultiHandle = NULL;
	long m_RequestIndex = 0;
	std::map<long, P_CURL_MOD_MEMORY> m_Data;
};

extern CPugCurl gPugCurl;
