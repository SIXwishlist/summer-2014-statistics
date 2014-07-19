#pragma once

typedef long int TBandwidth;
typedef long int THits;

// ��������� "������ ����" - ����������� ������ ����
struct TLogRecord
{
	std::string origin;				// ip �������
	std::string identd;			// (-)
	std::string auth;			// id ������� (-)
	//struct tm datetime;			// ����, �����. (m_isdst : 0 - ����� �� �������, �� 0 - �������)
	std::string date;
	int hour;						// ���
	std::string method;				// ����� �������
	std::string path;	// ��� �������������� �������
	std::string protocol;	// �������� ��������� (HTTP/1.0)
	std::string status;				// ��� ���������
	TBandwidth bytes;		// ������ �������, ������������ ������� (�����)
	std::string referer;		// URL, �� ������� �������� ������
	std::string client;		// ��� �������� �������
};

class Analyzer
{
private:
	int hitsInHour[24];					// ���������� �������� �� ���
	int uniqIPsInHour[24];				// ���������� ���������� IP �� ���
	TBandwidth bandwidthInHour[24];		// ���������� �������� ������ �� ���
	int pagesInHour[24];				// �����. �������� � ���
	int visitsInHour[24];				// ��������� � ���
	std::string currentDate;	// �������������� ����
	int currentHour;			// �������������� ���
	std::set<std::string> setOfUniqueIPs;		// ��������� ����, � ������� ���������� �� ����
	std::set<std::string> setOfUniqueIPsHour;	// ��������� ����, � ������� ���������� �� ���
	// �����������: IP <-> bytes/pages/visits/hits	
	std::map<std::string,TBandwidth> mapIpBandwidth;
	std::map<std::string,int> mapIpPages;
	std::map<std::string,THits>	mapIpHits;
	std::map<std::string,int>	mapIpVisits;


public:
	Analyzer(void);
	~Analyzer(void);
	///
	/// ���������������� ������ ����, ����������� ������������ � ���� processedLogFileName, 
	/// ��������� ���������� � uniqueVisitorsFileName
	///
	bool analyzeLogFile(const std::string logFileName, const std::string processedLogFileName, 
						const std::string uniqueVisitorsFileName, const std::string hitsFileName,
						const std::string bandwidthFileName, const std::string pagesFilename,
						const std::string visitsFileName);
	///
	/// ��������� ������
	///
	bool parseString(std::string str, TLogRecord & logRecord);
};

