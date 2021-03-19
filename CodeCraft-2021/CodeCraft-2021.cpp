#include <iostream>
#include <string>
#include <map>
#include <cmath>
using namespace std;

// #define DEBUG_POINT
// #define PRINTINFO
// #define FILEINPUT
#define MAX_SERVER_NUM 5000
#define ADD_SERVER_NUM 1000
#define SOLVED 1
#define UNSOLVED 0
#define ADD_VM 1
#define DEL_VM 2
#define NONE 0
#define NODE_A 1
#define NODE_B 2
#define FIT 0.4
#define CHECK_FIT 20
#define CHECK_POINT_OFFSET 0
#define CHECK_DISTANCE_OFFSET 0
#define UNACTIVE 1

// data structure
struct server_info
{
	string name;
	int count;
	int cpu_core;
	int memory;
	int device_cost;
	int energy_cost;
	double cpu_memory;
};
struct server_instance
{
	int type_id;
	int A_cpu_access;
	int A_memory_access;
	int B_cpu_access;
	int B_memory_access;
	double A_cpu_usage;
	double A_memory_usage;
	double B_cpu_usage;
	double B_memory_usage;
};
struct vm_info
{
	int cpu_core;
	int memory;
	int is_double_node;
	double cpu_memory;
};
struct vm_instance
{
	int type_id;
	int server_id;
	int node;
};

struct request
{
	int solved;
	int request;
	int vm_type;
	int vm_id;
	int server_type;
	int server_id;
	int node;
};

struct vm_node
{
	int vm_id;
	vm_node *next;
};

struct transfer
{
	int vm_id;
	int server_id;
	int node;
	transfer *next;
};


// cluster data
int max_server_num;
int server_type_num;
int server_instance_num;
int vm_type_num;
int vm_instance_num;
server_info *server_type_list;
server_instance *server_instance_list;
vm_info *vm_type_list;
vm_node **vm_node_chain;
map<string, int> server_type_index;
map<string, int> vm_type_index;
map<int, vm_instance> vm_instance_map;

// check data
double point_cpu_1;
double point_mem_1;
double point_cpu_2;
double point_mem_2;
double check_distance;

// runtime data
int day_num;
double *sort_cpu_memory;
int *sort_server_type_id;
double *sort_server_usage;
int *sort_server_id;
transfer trans_order_head;


void init_cluster();
void init_server_type_list();
void init_vm_type_list();
void sort_server();
void check_cluster(int server_instance_id);
double check_cond(server_instance s, vm_info v, int node);
int check_and_add(int vm_type_id, int vm_id, int start_sort_index);
int check_and_add(int vm_type_id, int vm_id, request *r);
// int check_and_add(int server_instance_id, int vm_type_id, int vm_id, request *r);
void delete_vm(int vm_id, vm_instance v);
void delete_vm(int vm_id, request *r);
int find_fit_server(double vm_cpu_memory, int vm_cpu, int vm_memory);
void buy_server(int type_id);
int add_vm(int server_instance_id, int vm_type_id, int vm_id, request *r);
int transfer_vm();
void daily_requests(int daily_request_num);
void all_requests();
void fresh(int server_instance_id);
void delete_cluster();
#ifdef PRINTINFO
void print_type_list();
void print_sort_list();
void print_server_info();
void print_usage();
#endif

int main()
{
#ifdef FILEINPUT
	freopen("../../training-data/training-1.txt", "r", stdin);
	freopen("../../training-data/training-1-res.txt", "w", stdout);
#endif
	init_cluster();
	init_server_type_list();
	init_vm_type_list();
	sort_server();
#ifdef PRINTINFO
	print_type_list();
	print_sort_list();
#endif
#ifdef DEBUG_POINT
	cout << "server_type_num: " << server_type_num << endl;
	cout << "server_instance_num: " << server_instance_num << endl;
	cout << "vm_type_num: " << vm_type_num << endl;
	cout << "vm_instance_num: " << vm_instance_num << endl;
#endif
	all_requests();
#ifdef DEBUG_POINT
	cout << "------------------------------------------------endexec" << endl;
#endif
#ifdef PRINTINFO
	print_server_info();
#endif
	delete_cluster();
	fflush(stdout);
	return 0;
}

void init_cluster()
{
	max_server_num = MAX_SERVER_NUM;
	server_type_num = 0;
	server_instance_num = 0;
	vm_type_num = 0;
	vm_instance_num = 0;
	server_instance_list = new server_instance[max_server_num];
	vm_node_chain = new vm_node *[max_server_num];
	for (int i = 0; i < max_server_num; i++)
	{
		vm_node_chain[i] = NULL;
	}
	sort_server_usage = new double[max_server_num];
	sort_server_id = new int[max_server_num];
	trans_order_head.next = NULL;
	point_cpu_1 = 1 + (CHECK_POINT_OFFSET);
	point_mem_1 = -(CHECK_POINT_OFFSET);
	point_cpu_2 = -(CHECK_POINT_OFFSET);
	point_mem_2 = 1 + (CHECK_POINT_OFFSET);
	check_distance = sqrt(point_cpu_1 * point_cpu_1 + point_mem_1 * point_mem_1) + (CHECK_DISTANCE_OFFSET);
}

