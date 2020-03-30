#include "stdafx.h"
#include "filewatcher/file_watcher.h"
#include <fstream>
#include <algorithm>

using namespace Tga2D;
CFileWatcher::CFileWatcher()
	: myThread(nullptr)
	, myShouldEndThread(false)
	, myThreadIsDone(false)

{
}


CFileWatcher::~CFileWatcher()
{
	myShouldEndThread = true;
	if (myThread)
	{
		while (!myThreadIsDone)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
		myThread->join();
		delete myThread;
	}

}


void CFileWatcher::FlushChanges()
{
	if (!CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Filewatcher) || !myThread)
	{
		return;
	}
	std::lock_guard<std::mutex> guard(myMutex);

	myFileChanged.swap(myFileChangedThreaded);

	for (fs::path& theString : myFileChanged)
	{
		std::string comparrableStringFromPath = theString.string();
		std::replace(comparrableStringFromPath.begin(), comparrableStringFromPath.end(), '\\', '#');
		std::vector<callback_function_file> callbacks = myCallbacks[comparrableStringFromPath];
		for (unsigned int i = 0; i < callbacks.size(); i++)
		{
			if (callbacks[i])
			{
				callbacks[i](theString.string());
			}
		}
	}


	myFileChanged.clear();


}

long long GetFileTimeStamp(const fs::path& aFilePath)
{
	if (std::filesystem::exists(aFilePath) == false)
	{
		return 0;
	}
	return std::filesystem::last_write_time(aFilePath).time_since_epoch().count();
}


void CFileWatcher::UpdateChanges()
{
	while (!myShouldEndThread)
	{
		myMutex.lock();
		myAddNewFolderMutex.lock();

		for (auto& iter : myThreadedFilesToWatch)
		{
			CheckFileChages(iter.first, iter.second);
		}
		
		myMutex.unlock();
		myAddNewFolderMutex.unlock();
	
		std::this_thread::sleep_for(std::chrono::milliseconds(32));
	}
	myThreadIsDone = true;
}

void CFileWatcher::CheckFileChages(const fs::path& aFile, long long aTimeStampLastChanged)
{
	long long latestTimeStamp = GetFileTimeStamp(aFile);
	if (latestTimeStamp > aTimeStampLastChanged)
	{
		OnFileChange(aFile);
		myThreadedFilesToWatch[aFile] = latestTimeStamp;
	}
}

void CFileWatcher::OnFileChange(const fs::path& aFile)
{
	for (unsigned int i = 0; i < myFileChangedThreaded.size(); i++)
	{
		if (myFileChangedThreaded[i].compare(aFile) == 0)
		{
			return;
		}
	}

	myFileChangedThreaded.push_back(aFile);

}
bool CheckIsFileExits(const char * aFilePath)
{
	return std::filesystem::exists(aFilePath);
}


bool CFileWatcher::WatchFileChange(std::string aFile, callback_function_file aFunctionToCallOnChange)
{
	if (!CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Filewatcher))
	{
		return false;
	}

	if (!CheckIsFileExits(aFile.c_str()))
	{
		return false;
	}

	std::string comparrableStringFromPath = aFile;
	std::replace(comparrableStringFromPath.begin(), comparrableStringFromPath.end(), '\\', '#');
	std::replace(comparrableStringFromPath.begin(), comparrableStringFromPath.end(), '/', '#');

	myCallbacks[comparrableStringFromPath].push_back(aFunctionToCallOnChange);

	long long timeStampChanged = GetFileTimeStamp(aFile.c_str());
	if (myThread)
	{
		myAddNewFolderMutex.lock();
		myThreadedFilesToWatch[aFile] = timeStampChanged;
		myAddNewFolderMutex.unlock();
	}
	else
	{
		myThreadedFilesToWatch[aFile] = timeStampChanged;
		myThread = new std::thread(&CFileWatcher::UpdateChanges, this);
	}


	return true;
}

