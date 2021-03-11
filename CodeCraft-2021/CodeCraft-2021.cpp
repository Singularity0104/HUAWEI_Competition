#include <iostream>
#include <string>
#include <map>
using namespace std;

#define DEBUG
#define MAX_SEVER_NUM 5000
#define ADD_SEVER_NUM 1000

int day;

struct sever_info {
	int cpu_core;
	int memory;
	int device_cost;
	int energy_cost;
};
struct sever_instance {
	int type_id;
	int is_on;
	int A_cpu_access;
	int B_cpu_access;
	int A_memory_access;
	int B_memory_access;
};
struct vm_info {
	int cpu_core;
	int memory;
	int is_double_node;
};
int sever_type_num;
int sever_instance_num;
int vm_type_num;
int vm_instance_num;
sever_info* sever_type_list;
sever_instance* sever_instance_list;
vm_info* vm_type_list;
map<string, int> sever_type_index;
map<string, int> vm_type_index;
map<int, int> vm_instence_id;
double* sort_cpu_memory;
int* sort_sever_type_id;


void init_cluster();
void init_sever_type_list();
void init_vm_type_list();
void sort_sever();
void delete_cluster();
#ifdef DEBUG
void print_type_list();
void print_sort_list();
#endif

int main()
{
	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);
	cout << "hello" << endl;
	init_cluster();
	init_sever_type_list();
	init_vm_type_list();
	sort_sever();
#ifdef DEBUG
	print_type_list();
	print_sort_list();
#endif
	delete_cluster();
	return 0;
}

void init_cluster() {
	sever_type_num = 0;
	sever_instance_num = 0;
	vm_type_num = 0;
	vm_instance_num = 0;
	sever_instance_list = new sever_instance[MAX_SEVER_NUM];
}

void init_sever_type_list() {
	cin >> sever_type_num;
	sever_type_list = new sever_info[sever_type_num];
	sort_cpu_memory = new double[sever_type_num];
	sort_sever_type_id = new int[sever_type_num];
	char tmp_char = 0;
	int tmp_cpu = 0;
	int tmp_memory = 0;
	int tmp_device_cost = 0;
	int tmp_energy_cost = 0;
	string tmp_name;
	for(int i = 0; i < sever_type_num; i++) {
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
		sever_type_index[tmp_name] = i;
		sever_type_list[i].cpu_core = tmp_cpu;
		sever_type_list[i].memory = tmp_memory;
		sever_type_list[i].device_cost = tmp_device_cost;
		sever_type_list[i].energy_cost = tmp_energy_cost;
		sort_sever_type_id[i] = i;
		sort_cpu_memory[i] = ((double)tmp_cpu) / ((double)tmp_memory);
	}

}

void init_vm_type_list() {
	cin >> vm_type_num;
	vm_type_list = new vm_info[vm_type_num];
	char tmp_char = 0;
	int tmp_cpu = 0;
	int tmp_memory = 0;
	int tmp_double_node;
	string tmp_name;
	for(int i = 0; i < vm_type_num; i++) {
		cin >> tmp_char;
		cin >> tmp_name;
		cin >> tmp_cpu;
		cin >> tmp_char;
		cin >> tmp_memory;
		cin >> tmp_char;
		cin >> tmp_double_node;
		cin >> tmp_char;
		tmp_name.pop_back();
		vm_type_index[tmp_name] = i;
		vm_type_list[i].cpu_core = tmp_cpu;
		vm_type_list[i].memory = tmp_memory;
		vm_type_list[i].is_double_node = tmp_double_node;
	}
}

void mergeSort(double *arr, int *arr_index, int len){
    if(len <= 1){
        return;
    }
    int leftlen = len / 2;
    int rightlen = len - leftlen;
    double *left = arr;
    int *left_index = arr_index;
    double *right = arr + leftlen;
    int *right_index = arr_index + leftlen;
    mergeSort(left,left_index, leftlen);
    mergeSort(right, right_index, rightlen);
    double *tmp = new double[len];
    int *tmp_index = new int[len];
    int i = 0;
    int j = leftlen;
    int k = 0;
    while(i < leftlen && j < len){
        if(arr[i] <= arr[j]){
            tmp[k] = arr[i];
            tmp_index[k] = arr_index[i];
            i++;
            k++;
        }
        else{
            tmp[k] = arr[j];
            tmp_index[k] = arr_index[j];
            j++;
            k++;
        }
    }
    while(i < leftlen){
        tmp[k] = arr[i];
        tmp_index[k] = arr_index[i];
        i++;
        k++; 
    }
    while(j < len){
        tmp[k] = arr[j];
        tmp_index[k] = arr_index[j];
        j++;
        k++; 
    }
    for(k = 0; k < len; k++){
        arr[k] = tmp[k];
        arr_index[k] = tmp_index[k];
    }
    delete []tmp;
    delete []tmp_index;
}

void sort_sever() {
	mergeSort(sort_cpu_memory, sort_sever_type_id, sever_type_num);
}

void delete_cluster()
{
	delete []sever_type_list;
	delete []sever_instance_list;
	delete []vm_type_list;
	delete []sort_cpu_memory;
	delete []sort_sever_type_id;
}

#ifdef DEBUG
void print_type_list() {
	cout << "debug-print-------------------------------------------" << endl;
	for(int i = 0; i < sever_type_num; i++) {
		cout << sever_type_list[i].cpu_core << " ";
		cout << sever_type_list[i].memory << " ";
		cout << sever_type_list[i].device_cost << " ";
		cout << sever_type_list[i].energy_cost << endl;
	}
	cout << "------------------------------------------------------" << endl;
	for(int i = 0; i < vm_type_num; i++) {
		cout << vm_type_list[i].cpu_core << " ";
		cout << vm_type_list[i].memory << " ";
		cout << vm_type_list[i].is_double_node << endl;
	}
	cout << "------------------------------------------------------" << endl;
}
void print_sort_list() {
	cout << "sort-print--------------------------------------------" << endl;
	for(int i = 0; i < sever_type_num; i++) {
		cout << sort_sever_type_id[i] << " ";
		cout << sort_cpu_memory[i] << " " << endl;
	}
	cout << "------------------------------------------------------" << endl;
}
#endif