void init_server_type_list()
{
	cin >> server_type_num;
	server_type_list = new server_info[server_type_num];
	sort_cpu_memory = new double[server_type_num];
	sort_server_type_id = new int[server_type_num];
	char tmp_char = 0;
	int tmp_cpu = 0;
	int tmp_memory = 0;
	int tmp_device_cost = 0;
	int tmp_energy_cost = 0;
	double tmp_cpu_memory = 0;
	string tmp_name;
	for (int i = 0; i < server_type_num; i++)
	{
		cin >> tmp_char;
		cin >> tmp_name;
		cin >> tmp_cpu;
		cin >> tmp_char;
		cin >> tmp_memory;
		cin >> tmp_char;
		cin >> tmp_device_cost;
		cin >> tmp_char;
		cin >> tmp_energy_cost;
		cin >> tmp_char;
		tmp_name.pop_back();
		tmp_cpu_memory = ((double)tmp_cpu) / ((double)tmp_memory);
		server_type_index[tmp_name] = i;
		server_type_list[i].name = tmp_name;
		server_type_list[i].count = 0;
		server_type_list[i].cpu_core = tmp_cpu;
		server_type_list[i].memory = tmp_memory;
		server_type_list[i].device_cost = tmp_device_cost;
		server_type_list[i].energy_cost = tmp_energy_cost;
		server_type_list[i].cpu_memory = tmp_cpu_memory;
		sort_server_type_id[i] = i;
		sort_cpu_memory[i] = tmp_cpu_memory;
	}
}

void init_vm_type_list()
{
	cin >> vm_type_num;
	vm_type_list = new vm_info[vm_type_num];
	char tmp_char = 0;
	int tmp_cpu = 0;
	int tmp_memory = 0;
	int tmp_double_node;
	double tmp_cpu_memory = 0;
	string tmp_name;
	for (int i = 0; i < vm_type_num; i++)
	{
		cin >> tmp_char;
		cin >> tmp_name;
		cin >> tmp_cpu;
		cin >> tmp_char;
		cin >> tmp_memory;
		cin >> tmp_char;
		cin >> tmp_double_node;
		cin >> tmp_char;
		tmp_name.pop_back();
		tmp_cpu_memory = ((double)tmp_cpu) / ((double)tmp_memory);
		vm_type_index[tmp_name] = i;
		vm_type_list[i].cpu_core = tmp_cpu;
		vm_type_list[i].memory = tmp_memory;
		vm_type_list[i].is_double_node = tmp_double_node;
		vm_type_list[i].cpu_memory = tmp_cpu_memory;
	}
}

void mergeSort(double *arr, int *arr_index, int len)
{
	if (len <= 1)
	{
		return;
	}
	int leftlen = len / 2;
	int rightlen = len - leftlen;
	double *left = arr;
	int *left_index = arr_index;
	double *right = arr + leftlen;
	int *right_index = arr_index + leftlen;
	mergeSort(left, left_index, leftlen);
	mergeSort(right, right_index, rightlen);
	double *tmp = new double[len];
	int *tmp_index = new int[len];
	int i = 0;
	int j = leftlen;
	int k = 0;
	while (i < leftlen && j < len)
	{
		if (arr[i] <= arr[j])
		{
			tmp[k] = arr[i];
			tmp_index[k] = arr_index[i];
			i++;
			k++;
		}
		else
		{
			tmp[k] = arr[j];
			tmp_index[k] = arr_index[j];
			j++;
			k++;
		}
	}
	while (i < leftlen)
	{
		tmp[k] = arr[i];
		tmp_index[k] = arr_index[i];
		i++;
		k++;
	}
	while (j < len)
	{
		tmp[k] = arr[j];
		tmp_index[k] = arr_index[j];
		j++;
		k++;
	}
	for (k = 0; k < len; k++)
	{
		arr[k] = tmp[k];
		arr_index[k] = tmp_index[k];
	}
	if (tmp != NULL)
	{
		delete[] tmp;
		tmp = NULL;
	}
	if (tmp_index != NULL)
	{
		delete[] tmp_index;
		tmp_index = NULL;
	}
}

void sort_server()
{
	mergeSort(sort_cpu_memory, sort_server_type_id, server_type_num);
}

void check_cluster(int num) // debug04: 服务器超过初始限制
{
	if (num >= max_server_num)
	{
		int old = max_server_num;
		max_server_num += ADD_SERVER_NUM;
		server_instance *tmp_list = new server_instance[max_server_num];
		vm_node **tmp_chain = new vm_node *[max_server_num];
		double *tmp_sort_usage = new double[max_server_num];
		int *tmp_sort_id = new int[max_server_num];
		for (int i = 0; i < old; i++)
		{
			tmp_list[i] = server_instance_list[i];
			tmp_chain[i] = vm_node_chain[i];
			tmp_sort_usage[i] = sort_server_usage[i];
			tmp_sort_id[i] = sort_server_id[i];
		}
		delete[] server_instance_list;
		delete[] vm_node_chain;
		delete[] sort_server_usage;
		delete[] sort_server_id;
		server_instance_list = new server_instance[max_server_num];
		vm_node_chain = new vm_node *[max_server_num];
		sort_server_usage = new double[max_server_num];
		sort_server_id = new int[max_server_num];
		for (int i = 0; i < max_server_num; i++)
		{
			if (i < old) {
				server_instance_list[i] = tmp_list[i];
				vm_node_chain[i] = tmp_chain[i];
				sort_server_usage[i] = tmp_sort_usage[i];
				sort_server_id[i] = tmp_sort_id[i];
			}
			else {
				vm_node_chain[i] = NULL;
			}
		}
		delete[] tmp_list;
		delete[] tmp_chain;
		delete[] tmp_sort_usage;
		delete[] tmp_sort_id;
	}
}

