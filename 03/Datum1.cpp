#ifndef __PROGTEST__
// Datum.cpp : Defines the entry point for the console application.
//
#include <cstdio>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

class InvalidDateException {};

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your 
// own working manipulator.
ios_base & dummy_date_format_manipulator(ios_base & x)
{
	return x;
}

ios_base & (*(date_format(const char * fmt))) (ios_base & x)
{
	return dummy_date_format_manipulator;
}
//=================================================================================================
class CDate
{
public:
	CDate(); 
	CDate(int y, int m, int d); // constructor ( y,m,d )
	
	CDate operator + (int cislo) const;				// operator + int
	CDate operator - (int cislo) const;				// operator - int
	int operator - ( CDate & datum) const;		// operator - CDate				//asi blbe (const CDate & datum) ale const byt nemuze
	CDate& operator ++();							// operators ++, --, both prefix and postfix
	CDate operator ++(int);							// operators ++, --, both prefix and postfix
	CDate& operator --();							// operators ++, --, both prefix and postfix
	CDate operator --(int);							// operators ++, --, both prefix and postfix
	bool operator ==(const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	bool operator !=(const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	bool operator <=(const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	bool operator < (const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	bool operator >=(const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	bool operator > (const CDate & datum) const;	// operators ==, !=, <=, <, >=, >
	
	friend ostream& operator << (ostream& out, const CDate & datum);		// operators << and >>
	friend istream& operator >> (istream& in, CDate & datum);				// operators << and >>
private:

	tm otisk;
};

//-----------------------------------------------------------------------------------------------------
CDate::CDate()
{
	otisk.tm_sec = 30;
	otisk.tm_min = 30;
	otisk.tm_hour = 11;
	otisk.tm_isdst = 0;
}

CDate::CDate(int y, int m, int d)
{
	otisk.tm_sec = 30;
	otisk.tm_min = 30;
	otisk.tm_hour = 11;
	otisk.tm_year =  (y - 1900);		//y == rok   ! pozor (od 1900) tj. 2000 bude 100
	otisk.tm_mday = d;					//d == den
	otisk.tm_mon = (m - 1);				//m == mesic ! pozor  0 - 11
	
	otisk.tm_isdst = 0;					// neco divnyho (flag letniho casu )

	//test
	if (mktime(&otisk) == -1) throw InvalidDateException();
	if ((otisk.tm_mday != d ) || (otisk.tm_year != (y - 1900)) || otisk.tm_mon != (m - 1)) throw InvalidDateException();
}

// ok ?
CDate CDate::operator+(int cislo) const
{
	CDate vysledek;

	vysledek.otisk = this->otisk;
	vysledek.otisk.tm_mday += cislo;

	if (mktime(&vysledek.otisk) == -1) throw InvalidDateException();

	return vysledek;
}
//ok  ?
CDate CDate::operator-(int cislo) const
{
	CDate vysledek;

	vysledek.otisk = this->otisk;
	vysledek.otisk.tm_mday -= cislo;

	if (mktime(&vysledek.otisk) == -1) throw InvalidDateException();

	return vysledek;
}
//asi pristupuju blbe
int CDate::operator-(  CDate & datum) const  
{
	CDate vysledek;
	vysledek.otisk = this->otisk; 

	int pocetDni = (int) (difftime(mktime(&vysledek.otisk), mktime(&datum.otisk)) / 86400);
	return pocetDni;
}
//--a
CDate& CDate::operator++() 
{
	this->otisk.tm_mday += 1;
	if (mktime(&this->otisk) == -1) throw InvalidDateException();

	return *this;
}
//c++
CDate CDate::operator++(int) 
{
	CDate vysledek= *this;

	++(*this);
	return vysledek;
}
//--c
CDate& CDate::operator--() 
{
	this->otisk.tm_mday -= 1;
	if (mktime(&this->otisk) == -1) throw InvalidDateException();

	return *this;
}
//c--
CDate CDate::operator--(int) 
{
	CDate vysledek = *this;

	--(*this);
	return vysledek;
}



bool CDate::operator==(const CDate & datum) const
{
	if (this->otisk.tm_mday == datum.otisk.tm_mday)
		if (this->otisk.tm_mon == datum.otisk.tm_mon)
			if (this->otisk.tm_year == datum.otisk.tm_year)
				return true;

	return false;
}

bool CDate::operator!=(const CDate & datum) const
{
	return !(operator==(datum));
}

bool CDate::operator<=(const CDate & datum) const
{
	if (operator<(datum) || operator==(datum)) return true;

	return false;
}

bool CDate::operator<(const CDate & datum) const
{
	if (this->otisk.tm_year < datum.otisk.tm_year) return true;
	if (this->otisk.tm_mon < datum.otisk.tm_mon) return true;
	if (this->otisk.tm_mday < datum.otisk.tm_mday) return true;

	return false;
}

bool CDate::operator>=(const CDate & datum) const
{
	if (operator>(datum) || operator==(datum)) return true;

	return false;
}

bool CDate::operator>(const CDate & datum) const
{
	if (this->otisk.tm_year > datum.otisk.tm_year) return true;
	if (this->otisk.tm_mon > datum.otisk.tm_mon) return true;
	if (this->otisk.tm_mday > datum.otisk.tm_mday) return true;
	return false;
}




ostream& operator << (ostream& out, const CDate & datum) {
	out << setw(4) << setfill('0') << (datum.otisk.tm_year + 1900);
	out << "-" << setw(2) << (datum.otisk.tm_mon + 1) << "-" << setw(2) << datum.otisk.tm_mday;
	return out;
}

istream& operator >> (istream& in, CDate & datum) {
	
	CDate nacti;
	int y, m, d;
	char sep1;
	char sep2;
	in >> y >> sep1 >> m >> sep2 >> d;

	if ( !(in.eof()) || (sep1 != '-') || (sep2 != '-')) {
		in.setstate(ios::failbit);
	}
	else {
		nacti.otisk.tm_year = (y - 1900);
		nacti.otisk.tm_mon = (m - 1);
		nacti.otisk.tm_mday = d;


		if ((mktime(&nacti.otisk) == -1) || (nacti.otisk.tm_mday != d) || (nacti.otisk.tm_year != (y - 1900)) || (nacti.otisk.tm_mon != (m - 1))) {
		in.setstate(ios::failbit);
		}
		else {
			datum = nacti;
		}
	}
	return in;

}

//-----------------------------------------------------------------------------------------------------
 #ifndef __PROGTEST__
int main(){

	ostringstream oss;
	istringstream iss;

	CDate a(2000, 1, 2);
	CDate b(2010, 2, 3);
	CDate c(2004, 2, 10);

	//----------------------------------------------------
	cout << "Testy zakladni" << endl;
	oss.str("");
	oss << a;
	cout << (oss.str() == "2000-01-02") << endl;
	oss.str("");
	oss << b;
	cout << (oss.str() == "2010-02-03") << endl;
	oss.str("");
	oss << c;
	cout << (oss.str() == "2004-02-10") << endl;

	//----------------------------------------------------
	cout << "Testy  1. dat + cislo, 2. dat - cislo " << endl;
	a = a + 1500;
	oss.str("");
	oss << a;
	cout << (oss.str() == "2004-02-10") << endl;
	b = b - 2000;
	oss.str("");
	oss << b;
	cout << (oss.str() == "2004-08-13") << endl;
	a = a + -1500;
	oss.str("");
	oss << a;
	cout << (oss.str() == "2000-01-02") << endl;
	b = b - -2000;
	oss.str("");
	oss << b;
	cout << (oss.str() == "2010-02-03") << endl;
	a = a + 1500;
	b = b - 2000;

	//----------------------------------------------------
	cout << "Test:  dat - dat  " << endl;
	cout << ((b - a) == 185) << endl;;

	//----------------------------------------------------
	cout << "Test: == ; !=  " << endl;
	cout << ((b == a) == false) << endl;
	cout << ((b != a) == true) << endl;

	//----------------------------------------------------
	cout << "Test: <= ; < ; >= ; > " << endl;
	cout << ((b <= a) == false) << endl;
	cout << ((b < a) == false) << endl;
	cout << ((b >= a) == true) << endl;
	cout << ((b > a) == true) << endl;
	
	//----------------------------------------------------
	cout << "Dasli testy: <= ; < ; >= ; > " << endl;
	cout << ((c == a) == true) << endl;
	cout << ((c != a) == false) << endl;
	cout << ((c <= a) == true) << endl;
	cout << ((c < a) == false) << endl;
	cout << ((c >= a) == true) << endl;
	cout << ((c > a) == false) << endl;

	//----------------------------------------------------
	cout << "Dalsi testy: ++c, --c, c++ , c--" << endl;
	a = ++c;
	oss.str("");
	oss << a << " " << c;
	cout << (oss.str() == "2004-02-11 2004-02-11") << endl;
	a = --c;
	oss.str("");
	oss << a << " " << c;
	cout << (oss.str() == "2004-02-10 2004-02-10") << endl;
	a = c++;	
	oss.str("");
	oss << a << " " << c;
	cout << (oss.str() == "2004-02-10 2004-02-11") << endl;
		a = c--;
	oss.str("");
	oss << a << " " << c;
	cout << (oss.str() == "2004-02-11 2004-02-10") << endl;

	//----------------------------------------------------
	cout << "Testy vstupni stream - spravny" << endl;
	iss.clear();
	iss.str("2015-09-03");
	cout << "nacti " << !(!(iss >> a)) << endl;
	oss.str("");
	oss << a;
	cout << (oss.str() == "2015-09-03") << endl;
	a = a + 70;
	oss.str("");
	oss << a;
	cout << (oss.str() == "2015-11-12") << endl;

	//----------------------------------------------------
	cout << "Test chybne datum" << endl;
	CDate d(2000, 1, 1);
	try
	{
		CDate e(2000, 32, 1);
		assert("No exception thrown!" == NULL); //  ok   ????
	}
	catch (...)
	{
	}

	//----------------------------------------------------
	cout << "Testy vstupni stream - spatny" << endl;
	iss.clear();
	iss.str("2000-12-33");
	cout << "nacti " << (!(iss >> d)) << endl;
	oss.str("");
	oss << d;
	cout << (oss.str() == "2000-01-01") << endl;
	iss.clear();

	iss.str("2000-11-31");
	cout << "nacti " << (!(iss >> d)) << endl;
	oss.str("");
	oss << d;
	cout << (oss.str() == "2000-01-01") << endl;
	iss.clear();

	iss.str("2000-02-29");
	cout << "nacti " << !(!(iss >> d)) << endl;
	oss.str("");
	oss << d;
	cout << (oss.str() == "2000-02-29") << endl;
	iss.clear();

	iss.str("2001-02-29");
	cout << "nacti " << (!(iss >> d)) << endl;
	oss.str("");
	oss << d;
	cout << (oss.str() == "2000-02-29") << endl;

	return 0;
}
   #endif /* __PROGTEST__ */

