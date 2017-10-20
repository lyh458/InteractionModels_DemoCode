
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <boost/range.hpp>

using namespace boost::algorithm;
using namespace std;
using namespace boost;

#pragma once

template< typename SequenceSequenceT, typename RangeT, typename PredicateT >
inline SequenceSequenceT& split_n(
    SequenceSequenceT& Result,
    RangeT& Input,
	std::size_t count,
    PredicateT Pred,
    token_compress_mode_type eCompress=token_compress_off )
{
	

	vector<iterator_range<string::iterator> > res1;
	split(res1,Input,Pred,eCompress);

	if (res1.size() < count) count=res1.size();

	SequenceSequenceT res2;
	typedef typename SequenceSequenceT::value_type stv;
	for (size_t i=0; i<(count-1); i++)
	{
		

		stv r(res1[i].begin(),res1[i].end());
		res2.push_back(r);
	}
	
	stv r_end(res1[count-1].begin(),(res1.back()).end());
	
	res2.push_back(r_end);

	Result.swap(res2);

	return Result;

}
