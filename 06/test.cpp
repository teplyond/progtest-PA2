#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <exception>
#include <typeinfo>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */
using namespace std;


class CSyntaxException : public exception
{
  public: 
    //---------------------------------------------------------------------------------------------
                              CSyntaxException             ( const string    & desc )
                              : m_Desc ( desc )
    {
    }
    //---------------------------------------------------------------------------------------------
    virtual const char      * what                         ( void ) const noexcept
    {
      return m_Desc . c_str ();
    }
    //---------------------------------------------------------------------------------------------
  private:
    string                    m_Desc;    
};
#endif /* __PROGTEST__ */   

class CType {
public:
	virtual ~CType() {};
	virtual CType * Clone(void) const = 0;
	virtual size_t Sizeof(void) const = 0;
	virtual bool operator!=(const CType & cdt) const { 
		return !( Equal(cdt) ); 
	};
	virtual bool operator==(const CType & cdt) const {
		return Equal(cdt);  
	};
	virtual bool Equal(const CType & value) const = 0;
	virtual void print(ostream& os) const  = 0;
	friend ostream& operator<<(ostream& os, const CType& cdt) {
		cdt.print(os); return os; 
	}
};
 
class CPrimitiveType : public  CType {
public:
	virtual ~CPrimitiveType() {};
	//virtual CType * Clone(void) const = 0;
};
//--------------------------------------------------------


class CDataTypeInt : public CPrimitiveType
{
public:
	virtual ~CDataTypeInt() {};
	virtual size_t Sizeof(void) const { 
		return 4;
	}
	virtual bool Equal(const CType & value) const {
		CDataTypeInt const * tmp = dynamic_cast<CDataTypeInt const *>(& value);
		return ( tmp != NULL );
	}
	virtual void print(ostream& os) const { 
		os << "  int"; 
	}
	virtual CPrimitiveType * Clone(void) const {
		return new CDataTypeInt(*this);
	}
};

class CDataTypeDouble : public CPrimitiveType
{
public:		
	virtual ~CDataTypeDouble() {};
	virtual size_t Sizeof(void) const {
		return 8; 
	}
	virtual bool Equal(const CType & value) const {
		CDataTypeDouble const * tmp = dynamic_cast<CDataTypeDouble const *>(&value);
		return (tmp != NULL);
	}									
	virtual void print(ostream& os) const { 
		os << "  double"; 
	}
	virtual CPrimitiveType * Clone(void) const {
		return new CDataTypeDouble(*this);
	}
};

class CDataTypeEnum : public CPrimitiveType
{
public:

	virtual ~CDataTypeEnum() {};
	virtual size_t Sizeof(void) const { 
		return 4;
	}

	virtual bool Equal(const CType & value) const {
		CDataTypeEnum const * tmp = dynamic_cast<CDataTypeEnum const *>(&value);
		int flag = 1;

		if (tmp == NULL) return false;
		if (tmp->m_valueEnum.size() != m_valueEnum.size()) return false;

		auto itakt = m_valueEnum.begin();
		for (auto it = tmp->m_valueEnum.begin(); it != tmp->m_valueEnum.end(); ++it) {
			if (*itakt != *it) flag = 0;

			++itakt;
		}


		if(flag) return true;
		else return false;
	}

	virtual void print(ostream& os) const { 
		os << "  enum\n" << "  {\n";
		for (auto it =  m_valueEnum.begin(); it !=  m_valueEnum.end(); ++it) {
			os << "    " << *it;
			if ((it + 1) == m_valueEnum.end())  os << "\n";
			else  os << ",\n";
		}
		os << "  }"; 
	}

	virtual CPrimitiveType * Clone(void) const {
		return new CDataTypeEnum(*this);
	}

	CDataTypeEnum& Add(const string & item) {
		auto itFind = find(m_valueEnum.begin(), m_valueEnum.end(), item);
		if (itFind != m_valueEnum.end()) throw CSyntaxException("Duplicate enum value: " + item);
		m_valueEnum.push_back(item);
		return *this;
	}

private:
	vector<string> m_valueEnum; 
};


class CDataTypeStruct : public  CType
{
public:
	CDataTypeStruct() {};

