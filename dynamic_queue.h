#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

// ���μ��� �з�: BG(background), FG(foreground)
enum class ProcessType {
	BG,
	FG
};

// ���μ��� Ŭ����
class Process {
private:
	thread t;
	int pid;
	ProcessType type;
	mutex mtx;

	// �ӽ� thread ���� �Լ�
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

	// ���μ��� Ÿ���� ��ȯ�ϱ� ���� �Լ�
	ProcessType get_type() const {
		return type;
	}

};

// ���μ��� ����Ʈ ��� Ŭ����
class ListNode {
public:
	Process* process;
	ListNode* next;

	ListNode(Process* process) : process(process), next(nullptr) { }
};

// ���� ��� Ŭ����
class StackNode {
public:
	ListNode* process_list;
	StackNode* next;

	StackNode() : process_list(new ListNode(nullptr)), next(nullptr) { }

	// ���� ��� �Ҹ���: ��带 ������ �� ������ִ� ���μ��� ����Ʈ�� ������
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
	StackNode* bottom; // �ٴ� ���
	StackNode* top; // �� �� ���
	int threshold; // �Ӱ�ġ
	int process_count; // ���μ��� ����: �Ӱ�ġ�� ����ϱ� ���� ���
	StackNode* P;

	// ���� ��� ������ �˾Ƴ��� ���� get �Լ�
	int get_stack_node_count() {
		int count = 0;
		StackNode* current = bottom; // ���� ������ ���� �Ʒ��� �ִ� ������ ����Ŵ
		while (current) { // current�� �����ϸ� while �ݺ��� ����
			count++;  // ������ 1 ������Ŵ
			current = current->next; // current�� ���� ��带 ����Ŵ
		}
		return count;
	}

	// threshold ������Ʈ �Լ�
	void update_threshold() {
		if (process_count == 0) {
			threshold = 0;
		}
		else {
			threshold = process_count / (get_stack_node_count() + 1);
		}
	}

public:
	// DQ ������: bottom���� ���� ��带 ����, top���� bottom�� ������ -> �ʱ� ���� bottom = top
	DynamicQueue() : bottom(new StackNode()), top(bottom), threshold(0), process_count(0), P(bottom) { }

	// euqueue: BG�� ��� bottom ��忡, FG�� ��� top ��� ���� ����
	void enqueue(Process* process) {
		if (process->get_type() == ProcessType::BG) { // ���μ����� BG�� ���
			ListNode* new_node = new ListNode(process);
			if (!bottom->process_list->process) { // bottom�� ���μ����� ���� ��� �ٷ� ����
				bottom->process_list = new_node;
			}
			else { // ���μ����� �ִ� ��� �ش� ���μ����� ���� ã�� ����
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
		else if (process->get_type() == ProcessType::FG) { // ���μ����� FG�� ���
			ListNode* new_node = new ListNode(process);
			if (!top->process_list->process) { // top�� ���μ����� �������� �ʴ� ��� �ٷ� ����
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
	}

	Process* dequeue() {
		if (!top->process_list->process) { // ����� ���μ����� ���� ���
			return nullptr;
		}

		ListNode* removed_node = top->process_list;
		top->process_list = removed_node->next;
		Process* removed_process = removed_node->process;
		delete removed_node;

		// ���μ��� ���� ���� top�� ���μ����� �������� ������ top�� ���� ���� �ű�� �ش� top�� ����
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
		return removed_process;
	}
};
