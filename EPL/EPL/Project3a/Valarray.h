// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */
#include <vector>

#ifndef _Valarray_h
#define _Valarray_h

using std::vector; // during development and testing
// using epl::vector; // after submission

namespace epl {
	template<typename T> struct choose_ref {
		using type = T;
	};

	template<typename T> struct choose_ref<valarray<T>> {
		using type = valarray<T> const&;
	};

	template<typename T> using Choose_type = choose_ref<T>;

	template<typename T1, typename T2, typename Op>
	class proxy {
		using LeftType = Choose_type<T1>;
		using RightType = Choose_type<T2>;
		LeftType p1;
		RightType p2;
	public:
		proxy(T1 const& left, T2 const& right) :
			p1(left), p2(right) {}
		
	};

	template<typename T>
	class valarray : public vector<T> {
		using Same = valarray<T>;
	public:
		using vector<T>::vector;
		Same& operator=(Same const& rhs) {
			unint64_t size = std::min(this->size(), rhs.size());
			Same& lhs{ *this };
			for (int i = 0; i < size; ++i) {
				lhs[i] = rhs[i];
			}
			return *this;
		}

		template<>
		Same& operator=(proxy)
	};

	template<typename T>
	class wrapper : public valarray<T> {

	};

	// binary operator +
	template<typename LHSType, typename RHSType>
	wrapper<proxy<LHSType, RHSType>>
		operator+(wrapper<LHSType> const& lhs, wrapper<RHSType> const& rhs) {
		LHSType const& left{ lhs };
		RHSType const& right{ rhs };
		proxy<LHSType, RHSType> result{};
	}
}

#endif /* _Valarray_h */

