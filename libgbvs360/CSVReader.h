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
