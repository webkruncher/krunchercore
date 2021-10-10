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

#ifndef KRUNCHER_DIRECTORY_H
#define  KRUNCHER_DIRECTORY_H
#include <dirent.h>
namespace KruncherDirectory
{
	using namespace KruncherTools;
	inline bool DirectoryExists( const string pathname )
	{
		struct stat sb;
		if ( stat( pathname.c_str(), &sb ) != 0 ) return false;
		return S_ISDIR(sb.st_mode);
	}

	struct Directory : stringvector
	{
		Directory() : recurse( false ) {}
		Directory( const string& _where ) : where( _where ), recurse( false ) {}
		Directory( const string& _where, const bool _recurse ) : where( _where ), recurse( _recurse ) {}
		void operator()( const mode_t mode=0777 )
		{
			split( where, "/" );
			if ( empty() ) return;
			string& start( *begin() );
			if ( start == "" )  start="/";
			stringstream sspath;
			for ( iterator it=begin();it!=end();it++ )
			{
				sspath<<*it;
				if ( sspath.str()!="/" ) sspath<<"/";
				if ( DirectoryExists( sspath.str() ) ) continue;
				cerr << "creating " << sspath.str() << endl;
				mkdir( sspath.str().c_str(), mode );
				
			}
		}
		virtual operator bool ();
		const stringvector& Directories() { return directories; }
		protected:
		mutable string where;
		bool recurse;
		stringvector directories;
		private:
		virtual Directory& NewSub( const string _where, const bool _recurse ) 
			{ throw string("No NewSub implemented for Directory" ); return *this;}
		virtual bool Filter( const dirent& ent ) const { return false; }
		virtual void operator+=( const dirent& );
		friend ostream& operator<<(ostream&,const Directory&);
		virtual ostream& operator<<(ostream& o) const
		{
			o << "[directories]" << endl << directories ;
			const stringvector& me( *this );
			o << "[files]"  << endl << me;
			return o;
		}
	}; 

	inline ostream& operator<<(ostream& o,const Directory& m) { return m.operator<<(o); }

	inline void Directory::operator+=( const dirent& ent )
	{
		if ( Filter( ent ) ) return;
		if ( ent.d_type == DT_DIR )
			directories.push_back( ent.d_name );
		else
			if ( ent.d_type == DT_REG )
				push_back( ent.d_name );
	}

	inline Directory::operator bool ()
	{
		Directory& D( *this );
		struct dirent *pDirent( NULL ); 
		DIR* pDir( opendir (where.c_str() ) );
		if ( ! pDir ) throw string("Directory:" ) + where;
		pDirent = readdir(pDir);
		while ((pDirent = readdir(pDir)) != NULL) D+=*pDirent;
		closedir (pDir);

		for ( stringvector::const_iterator dit=directories.begin();dit!=directories.end();dit++)
		{
			const string name( *dit );
			if ( name == "." ) continue;
			if ( name == ".." ) continue;
			const string subwhere( where + string( "/" ) + name );
			Directory& sub( NewSub( subwhere, recurse ) );
			if ( ! sub ) return false;
		}

		return true;
	}

	inline bool FileExists( const string filename )
	{
		struct stat sb;
		return ( stat( filename.c_str(), &sb ) == 0 );
	}



	inline size_t FileSize(const string filename)
	{
		string pathname("text/");
		pathname+=filename;
		struct stat sb;
		memset(&sb, 0, sizeof(sb));
		if (stat(pathname.c_str(), &sb)==0) 
			return sb.st_size;
		else
			return 0;
	}

	inline string LoadFile(const string& filename, int maxlines=-1, int maxlinelen=-1 )
	{
		if ( ! FileExists( filename ) ) return "";
		stringstream ss;
		ifstream in(filename.c_str());
		string line;
		if (in.fail()) return "";
		while (!in.eof())
		{
			if ( maxlines != -1 )
				if ( !(maxlines--) ) throw string("file too long:" + filename );
			getline(in, line); 
			if ( maxlinelen !=-1 )
				if ( (int) line.size() > maxlinelen ) throw string("Line too long in file:" ) + filename + "->" + line ;
			if ( in.fail() ) throw filename;
			ss << line << endl;
		}
		return ss.str();
	}

	inline void LoadFile(const string& filename, stringstream& ss, int maxlines=-1, int maxlinelen=-1)
	{
		if ( ! FileExists( filename ) ) return ;
		ifstream in(filename.c_str());
		string line;
		if (in.fail()) return;
		while (!in.eof()) 
		{
			if ( maxlines != -1 )
				if ( !(maxlines--) ) throw string("file too long:" + filename );
			getline(in, line); 
			if ( maxlinelen !=-1 )
				if ( (int) line.size() > maxlinelen ) throw string("Line too long in file:" ) + filename + "->" + line ;
			if ( in.fail() ) throw filename;
			ss << line << endl;
		}
	}
};
#endif //  KRUNCHER_DIRECTORY_H

