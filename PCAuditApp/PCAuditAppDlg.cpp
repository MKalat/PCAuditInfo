
// PCAuditAppDlg.cpp: plik implementacji
//

#include "pch.h"
#include "framework.h"
#include "PCAuditApp.h"
#include "PCAuditAppDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <winternl.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// okno dialogowe CPCAuditAppDlg



CPCAuditAppDlg::CPCAuditAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCAUDITAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCAuditAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit1);


}

BEGIN_MESSAGE_MAP(CPCAuditAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CPCAuditAppDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPCAuditAppDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_GETINFO, &CPCAuditAppDlg::OnBnClickedGetinfo)
END_MESSAGE_MAP()


// Procedury obsługi komunikatów CPCAuditAppDlg

BOOL CPCAuditAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Ustaw ikonę dla tego okna dialogowego. Struktura wykonuje to automatycznie
	//  gdy okno główne aplikacji nie jest oknem dialogowym
	SetIcon(m_hIcon, TRUE);			// Ustaw duże ikony
	SetIcon(m_hIcon, FALSE);		// Ustaw małe ikony

	// TODO: Dodaj tutaj dodatkowe inicjowanie
	CPCAuditAppDlg::getOSVersion();
	CPCAuditAppDlg::getProcessorInfo();
	CPCAuditAppDlg::getAddressInfo();

	return TRUE;  // zwracaj wartość TRUE, dopóki fokus nie zostanie ustawiony na formant
}

// Jeśli dodasz przycisk minimalizacji do okna dialogowego, będziesz potrzebować poniższego kodu
//  aby narysować ikonę. Dla aplikacji MFC używających modelu dokumentu/widoku
//  to jest wykonywane automatycznie przez strukturę.

void CPCAuditAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // kontekst urządzenia dotyczący malowania

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Wyśrodkuj ikonę w prostokącie klienta
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Rysuj ikonę
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// System wywołuje tę funkcję, aby uzyskać kursor wyświetlany podczas przeciągania przez użytkownika
//  zminimalizowane okno.
HCURSOR CPCAuditAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCAuditAppDlg::OnBnClickedOk()
{
	CPCAuditAppDlg::SaveToCsv();
	CDialogEx::OnOK();
}


void CPCAuditAppDlg::OnBnClickedCancel()
{
	// TODO: Dodaj tutaj swój kod procedury obsługi powiadamiania kontrolki
	CDialogEx::OnCancel();
}

int CPCAuditAppDlg::SaveToCsv()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		int msgboxID = MessageBox(NULL, "Error allocating memory needed to call GetAdaptersinfo", MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);

		return 1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			int msgboxID = MessageBox(
				NULL,
				"Error allocating memory needed to call GetAdaptersinfo",
				MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
			);

			return 1;
		}
	}
	char edit_text[4096];
	char buff[2048];
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		
	#define INFO_BUFFER_SIZE 32767
		char infoBuf[INFO_BUFFER_SIZE];
		char computerName[INFO_BUFFER_SIZE];
		char userName[INFO_BUFFER_SIZE];
		DWORD bufCharCount = INFO_BUFFER_SIZE;
		
		if (!GetComputerNameA(infoBuf, &bufCharCount))
			strcpy(computerName, "Error retrieving computer name");
		strcpy(computerName, infoBuf);
		


		if (!GetUserNameA(infoBuf, &bufCharCount))
			strcat(userName, "Error retrieving current user name");
		strcat(userName, infoBuf);
		
		while (pAdapter) {
			strcpy(edit_text, computerName);
			strcat(edit_text, ",");
			strcat(edit_text, userName);
			strcat(edit_text, ",");
			sprintf(buff, "%d", pAdapter->ComboIndex);
			strcat(edit_text, buff);
			strcat(edit_text, ",");
			strcat(edit_text, pAdapter->AdapterName);
			strcat(edit_text, ",");
			strcat(edit_text, pAdapter->Description);
			strcat(edit_text, ",");
			for (i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
				{
					sprintf(buff, "%.2X", (int)pAdapter->Address[i]);
					strcat(edit_text, buff);
				}
				else
				{
					sprintf(buff, "%.2X-", (int)pAdapter->Address[i]);
					strcat(edit_text, buff);
				}
			}
			strcat(edit_text, ",");
			sprintf(buff, "%d", pAdapter->Index);
			strcat(edit_text, buff);
			strcat(edit_text, ",");
			switch (pAdapter->Type) {
			case MIB_IF_TYPE_OTHER:
				strcat(edit_text, "Other");
				break;
			case MIB_IF_TYPE_ETHERNET:
				strcat(edit_text, "Ethernet");
				break;
			case MIB_IF_TYPE_TOKENRING:
				strcat(edit_text, "Token Ring");
				break;
			case MIB_IF_TYPE_FDDI:
				strcat(edit_text, "FDDI");
				break;
			case MIB_IF_TYPE_PPP:
				strcat(edit_text, "PPP");
				break;
			case MIB_IF_TYPE_LOOPBACK:
				strcat(edit_text, "Lookback");
				break;
			case MIB_IF_TYPE_SLIP:
				strcat(edit_text, "Slip");
				break;
			default:
				strcat(edit_text, "Unknown type ");
				sprintf(buff, "%u", pAdapter->Type);
				strcat(edit_text, buff);
				break;
			}
			strcat(edit_text, ",");
			
			strcat(edit_text, pAdapter->IpAddressList.IpAddress.String);
			strcat(edit_text, ",");
			strcat(edit_text, pAdapter->IpAddressList.IpMask.String);
			strcat(edit_text, ",");
			
			strcat(edit_text, pAdapter->GatewayList.IpAddress.String);
			strcat(edit_text, ",");
			strcat(edit_text, "\r\n");
			
			
			pAdapter = pAdapter->Next;
			std::ofstream outfile;

			outfile.open("pc_audit.csv", std::ios_base::app); // append instead of overwrite
			outfile << edit_text;
			outfile.close();
		}
	}
	
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return 0;
}

