#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl{

class invalid_iterator {
	public:
	enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
	SeverityLevel level;	

	invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
	virtual const char* what() const {
    switch(level){
      case WARNING:   return "Warning"; // not used in Spring 2015
      case MILD:      return "Mild";
      case MODERATE:  return "Moderate";
      case SEVERE:    return "Severe";
      default:        return "ERROR"; // should not be used
    }
	}
};

template <typename T>
class vector{
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
	// a random access iterator should support *, ++, ==, --, +, -
	// all other operator can be implememted by those six basic operators
	template<typename item>
	class iterator_helper : public std::iterator<std::random_access_iterator_tag, item> {
	private:
		item* ptr;
		using Same = iterator_helper<item>;
	public:
		item& operator*(void) { return ptr[-1]; }
		Same& operator++(void) {
			++ptr;
			return *this;
		}
		Same operator--(void) {
			--ptr;
			return *this;
		}
		bool operator==(Same const& rhs) {
			return this->ptr == rhs.ptr;
		}
		bool operator!=(Same const& rhs) {
			return !(*this == rhs);
		}

		Same operator+(int32_t k) {
			Same result{};
			result.ptr = this->ptr + k;
			return result;
		}
		Same operator-(int32_t q) {
			Same result{};
			result.ptr = this->ptr - q;
			return result;
		}
		friend vector;
	};

public:
	using iterator = iterator_helper<T>;
	using const_iterator = iterator_helper<const T>;

	iterator begin(void) {
		iterator p;
		p.ptr = data;
		return p;
	}

	const_iterator begin(void) const {
		const_iterator p;
		p.ptr = data;
		return p;
	}
	iterator end(void) {
		iterator p;
		p.ptr = data + count;
		return p;
	}
	const_iterator end(void) const {
		const_iterator p;
		p.ptr = data + count;
		return p;
	}
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
			return data[front + k];
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
				new (&data[rear + 1]) T{ rhs };
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

#endif
