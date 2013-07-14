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
	if(theQueue.size() >= 10){
		clear();
	}
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
	if(theQueue.size() >= 10){
		clear();
	}
	lock.unlock();
}

int MatrixQueue::getLenght(){
	return theQueue.size();
}

void MatrixQueue::clear(){
	for (unsigned int i = 0; i < theQueue.size(); i++){
		theQueue.pop();
	} 
}
