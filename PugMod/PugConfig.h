#pragma once

class CPugConfig
{
public:
	void ServerActivate();
	void Execute(std::string File);
	std::string GetFullPath();

private:
	std::string m_Path;
};

extern CPugConfig gPugConfig;