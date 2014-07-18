#include "stdafx.h"
#include "Analyzer.h"


Analyzer::Analyzer(void)
{
}


Analyzer::~Analyzer(void)
{
}

bool Analyzer::analyzeLogFile(const std::string logFileName, const std::string processedLogFileName, 
					const std::string uniqueVisitorsFileName, const std::string hitsFileName)
{
	using namespace std;
	// ������� �������
	ifstream logFile(logFileName);
	if (!logFile.is_open())
	{
		std::cerr << "<ExStatistics> Can't open logFile " + logFileName << endl;
		return false;
	}

	// ������� ���� ������� � ������ �����
	std::vector<std::string> stringsToAnalyze;
	while (!logFile.eof())
	{
		std::string temp;
		getline(logFile,temp);
		stringsToAnalyze.push_back(temp);
	}
	logFile.close();

	// ������� �������
	{ofstream of; of.open(logFileName,std::ios_base::trunc);}

	// ������� ������������ ���� � ����� ����� ������������ �����
	ofstream processedLogFile(processedLogFileName,std::ios::app);
	if (!processedLogFile.is_open()){
		std::cerr << "<ExStatistics> Access to processedLogFile " + logFileName + " is forbidden" << endl;
		// TODO: �������� ���� ������� � logFileName, ����� �� ����������
	} else
		for(auto it = stringsToAnalyze.begin() ; it != stringsToAnalyze.end(); ++it)
		{
			processedLogFile << *it << endl;
		}
	currentDate = "";
	// ���������� �������������� ����
	for(auto it = stringsToAnalyze.begin() ; it != stringsToAnalyze.end(); ++it)
	{
		TLogRecord record;
		bool ok = parseString(*it,record);	// ������ ������ (����������� � � ������)
		// ���� ������� ��������� (� ���� ���� �� �����), ���������� ������
		if (ok && record.date != "")
		{
			// ���� �������������� ��������� ����, �� ������� ��������
			if (currentDate != record.date)
			{
				currentDate = record.date;
				for(int i=0;i<24;i++) hitsInHour[i]=0;
			}
			hitsInHour[record.hour]++;		// �������� ����� ��������� � ���� ��� �� 1
		}
	}

	// HITS
	// ������� ���������� � ������� ���� � ����� �����
	ofstream hitsFile(hitsFileName,std::ios::app);
	if (hitsFile.is_open())
		std::cerr << "<ExStatistics> Access to hitsFile " + hitsFileName+ " is forbidden" << endl;
	// ������� ����, � ����� ���������� � ����������� �� �����
	hitsFile << 
}

const std::string LOG_NOT_AVAILIABLE = "-";

bool Analyzer::parseString(std::string str, TLogRecord & logRecord)
{
	using namespace std;
	string temp;
	char ctemp;
	stringstream sstream(str);
	
	// ������� ip
	sstream >> temp;
	if (temp != LOG_NOT_AVAILIABLE)
		logRecord.origin = temp;
	else
		logRecord.origin = "";

	// ������� identd
	sstream >> temp;
	if (temp != LOG_NOT_AVAILIABLE)
		logRecord.identd = temp;
	else
		logRecord.identd = "";

	// ������� auth
	sstream >> temp;
	if (temp != LOG_NOT_AVAILIABLE)
		logRecord.auth = temp;
	else
		logRecord.auth = "";

	// ������� ���������
	//sstream >> ctemp;		// ������
	sstream >> ctemp;		// - ��� �������
	temp = ctemp;
	if (temp != LOG_NOT_AVAILIABLE)
	{
		sstream >> temp;		// ������ ���� + �����
		size_t pos1 = temp.find_first_of(':');
		logRecord.date = temp.substr(0,pos1);	// ������ � �����
		size_t pos2 = temp.find_first_of(pos1+1,':');	
		logRecord.hour = std::stoi(temp.substr(pos1+1,pos2-1));	// ���
		sstream >> temp;	// ������� ��������� ������
	}
	else{
		logRecord.date = ""; logRecord.hour = "";
	}

	// ������� �����, ������, ��������
	//sstream >> ctemp;		// ������
	sstream >> ctemp;		// - ��� �������
	temp = ctemp;
	if (temp != LOG_NOT_AVAILIABLE)
	{
		sstream >> logRecord.method;
		sstream >> logRecord.path;
		sstream >> logRecord.protocol;  
		logRecord.protocol.pop_back(); // ������ "
	}else
	{
		logRecord.method="";
		logRecord.path = "";
		logRecord.protocol = "";
	}

	// ������� ��� ���������
	sstream >> temp;
	if (temp != LOG_NOT_AVAILIABLE)
	{
		logRecord.status = temp;
	} else
		logRecord.status = "";

	// ������� ���������� ������
	logRecord.bytes = 0;
	sstream >> logRecord.bytes;
	if (logRecord.bytes == 0)
		sstream >> temp;

	// ������� ����� URL, ������� ������ ������������� ����
	sstream >> temp;
	if (temp != LOG_NOT_AVAILIABLE)
	{
		temp.pop_back(); temp.erase(0,1); // ������ ������� ������� � �����
		// ������ http(s)://���_�����
		size_t pos = temp.find_first_of('/');
		pos = temp.find_first_of(pos+1,'/');
		pos = temp.find_first_of(pos+1,'/');
		temp.erase(0,pos);
		logRecord.referer = temp;
	}else
		logRecord.referer = "";

	// ������� ���������� � ��������
	sstream >> ctemp;	// ������
	sstream >> ctemp;	// - ��� "
	temp = ctemp;
	if (temp != LOG_NOT_AVAILIABLE)
	{
		//size_t pos = sstream.str().find_first_of('\"');
		//logRecord.client = sstream.str().substr(0,pos);
		sstream >> std::noskipws;
		logRecord.client.clear();
		do{
			logRecord.client += ctemp;
			sstream >> ctemp;
		}while(ctemp !='\"');
		sstream >> std::skipws;
	}
	else
		logRecord.client = "";
	
	return true;
}