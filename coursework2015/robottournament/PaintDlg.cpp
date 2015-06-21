// PaintDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "robottournament.h"
#include "PaintDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "math.h"

// диалоговое окно CPaintDlg

IMPLEMENT_DYNAMIC(CPaintDlg, CDialogEx)

CPaintDlg::CPaintDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPaintDlg::IDD, pParent)
{
	upperleftCellCoords[0] = 0;
	upperleftCellCoords[1] = 0;
	n = fieldSide;

	Black = RGB(0,0,0);
	White = RGB(255,255,255);
	Green = RGB(0,200,0);
	GridPen.CreatePen(PS_SOLID,1,Black);
	MapPen1.CreatePen(PS_SOLID,3,Black);
	ratio = (double)FieldParameters.fieldWidth/(double)FieldParameters.fieldHeight;
}

CPaintDlg::~CPaintDlg()
{
}

void CPaintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaintDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// обработчики сообщений CPaintDlg


void CPaintDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	GetClientRect(&DrawArea);
	DrawArea.right = DrawArea.bottom;
	double xcoor = DrawArea.left;
	double ycoor = DrawArea.top;
	dc.SelectObject(&GridPen);
	for (int i=0;i<=n;i++)
	{
		dc.MoveTo(xcoor,ycoor);
		dc.LineTo(xcoor,DrawArea.right);
		xcoor+=DrawArea.right/n;
	}
	xcoor = DrawArea.left;
	for (int i=0;i<=n;i++)
	{
		dc.MoveTo(xcoor,ycoor);
		dc.LineTo(DrawArea.bottom,ycoor);
		ycoor+=DrawArea.bottom/n;
	}
}


void CPaintDlg::DrawMap()
{
	CPaintDC dc(this);
	GetClientRect(&DrawArea);
	dc.MoveTo(DrawArea.left,DrawArea.top);
	dc.LineTo(DrawArea.right,DrawArea.bottom);
}

void CPaintDlg::DrawRobot(int x, int y, COLORREF color)
{
	CClientDC dc(this);
	GetClientRect(&DrawArea);
	CBrush Brush(color);
	dc.SelectObject(Brush);
	int side = DrawArea.bottom/n;
	Ellipse(dc,x*side+1,y*side+1,x*side+side,y*side+side);
}

void CPaintDlg::DrawObject(int x, int y, int type)
{
	COLORREF color;
	if (type == OBJ_CHARGER)
		color = RGB(0,0,255);
	else //type == OBJ_TECH
		color = RGB(255,0,0);
	CClientDC dc(this);
	GetClientRect(&DrawArea);
	CPen Pen(PS_SOLID,2,color);
	dc.SelectObject(Pen);
	int side = DrawArea.bottom/n;
	dc.MoveTo(x*side+1,y*side+1);
	dc.LineTo(x*side+side,y*side+side);
	dc.MoveTo(x*side+1,y*side+side);
	dc.LineTo(x*side+side,y*side+1);
}

void CPaintDlg::DrawRobots()
{
	Invalidate();
	AfxGetApp()->PumpMessage();
	OnPaint();
	if (cameraLock > -1)
	{
		upperleftCellCoords[0] = robots[cameraLock]->x-fieldSide/2;
		upperleftCellCoords[1] = robots[cameraLock]->y-fieldSide/2;

		if (upperleftCellCoords[0]<0)
			upperleftCellCoords[0]+=FieldParameters.fieldWidth;
		if (upperleftCellCoords[0]>=FieldParameters.fieldWidth)
			upperleftCellCoords[0]-=FieldParameters.fieldWidth;

		if (upperleftCellCoords[1]<0)
			upperleftCellCoords[1]+=FieldParameters.fieldHeight;
		if (upperleftCellCoords[1]>=FieldParameters.fieldHeight)
			upperleftCellCoords[1]-=FieldParameters.fieldHeight;
	}
	int xlocal = 0;
	int ylocal = 0;
	int xreal, yreal;
	for (int x = upperleftCellCoords[0]; x < upperleftCellCoords[0]+n; x++)
	{
		xreal = x;
		if (xreal<0)
			xreal+=FieldParameters.fieldWidth;
		if (xreal>=FieldParameters.fieldWidth)
			xreal-=FieldParameters.fieldWidth;
		for (int y = upperleftCellCoords[1]; y < upperleftCellCoords[1]+n; y++)
		{
			yreal = y;
			if (yreal<0)
				yreal+=FieldParameters.fieldHeight;
			if (yreal>=FieldParameters.fieldHeight)
				yreal-=FieldParameters.fieldHeight;
			if (matrix[xreal][yreal] > -1)
				DrawRobot(xlocal,ylocal,robots[matrix[xreal][yreal]]->color);
			else if (matrix[xreal][yreal] == SEVERAL)
				DrawRobot(xlocal,ylocal,Black);
			else if (matrix[xreal][yreal] == OBJ_DEAD)
				DrawRobot(xlocal,ylocal,White);
			else if (matrix[xreal][yreal] < -1)
				DrawObject(xlocal,ylocal,matrix[xreal][yreal]);
			ylocal++;
		}
		xlocal++;
		ylocal = 0;
	}
}



void CPaintDlg::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	//CDialogEx::OnClose();
}
