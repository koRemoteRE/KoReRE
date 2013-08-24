#ifndef SERVER_IMAGE_QUEUE
#define SERVER_IMAGE_QUEUE

#include <queue>
#include <boost/thread.hpp>
#include "SerializableImage.hpp"

class ImageQueue
{
private:
	ImageQueue(void);
	std::queue<SerializableImage> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static ImageQueue* getInstance(){
		static ImageQueue queue;
		return &queue;
	};
	~ImageQueue();
	//Pushes an SerializableImage to queue
	void push(SerializableImage const &data);
	//true, if queue is empty
	bool empty() const;
	/*tries to pop the front of queue. 
	Returns true, if its not possible*/
	bool tryPop(SerializableImage &poppedValue);
	/*Tries to pop the first element of queue,
	 Stops the thread if it's not possible.*/
	void waitAndPop(SerializableImage &poppedValue);
	/*Returns length of queue*/
	int getLenght();
};
#endif

