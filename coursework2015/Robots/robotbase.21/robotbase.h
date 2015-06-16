// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа ROBOTBASE_EXPORTS,
// в командной строке. Этот символ не должен быть определен в каком-либо проекте
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции ROBOTBASE_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.

#include "robostuff.h"
#ifdef ROBOTBASE_EXPORTS
#define ROBOTBASE_API __declspec(dllexport)
#else
#define ROBOTBASE_API __declspec(dllimport)
#endif

// Этот класс экспортирован из robotbase.dll
class ROBOTBASE_API Crobotbase {
public:
	Crobotbase(void);
	// TODO: Добавьте здесь свои методы.
};

extern ROBOTBASE_API int nrobotbase;

ROBOTBASE_API int fnrobotbase(void);

extern "C" ROBOTBASE_API void DoStep(stepinfo *Info, step *Step);
