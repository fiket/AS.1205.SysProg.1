
// robottournament.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CrobottournamentApp:
// О реализации данного класса см. robottournament.cpp
//

class CrobottournamentApp : public CWinApp
{
public:
	CrobottournamentApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CrobottournamentApp theApp;