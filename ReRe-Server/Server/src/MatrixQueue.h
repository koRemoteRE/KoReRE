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

	void clear();
public:
	static MatrixQueue* getInstance(){
		static MatrixQueue queue;
		return &queue;
	};
	~MatrixQueue();
	//Pushes an SerializableMatrix to queue
	void push(SerializableMatrix const &data);
	//true, if queue is empty
	bool empty() const;
	/*tries to pop the front of queue. 
	Returns true, if its not possible*/
	bool tryPop(SerializableMatrix &poppedValue);
	/*Tries to pop the first element of queue,
	 Stops the thread if it's not possible.*/
	void waitAndPop(SerializableMatrix &poppedValue);
	/*Returns length of queue*/
	int getLenght();
};
#endif

