//#include "stdafx.h"
#include "Analyzer.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <map>


Analyzer::Analyzer(void)
{
}


Analyzer::~Analyzer(void)
{
}

bool Analyzer::analyzeLogFile(const std::string logFileName, const std::string processedLogFileName, 
						const std::string uniqueVisitorsFileName, const std::string hitsFileName,
						const std::string bandwidthFileName, const std::string pagesFilename,
						const std::string visitsFileName, const std::string ipMappingFileName)
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
		if (!temp.empty())
			stringsToAnalyze.push_back(temp);
	}
	logFile.close();

	// ������� �������
	{ofstream of; of.open(logFileName,std::ios_base::trunc); of.close();}

	// ������� ������������ ���� � ����� ����� ������������ �����
	ofstream processedLogFile(processedLogFileName,std::ios::app);
	if (!processedLogFile.is_open()){
		std::cerr << "<ExStatistics> Access to processedLogFile " + processedLogFileName + " is forbidden" << endl;
		// TODO: �������� ���� ������� � logFileName, ����� �� ����������
	} else
		for(auto it = stringsToAnalyze.begin() ; it != stringsToAnalyze.end(); ++it)
			processedLogFile << *it << endl;
	processedLogFile.close();

	// ������� �����, � ������� �� ����� ������ ����������
	ofstream uniqueVisitorsFile(uniqueVisitorsFileName,std::ios::app);
	if (!uniqueVisitorsFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + uniqueVisitorsFileName+ " is forbidden" << endl;
		return false;
	}
	ofstream hitsFile(hitsFileName,std::ios::app);
	if (!hitsFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + hitsFileName+ " is forbidden" << endl;
		return false;
	}
	ofstream bandwidthFile(bandwidthFileName,std::ios::app);
	if (!bandwidthFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + bandwidthFileName+ " is forbidden" << endl;
		return false;
	}
	ofstream pagesFile(pagesFilename,std::ios::app);
	if (!pagesFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + pagesFilename+ " is forbidden" << endl;
		return false;
	}
	ofstream visitsFile(visitsFileName,std::ios::app);
	if (!visitsFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + visitsFileName + " is forbidden" << endl;
		return false;
	}
	ofstream ipMappingFile(ipMappingFileName,std::ios::app);
	if (!ipMappingFile.is_open()){
		std::cerr << "<ExStatistics> Access to " + ipMappingFileName + " is forbidden" << endl;
		return false;
	}
		
	currentDate = "";
	currentHour = -1;
	// ���������� �������������� ����
	for(auto it = stringsToAnalyze.begin() ; it != stringsToAnalyze.end(); ++it)
	{
		TLogRecord record;
		bool ok = parseString(*it,record);	// ������ ������ (����������� � � ������)
		// ���� ������� ��������� (� ���� ���� �� �����), ���������� ������
		if (ok && record.date != "")
		{
			///
			/// ������ � ����� ����������
			///
			// ���� ����� �������������� ��������� ����
			if (currentDate != record.date)
			{
				// ������� ���������� � ���������� ��� � ����� ������
				if (currentDate != "")
				{
					writeToStatFiles(hitsFile,uniqueVisitorsFile, bandwidthFile,  pagesFile, visitsFile,  ipMappingFile);
				}
				// ������� ��������
				currentDate = record.date;
				for(int i=0;i<24;i++) {hitsInHour[i]=0;	uniqIPsInHour[i]=0; bandwidthInHour[i]=0; pagesInHour[i]=0; visitsInHour[i]=0;}
				setOfUniqueIPs.clear();
				mapIpBandwidth.clear();
				mapIpHits.clear();
				mapIpPages.clear();
				mapIpVisits.clear();
			}
			// ���� ����� �������������� ��������� ���
			if (currentHour!=record.hour){
				setOfUniqueIPsHour.clear();	// ������� ���� �� ���
				currentHour = record.hour;
			}

			///
			/// ������ ����������
			///
			// HITS
			hitsInHour[record.hour]++;		// �������� ����� ��������� � ���� ��� �� 1
			// UNIQUE VISITORS
			if (!record.origin.empty()){
				// ���� ������ ip ��� �� ����, �������� ������� � ������� ��� � ������
				if (setOfUniqueIPs.find(record.origin) == setOfUniqueIPs.end()){
					uniqIPsInHour[record.hour]++;
					setOfUniqueIPs.insert(record.origin);
				}
			}
			// BANDWIDTH
			bandwidthInHour[record.hour] += record.bytes;
			// PAGES
			// ���� �� ������������� ������, �� ��� ������ ��������
			if (record.path.rfind('.')==string::npos && record.path.rfind('?')==string::npos) {
				pagesInHour[record.hour]++;
				// IP <-> PAGES
				if (mapIpPages.find(record.origin) != mapIpPages.end())
					mapIpPages[record.origin]++;
				else
					mapIpPages[record.origin] = 1;
			}
			// VISITS
			// ������ ��������� �������, ���� �� ��� ������ � ��������� ���
			if (!record.origin.empty())
			{
				if (setOfUniqueIPsHour.find(record.origin) == setOfUniqueIPsHour.end()) {
					visitsInHour[record.hour]++;
					setOfUniqueIPsHour.insert(record.origin);
				
					// IP <-> VISITS
					if (mapIpVisits.find(record.origin) != mapIpVisits.end())
						mapIpVisits[record.origin]++;
					else
						mapIpVisits[record.origin] = 1;
				}
			}
		}
		// IP <-> HITS
		if (mapIpHits.find(record.origin) != mapIpHits.end())
			mapIpHits[record.origin]++;
		else
			mapIpHits[record.origin] = 1;
		// IP <-> BANDWIDTH
		if (mapIpBandwidth.find(record.origin) != mapIpBandwidth.end())
			mapIpBandwidth[record.origin] += record.bytes;
		else
			mapIpBandwidth[record.origin] = record.bytes;
	}
	// ������� ����������
	writeToStatFiles(hitsFile, uniqueVisitorsFile, bandwidthFile, pagesFile, visitsFile, ipMappingFile);
	hitsFile.close();
	logFile.close();
	pagesFile.close();
	bandwidthFile.close();
	return true;
}


