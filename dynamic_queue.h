#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

// 프로세스 분류: BG(background), FG(foreground)
enum class ProcessType {
	BG,
	FG
};

// 프로세스 클래스
class Process {
private:
	thread t;
	int pid;
	ProcessType type;
	mutex mtx;

	// 임시 thread 실행 함수
	void worker() {
		for (int i = 0; i < 10; i++) {
			Sleep(1000);
			mtx.lock();
			cout << "PID: " << pid << ", " << i << endl;
			mtx.unlock();
		}
	}

public:
	Process(int pid, ProcessType type) : pid(pid), type(type) { }

	void start() {
		t = thread(&Process::worker, this);
	}

	void join() {
		t.join();
	}

	int get_pid() const {
		return pid;
	}

	// 프로세스 타입을 반환하기 위한 함수
	ProcessType get_type() const {
		return type;
	}

};

// 프로세스 리스트 노드 클래스
class ListNode {
public:
	Process* process;
	ListNode* next;

	ListNode(Process* process) : process(process), next(nullptr) { }
};

// 스택 노드 클래스
class StackNode {
public:
	ListNode* process_list;
	StackNode* next;

	StackNode() : process_list(new ListNode(nullptr)), next(nullptr) { }

	// 스택 노드 소멸자: 노드를 삭제할 때 저장되있던 프로세스 리스트를 삭제함
	~StackNode() {
		while (process_list) {
			ListNode* temp = process_list;
			process_list = process_list->next;
			delete temp;
		}
	}
};

class DynamicQueue {
private:
	StackNode* bottom; // 바닥 노드
	StackNode* top; // 맨 위 노드
	int threshold; // 임계치
	int process_count; // 프로세스 개수: 임계치를 계산하기 위해 사용
	StackNode* P;

	// 스택 노드 개수를 알아내기 위한 get 함수
	int get_stack_node_count() {
		int count = 0;
		StackNode* current = bottom; // 현재 스택은 가장 아래에 있는 스택을 가리킴
		while (current) { // current가 존재하면 while 반복문 실행
			count++;  // 개수를 1 증가시킴
			current = current->next; // current는 다음 노드를 가리킴
		}
		return count;
	}

	// threshold 업데이트 함수
	void update_threshold() {
		if (process_count == 0) {
			threshold = 0;
		}
		else {
			threshold = process_count / (get_stack_node_count() + 1);
		}
	}
	
	void promote() {
		if (P == nullptr || P->process_list == nullptr || P->process_list->process == nullptr) {
			return;
		}

		ListNode* P_head_node = P->process_list; // P의 헤드노드를 저장
		if (P->process_list->next) {
			P->process_list = P->process_list->next; // P의 프로세스 리스트들을 앞당김
		}
		else {
			P->process_list = nullptr;
		}
		P_head_node->next = nullptr;

		if (P == top) { // P가 top이면 새로운 스택 노드를 생성하고 top을 연결 및 갱신함
			StackNode* new_stack_node = new StackNode();
			top->next = new_stack_node;
			top = new_stack_node;
			top->process_list = P_head_node;
			P = bottom;
		}
		else { // P가 top이 아닐 때
			ListNode* node = P->next->process_list;
			if (!node) {
				P->next->process_list = P_head_node;
			}
			else {
				while (node->next) {
					node = node->next;
				}
				node->next = P_head_node;
			}
		}

		// P->process_list가 null이라면 P를 삭제하고 이전 노드와 다음 노드를 연결함
		if (P->process_list == nullptr) {
			if (P == bottom) {
				bottom = P->next;
			}
			else {
				StackNode* prev = bottom;
				while (prev->next != P) {
					prev = prev->next;
				}
				prev->next = P->next;
			}

			StackNode* temp = P;
			P = P->next ? P->next : bottom;
			delete temp;
		}
	}

public:
	// DQ 생성자: bottom에는 스택 노드를 저장, top에는 bottom을 저장함 -> 초기 상태 bottom = top
	DynamicQueue() : bottom(new StackNode()), top(bottom), threshold(0), process_count(0), P(bottom) { }

	// euqueue: BG인 경우 bottom 노드에, FG인 경우 top 노드 끝에 삽입
	void enqueue(Process* process) {
		if (process->get_type() == ProcessType::BG) { // 프로세스가 BG인 경우
			ListNode* new_node = new ListNode(process);
			if (!bottom->process_list->process) { // bottom에 프로세스가 없는 경우 바로 삽입
				bottom->process_list = new_node;
			}
			else { // 프로세스가 있는 경우 해당 프로세스의 끝을 찾아 삽입
				ListNode* temp = bottom->process_list;
				while (temp->next) {
					temp = temp->next;
				}
				temp->next = new_node;
				Process* process = temp->next->process;
				if (!temp->process) {
				}
			}
		}
		else if (process->get_type() == ProcessType::FG) { // 프로세스가 FG인 경우
			ListNode* new_node = new ListNode(process);
			if (!top->process_list->process) { // top에 프로세스가 존재하지 않는 경우 바로 삽입
				top->process_list = new_node;
				cout << new_node << endl;
			}
			else {
				ListNode* temp = top->process_list;
				while (temp->next) {
					temp = temp->next;
				}
				temp->next = new_node;
			}
		}
		process_count++;
		update_threshold();
		promote();
	}

	Process* dequeue() {
		if (!top->process_list->process) { // 저장된 프로세스가 없는 경우
			return nullptr;
		}

		ListNode* removed_node = top->process_list;
		top->process_list = removed_node->next;
		Process* removed_process = removed_node->process;
		delete removed_node;

		// 프로세스 삭제 이후 top에 프로세스가 존재하지 않으면 top을 이전 노드로 옮기고 해당 top을 삭제
		if (get_stack_node_count() > 1 && !top->process_list) {
			StackNode* prev = bottom;
			while (prev->next != top) {
				prev = prev->next;
			}
			prev->next = nullptr;
			delete top;
			top = prev;
		}
		process_count--;
		update_threshold();
		promote();
		return removed_process;
	}
};
