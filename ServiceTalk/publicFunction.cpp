#include "StdAfx.h"
#include "publicFunction.h"


BOOL GetVersionInfo(LPCTSTR lpszFileName, CString& sVersion, CString& sName, CString& sUpgradeInfo)
{
	BOOL bResult = TRUE;
	UINT cbVersion = 0, cbTranslate = 0, dwBytes = 0;
	void* pBlock = NULL;
	void* lpBufferName = NULL;
	void* lpDescription = NULL;
	void* lpBufferVer = NULL;

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate = NULL;

	try
	{
		DWORD dwHandle = 0;

		int csize = ::GetFileVersionInfoSize(lpszFileName, &dwHandle);
		if (csize < 0)
			throw "没有取到文件版本信息长度！";

		pBlock = (void*)new char[csize];

		TRACE("\n[%s]:%d new pBlock \n", __FILE__, __LINE__);
		ZeroMemory(pBlock, csize);

		if (!::GetFileVersionInfo(lpszFileName, 0, csize, pBlock))
			throw "没有取到文件版本信息！";

		VS_FIXEDFILEINFO* lpVersion = NULL;
		if (!VerQueryValue(pBlock, TEXT("\\"), (LPVOID*)&lpVersion, &cbVersion))
			throw "无法取到版本信息！";

		sVersion.Format("%d.%d.%d.%d",
			(int)lpVersion->dwProductVersionMS >> 16,
			(int)lpVersion->dwProductVersionMS & 0x0000ffff,
			(int)lpVersion->dwProductVersionLS >> 16,
			(int)lpVersion->dwProductVersionLS & 0x0000ffff);

		if (VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate))
		{
			CString sSubBlock;
			for (int i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)) && i < 1; i++)
			{
				sSubBlock.Format("\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
				// Retrieve file description for language and code page "i". 
				VerQueryValue(pBlock, (char*)LPCTSTR(sSubBlock), &lpBufferName, &dwBytes);

				sSubBlock.Format("\\StringFileInfo\\%04x%04x\\Comments", lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
				// Retrieve file description for language and code page "i". 
				VerQueryValue(pBlock, (char*)LPCTSTR(sSubBlock), &lpDescription, &dwBytes);
			}
		}

		sName = lpBufferName ? (char*)lpBufferName : "";
		sName.Replace("<", "&lt;");
		sName.Replace(">", "&gt;");
		sUpgradeInfo = lpDescription ? (char*)lpDescription : "";
		sUpgradeInfo.Replace("<", "&lt;");
		sUpgradeInfo.Replace(">", "&gt;");
	}
	catch (LPCTSTR)
	{
		bResult = FALSE;
	}

	if (pBlock)
		delete[] pBlock;

	return bResult;
}