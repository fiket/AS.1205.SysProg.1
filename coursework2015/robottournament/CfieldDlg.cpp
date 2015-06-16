// CfieldDlg.cpp: файл реализации
//


#include "stdafx.h"
#include "robottournament.h"
#include "CfieldDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "math.h"
#include <string>


// диалоговое окно CfieldDlg

IMPLEMENT_DYNAMIC(CfieldDlg, CDialogEx)

	CfieldDlg::CfieldDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfieldDlg::IDD, pParent)
	, rx(0)
	, ry(0)
	, rE(0)
	, rL(0)
	, rA(0)
	, rP(0)
	, rV(0)
	, stepNumber(0)
	, infoLock(-1)
	, rkills(0)
{
}

CfieldDlg::~CfieldDlg()
{
	for (int i=0;i<paintDlg.FieldParameters.fieldWidth;i++)
		delete paintDlg.matrix[i];
	delete [] paintDlg.matrix;
	for (int i=0;i<paintDlg.FieldParameters.rivals;i++)
		delete paintDlg.robots[i];
	delete [] paintDlg.robots;
	for (int i=0;i<paintDlg.FieldParameters.Ne+paintDlg.FieldParameters.Nl;i++)
		delete paintDlg.objects[i];
	delete [] paintDlg.objects;
}

void CfieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_BUTTON1, m_StartButton);
	DDX_Text(pDX, IDC_EDIT1, rx);
	DDX_Text(pDX, IDC_EDIT2, ry);
	DDX_Text(pDX, IDC_EDIT3, rE);
	DDX_Text(pDX, IDC_EDIT18, rL);
	DDX_Text(pDX, IDC_EDIT4, rA);
	DDX_Text(pDX, IDC_EDIT5, rP);
	DDX_Text(pDX, IDC_EDIT8, rV);
	DDX_Text(pDX, IDC_EDIT9, stepNumber);
	DDX_Text(pDX, IDC_EDIT10, rkills);
}


BEGIN_MESSAGE_MAP(CfieldDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CfieldDlg::OnBnClickedOk)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CfieldDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_UPBTN, &CfieldDlg::OnBnClickedUpbtn)
	ON_BN_CLICKED(IDC_DOWNBTN, &CfieldDlg::OnBnClickedDownbtn)
	ON_BN_CLICKED(IDC_LEFTBTN, &CfieldDlg::OnBnClickedLeftbtn)
	ON_BN_CLICKED(IDC_RIGHTBTN, &CfieldDlg::OnBnClickedRightbtn)
	ON_BN_CLICKED(IDC_BUTTON2, &CfieldDlg::OnBnClickedButton2)
	ON_LBN_SELCHANGE(IDC_LIST1, &CfieldDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// обработчики сообщений CfieldDlg


void CfieldDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	AfxGetMainWnd()->DestroyWindow();
	CDialogEx::OnOK();
}

void CfieldDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//CRect DrawArea;

	// Не вызывать CDialogEx::OnPaint() для сообщений рисования
}

UINT thread(LPVOID pParam)
{
	toThread *Data = (toThread*)pParam;
	Data->Function(Data->Info,Data->Step);

	return 0;
}

