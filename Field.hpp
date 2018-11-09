#ifndef FIELD_HPP_
#define FIELD_HPP_

#include "bignum.hpp"

#include <boost/operators.hpp>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace arithmetic
{
	struct tag {};
	
	//NB: N doesn't do any particular work here, may be there is a way to get rid of it?
	template<limb_index_t N, typename field_tag>
	class Field : public boost::ordered_field_operators<Field<N, field_tag>>
	{
	private:
		static bignum<N> modulus_;
		//precomputed constant for Barett modular reduction
		//look in Menezes A., Van Oorschot P. C., Vanstone S. A. "Handbook of applied cryptography, p 604
		static bignum<N> barett_constant_;
		static bit_pos_t char_bitlen_;

		mutable bignum<N> rep_;
		mutable bool reduced_;

		void reduce() const
		{
			if (reduced_)
				return;
			
			//TBD (look in MOV, p.604)
		}

	public:
		//this method should be called first, before any playing with field
		
		static bool init(static const bignum<N>& modulus)
		{
			modulus_ = modulus;
			char_bitlen_ = modulus.get_MSB_pos();

			//barett_constant = 
		}

		//static public get random elements

		bit_pos_t get_msb_bit() const { return rep_.get_MSB_pos(); }

		explicit Field(uint_default_t n = 0) : rep_(n), reduced_(true) {}
		explicit Field(const bignum<N>& n) : rep_(n), reduced_(rep_.get_MSB_pos() < modulus_.get_MSB()) {}
		
		Field(const Field& other) = default;
		Field(Field&& other) = default;
		Field& operator=(const Field&) = default;
		Field& operator=(Field&&) = default;

		Field& operator+(const Field& other)
		{
			if (MAX(get_msb_bit(), other.get_msb_bit()) == bits_per_limb * N - 1)
			{
				reduce();
				other.reduce();
			}
			 
			rep_ += other.rep_;
			reduced_ = (get_msb_bit() < char_bitlen_);
		}

		Field& operator-(const Field& other)
		{

		}

		Field& operator*(const Field& other)
		{
			bignum<2 * N> temp(rep_ * other.rep_);
			//TODO: reimplement reduce algorithm for large modulus
			//reduce(temp);
		}

		Field& operator/(const Field& other)
		{

		}

		bool operator<(const Field& other)
		{
			reduce();
			other.reduce();
			return (rep_ < other.rep_);
		}

		bool operator==(const Field& other)
		{
			reduce();
			other.reduce();
			return (rep_ == other.rep_);
		}

		Field inverse()
		{

		}

		template<limb_index_t M, typename field_tag>
		friend std::ostream& operator<<(std::ostream& os, const Field<M, field_tag>& elem);
	};

	template<limb_index_t M, typename field_tag>
	std::ostream& operator<<(std::ostream& os, const Field<M, field_tag>& elem)
	{

	}

	/*ostream& operator<<(ostream& os, const Date& dt)
	{
		os << dt.mo << '/' << dt.da << '/' << dt.yr;
		return os;
	}*/
}

#endif // !FIELD_HPP_

