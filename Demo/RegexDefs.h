//---------------------------------------------------------------------------

#ifndef RegexDefsH
#define RegexDefsH

#include <string>

//#define USE_STD_REGEX

#if !defined( USE_STD_REGEX )
 #include <boost/regex.hpp>
#else
 #include <regex>
#endif

#if !defined( USE_STD_REGEX )
 using regex_type_a = boost::regex;
 using regex_cmatch_type_a = boost::cmatch;
 using regex_smatch_type_a = boost::smatch;
#else
 using regex_type_a = std::regex;
 using regex_cmatch_type_a = std::cmatch;
 using regex_smatch_type_a = std::smatch;
#endif

#if !defined( USE_STD_REGEX )
 using regex_type_w = boost::wregex;
 using regex_cmatch_type_w = boost::wcmatch;
 using regex_smatch_type_w = boost::wsmatch;
#else
 using regex_type_w = std::wregex;
 using regex_cmatch_type_w = std::wcmatch;
 using regex_smatch_type_w = std::wsmatch;
#endif

#if !defined(_UNICODE)
 using regex_type = regex_type_a;
 using regex_cmatch_type = regex_cmatch_type_a;
 using regex_smatch_type = regex_smatch_type_a;
 using std_string = std::string;
#else
 using regex_type = regex_type_w;
 using regex_cmatch_type = regex_cmatch_type_w;
 using regex_smatch_type = regex_smatch_type_w;
 using std_string = std::wstring;
#endif

//---------------------------------------------------------------------------
#endif


