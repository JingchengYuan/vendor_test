#pragma once

#include "../../stdext/stdext.h"
#include "value.h"
#include <vector>
#include <map>

namespace jcparam
{
	class CArguDesc	// Argument Description
	{
	public:
		CArguDesc(LPCTSTR name, TCHAR abbrev, jcparam::VALUE_TYPE vt, LPCTSTR desc = NULL)
			: mName(name), mAbbrev(abbrev), mValueType(vt), mDescription(desc) {};
		CArguDesc(const CArguDesc & arg)
			: mName(arg.mName), mAbbrev(arg.mAbbrev)
			, mValueType(arg.mValueType), mDescription(arg.mDescription) {};
		CJCStringT mName;			// 参数名称
		TCHAR	mAbbrev;		// 略称
		VALUE_TYPE		mValueType;		// 值类型，降值解释为数字或者是字符串。对于COMMAND和SWITCH，忽略此项。
		LPCTSTR mDescription;
	};	

	class CParameterDefinition
	{
	public:
		enum PROPERTY
		{
			PROP_MATCH_PARAM_NAME = 0x80000000,
		};

		class RULE;

	public:
		CParameterDefinition(void);
		CParameterDefinition(const RULE & rule, DWORD properties = 0);
		virtual ~CParameterDefinition(void);
		void OutputHelpString(FILE * output) const;

		bool CheckParameterName(const CJCStringT & param_name) const;
		const CArguDesc * GetParamDef(TCHAR abbrev) const
		{
			JCASSERT(m_abbr_map);
			return m_abbr_map[(abbrev & 0x7f)];
		}

	protected:
		typedef std::map<CJCStringT, const CArguDesc *>	PARAM_MAP;
		typedef PARAM_MAP::const_iterator				PARAM_ITERATOR;
		typedef std::pair<CJCStringT, const CArguDesc*>	ARG_DESC_PAIR;
		typedef const CArguDesc *						PTR_ARG_DESC;

		PARAM_MAP			*m_param_map;
		PTR_ARG_DESC		*m_abbr_map;
		DWORD				m_properties;
	};

	class CParameterDefinition::RULE
	{
	public:
		RULE();
		RULE & operator () (LPCTSTR name, TCHAR abbrev, jcparam::VALUE_TYPE vt, LPCTSTR desc = NULL);
		friend class CParameterDefinition;

	protected:
		PARAM_MAP			* m_param_map;
		PTR_ARG_DESC		* m_abbr_map;

	};

	class CArguSet : public CParamSet
	{
	public:
		template <typename TYPE>
		bool GetValT(LPCTSTR arg, TYPE & val)
		{
			stdext::auto_interface<IValue> ptr_val;
			GetSubValue(arg, ptr_val);
			if ( ! ptr_val ) return false;
			CJCStringT str_tmp;
			IValueConvertor * cov = ptr_val.d_cast<IValueConvertor*>();
			if (cov) cov->GetValueText(str_tmp);
			CConvertor<TYPE>::S2T(str_tmp.c_str(), val);
			return true; 
		}

		bool GetCommand(JCSIZE index, CJCStringT & cmd);
		bool GetCommand(JCSIZE index, IValue * & val);
		void AddValue(const CJCStringT & name, IValue * value);
		bool Exist(const CJCStringT & name);
	};




	class CCmdLineParser
	{
	//public:
	//	CCmdLineParser(void);
	//	CCmdLineParser(const CRule & rule, DWORD properties = 0);
	//	virtual ~CCmdLineParser(void);

	public:
		//static const int	MAX_ARG_LEN;
		//void ParseParameter(
		//	CArguSet & arg, const CJCStringT & param_name, 
		//	IValue * value) const;

		//// for abbrev
		//void ParseParameter(CArguSet & argset, TCHAR abbrev, IValue * value) const;

		// Search parameter defination by abbrev
		//bool ParseParamName(CJCStringT & out_name, const CJCStringT & param_name, bool abbrev) const;

		static bool ParseCommandLine(const CParameterDefinition & param_def, LPCTSTR cmd, CArguSet & arg);
		//bool ParseCommandLine(int argc, TCHAR * argv[], CArguSet & arg) const;
		//void Parse(const CJCStringT & param_name, LPCTSTR value, CArguSet & arg) const;
		//void ParseAbbrev(LPCTSTR str, CArguSet & arg) const;		//??obsolete

	protected:
		static bool ParseToken(const CParameterDefinition & param_def, LPCTSTR token, JCSIZE len, CArguSet & arg);
		//bool AddParameterInfo(LPCTSTR name, TCHAR abbreviation, VALUE_TYPE type, LPCTSTR description = NULL, LPCTSTR defaultval=NULL);
		//void AddParameterInfo(const CArguDesc * arg);

	protected:
		static int		m_command_index;
	};
};
