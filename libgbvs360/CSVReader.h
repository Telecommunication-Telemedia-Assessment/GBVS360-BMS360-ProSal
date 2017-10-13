// **************************************************************************************************
//
// The MIT License (MIT)
// 
// Copyright (c) 2017 Pierre Lebreton
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
// associated documentation files (the "Software"), to deal in the Software without restriction, including 
// without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the 
// following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial 
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// **************************************************************************************************



#ifndef _CSVREADER_
#define _CSVREADER_

#include <boost/function.hpp>

#include <vector>
#include <map>


template<typename T>
class CSVReader {

private:
	std::vector< std::vector<T> > 	datas;
	std::map<std::string, int> 		columnnames;

public:
	bool 					open		(const std::string& filename, const char *separator = ",", bool has_header = false);
/*	const std::vector<T> &	getColumn	(const std::string& colname) const;*/
	const std::vector<T> &	getColumn	(size_t num) const;

	std::vector<T>  		filter		(boost::function<bool (T)>, size_t lookup_column, size_t lookup_values) const;

	template<typename T2>
	std::vector<T>			lookup      (const std::vector<T2>& search, size_t lookup_column, size_t lookup_values) const;

	inline const std::map<std::string, int> &getColumnNames() const 		{ return columnnames; 	}
	inline size_t 			getCols		() const							{ return datas.size(); 	}

/*	const std::vector<T> & operator[]	(const std::string& colname) const;*/
	const std::vector<T> & operator[]	(size_t num) const;

};


#include "CSVReader.hpp"


#endif