void CfieldDlg::Play()
{
	while (stepNumber < paintDlg.FieldParameters.N && aliveRobots > 1)
	{
		stepNumber++;
		stepinfo *Stepinfo = new stepinfo;
		Stepinfo->stepnum = stepNumber;
		Stepinfo->robots = new robotinfo*[paintDlg.FieldParameters.rivals];
		Stepinfo->objects = new object*[paintDlg.FieldParameters.Ne+paintDlg.FieldParameters.Nl];
		Stepinfo->history = new step*[paintDlg.FieldParameters.rivals];
		Stepinfo->field = new fieldData;
		memcpy(Stepinfo->field, &paintDlg.FieldParameters, sizeof(fieldData));	//инфо о поле
		for (int i=0; i<paintDlg.FieldParameters.rivals; i++)	//инфо о роботах
		{
			Stepinfo->robots[i] = new robotinfo;
			Stepinfo->robots[i]->name = paintDlg.robots[i]->name;
			Stepinfo->robots[i]->x = paintDlg.robots[i]->x;
			Stepinfo->robots[i]->y = paintDlg.robots[i]->y;
			Stepinfo->robots[i]->A = paintDlg.robots[i]->A;
			Stepinfo->robots[i]->P = paintDlg.robots[i]->P;
			Stepinfo->robots[i]->V = paintDlg.robots[i]->V;
			Stepinfo->robots[i]->L = paintDlg.robots[i]->L;
			Stepinfo->robots[i]->E = paintDlg.robots[i]->E;
			Stepinfo->robots[i]->alive = paintDlg.robots[i]->alive;
			Stepinfo->robots[i]->kills = paintDlg.robots[i]->kills;
			Stepinfo->robots[i]->playerid = paintDlg.robots[i]->player;

			Stepinfo->history[i] = history[i];
		}
		for (int i=0; i<paintDlg.FieldParameters.Ne+paintDlg.FieldParameters.Nl; i++)	//инфо об объектах
		{
			Stepinfo->objects[i] = new object;
			memcpy(&Stepinfo->objects[i], &paintDlg.objects[i], sizeof(object));
		}

		for (int i = 0; i < paintDlg.FieldParameters.rivals; i++)	//обнуление атакующих
		{
			for (int j = 0; j < paintDlg.FieldParameters.rivals; j++)
				paintDlg.robots[i]->attackedBy[j] = false;
		}

		for (int actingRobot=0; actingRobot<paintDlg.FieldParameters.rivals; actingRobot++)	//шаг
		{
			Stepinfo->yourNumber = actingRobot;
			if (paintDlg.robots[actingRobot]->alive)
			{
				toThread *threadData = new toThread;
				step *Step = new step;
				for (int i = 0; i<3; i++)
					Step->actions[i] = NULL;

				threadData->Function = paintDlg.robots[actingRobot]->DoStep;
				threadData->Info = Stepinfo;
				threadData->Step = Step;
				HANDLE hThread = AfxBeginThread(thread,threadData)->m_hThread;
				if (WaitForSingleObject(hThread, paintDlg.FieldParameters.T) == WAIT_TIMEOUT)
					TerminateThread(hThread, NULL);

				if (history[actingRobot])	//очистка истории
				{
					for (int i = 0; i<3; i++)	//
					{
						if (history[actingRobot]->actions[i])
							delete history[actingRobot]->actions[i];
					}
					delete history[actingRobot];	
				}

				history[actingRobot] = Step;
				int curx = paintDlg.robots[actingRobot]->x;
				int cury = paintDlg.robots[actingRobot]->y;
				paintDlg.robots[actingRobot]->newx = curx;
				paintDlg.robots[actingRobot]->newy = cury;

				if (Step)
				{
					for (int i = 0; i<3; i++)
					{
						if (Step->actions[i])
						{
							switch (i)
							{
							case ACT_MOVE:
								{
									if (sqrt(pow(Step->actions[i]->dx,2) + pow(Step->actions[i]->dy,2)) <= (double)paintDlg.FieldParameters.Vmax*(double)paintDlg.robots[actingRobot]->V/(double)paintDlg.FieldParameters.Lmax*(double)paintDlg.robots[actingRobot]->E/(double)paintDlg.FieldParameters.Emax)
									{
										paintDlg.robots[actingRobot]->newx = curx+Step->actions[i]->dx;
										paintDlg.robots[actingRobot]->newy = cury+Step->actions[i]->dy;
										if (paintDlg.robots[actingRobot]->newx<0)
											paintDlg.robots[actingRobot]->newx+=paintDlg.FieldParameters.fieldWidth;
										else if (paintDlg.robots[actingRobot]->newx>=paintDlg.FieldParameters.fieldWidth)
											paintDlg.robots[actingRobot]->newx-=paintDlg.FieldParameters.fieldWidth;
										if (paintDlg.robots[actingRobot]->newy<0)
											paintDlg.robots[actingRobot]->newy+=paintDlg.FieldParameters.fieldHeight;
										else if (paintDlg.robots[actingRobot]->newy>=paintDlg.FieldParameters.fieldHeight)
											paintDlg.robots[actingRobot]->newy-=paintDlg.FieldParameters.fieldHeight;
										paintDlg.robots[actingRobot]->newE -= paintDlg.FieldParameters.dEv;
									}
									break;
								}
							case ACT_ATTACK:
								{
									if ( (sqrt(pow(Step->actions[i]->dx,2) + pow(Step->actions[i]->dy,2)) <= paintDlg.FieldParameters.Rmax*paintDlg.robots[actingRobot]->V/paintDlg.FieldParameters.Lmax*paintDlg.robots[actingRobot]->E/paintDlg.FieldParameters.Emax))
									{
										int destx = curx + Step->actions[i]->dx;
										int desty = cury + Step->actions[i]->dy;
										if (destx<0)
											destx+=paintDlg.FieldParameters.fieldWidth;
										else if (destx>=paintDlg.FieldParameters.fieldWidth)
											destx-=paintDlg.FieldParameters.fieldWidth;
										if (desty<0)
											desty+=paintDlg.FieldParameters.fieldHeight;
										else if (desty>=paintDlg.FieldParameters.fieldHeight)
											desty-=paintDlg.FieldParameters.fieldHeight;
										int victim = paintDlg.matrix[destx][desty];
										if (victim != -1)
										{
											int a, b;
											if (victim > -1)	//если в точке один робот
											{
												a = victim;
												b = a+1;
											}
											else
											{
												a = 0;
												b = paintDlg.FieldParameters.rivals;
											}
											for (int j = a; j<b; j++)
											{
												if (paintDlg.robots[j]->x == destx && paintDlg.robots[j]->y == desty && paintDlg.robots[j]->alive)	//ищем всех в данной координате
												{
													victim = j;
													paintDlg.robots[actingRobot]->newE -= paintDlg.FieldParameters.dEa;
													paintDlg.robots[victim]->newE -= paintDlg.FieldParameters.dEp;
													srand(time(0));
													double rnd = rand() % (int)(paintDlg.FieldParameters.rndmax*100+1) + paintDlg.FieldParameters.rndmin*100;
													rnd /= 100;
													double A = rnd*paintDlg.robots[actingRobot]->A*paintDlg.robots[actingRobot]->E/paintDlg.FieldParameters.Emax;
													double P = (1-rnd)*paintDlg.robots[victim]->P*paintDlg.robots[victim]->E/paintDlg.FieldParameters.Emax;
													paintDlg.robots[victim]->attackedBy[actingRobot] = true;	//пишем, что роботы атаковали друг друга
													paintDlg.robots[actingRobot]->attackedBy[victim] = true;
													if (A > P)	//удачная атака
													{
														if (paintDlg.robots[victim]->newP > 0 && paintDlg.robots[victim]->P > 0)
														{
															paintDlg.robots[victim]->newP -= A-P;
															paintDlg.robots[victim]->newL -= A-P;
														}
														else
															paintDlg.robots[victim]->newE -= abs(P - A)*paintDlg.FieldParameters.Emax/paintDlg.FieldParameters.Lmax;
													}
													else    //неудачная атака
													{
														if (paintDlg.robots[actingRobot]->newA > 0 && paintDlg.robots[actingRobot]->A > 0)
														{
															paintDlg.robots[actingRobot]->newA -= P-A;
															paintDlg.robots[actingRobot]->newL -= P-A;
														}
														else
															paintDlg.robots[actingRobot]->newE -= abs(P - A)*paintDlg.FieldParameters.Emax/paintDlg.FieldParameters.Lmax;
													}
													if (paintDlg.robots[victim]->newE <= 0)	//проверка убийства
														paintDlg.robots[victim]->killed = true;
													if (paintDlg.robots[actingRobot]->newE <= 0)	//проверка убийства
														paintDlg.robots[actingRobot]->killed = true;
												}
											}
										}
									}
									break;
								}
							case ACT_TECH:
								{
									if (Step->actions[i]->A < 0 || Step->actions[i]->P < 0 || Step->actions[i]->V < 0)
										break;
									int Lcur = paintDlg.robots[actingRobot]->L;
									int Lnew = paintDlg.robots[actingRobot]->newL;
									int Ldes = Step->actions[i]->A + Step->actions[i]->P + Step->actions[i]->V;
									if (Ldes <= Lcur)
									{
										if (Ldes <= Lnew)
										{
											paintDlg.robots[actingRobot]->newA = Step->actions[i]->A;
											paintDlg.robots[actingRobot]->newP = Step->actions[i]->P;
											paintDlg.robots[actingRobot]->newV = Step->actions[i]->V;
										}
										else
										{
											double k = (double)Lnew/(double)Lcur;
											paintDlg.robots[actingRobot]->newA = (double)Step->actions[i]->A*k;
											paintDlg.robots[actingRobot]->newP = (double)Step->actions[i]->P*k;
											paintDlg.robots[actingRobot]->newV = (double)Step->actions[i]->V*k;
										}
									}
									break;
								}
							}
						}
					}
				}
				if (paintDlg.robots[actingRobot]->E == paintDlg.robots[actingRobot]->newE)	//если  простой
					paintDlg.robots[actingRobot]->newE -= paintDlg.FieldParameters.dEs;
				int newx = paintDlg.robots[actingRobot]->newx;
				int newy = paintDlg.robots[actingRobot]->newy;
				int curobj = paintDlg.matrix[newx][newy];
				if (curobj == OBJ_CHARGER)
				{
					paintDlg.robots[actingRobot]->newE += paintDlg.FieldParameters.dE;
					for (int i=0; i<paintDlg.FieldParameters.rivals; i++)
					{
						if (i != actingRobot && !paintDlg.robots[i]->alive && paintDlg.robots[i]->x == newx && paintDlg.robots[i]->y == newy) 
						{
							curobj = OBJ_DEAD;
							break;
						}
					}
				}
				if (curobj == OBJ_TECH)
					paintDlg.robots[actingRobot]->newL += paintDlg.FieldParameters.dL;
				if (curobj == OBJ_DEAD)
				{
					for (int i=0; i<paintDlg.FieldParameters.rivals; i++)	//ищем труп, на котором стоим
					{
						if (paintDlg.robots[i]->x == newx && paintDlg.robots[i]->y == newy && !paintDlg.robots[i]->alive)
						{
							curobj = i;
							break;
						}
					}
					int curL;
					if (paintDlg.robots[curobj]->L >= paintDlg.FieldParameters.dL)
						curL = paintDlg.FieldParameters.dL;
					else
						curL = paintDlg.robots[curobj]->L;
					if (paintDlg.robots[actingRobot]->L + curL > paintDlg.FieldParameters.Lmax)
						curL = paintDlg.FieldParameters.Lmax - paintDlg.robots[actingRobot]->L;
					paintDlg.robots[actingRobot]->newL += curL;
					paintDlg.robots[curobj]->newL -= curL;
				}
			}
		}
		for (int i=0; i<paintDlg.FieldParameters.rivals; i++)	//обновляем параметры, выносим трупы
		{
			if (paintDlg.robots[i]->alive)
			{
				paintDlg.robots[i]->stepsAlive++;
				if (paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] == i || paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] == SEVERAL)	//перемещение
					paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] = -1;
				paintDlg.robots[i]->x = paintDlg.robots[i]->newx;
				paintDlg.robots[i]->y = paintDlg.robots[i]->newy;
				if (paintDlg.matrix[paintDlg.robots[i]->newx][paintDlg.robots[i]->newy] == -1)	//если пустая клетка
					paintDlg.matrix[paintDlg.robots[i]->newx][paintDlg.robots[i]->newy] = i;
				else if (paintDlg.matrix[paintDlg.robots[i]->newx][paintDlg.robots[i]->newy] >-1 || paintDlg.matrix[paintDlg.robots[i]->newx][paintDlg.robots[i]->newy] == SEVERAL)	//если кто-то стоит
					paintDlg.matrix[paintDlg.robots[i]->newx][paintDlg.robots[i]->newy] = SEVERAL;	//

				if (paintDlg.robots[i]->newA < 0)
				{
					paintDlg.robots[i]->newL -= paintDlg.robots[i]->newA;
					paintDlg.robots[i]->newA = 0;
				}
				if (paintDlg.robots[i]->newP < 0)
				{
					paintDlg.robots[i]->newL -= paintDlg.robots[i]->newP;
					paintDlg.robots[i]->newP = 0;
				}
				if (paintDlg.robots[i]->newL < 0)
					paintDlg.robots[i]->newL = 0;
				if (paintDlg.robots[i]->newL > paintDlg.FieldParameters.Lmax)
					paintDlg.robots[i]->newL = paintDlg.FieldParameters.Lmax;
				if (paintDlg.robots[i]->newE > paintDlg.FieldParameters.Emax)
					paintDlg.robots[i]->newE = paintDlg.FieldParameters.Emax;
				paintDlg.robots[i]->A = paintDlg.robots[i]->newA;
				paintDlg.robots[i]->P = paintDlg.robots[i]->newP;
				paintDlg.robots[i]->V = paintDlg.robots[i]->newV;
				paintDlg.robots[i]->L = paintDlg.robots[i]->newL;
				paintDlg.robots[i]->E = paintDlg.robots[i]->newE;
				if (paintDlg.robots[i]->killed || paintDlg.robots[i]->E<=0)
				{
					paintDlg.robots[i]->alive = false;
					if (paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] == i || paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] == SEVERAL)
						paintDlg.matrix[paintDlg.robots[i]->x][paintDlg.robots[i]->y] = OBJ_DEAD;
					for (int j = 0; j < paintDlg.FieldParameters.rivals; j++)	//призы убийцам
					{
						if (paintDlg.robots[i]->attackedBy[j])
							paintDlg.robots[j]->kills++;
					}
					aliveRobots--;
				}
			}
			else
				paintDlg.robots[i]->L = paintDlg.robots[i]->newL;	//обновляем кол-во еды у трупа
		}

		if (infoLock != -1)
		{
			rx = paintDlg.robots[infoLock]->x;
			ry = paintDlg.robots[infoLock]->y;
			rE = paintDlg.robots[infoLock]->E;
			rL = paintDlg.robots[infoLock]->L;
			rA = paintDlg.robots[infoLock]->A;
			rP = paintDlg.robots[infoLock]->P;
			rV = paintDlg.robots[infoLock]->V;
			rkills = paintDlg.robots[infoLock]->kills;
		}
		UpdateData(FALSE);
		UpdateWindow();

		paintDlg.DrawRobots();

		for (int i = 0; i < paintDlg.FieldParameters.rivals; i++)
			delete Stepinfo->robots[i];
		delete Stepinfo->field;
		delete Stepinfo->robots;
		delete Stepinfo->objects;
		delete Stepinfo->history;
		delete Stepinfo;

		Sleep(50);
	}

	for (int i = 0; i < paintDlg.FieldParameters.rivals; i++)	//после битвы
	{
		if (paintDlg.robots[i]->alive)
			paintDlg.robots[i]->points = paintDlg.robots[i]->newE + paintDlg.robots[i]->kills*paintDlg.FieldParameters.K;
		else
			paintDlg.robots[i]->points = paintDlg.robots[i]->stepsAlive - paintDlg.FieldParameters.N;
	}
	Sort();
	string results = "Место    Имя                     Статус    Убил   Очки\n";
	for (int i = 0; i < paintDlg.FieldParameters.rivals; i++)
	{
		int id = finalStandings[i];
		string space2 = "   ";
		if (paintDlg.robots[id]->name.GetLength() < 13)
			space2+="   ";

		string space1 = ".           ";
		if (i<9)
			space1+="  ";

		string status = "";
		if (paintDlg.robots[id]->alive)
			status = "Выжил";
		else if (paintDlg.robots[id]->killed)
		{
			status = "Убит    ";
			paintDlg.robots[id]->points += paintDlg.FieldParameters.N;
		}
		else
		{
			status = "Умер   ";
			paintDlg.robots[id]->points += paintDlg.FieldParameters.N;
		}
		results += to_string(i+1) + space1 + paintDlg.robots[id]->name.GetString() + space2 + status + "   " + to_string(paintDlg.robots[id]->kills) + "           "  + to_string(paintDlg.robots[id]->points) + "\n";
	}
	MessageBox(results.c_str(),"Результаты раунда");
	int opt = MessageBox("Обновить роботов во входном списке?","Очень важный вопрос",MB_YESNO | MB_ICONQUESTION);
	if (opt == IDYES)
	{
		ofstream List(path);
		for (int i = 0; i < paintDlg.FieldParameters.rivals; i++)
		{
			if (paintDlg.robots[i]->player)
			{
				int player = paintDlg.robots[i]->player;
				int rofp = 0;
				string name = paintDlg.robots[i]->name.GetString();
				if (name.size() > 12)
				{
					name.pop_back(); name.pop_back();
				}
				for (int j = i; j < paintDlg.FieldParameters.rivals; j++)
				{
					if (paintDlg.robots[j]->player == player)
					{
						paintDlg.robots[j]->player = 0;
						if (paintDlg.robots[j]->alive)
							rofp++;
					}
				}
				rofp++;
				List<<name<<".dll "<<rofp<<endl;
			}
		}
		List.close();
		MessageBox("Готово","Обновление данных");
	}
}


