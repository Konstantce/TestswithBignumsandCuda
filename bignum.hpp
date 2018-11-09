#ifndef BIGNUM_HPP_
#define BIGNUM_HPP_

#if defined(_MSC_VER)
	#include <intrin.h>
	#if defined(_WIN64)
		#define ENV64BIT
	#elif defined(_WIN32)
		#define ENV32BIT
	#else
		#error Windows bitness is unspecified
	#endif
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
		#define ENV64BIT
	#elif defined(__i386__)
	#defined ENV32BIT
	#else
		#error Linux bitness in unspecified
	#endif
#else
	#error Operation system is unspecified
#endif

#include <stdint.h>

#if defined(ENV64BIT)
	constexpr unsigned LIMBSIZE = 64;
	using uint_default_t = uint64_t;
	#define BITNESS 64
#elif defined(ENV32BIT)
	constexpr unsigned LIMBSIZE = 32;
	using uint_default_t = uint32_t;
	#define BITNESS 32
#else
	#error ENV**BIT MACRO is not defined!
#endif

#include <assert.h>

#include <string>
#include <array>

#include <string_view>

#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>

#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A, B)

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define MAX(a,b) ((a) < (b) ? (a) : (b))

#define MAX_BITSIZE 256
#define MAX_LIMB_COUNT ((MAX_BITSIZE + LIMBSIZE - 1)/LIMBSZIE)

extern "C" uint64_t long_div_asm(uint64_t, uint64_t, uint64_t, uint64_t*);

//short for CUDA finite field
namespace arithmetic
{
	constexpr size_t bytes_per_limb = sizeof(uint_default_t);
	constexpr size_t bits_per_limb = bytes_per_limb * 8;

	using limb_index_t = uint8_t;
	//-1 stands for incorrect position
	using bit_pos_t = int32_t;

	//clz stands for count leading zeroes
	static bit_pos_t clz(uint_default_t x)
	{
#if defined(_MSC_VER)
	#if defined(_WIN64)
		#define clz_impl(x) __lzcnt64(x)
	#else
		#define clz_impl(x) __lzcnt(x)
	#endif
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
		#define clz_impl(x) __builtin_clzll(x)
	#else defined(__i386__)
		#define clz_impl(x) __builtin_clz(x)
	#endif
#else
	#error Operation system is unspecified
#endif
		return clz_impl(x);
	}

#if defined(ENV64BIT)
	//taken from here: https://github.com/ridiculousfish/libdivide/blob/master/libdivide.h
	//TODO: it is much better to use inline assembly

	static uint64_t double_precision_division(uint64_t u1, uint64_t u0, uint64_t v, uint64_t *r)
	{
		return long_div_asm(u1, u0, v, r);
#if FALSE
		const uint64_t b = (1ULL << 32); // Number base (16 bits)
		uint64_t un1, un0; // Norm. dividend LSD's
		uint64_t vn1, vn0; // Norm. divisor digits
		uint64_t q1, q0; // Quotient digits
		uint64_t un64, un21, un10; // Dividend digit pairs
		uint64_t rhat; // A remainder
		int32_t s; // Shift amount for norm

		// If overflow, set rem. to an impossible value,
		// and return the largest possible quotient
		if (u1 >= v) {
			if (r != NULL)
				*r = (uint64_t)-1;
			return (uint64_t)-1;
		}

		// count leading zeros
		s = clz(v);
		if (s > 0) {
			// Normalize divisor
			v = v << s;
			un64 = (u1 << s) | ((u0 >> (64 - s)) & (-s >> 31));
			un10 = u0 << s; // Shift dividend left
	}
		else {
			// Avoid undefined behavior
			un64 = u1 | u0;
			un10 = u0;
		}

		// Break divisor up into two 32-bit digits
		vn1 = v >> 32;
		vn0 = v & 0xFFFFFFFF;

		// Break right half of dividend into two digits
		un1 = un10 >> 32;
		un0 = un10 & 0xFFFFFFFF;

		// Compute the first quotient digit, q1
		q1 = un64 / vn1;
		rhat = un64 - q1 * vn1;

		while (q1 >= b || q1 * vn0 > b * rhat + un1) {
			q1 = q1 - 1;
			rhat = rhat + vn1;
			if (rhat >= b)
				break;
		}

		// Multiply and subtract
		un21 = un64 * b + un1 - q1 * v;

		// Compute the second quotient digit
		q0 = un21 / vn1;
		rhat = un21 - q0 * vn1;

		while (q0 >= b || q0 * vn0 > b * rhat + un0) {
			q0 = q0 - 1;
			rhat = rhat + vn1;
			if (rhat >= b)
				break;
		}

		// If remainder is wanted, return it
		if (r != NULL)
			*r = (un21 * b + un0 - q0 * v) >> s;

		return q1 * b + q0;
#endif
	}

