/*
 * Copyright (c) Jack M. Thompson WebKruncher.com, exexml.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebKruncher nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Jack M. Thompson ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Jack M. Thompson BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef KRUNCHER_TOOLS_H
#define KRUNCHER_TOOLS_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <iomanip>

#include <unistd.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <syslog.h>


#ifdef UNIX
#include <uuid.h>
#endif

#include <termios.h>
#include <stdio.h>

extern volatile bool TERMINATE;
inline void SetExitFlag()
{ 
	std::cerr << "Setting exit flag" << std::endl;
	TERMINATE=true; 
}
extern unsigned long VERBOSITY;

#define VERB_EVERYTHING 	0XFFFFFFFFFFFF
#define VERB_CONSOLE 		0X800000000000
#define VERB_SERVICE 		0X10000
#define VERB_ALWAYS 		0XFFFFFFFFFFFF
#define VERB_SIGNALS 		0X1000
#define VERB_CRUD 		0X2000
#define VERB_PSOCKETS 		0X100
#define VERB_SSOCKETS 		0X200
#define VERB_ASOCKETS 		(VERB_SSOCKETS|VERB_PSOCKETS)

#define VERB_REST_1		0X1
#define VERB_REST_2		0X2
#define VERB_REST_3		0X4

#define VERB_CURSOR_1		0X10
#define VERB_CURSOR_2		0X20
#define VERB_CURSOR_3		0X40

#define VERB_KREST_1		0X10000
#define VERB_KREST_2		0X20000
#define VERB_KREST_3		0X40000

namespace KruncherTools
{
	enum Crud
	{
		Create, Retreive, Update, Delete
	};


	using namespace std;
	inline const string GetUuid(){return "";}
#if 0
#ifdef UNIX
	inline const string GetUuid()
	{
		uuid_t  UuId;
		unsigned int uuidstatus( 0 );
		uuid_create(&UuId, &uuidstatus); 
		char* result( nullptr );	
		uuid_to_string(&UuId, &result, &uuidstatus);
		const string uuid( (char*) result );
		free( result );
		return uuid;
	}
#endif

#ifdef LINUX
	inline const string GetUuid()
	{
		unsigned char result[ 512 ];
		uuid_generate(result);
		const string uuid( (char*) result );
		return uuid;
	}
#endif
#endif



	inline ostream& separator( ostream& o ) { o << "/"; return o; }
	inline ostream& widetxt(ostream& o) {o<<left<<setfill(' ')<<setw(12); return o;}
	inline ostream& widenum(ostream& o) {o<<right<<setfill(' ')<<setw(12)<<fixed<<setprecision( 2 ); return o;}
	inline ostream& tab( ostream& o ) { o << "\t"; return o; }
	inline ostream& semi( ostream& o ) { o << ";"; return o; }
	inline ostream& coma( ostream& o ) { o << ","; return o; }
	inline ostream& normal( ostream& o ) { o << "\033[0m"; return o; }
	inline ostream& equ( ostream& o ) { o << "="; return o; }
	inline ostream& bold( ostream& o ) { o << "\033[1m"; return o; }
	inline ostream& italic( ostream& o ) { o << "\033[3m"; return o; }
	inline ostream& rvid( ostream& o ) { o << "\033[7m"; return o; }
	inline ostream& ulin( ostream& o ) { o << "\033[4m"; return o; }
	inline ostream& blink( ostream& o ) { o << "\033[5m"; return o; }

	inline ostream& blackbk( ostream& o ) { o << "\033[40m"; return o; }
	inline ostream& redbk( ostream& o ) { o << "\033[41m"; return o; }
	inline ostream& greenbk( ostream& o ) { o << "\033[42m"; return o; }
	inline ostream& yellowbk( ostream& o ) { o << "\033[43m"; return o; }
	inline ostream& bluebk( ostream& o ) { o << "\033[44m"; return o; }
	inline ostream& mgentabk( ostream& o ) { o << "\033[45m"; return o; }
	inline ostream& tealbk( ostream& o ) { o << "\033[46m"; return o; }
	inline ostream& whitebk( ostream& o ) { o << "\033[47m"; return o; }

	inline ostream& black( ostream& o ) { o << "\033[30m"; return o; } 
	inline ostream& red( ostream& o ) { o << "\033[31m"; return o; }
	inline ostream& green( ostream& o ) { o << "\033[32m"; return o; }
	inline ostream& yellow( ostream& o ) { o << "\033[33m"; return o; }
	inline ostream& blue( ostream& o ) { o << "\033[34m"; return o; }
	inline ostream& mgenta( ostream& o ) { o << "\033[35m"; return o; }
	inline ostream& teal( ostream& o ) { o << "\033[36m"; return o; }
	inline ostream& white( ostream& o ) { o << "\033[37m"; return o; }
	inline ostream& Endl( ostream& o ) { o << "\r\n"; return o; }


	inline ostream& scope( ostream& o ) { o << "::"; return o; }
	inline ostream& fence( ostream& o ) { o << "|"; return o; }
	inline ostream& colon( ostream& o ) { o << ":"; return o; }
	inline ostream& clrscr( ostream& o ) { o << "\033[2J"; return o; }
	inline ostream& clreol( ostream& o ) { o << "\033[K"; return o; }
	inline ostream& crgret( ostream& o ) { o << "\r"; return o; }

	inline ostream& logtime( ostream& o ) 
	{ 
		struct timeval when ;
		gettimeofday( &when, nullptr );
		const unsigned long tod( when.tv_sec*1E6 + when.tv_usec ); 
		o << fence << tod << fence; 
		return o; 
	}

	struct stringmap : map<string,string>
	{
		friend ostream& operator<<(ostream&,const stringmap&);

		ostream& operator<<(ostream& o) const
		{
			for ( const_iterator it=begin();it!=end();it++) o << it->first << ": " << it->second << ";" << endl;
			return o;
		}
	}; 
	inline ostream& operator<<(ostream& o,const stringmap& m) {return m.operator<<(o);}


	struct stringset : set<string>
	{
		virtual void split( string what, string _how )
		{
			while ( true )
			{
				const size_t where( what.find( _how ) );
				if ( where  == string::npos )
				{

					if ( what.size() ) insert( what );
					return;
				}
				insert( what.substr(0, where ) );
				what.erase( 0, where + _how.size() );
			}
		}
		friend ostream& operator<<(ostream&,const stringset&);

		ostream& operator<<(ostream& o) const
		{
			for ( const_iterator it=begin();it!=end();it++) o << (*it) << fence ;
			return o;
		}
	}; 
	inline ostream& operator<<(ostream& o,const stringset& m) {return m.operator<<(o);}


	struct stringstack : stack<string>
	{
		virtual void split( string what, string _how )
		{
			while ( true )
			{
				const size_t where( what.rfind( _how ) );
				if ( where  == string::npos )
				{
					if ( what.size() ) push( what );
					return;
				}

				const string b( what.substr( where+1, what.size()-1 ) ); 
				push( b );
				what.erase( where,  b.size()+1 );
			}
		}
	}; 


	struct stringvector : vector<string> 
	{
		stringvector( ) {}

		virtual void split( string what ) { return split( what, "|" ); }

		virtual void split( string what, string _how )
		{
			while ( true )
			{
				const size_t where( what.find( _how ) );
				if ( where  == string::npos )
				{

					if ( what.size() ) push_back( what );
					return;
				}
				push_back( what.substr(0, where ) );
				what.erase( 0, where + _how.size() );
			}
		}



		private:
		friend ostream& operator<<(ostream&,const stringvector&);
		ostream& operator<<(ostream& o) const
		{
			for (const_iterator it=begin();it!=end();it++) o << it->c_str() << endl;
			return o;
		}
	}; 

	inline ostream& operator<<(ostream& o,const stringvector& m) { return m.operator<<(o); }

	struct ignorecase_traits : public std::char_traits<char>
	{
		static bool eq(const char& c1,const char& c2) {return toupper(c1)==toupper(c2);}
		static bool lt(const char& c1,const char& c2) {return toupper(c1)<toupper(c2);}
		static int compare(const char* s1,const char* s2,std::size_t n)
		{
			for (std::size_t i=0;i<n;i++) 
				if (!eq(s1[i],s2[i])) 
					return lt(s1[i],s2[i])?-1:1;
			return 0;
		}
		static const char* find(const char* s,std::size_t n,const char& c)
		{
			for(std::size_t i=0;i<n;i++)
				if (eq(s[i],c))
					return &(s[i]);
			return 0;
		}
	};

	struct icstring : public std::basic_string<char,ignorecase_traits>
	{
		icstring (){}
		icstring (const icstring& c) :
			std::basic_string<char,ignorecase_traits>(c.c_str()) { }
		icstring (const string& c) :
			std::basic_string<char,ignorecase_traits>(c.c_str()) { }
		icstring (const char* c) :
			std::basic_string<char,ignorecase_traits>(c) { }
		icstring& operator=(const char* c)
		{
			std::basic_string<char,ignorecase_traits>::operator=(c);
			return *this;
		}
		icstring& operator=(const icstring& c)
		{
			std::basic_string<char,ignorecase_traits>::operator=(c.c_str());
			return *this;
		}
		friend ostream& operator<<(ostream&,const icstring&);
		ostream& operator<<(ostream& o) const
		{
			o << c_str() ;
			return o;
		}
	}; 
	inline ostream& operator<<(ostream& o,const icstring& m) {return m.operator<<(o);}

	struct icstringmap : map<icstring,string> 
	{
		friend ostream& operator<<(ostream&,const icstringmap&);
		ostream& operator<<(ostream& o) const
		{
			for ( const_iterator it=begin();it!=end();it++) o << it->first << ":" << it->second << ";" << endl;
			return o;
		}
	}; 
	inline ostream& operator<<(ostream& o,const icstringmap& m) {return m.operator<<(o);}

	struct Args : map< string, string >
	{
		Args() {}
		Args( int _argc, char** _argv ) : processname( _argv[ 0 ] ), argc( _argc ), argv( _argv ) {}
		Args( const Args& that ) : argc( that.argc ), argv( that.argv ), processname( that.processname )
		{
			for ( const_iterator it=that.begin();it!=that.end();it++) insert( *it );
		} 
		bool operator()( int _argc, char** _argv )
		{
			argc=_argc;
			argv=_argv;
			Args& me( *this );
			return !!me;
		}
		string svalue( const string& shortname, const string& longname, const string& defval ) const;
		int ivalue( const string& shortname, const string& longname, const int& defval ) const;
		virtual operator bool ();
		const string ProcessName() const { return processname; }
		protected:
		const string processname;
		virtual ostream& operator<<(ostream& o) const;
		int argc;
		char** argv;
		friend ostream& operator<<(ostream&,const Args&);
	}; 

	inline ostream& operator<<(ostream& o,const Args& m) { return m.operator<<(o); }

	inline string Args::svalue( const string& shortname, const string& longname, const string& defval ) const
	{
		const_iterator sit( find( shortname ) );
		const_iterator lit( find( longname ) );
		if ( ( sit == end() ) && ( lit == end() ) ) return defval;
		const_iterator it( ( sit != end() ) ? sit : lit );
		return it->second.c_str();  
	}

	inline int Args::ivalue( const string& shortname, const string& longname, const int& defval ) const
	{
		const_iterator sit( find( shortname ) );
		const_iterator lit( find( longname ) );
		if ( ( sit == end() ) && ( lit == end() ) ) return defval;
		const_iterator it( ( sit != end() ) ? sit : lit );
		string n( it->second );
		if ( n.empty() ) throw longname;
		if ( n[0]=='+' ) n.erase( 0, 1 );
		int r(  atoi( n.c_str() ) );  
		return r;
	}

	inline Args::operator bool ()
	{
		string opt;
		for ( int j=0; j<argc; j++ )
		{
			string arg( argv[ j ] );
			if ( arg.empty() ) throw string("Impossible");
			if ( arg[ 0 ] == '-' )
			{
				if ( ! opt.empty() ) 
				{
					insert( pair<string,string>( opt, "" ) );
					opt.clear();
				}
				opt=arg;
			} else {
				if ( ! opt.empty() )
				{
					insert( pair<string,string>( opt, arg ) );
					opt.clear();
				}
			}
		}

		if ( ! opt.empty() )
			if ( find( opt ) == end() )  
			{
				insert( pair<string,string>( opt, "" ) );
				opt.clear();
			}

		return true;
	}

	inline ostream& Args::operator<<(ostream& o) const
	{
		for (const_iterator it=begin();it!=end();it++) 
			o << it->first << colon << it->second << semi<< endl;
		return o;
	}


	template <typename VT > struct SharedMem
	{
		SharedMem( unsigned long _memid )
			: memid( _memid ) 
		{
			key_t key( memid );
			int shmid( 0 );
			void* shm( nullptr );

			if ((shmid = shmget(key, sizeof( VT ), IPC_CREAT | 0666)) < 0) 
				throw string("Cannot create shared memory");

			if ((shm = shmat(shmid, nullptr, 0)) == (char *) -1) 
				throw string("Cannot attach to shared memory");

			variable=(VT*) shm;
		}

		operator VT& ()
		{
			if ( ! variable ) throw string("Shared memory is not initialized");
			return *variable;
		}

		private:
		const unsigned long memid;
		VT* variable;
	};





	struct TestCase : private map< string, string >
	{ 
		TestCase() : ndx( 0 ) {}
		int operator < ( const TestCase& that ) const
		{
			if ( empty() ) throw string("Empty test case");
			if ( that.empty() ) throw string("Empty that test case");
			const_iterator ait( find("Name") );
			if ( ait == end() ) throw string("Invalid test case");
			const_iterator bit( that.find("Name") );
			if ( bit == end() ) throw string("Invalid test case");
			const string& a( ait->second );
			const string& b( bit->second );
			if ( a < b ) return 1;
			return ndx < that.ndx;
		}
		operator string()
		{
			const_iterator ait( find("Name") );
			if ( ait == end() ) throw string("Invalid test case");
			const string& a( ait->second );
			if ( a.empty() ) throw string("Empty test case");
			stringstream ss;
			ss <<  a << "." << ndx;
			return ss.str();
		}
		void operator = ( string name )
		{ 
			string n( "Name" );
			insert( pair<string,string>(  n , name ) ); 
		}
		void operator++() { ndx++; }
		private:
		int ndx;
		friend ostream& operator<<(ostream&, const TestCase&);
		ostream& operator<<(ostream& o) const
		{
			if ( empty() ) throw string("Empty test case");
			o << begin()->first << endl;
			return o;
		}
	};
	inline ostream& operator<<(ostream& o, const TestCase& k) { return k.operator<<(o);}

	struct TestCases : set< TestCase > {}; 

	inline void change( string& what, const string where, const string with ) 
	{
		while ( true )
		{
			size_t sop(what.find( where ));
			if ( sop == string::npos ) break;
			what.replace( sop, where.size(), with );
		}
	}

	inline void sanitize( string& what )
	{
		//cerr << "Sanitize( " << endl << what << endl << " ) " << endl;
#if 0
		if ( true )
		{
			auto hexdump = [](string a, stringstream& ss)
			{
				ss << a.size() << " characters";
				for (auto it = a.cbegin() ; it != a.cend(); ++it)
				{
					char c( *it );
					ss << " 0X" << hex << (int) c << "(" << c << ")" ;
				}
				ss << endl;

			};
			stringstream sso;  
			hexdump( what, sso );
			ofstream oo( "/tmp/log.txt",ios::app );
			oo << "WHAT:" << endl << what<< endl;
			oo << "HEX:" << endl << sso.str() << endl;
		}
#endif

		if ( what.empty() ) return;
		stringstream ss;
		for ( string::iterator it=what.begin(); it!=what.end(); it++ )
		{
			char C( *it ); 
			if ( C == '\\' ) 
				ss << "\\";
			else if 
				( 
				 isalnum( C ) || 
				 ispunct( C ) || 
				 ( C == ' ' ) 
				) 
					ss << C;
		}
		what=ss.str();
	}

	inline const string tracetabs( const int Level ) { stringstream ss; for ( int j=0; j<(Level+1); j++ ) ss << tab; return ss.str();}


	inline string StripNewLines( string in )
	{
		while ( true )
		{
			size_t l( in.find_first_of( "\r\n" ) );
			if ( l != string::npos ) in.erase( l, 1 );
			else return in;
		}
	}
#if 0
	struct Options : map<string,string>
	{
		Options() = delete;
		Options( const int _argc, const char** _argv ) 
			: argc( _argc ), argv( _argv ) { }
		operator bool ()
		{
			HomePages( getenv("ScriptBase" ) );
			return true;
		}
		private:
		void HomePages( const char *ScriptBase )
		{
			if ( argc > 1 )
			{
				insert( make_pair<string,string>( "Root", string( argv[ 1 ] ) + string("/pages/" ) ) );
				return;
			}
			if ( ScriptBase ) 
			{
				insert( make_pair<string,string>( "Root", string( ScriptBase ) + string("/pages/" ) ) );
				return;
			}
			throw string("ScriptBase is not set");
		}
		const int argc;
		const char** argv;
	};
#endif

	inline string runpipe(string what)
	{
		string ret; 
		FILE* fp = popen((char*)what.c_str(),"r");
		if (fp == nullptr) {return "";}
		while ( true )
		{
			char buf[514];
			memset(buf,0,514);
			int l = fread(buf,1,512,fp);
			if (l > 0) ret+=buf;
			else return ret;
		}
		return ret;
	}

	inline void Log( const string txt )
	{
		if ( VERBOSITY != VERB_EVERYTHING ) return;
		stringstream ssmsg;
		ssmsg << fence << getpid() << fence << pthread_self() << fence << txt << fence;
		syslog(LOG_NOTICE, "%s", ssmsg.str().c_str() );
	}

	inline void Log( const string& where, const string txt )
	{
		if ( VERBOSITY & VERB_CONSOLE )
		{
			cerr << fence << where << fence << txt << fence << endl;
		}
		if ( VERBOSITY != VERB_EVERYTHING ) return;
		stringstream ssmsg;
		ssmsg << fence << where << fence << getpid() << fence << pthread_self() << fence << txt << fence;
		syslog(LOG_NOTICE, "%s", ssmsg.str().c_str() );
	}

	inline void Log( const unsigned long verbose, const string where, const string txt )
	{
		if ( VERBOSITY & VERB_CONSOLE )
		{
			cerr << fence << where << fence << txt << fence << endl;
		}
		bool Send( false );

		if ( verbose & VERBOSITY )  Send=true;
		if ( VERBOSITY == VERB_EVERYTHING ) Send=true;
		if ( verbose == VERB_ALWAYS ) Send=true;

		if ( ! Send ) return;

		stringstream ssmsg;
		ssmsg << fence <<  where << fence << getpid() << fence << pthread_self() << fence << txt << fence;
		syslog(LOG_NOTICE, "%s", ssmsg.str().c_str() );
	}

	inline void Log( const unsigned long verbose, const unsigned char _id, const string txt )
	{
		if ( ! ( verbose & VERBOSITY ) ) 
			if ( VERBOSITY != VERB_EVERYTHING ) return;

		stringstream ssmsg;
		ssmsg << fence <<  _id << fence << getpid() << fence << pthread_self() << fence << txt << fence;
		syslog(LOG_NOTICE, "%s", ssmsg.str().c_str() );
	}



	inline void ExceptionLog( const string what, const string details )
	{
		cerr << red << yellowbk << bold << "Logging Exception:" << yellow << redbk << endl << what << endl << details << endl << normal;
		const char* kruncherhome( getenv( "KRUNCHER_HOME" ) );
		if ( ! kruncherhome ) return;
		stringstream ssexceptionlog;
		ssexceptionlog << kruncherhome << "/log/exceptions.log";
		ofstream o( ssexceptionlog.str().c_str(), ios::app );
		if ( o.fail() ) return;
		stringstream lmsg;
		lmsg << fence << what << fence << details << fence << endl; 
		o << lmsg.str();
		openlog("ExceptionLog",LOG_NOWAIT|LOG_PID,LOG_USER); 
		syslog(LOG_NOTICE, "%s", lmsg.str().c_str() );
	}

	inline long RightSix( long t )
	{
		stringstream ss; ss << t;
		if ( ss.str().size() < 6 ) return 0;
		const int len( ss.str().size() );
		string rightside( ss.str().substr( len-6, len ) );
		return atol( rightside.c_str() );
	}

	struct timevector : vector<long>
	{
		void Average()
		{
			long much( 0 );
			for ( iterator it=begin();it!=end();it++)
			{
				if ( (*it) > 0 ) much+=(*it);
			}
			Avg=much/size();
		}
		long Avg;
		private:
		friend ostream& operator << (ostream& , const timevector& );
		ostream& operator << ( ostream& o ) const
		{
			for ( const_iterator it=begin(); it!=end(); it++ )
			{
				o << "\t" << (*it) << endl;
			}
			return o;
		}
	};

	inline ostream& operator << (ostream& o, const timevector& tv ) { return tv.operator<<(o); }

	inline string LogTime( long when )
	{
		const time_t Before( when );
		if ( Before == 0 ) return "";
		const long MicroSeconds( RightSix( Before ) );

		const time_t Seconds( Before / 1e6 );

		const struct tm * Tm( localtime( &Seconds ) );
		string When( asctime( Tm ) );
		{ const size_t eot( When.find("\n") ); if ( eot!=string::npos) When.resize( eot ); }
		stringstream logtime; 

		logtime << 
			Tm->tm_year+1900 << "-"  <<
			setfill( '0' ) << setw( 2 ) << Tm->tm_mon+1 << "-"  <<
			setfill( '0' ) << setw( 2 ) << Tm->tm_mday << 
			"T" <<
			setfill( '0' ) << setw( 2 ) << Tm->tm_hour << ":" <<
			setfill( '0' ) << setw( 2 ) << Tm->tm_min << ":" <<
			setfill( '0' ) << setw( 2 ) << Tm->tm_sec <<
			"." << setfill( '0' ) << setw( 6 ) << MicroSeconds << "Z";
		return logtime.str(); 
	}

	inline unsigned long long  NixTime( const string& timestamp )
	{
		// 2020-09-16T12:49:26.218670Z
		const string year ( timestamp.substr( 0, 4 ) );
		const string month( timestamp.substr( 5, 2 ) );
		const string day  ( timestamp.substr( 8, 2 ) );
		const string hour ( timestamp.substr( 11, 2 ) );
		const string min  ( timestamp.substr( 14, 2 ) );
		const string sec  ( timestamp.substr( 17, 2 ) );
		const string usec ( timestamp.size() > 20 ? timestamp.substr( 20, 6 ) : "0000" );

		if ( 0 )
			cout << timestamp << "," << 
				" year: " << year  <<
				" month: " << month  <<
				" day: " << day  <<
				" hour: " << hour  <<
				" min: " << min  <<
				" sec: " << sec  <<
				" usec: " << usec  <<
				endl;

		struct tm tim;
		tim.tm_year=atoi(year.c_str())-1900;
		tim.tm_mon=atoi(month.c_str())-1;
		tim.tm_mday=atoi(day.c_str());
		tim.tm_hour=atoi(hour.c_str());
		tim.tm_min=atoi(min.c_str());
		tim.tm_sec=atoi(sec.c_str());
		tim.tm_isdst=0;

		if ( 0 ) cout << "NixTime:" << mktime( &tim ) << endl;
		long much( mktime( &tim ) );
		much*=1000000;
		much+=atol( usec.c_str() ) ;
		return much;
	}



	inline string ActivityTime( const string& line )
	{
		if ( isdigit( line[0] ) )
			return line.substr( 0, strlen( "2020-09-12T12:35:31.944409" ) );
		else return "";
	}

	inline string TraceTime( const string& line )
	{
		if ( line.find("####[")==0)
		{
			const size_t firstbracket( line.find("[") );
			if ( firstbracket==string::npos) return "";
			const size_t secondbracket( line.find("[", firstbracket+1) );
			if ( secondbracket==string::npos) return "";
			return line.substr( secondbracket+2, strlen( "2020-09-12T12:35:31.944409" ) );
		} else return "";
	}

	struct TimeDeltas : vector<pair<string,time_t> > 
	{
		TimeDeltas( const double spikethreshold ) : SpikeThreshold( spikethreshold ) {}
		operator bool ()
		{
			long ttl( 0 );
			for ( iterator it=begin(); it!=end();it++)
				ttl+=it->second;
			avg=ttl/size();
			over=avg+(avg*SpikeThreshold);
			return true;
		}
		private:
		double SpikeThreshold;
		long avg,over;
		friend ostream& operator << (ostream&, const TimeDeltas&); 
		ostream& operator<<( ostream& o ) const 
		{
			o << "#Average:" << avg << endl;
			for ( const_iterator it=begin(); it!=end();it++)
			{
				if ( it->second > over )
					o << it->first << ":" << it->second << endl;	
			}
			return o;
		}
	};
	inline ostream& operator << (ostream& o, const TimeDeltas& d ) { return d.operator<<(o); }

	inline string LogTimeFromEntTime( string entime )
	{
		const size_t valid( entime.find("UTC" ) );
		if ( valid != entime.size()-strlen("UTC") ) return "";
		const size_t space( entime.find(" " ) );
		if ( space == string::npos ) return "";
		entime[ space ] = 'T';
		const size_t utc( entime.find(" ", space+1 ) );
		if ( utc == string::npos ) return "";
		if ( utc != string::npos ) entime.resize( utc );
		entime+=".000000";
		return entime;
	} 


	inline string HomeData( const string what )
	{
		const char* kruncherhome( getenv("KRUNCHER_HOME") );
		if ( ! kruncherhome ) return what;
		stringstream sstracename; sstracename << kruncherhome << "/log/" << what << "." << getpid() << "." << pthread_self() << ".log";
		return sstracename.str();
	} 

	struct Trace 
	{ 
		Trace() : pid( 0 ), tid( 0 ) {}
		Trace( const string what, pid_t _pid, pthread_t _tid ) 
			: fname(HomeData( what ) ), pid( _pid ), tid( _tid )
		{}

		void operator()(const string where, const string what ="" ) const
		{
			ofstream oo( fname.c_str(), ios::app ); 
			if ( ! what.empty() )
				oo << logtime << pid << fence << tid << fence << where << fence << what << fence << endl; 
			else
				oo << logtime << pid << fence << tid << fence << where << fence << endl; 
		}
		const string fname;
		const pid_t pid;
		const pthread_t tid;
	}; 


	inline string getpass()
	{
		struct termios oldterm, newterm;

		if (tcgetattr (STDIN_FILENO, &oldterm) != 0) return "";
		newterm = oldterm;
		newterm.c_lflag &= ~ECHO;
		if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &newterm) != 0) return "";

		string line;
		getline( cin, line );
		(void) tcsetattr (STDIN_FILENO, TCSAFLUSH, &oldterm);

		return line;
	}

	inline string Tabify( const string line, const int ntabs )
	{
		stringstream ss;
		stringvector lines;
		lines.split( line, "\n" );
		for ( stringvector::iterator it=lines.begin();it!=lines.end();it++)
		{
			string L( *it );
			for ( int j=0; j<ntabs; j++ ) ss << tab;
			ss << L << endl;
			
		}
		return ss.str();
	}

	inline bool onebitcheck( const unsigned long bits )
	{
		bool sett( false );
		unsigned long mask( 1 );
		while ( mask )
		{
			const bool s( bits & mask );
			if ( s )
			{
				if ( sett ) return true;
				sett=true;
			}
			mask <<= 1;
		}
		return false;
	}
		

	struct binarystring : basic_string< unsigned char> 
	{
		binarystring(){}
		binarystring( const binarystring& that ) : basic_string< unsigned char>( that ) {}
		binarystring( const unsigned char* that ) : basic_string< unsigned char>( that ) {}
		binarystring( const unsigned char* that, const size_t len ) : basic_string< unsigned char>( that, len ) {}
		pair< bool, size_t > nontext() const
		{
			//return pair< bool, size_t >( false, 0 );
			const basic_string< unsigned char>& me( *this );
			for ( size_t i=0; i < size(); i++ )
			{
				const unsigned char c( me[ i ] );
				if ( c == ' ' ) continue;
				if ( c == '\t' ) continue;
				if ( c == '\r' ) continue;
				if ( c == '\n' ) continue;
				if ( isalnum( c ) ) continue;
				if ( ispunct( c ) ) continue;
				return pair< bool, size_t >( true, i );
			}
			return pair< bool, size_t >( false, 0 );
		}
	};



	inline void ltrim(std::string &s)
	{
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	    }));
	}

	inline void rtrim(std::string &s)
	{
	    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	    }).base(), s.end());
	}

	inline void trim(std::string &s)
	{
	    ltrim(s);
	    rtrim(s);
	}

	inline bool isEmpty(const std::string &s) { return s.empty(); }
	inline size_t StartKeySize(const std::string &s) { return s.size(); }

	inline void* StartKeyDataPtr(const std::string &s) { return (void*) s.c_str(); }

	inline size_t EndKeySize(const std::string &s) { return 0; }

	inline void* EndKeyStr(const std::string &s) { return (void*) nullptr; }

	inline string StringOf(const std::string &s) {  return s; }

	inline string StringOfPtr(const void* start, const size_t sz) 
	{
		string ret;
		if ( ( start ) && ( sz ) ) ret.assign( (char*) start, sz );
		return ret;
	}

	inline string StartKeyOf(const std::string &s) {  return s; }
	inline string EndKeyKeyOf(const std::string &s) {  return s; }
	inline int KeyLimiter(const std::string &s) {  return 0; }
	inline int KeySkipper(const std::string &s) { return 0; }


	inline string Slice( const string& txt, const pair<size_t,size_t>& pos ) { return txt.substr( pos.first, pos.second-pos.first ); }

	inline string& assign( string& a, const string& s )
	{
		a.assign(s);
		return a;
	}

	inline string TimeFormat( struct tm* tim )
	{
		if ( ! tim ) return string("");
		stringstream ss;
		ss << 
			setw( 4 ) << setfill( '0' ) << tim->tm_year+1900 << "-" <<
			setw( 2 ) << setfill( '0' ) << tim->tm_mon+1 << "-" <<
			setw( 2 ) << setfill( '0' ) << tim->tm_mday << "T" <<
			setw( 2 ) << setfill( '0' ) << tim->tm_hour << ":" <<
			setw( 2 ) << setfill( '0' ) << tim->tm_min << ":" <<
			setw( 2 ) << setfill( '0' ) << tim->tm_sec << "Z"; 
		string s( ss.str() );
		return s;
	};

	inline string DeBrace( const string what )
	{
		const size_t openbracket( what.find( "{" ) );
		if ( openbracket == string::npos ) return what;
		const size_t closebracket( what.find( "}", openbracket ) );
		if ( closebracket == string::npos ) return what;
		if ( openbracket == string::npos ) return what;
		const string a( what.substr( 0, openbracket ) );
		const string b( what.substr( closebracket+1, what.size()-1 ) );
				       
		return a + string( "*" ) + b;
	}


	typedef vector<char*> CharVector;

	struct CmdArguments : CharVector
	{
		CmdArguments() {}
		CmdArguments( const string& what )
		{
			stringvector parts;
			parts.split( what, " " );
			for ( stringvector::const_iterator it=parts.begin();it!=parts.end();it++)
				push_back( (char*) it->c_str() );
			push_back( nullptr );
		}
		bool exists( const string what )
		{
			return (find( begin(), end(), what ) == end() );
		}
		void operator()( const string& name, const string& value )
		{
			push_back( (char*) name.c_str() );
			push_back( (char*) value.c_str() );
		}
		void operator=( const string& what )
		{
			stringvector parts;
			parts.split( what, " " );
			for ( stringvector::const_iterator it=parts.begin();it!=parts.end();it++)
				push_back( (char*) it->c_str() );
		}
	};

	inline CmdArguments Defaults( int argc, char** argv, CharVector& defaults )
	{
		CmdArguments args;
		for ( int j=0;j<argc;j++) args.push_back( argv[ j ] );
		for (CharVector::iterator ait=defaults.begin();ait!=defaults.end();ait++)
		{
			const string a( *ait );
			if ( a.empty() ) continue;
			if ( a[ 0 ] == '-' )
			{
				if ( find( args.begin(), args.end(), a ) == args.end() )
				{
					args.push_back( *ait );
					ait++;
					if ( ait==args.end() ) break;
					const string v( *ait );
					if ( v.empty() ) break;
					if ( v[ 0 ] == '-' ) continue;
					args.push_back( *ait );
				}
			}
		}
		return args;
	}

	inline void forkpipe( const string exe, const KruncherTools::CharVector& args, const string input, ostream& out )
	{
		#define MAX_RESULT_BUFFER 4069
		int fd1[2];
		int fd2[2];
		pid_t pid;
		char line[ MAX_RESULT_BUFFER ];

		if ( (pipe(fd1) < 0) || (pipe(fd2) < 0) ) return ;
		if ( (pid = fork()) < 0 ) return ;

		else  if (pid == 0)     
		{
			// Child
			close(fd1[1]);
			close(fd2[0]);

			if (fd1[0] != STDIN_FILENO)
			{
				if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO) return ;
				close(fd1[0]);
			}

			if (fd2[1] != STDOUT_FILENO)
			{
				if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO) return ;
				close(fd2[1]);
			}


			if ( execvp( (const char*) exe.c_str(),  &args[0] ) < 0 ) return ; 

		} else {
			// Parent
			int rv;
			close(fd1[0]);
			close(fd2[1]); 

			if ( ! input.empty() )
			{
				if ( (size_t) write(fd1[1], input.c_str(), input.size() ) != (size_t) input.size() ) return ;
				close( fd1[1] );
				wait(NULL);
			} 


			while ( true )
			{
				memset( line, 0, MAX_RESULT_BUFFER );
				rv = read(fd2[0], line, MAX_RESULT_BUFFER );
				if ( rv <= 0 ) break;
				out << line;
			}

		}
	} 

	struct XmlBase
	{
		XmlBase(){}
	};

} // KruncherTools


#endif // KRUNCHER_TOOLS_H

