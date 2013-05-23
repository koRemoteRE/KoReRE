#include "ConcurrentQueue.h"

ConcurrentQueue::ConcurrentQueue(){

}

ConcurrentQueue::~ConcurrentQueue(){

}

void ConcurrentQueue::push(AVPacket const &data){
	boost::mutex::scoped_lock lock(theMutex);
	theQueue.push(data);
	lock.unlock();
	theConditionVariable.notify_one();
}

bool ConcurrentQueue::empty() const{
	boost::mutex::scoped_lock lock(theMutex);
	return theQueue.empty();
}

bool ConcurrentQueue::tryPop(AVPacket &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	if(theQueue.empty()){
		return false;
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	return true;
}

void ConcurrentQueue::waitAndPop(AVPacket &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	while(theQueue.empty()){
		theConditionVariable.wait(lock);
	}

	poppedValue = theQueue.front();
	theQueue.pop();
}

int ConcurrentQueue::getLenght(){
	return theQueue.size();
}
