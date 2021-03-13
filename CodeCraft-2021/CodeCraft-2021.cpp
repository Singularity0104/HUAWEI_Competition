#include <iostream>
#include <string>
#include <map>
using namespace std;

// #define DEBUG
#define MAX_SERVER_NUM 5000
#define ADD_SERVER_NUM 1000
#define SOLOVED 1
#define UNSLOVED 0
#define ADD_VM 1
#define DEL_VM 2
#define NONE 0
#define NODE_A 1
#define NODE_B 2
#define FIT 0.3

// data structure
struct server_info
{
	string name;
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
	int B_cpu_access;
	int A_memory_access;
	int B_memory_access;
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
	int sloved;
	int request;
	int vm_type;
	int vm_id;
	int server_type;
	int server_id;
	int node;
};

// cluster data
int server_type_num;
int server_instance_num;
int vm_type_num;
int vm_instance_num;
server_info *server_type_list;
server_instance *server_instance_list;
vm_info *vm_type_list;
map<string, int> server_type_index;
map<string, int> vm_type_index;
map<int, vm_instance> vm_instance_map;

// runtime data
int day_num;
double *sort_cpu_memory;
int *sort_server_type_id;

void init_cluster();
void init_server_type_list();
void init_vm_type_list();
void sort_server();
int check_and_add(int server_instance_id, int vm_type_id, int vm_id, request *r);
void delete_vm(int vm_id, request *r);
int find_fit_server(double vm_cpu_memory, int vm_cpu, int vm_memory);
void buy_server(int type_id);
int add_vm(int server_instance_id, int vm_type_id, int vm_id, request *r);
void daily_requests();
void all_requests();
void delete_cluster();
#ifdef DEBUG
void print_type_list();
void print_sort_list();
#endif

int main()
{
	init_cluster();
	init_server_type_list();
	init_vm_type_list();
	sort_server();
	cin >> day_num;
	all_requests();
#ifdef DEBUG
	print_type_list();
	print_sort_list();
#endif
	delete_cluster();
	return 0;
}

void init_cluster()
{
	server_type_num = 0;
	server_instance_num = 0;
	vm_type_num = 0;
	vm_instance_num = 0;
	server_instance_list = new server_instance[MAX_SERVER_NUM];
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
	delete[] tmp;
	delete[] tmp_index;
}

void sort_server()
{
	mergeSort(sort_cpu_memory, sort_server_type_id, server_type_num);
}

int check_and_add(int server_instance_id, int vm_type_id, int vm_id, request *r)
{
	server_instance s = server_instance_list[server_instance_id];
	vm_info v = vm_type_list[vm_type_id];
	if (abs(server_type_list[s.type_id].cpu_memory - v.cpu_memory) < FIT)
	{
		if (v.is_double_node == 0)
		{
			if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
			{
				vm_instance_map[vm_id].type_id = vm_type_id;
				vm_instance_map[vm_id].server_id = server_instance_id;
				vm_instance_map[vm_id].node = NODE_A;
				server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
				server_instance_list[server_instance_id].A_memory_access -= v.memory;
				r->sloved = SOLOVED;
				r->server_id = server_instance_id;
				r->node = NODE_A;
				return 1;
			}
			else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
			{
				vm_instance_map[vm_id].type_id = vm_type_id;
				vm_instance_map[vm_id].server_id = server_instance_id;
				vm_instance_map[vm_id].node = NODE_B;
				server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
				server_instance_list[server_instance_id].B_memory_access -= v.memory;
				r->sloved = SOLOVED;
				r->server_id = server_instance_id;
				r->node = NODE_B;
				return 1;
			}
		}
		else
		{
			int node_cpu = v.cpu_core / 2;
			int node_memory = v.memory / 2;
			if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_cpu)
			{
				vm_instance_map[vm_id].type_id = vm_type_id;
				vm_instance_map[vm_id].server_id = server_instance_id;
				vm_instance_map[vm_id].node = NONE;
				server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
				server_instance_list[server_instance_id].A_memory_access -= node_memory;
				server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
				server_instance_list[server_instance_id].B_memory_access -= node_memory;
				r->sloved = SOLOVED;
				r->server_id = server_instance_id;
				r->node = NONE;
				return 1;
			}
		}
	}
	r->sloved = UNSLOVED;
	return 0;
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
	vm_instance_map.erase(vm_id);
	r->sloved = SOLOVED;
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
		for (i = 0; i < server_type_num; i++)
		{
			if (server_type_list[i].cpu_core >= vm_cpu && server_type_list[i].memory >= vm_memory)
			{
				return i;
			}
		}
		return -1;
	}
}