void CfieldDlg::OnBnClickedButton1()
{
	m_StartButton.EnableWindow(0);
	m_ListBox.AddString("Свободная камера");

	paintDlg.Create(IDD_DIALOG1,0);
	paintDlg.ShowWindow(SW_SHOW);
	srand(time(NULL));
	int xrand = 0;
	int yrand = 0;
	for (int i = 0; i<paintDlg.FieldParameters.Ne; i++)
	{
		do
		{
			xrand = rand() % paintDlg.FieldParameters.fieldWidth;
			yrand = rand() % paintDlg.FieldParameters.fieldHeight;
		}
		while (paintDlg.matrix[xrand][yrand] != -1);
		paintDlg.matrix[xrand][yrand] = OBJ_CHARGER;
		paintDlg.objects[i]->type = OBJ_CHARGER;
		paintDlg.objects[i]->x = xrand;
		paintDlg.objects[i]->y = yrand;
	}
	for (int i = paintDlg.FieldParameters.Ne; i<paintDlg.FieldParameters.Ne+paintDlg.FieldParameters.Nl; i++)
	{
		do
		{
			xrand = rand() % paintDlg.FieldParameters.fieldWidth;
			yrand = rand() % paintDlg.FieldParameters.fieldHeight;
		}
		while (paintDlg.matrix[xrand][yrand] != -1);
		paintDlg.matrix[xrand][yrand] = OBJ_TECH;
		paintDlg.objects[i]->type = OBJ_TECH;
		paintDlg.objects[i]->x = xrand;
		paintDlg.objects[i]->y = yrand;
	}
	for (int i=0; i<paintDlg.FieldParameters.rivals; i++)
	{
		do
		{
			xrand = rand() % paintDlg.FieldParameters.fieldWidth;
			yrand = rand() % paintDlg.FieldParameters.fieldHeight;
		}
		while (paintDlg.matrix[xrand][yrand] != -1);
		paintDlg.matrix[xrand][yrand] = i;
		paintDlg.robots[i]->x = xrand;
		paintDlg.robots[i]->y = yrand;
		m_ListBox.AddString(paintDlg.robots[i]->name);
		paintDlg.robots[i]->attackedBy = new bool[paintDlg.FieldParameters.rivals];
		for (int j = 0; j < paintDlg.FieldParameters.rivals; j++)
			paintDlg.robots[i]->attackedBy[j] = false;
	}
	//UpdateData(false);
	UpdateWindow();
	paintDlg.cameraLock = -1;
	aliveRobots = paintDlg.FieldParameters.rivals;
	stepNumber = 0;
	paintDlg.DrawRobots();
	//paintDlg.UpdateWindow();
	Play();	
}