	static uint64_t double_precision_multiplication(uint64_t x, uint64_t y, uint64_t* high)
	{
#if defined(_MSC_VER)
		*high = __umulh(x, y);
#else
		// full 128 bits are x0 * y0 + (x0 * y1 << 32) + (x1 * y0 << 32) + (x1 * y1 << 64)
		uint32_t mask = 0xFFFFFFFF;
		uint32_t x0 = (uint32_t)(x & mask);
		uint32_t x1 = (uint32_t)(x >> 32);
		uint32_t y0 = (uint32_t)(y & mask);
		uint32_t y1 = (uint32_t)(y >> 32);
		uint32_t x0y0_hi = libdivide__mullhi_u32(x0, y0);
		uint64_t x0y1 = x0 * (uint64_t)y1;
		uint64_t x1y0 = x1 * (uint64_t)y0;
		uint64_t x1y1 = x1 * (uint64_t)y1;
		uint64_t temp = x1y0 + x0y0_hi;
		uint64_t temp_lo = temp & mask;
		uint64_t temp_hi = temp >> 32;

		*high = x1y1 + temp_hi + ((temp_lo + x0y1) >> 32);
#endif
		return x * y;
}

	//returns x + y;
	//if (x + y) overflows than increment value pointed by high
	static uint64_t addition_with_carry(uint64_t x, uint64_t y, uint64_t* high_ptr)
	{
		unsigned char carry = 0;
		uint64_t result;
#if defined(_MSC_VER)
#define add_carry CONCAT(_addcarry_u, BITNESS)
		carry = add_carry(carry, x, y, &result);
		if (carry)
			(*high_ptr)++;
#else
#error "Not implemented!"
#endif
		return result;
	}

#else

	static uint32_t double_precision_division(uint32_t u1, uint32_t u0, uint32_t v, uint32_t *r)
	{
		uint64_t n = (((uint64_t)u1) << 32) | u0;
		uint32_t result = (uint32_t)(n / v);
		*r = (uint32_t)(n - result * (uint64_t)v);
		return result;
	}

#endif

	//we use it to enable default initialization to zero
	struct limb
	{
		uint_default_t x = 0;
		limb(uint_default_t n = 0) : x(n) {}
	};
	
	//this class represents fixed size unsigned bignum
	//this class doesnt'c check for overflows
	//NB: there are many possible optimizations using MSB bit
	template<limb_index_t N = MAX_BITSIZE>
	class bignum
	{
	private:
		std::array<limb, N> limbs_;
		bit_pos_t MSB_position_ = -1;

		void constexpr calc_MSB()
		{
			for (size_t i = N - 1; i >= 0; i--)
			{
				auto num_of_zeroes = clz(limbs_[i].x);
				if (num_of_zeroes < bits_per_limb)
				{
					MSB_position_ = bits_per_limb * (i + 1) - num_of_zeroes;
					return;
				}
			}
			MSB_position_ = -1;
		}

	public:
		constexpr explicit bignum(uint_default_t n = 0)
		{
			limbs_[0] = n;
			MSB_position_ = clz(n);
		}

		constexpr explicit bignum(const std::initializer_list<uint_default_t>& num_list)
		{
			size_t list_len = num_list.size();
			assert(list_len <= N);
			limb_index_t i = list_len - 1;

			for (auto elem: num_list)
				limbs_[i--] = elem;
			calc_MSB();
		}

		template<limb_index_t M>
		constexpr explicit bignum(const bignum<M>& other)
		{
			static_assert(M <= N, "incorrect number of limbs");

			for (limb_index_t i = 0; i < M; i++)
				limbs_[i] = other[i];
			MSB_position_ = other.get_MSB_pos();
		}

		//NB: this costructor can't be constexpr!
		explicit bignum(const std::string& str)
		{			
			static constexpr size_t chars_per_limb = 2 * bytes_per_limb;

			const size_t str_len = str.size();
			assert(str_len <= 2 * bytes_per_limb * N);

			if (str_len == 0)
				return;
			
			boost::cnv::cstream ccnv;
			ccnv(std::hex)(std::skipws);

			size_t i = str_len;
			limb_index_t limb_index = 0;
			while (i > 0)
			{
				size_t j = (2 * bytes_per_limb > i ? 0 : i - 2 * bytes_per_limb);
				std::string_view str_view(str.c_str() + j, i - j);
				i -= (i > 2 * bytes_per_limb ? 2 * bytes_per_limb : i);
				auto opt_val = boost::convert<uint_default_t>(str_view, ccnv);
				if (opt_val.has_value())
					limbs_[limb_index++].x = opt_val.get();
				else
					throw std::runtime_error("Incorrect conversion");
			}

			calc_MSB();
		}