double check_cond(server_instance s, vm_info v, int node)
{
	if (node == NODE_A)
	{
		double cpu = 1 - (double)(s.A_cpu_access - v.cpu_core) / (double)(server_type_list[s.type_id].cpu_core / 2);
		double mem = 1 - (double)(s.A_memory_access - v.memory) / (double)(server_type_list[s.type_id].memory / 2);
		double distance_point_1 = sqrt((point_cpu_1 - cpu) * (point_cpu_1 - cpu) + (mem - point_mem_1) * (mem - point_mem_1));
		double distance_point_2 = sqrt((point_mem_2 - mem) * (point_mem_2 - mem) + (cpu - point_cpu_2) * (cpu - point_cpu_2));
		if (distance_point_1 < check_distance && distance_point_2 < check_distance)
		{
			return cpu + mem;
		}
	}
	else if (node == NODE_B)
	{
		double cpu = 1 - (double)(s.B_cpu_access - v.cpu_core) / (double)(server_type_list[s.type_id].cpu_core / 2);
		double mem = 1 - (double)(s.B_memory_access - v.memory) / (double)(server_type_list[s.type_id].memory / 2);
		double distance_point_1 = sqrt((1 - cpu) * (1 - cpu) + mem * mem);
		double distance_point_2 = sqrt((1 - mem) * (1 - mem) + cpu * cpu);
		if (distance_point_1 < check_distance && distance_point_2 < check_distance)
		{
			return cpu + mem;
		}
	}
	else
	{
		double cpu_A = 1 - (double)(s.A_cpu_access - v.cpu_core / 2) / (double)(server_type_list[s.type_id].cpu_core / 2);
		double mem_A = 1 - (double)(s.A_memory_access - v.memory / 2) / (double)(server_type_list[s.type_id].memory / 2);
		double cpu_B = 1 - (double)(s.B_cpu_access - v.cpu_core / 2) / (double)(server_type_list[s.type_id].cpu_core / 2);
		double mem_B = 1 - (double)(s.B_memory_access - v.memory / 2) / (double)(server_type_list[s.type_id].memory / 2);
		double distance_point_1_A = sqrt((1 - cpu_A) * (1 - cpu_A) + mem_A * mem_A);
		double distance_point_2_A = sqrt((1 - mem_A) * (1 - mem_A) + cpu_A * cpu_A);
		double distance_point_1_B = sqrt((1 - cpu_B) * (1 - cpu_B) + mem_B * mem_B);
		double distance_point_2_B = sqrt((1 - mem_B) * (1 - mem_B) + cpu_B * cpu_B);
		if (distance_point_1_A < check_distance && distance_point_2_A < check_distance && distance_point_1_B < check_distance && distance_point_2_B < check_distance)
		{
			return cpu_A + mem_A + cpu_B + mem_B;
		}
	}
	return -1;
}

int check_and_add(int vm_type_id, int vm_id, int start_sort_index)
{ // 用于迁移
	int server_instance_id = -1;
	int node = NONE;
	double max_distance = 0;
	vm_info v = vm_type_list[vm_type_id];
	for (int i = start_sort_index; i < server_instance_num; i++)
	{
		server_instance s = server_instance_list[sort_server_id[i]];
		if (v.is_double_node == 0)
		{
			if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
			{
				double tmp = check_cond(s, v, NODE_A);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = sort_server_id[i];
					node = NODE_A;
				}
			}
			else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
			{
				double tmp = check_cond(s, v, NODE_B);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = sort_server_id[i];
					node = NODE_B;
				}
			}
		}
		else
		{
			int node_cpu = v.cpu_core / 2;
			int node_memory = v.memory / 2;
			if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_memory)
			{
				double tmp = check_cond(s, v, NONE);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = sort_server_id[i];
					node = NONE;
				}
			}
		}
	}
	if (server_instance_id != -1)
	{
		if (node == NODE_A)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_A;
			server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].A_memory_access -= v.memory;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
		else if (node == NODE_B)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_B;
			server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].B_memory_access -= v.memory;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
		else
		{
			int node_cpu = v.cpu_core / 2;
			int node_memory = v.memory / 2;
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NONE;
			server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].A_memory_access -= node_memory;
			server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].B_memory_access -= node_memory;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
	}
	return 0;
}

int check_and_add(int vm_type_id, int vm_id, request *r)
{ // debug04 check_and_add重写，使用几何约束
	int server_instance_id = -1;
	int node = NONE;
	double max_distance = 0;
	vm_info v = vm_type_list[vm_type_id];
	for (int i = 0; i < server_instance_num; i++)
	{
		server_instance s = server_instance_list[i];
		if (v.is_double_node == 0)
		{
			if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
			{
				double tmp = check_cond(s, v, NODE_A);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = i;
					node = NODE_A;
				}
			}
			else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
			{
				double tmp = check_cond(s, v, NODE_B);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = i;
					node = NODE_B;
				}
			}
		}
		else
		{
			int node_cpu = v.cpu_core / 2;
			int node_memory = v.memory / 2;
			if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_memory)
			{
				double tmp = check_cond(s, v, NONE);
				if (tmp >= 0 && tmp > max_distance)
				{
					max_distance = tmp;
					server_instance_id = i;
					node = NONE;
				}
			}
		}
	}
	if (server_instance_id != -1)
	{
		if (node == NODE_A)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_A;
			server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].A_memory_access -= v.memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NODE_A;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
		else if (node == NODE_B)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_B;
			server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].B_memory_access -= v.memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NODE_B;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
		else
		{
			int node_cpu = v.cpu_core / 2;
			int node_memory = v.memory / 2;
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NONE;
			server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].A_memory_access -= node_memory;
			server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].B_memory_access -= node_memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NONE;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
	}
	r->solved = UNSOLVED;
	return 0;
}