void CfieldDlg::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	CDialogEx::OnClose();
	exit(0);
}


void CfieldDlg::OnBnClickedUpbtn()
{
	paintDlg.upperleftCellCoords[1] -= cameraStep;
	if (paintDlg.upperleftCellCoords[1]<0)
		paintDlg.upperleftCellCoords[1]+=paintDlg.FieldParameters.fieldHeight;
	paintDlg.DrawRobots();
}


void CfieldDlg::OnBnClickedDownbtn()
{
	paintDlg.upperleftCellCoords[1] += cameraStep;
	if (paintDlg.upperleftCellCoords[1]>=paintDlg.FieldParameters.fieldHeight)
		paintDlg.upperleftCellCoords[1]-=paintDlg.FieldParameters.fieldHeight;
	paintDlg.DrawRobots();
}


void CfieldDlg::OnBnClickedLeftbtn()
{
	paintDlg.upperleftCellCoords[0] -= cameraStep;
	if (paintDlg.upperleftCellCoords[0]<0)
		paintDlg.upperleftCellCoords[0]+=paintDlg.FieldParameters.fieldWidth;
	paintDlg.DrawRobots();
}


void CfieldDlg::OnBnClickedRightbtn()
{
	paintDlg.upperleftCellCoords[0] += cameraStep;
	if (paintDlg.upperleftCellCoords[0]>=paintDlg.FieldParameters.fieldWidth)
		paintDlg.upperleftCellCoords[0]-=paintDlg.FieldParameters.fieldWidth;
	paintDlg.DrawRobots();
}


