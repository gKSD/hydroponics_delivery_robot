#ifndef _TRAVELLING_SALESMAN_HPP_
#define _TRAVELLING_SALESMAN_HPP_

const int maxn = 100;
class Travelling_salesman
// решение непосредственно задачи коммивояжера
{
    private:

		int _n, _i, _s, _min, _count, _found;
		//n-количество городов
		//i-счетчик
		//s-текущая сумма
		//min-минимальная сумма
		//count-счетчик пройденных городов
		//found-найден ли город
		int **a;               //матрица рассояний
		int *_m, *_minm;            //m-текущий путь
		//minm-минимальный путь
		void Search(int x);
    public:		
		Travelling_salesman(int n, int **routes);
		int* Output();
		void Run();
};


#endif
