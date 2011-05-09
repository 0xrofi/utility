#ifndef	OutputConsoleStream_H
#define	OutputConsoleStream_H

#include <Windows.h>
#include <tchar.h>
#include <string>

class OutputConsoleStream {
public:
	enum CONSOLE_OUTPUT {
		STDOUT,
		STDERR
	};
	OutputConsoleStream( const CONSOLE_OUTPUT output, const WORD &wAttributes ) :
		m_needfree( AllocConsole() != 0 ),
		m_hConsoleOutput( output == STDOUT ? GetStdHandle( STD_OUTPUT_HANDLE ) : GetStdHandle( STD_ERROR_HANDLE ) ),
		m_wAttributes( wAttributes ),
		m_restore( false )
	{}
	~OutputConsoleStream()
	{
		if( m_restore )
			SetConsoleTextAttribute( m_hConsoleOutput, m_wAttributes );
		if( m_needfree )
			FreeConsole();
	}

	template<class T>
	OutputConsoleStream operator<<( const T &arg ) {
		CONSOLE_SCREEN_BUFFER_INFO	csbi = {};

		if( m_restore ) {
			const_cast<bool&>(m_restore) = false;
			csbi.wAttributes = m_wAttributes;
		} else {
			GetConsoleScreenBufferInfo( m_hConsoleOutput, &csbi );
			SetConsoleTextAttribute( m_hConsoleOutput, m_wAttributes );
		}

		OutputConsoleStream		ocs( m_hConsoleOutput, csbi.wAttributes );
		ocs.output( arg );
		return ocs;
	}

	template<class T>
	OutputConsoleStream operator<<( const T *arg ) {
		CONSOLE_SCREEN_BUFFER_INFO	csbi = {};

		if( m_restore ) {
			const_cast<bool&>(m_restore) = false;
			csbi.wAttributes = m_wAttributes;
		} else {
			GetConsoleScreenBufferInfo( m_hConsoleOutput, &csbi );
			SetConsoleTextAttribute( m_hConsoleOutput, m_wAttributes );
		}

		OutputConsoleStream		ocs( m_hConsoleOutput, csbi.wAttributes );
		ocs.output( arg );
		return ocs;
	}

	friend OutputConsoleStream endl( OutputConsoleStream &ocs ) {
		ocs.output( _T("\n") );
		return OutputConsoleStream( ocs );
	}

private:
	OutputConsoleStream( const HANDLE &hConsoleOutput, const WORD &wAttributes ) :
		m_needfree( false ),
		m_hConsoleOutput( hConsoleOutput ),
		m_wAttributes( wAttributes ),
		m_restore( true )
	{
	}

	OutputConsoleStream( OutputConsoleStream &ocs ) :
		m_needfree( false ),
		m_hConsoleOutput( ocs.m_hConsoleOutput ),
		m_wAttributes( ocs.m_wAttributes ),
		m_restore( true )
	{
		const_cast<bool&>(ocs.m_restore) = false;
	}

	// char
	void output( const char &c ) {
		DWORD		nWritten;
		WriteConsoleA( m_hConsoleOutput, &c, 1, &nWritten, 0 );
	}

	// wchar_t
	void output( const wchar_t &wc ) {
		DWORD		nWritten;
		WriteConsoleW( m_hConsoleOutput, &wc, 1, &nWritten, 0 );
	}

	// char*
	void output( const char *str ) {
		DWORD		nWritten;
		WriteConsoleA( m_hConsoleOutput, str, strlen( str ), &nWritten, 0 );
	}

	// wchar_t*
	void output( const wchar_t *wstr ) {
		DWORD		nWritten;
		WriteConsoleW( m_hConsoleOutput, wstr, wcslen( wstr ), &nWritten, 0 );
	}

	// string
	void output( const std::string &str ) {
		output( str.c_str() );
	}

	// wstring
	void output( const std::wstring &wstr ) {
		output( wstr.c_str() );
	}

	// int
	void output( const int &i ) {
		TCHAR		str[1024];
		DWORD		nWritten;

		wsprintf( str, _T("%d"), i );
		WriteConsole( m_hConsoleOutput, str, _tcslen( str ), &nWritten, 0 );
	}

	// unsigned int
	void output( const unsigned int &i ) {
		TCHAR		str[1024];
		DWORD		nWritten;

		wsprintf( str, _T("%u"), i );
		WriteConsole( m_hConsoleOutput, str, _tcslen( str ), &nWritten, 0 );
	}

	// function
	OutputConsoleStream output( OutputConsoleStream (*func)( OutputConsoleStream &ocs ) ) {
		return ( *func )( *this );
	}

	const bool		m_needfree;
	const HANDLE	m_hConsoleOutput;
	const WORD		m_wAttributes;
	const bool		m_restore;
};

#endif	// OutputConsoleStream_H