void CfieldDlg::OnBnClickedButton2()
{
	paintDlg.cameraLock = m_ListBox.GetCurSel()-1;
	paintDlg.DrawRobots();
}


void CfieldDlg::OnLbnSelchangeList1()
{
	infoLock = m_ListBox.GetCurSel()-1;
	if (infoLock != -1)
	{
		rx = paintDlg.robots[infoLock]->x;
		ry = paintDlg.robots[infoLock]->y;
		rE = paintDlg.robots[infoLock]->E;
		rL = paintDlg.robots[infoLock]->L;
		rA = paintDlg.robots[infoLock]->A;
		rP = paintDlg.robots[infoLock]->P;
		rV = paintDlg.robots[infoLock]->V;
		rkills = paintDlg.robots[infoLock]->kills;
	}
	UpdateData(FALSE);
}


void CfieldDlg::Sort()
{
	int size = paintDlg.FieldParameters.rivals;
	finalStandings = new int[size];
	for (int i = 0; i < size; i++)
		finalStandings[i] = i;
	int temp;
	for (int i=0; i<=(size-1); i++)
	{
		for (int j=0; j<=(size-2); j++)
		{
			if  (paintDlg.robots[finalStandings[j+1]]->points > paintDlg.robots[finalStandings[j]]->points) 
			{ 
				temp = finalStandings[j];
				finalStandings[j] = finalStandings[j+1];
				finalStandings[j+1] = temp;
			}
		}
	}
}