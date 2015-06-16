#pragma once

#define cameraStep 1
#define fieldSide 20

struct fieldData
{
	int fieldHeight;
	int fieldWidth;
	int rivals,
	N,    //1000
	T,    //100
	Emax, //1000
	Lmax, //100
	Vmax, //30
	Rmax, //5
	dL,   //10
	dEs,  //1
	dEv,  //2
	dEa,  //10
	dEp,  //5
	dE,   //100
	Ne,   //10
	Nl,   //10
	K;
	double rndmin, rndmax;
};

struct action
{
	int dx;
	int dy;
	int A;
	int P;
	int V;
};

struct step
{
	action* actions[3];
};

struct object
{
	int type;
	int x;
	int y;
};

struct robotinfo
{
	CString name;
	int x;
	int y;
	int E;
	int L;
	int V;
	int A;
	int P;
	int kills;
	bool alive;
	int playerid;
};

struct stepinfo
{
	int stepnum;
	int yourNumber;
	robotinfo **robots;
	object **objects;
	step **history;
	fieldData *field;
};

typedef void (*robobrain)(stepinfo *Info, step *Step);

struct robot
{
	CString name;
	COLORREF color;
	robobrain DoStep;
	int x;
	int y;
	int E;
	int L;
	int V;
	int A;
	int P;
	int newx;
	int newy;
	int newA;
	int newP;
	int newV;
	int newL;
	int newE;
	int kills;
	bool alive;
	bool killed;
	int points;
	int stepsAlive;
	bool *attackedBy;
	int player;
};

struct toThread
{
	robobrain Function;
	stepinfo *Info;
	step *Step;
};

#define ACT_MOVE   0
#define ACT_ATTACK 1
#define ACT_TECH   2

#define OBJ_CHARGER -2
#define OBJ_TECH -3
#define OBJ_DEAD -4
#define SEVERAL -5


inline void DoAction(step *Step, int type, int dx, int dy)
{
	if (type != ACT_MOVE && type != ACT_ATTACK)
		return;
	if (type == ACT_MOVE && !dx && !dy)
		return;
	if (Step->actions[type])
		delete Step->actions[type];
	Step->actions[type] = new action;
	Step->actions[type]->dx = dx;
	Step->actions[type]->dy = dy;
}


inline void DoAction(step *Step, int type, int A, int P, int V)
{
	if (type != ACT_TECH || A<0 || P<0 || V<0)
		return;
	if (Step->actions[type])
		delete Step->actions[type];
	Step->actions[type] = new action;
	Step->actions[type]->A = A;
	Step->actions[type]->P = P;
	Step->actions[type]->V = V;
}


inline void DoAction(step *Step, int type, int dx, int dy, int A, int P, int V)
{
	if (type == ACT_MOVE && !dx && !dy)
		return;
	if (type == ACT_TECH && (A<0 || P<0 || V<0))
		return;
	if (Step->actions[type])
		delete Step->actions[type];
	Step->actions[type] = new action;
	Step->actions[type]->dx = dx;
	Step->actions[type]->dy = dy;
	Step->actions[type]->A = A;
	Step->actions[type]->P = P;
	Step->actions[type]->V = V;
}