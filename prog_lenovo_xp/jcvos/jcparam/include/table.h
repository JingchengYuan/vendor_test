#pragma once

#include "ivalue.h"

namespace jcparam
{
	class CColInfoBase
	{
	public:
		CColInfoBase(JCSIZE id, JCSIZE width, JCSIZE offset, LPCTSTR name)
			: m_id(id), m_width(width), m_offset(offset),
			m_name(name)
		{};

		virtual void GetText(void * row, CJCStringT & str) const {};
		virtual void CreateValue(BYTE * src, IValue * & val) const {};
		LPCTSTR name(void) const {return m_name.c_str(); }

	public:
		JCSIZE		m_id;
		JCSIZE		m_width;
		JCSIZE		m_offset;
		//LPCTSTR		m_name;
		CJCStringT	m_name;
	};

	template <typename VAL_TYPE, class CONVERTOR = CConvertor<VAL_TYPE> >
	class CTypedColInfo : public CColInfoBase
	{
	public:
		CTypedColInfo (int id, JCSIZE offset, LPCTSTR name)
			: CColInfoBase(id, sizeof(VAL_TYPE), offset, name)
		{};
		virtual void GetText(void * row, CJCStringT & str) const
		{
			VAL_TYPE * val = reinterpret_cast<VAL_TYPE*>((BYTE*)row + m_offset);
			CONVERTOR::T2S(*val, str);
		}
		virtual void CreateValue(BYTE * src, IValue * & val) const
		{
			JCASSERT(NULL == val);
			BYTE * p = src + m_offset;
			VAL_TYPE & vsrc= *((VAL_TYPE*)p);
			val = CValue<VAL_TYPE>::Create(vsrc);
		}
	};

	class CStringColInfo : public CColInfoBase
	{
	public:
		CStringColInfo (int id, JCSIZE offset, LPCTSTR name)
			: CColInfoBase(id, sizeof(CJCStringT), offset, name)
		{};
		virtual void GetText(void * row, CJCStringT & str) const
		{
			str = *(reinterpret_cast<CJCStringT*>((BYTE*)row + m_offset));
		}
		virtual void CreateValue(BYTE * src, IValue * & val) const
		{
			JCASSERT(NULL == val);
			BYTE * p = src + m_offset;
			val = CValue<CJCStringT>::Create(*(reinterpret_cast<CJCStringT*>(p)));
		}
	};	
	
	class ITableRow;
	class ITableColumn;

	class ITable : virtual public IValue
	{
	public:
		virtual JCSIZE GetRowSize() const = 0;
		virtual void GetRow(JCSIZE index, IValue * & row) = 0;
		virtual JCSIZE GetColumnSize() const = 0;
		virtual void Append(IValue * source) = 0;
	};

	class ITableRow : virtual public IValue
	{
	public:
		virtual JCSIZE GetRowID(void) const = 0;
		virtual int GetColumnSize() const = 0;

		// 从一个通用的行中取得通用的列数据
		//virtual void GetColumnData(int field, CJCStringT& data)	const = 0;
		//virtual bool GetColumnData(const char * field_name, CJCStringT& data) const = 0;

		virtual void GetColumnData(const CColInfoBase * col, IValue * & val) const = 0;
		virtual void GetColumnData(int field, IValue * &)	const = 0;
		virtual void GetColumnData(LPCTSTR field_name, IValue * &) const = 0;

		virtual LPCTSTR GetColumnName(int field_id) const = 0;
	};
};