void buy_server(int type_id)
{
	server_instance_list[server_instance_num].type_id = type_id;
	int node_cpu = server_type_list[type_id].cpu_core / 2;
	int node_memory = server_type_list[type_id].memory / 2;
	server_instance_list[server_instance_num].A_cpu_access = node_cpu;
	server_instance_list[server_instance_num].A_memory_access = node_memory;
	server_instance_list[server_instance_num].B_cpu_access = node_cpu;
	server_instance_list[server_instance_num].B_memory_access = node_memory;
	server_instance_num++;
}

int add_vm(int server_instance_id, int vm_type_id, int vm_id, request *r)
{
	server_instance s = server_instance_list[server_instance_id];
	vm_info v = vm_type_list[vm_type_id];
	if (v.is_double_node == 0)
	{
		if (s.A_cpu_access >= v.cpu_core && s.A_memory_access >= v.memory)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_A;
			server_instance_list[server_instance_id].A_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].A_memory_access -= v.memory;
			r->sloved = SOLOVED;
			r->server_id = server_instance_id;
			r->node = NODE_A;
			return 1;
		}
		else if (s.B_cpu_access >= v.cpu_core && s.B_memory_access >= v.memory)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NODE_B;
			server_instance_list[server_instance_id].B_cpu_access -= v.cpu_core;
			server_instance_list[server_instance_id].B_memory_access -= v.memory;
			r->sloved = SOLOVED;
			r->server_id = server_instance_id;
			r->node = NODE_B;
			return 1;
		}
	}
	else
	{
		int node_cpu = v.cpu_core / 2;
		int node_memory = v.memory / 2;
		if (s.A_cpu_access >= node_cpu && s.A_memory_access >= node_memory && s.B_cpu_access >= node_cpu && s.B_memory_access >= node_cpu)
		{
			vm_instance_map[vm_id].type_id = vm_type_id;
			vm_instance_map[vm_id].server_id = server_instance_id;
			vm_instance_map[vm_id].node = NONE;
			server_instance_list[server_instance_id].A_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].A_memory_access -= node_memory;
			server_instance_list[server_instance_id].B_cpu_access -= node_cpu;
			server_instance_list[server_instance_id].B_memory_access -= node_memory;
			r->sloved = SOLOVED;
			r->server_id = server_instance_id;
			r->node = NONE;
			return 1;
		}
	}
	r->sloved = UNSLOVED;
	return 0;
}

