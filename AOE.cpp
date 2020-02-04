/*
 *问题描述：
小翟为了完成一篇论文，一共要抄袭n篇文章。其中第i篇文章需要a[i]的时间去完成。
小翟可以发动粉丝同时抄袭多篇文章，但存在一些文章，只有当它的若干个前置文章抄袭完成后，
才能开始抄袭该文章。同时我们认为小翟和其粉丝在一篇文章的前置文章都完成后，能马上开始抄袭该文章。
为了让小翟尽快完成论文,获得博士学位，在最优的情况下，小翟的论文什么时候可以完成？

小翟虽然不知道知网，但是他想知道在保证论文尽快完成的情况下（即保证上题答案不变情况），
每篇文章最早什么时候可以开始抄袭，和最晚什么时候可以开始抄袭。
设第i篇文章最早可以抄袭的时间为f[i];在不影响论文完成时间前提下，第i篇文章最晚可以抄袭的时间为g[i].
 请你回答∏i=0-n (g[i]-f[i]+10) 除以10^9+7所得的余数,题目保证有解。

输入：
第一行输入一个整数n，m
第二行输入n个正整数，a1，a2，…，an描述每篇文章抄袭所需要的时间。
接下来读入m行，每行读入两个整数u，v，表示编号为u的文章是编号为v文章的前置文章
所有的输入数据都满足1<=n<=5*10^5，1<=m<=10^6，1<=a[i]<=10^6

输出：
第一行输出一个整数表示论文最早完成时间。
第二行输出一个整数表示∏i=0-n (g[i]-f[i]+10) 除以10^9+7所得的余数

样例输入：
8 9
11 17 16 20 14 12 13 15
1 3
2 4
4 3
3 6
5 6
2 5
6 8
5 7
7 8

样例输出：
80
459599979

备注：
∏i=0-n 为i从0到n的累乘符号
 * 
 */
#include<stdio.h>
#include<iostream>
#include<vector>
#include<queue>
#include<stack>
#include<string.h>

using namespace std;

const long long  INF = 0x0fffffff;
const long long  MAX = 1000000007;
int g_n, g_m;	
stack<int> g_stack;

typedef struct vertex
{
    int in = 0;     //入度
	int out = 0;  //出度
	int w = 0; //边的权重
	long long early;		//表示最早可以完成该活动的时刻。
	long long latest;		//表示在不影响总时间的情况下，最晚可以完成该活动的时刻。
}Vertex;

class Graph
{
private:
	//分别用来标记节点的前驱和后继
	vector<int>* p_n = NULL;  //节点的前驱
	vector<int>* n_n = NULL;  //节点的后继

public:
	vector <Vertex> v;	//拓扑图中的边
	int total_task;

	int f_in(int i){	return v[i].in;		}
	int f_out(int i){	return v[i].out;	}
	int f_p_n(int i,int j){	return p_n[i][j];	}
	int f_n_n(int i,int j){	return n_n[i][j]; }

	Graph(){};
	~Graph(){};
	void init(int n){ 
		total_task = n;
		v.resize(total_task+1); 
		p_n = (vector<int>*)malloc(sizeof(vector<int>)*(n+1));
		n_n = (vector<int>*)malloc(sizeof(vector<int>)*(n+1));
	}
	void make_graph(int prev, int next)
	{
		p_n[next].push_back(prev);	//存储next节点对应的前驱节点
		n_n[prev].push_back(next);	//存储prev节点对应的后继节点

		v[prev].out ++;
		v[next].in ++;
	}

	
};
	
Graph g_graph;

/*
 *　　说明：开始和完成每一篇受限文章对应为一个事件，完成前置文章的时间对应为一个活动，
 *		  采取贪婪策略，我们可以获取到每一篇受限文章完成的最早时间。
 *		  从完成的第一篇受限文章开始，我们选择完成该篇文章所需的前置文章里面，
 *		  花费时间最长的文章，叠加当前的时间,作为该文章最早可以完成的时间。
 * 		  下面使用的是广度优先搜索算法。（AOE网络关键路径）
 */
