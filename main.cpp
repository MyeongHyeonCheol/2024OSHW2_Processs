#include "dynamic_queue.h"

int main() {
	DynamicQueue dq;
	Process* p1 = new Process(1, ProcessType::BG);
	Process* p2 = new Process(2, ProcessType::FG);
	Process* p3 = new Process(3, ProcessType::FG);
	Process* p4 = new Process(4, ProcessType::BG);

	dq.enqueue(p1);
	dq.enqueue(p2);
	dq.enqueue(p3);
	dq.enqueue(p4);

	Process* process = dq.dequeue();
	process->start();
	process->join();

	process = dq.dequeue();
	process->start();
	process->join();

	process = dq.dequeue();
	process->start();
	process->join();

	process = dq.dequeue();
	process->start();
	process->join();
	return 0;
}