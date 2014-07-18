#pragma once

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
	long int bytes;		// ������ �������, ������������ ������� (�����)
	std::string referer;		// URL, �� ������� �������� ������
	std::string client;		// ��� �������� �������
};

class Analyzer
{
private:
	int hitsInHour[24];			// ���������� �������� �� ���
	std::string currentDate;	// �������������� ����

public:
	Analyzer(void);
	~Analyzer(void);

	// ���������������� ������ ����, ����������� ������������ � ���� processedLogFileName, 
	// ��������� ���������� � uniqueVisitorsFileName
	bool analyzeLogFile(const std::string logFileName, const std::string processedLogFileName, 
						const std::string uniqueVisitorsFileName, const std::string hitsFileName);
	// ��������� ������
	bool parseString(std::string str, TLogRecord & logRecord);
};

