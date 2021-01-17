
// PCAuditAppDlg.h: plik nagłówkowy
//
#include "Resource.h"		// główne symbole
#pragma once



// okno dialogowe CPCAuditAppDlg
class CPCAuditAppDlg : public CDialogEx
{
// Konstrukcja
public:
	CPCAuditAppDlg(CWnd* pParent = nullptr);	// konstruktor standardowy

// Dane okna dialogowego

	enum { IDD = IDD_PCAUDITAPP_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// obsługa DDX/DDV


// Implementacja
protected:
	HICON m_hIcon;

	// Wygenerowano funkcje mapy komunikatów
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int SaveToCsv();
	int getAddressInfo();
	CEdit edit1;
	CButton zapisz_button;
	CButton cancel_button;
	CButton getInfo_button;
	afx_msg void OnBnClickedGetinfo();
};
