﻿#pragma once

#ifdef __linux__
#include "comm_linux.h"
#endif

#ifdef WIN32
#include "comm_windows.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>

// 关于整数的定义

// 对于32位系统，所有用于内存索引，下标的类型全部定义为32位
typedef UINT	JCSIZE;
//JCSIZE operator = (size_t s)	{return (JCSIZE)s;};

// 外部索引，全部使用64位
typedef UINT64	FILESIZE;

inline UINT64 MAKEQWORD(UINT a, UINT b)
{
	return ((UINT64)(a) & 0xffffffff) | (((UINT64)b & 0xffffffff) << 32);
}


// 关于字符串和UNICODE的定义

typedef std::basic_string<char>		CJCStringA;
typedef std::basic_string<wchar_t>		CJCStringW;


#ifdef _UNICODE

typedef CJCStringW	CJCStringT;

#else
//typedef char 
typedef CJCStringA	CJCStringT;
#endif



typedef const char * LPCSTR;
typedef char * LPSTR;

typedef const TCHAR * LPCTSTR;
typedef TCHAR * LPTSTR;

// Secure functions for std
#include "jc_secure.h"

//typedef unsigned int	UINT;
//typedef unsigned char	BYTE;

// 用于字符串标示符的快速比较。通常，对于字符串的标识符，比较双方都引自字符串常量，
// 因此可以简单的比较其指针是否一致。如果指针不一致，在比较字符串内容

#define FastCmpT(str1, str2) \
	(str1 == str2 || _tcscmp(str1, str2) == 0 )

#define FastCmpA(str1, str2) \
	(str1 == str2 || strcmp(str1, str2) == 0 )

//#define _SASSERT(exp)   __ASSERT__(exp)

#ifdef  _DEBUG

extern "C"
{
	void LogAssertion(LPCSTR source_name, int source_line, LPCTSTR str_exp);
}

#define JCASSERT(exp) {										\
    if ( ! (exp) ) {										\
        LogAssertion(__FILE__, __LINE__, _T(#exp));		\
        jcbreak;										\
    }   }
#else

#define JCASSERT(exp)

#endif