// int check_and_add(int server_instance_id, int vm_type_id, int vm_id, request *r)
// {
// 	server_instance s = server_instance_list[server_instance_id];
// 	vm_info v = vm_type_list[vm_type_id];
// 	if (abs(server_type_list[s.type_id].cpu_memory - v.cpu_memory) < CHECK_FIT)
// 	{
// 		if (v.is_double_node == 0)
// 		{
// 			int select = NONE;
// 			if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory && s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
// 			{
// 				if (s.A_cpu_access + s.A_memory_access >= s.B_cpu_access + s.B_memory_access)
// 				{
// 					select = NODE_A;
// 				}
// 				else
// 				{
// 					select = NODE_B;
// 				}
// 			}
// 			else if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
// 			{
// 				select = NODE_A;
// 			}
// 			else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
// 			{
// 				select = NODE_B;
// 			}
// 			if (select == NODE_A)
// 			{
// 				vm_instance_map[vm_id].type_id = vm_type_id;
// 				vm_instance_map[vm_id].server_id = server_instance_id;
// 				vm_instance_map[vm_id].node = NODE_A;
// 				server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
// 				server_instance_list[server_instance_id].A_memory_access -= v.memory;
// 				r->solved = SOLVED;
// 				r->server_id = server_instance_id;
// 				r->node = NODE_A;
// 				return 1;
// 			}
// 			else if (select == NODE_B)
// 			{
// 				vm_instance_map[vm_id].type_id = vm_type_id;
// 				vm_instance_map[vm_id].server_id = server_instance_id;
// 				vm_instance_map[vm_id].node = NODE_B;
// 				server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
// 				server_instance_list[server_instance_id].B_memory_access -= v.memory;
// 				r->solved = SOLVED;
// 				r->server_id = server_instance_id;
// 				r->node = NODE_B;
// 				return 1;
// 			}
// 		}
// 		else
// 		{
// 			int node_cpu = v.cpu_core / 2;
// 			int node_memory = v.memory / 2;
// 			if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_memory)
// 			{
// 				vm_instance_map[vm_id].type_id = vm_type_id;
// 				vm_instance_map[vm_id].server_id = server_instance_id;
// 				vm_instance_map[vm_id].node = NONE;
// 				server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
// 				server_instance_list[server_instance_id].A_memory_access -= node_memory;
// 				server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
// 				server_instance_list[server_instance_id].B_memory_access -= node_memory;
// 				r->solved = SOLVED;
// 				r->server_id = server_instance_id;
// 				r->node = NONE;
// 				return 1;
// 			}
// 		}
// 	}
// 	r->solved = UNSOLVED;
// 	return 0;
// }

void delete_vm(int vm_id, vm_instance v)
{  //用于迁移
	int server_id = v.server_id;
	int server_node = v.node;
	int vm_type = v.type_id;
	vm_info v_i = vm_type_list[vm_type];
	int cpu = v_i.cpu_core;
	int memory = v_i.memory;
	if (server_node == NODE_A)
	{
		server_instance_list[server_id].A_cpu_access += cpu;
		server_instance_list[server_id].A_memory_access += memory;
	}
	else if (server_node == NODE_B)
	{
		server_instance_list[server_id].B_cpu_access += cpu;
		server_instance_list[server_id].B_memory_access += memory;
	}
	else
	{
		int node_cpu = cpu / 2;
		int node_memory = memory / 2;
		server_instance_list[server_id].A_cpu_access += node_cpu;
		server_instance_list[server_id].A_memory_access += node_memory;
		server_instance_list[server_id].B_cpu_access += node_cpu;
		server_instance_list[server_id].B_memory_access += node_memory;
	}
	vm_node *p = vm_node_chain[server_id];
	vm_node *q = NULL;
	while (p != NULL)
	{
		if (p->vm_id == vm_id) {
			break;
		}
		q = p;
		p = p->next;
	}
	if (q == NULL) {
		vm_node_chain[server_id] = p->next;
		delete p;
		p = NULL;
	}
	else {
		q->next = p->next;
		delete p;
		p = NULL;
	}
}

void delete_vm(int vm_id, request *r)
{
	vm_instance v = vm_instance_map[vm_id];
	int server_id = v.server_id;
	int server_node = v.node;
	int vm_type = v.type_id;
	vm_info v_i = vm_type_list[vm_type];
	int cpu = v_i.cpu_core;
	int memory = v_i.memory;
	if (server_node == NODE_A)
	{
		server_instance_list[server_id].A_cpu_access += cpu;
		server_instance_list[server_id].A_memory_access += memory;
	}
	else if (server_node == NODE_B)
	{
		server_instance_list[server_id].B_cpu_access += cpu;
		server_instance_list[server_id].B_memory_access += memory;
	}
	else
	{
		int node_cpu = cpu / 2;
		int node_memory = memory / 2;
		server_instance_list[server_id].A_cpu_access += node_cpu;
		server_instance_list[server_id].A_memory_access += node_memory;
		server_instance_list[server_id].B_cpu_access += node_cpu;
		server_instance_list[server_id].B_memory_access += node_memory;
	}
	vm_node *p = vm_node_chain[server_id];
	vm_node *q = NULL;
	while (p != NULL)
	{
		if (p->vm_id == vm_id) {
			break;
		}
		q = p;
		p = p->next;
	}
	if (q == NULL) {
		vm_node_chain[server_id] = p->next;
		delete p;
		p = NULL;
	}
	else {
		q->next = p->next;
		delete p;
		p = NULL;
	}
	vm_instance_map.erase(vm_id);
	r->solved = SOLVED;
}

