#include "ConcurrentQueue.h"

ConcurrentQueue::ConcurrentQueue(){

}

ConcurrentQueue::~ConcurrentQueue(){

}

void ConcurrentQueue::push(QueuePacket const &data){
	boost::mutex::scoped_lock lock(theMutex);
	theQueue.push(data);
	lock.unlock();
	theConditionVariable.notify_one();
}

bool ConcurrentQueue::empty() const{
	boost::mutex::scoped_lock lock(theMutex);
	return theQueue.empty();
}

bool ConcurrentQueue::tryPop(QueuePacket &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	if(theQueue.empty()){
		return false;
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
	return true;
}

void ConcurrentQueue::waitAndPop(QueuePacket &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	while(theQueue.empty()){
		theConditionVariable.wait(lock);
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
}

int ConcurrentQueue::getLenght(){
	return theQueue.size();
}
