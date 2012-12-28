
#include "../include/value.h"
#include "../include/table_base.h"

using namespace jcparam;

// ColumnInfo --

//template <> void CTypedColInfo<char, CConvertor<char> >::CreateValue(void * src, IValue * & val)
//{
//	JCASSERT(NULL == val);
//	CValue<char>
//}

// CColumn --

CColumn::CColumn(ITable * tab, const CColInfoBase * col_info)
	: m_parent_tab(tab)
	, m_col_info(col_info)
{
	JCASSERT(m_parent_tab);
	JCASSERT(m_col_info);
	m_parent_tab->AddRef();
}

CColumn::~CColumn(void)
{
	m_parent_tab->Release();
	m_parent_tab = NULL;
}

JCSIZE CColumn::GetRowSize() const
{
	JCASSERT(m_parent_tab);
	return m_parent_tab->GetRowSize();
}

void CColumn::GetRow(JCSIZE index, IValue * & val)
{
	JCASSERT(NULL == val);
	stdext::auto_interface<IValue> row;
	m_parent_tab->GetRow(index, row);
	ITableRow * _row = dynamic_cast<ITableRow*>((IValue*)row);
	JCASSERT(_row);
	if (_row) _row->GetColumnData(m_col_info, val);
}

JCSIZE CColumn::GetColumnSize() const
{
	return 1;
}
	
void CColumn::Append(IValue * source)
{
	THROW_ERROR(ERR_UNSUPPORT, _T("Nor support"));
}

//void CColumn::GetValueText(CJCStringT & str) const
//{
//	THROW_ERROR(ERR_UNSUPPORT, _T("Nor support"));
//}
//
//void CColumn::SetValueText(LPCTSTR str)
//{
//	THROW_ERROR(ERR_UNSUPPORT, _T("Nor support"));
//}

void CColumn::GetSubValue(LPCTSTR name, IValue * & val)
{
	THROW_ERROR(ERR_UNSUPPORT, _T("Nor support"));
}

void CColumn::SetSubValue(LPCTSTR name, IValue * val)
{
	THROW_ERROR(ERR_UNSUPPORT, _T("Nor support"));
}
