#pragma once

#include "DriverBase.h"

typedef VOID(*FILESEARCH_CALLBACK)(PFILE_BOTH_DIR_INFORMATION);

HANDLE kFindFirstFile(
	LPTSTR lpDirectory,
	PFILE_BOTH_DIR_INFORMATION pDir,
	ULONG uLength
);

BOOLEAN kFindNextFile(
	HANDLE hFind,
	PFILE_BOTH_DIR_INFORMATION pDir,
	ULONG uLength
);

ULONG SearchDirectory(
	LPTSTR lpPath,
	FILESEARCH_CALLBACK pFun
);

BOOLEAN DeleteFiles(
	PWCHAR pFileName
);