		bignum(const bignum& other) = default;
		bignum(bignum&& other) = default;
		bignum& operator=(const bignum&) = default;
		bignum& operator=(bignum&&) = default;

		bool operator<(const bignum& other) const
		{
			for (limb_index_t i = N - 1; i >= 0; i--)
				if (limbs_[i].x < other.limbs_[i].x)
					return true;
			return false;
		}

		bool operator==(const bignum& other) const
		{
			for (limb_index_t i = 0; i < N; i++)
				if (limbs_[i].x != other.limbs_[i].x)
					return false;
			return true;
		}

		bool operator>=(const bignum& other) const
		{
			return (*this == other) || (other < *this);
		}

		bool operator>(const bignum& other) const
		{
			return (other < *this);
		}

		bool get_bit(size_t index)
		{
			assert((index >= 0) && (index <= sizeof(uint_default_t) * N));
			auto num = limbs_[index / N].x;
			return CHECK_BIT(num, N);
		}

		//this method works incorrectly in the case of overflows
		bignum& operator+=(const bignum& other)
		{
			unsigned char carry = 0;
			for (limb_index_t i = 0; i < N; i++)
#if defined(_MSC_VER)
#define add_carry CONCAT(_addcarry_u, BITNESS)
				carry = add_carry(carry, limbs_[i].x, other.limbs_[i].x, &limbs_[i].x);
#else
#error "Not implemented!"
#endif
			bit_pos_t j = MAX(MSB_position_, other.MSB_position_);
			//MSB_position_ = (get_bit(j + 1) ? j + 1 : j);
			return *this;
		}

		//this method works incorrectly in the case of overflows
		bignum& operator-=(const bignum& other)
		{
			unsigned char borrow = 0;
			for (limb_index_t i = 0; i < N; i++)
#if defined(_MSC_VER)
#define sub_borrow CONCAT(_subborrow_u, BITNESS)
				borrow = sub_borrow(borrow, limbs_[i].x, other.limbs_[i].x, &limbs_[i].x);
#else
#error "Not implemented!"
#endif
			calc_MSB();
			return *this;
		}

		uint_default_t operator[](limb_index_t index) const
		{
			assert(index >= 0 && index < N);
			return limbs_[index].x;
		}

		decltype(auto) get_MSB_pos() const { return MSB_position_; }

		template<limb_index_t M>
		friend std::ostream& operator<<(std::ostream& os, const bignum<M>& num);

		template<limb_index_t T, limb_index_t M>
		friend constexpr bignum<T + M> operator*(const bignum<T>&, const bignum<M>&);

		template<limb_index_t T, limb_index_t M>
		friend constexpr std::pair<bignum<T - M + 1>, bignum<M>> operator/(const bignum<T>&, const bignum<M>&);

		template<limb_index_t T>
		friend constexpr std::pair<bignum<T>, bignum<1>> operator/(const bignum<T>&, const bignum<1>&);

		/*template<limb_index_t M>
		constexpr bignum<N + M> shift_left()
		{
			bignum<N> shifted_v;
			for (limb_index_t i = 0; i < M; i++)
				shifted_v.limbs_[N - i - 1].x = v[M - 1 - i];
		}*/
	};

	template<limb_index_t N>
	inline bool operator<(const bignum<N>& lhs, const bignum<N>& rhs)
	{
		for (limb_index_t i = N - 1; i >= 0; i--)
			if (lhs[i] < rhs[i])
				return true;
		return false;
	}

	template<limb_index_t N>
	std::ostream& operator<<(std::ostream& os, const bignum<N>& num)
	{
		boost::cnv::cstream ccnv;
		ccnv(std::showbase)(std::uppercase)(std::hex);

		os << "0x";
		for (limb_index_t i = N - 1; i >= 0; i--)
		{
			auto opt_str = boost::convert<std::string>(num.limbs_[i].x, ccnv);
			if (opt_str.has_value())
				os << opt_str;
			else
				throw std::runtime_error("Incorrect conversion");
		}
			
		return os;
	}

	template<limb_index_t N>
	constexpr bignum<N> operator+(const bignum<N>& u, const bignum<N>& v)
	{
		bignum<N> r(u);
		r += v;
		return r;
	}

	template<limb_index_t N>
	constexpr bignum<N> operator-(const bignum<N>& u, const bignum<N>& v)
	{
		bignum<N> r(u);
		r -= v;
		return r;
	}

