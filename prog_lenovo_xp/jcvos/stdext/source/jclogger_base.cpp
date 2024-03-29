
#include "../include/jclogger_base.h"
#include "../include/jcexception.h"
#include <time.h>

CJCLogger * CJCLogger::Instance(void)
{
    static CJCLogger __logger_object__(NULL);
    return &__logger_object__;
}

CJCLogger::CJCLogger(CJCLoggerAppender * appender)
    : m_appender(appender)
	, m_ts_cycle(0)
{
#ifdef WIN32
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_ts_cycle = 1000.0 * 1000.0 / (double)(freq.QuadPart);
#endif

}

CJCLogger::~CJCLogger(void)
{
    LoggerCategoryMap::iterator it = m_logger_category.begin();
    LoggerCategoryMap::iterator last_it = m_logger_category.end();
    for (; it != last_it; it++)
    {
        CJCLoggerNode * node = (it->second);
        node->Delete();
    }
    m_logger_category.clear();
}



CJCLoggerNode * CJCLogger::EnableCategory(const CJCStringW & name, int level)
{
    LoggerCategoryMap::iterator it = m_logger_category.find(name);
    if (it == m_logger_category.end() )
    {
        // Create new logger
        CJCLoggerNode * logger = CJCLoggerNode::CreateLoggerNode(name, level);
        std::pair<LoggerCategoryMap::iterator, bool> rc;
        rc = m_logger_category.insert(LoggerCategoryMap::value_type(name, logger) );
        it = rc.first;
    }
    return it->second;
}

CJCLoggerNode * CJCLogger::GetLogger(const CJCStringW & name/*, int level*/)
{
    CJCLoggerNode * logger = NULL;
    LoggerCategoryMap::iterator it = m_logger_category.find(name);
    if ( it != m_logger_category.end() )
    {
        logger = it->second;
    }
    return logger;
}

bool CJCLogger::Configurate(FILE * config)
{
	JCASSERT(config);
	static const JCSIZE MAX_LINE_BUF=128;
	wchar_t * line_buf = new wchar_t[MAX_LINE_BUF];
	//stdext::auto_array<TCHAR> line_buf(MAX_LINE_BUF);

	while (1)
	{
		if ( !_fgetts(line_buf, MAX_LINE_BUF, config) ) break;
		// Remove EOL
		if ( _T('#') == line_buf[0] ) continue;

		wchar_t * sep = _tcschr(line_buf, _T('\n'));
		if (sep) *sep = 0;
		
		sep = _tcschr(line_buf, _T(','));
		if (NULL == sep) continue;
		//{
		//	delete [] line_buf;
		//	stdext::CJCException err(_T("Log config format error"), stdext::CJCException::ERR_APP);
		//	throw err;
		//}
		*sep = 0;

		const wchar_t * str_level = sep + 1;
		int level = 0;
		if		( 0 == _tcscmp(_T("NONE"), str_level) )			level = LOGGER_LEVEL_NONE;
		else if ( 0 == _tcscmp(_T("ALERT"), str_level) )		level = LOGGER_LEVEL_ALERT;
		else if ( 0 == _tcscmp(_T("CRITICAL"), str_level) )		level = LOGGER_LEVEL_CRITICAL;
		else if ( 0 == _tcscmp(_T("RELEASEINFO"), str_level) )	level = LOGGER_LEVEL_RELEASEINFO;
		else if ( 0 == _tcscmp(_T("ERROR"), str_level) )		level = LOGGER_LEVEL_ERROR;
		else if ( 0 == _tcscmp(_T("WARNING"), str_level) )		level = LOGGER_LEVEL_WARNING;
		else if ( 0 == _tcscmp(_T("NOTICE"), str_level) )		level = LOGGER_LEVEL_NOTICE;
		else if ( 0 == _tcscmp(_T("TRACE"), str_level) )		level = LOGGER_LEVEL_TRACE;
		else if ( 0 == _tcscmp(_T("DEBUGINFO"), str_level) )	level = LOGGER_LEVEL_DEBUGINFO;
		else if ( 0 == _tcscmp(_T("ALL"), str_level) )			level = LOGGER_LEVEL_ALL;
			
		const wchar_t * name = line_buf;
		LoggerCategoryMap::iterator it = m_logger_category.find(name);
		if (it == m_logger_category.end() )
		{
			// Create new logger
			CJCLoggerNode * logger = CJCLoggerNode::CreateLoggerNode(name, level);
			std::pair<LoggerCategoryMap::iterator, bool> rc;
			rc = m_logger_category.insert(LoggerCategoryMap::value_type(name, logger) );
			it = rc.first;
		}
		it->second->SetLevel(level);
	}
	delete [] line_buf;
	return true;
}




bool CJCLogger::RegisterLoggerNode(CJCLoggerNode * node)
{
    const CJCStringW &node_name = node->GetNodeName();
    bool rc = true;
    LoggerCategoryMap::iterator it = m_logger_category.find(node_name);
    if ( it != m_logger_category.end() )
    {
        // check if the exist node is equalt to node
        rc = ((it->second) == node);
    }
    else
    {
        m_logger_category.insert(LoggerCategoryMap::value_type(node_name, node) );
    }
    return rc;
}

bool CJCLogger::UnregisterLoggerNode(CJCLoggerNode * node)
{
    return m_logger_category.erase(node->GetNodeName()) > 0;
}

