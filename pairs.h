//描述了粒子的类，它有下标有位置，而且可以和同类比较他们的位置大小
#ifndef PAIRS
#define PAIRS


class pairs {
public:
	//用于把连续粒子离散化
	double part_x;//粒子位置
	int idx;//对应下标
	pairs() :part_x(-1), idx(-1) {}//默认粒子位置初始值为-1以及它的对应下标也是-1
	pairs(double part_x, int idx) :part_x(part_x), idx(idx) {}

	friend bool operator<(const pairs &a, const pairs &b) {//操作符重载<,可以比较粒子的位置
		return (a.part_x > b.part_x);
	}
};

#endif