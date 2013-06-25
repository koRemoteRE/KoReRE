#ifndef SERVER_MATRIX_QUEUE
#define SERVER_MATRIX_QUEUE

#include <queue>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>

struct MatrixQueuePacket{
	int id;
	std::vector<uchar> buffer;
};

class MatrixQueue
{
private:
	MatrixQueue(void);
	std::queue<MatrixQueuePacket> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static MatrixQueue* getInstance(){
		static MatrixQueue queue;
		return &queue;
	};
	~MatrixQueue();
	void push(MatrixQueuePacket const &data);
	bool empty() const;
	bool tryPop(MatrixQueuePacket &poppedValue);
	void waitAndPop(MatrixQueuePacket &poppedValue);
	int getLenght();
};
#endif