int binary_search(double *arr, double tar, int len)
{
	int left = 0;
	int right = len - 1;
	int mid;
	while (left < right)
	{
		mid = (left + right) / 2;
		if (tar <= arr[mid])
		{
			right = mid;
		}
		else
		{
			left = mid + 1;
		}
	}
	return left;
}

int find_fit_server(double vm_cpu_memory, int vm_cpu, int vm_memory)
{
	int find_flag = 0;
	int server_type_id = binary_search(sort_cpu_memory, vm_cpu_memory, server_type_num);
	double average_cost = (server_type_list[server_type_id].device_cost + day_num * server_type_list[server_type_id].energy_cost) / server_type_list[server_type_id].cpu_core;
	if (server_type_list[server_type_id].cpu_core >= vm_cpu && server_type_list[server_type_id].memory >= vm_memory)
	{
		find_flag = 1;
	}
	int i = server_type_id + 1;
	while (abs(server_type_list[i].cpu_memory - vm_cpu_memory) < FIT / 2 && i < server_type_num)
	{
		double this_average_cost = (server_type_list[i].device_cost + day_num * server_type_list[i].energy_cost) / server_type_list[i].cpu_core;
		if (this_average_cost < average_cost && server_type_list[i].cpu_core >= vm_cpu && server_type_list[i].memory >= vm_memory)
		{
			average_cost = this_average_cost;
			server_type_id = i;
			find_flag = 1;
		}
		i++;
	}
	i = server_type_id - 1;
	while (abs(server_type_list[i].cpu_memory - vm_cpu_memory) < FIT / 2 && i >= 0)
	{
		double this_average_cost = (server_type_list[i].device_cost + day_num * server_type_list[i].energy_cost) / server_type_list[i].cpu_core;
		if (this_average_cost < average_cost && server_type_list[i].cpu_core >= vm_cpu && server_type_list[i].memory >= vm_memory)
		{
			average_cost = this_average_cost;
			server_type_id = i;
			find_flag = 1;
		}
		i--;
	}
	if (find_flag == 1)
	{
		return server_type_id;
	}
	else
	{
		double fit_value = 100000;
		int index = -1;
		for (i = 0; i < server_type_num; i++)
		{
			if (server_type_list[i].cpu_core >= vm_cpu && server_type_list[i].memory >= vm_memory)
			{
				if (abs(server_type_list[i].cpu_memory - vm_cpu_memory) < fit_value)
				{
					fit_value = abs(server_type_list[i].cpu_memory - vm_cpu_memory);
					index = i;
				}
			}
		}
		return index;
	}
}

void buy_server(int type_id)
{
	check_cluster(server_instance_num);
	server_instance_list[server_instance_num].type_id = type_id;
	int node_cpu = server_type_list[type_id].cpu_core / 2;
	int node_memory = server_type_list[type_id].memory / 2;
	server_instance_list[server_instance_num].A_cpu_access = node_cpu;
	server_instance_list[server_instance_num].A_memory_access = node_memory;
	server_instance_list[server_instance_num].B_cpu_access = node_cpu;
	server_instance_list[server_instance_num].B_memory_access = node_memory;
	server_instance_num++;
	server_type_list[type_id].count++;
}

int add_vm(int server_instance_id, int vm_type_id, int vm_id, request *r)
{
	server_instance s = server_instance_list[server_instance_id];
	vm_info v = vm_type_list[vm_type_id];
	if (v.is_double_node == 0)
	{
		int select = NONE;
		if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory && s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
		{
			if (s.A_cpu_access + s.A_memory_access >= s.B_cpu_access + s.B_memory_access)
			{
				select = NODE_A;
			}
			else
			{
				select = NODE_B;
			}
		}
		else if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
		{
			select = NODE_A;
		}
		else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
		{
			select = NODE_B;
		}
		if (select == NODE_A)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_A;
			server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].A_memory_access -= v.memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NODE_A;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
		else if (select == NODE_B)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_B;
			server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].B_memory_access -= v.memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NODE_B;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
	}
	else
	{
		int node_cpu = v.cpu_core / 2;
		int node_memory = v.memory / 2;
		if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_memory)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NONE;
			server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].A_memory_access -= node_memory;
			server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].B_memory_access -= node_memory;
			r->solved = SOLVED;
			r->server_id = server_instance_id;
			r->node = NONE;
			vm_node *new_vm = new vm_node;
			new_vm->vm_id = vm_id;
			new_vm->next = vm_node_chain[server_instance_id];
			vm_node_chain[server_instance_id] = new_vm;
			return 1;
		}
	}
	r->solved = UNSOLVED;
	return 0;
}

