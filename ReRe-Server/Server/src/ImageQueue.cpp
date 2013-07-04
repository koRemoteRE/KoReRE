#include "ImageQueue.h"
#include "logger.h"

ImageQueue::ImageQueue(){}

ImageQueue::~ImageQueue(){}

void ImageQueue::push(SerializableImage const &data){
	boost::mutex::scoped_lock lock(theMutex);
	theQueue.push(data);
	lock.unlock();
	theConditionVariable.notify_one();
}

bool ImageQueue::empty() const{
	boost::mutex::scoped_lock lock(theMutex);
	return theQueue.empty();
}

bool ImageQueue::tryPop(SerializableImage &poppedValue){
	boost::mutex::scoped_lock lock(theMutex);
	if(theQueue.empty()){
		return false;
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
	return true;
}

void ImageQueue::waitAndPop(SerializableImage &poppedValue){
	logger::printTime("Wait and pop()");
	boost::mutex::scoped_lock lock(theMutex);
	while(theQueue.empty()){
		theConditionVariable.wait(lock);
	}

	poppedValue = theQueue.front();
	theQueue.pop();
	lock.unlock();
	logger::printTime("Popped image");
}

int ImageQueue::getLenght(){
	return theQueue.size();
}
