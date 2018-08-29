#include "DataToCSV.h"
#include <thread>

DataToCSV* DataToCSV::pInstance = nullptr;
std::mutex DataToCSV::mutexDataToCSVTaskChecker;
bool DataToCSV::bContinueTask = false;
DataToCSV::DataToCSV( ) 
{
	strDelimeter = ",";
	ulLinesCount = 0;
}


DataToCSV::~DataToCSV()
{
}
DataToCSV*  DataToCSV::getInstance()
{
	if (!pInstance)
	{
		pInstance = new DataToCSV();
	}
	return pInstance;
}

void DataToCSV::vTaskCyclicWriteOfMotorData(std::shared_ptr<CMaxonMotor> pMaxonMotor)
{
	std::thread vTaskCyclicWriteOfMotorDataThread{ [&]()
	{
		std::vector<std::string> vecstrInputData = {};
		// RaspiTimestamp
		long long  llRaspiTimestamp;
		std::string strRaspiTimestamp;
		// Motor Data
		int iCurrentPosition;

		mutexDataToCSVTaskChecker.lock();
		bool bCopyOfContinueTask = DataToCSV::bContinueTask;
		mutexDataToCSVTaskChecker.unlock();
		while (bCopyOfContinueTask)
		{
			vOpenFile();
			vecstrInputData.clear();
			// Motordata
			cout << "CALL INSIDE TASK" << endl;
			pMaxonMotor->getCurrentPosition(iCurrentPosition);
			cout << "CALL INSIDE TASK DONE"  << endl;
			vecstrInputData.push_back(std::to_string(iCurrentPosition));
			cout << "GETTING THE MOTORDATaA YEAH " << iCurrentPosition<<endl;
			// Timestamp
			llRaspiTimestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			std::string strRaspiTimestamp = std::to_string(llRaspiTimestamp);
			vecstrInputData.push_back(strRaspiTimestamp);

			addDatainRow(vecstrInputData.begin(), vecstrInputData.end());

			mutexDataToCSVTaskChecker.lock();
			bCopyOfContinueTask = DataToCSV::bContinueTask;
			mutexDataToCSVTaskChecker.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(uiUpdateRateMs));
			vCloseFile();
		}
		
		cout << "CIAO THREAD" << endl;
	 }}; vTaskCyclicWriteOfMotorDataThread.detach();

}