	//we prefer to store multiplication and division operations separately
	//to explicitely make them constexpr
	//NB: we do not use the fastest possible algorithm for multiplication
	template<limb_index_t N, limb_index_t M>
	constexpr bignum<N + M> operator*(const bignum<N>& u, const bignum<M>& v)
	{
		bignum<N + M> w;
		limb_index_t j = 0;

		while (j < M)
		{
			limb_index_t i = 0;
			uint_default_t k = 0;

			while (i < N)
			{
				uint_default_t high_word = 0;
				uint_default_t low_word = 0;
				low_word = double_precision_multiplication(u[i], v[j], &high_word);
				low_word = addition_with_carry(low_word, w[i + j], &high_word);
				low_word = addition_with_carry(low_word, k, &high_word);
				k = high_word;
				w.limbs_[i + j] = low_word;
				//here algo is incorrect - large division should be used!
				i++;
			}
			w.limbs_[N + j] = k;
			j++;
		}

		w.MSB_position_ = u.get_MSB_pos() + v.get_MSB_pos();
		return w;
	}

	//NB: in reality it is possible for N to be smaller than M
	//we return both the quotient and remainder
	template<limb_index_t N, limb_index_t M>
	constexpr std::pair<bignum<N - M + 1>, bignum<M>> operator/(const bignum<N>& u, const bignum<M>& v)
	{
		static_assert(N >= M, "Incorrect params for division");

		bignum<N - M + 1> q;
		bignum<M> r;

		bignum<N> temp_u;
		for (limb_index_t i = 0; i < N; i++)
			temp_u.limbs_[i].x = u[i];

		bignum<N> shifted_v;
		for (limb_index_t i = 0; i < M; i++)
			shifted_v.limbs_[N - i - 1].x = v[M - 1 - i];
		//for (limb_index_t i = M - 1; i >= 0; i--)
			//shifted_v.limbs_[i + N - M].x = v[i];

		bignum<2> divisor_highest_terms;
		divisor_highest_terms.limbs_[0].x = v[M - 2];
		divisor_highest_terms.limbs_[1].x = v[M - 1];

		bignum<3> divisor_highest_terms_ext(divisor_highest_terms);

		while (temp_u >= shifted_v)
		{
			q.limbs_[N - M].x++;
			temp_u -= shifted_v;
		}

		for (limb_index_t i = N - 1; i >= M; i--)
		{
			if (temp_u[i] >= v[M - 1])
				q.limbs_[i - M].x = ~(uint_default_t)(0);
			else
			{
				uint_default_t r;
				q.limbs_[i - M].x = double_precision_division(u[i], u[i - 1], v[M - 1], &r);
			}

			bignum<3> x, y;

			x = divisor_highest_terms * bignum<1>(q[i - M]);

			y.limbs_[0].x = u[i - 2];
			y.limbs_[1].x = u[i - 1];
			y.limbs_[2].x = u[i];

			//but x > y doesn't work for some reasons!
			while (y < x)
			{
				q.limbs_[i - M].x--;
				x -= divisor_highest_terms_ext;
			}

			bignum<N - 1> q_rem_;
			for (limb_index_t i = M - 1; i >= 0; i--)
				q_rem_.limbs_[i - M].x = v[i];

			bignum<N> q_rem(bignum<1>(q.limbs_[i - M].x) * q_rem_);

			if (temp_u > q_rem)
			{
				temp_u -= q_rem;
			}
			else
			{
				for (limb_index_t i = M - 1; i >= 0; i--)
					q_rem_.limbs_[i - M].x = v[i];

				bignum<N> t_;
				for (limb_index_t i = M - 1; i >= 0; i--)
					q_rem_.limbs_[i - M].x = v[i];
				
				temp_u -= q_rem;
				temp_u += t_;
				q.limbs_[i - M].x--;
			}
		}

		q.MSB_position_ = u.get_MSB_pos() - v.get_MSB_pos();

		return std::make_pair(q, r);
	}

	//previous algorithm works incorrectly if size of divident is one limb
	template<limb_index_t N>
	constexpr std::pair<bignum<N>, bignum<1>> operator/(const bignum<N>& u, const bignum<1>& v)
	{
		uint_default_t r = 0;
		limb_index_t i = N;
		const uint_default_t divider = v[0];
		bignum<N> q;

		while (i-- > 0)
		{			
			q.limbs_[i].x = double_precision_division(r, u[i], divider, &r);
		}

		return std::make_pair(q, bignum<1>(r));
	}

}

#endif // !BIGNUM_HPP_


