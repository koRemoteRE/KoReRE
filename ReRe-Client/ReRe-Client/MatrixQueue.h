#ifndef SERVER_MATRIX_QUEUE
#define SERVER_MATRIX_QUEUE

#include <queue>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
#include <glm/glm.hpp>
#include "SerializableMatrix.hpp"

class MatrixQueue
{
private:
	MatrixQueue(void);
	std::queue<SerializableMatrix> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static MatrixQueue* getInstance(){
		static MatrixQueue queue;
		return &queue;
	};
	~MatrixQueue();
	void push(SerializableMatrix const &data);
	bool empty() const;
	bool tryPop(SerializableMatrix &poppedValue);
	void waitAndPop(SerializableMatrix &poppedValue);
	int getLenght();
};
#endif
