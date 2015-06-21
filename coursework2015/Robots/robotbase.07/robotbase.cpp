// robotbase.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "robotbase.h"
#include <fstream>
#include <string>

using namespace std;

extern "C" 
void DoStep(stepinfo *Info, step *Step)
{
	int id = Info->yourNumber;
	int clone = Info->robots[id]->playerid;
	int myx = Info->robots[id]->x;
	int myy = Info->robots[id]->y;
	int myA = Info->robots[id]->A;
	int myP = Info->robots[id]->P;
	int myV = Info->robots[id]->V;
	int myL = Info->robots[id]->L;
	int myE = Info->robots[id]->E;
	string filename = "../Robots/robotbase.07/r" + to_string(id) + ".txt";
	bool urgent = false;

	bool main = false;
	for (int i = 0; i<Info->field->rivals; i++)
	{
		if (Info->robots[i]->playerid == clone && i == id)
		{
			main = true;
			break;
		}
	}

	if (Info->stepnum == 1)
	{
		if (main)
		{
			double Ed = 111111;
			int Ei = 0;
			for (int i = 0; i<Info->field->Ne; i++)
			{
				double cd = sqrt(pow(Info->objects[i]->x,2) + pow(Info->objects[i]->y,2));
				if (cd < Ed)
				{
					Ed = cd;
					Ei = i;
				}
			}
			ofstream File("../Robots/robotbase.07/main.txt");
			File << Ei;
			File.close();
		}
		ofstream File(filename);
		File << 0;
		File.close();
	}

	int direction;

	if (myE < 0.7*Info->field->Emax)
	{
		urgent = true;
		ofstream File(filename);
		File << 2;
		File.close();
		int V = Info->field->Vmax;
		if (V > myL)
			V = myL;
		DoAction(Step, ACT_TECH, 0, myL-V, V);
	}
	else if (myL < Info->field->Lmax)
	{
		urgent = true;
		ofstream File(filename);
		File << 1;
		File.close();
		int V = Info->field->Vmax;
		if (V > myL)
			V = myL;
		DoAction(Step, ACT_TECH, 0, myL-V, V);
	}
	else
	{
		urgent = false;
		DoAction(Step, ACT_TECH, 74, 1, 25);
	}
	
	ifstream File(filename);
	File >> direction;
	File.close();

	int mainpoint = 0;
	if (!direction)
	{
		ifstream File("../Robots/robotbase.07/main.txt");
		File >> mainpoint;
		File.close();
	}

	double maxstep = Info->field->Vmax*myV/Info->field->Lmax*myE/Info->field->Emax;

	switch(direction)	//перемещение
	{
	case 0:		//идем к зарядке, выбранной главным роботом
		{
			int destx = Info->objects[mainpoint]->x;
			int desty = Info->objects[mainpoint]->y;
			double d = sqrt(pow(destx-myx,2) + pow(desty-myy,2));
			int dx, dy;
			if (d > maxstep)
			{
				dx = (destx - myx)*maxstep/d;
				dy = (desty - myy)*maxstep/d;
			}
			else
			{
				dx = destx - myx;
				dy = desty - myy;
				ofstream File(filename);
				File << 1;
				File.close();
			}
			DoAction(Step, ACT_MOVE, dx, dy);
			break;
		}
	case 1:		//идем к техточке
		{
			double Ld = 111111;
			int Li = 0;
			for (int i = Info->field->Ne; i<Info->field->Ne+Info->field->Nl; i++)
			{
				double cd = sqrt(pow(Info->objects[i]->x,2) + pow(Info->objects[i]->y,2));
				if (cd < Ld)
				{
					Ld = cd;
					Li = i;
				}
			}
			int destx = Info->objects[Li]->x;
			int desty = Info->objects[Li]->y;
			double d = sqrt(pow(destx-myx,2) + pow(desty-myy,2));
			int dx, dy;
			if (d > maxstep)
			{
				dx = (Info->objects[Li]->x - myx)*maxstep/d;
				dy = (Info->objects[Li]->y - myy)*maxstep/d;
			}
			else
			{
				dx = Info->objects[Li]->x - myx;
				dy = Info->objects[Li]->y - myy;
				ofstream File(filename);
				File << 2;
				File.close();
				if (urgent)
					DoAction(Step, ACT_TECH, 0, myL, 0);
			}
			DoAction(Step, ACT_MOVE, dx, dy);
			break;
		}
	case 2:		//идем к зарядке
		{
			double Ed = 111111;
			int Ei = 0;
			for (int i = 0; i<Info->field->Ne; i++)
			{
				double cd = sqrt(pow(Info->objects[i]->x,2) + pow(Info->objects[i]->y,2));
				if (cd < Ed)
				{
					Ed = cd;
					Ei = i;
				}
			}
			int destx = Info->objects[Ei]->x;
			int desty = Info->objects[Ei]->y;
			double d = sqrt(pow(destx-myx,2) + pow(desty-myy,2));
			double dx, dy;
			if (d > maxstep)
			{
				dx = (Info->objects[Ei]->x - myx)*maxstep/d;
				dy = (Info->objects[Ei]->y - myy)*maxstep/d;
			}
			else
			{
				dx = Info->objects[Ei]->x - myx;
				dy = Info->objects[Ei]->y - myy;
				ofstream File(filename);
				File << 1;
				File.close();
				if (urgent)
					DoAction(Step, ACT_TECH, 0, myL, 0);
			}
			DoAction(Step, ACT_MOVE, dx, dy);
			break;
		}
	}

	if (!urgent)	//ищем и бьем цель
	{
		double maxattack = Info->field->Rmax*myV/Info->field->Lmax*myE/Info->field->Emax;
		for (int i = 0; i<Info->field->rivals; i++)
		{
			if (Info->robots[i]->alive && Info->robots[i]->playerid != clone)
			{
				int destx = Info->robots[i]->x;
				int desty = Info->robots[i]->y;
				double d = sqrt(pow(destx-myx,2) + pow(desty-myy,2));
				if (d <= maxattack)
				{
					double destP = Info->robots[i]->P * Info->robots[i]->E / Info->field->Emax;
					double destA = myA * myE / Info->field->Emax;
					if (destA * 1.02 > destP)
					{
						int dx = destx - myx;
						int dy = desty - myy;
						DoAction(Step, ACT_ATTACK, dx, dy);
						break;
					}
				}
			}
		}
	}
	return;
}