void CJCLogger::WriteString(LPCTSTR str)
{
    //JCASSERT(m_appender);
	if (m_appender) m_appender->WriteString(str);
}


void CJCLogger::Init(CJCLoggerAppender * appender)
{
	Instance()->SetAppender(appender);
}





////////////////////////////////////////////////////////////////////////////////
// --CJCLoggerNode

double CJCLoggerNode::m_ts_cycle = CJCLogger::Instance()->GetTimeStampCycle();

CJCLoggerNode::CJCLoggerNode(const CJCStringW & name, int level, CJCLogger * logger)
    : m_category(name)
    , m_level(level)
    , m_logger(logger)
{
    if (NULL == m_logger) m_logger = CJCLogger::Instance();
    JCASSERT(m_logger);
	m_logger->RegisterLoggerNode(this);
}

CJCLoggerNode::~CJCLoggerNode(void)
{
}

void CJCLoggerNode::LogMessageFunc(LPCSTR function, LPCTSTR format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    LogMessageFuncV(function, format, argptr);
}

void CJCLoggerNode::LogMessageFuncV(LPCSTR function, LPCTSTR format, va_list arg)
{
	DWORD col_sel = m_logger->GetColumnSelect();
    TCHAR str_msg[LOGGER_MSG_BUF_SIZE];
	LPTSTR str = str_msg;
	int ir = 0, remain = LOGGER_MSG_BUF_SIZE;


#if defined(WIN32)
	if (col_sel & CJCLogger::COL_THREAD_ID)
	{
		DWORD tid = GetCurrentThreadId();
		ir = stdext::jc_sprintf(str, remain, _T("<TID=%04d> "), tid);
		if (ir >=0 )  str+=ir, remain-=ir;
	}

	if (col_sel & CJCLogger::COL_TIME_STAMP)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		//unsigned int inow = static_cast<unsigned int>( (now.QuadPart) & (long long)(0xFFFFFFFF));
		double ts = now.QuadPart * m_ts_cycle;
		unsigned int inow = (unsigned int)(ts);
		ir = stdext::jc_sprintf(str, remain, _T("<TS=%u> "), inow);
		if (ir >=0 )  str+=ir, remain-=ir;
	}
#endif
	if (col_sel & CJCLogger::COL_REAL_TIME)
	{
		TCHAR strtime[32];
		time_t now = time(NULL);
		struct tm * now_t = localtime(&now);

		stdext::jc_strftime(strtime, 32, _T("%H:%M:%S"), now_t);
		strtime[24] = 0;
		ir = stdext::jc_sprintf(str, remain, _T("<%ls> "), strtime);
		if (ir >=0 )  str+=ir, remain-=ir;
	}

	if (col_sel & CJCLogger::COL_COMPNENT_NAME)
	{
		ir = stdext::jc_sprintf(str, remain, _T("<COM=%ls> "), m_category.c_str());
		if (ir >=0 )  str+=ir, remain-=ir;
	}

	if (col_sel & CJCLogger::COL_FUNCTION_NAME)
	{
#if defined(WIN32)
		ir = stdext::jc_sprintf(str, remain, _T("<FUN=%S> "), function);
#else
		ir = stdext::jc_sprintf(str, remain, _T("<FUN=%s> "), function);
#endif
		if (ir >=0 )  str+=ir, remain-=ir;
	}
    ir = stdext::jc_vsprintf(str, remain, format, arg);
	if (ir >= 0 )  str+=ir, remain-=ir;
	stdext::jc_strcat(str, remain, _T("\n"));

    m_logger->WriteString(str_msg);
}


///////////////////////////////////////////////////////////////////////////////
//-- JCStaticLoggerNode
JCStaticLoggerNode::JCStaticLoggerNode(
        const CJCStringW & name, int level)
    : CJCLoggerNode(name, level)
{
    CJCLogger * logger = CJCLogger::Instance();
    JCASSERT(logger);
    bool rc = logger->RegisterLoggerNode(this);
    JCASSERT(rc);
}

JCStaticLoggerNode::~JCStaticLoggerNode()
{
    CJCLogger * logger = CJCLogger::Instance();
    JCASSERT(logger);
    bool rc = logger->UnregisterLoggerNode(this);
    JCASSERT(rc);
}
	
///////////////////////////////////////////////////////////////////////////////
//-- CJCStackTrace

CJCStackTrace::CJCStackTrace(CJCLoggerNode *log, const char *func_name)
    : m_log(log)
    , m_func_name(func_name)
{
	if (log && log->GetLevel() >= LOGGER_LEVEL_TRACE)
	{
		log->LogMessageFunc(func_name, _T("[TRACE IN]"));
	}
}

CJCStackTrace::~CJCStackTrace(void)
{
	if (m_log && m_log->GetLevel() >= LOGGER_LEVEL_TRACE)
	{
		m_log->LogMessageFunc(m_func_name.c_str(), _T("[TRACE OUT]"));
	}
}

void CJCStackTrace::Parameter(const char * format, ...)
{
    //va_list arglist;
    //va_start(arglist, format);

    //char buf[MAX_LOG_BUF_SIZE];

    //oscl_vsnprintf(buf, LOGGER_MSG_BUF_SIZE-1, format, arglist);
    //CJCLoggerNode_LOGMSG( PVLOGMSG_INST_HLDBG, m_log, PVLOGMSG_STACK_TRACE
    //    , (0, "%s : [PARAMETERS] %s", m_func_name.get_cstr(), buf) );
}