int transfer_vm() {
	transfer *tail = &trans_order_head;
	int trans_num = 0;
	if (trans_num + 1 > vm_instance_num * 5 / 1000) {
		return 0;
	}
	for (int i = 0; i < server_instance_num; i++) {
		fresh(i);
		sort_server_id[i] = i;
		sort_server_usage[i] = (server_instance_list[i].A_cpu_usage + server_instance_list[i].A_memory_usage + server_instance_list[i].B_cpu_usage + server_instance_list[i].B_memory_usage) / 4.0;
	}
	mergeSort(sort_server_usage, sort_server_id, server_instance_num);
	int stop = 0;
	for(int i = 0; i < server_instance_num; i++) {
		if (stop == 1 || sort_server_usage[i] > UNACTIVE) {
			break;
		}
		if (sort_server_usage[i] == 0) {
			continue;
		}
		int server_id = sort_server_id[i];
		vm_node *p = vm_node_chain[server_id];
		while (p != NULL)
		{
			int vm_id = p->vm_id;
			p = p->next;
			vm_instance old_vm = vm_instance_map[vm_id];
			int vm_type = old_vm.type_id;
			int flag = check_and_add(vm_type, vm_id, i + 1);
			if (flag == 1) {
				delete_vm(vm_id, old_vm);
				trans_num++;
				transfer *new_t = new transfer;
				new_t->vm_id = vm_id;
				new_t->server_id = vm_instance_map[vm_id].server_id;
				new_t->node = vm_instance_map[vm_id].node;
				new_t->next = NULL;
				tail->next = new_t;
				tail = new_t;
				if (trans_num + 1 > vm_instance_num * 5 / 1000) {
					stop = 1;
					break;
				}
			}
		}
	}
	return trans_num;
}

void daily_requests(int daily_request_num)
{
#ifdef DEBUG_POINT
	cout << "daily_request_num: " << daily_request_num << endl;
#endif
	if (daily_request_num == 0)
	{ // debug03: 如果请求为0，依然会进入循环，购买服务器
		cout << "(purchase,0)" << endl;
		cout << "(migration, 0)" << endl;
		return;
	}
#ifdef DEBUG_POINT
	cout << "------------------------------------------------point1" << endl;
#endif
	int trans = transfer_vm();
	request *daily_request_list;
	int *sort_request_id;
	double *sort_vm_cpu_memory;
	int *buying_list;
	map<int, int> buying_num;
	int buying_count = 0;
	daily_request_list = new request[daily_request_num];
	sort_request_id = new int[daily_request_num];
	sort_vm_cpu_memory = new double[daily_request_num];
	buying_list = new int[server_type_num];
	char tmp_char;
	int tmp_vm_id = 0;
	int tmp_type_id = 0;
	string tmp_request;
	string tmp_name;
	for (int i = 0; i < daily_request_num; i++)
	{
		sort_request_id[i] = i;
		cin >> tmp_char;
		cin >> tmp_request;
		tmp_request.pop_back();
		if (tmp_request == "add")
		{
			vm_instance_num++;
			daily_request_list[i].request = ADD_VM;
			cin >> tmp_name;
			tmp_name.pop_back();
			tmp_type_id = vm_type_index[tmp_name];
			daily_request_list[i].vm_type = tmp_type_id;
			sort_vm_cpu_memory[i] = vm_type_list[tmp_type_id].cpu_memory;
		}
		else
		{
			daily_request_list[i].request = DEL_VM;
			sort_vm_cpu_memory[i] = 0;
		}
		cin >> tmp_vm_id;
		cin >> tmp_char;
		daily_request_list[i].vm_id = tmp_vm_id;
		if (tmp_request == "add")
		{
			if (check_and_add(tmp_type_id, tmp_vm_id, &daily_request_list[i]) == 1)
			{
				sort_vm_cpu_memory[i] = 0; // todo
			}
			// for (int j = 0; j < server_instance_num; j++)
			// {
			// 	if (check_and_add(j, tmp_type_id, tmp_vm_id, &daily_request_list[i]) == 1)
			// 	{
			// 		sort_vm_cpu_memory[i] = 0;
			// 		break;
			// 	}
			// }
		}
	}
#ifdef DEBUG_POINT
	cout << "------------------------------------------------point2" << endl;
#endif
	mergeSort(sort_vm_cpu_memory, sort_request_id, daily_request_num);
	if (sort_vm_cpu_memory[daily_request_num - 1] != 0)
	{
		int start_index = 0;
		double start = sort_vm_cpu_memory[start_index];
#ifdef DEBUG_POINT
		cout << daily_request_num << endl;
		for (int i = 0; i < daily_request_num; i++)
		{
			cout << sort_request_id[i] << ":" << sort_vm_cpu_memory[i] << " ";
		}
		cout << endl;
#endif
		int max_cpu = 0; // debug02: 如果命令是del，vm_type无初值，出现越界错误
		int max_memory = 0;
		int sum_cpu = 0;
		int sum_memory = 0;
#ifdef DEBUG_POINT
		cout << "------------------------------------------------point3" << endl;
#endif
		for (int i = 0; i <= daily_request_num; i++)
		{
			if (i != daily_request_num && sort_vm_cpu_memory[i] == 0)
			{
				start_index++;
				start = sort_vm_cpu_memory[start_index];
			}
			else
			{
				if (i == daily_request_num || abs(sort_vm_cpu_memory[i] - start) > FIT)
				{
#ifdef DEBUG_POINT
					cout << "------------------------------------------------point4" << endl;
#endif
					double average = (double)(sum_cpu) / (double)(sum_memory);
					int server_type_id = find_fit_server(average, max_cpu, max_memory);
#ifdef DEBUG_POINT
					cout << "------------------------------------------------point5" << endl;
#endif
					if (server_type_id != -1)
					{
						buying_num[server_type_id] = 0;
						for (int j = start_index; j < i; j++)
						{
							daily_request_list[sort_request_id[j]].server_type = server_type_id;
						}
					}
					else
					{ // todo
						for (int j = start_index; j < i; j++)
						{
							int find_vm_cpu = vm_type_list[daily_request_list[sort_request_id[j]].vm_type].cpu_core;
							int find_vm_memory = vm_type_list[daily_request_list[sort_request_id[j]].vm_type].memory;
							if (vm_type_list[daily_request_list[sort_request_id[j]].vm_type].is_double_node == 0)
							{
								find_vm_cpu *= 2;
								find_vm_memory *= 2;
							}
							server_type_id = find_fit_server(vm_type_list[daily_request_list[sort_request_id[j]].vm_type].cpu_memory, find_vm_cpu, find_vm_memory);
							daily_request_list[sort_request_id[j]].server_type = server_type_id;
							buying_num[server_type_id] = 0;
						}
					}
					if (i < daily_request_num) // debug01: i越界错误
					{
						start_index = i;
						start = sort_vm_cpu_memory[start_index];
						if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].is_double_node == 1)
						{
							max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
							max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
							sum_cpu = max_cpu;
							sum_memory = max_memory;
						}
						else
						{
							max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core * 2;
							max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory * 2;
							sum_cpu = max_cpu;
							sum_memory = max_memory;
						}
					}
				}
				else
				{
					if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].is_double_node == 1)
					{
						if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core > max_cpu)
						{
							max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
						}
						if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory > max_memory)
						{
							max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
						}
						sum_cpu += vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
						sum_memory += vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
					}
					else
					{
						if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core * 2 > max_cpu)
						{
							max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core * 2;
						}
						if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory * 2 > max_memory)
						{
							max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory * 2;
						}
						sum_cpu += vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
						sum_memory += vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
					}
				}
			}
		}
	}