void daily_requests()
{
	int daily_request_num;
	request *daily_request_list;
	int *sort_request_id;
	double *sort_vm_cpu_memory;
	int *buying_list;
	map<int, int> buying_num;
	int buying_count = 0;
	cin >> daily_request_num;
	daily_request_list = new request[daily_request_num];
	sort_request_id = new int[daily_request_num];
	sort_vm_cpu_memory = new double[daily_request_num];
	buying_list = new int[server_type_num];
	char tmp_char;
	int tmp_vm_id;
	int tmp_type_id;
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
			for (int j = 0; j < server_instance_num; j++)
			{
				if (check_and_add(j, tmp_type_id, tmp_vm_id, &daily_request_list[i]) == 1)
				{
					sort_vm_cpu_memory[i] = 0;
					break;
				}
			}
		}
		else
		{
			delete_vm(tmp_vm_id, &daily_request_list[i]);
		}
	}
	mergeSort(sort_vm_cpu_memory, sort_request_id, daily_request_num);
	if (sort_vm_cpu_memory[daily_request_num - 1] != 0)
	{
		int start_index = 0;
		double start = sort_vm_cpu_memory[start_index];
		double sum = 0;
		int max_cpu = vm_type_list[daily_request_list[sort_request_id[start_index]].vm_type].cpu_core;
		int max_memory = vm_type_list[daily_request_list[sort_request_id[start_index]].vm_type].memory;
		for (int i = 0; i <= daily_request_num; i++)
		{
			if (i != daily_request_num && sort_vm_cpu_memory[i] == 0)
			{
				start_index++;
				start = sort_vm_cpu_memory[start_index];
				sum = 0;
			}
			else
			{
				if (i == daily_request_num || abs(sort_vm_cpu_memory[i] - start) > FIT)
				{
					double average = sum / (i - start_index);
					int server_type_id = find_fit_server(average, max_cpu, max_memory);

					if (server_type_id != -1)
					{
						buying_num[server_type_id] = 0;
						for (int j = start_index; j < i; j++)
						{
							daily_request_list[sort_request_id[j]].server_type = server_type_id;
						}
					}
					else
					{
						for (int j = start_index; j < i; j++)
						{
							server_type_id = find_fit_server(vm_type_list[daily_request_list[sort_request_id[j]].vm_type].cpu_memory, vm_type_list[daily_request_list[sort_request_id[j]].vm_type].cpu_core, vm_type_list[daily_request_list[sort_request_id[j]].vm_type].memory);
							daily_request_list[sort_request_id[j]].server_type = server_type_id;
							buying_num[server_type_id] = 0;
						}
					}
					if (i < daily_request_num) {
						start_index = i;
						start = sort_vm_cpu_memory[start_index];
						sum = start;
						max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
						max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
					}
				}
				else
				{
					sum += sort_vm_cpu_memory[i];
					if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core > max_cpu)
					{
						max_cpu = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].cpu_core;
					}
					if (vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory > max_memory)
					{
						max_memory = vm_type_list[daily_request_list[sort_request_id[i]].vm_type].memory;
					}
				}
			}
		}
	}

	map<int, int>::iterator iter;
	for (iter = buying_num.begin(); iter != buying_num.end(); iter++)
	{
		int server_type = iter->first;
		buying_list[buying_count] = server_type;
		buying_count++;
		buy_server(server_type);
		iter->second++;
		int cur_server_instance = server_instance_num - 1;
		for (int i = 0; i < daily_request_num; i++)
		{
			if (daily_request_list[i].sloved == SOLOVED)
			{
				continue;
			}
			else if (daily_request_list[i].request == ADD_VM && daily_request_list[i].server_type == server_type)
			{
				int flag = add_vm(cur_server_instance, daily_request_list[i].vm_type, daily_request_list[i].vm_id, &daily_request_list[i]);
				if (flag == 0) {
					buy_server(server_type);
					iter->second++;
					cur_server_instance = server_instance_num - 1;
					add_vm(cur_server_instance, daily_request_list[i].vm_type, daily_request_list[i].vm_id, &daily_request_list[i]);
				}
			}
		}
	}
	cout << "(purchase," << buying_count << ")" << endl;
	for(int i = 0; i < buying_count; i++) {
		cout << "(" << server_type_list[buying_list[i]].name << ", " << buying_num[buying_list[i]] << ")" << endl;
	}
	cout << "(migration, 0)" << endl;
	for (int i = 0; i < daily_request_num; i++) {
		if(daily_request_list[i].request == ADD_VM) {
			cout << "(" << daily_request_list[i].server_id;
			if (daily_request_list[i].node == NODE_A) {
				cout << ", A)" << endl;
			}
			else if (daily_request_list[i].node == NODE_B) {
				cout << ", B)" << endl;
			}
			else {
				cout << ")" << endl;
			}
		}
	}
	delete[] daily_request_list;
	delete[] sort_request_id;
	delete[] sort_vm_cpu_memory;
	delete[] buying_list;
	buying_num.clear();
}

void all_requests()
{
	for (int i = 0; i < day_num; i++) {
		daily_requests();
	}
}

void delete_cluster()
{
	delete[] server_type_list;
	delete[] server_instance_list;
	delete[] vm_type_list;
	delete[] sort_cpu_memory;
	delete[] sort_server_type_id;
}

#ifdef DEBUG
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
#endif
