#pragma once
#include "Parameters.h"

// диалоговое окно CPaintDlg

class CPaintDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPaintDlg)

public:
	CPaintDlg(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~CPaintDlg();

// Данные диалогового окна
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	fieldData FieldParameters;
	CRect DrawArea;
	COLORREF Black;
	COLORREF White;
	COLORREF Green;
	CPen GridPen;
	CPen MapPen1;
	CPen MapPen2;
	double ratio;
	int n;
	int **matrix;
	int upperleftCellCoords[2];
	robot **robots;
	object **objects;
	int cameraLock;
	void DrawRobot(int x, int y, /*int r, int g, int b*/ COLORREF color);
	void DrawObject(int x, int y, int type);
	void DrawRobots();
	void DrawMap();
	afx_msg void OnPaint();
	afx_msg void OnClose();
};