	CDataTypeStruct(const CDataTypeStruct & copy) {
		for (auto it : copy.m_valueStruct) 
			m_valueStruct.push_back(pair<CType*, string>(it.first->Clone() , it.second));
		
	}

	~CDataTypeStruct() {
		for (auto it : m_valueStruct)
			delete(it.first);
	}

	virtual CType * Clone(void) const { return new CDataTypeStruct; };//

	CDataTypeStruct& AddField(const string& name, const CPrimitiveType& type) {

		for (auto it = m_valueStruct.begin(); it != m_valueStruct.end(); ++it) {
			if (it->second == name) throw CSyntaxException("Duplicate field: " + name);
		}
		
		m_valueStruct.push_back(pair<CType*, string>(type.Clone(), name));
		return *this;
	} 

	CType& Field(const string& name) const {
		auto it = m_valueStruct.begin();

		while (it != m_valueStruct.end()) {
			if (it->second == name) break;
			++it;
		}

		if (it == m_valueStruct.end()) throw CSyntaxException("Unknown field: " + name);

		return *it->first;
	}

	virtual size_t Sizeof(void) const {
		size_t sizeS = 0;
		for (auto it = m_valueStruct.begin(); it != m_valueStruct.end(); ++it) {
			sizeS += it->first->Sizeof();
		}
		return sizeS;
	}

	virtual bool Equal(const CType & value) const {
		CDataTypeStruct const * tmp = dynamic_cast<CDataTypeStruct const *>(&value);

		if (tmp == NULL) return false;
		if (tmp->m_valueStruct.size() != m_valueStruct.size()) return false;
		int flag = 1;

		auto itakt = m_valueStruct.begin();
		for (auto it = tmp->m_valueStruct.begin(); it != tmp->m_valueStruct.end(); ++it) {
			if ((*itakt->first) != (*it->first)) flag = 0;

			++itakt;
		}

		if (flag) return true;
		else return false;
	}

	virtual void print(ostream& os) const {
		os << "struct\n" << "{\n";
		for (auto it = m_valueStruct.begin(); it != m_valueStruct.end(); ++it) {
			os << *it->first << " " << it->second << ";\n";
		}
		os << "}";
	}

private:
	vector<pair<CType*, string> > m_valueStruct;
};
//--------------------------------------------------------

#ifndef __PROGTEST__
//-------------------------------------------------------------------------------------------------
static bool        whitespaceMatch                         ( const string    & out,
                                                             const string    & ref )
{
  const char * o = out . c_str ();
  const char * r = ref . c_str ();
  
  while ( 1 )
  {
    while ( isspace ( *o )) o ++;
    while ( isspace ( *r )) r ++;
    if ( ! *r && ! *o ) return true;
    if ( ! *r || ! *o || *r++ != *o++ ) return false;
  }
}
//-------------------------------------------------------------------------------------------------
template <typename _T>
static bool        whitespaceMatch                         ( const _T        & x,
                                                             const string    & ref )
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch ( oss . str (), ref );
}
//-------------------------------------------------------------------------------------------------
int                main                                    ( void )
{
  CDataTypeStruct  a = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct b = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "READY" ) ).
                        AddField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct c =  CDataTypeStruct () .
                        AddField ( "m_First", CDataTypeInt () ) .
                        AddField ( "m_Second", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Ratio", CDataTypeInt () );
  
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "}") );

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . Field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  b . AddField ( "m_Other", CDataTypeDouble ());

  a . AddField ( "m_Sum", CDataTypeInt ());

  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  int m_Sum;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  double m_Other;\n"
    "}") );

  assert ( a . Sizeof () == 20 );
  assert ( b . Sizeof () == 24 );
  try
  {
    a . AddField ( "m_Status", CDataTypeInt () );
    assert ( "AddField: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) == "Duplicate field: m_Status" );
  }

  try
  {
    cout << a . Field ( "m_Fail" ) << endl;
    assert ( "Field: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) == "Unknown field: m_Fail" );
  }
  
  try
  {
    CDataTypeEnum en;
    en . Add ( "FIRST" ) .
         Add ( "SECOND" ) .
         Add ( "FIRST" );
    assert ( "Add: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) ==  "Duplicate enum value: FIRST" );
  }
  return 0;
}
#endif /* __PROGTEST__ */   