﻿#pragma once

#include "../../stdext/stdext.h"

namespace jcparam
{
	const TCHAR PROP_TYPE[] = _T("type");
	const TCHAR PROP_SUBTYPE[] = _T("subtype");
	const TCHAR PROP_CLASS[] = _T("class");
	const TCHAR PROP_PLUGIN[] = _T("plugin");

	class IValue;

	class CValueEnumerateListener
	{
		virtual void OnEnumerate(const CJCStringT & name, IValue * val) = 0;
	};

	class IValue : virtual public IJCInterface 
	{
	public:
		// 转换器
		//virtual void GetValueText(CJCStringT & str) const = 0;
		//virtual void SetValueText(LPCTSTR str)  = 0;
		virtual void GetSubValue(LPCTSTR name, IValue * & val) = 0;
		// 如果name不存在，则插入，否则修改name的值
		virtual void SetSubValue(LPCTSTR name, IValue * val) = 0;
	};

	class IValueConvertor : virtual public IJCInterface
	{
	public:
		// 转换器
		virtual void GetValueText(CJCStringT & str) const = 0;
		virtual void SetValueText(LPCTSTR str)  = 0;
	};

	class IValueViewer : virtual public IJCInterface
	{

	};

	const char IF_NAME_VALUE_FORMAT[] = "IValueFormat";

	class IValueFormat : virtual public IJCInterface
	{
	public:
		virtual void Format(FILE * file, LPCTSTR format) = 0;

	};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CConvertor<T>
//	类型转换器
//	
	template <typename T>
//	class __declspec(dllexport) CConvertor
	class CConvertor
	{
	public:
		static void T2S(const T &t, CJCStringT &v);
		static void S2T(LPCTSTR, T &t);
	};

	template <typename T, UINT L>
	class CHexConvertorL
	{
	public:
		static void T2S(const T &typ, CJCStringT &str)
		{
			TCHAR _str[20];
			JCASSERT(L < 20);
			JCASSERT(L > 0);
			T _t = typ;
			wmemset(_str, _T('0'), L);
			_str[L] = 0;
			for (int ii = L-1; (ii >= 0) && (_t != 0); --ii, _t >>= 4)
			{
				BYTE v = (_t & 0xF);
				if ( v < 10) 	_str[ii] = _T('0') + v;
				else			_str[ii] = _T('A') + v - 10;
			}
			str = _str;
		}

		static void S2T(LPCTSTR str, T &typ)
		{
			LPTSTR end = NULL;
			typ = (T)stdext::jc_str2l(str, &end, (int)10);
		}
	};

	template <typename T>
	class CHexConvertor
	{
	public:
		static void T2S(const T &typ, CJCStringT &str)
		{
			TCHAR _str[20];
			stdext::jc_sprintf(_str, _T("%X"),  typ);
			str = _str;
		}

		static void S2T(LPCTSTR str, T &typ)
		{
			LPTSTR end = NULL;
			typ = (T)stdext::jc_str2l(str, &end, (int)10);
		}
	};

// 一些基本实现
	class CTypedValueBase : virtual public IValue
	{
	public:
		virtual void GetSubValue(LPCTSTR name, IValue * & val);
		virtual void SetSubValue(LPCTSTR name, IValue * val);
	};
};