int CPCAuditAppDlg::getAddressInfo()
{
	/* Declare and initialize variables */

// It is possible for an adapter to have multiple
// IPv4 addresses, gateways, and secondary WINS servers
// assigned to the adapter. 
//
// Note that this sample code only prints out the 
// first entry for the IP address/mask, and gateway, and
// the primary and secondary WINS server for each adapter. 

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		int msgboxID = MessageBox(NULL,"Error allocating memory needed to call GetAdaptersinfo",MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
		
		return 1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			int msgboxID = MessageBox(
				NULL,
				"Error allocating memory needed to call GetAdaptersinfo",
				MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
			);
			
			return 1;
		}
	}
	char edit_content[4096];
	edit1.GetWindowTextA(edit_content, 4096);
	char edit_text[4096];
	char buff[2048];
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		//IDD_IDC_EDIT1.SetWindowText((LPCTSTR)"");
		strcpy(edit_text, "\r\nDetailed Computer Info");
		
		#define INFO_BUFFER_SIZE 32767
		char infoBuf[INFO_BUFFER_SIZE];
		DWORD bufCharCount = INFO_BUFFER_SIZE;
		strcat(edit_text, "\r\n Computer Name :");
		if (!GetComputerNameA(infoBuf, &bufCharCount))
			strcat(edit_text, "Error retrieving computer name");
		strcat(edit_text, infoBuf);

		strcat(edit_text, "\r\n Current User Name :");
		if (!GetUserNameA(infoBuf, &bufCharCount))
			strcat(edit_text, "Error retrieving current user name");
		strcat(edit_text, infoBuf);
		strcat(edit_text,"\r\n Adapters Info\r\n");
		while (pAdapter) {
			strcat(edit_text, "\r\n\tComboIndex: ");
			sprintf(buff, "%d", pAdapter->ComboIndex);
			strcat(edit_text,buff);
			strcat(edit_text, "\r\n\tAdapter Name: \t");
			strcat(edit_text, pAdapter->AdapterName);
			strcat(edit_text, "\r\n\tAdapter Desc: \t");
			strcat(edit_text, pAdapter->Description);
			strcat(edit_text, "\r\n\tAdapter Addr: \t");
			for (i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
				{
					sprintf(buff, "%.2X", (int)pAdapter->Address[i]);
					strcat(edit_text, buff);
				}
				else
				{
					sprintf(buff, "%.2X-", (int)pAdapter->Address[i]);
					strcat(edit_text, buff);
				}
			}
			strcat(edit_text, "\r\n\tIndex: \t");
			sprintf(buff, "%d", pAdapter->Index);
			strcat(edit_text, buff);
			strcat(edit_text,"\r\n\tType: \t");
			switch (pAdapter->Type) {
			case MIB_IF_TYPE_OTHER:
				strcat(edit_text, "Other");
				break;
			case MIB_IF_TYPE_ETHERNET:
				strcat(edit_text, "Ethernet");
				break;
			case MIB_IF_TYPE_TOKENRING:
				strcat(edit_text, "Token Ring");
				break;
			case MIB_IF_TYPE_FDDI:
				strcat(edit_text, "FDDI");
				break;
			case MIB_IF_TYPE_PPP:
				strcat(edit_text, "PPP");
				break;
			case MIB_IF_TYPE_LOOPBACK:
				strcat(edit_text, "Lookback");
				break;
			case MIB_IF_TYPE_SLIP:
				strcat(edit_text, "Slip");
				break;
			default:
				strcat(edit_text, "Unknown type %ld");
				sprintf(buff, "%u", pAdapter->Type);
				strcat(edit_text, buff);
				break;
			}

			strcat(edit_text, "\r\n\tIP Address: \t");
			strcat(edit_text,	pAdapter->IpAddressList.IpAddress.String);
			strcat(edit_text, "\r\n\tIP Mask: \t");
			strcat(edit_text,  pAdapter->IpAddressList.IpMask.String);

			strcat(edit_text, "\r\n\tGateway: \t");
			strcat(edit_text,  pAdapter->GatewayList.IpAddress.String);
			strcat(edit_text, "\r\n\t***");

			if (pAdapter->DhcpEnabled) {
				strcat(edit_text, "\r\n\tDHCP Enabled: Yes");
				strcat(edit_text, "\r\n\t  DHCP Server: \t");
				strcat(edit_text, pAdapter->DhcpServer.IpAddress.String);

				strcat(edit_text, "\r\n\t  Lease Obtained: ");
				/* Display local time */
				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
				if (error)
					strcat(edit_text, "Invalid Argument to _localtime32_s");
				else {
					// Convert to an ASCII representation 
					error = asctime_s(buffer, 32, &newtime);
					if (error)
						strcat(edit_text, "Invalid Argument to asctime_s");
					else
						/* asctime_s returns the string terminated by \n\0 */
						strcat(edit_text, buffer);
				}

				strcat(edit_text, "\r\n\t  Lease Expires:  ");
				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
				if (error)
					strcat(edit_text, "Invalid Argument to _localtime32_s");
				else {
					// Convert to an ASCII representation 
					error = asctime_s(buffer, 32, &newtime);
					if (error)
						strcat(edit_text, "Invalid Argument to asctime_s");
					else
						/* asctime_s returns the string terminated by \n\0 */
						strcat(edit_text, buffer);
				}
			}
			else
				strcat(edit_text, "\r\n\tDHCP Enabled: No");

			if (pAdapter->HaveWins) {
				strcat(edit_text, "\r\n\tHave Wins: Yes");
				strcat(edit_text, "\r\n\t  Primary Wins Server:    ");
				strcat(edit_text, pAdapter->PrimaryWinsServer.IpAddress.String);
				strcat(edit_text, "\r\n\t  Secondary Wins Server:  "),
				strcat(edit_text, pAdapter->SecondaryWinsServer.IpAddress.String);
			}
			else
				strcat(edit_text, "\r\n\tHave Wins: No");
			pAdapter = pAdapter->Next;
			strcat(edit_text, "\r\n");
		}
	}
	else {
	strcat(edit_text, "GetAdaptersInfo failed with error:");
	sprintf(buff, "%d", dwRetVal);
	strcat(edit_text, buff);
	}
	strcat(edit_content, edit_text);

	
	edit1.SetWindowText(edit_content);
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return 0;
}