#ifdef DEBUG_POINT
	cout << "------------------------------------------------point6" << endl;
#endif
	map<int, int>::iterator iter;
	for (iter = buying_num.begin(); iter != buying_num.end(); iter++)
	{
		int server_type = iter->first;
		int cur_server_instance = server_instance_num - 1;
		for (int i = 0; i < daily_request_num; i++)
		{
			if (daily_request_list[i].solved == SOLVED)
			{
				continue;
			}
			else if (daily_request_list[i].request == ADD_VM && daily_request_list[i].server_type == server_type)
			{
				int flag = 0;
				if (iter->second > 0)
				{
					flag = add_vm(cur_server_instance, daily_request_list[i].vm_type, daily_request_list[i].vm_id, &daily_request_list[i]);
				}
				if (flag == 0)
				{
					int if_solved = check_and_add(daily_request_list[i].vm_type, daily_request_list[i].vm_id, &daily_request_list[i]);
					if (if_solved == 0)
					{
						if (iter->second == 0)
						{
							buying_list[buying_count] = server_type;
							buying_count++;
						}
						buy_server(server_type);
						iter->second++;
						cur_server_instance = server_instance_num - 1;
						flag = add_vm(cur_server_instance, daily_request_list[i].vm_type, daily_request_list[i].vm_id, &daily_request_list[i]);
						if (flag == 0)
						{
							cout << "-----------------------------------------------------------------no add\n";
						}
					}
				}
			}
		}
	}
	cout << "(purchase, " << buying_count << ")" << endl;
	for (int i = 0; i < buying_count; i++)
	{
		cout << "(" << server_type_list[buying_list[i]].name << ", " << buying_num[buying_list[i]] << ")" << endl;
	}
	cout << "(migration, " << trans << ")" << endl;
	transfer *head = trans_order_head.next;
	trans_order_head.next = NULL;
	transfer *del = NULL;
	while(head != NULL) {
		if(head->node == NODE_A) {
			cout << "(" << head->vm_id << ", " << head->server_id << ", A)" << endl;
		}
		else if(head->node == NODE_B) {
			cout << "(" << head->vm_id << ", " << head->server_id << ", B)" << endl;
		}
		else {
			cout << "(" << head->vm_id << ", " << head->server_id << ")" << endl;
		}
		del = head;
		head = head->next;
		delete del;
		del = NULL;
	}
	for (int i = 0; i < daily_request_num; i++)
	{
#ifdef DEBUG_POINT
		cout << daily_request_list[i].vm_id << ":" << endl;
#endif
		if (daily_request_list[i].request == ADD_VM)
		{
			if (daily_request_list[i].solved == UNSOLVED)
			{
				cout << "==========================UNSOLVED\n";
			}
			if (daily_request_list[i].node == NODE_A)
			{
				cout << "(" << daily_request_list[i].server_id << ", A)" << endl;
				if (vm_type_list[daily_request_list[i].vm_type].is_double_node == 1)
				{
					cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<error\n";
				}
			}
			else if (daily_request_list[i].node == NODE_B)
			{
				cout << "(" << daily_request_list[i].server_id << ", B)" << endl;
				if (vm_type_list[daily_request_list[i].vm_type].is_double_node == 1)
				{
					cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<error\n";
				}
			}
			else
			{
				cout << "(" << daily_request_list[i].server_id << ")" << endl;
				if (vm_type_list[daily_request_list[i].vm_type].is_double_node == 0)
				{
					cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<error\n";
				}
			}
		}
		else
		{
			delete_vm(daily_request_list[i].vm_id, &daily_request_list[i]);
			vm_instance_num--;
		}
	}
	if (daily_request_list != NULL)
	{
		delete[] daily_request_list;
		daily_request_list = NULL;
	}
	if (sort_request_id != NULL)
	{
		delete[] sort_request_id;
		sort_request_id = NULL;
	}
	if (sort_vm_cpu_memory != NULL)
	{
		delete[] sort_vm_cpu_memory;
		sort_vm_cpu_memory = NULL;
	}
	if (buying_list != NULL)
	{
		delete[] buying_list;
		buying_list = NULL;
	}
	buying_num.clear();
}

