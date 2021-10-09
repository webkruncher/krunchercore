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
		Directory() {}
		Directory( const string& _where ) : where( _where ) {}
		void operator=(const string& _where ) const { where=_where; }
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
		virtual operator bool ()
		{
			struct dirent *pDirent( NULL ); 
			DIR* pDir( opendir (where.c_str() ) );
			if ( ! pDir ) throw string("Directory:" ) + where;
			pDirent = readdir(pDir);
			while ((pDirent = readdir(pDir)) != NULL)
				if ( pDirent->d_type == DT_DIR )
				{
					directories.push_back( pDirent->d_name );
				} else {
					if ( pDirent->d_type == DT_REG )
					{
						push_back( pDirent->d_name );
					}
				}

			closedir (pDir);

			return true;
		}
		const stringvector& Directories() { return directories; }
		protected:
		mutable string where;
		stringvector directories;
		private:
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
		//cout << pathname << "^" << endl;
		if (stat(pathname.c_str(), &sb)==0) 
		{
			//stringstream ssout; ssout << fence << "[fsize]" << fence << pathname << fence << sb.st_size << fence; Log(ssout.str());
			return sb.st_size;
		} else {
			//stringstream ssout; ssout << fence << logerror << "[fsize]" << fence << pathname << fence << sb.st_size << fence; Log(ssout.str());
			return 0;
		}
	}

	inline string LoadFile(const string& filename )
	{
		stringstream ss;
		ifstream in(filename.c_str());
		string line;
		if (!in.fail())
		while (!in.eof()) { getline(in, line); ss << line << endl;}
		return ss.str();
		//cout << "\033[32m" << ">" << "\033[0m"; cout.flush();
	}

	inline void LoadFile(const string& filename, stringstream& ss)
	{
		ifstream in(filename.c_str());
		string line;
		if (!in.fail())
		while (!in.eof()) { getline(in, line); ss << line << endl;}
		//cout << "\033[32m" << ">" << "\033[0m"; cout.flush();
	}
};
#endif //  KRUNCHER_DIRECTORY_H

