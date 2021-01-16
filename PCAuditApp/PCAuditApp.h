
// PCAuditApp.h: główny plik nagłówkowy aplikacji PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// główne symbole


// CPCAuditAppApp:
// Aby uzyskać implementację klasy, zobacz PCAuditApp.cpp
//

class CPCAuditAppApp : public CWinApp
{
public:
	CPCAuditAppApp();

// Przesłania
public:
	virtual BOOL InitInstance();

// Implementacja

	DECLARE_MESSAGE_MAP()
};

extern CPCAuditAppApp theApp;
