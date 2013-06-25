#ifndef SERVER_IMAGE_QUEUE
#define SERVER_IMAGE_QUEUE

#include <queue>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>

struct ImageQueuePacket{
	int id;
	std::vector<uchar> buffer;
};

class ImageQueue
{
private:
	ImageQueue(void);
	std::queue<ImageQueuePacket> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static ImageQueue* getInstance(){
		static ImageQueue queue;
		return &queue;
	};
	~ImageQueue();
	void push(ImageQueuePacket const &data);
	bool empty() const;
	bool tryPop(ImageQueuePacket &poppedValue);
	void waitAndPop(ImageQueuePacket &poppedValue);
	int getLenght();
};
#endif

