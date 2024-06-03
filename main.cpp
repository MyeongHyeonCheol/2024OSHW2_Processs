#include "dynamic_queue.h"

// 2-1 부분구현: enqueue(), dequeue(), promote(), merge() 구현
int main() {
	DynamicQueue dq;
	Process* p1 = new Process(1, ProcessType::BG);
	Process* p2 = new Process(2, ProcessType::FG);
	Process* p3 = new Process(3, ProcessType::FG);
	Process* p4 = new Process(4, ProcessType::BG);
	Process* p5 = new Process(5, ProcessType::FG);
	Process* p6 = new Process(6, ProcessType::FG);
	Process* p7 = new Process(7, ProcessType::FG);
	Process* p8 = new Process(8, ProcessType::FG);
	Process* p9 = new Process(9, ProcessType::FG);
	Process* p10 = new Process(10, ProcessType::FG);

	dq.enqueue(p1);
	dq.enqueue(p2);
	dq.enqueue(p3);
	dq.enqueue(p4);
	dq.enqueue(p5);
	dq.enqueue(p6);
	dq.enqueue(p7);
	dq.enqueue(p8);
	dq.enqueue(p9);
	dq.enqueue(p10);

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

	process = dq.dequeue();
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

	process = dq.dequeue();
	process->start();
	process->join();

	process = dq.dequeue();
	process->start();
	process->join();
	return 0;
}
