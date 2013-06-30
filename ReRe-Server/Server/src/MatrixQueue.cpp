#include "MatrixQueue.h"

MatrixQueue::MatrixQueue(){}

MatrixQueue::~MatrixQueue(){}

void MatrixQueue::push(SerializableMatrix const &data){
	boost::mutex::scoped_lock lock(theMutex);
	theQueue.push(data);
	lock.unlock();
	theConditionVariable.notify_one();
}

bool MatrixQueue::empty() const{
	boost::mutex::scoped_lock lock(theMutex);
	return theQueue.empty();
}

bool MatrixQueue::tryPop(SerializableMatrix &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	if(theQueue.empty()){
		return false;
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
	return true;
}

void MatrixQueue::waitAndPop(SerializableMatrix &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	while(theQueue.empty()){
		theConditionVariable.wait(lock);
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
}

int MatrixQueue::getLenght(){
	return theQueue.size();
}
