#ifndef SERVER_CONCURRENT_QUEUE
#define SERVER_CONCURRENT_QUEUE

#include <queue>
#include <boost/thread.hpp>

extern "C" { 
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

struct QueuePacket{
	int id;
	AVPacket framePacket;
};

class ConcurrentQueue
{
private:
	ConcurrentQueue(void);
	std::queue<QueuePacket> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static ConcurrentQueue* getInstance(){
		static ConcurrentQueue queue;
		return &queue;
	};
	~ConcurrentQueue();
	void push(QueuePacket const &data);
	bool empty() const;
	bool tryPop(QueuePacket &poppedValue);
	void waitAndPop(QueuePacket &poppedValue);
	int getLenght();
};
#endif