void CPCAuditAppDlg::OnBnClickedGetinfo()
{
	CPCAuditAppDlg::getOSVersion();
	CPCAuditAppDlg::getProcessorInfo();
	CPCAuditAppDlg::getAddressInfo();
}

void CPCAuditAppDlg::getProcessorInfo()
{
	char edit_content[4096];
	edit1.GetWindowTextA(edit_content, 4096);
	char output[4096];
	char buff[2048];
	strcpy(output, "\r\nProcessor and Memory Info :");
	int CPUInfo[4] = { -1 };
	unsigned   nExIds, i = 0;
	char CPUBrandString[0x40];
	// Get the information associated with each extended ID.
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	for (i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}
	//string includes manufacturer, model and clockspeed
	strcat(output, "\r\nCPU Type: ");
	strcat(output, CPUBrandString);


	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	strcat(output, "\r\nNumber of Cores: ");
	sprintf(buff, "%d", sysInfo.dwNumberOfProcessors);
	strcat(output,buff);

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	strcat(output, "\r\nTotal System Memory: ");
	sprintf(buff, "%lu", (statex.ullTotalPhys / 1024) / 1024);
	strcat(output,buff);
	strcat(output, "MB");
	strcat(edit_content, output);
	edit1.SetWindowTextA(edit_content);
}

void CPCAuditAppDlg::getOSVersion()
{
	char edit_content[4096];
	edit1.GetWindowTextA(edit_content, 4096);
	char output[4096];
	char buff[2048];
	
	int osverMajor = 0;
	int osverMinor = 0;

	NTSTATUS(WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXW);

	OSVERSIONINFOEXW osInfo;

	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
		osverMajor = osInfo.dwMajorVersion;
		osverMinor = osInfo.dwMinorVersion;


		strcpy(output, "OS Version Information");
		strcat(output, "\r\nWindows version: ");
		sprintf(buff, "%u.%u", osverMajor, osverMinor);
		strcat(output, buff);
		strcat(edit_content, output);
		edit1.SetWindowTextA(edit_content);
	}
}
