#ifndef __RANGEPARSER_H__
#define __RANGEPARSER_H__

// rangeParser turns a packet selection such as "1001-1002,1007,1012-1020"
// into the list of numbers it stands for, in the order given, and appends
// them to "selection". A reversed range like "1020-1012" selects nothing.
// Returns 0 if all is well, 1 if the string is malformed (empty items,
// a trailing "," or "-", anything that is not a number...).
//
// Used by ddump and lastEvent. Plain C++ - this replaces the earlier version
// that needed boost (split and lexical_cast) for just this.

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

// a non-negative number, digits only, nothing else
inline int rangeParserNumber (const std::string &s, int &value)
{
  if ( s.empty() ) return 1;
  for (char c : s)
    {
      if ( ! std::isdigit( (unsigned char) c) ) return 1;
    }

  errno = 0;
  long v = std::strtol(s.c_str(), nullptr, 10);
  if ( errno == ERANGE || v > INT_MAX ) return 1;

  value = (int) v;
  return 0;
}

inline int rangeParser ( const std::string &spec, std::vector<int> &selection)
{
  if ( spec.empty() || spec.back() == ',' ) return 1;

  std::istringstream list(spec);
  std::string item;

  while ( std::getline(list, item, ',') )
    {
      // an item is "n" or "low-high"
      if ( item.empty() || item.back() == '-' ) return 1;

      std::istringstream range(item);
      std::string part;
      std::vector<std::string> r;
      while ( std::getline(range, part, '-') )
	{
	  r.push_back(part);
	}
      if ( r.size() > 2 ) return 1;

      int low, high;
      if ( rangeParserNumber(r[0], low) ) return 1;
      high = low;
      if ( r.size() == 2 && rangeParserNumber(r[1], high) ) return 1;

      for (int i = low; i <= high; ++i)
	{
	  selection.push_back(i);
	}
    }

  return 0;
}

#endif /* __RANGEPARSER_H__ */
