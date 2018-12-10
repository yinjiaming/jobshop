#ifndef PARTICLE
#define PARTICLE
#define Vmax 1      //最高速度
#define Xmax 5      //最大范围
#define Wmax 3.2    //最大惯性
#define Wmin 0.4    //最小惯性
#define Itermax 600 //最高迭代次数
#define Pmax 300     //种群规模
#define K 5         //最高不变解跳出次数
#include <queue>
#include <cmath>
#include <climits>
#include <cassert>
#include<vector>
#include"pairs.h"
using namespace std;
class particle {
public:
	//粒子
	int n, p, size;//粒子内部信息容量
	int value;//粒子的价值(对应make_span的长短)
	double* x;//位置
	double* v;//速度
	double* Pbest;//个人所到达过的佳点
	int Pb_value;//个人最佳点价值
	int* operations;//用于存储解码出的规划

	particle() :n(0), p(0), size(0), x(nullptr), v(nullptr), \
		Pbest(nullptr), value(INT_MAX), Pb_value(INT_MAX) {}

	particle(int n, int p) :n(n), p(p), size(n*p), value(INT_MAX), Pb_value(INT_MAX) {//指定粒子的一些信息
		x = new double[size];//size维的解
		v = new double[size];//变化量
		Pbest = new double[size];
		operations = new int[size];
	}

	particle(const particle& r) {//这个是用来拷贝构造用的
		n = r.n, p = r.p, size = r.size;
		value = r.value;
		Pb_value = r.Pb_value;
		x = new double[size];
		v = new double[size];
		Pbest = new double[size];
		operations = new int[size];
		for (int i = 0; i < size; ++i) {
			x[i] = r.x[i];
			v[i] = r.v[i];
			Pbest[i] = r.Pbest[i];
			operations[i] = r.operations[i];
		}
	}

	void init() {
		//粒子初始化
		for (int i = 0; i < size; ++i) {//随机生成
			x[i] = 2 * Xmax*rand() / double(RAND_MAX) - Xmax;
			v[i] = 2 * Vmax*rand() / double(RAND_MAX) - Vmax;
		}
	}

	particle& operator=(const particle& r) {//运算符重载‘=’把我这个粒子对象变成（）内传入粒子对象的样子，并返回改变后的对象的引用
		n = r.n, p = r.p, size = r.size;
		value = r.value;
		Pb_value = r.Pb_value;
		x = new double[size];
		v = new double[size];
		Pbest = new double[size];
		operations = new int[size];
		for (int i = 0; i < size; ++i) {
			x[i] = r.x[i];
			v[i] = r.v[i];
			Pbest[i] = r.Pbest[i];
			operations[i] = r.operations[i];
		}
		return *this;
	}

	bool operator<(const particle& r) {//运算符重载对粒子进行比较
		return (value < r.value);
	}

	void ROV() {
		//ROV规则 离散化(解码)
		priority_queue<pairs> q;
		for (int i = 0; i < p; ++i) {
			for (int j = 0; j < n; ++j) {
				q.push(pairs(x[i*n + j], j));
			}
			for (int j = 0; j < n; ++j) {
				operations[i*n + q.top().idx] = j;
				q.pop();
			}
		}
	}

	~particle() {
		//        if (x != nullptr)
		delete[] x;
		//        if (v != nullptr)
		delete[] v;
		//        if (Pbest != nullptr)
		delete[] Pbest;
		//        if (operations != nullptr)
		delete[] operations;
	}

};

#endif