// AutoInstallSSLCert.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <Windows.h>
#include <wincrypt.h>
#include <iostream>
#include <iomanip>
#include <malloc.h>

#pragma comment(lib,"Crypt32.lib")


#define T(CONTEXTS) TEXT(CONTEXTS)

using namespace std;


void printCertInfo(DWORD cbData, BYTE *pbData) {
	for (register DWORD dwCount = 1; dwCount <= cbData; ++dwCount) {
		cout << hex << uppercase << setw(2) << setfill('0') << (DWORD)pbData[dwCount - 1] << " ";
	}
	cout << endl;

}
void printErrorMessage(const char *Args) {
	cout << "Error !" << endl << Args << endl << "GetLastError() : " << GetLastError() << endl;
}



int main()
{

	HANDLE hCertFile = CreateFile(T("test.der"),
		GENERIC_READ,
		FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!hCertFile) {
		printErrorMessage("无法打开本地证书文件");
		return 0;
	}

	DWORD dwFileSize = GetFileSize(hCertFile, NULL);

	BYTE *lpBuffer = (BYTE *)malloc(sizeof(BYTE) * dwFileSize);

	if (!lpBuffer) {
		printErrorMessage("无法分配足够内存");
		CloseHandle(hCertFile);
		return 0;
	}

	DWORD dwRead = 0;

	ReadFile(hCertFile, lpBuffer, dwFileSize, &dwRead, NULL);
	if (dwRead != dwFileSize) {
		printErrorMessage("无法装载证书文件");
		CloseHandle(hCertFile);
		free(lpBuffer);
		return 0;
	}



	PCCERT_CONTEXT pCertContent = NULL;

	HANDLE hStore = CertOpenSystemStore(NULL, T("MY"));

	if (!hStore) {
		printErrorMessage("无法打开系统证书库");
		CloseHandle(hCertFile);
		free(lpBuffer);
		return 0;
	}

	CertAddEncodedCertificateToStore(hStore,
		X509_ASN_ENCODING,
		lpBuffer,
		dwFileSize,
		CERT_STORE_ADD_REPLACE_EXISTING,
		&pCertContent
	);

	free(lpBuffer);
	

	if (!pCertContent) {
		printErrorMessage("错误的证书格式");
		CertCloseStore(hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return 0;
	}

	//输出FingerPoint
	BYTE byteSHA1Value[20];
	DWORD dwDataSize = 20;

	CertGetCertificateContextProperty(pCertContent, CERT_HASH_PROP_ID, &byteSHA1Value, &dwDataSize);

	printCertInfo(dwDataSize, byteSHA1Value);
	CertFreeCertificateContext(pCertContent);


	CertCloseStore(hStore, CERT_CLOSE_STORE_FORCE_FLAG);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
