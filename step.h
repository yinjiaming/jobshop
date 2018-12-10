//这个类描述了工件所在机器完成的时间
#ifndef STEP
#define STEP

class step {
public:
	//工序
	int j_NO;//对应工件序号
	int m_NO;//对应机器序号
	int duration;//完成工序所需时间
	step() :j_NO(0), m_NO(0), duration(0) {}//默认工件序号，机器序号，完成工序所需时间都初始化为0
	step(int j_NO, int m_NO, int t) :j_NO(j_NO), m_NO(m_NO), duration(t) {}
};


#endif