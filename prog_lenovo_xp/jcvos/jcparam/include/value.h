#pragma once


#include "ivalue.h"

#include <vector>
#include <map>

#define EMPTY _T("")

//#include "variable_interface.h"

namespace jcparam
{
	enum VALUE_TYPE
	{
		VT_UNKNOW,
		VT_BOOL,
		VT_CHAR,		VT_UCHAR,
		VT_SHORT,		VT_USHORT,
		VT_INT,			VT_UINT,
		VT_INT64,		VT_UINT64,
		VT_FLOAT,		VT_DOUBLE,
		VT_STRING,
	};

	IValue * CreateTypedValue(VALUE_TYPE vt, void * data=NULL);
	VALUE_TYPE StringToType(LPCTSTR str);

	class ClassNameNull
	{
	public:
		static LPCTSTR classname() {return EMPTY;};
	};

	template <typename DATATYPE, typename CONVERTOR = CConvertor<DATATYPE> >
	class CValue 
		: virtual public IValue
		, virtual public IValueFormat
		, virtual public IValueConvertor
		, public CJCInterfaceBase
		, public CTypedValueBase
	{
	public:
		static CValue * Create()	{return new CValue();};
		static CValue * Create(const DATATYPE &d)	{return new CValue(d);};
	protected:
		CValue(void) {};
		CValue(const DATATYPE &d) : m_val(d) {}
	
		CValue(const CValue<DATATYPE, CONVERTOR> & p) : m_val(p.m_val) 	{}


		virtual ~CValue(void) {}

	public:
		virtual void GetValueText(CJCStringT & str) const 	
		{
			CONVERTOR::T2S(m_val, str);
		}

		virtual void SetValueText(LPCTSTR str) 
		{
			CONVERTOR::S2T(str, m_val);
		}

		CValue<DATATYPE, CONVERTOR> & operator = (const DATATYPE p)	
		{ 
			m_val = p; return (*this);
		}

		operator DATATYPE & () {	return m_val; };
		operator const DATATYPE & () const { return m_val;}

		CValue<DATATYPE, CONVERTOR> & operator = ( const CValue<DATATYPE, CONVERTOR> & p) 
		{
			m_val = p.m_val; 
			return (*this);
		}
		void SetData(DATATYPE * pdata) 	{ if (pdata) m_val = *pdata; }
		//DATATYPE GetData() {return m_val;}
	public:
		virtual void Format(FILE * file, LPCTSTR format)
		{
			CJCStringT str;
			GetValueText(str);
			stdext::jc_fprintf(file, _T("%s"), str.c_str());
		}

		virtual bool QueryInterface(const char * if_name, IJCInterface * &if_ptr)
		{
			JCASSERT(NULL == if_ptr);
			bool br = false;
			if ( FastCmpA(IF_NAME_VALUE_FORMAT, if_name) )
			{
				if_ptr = static_cast<IJCInterface*>(this);
				if_ptr->AddRef();
				br = true;
			}
			else br = __super::QueryInterface(if_name, if_ptr);
			return br;
		}
	protected:
		DATATYPE m_val;
	};

	class CParamSet : virtual public IValue, public CJCInterfaceBase
	{
	public:
		typedef std::map<CJCStringT, IValue *>	PARAM_MAP;
		typedef PARAM_MAP::iterator ITERATOR;

	public:
		static CParamSet * Create(void) { return new CParamSet(); };
	protected:
		CParamSet(void);
		virtual ~CParamSet(void);

	public:
		//virtual void GetValueText(CJCStringT & str) const;
		//virtual void SetValueText(LPCTSTR str);
		virtual void GetSubValue(LPCTSTR param_name, IValue * & value);
		virtual void SetSubValue(LPCTSTR name, IValue * val);
		//virtual void Remove(LPCTSTR name);
		virtual LPCTSTR GetProperty(LPCTSTR prop) const { return _T("set");};


		bool InsertValue(const CJCStringT & param_name, IValue* value);
		bool RemoveValue(LPCTSTR param_name);

		// enumator
		ITERATOR Begin(void)	{ return m_param_map.begin(); };
		ITERATOR End(void)		{ return m_param_map.end();	};

	protected:
		PARAM_MAP m_param_map;
	};

	template <typename DATATYPE>
	class CSimpleArray :
		virtual public IValue, public CJCInterfaceBase, public CTypedValueBase
	{
	protected:
		CSimpleArray(JCSIZE count) : m_size(count) { m_array = new DATATYPE[count]; };
		virtual ~CSimpleArray(void) { delete[] m_array; };

	public:
		DATATYPE* Lock(void)	{ return m_array; }
		const DATATYPE* Lock(void) const {return m_array;}
		void Unlock(void) const {};
		operator const DATATYPE * () const {return m_array;}
		JCSIZE GetSize(void) const { return m_size; }

	protected:
		DATATYPE *	 m_array;
		JCSIZE		m_size;
	};

	// 元素不能为NULL
	class CValueArray : virtual public IValue, public CJCInterfaceBase, public CTypedValueBase
	{
	protected:
		typedef std::vector<IValue *> VALUE_VECTOR;
		typedef VALUE_VECTOR::iterator VALUE_ITERATOR;

	public:
		CValueArray(void) {};
		virtual ~CValueArray(void);

	public:
		//virtual void GetValueText(CJCStringT & str) const;
		//virtual void SetValueText(LPCTSTR str);

		virtual bool GetValueAt(int index, IValue * &value);
		virtual bool PushBack(IValue * value);
		virtual LPCTSTR GetProperty(LPCTSTR prop) const { return _T("array");};

		JCSIZE GetSize()	{ return (JCSIZE)m_value_vector.size(); }

		VALUE_VECTOR	m_value_vector;
	};

	typedef std::pair<CJCStringT, IValue*>	KVP;
};
