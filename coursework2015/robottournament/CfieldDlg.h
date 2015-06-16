#pragma once

#include "Parameters.h"
#include "PaintDlg.h"
#include "afxwin.h"

using namespace std;
// диалоговое окно CfieldDlg

class CfieldDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CfieldDlg)

public:
	CfieldDlg(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~CfieldDlg();

// Данные диалогового окна
	enum { IDD = IDD_FIELDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CPaintDlg paintDlg;
	CString *Dlls;
	int actingRobot;
	int aliveRobots;
	void Play();
	step **history;
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();
	afx_msg void OnBnClickedUpbtn();
	afx_msg void OnBnClickedDownbtn();
	afx_msg void OnBnClickedLeftbtn();
	afx_msg void OnBnClickedRightbtn();
	CListBox m_ListBox;
	afx_msg void OnBnClickedButton2();
	CButton m_StartButton;
	int rx;
	int ry;
	int rE;
	int rL;
	int rA;
	int rP;
	int rV;
	int stepNumber;
	int infoLock;
	afx_msg void OnLbnSelchangeList1();
	int rkills;
	void Sort();
	CString path;
	int players;
	int *finalStandings;
};

