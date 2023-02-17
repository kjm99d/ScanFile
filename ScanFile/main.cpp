#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#pragma comment(lib, "Version.lib")


typedef struct _FileVersion {
    WORD major;
    WORD minor;
    WORD buildNum;
    WORD revisionNum;

    std::string GetString() {
        std::stringstream stream;
        stream << major << "." << minor << "." << buildNum << "." << revisionNum;
        return stream.str();
    }
} FileVersion;


BOOL GetFileVersion(std::string strFilePath, FileVersion& version);



int main(int argc, TCHAR * argv[])
{
	CHAR szPath[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, szPath);

	std::filesystem::path strCurrentPath(szPath);
	std::cout << "Directory >> " << strCurrentPath << std::endl;
	
    std::ofstream logger("ScanFile.csv");

	for (const auto& p : std::filesystem::recursive_directory_iterator(szPath))
	{
		std::filesystem::path relative = std::filesystem::relative(p, strCurrentPath);
        FileVersion version;
        if (GetFileVersion(p.path().u8string(), version))
        {
            logger << relative << "," << version.GetString() << std::endl;
            std::cout << relative << "\t" << version.GetString()  << std::endl;
        }

	}

    logger.close();


    system("pause");

	return 0;
}





BOOL GetFileVersion(std::string strFilePath, FileVersion & version)
{
    BOOL nRet = FALSE;

    if (strFilePath.empty() == false)
    {
        // 파일 정보 저장 버퍼
        TCHAR atcBuffer[MAX_PATH] = { 0, };
        if (FALSE != GetFileVersionInfoA(strFilePath.c_str(), 0, MAX_PATH, atcBuffer))
        {
            VS_FIXEDFILEINFO* pFineInfo = NULL;
            UINT bufLen = 0;
            // VS_FIXEDFILEINFO 정보 가져오기
            if (FALSE != VerQueryValueA(atcBuffer, "\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
            {
                WORD majorVer, minorVer, buildNum, revisionNum;
                majorVer = HIWORD(pFineInfo->dwFileVersionMS);
                minorVer = LOWORD(pFineInfo->dwFileVersionMS);
                buildNum = HIWORD(pFineInfo->dwFileVersionLS);
                revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

                version = {
                    majorVer,
                    minorVer,
                    buildNum,
                    revisionNum
                };

                nRet = TRUE;
            }
        }
    }
    return nRet;
}