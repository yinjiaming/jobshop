#include <iostream>

//
//  main.cpp
//  JSP-PSO
//
//
//  使用中级元启发式算法解决JSP问题
//  使用PSO粒子群优化算法，多次迭代使其收敛于最优解
//

#include <cmath>
#include <climits>
#include <queue>
#include <ctime>
#include <cassert>
#include<fstream>
#include<string>
#include<vector>
#include"step.h"
#include"pairs.h"
#include"particle.h"

using namespace std;
//参数
#define Vmax 1      //最高速度
#define Xmax 5      //最大范围
#define Wmax 2.2    //最大惯性
#define Wmin 0.4    //最小惯性
#define Itermax 600 //最高迭代次数
#define Pmax 100     //种群规模
#define K 5         //最高不变解跳出次数


//功能函数切割字符串，切割符可以自定义
vector<string> splitString(const string& s,const string& s1)

{

	vector<string> ans;
	int len = s.length();
	if (len == 0) return ans;
	for (int i = 0; i < len;) {
		int pos = s.find(s1, i);
		if (pos != string::npos) {
			if (pos == i) {//跳过多个连续的空格
				i = pos + 1;
				continue;
			}
			else {
				string strTemp = s.substr(i, pos - i);
				ans.push_back(strTemp);
				i = pos + 1;
			}
		}

		else {
			string strTemp = s.substr(i, len - i);
			ans.push_back(strTemp);
			break;
		}
	}
	return ans;
}
//功能函数，将传入的字符串数字转化为int类型
int changestr(string str)
 {
	   int len = str.length();
	   int sum = 0;
	   float carry = 1.0 / 10;
	   for (int i = 0; i < len; i++)
		    {
		          carry *= 10;
		          sum += (str[len - 1 - i] - '0')*carry;
		    }
	        return sum;
	 }
