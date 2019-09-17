#pragma once

template<class T>
class QueueArray {
private:
	std::size_t mFirst, mLast, mLength, mCapacity;
	T *mData;
	static void MakeHardwareReset(String errorMess);
public:
	QueueArray(int capacity = 16) {
		mCapacity = capacity;
		mData = new T[mCapacity];
		mLength = 0;
		mFirst = 0;
		mLast = mCapacity - 1;
	}
	~QueueArray() {
		delete[] mData;
	}
	T& operator[](std::size_t pos);
	std::size_t Length() const;
	void PushBack(const T &item);
	T First();
	T Last();
	T PopFirst();
	T PopBack();
	void Clear();
};

template<class T>
void QueueArray<T>::MakeHardwareReset(const String errorMess)
{
	Serial.println(errorMess);
	while (true);
}

template<class T>
T& QueueArray<T>::operator[](std::size_t pos)
{
	if (pos >= mLength) MakeHardwareReset(F("Out of range in QueueArray."));
	pos += mFirst;
	if (pos >= mCapacity) pos -= mCapacity;
	return mData[pos];
}

template<class T>
std::size_t QueueArray<T>::Length() const
{
  return mLength;
}

template<class T>
void QueueArray<T>::PushBack(const T &item)
{
  if(mLength < mCapacity) 
  {
    ++mLength;
    if (mLast < mCapacity-1) ++mLast;
    else mLast = 0;
    mData[mLast]=item;
  }
  else
  {
	//Serial.println(F("Overflow in QueueArray. Rewriting first element."));
	PopFirst();
	PushBack(item);
  }
}

template<class T>
T QueueArray<T>::First() 
{
  if(mLength <= 0) MakeHardwareReset(F("Out of range in QueueArray."));
  return mData[mFirst];
}

template<class T>
T QueueArray<T>::Last() 
{
  if(mLength <= 0) MakeHardwareReset(F("Can't get last element in QueueArray with 0 elements."));
  return mData[mLast];
}

template<class T>
T QueueArray<T>::PopFirst() 
{
	if (mLength <= 0) MakeHardwareReset(F("Can't pop first element in QueueArray with 0 elements."));
	T result = mData[mFirst];
	--mLength;
	if (mFirst < mCapacity - 1) ++mFirst;
	else mFirst = 0;
	return result;
}

template<class T>
T QueueArray<T>::PopBack()
{
	if (mLength <= 0) MakeHardwareReset(F("Can't pop last element in QueueArray with 0 elements."));
	T result = mData[mLast];
	--mLength;
	if (mLast > 0) --mLast;
	else mLast = mCapacity - 1;
	return result;
}

template<class T>
void QueueArray<T>::Clear() 
{
  mFirst = 0;
  mLast = mCapacity - 1;
  mLength = 0;
  memset_s(mData, 0, sizeof(T) * mCapacity);
}
