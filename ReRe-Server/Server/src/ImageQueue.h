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
	void push(SerializableImage const &data);
	bool empty() const;
	bool tryPop(SerializableImage &poppedValue);
	void waitAndPop(SerializableImage &poppedValue);
	int getLenght();
};
#endif

