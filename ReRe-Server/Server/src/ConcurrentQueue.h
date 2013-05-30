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


class ConcurrentQueue
{
private:
	ConcurrentQueue(void);
	std::queue<AVPacket> theQueue;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;
public:
	static ConcurrentQueue* getInstance(){
		static ConcurrentQueue queue;
		return &queue;
	};
	~ConcurrentQueue();
	void push(AVPacket const &data);
	bool empty() const;
	bool tryPop(AVPacket &poppedValue);
	void waitAndPop(AVPacket &poppedValue);
	int getLenght();
};
#endif