void BFS_cal_early()
{
	int in_temp[g_graph.total_task+1];
	int push_to_stack_temp;

	for(int i=1;i<=g_graph.total_task;i++){	//设置临时数组，不对入度值直接进行修改
		in_temp[i] = g_graph.v[i].in;
	}

	queue<int> q;
	for(int i=1; i<=g_n; i++){
		if(0 == g_graph.f_in(i)){
			q.push(i);
			g_stack.push(i);
		}
	}
	while(!q.empty()){
        int zero = q.front();   //将入度为0的节点加入到队列中
        q.pop();
        for(int i=0;i<g_graph.f_out(zero);i++){     //访问已经加入队列的顶点的各个出度
            int n_n_num=g_graph.f_n_n(zero,i);
			in_temp[n_n_num] --;
			push_to_stack_temp = n_n_num;
            if(0 == in_temp[n_n_num]){ //当该顶点的入度为零时候，将该顶点加入队列中
                q.push(n_n_num);
				g_stack.push(push_to_stack_temp);      
            }
		// 	//逐步递归，取最大值构成关键路径，即对应节点可以发生的最早开始时间
			g_graph.v[n_n_num].early  = \
			max(g_graph.v[n_n_num].early ,g_graph.v[zero].early + g_graph.v[n_n_num].w);
		}
    }
}

//拓扑有序，其逆拓扑也一定有序，
void cal_latest()
{
	int p_n_num;
	queue<int> q;
	while(!g_stack.empty()){
		q.push(g_stack.top());
		g_stack.pop();
	}	
	while(!q.empty()){
        int zero_out_node = q.front();
        q.pop();
		long long res=INF;
        for(int i=0;i<g_graph.f_in(zero_out_node);i++){     //访问已经加入队列的顶点的各个出度
            p_n_num = g_graph.f_p_n(zero_out_node,i);
			g_graph.v[p_n_num].latest  = \
			min(g_graph.v[p_n_num].latest, g_graph.v[zero_out_node].latest - g_graph.v[zero_out_node].w);
		}
    }
}

/*
 *说明：　设第i篇文章最早可以抄袭的时间为f[i];在不影响论文完成时间前提下，第i篇文章最晚可以抄袭的时间为g[i].
 *		设第i篇文章最早可以完成抄袭的时间为F[i]，则F[i] = f[i] + 完成第i篇文章所需的时间
 *		在不影响论文完成时间前提下，第i篇文章最晚可以完成抄袭的时间为G[i],则G[i] = g[i] + 完成第i篇所需文章的时间。
 *		则有——> f[i]-g[i] = F[i]-G[i]
 */
int main()
{
	freopen("1.txt","r",stdin);
	cin>> g_n >> g_m;
	g_graph.init(g_n);			//文章篇数
	for(int i=1; i<=g_n; i++){
		 cin >> g_graph.v[i].w;
		 g_graph.v[i].early = g_graph.v[i].w;
	}
	int prev,next;

	for(int i=1; i<=g_m; i++)
	{
		cin >> prev >> next;
		g_graph.make_graph(prev,next);
	}

	BFS_cal_early();

	//查找论文完成的最快时间
	int max_flag = 1;
	int temp_max_compare = 0 ;
	for(int i=1; i<=g_graph.total_task; i++){
		if(g_graph.v[i].early > temp_max_compare){
			temp_max_compare = g_graph.v[i].early;
			max_flag = i;
		}
	}

	//在后面逆拓扑排序中各个 lastest_finished_time 会逐渐收敛
	for(int i=1; i<=g_graph.total_task; i++){
		g_graph.v[i].latest = g_graph.v[max_flag].early;
	}

	cal_latest();

	long long sum=1;
    for(int i=1;i<=g_graph.total_task;i++){
        sum=sum*(g_graph.v[i].latest - g_graph.v[i].early +10);
        sum=sum%MAX;
    }
    printf("%lld\n%lld\n",g_graph.v[max_flag].early,sum);
	return 0;
}