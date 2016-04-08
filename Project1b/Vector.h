// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

//#define MIN_CAPACITY (8);
const int MIN_CAPACITY = 8;

namespace epl{

template <typename T>
class vector {
private:
	T* data;
	int length;
	int front;
	int rear;
	int count;
private:
	void init(uint64_t n) {
		if (n == 0) {
			data = (T*) ::operator new(sizeof(T)*MIN_CAPACITY);
			length = MIN_CAPACITY;
			front = 0;
			rear = 0;
			count = 0;
		}
		else {
			front = 0;
			rear = n - 1;
			count = n;
			//data = new T[n];
			data = (T*) ::operator new(sizeof(T)*n);
			for (int i = 0; i < n; i++)
				new (&data[i]) T{};
			length = n;
		}
	}
	void copy(vector const& that) {
		this->length = that.length;
		this->count = that.count;
		this->front = that.front;
		this->rear = that.rear;
		this->data = (T*) ::operator new(sizeof(T)*this->length);
		if (that.count > 0) {
			for (int i = that.front; i <= that.rear; i++)
				new (&(this->data[i])) T{ that.data[i] };
		}
	}
	void move(vector && tmp) {
		this->length = tmp.length;
		this->count = tmp.count;
		this->front = tmp.front;
		this->rear = tmp.rear;
		this->data = tmp.data;
		tmp.data = nullptr;
	}
	T* resize(int i) {
		int orignal_length;
		T* res;
		orignal_length = this->length;
		this->length *= 2;
		T* tmp = (T*) ::operator new(this->length*sizeof(T));
		if (i == 0) {
			//resize for push_back
			for (int i = front; i <= rear; i++)
				new (&tmp[i]) T{ std::move(data[i]) };
		}
		else {
			// resize for push_front
			for (int i = front; i <= rear; i++)
				new (&(tmp[orignal_length + i])) T{ std::move(data[i]) };
			this->front = orignal_length;
			this->rear += orignal_length;
		}
		//::operator delete (data);
		res = this->data;
		this->data = tmp;
		tmp = nullptr;
		return res;
	}

	void destroy(void) {
		if (count == 0) {
			::operator delete(data);
		}
		else {
			if (data != NULL) {
				for (int i = front; i <= rear; i++)
					data[i].~T();
				::operator delete(data);
			}
		}
	}
public:
	vector(void) { init(0); }
	vector(uint64_t n) { init(n); }
	// copy constructor
	vector(vector const& that) { copy(that); }
	// move constructor
	vector(vector&& that) { move((vector &&)that); }
	uint64_t size(void) { return count; }
	// copy assignment operator
	vector& operator=(vector const& rhs) {
		destroy();
		copy(rhs);
		return *this;
	}
	// move assignment operator
	vector& operator=(vector&& rhs) { move((vector&&)rhs); }
	T& operator[](uint64_t k) {
		if (k >= count)
			throw std::out_of_range("subscript out of range");
		else {
			return data[front + k];
		}
	}
	const T& operator[](uint64_t k) const {
		if (k >= count)
			throw std::out_of_range("subscript out of range");
		else
			return data[front+k];
	}
	void push_back(const T& rhs) {
		if (count == 0) {
			new (&(this->data[rear])) T{ rhs };
			this->count++;
		}
		else {
			if (rear < length - 1) {
				rear++;
				new (&(this->data[rear])) T{ rhs };
				this->count++;
			}
			else {
				// resize the vector
				T* tt;
				tt = resize(0);
				//this->data = (T*)realloc(this->data, this->length*sizeof(T));
				new (&data[rear+1]) T{ rhs };
				::operator delete(tt);
				this->rear++;
				this->count++;
			}
		}
	}
	void push_back(T&& rhs) {
		if (count == 0) {
			new (&(this->data[rear])) T{ std::move(rhs) };
			this->count++;
		}
		else {
			if (rear < length - 1) {
				rear++;
				new (&(this->data[rear])) T{ std::move(rhs) };
				this->count++;
			}
			else {
				T* tt;
				tt = resize(0);
				//this->data = (T*)realloc(this->data, this->length*sizeof(T));
				rear++;
				new (&(this->data[rear])) T{ std::move(rhs) };
				::operator delete(tt);
				this->count++;
			}
		}
	}
	void push_front(const T& rhs) {
		if (this->count == 0) {
			new (&(this->data[front])) T{ rhs };
			this->count++;
		}
		else {
			if (front > 0) {
				front--;
				new (&(this->data[front])) T{ rhs };
				this->count++;
			}
			else {
				T* tt;
				tt = resize(1);
				this->front--;
				new (&(data[front])) T{ rhs };
				::operator delete(tt);
				this->count++;
			}
		}
	}
	void push_front(T&& rhs) {
		if (this->count == 0) {
			new (&(this->data[front])) T{ std::move(rhs) };
			this->count++;
		}
		else {
			if (front > 0) {
				front--;
				new (&(this->data[front])) T{ std::move(rhs) };
				this->count++;
			}
			else {
				T* tt;
				tt = resize(1);
				this->front--;
				new (&(this->data[front])) T{ std::move(rhs) };
				::operator delete(tt);
				this->count++;
			}
		}
	}
	void pop_back(void) {
		if (count == 0)
			throw std::out_of_range("No available object for pop_back!");
		else {
			count--;
			data[rear].~T();
			if (count > 0)
				rear--;
		}
	}
	void pop_front(void) {
		if (count == 0)
			throw std::out_of_range("No available object for pop_front!");
		else {
			count--;
			data[front].~T();
			if (count > 0)
				front++;
		}
	}

	~vector() { destroy(); }
};

} //namespace epl

#endif /* _Vector_h */
