#ifndef POLYNOMIAL_HPP_
#define POLYNOMIAL_HPP_

#include <vector>
#include <string>

#include <boost/operators.hpp>

namespace arithmetic
{
	//TODO: use BOOST/C++ concept (via static assert that K is commutative unital ring)
	//TODO: this type should be incrementable\decrementable, it should also be ordered if the base field was ordered
	template<typename K>
	class polynomial : public boost::euclidean_ring_operators<polynomial<K>>
	{
	private:
		//TODO: use small vector with preallocated number of terms for optimization purposes
		const std::vector<K> coefficients_;
	public:
		//TODO: provide and use default for K
		//TODO: sometimes it is better to make parameter non reference
		//we assume that Z belongs to K
		explicit polynomial(const K&);
		explicit polynomial(unsigned int n);
		explicit polynomial(const std::string& str);

		polynomial(const polynomial& other) = default;
		polynomial(polynomial&& other) = default;
		polynomial& operator=(const polynomial&) = default;
		polynomial& operator=(polynomial&&) = default;

		polynomial& operator+(const polynomial& other);		
		polynomial& operator-(const polynomial& other);
		polynomial& operator*(const polynomial& other);
		polynomial& operator/(const polynomial& other);
		polynomial& operator%(const polynomial& other);

		//TODO: the following should be defined if K is itself ordered
		//bool operator<(const polynomial& other);
		//bool operator==(const Field& other)
		
		template<typename K>
		friend std::ostream& operator<<(std::ostream& os, const polynomial<K>& poly);

		//static public get random poly (of fixed degree)
	};

	template<typename K>
	std::ostream& operator<<(std::ostream& os, const polynomial<K>& poly)
	{
		//TBD
		return os;
	}
};

#endif
