
#include "CSVReader.h"

#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <streambuf>
#include <limits>

template<typename T>
bool CSVReader<T>::open(const std::string& filename, const char *separator, bool has_header) {
	std::ifstream ifs;
	ifs.open(filename.c_str(), std::ifstream::in);

	if (!ifs.is_open()) {
		return false;
	}

	boost::regex regex("\\s*([\\+-]?[0-9]*\\.?[0-9]*e?[\\+-]?[0-9]*)\\s*");
	boost::cmatch matches;

	bool first_line = true;
	size_t col_counter = 0;
	size_t broken_counter = 0;

	std::string str;
	std::vector<std::string> tokens;

	while (getline(ifs, str)) {
		boost::split(tokens, str, boost::is_any_of(separator));

		// skip lines, which don't contain enough data
		if (tokens.size() < datas.size()) {
			broken_counter++;
			continue;
		}

		for (size_t i = 0; i < tokens.size(); i++) {
			std::string value = tokens[i];

			// use regexp to remove extra stuff such as spaces, tabs, ... around the column name
			if (has_header && first_line) {
				boost::regex regex("\\s*([\\w\\s]*)\\s*");
				boost::cmatch matches;
				if (boost::regex_match(value.c_str(), matches, regex)) {
					for (int i = 1; i < static_cast<int>(matches.size()) ; i++) {
						std::string match(matches[i].first, matches[i].second);
						columnnames[match] = static_cast<int>(col_counter);
					}
				}

			} else {
				// use regexp to remove extra stuff such as spaces, tabs, ...
				if (boost::regex_match(value.c_str(), matches, regex)) {
					for (size_t i = 1; i < matches.size(); i++) {
						std::string match(matches[i].first, matches[i].second);
						if (datas.size() <= col_counter) {
							datas.push_back(std::vector<T>());
						}
						datas[col_counter].push_back(boost::lexical_cast<T>(match));
					}
				}
			}

			++col_counter;
		}

		first_line = false;
		col_counter = 0;
	}

	if (broken_counter > 0)
		std::cout << "Warning: skipped " << broken_counter << " broken rows!" << std::endl;

	return true;
}


/*template<typename T>
const std::vector<T> &	CSVReader<T>::getColumn	(const std::string& colname) const {
	if(columnnames.find(colname) == columnnames.end())
		throw std::logic_error( "CSVReader<T>::getColumn : Column does not exist!" );

	return datas[columnnames[colname]];
}*/


/*template<typename T>
const std::vector<T> & CSVReader<T>::operator[]	(const std::string& colname) const {
	return getColumn(colname);
}*/


template<typename T>
const std::vector<T> &	CSVReader<T>::getColumn	(size_t num) const {
	if(num < 0 || num >= datas.size())
		throw std::logic_error( "CSVReader<T>::getColumn : Column does not exist!" );

	return datas[num];
}


template<typename T>
const std::vector<T> & CSVReader<T>::operator[] (size_t num) const {
	return getColumn(num);
}


template<typename T>
std::vector<T>  CSVReader<T>::filter(boost::function<bool (T)> op, size_t lookup_column, size_t lookup_values) const {
	std::vector<T> result;

	for(size_t i = 0 ; i < getColumn(lookup_column).size() ; ++i) {
		if(op(getColumn(lookup_column)[i])) {
			if(i < getColumn(lookup_values).size())
				result.push_back(getColumn(lookup_values)[i]);
			else
				throw std::logic_error( "CSVReader<T>::filter : The columns do not have the same number of elements!" );
		}
	}

	return result;
}


template<typename T>
template<typename T2>
std::vector<T> CSVReader<T>::lookup(const std::vector<T2>& search, size_t lookup_column, size_t lookup_values) const {
	std::vector<T> result;

	for (size_t r = 0; r < search.size(); ++r) {
		T minDiff = std::numeric_limits<T>::max();
		size_t minDiffPos = 0;

		for (size_t i = 0; i < getColumn(lookup_column).size(); ++i) {
			T diff = std::abs<T>(search[r] - getColumn(lookup_column)[i]);

			if (diff < minDiff) {
				minDiff = diff;
				minDiffPos = i;
			}
		}

		if (minDiffPos < getColumn(lookup_values).size())
			result.push_back(getColumn(lookup_values)[minDiffPos]);
		else
			throw std::logic_error( "CSVReader<T>::filter : The columns do not have the same number of elements!" );
	}

	return result;

}