void all_requests()
{
	cin >> day_num;
	for (int i = 0; i < day_num; i++)
	{
		int daily_request_num = 0;
		cin >> daily_request_num;
#ifdef PRINTINFO
		cout << "------------------------------------------------cin:" << cin.fail() << endl;
		cout << "------------------------------------------------day" << i << endl;
#endif
		daily_requests(daily_request_num);
#ifdef PRINTINFO
		print_usage();
		cout << "---------------------------------------------endday" << i << endl;
#endif
	}
}

void fresh(int server_instance_id)
{
	server_instance_list[server_instance_id].A_cpu_usage = (double)(server_type_list[server_instance_list[server_instance_id].type_id].cpu_core / 2 - server_instance_list[server_instance_id].A_cpu_access) / (double)(server_type_list[server_instance_list[server_instance_id].type_id].cpu_core / 2);
	server_instance_list[server_instance_id].B_cpu_usage = (double)(server_type_list[server_instance_list[server_instance_id].type_id].cpu_core / 2 - server_instance_list[server_instance_id].B_cpu_access) / (double)(server_type_list[server_instance_list[server_instance_id].type_id].cpu_core / 2);
	server_instance_list[server_instance_id].A_memory_usage = (double)(server_type_list[server_instance_list[server_instance_id].type_id].memory / 2 - server_instance_list[server_instance_id].A_memory_access) / (double)(server_type_list[server_instance_list[server_instance_id].type_id].memory / 2);
	server_instance_list[server_instance_id].B_memory_usage = (double)(server_type_list[server_instance_list[server_instance_id].type_id].memory / 2 - server_instance_list[server_instance_id].B_memory_access) / (double)(server_type_list[server_instance_list[server_instance_id].type_id].memory / 2);
}

void delete_cluster()
{
	if (server_type_list != NULL)
	{
		delete[] server_type_list;
		server_type_list = NULL;
	}
	if (server_instance_list != NULL)
	{
		delete[] server_instance_list;
		server_instance_list = NULL;
	}
	if (vm_type_list != NULL)
	{
		delete[] vm_type_list;
		vm_type_list = NULL;
	}
	if (sort_cpu_memory != NULL)
	{
		delete[] sort_cpu_memory;
		sort_cpu_memory = NULL;
	}
	if (sort_server_type_id != NULL)
	{
		delete[] sort_server_type_id;
		sort_server_type_id = NULL;
	}
	if (sort_server_usage != NULL) {
		delete[] sort_server_usage;
		sort_server_usage = NULL;
	}
	if (sort_server_id != NULL) {
		delete[] sort_server_id;
		sort_server_id = NULL;
	}
	for (int i = 0; i < max_server_num; i++)
	{
		vm_node *p = vm_node_chain[i];
		vm_node *q = NULL;
		while (p != NULL)
		{
			q = p;
			p = p->next;
			delete q;
			q = NULL;
		}
		cout << endl;
	}
	if (vm_node_chain != NULL) {
		delete[] vm_node_chain;
		vm_node_chain = NULL;
	}
}

#ifdef PRINTINFO
void print_type_list()
{
	cout << "debug-print-------------------------------------------" << endl;
	for (int i = 0; i < server_type_num; i++)
	{
		cout << server_type_list[i].cpu_core << " ";
		cout << server_type_list[i].memory << " ";
		cout << server_type_list[i].device_cost << " ";
		cout << server_type_list[i].energy_cost << endl;
	}
	cout << "------------------------------------------------------" << endl;
	for (int i = 0; i < vm_type_num; i++)
	{
		cout << vm_type_list[i].cpu_core << " ";
		cout << vm_type_list[i].memory << " ";
		cout << vm_type_list[i].is_double_node << endl;
	}
	cout << "------------------------------------------------------" << endl;
}
void print_sort_list()
{
	cout << "sort-print--------------------------------------------" << endl;
	for (int i = 0; i < server_type_num; i++)
	{
		cout << sort_server_type_id[i] << " ";
		cout << sort_cpu_memory[i] << " " << endl;
	}
	cout << "------------------------------------------------------" << endl;
}

void print_server_info()
{
	cout << "----------------------------------nosort" << endl;
	for (int i = 0; i < server_type_num; i++)
	{
		cout << server_type_list[i].name << ": " << server_type_list[i].count << endl;
	}
	cout << "------------------------------------sort" << endl;
	for (int i = 0; i < server_type_num; i++)
	{
		cout << server_type_list[sort_server_type_id[i]].name << ": " << server_type_list[sort_server_type_id[i]].count << endl;
	}
}

void print_usage()
{
	for (int i = 0; i < server_instance_num; i++)
	{
		fresh(i);
		cout << i << "|| ";
		cout << "A_cpu: " << server_instance_list[i].A_cpu_usage * 100 << "%"
			 << "  ";
		cout << "A_mem: " << server_instance_list[i].A_memory_usage * 100 << "%"
			 << "  ";
		cout << "B_cpu: " << server_instance_list[i].B_cpu_usage * 100 << "%"
			 << "  ";
		cout << "B_mem: " << server_instance_list[i].B_memory_usage * 100 << "%" << endl;
	}
}

#endif