void init(int& n,int& m,int& p,int& size,const string& filename)
{
	vector<string> v=splitString(filename,"_");
	vector<string>::iterator it=v.begin();
	it++;
	n=changestr(*(++it));
	vector<string> v2=splitString(*(++it),".");
	vector<string>::iterator it2=v2.begin();
	m=changestr(*(it2));
	p=m;
	size = n*p;

}
int main(int argc, const char * argv[]) {
	//主函数
//    freopen("/users/LLL/Desktop/la12.in", "r", stdin);
	//基础输入

	string filename="E:\\bottleneck_3_10_5.jsm";
	cout<<"begin"<<endl;
	clock_t t1 = clock();
	int n, m, p, size;//n个工件,m台机器,p道工序,矩阵规模size
	init(n,m,p,size,filename);
	int** cost = new int*[2];
	for (int i = 0; i < 2; ++i)
	{
		cost[i] = new int[4];
	}
	srand(int(time(NULL)));
	//内存分配
	step** steps = new step*[n + 1];//定义了n+1步
	particle *parts = new particle[Pmax];
	int* Tm = new int[m];//代表每台机器上的累计加工时间
	int* Tp = new int[n];//代表每个工件上的累计加工时间
	int* idx = new int[n];//代表每个工件当前进行到的工序数

	//工序、时间 信息输入，steps相当于一个二维表
	for (int i = 0; i < n; ++i) {
		steps[i] = new step[p];
	}
	fstream f(filename);//创建一个fstream文件流对象
	vector<string>  words; //创建一个vector<string>对象
	string line; //保存读入的每一行
	getline(f, line);
	int i = 0;
	while (getline(f, line))//会自动把\n换行符去掉 
	{
		vector<string> ans = splitString(line,"\t");
		vector<string>::iterator it= ans.begin();
		cost[1][i] = changestr(*it);
		it++;
		it++;
		cost[0][i] = changestr(*it);
		it++;
		for (int j = 0; j < p; j++)
		{
			steps[i][j].m_NO = changestr(*it);
			it++;
			steps[i][j].duration = changestr(*it);
			it++;
			steps[i][j].j_NO = i;
		}
		i++;
	}


	//参数初始化
	double w = Wmin, c1 = 2, c2 = 2;
	particle Gbest(n, p);
	double make_span_old = 0, make_span_new = 0;

	//粒子群初始化
	for (int i = 0; i < Pmax; ++i) {
		parts[i] = particle(n, p);//n,p是粒子内部的一些信息，描述了一共有几个工件几道工序
		parts[i].init();//随机初始化
	}



	int iter_times = 0, ctr = 0;
	bool updated = false;
	for (iter_times = 0; iter_times < Itermax; ++iter_times) {
		updated = false;//用于判断解是否更新过
		particle* min = &parts[0];//把第一个粒子内存的地址赋给min指针
		for (int i = 0; i < Pmax; ++i) {
			//解码
			parts[i].ROV();//离散化
			

			//初始化
			for (int j = 0; j < m; ++j) { Tm[j] = 0; }
			for (int j = 0; j < n; ++j) { Tp[j] = 0; }
			for (int j = 0; j < n; ++j) { idx[j] = 0; }
			//计算目标函数值
			int NO=0;
			int T=0;
			int R=0;
			for (int j = 0; j < size; ++j) {
				NO = parts[i].operations[j];
				T = steps[NO][idx[NO]].duration;
				R = (Tp[NO] > Tm[steps[NO][idx[NO]].m_NO] ? Tp[NO] : Tm[steps[NO][idx[NO]].m_NO]);
				Tp[NO] = R + T;
				Tm[steps[NO][idx[NO]].m_NO] = R + T;
				++idx[NO];//把当前工件进度加1
			}
			//max记录了工程花费的时间
			
			int max = 0;
			for (int j = 0; j < n; ++j) {

				max+=(Tp[j] - cost[0][j])*cost[1][j];
			}

			parts[i].value = max;
			assert(max > 0);
			//搜索局部最优
			if (parts[i].value < parts[i].Pb_value) {
				parts[i].Pb_value = parts[i].value;
				for (int j = 0; j < size; ++j) {
					parts[i].Pbest[j] = parts[i].x[j];
				}
			}
			//搜索全局最优
			if (parts[i].value < (*min).value) {
				min = &parts[i];
			}
		}
		/*for (int j = 0; j < size; j++) {
			cout << parts[1].operations[j] << " ";
		}
		cout << "" << endl;*/
		//更新全局最优
/* 		for(int j=0;j<size;++j)
		{
			cout<<parts[1].operations[j]<<" ";
			if((j+11)%10==0)
			{
				cout<<""<<endl;
			}
		}
		cout<<"--------------------"<<endl; */
		if (min->value < Gbest.value) {

			updated = true;
			Gbest = *min;
		}



		//粒子迭代
		for (int k = 0; k < Pmax; ++k) {
			for (int i = 0; i < size; ++i) {
				parts[k].v[i] = w * parts[k].v[i] + \
					c1*rand() / double(RAND_MAX)*(parts[k].Pbest[i] - parts[k].x[i]) + \
					c2*rand() / double(RAND_MAX)*(Gbest.x[i] - parts[k].x[i]);
				parts[k].x[i] += parts[k].v[i];
				
				//限制在范围内 引入随机化因素
				if (fabs(parts[k].v[i]) > Vmax) {
					parts[k].v[i] = (1 - 0.1*rand() / double(RAND_MAX))* \
						(parts[k].v[i] > 0 ? Vmax : -Vmax);
				}
				if (fabs(parts[k].x[i]) > Xmax) {
					parts[k].x[i] = (1 - 0.1*rand() / double(RAND_MAX))* \
						(parts[k].x[i] > 0 ? Xmax : -Xmax);
				}
			}
		}
		//cout << parts[5].x[5] << endl;
		//动态参数更新
		w = Wmin +(Wmax-Wmin)/(double(n) * iter_times);
		w = Wmin +(Wmax-Wmin)/(double(n) * iter_times);
		make_span_new = Gbest.value;
		//判定是否继续迭代(连续K次解都不变则跳出)
		if (updated && fabs(make_span_new - make_span_old) < 0.000000001 && ++ctr > K) {
			break;
		}
        cout<<make_span_new<<endl;
		make_span_old = make_span_new;
	}


	clock_t t2 = clock();
	float diff = ((float)(t2 - t1) / 1000000.0F);
	printf("%f\n", diff);

	cout << make_span_new <<"the best one"<< endl;


	delete[] parts;
	for (int i = 0; i < n; ++i) {
		delete[] steps[i];
	}
	return 0;
}