void Analyzer::writeToStatFiles(std::ofstream & hitsFile, std::ofstream & uniqueVisitorsFile,
								std::ofstream & bandwidthFile, std::ofstream & pagesFile,
								std::ofstream & visitsFile, std::ofstream & ipMappingFile)
{
	using namespace std;
	// HITS - ������� ����, � ����� ���������� � ����������� �� �����
	hitsFile << currentDate << " ";
	for (int i=0; i<24; i++)
		hitsFile << hitsInHour[i] << " ";
	hitsFile << endl;
	// uniqueIPs - ������� ����, � ����� ���������� � ����������� �� �����
	uniqueVisitorsFile << currentDate << " ";
	for (int i=0; i<24; i++)
		uniqueVisitorsFile << uniqIPsInHour[i] << " ";
	uniqueVisitorsFile << endl;
	// BANDWIDTH
	bandwidthFile << currentDate << " ";
	for (int i=0; i<24; i++)
		bandwidthFile << bandwidthInHour[i] << " ";
	bandwidthFile << endl;
	// PAGES
	pagesFile << currentDate << " ";
	for (int i=0; i<24; i++)
		pagesFile << pagesInHour[i] << " ";
	pagesFile << endl;
	// VISITS
	visitsFile << currentDate << " ";
	for (int i=0; i<24; i++)
		visitsFile << visitsInHour[i] << " ";
	visitsFile << endl;
	// �����������: IP <-> hits/bytes/pages/visits/
	// ������ ip ��������� _END_
	ipMappingFile << currentDate << endl;
	if (!mapIpHits.empty())
	{
		auto mapItem = mapIpHits.begin();
		for (;mapItem!=mapIpHits.end();mapItem++){
			ipMappingFile << mapItem->first << " " << mapItem->second << " ";	//hits
			if(mapIpBandwidth.find(mapItem->first)!=mapIpBandwidth.end())		// bits
				ipMappingFile << mapIpBandwidth[mapItem->first] << " ";
			else
				ipMappingFile << "0" << " ";
			if(mapIpPages.find(mapItem->first)!=mapIpPages.end())				// pages
				ipMappingFile << mapIpPages[mapItem->first] << " ";
			else
				ipMappingFile << "0" << " ";
			if(mapIpVisits.find(mapItem->first)!=mapIpVisits.end())				// visits
				ipMappingFile << mapIpVisits[mapItem->first];
			else
				ipMappingFile << "0";
			ipMappingFile << endl;
		}
		ipMappingFile << "_END_" << endl;
	}
}


const std::string LOG_NOT_AVAILIABLE = "-";

bool Analyzer::parseString(std::string str, TLogRecord & logRecord)
{
	using namespace std;
	string temp;
	char ctemp;
	stringstream sstream(str);

	if (str.size() < 10)
		return false;
	
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
		logRecord.date = ""; logRecord.hour = -1;
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