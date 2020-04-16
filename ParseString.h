/*!
  This module is part of the SOFTDEV system
  Copyright (c) SCC Soft Computer, Inc. 1998-2006
  All Rights Reserved.
  This document contains unpublished, confidential and proprietary 
  information of Soft Computer Consultants, Inc. No disclosure or use of
  any portion of the contents of these materials may be made without the
  express written consent of Soft Computer Consultants, Inc.

  \file
  \date 16/11/2006 [created]
  \author Marcin Sus, Grzegorz Oronowicz
  \brief File provide bunch of methods for string parsing
   \brief static functions operating on the string
*/

#if !defined(AFX_PARSESTRING_H__617F3F19_AA1C_4FD2_BE30_E1B438C9CEA8__INCLUDED_)
#define AFX_PARSESTRING_H__617F3F19_AA1C_4FD2_BE30_E1B438C9CEA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <sstream>
#include <cstdlib>
#include <list>
#include <vector>
#include <afxtempl.h>
#include <regex>
#include <iostream>
#include <iterator>
#include <string>

//-----------------<ParseString>----------------//

template<bool> struct CompileTimeChecker
{
  //CompileTimeError(...);
};

template<> struct CompileTimeChecker<false> {};

#define STATIC_CHECK(expr, msg) \
        {\
          class ERROR_##msg {}; \
          (void)sizeof(CompileTimeChecker<(expr) != 0>((ERROR_##msg())));\
        }

template <typename T> 
inline CString ParseString(T arg) 
{ 
  STATIC_CHECK(1, USE_WITH_UNSUPPORTED_TYPE);
  
  return CString(""); 
}

template <>
inline CString ParseString<CString>(CString str)
{
  str.Replace("'", "''");

  return str;
}

template <>
inline CString ParseString<char*>(char* pStr)
{
  CString str(pStr);
  str.Replace("'", "''");

  return str;
}

template <>
inline CString ParseString<const char*>(const char* pStr)
{
  CString str(pStr);
  str.Replace("'", "''");

  return str;
}

//to avoid fingers aches
template <typename T>
inline CString _R(T str)
{
  return ParseString(str);
}

inline std::ostringstream& operator<<(std::ostringstream& os, __int64& n)
{
  char buffer[64] = {0};
  _i64toa(n, buffer, 10);
  os << buffer;
  return (os);
}

inline std::istringstream &operator >>(std::istringstream& iss, __int64& n)
{
  sscanf(iss.str().c_str(), "%I64d", &n);
  return iss;
}

template<typename T>
inline T FromString(const std::string& s)
{
  std::istringstream iss(s);
  T x = static_cast<T>(0);
  iss >> x;
  return x;
}

template<typename T>
inline T FromString(const CString& s)
{
  std::istringstream iss(static_cast<LPCTSTR>(s));
  T x = static_cast<T>(0);
  iss >> x;
  return x;
}

template<typename T>
inline CString ToString(const T& x)
{
  std::ostringstream oss;
  oss << static_cast<T>(x);
  return oss.str().c_str();
}

template<>
inline bool FromString<bool>(const CString& s)
{
  return (s == "TRUE") ? true : false;
}

template<>
inline CString ToString<bool>(const bool &x)
{
  return x ? "TRUE" : "FALSE";
}

template<>
inline CString ToString<CString>(const CString &x)
{
  return x;
}

template<typename T>
inline CString ToString(std::list<T>& values, CString sSeparator = _T(", "))
{
  std::list<T>::iterator it = values.begin();
  CString strOutput;
  CString strValue;

  for (; it != values.end(); ++it)
  {
    strValue = ToString<T>(*it);
    strOutput += strValue;
    if (it != --values.end())
    {
			strOutput += sSeparator;
    }
  }

  return strOutput;
}

template<typename T>
inline CString ToString(std::vector<T>& values, CString sSeparator = _T(", "))
{
  std::vector<T>::iterator it = values.begin();
  CString strOutput;
  CString strValue;

  for (; it != values.end(); ++it)
  {
    strValue = ToString<T>(*it);
    strOutput += strValue;
    if (it != --values.end())
    {
			strOutput += sSeparator;
    }
  }

  return strOutput;
}

template<typename T>
inline CString ToString(CArray<T, T>& values, CString sSeparator = _T(", "))
{
  CString strOutput;
  CString strValue;
  int nSize = values.GetSize();
  for (int nIndex = 0; nIndex < nSize; nIndex++)
  {
    strValue = ToString<T>(values[nIndex]);
    strOutput += strValue;
    if (nIndex < nSize - 1)
    {
			strOutput += sSeparator;
    }
  }
  return strOutput;
}

template<typename StrT>
int split(const char* str, const char* delim, std::vector<StrT>& results, bool empties = true)
{
  char* pstr = const_cast<char*>(str);
  char* r    = NULL;

  r = strstr(pstr, delim);
  int dlen = strlen(delim);

  while (r != NULL)
  {
    char* cp = new char[(r-pstr) + 1];
    memcpy(cp, pstr, (r - pstr));
    cp[(r - pstr)] = '\0';

    if (strlen(cp) > 0 || empties)
    {
      StrT s(cp);
      results.push_back(s);
    }

    delete[] cp;
    pstr = r + dlen;
    r = strstr(pstr, delim);
  }

  if (strlen(pstr) > 0 || empties)
  {
    results.push_back(StrT(pstr));
  }

  return results.size();
}

template<typename StrT>
int split(const char* str, const char* delim, std::list<StrT>& results, bool empties = true)
{
  std::vector<StrT> vec;
  results.clear();
  int nItems = split(str, delim, vec, empties);
  if (0 < nItems)
  {
    results.resize(nItems);
    std::copy(vec.begin(), vec.end(), results.begin());
  }
  return nItems;
}

//-----------------</ParseString>---------------//


class AFX_EXT_CLASS CParseString  
{
public:
	CParseString();
	virtual ~CParseString();
}

#endif // !defined(AFX_PARSESTRING_H__617F3F19_AA1C_4FD2_BE30_E1B438C9CEA8__INCLUDED_)
