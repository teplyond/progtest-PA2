#ifndef __PROGTEST__
// polynomy.cpp : Defines the entry point for the console application.
//

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
//#include <fftw.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <memory>
#endif /* C++ 11 */
using namespace std;
#endif /* __PROGTEST__ */

class CPolynomial
{
public:
	CPolynomial();			// default 

	CPolynomial operator + (const CPolynomial & pol) const;			// operator +
	CPolynomial operator - (const CPolynomial & pol) const;			// operatnr -
	CPolynomial operator * (const CPolynomial & pol) const;		    // operator * (polynomial, double)
	CPolynomial operator * (double nasobitel) const;
	bool operator == (const CPolynomial & pol) const;			// operator ==
	bool operator != (const CPolynomial & pol) const;			// operator !=
	double operator[](int index) const;							// operator [] a[5] = 2;
	double & operator[] (int index) ;						    // operator [] b = a[5]; //mozna bez &
	double operator() (double bod) const;						// operator ()
	int Degree() const;										 	// metoda zjistí stupeò polynomu,  // unsigned ?

	friend ostream& operator << (ostream& out, const CPolynomial & pol);
	friend CPolynomial karatsuba(const CPolynomial & pol1, const CPolynomial & pol2);
		
private:
	//velikost 
	int m_velikost;
	vector<double> m_polynom;

	int m_stupen;
	
	void zmenVelikost(int stupen);
};

// -------------------------------------------------------------------------------------


CPolynomial::CPolynomial()
{
	m_stupen = -1;
	m_polynom.resize(2);	
	m_velikost = 2;
}

void CPolynomial::zmenVelikost(int val)
{
	//pouze nafukovani
	if (this->m_velikost <= val) {
		this->m_velikost = (2 * val + 1);
		this->m_polynom.resize(this->m_velikost);
		this->m_stupen = val;
	}
	if (this->m_stupen < val) this->m_stupen = val;
}


CPolynomial CPolynomial::operator+(const CPolynomial & pol) const
{
	 CPolynomial vys;
	 int i;
	 int  maximum = (pol.m_stupen > this->m_stupen) ? pol.m_stupen : this->m_stupen;
	 vys.zmenVelikost(maximum);
	
	
	 // musim se podivat ktery je mensi a podle toho iteruju, potom uz jen zkopiruju ten druhy
	 if (pol.m_stupen < this->m_stupen) {
		 for (i = 0; i <= pol.m_stupen; i++) {
			 vys.m_polynom[i] = pol.m_polynom[i] + this->m_polynom[i];
		 }
		 while( i <= this->m_stupen) {
			 vys.m_polynom[i] = this->m_polynom[i];
			 i++;
		 }
	 }
	 else {
		 for (i = 0; i <= this->m_stupen; i++) {
			 vys.m_polynom[i] = pol.m_polynom[i] + this->m_polynom[i];
		 }
		 while( i <= pol.m_stupen) {
			 vys.m_polynom[i] = pol.m_polynom[i];
			 i++;
		 }
	 }
	return vys;
}

CPolynomial CPolynomial::operator-(const CPolynomial & pol) const
{
	CPolynomial vys;
	int i;
	int  maximum = (pol.m_stupen > this->m_stupen) ? pol.m_stupen : this->m_stupen;
	vys.zmenVelikost(maximum);


	// musim se podivat ktery je mensi a podle toho iteruju, potom uz jen zkopiruju ten druhy
	if (pol.m_stupen < this->m_stupen) {
		for (i = 0; i <= pol.m_stupen; i++) {
			vys.m_polynom[i] = this->m_polynom[i] - pol.m_polynom[i];
		}
		while( i <= this->m_stupen) {
			vys.m_polynom[i] = this->m_polynom[i];
			i++;
		}
	}
	else {
		for (i = 0; i <= this->m_stupen; i++) {
			vys.m_polynom[i] =  this->m_polynom[i] - pol.m_polynom[i];
		}
		while( i <= pol.m_stupen) {
			vys.m_polynom[i] = - pol.m_polynom[i];
			i++;
		}
	}
	return vys;
}

CPolynomial CPolynomial::operator*(const CPolynomial & pol) const
{	
	CPolynomial vys;
	int vel = this->m_stupen + pol.m_stupen;
	vys.zmenVelikost(vel);

	for (int i = 0; i <= this->m_stupen; i++)
		for (int j = 0; j <= pol.m_stupen; j++)
			vys[i + j] = vys[i + j] + this->m_polynom[i] * pol.m_polynom[j];

	return vys;
}


CPolynomial CPolynomial::operator*(double nasobitel) const
{
	CPolynomial vys;
	vys.zmenVelikost(this->m_stupen);

	for (int i = 0; i <= vys.m_stupen; i++) {
		vys.m_polynom[i] = this->m_polynom[i] * nasobitel;
	}

	return vys;
}

bool CPolynomial::operator==(const CPolynomial & pol) const
{
	for (int i = 0; i < pol.m_stupen; i++) {
			if (pol.m_polynom[i] != this->m_polynom[i])
				return false;
	}
	return true;
}

bool CPolynomial::operator!=(const CPolynomial & pol) const
{
	return !(operator==(pol)); 
}



double CPolynomial::operator[](int index) const
{
	if ((index  < 0) || (index > this->m_velikost)) return 0; 
	return this->m_polynom[index];
}

double & CPolynomial::operator[](int index) 					
{
	if (index  > 0) zmenVelikost(index);
	return this->m_polynom[index];
}

double CPolynomial::operator()(double bod) const 
{
	double hodnota = 0;
	
	//hornerovo schema
	for (int i = this->m_stupen; i >= 0; i--) {
		hodnota = hodnota * bod + this->m_polynom[i];
	}
	return hodnota;
}

// --------------------------

int CPolynomial::Degree() const
{
	for (int i = this->m_stupen; i >= 0; i--) {
		if (this->m_polynom[i] != 0) {
															//this->m_stupen = i;	// funguje to ?
			return i;
		}
	}

	return 0;
}



ostream& operator<<(ostream& out, const CPolynomial& pol) {
	bool flag = false;

	if (pol.m_stupen == -1) {
		out << "0";
		return out;
	}

	for (int i = pol.m_stupen; i >= 0; i--) {
		if ((flag == false) && (i == 0)) {
			if (pol.m_polynom[i] > 0) {
				out  << pol.m_polynom[i];							// "" ___ " + 6"
			}
			else {
				out << "- " << abs(pol.m_polynom[i]);			// "" ___ " - 6"
			}
			continue;
		}
		if ((flag == true) && (i == 0) && (pol.m_polynom[i] != 0)) {
			if (pol.m_polynom[i] > 0) {
				out << " + " << pol.m_polynom[i];				// "2*x^8" ___ " + 6"
			}
			else {
				out << " - " << abs(pol.m_polynom[i]);			// "2*x^8" ___ " - 6"
			}
			continue;
		}
		if (pol.m_polynom[i] == 0.0) continue;					// "0x^2" nevypisuju
		

		 //-----------------   pokud jsem na zacatku   ------------------
		if (flag == false) {
			flag = true;
			if ((pol.m_polynom[i] == 1.0) || (pol.m_polynom[i] == -1.0)) {
				if (pol.m_polynom[i] == 1.0) 
					out << "x^" << i;			// "" ___ "x^5"
				else 
					out << "- x^" << i;											// "" ___ "- x^5"		
			}	
			else {
				//pro cislo na zacatku
				if (pol.m_polynom[i] > 0) 
					out << pol.m_polynom[i] << "*x^" << i;						// "" ___ "2*x^5"
				else 
					out << "- " << abs(pol.m_polynom[i]) << "*x^" << i;			// "" ___ "- 2*x^5"		
			}
		}
		//-----------------   pokud nejsem na zacatku   ------------------
		else {
			if ((pol.m_polynom[i] == 1.0) || (pol.m_polynom[i] == -1.0)) {
				if (pol.m_polynom[i] == -1.0) 
					out << " - x^" << i;			// "- x^5" ___ " - x^4"
				else 
					out << " + x^" << i;			// "- x^5" ___ " + x^4"
			}
			else {
				if (pol.m_polynom[i] > 0) 
					out << " + " << pol.m_polynom[i] << "*x^" << i;			// "2*x^8" ___ " + 7*x^6"
				else 
					out << " - " << abs(pol.m_polynom[i]) << "*x^" << i;	// "2*x^8" ___ " - 7*x^6"
			}
		}
	//konec cyklu
	}

	return out;
}



// ---------------------------------------------------------------------------------------
#ifndef __PROGTEST__
bool smallDiff(double a, double b)
{
	return ( a == b );
}

bool dumpMatch(const CPolynomial & x, const vector<double> & ref) // 
{
	for (int i = 0; i < ref.size(); i++) {
		if (ref[i] != x[i]) return false;
	}

	return true;
}


int main(void)
{
	CPolynomial a, b, c, d;
	ostringstream out;

	a[0] = -10;
	a[1] = 3.5;
	a[3] = 1;
	cout << "A je toto "<< a << endl;
	/*-------------------------------------------------------*/
	cout << "Test vypoctu hodnoty v bode?" << endl;
	cout << (smallDiff(a(2), 5)) << endl;
	out.str("");
	out << a;
	cout << "Test vypisu retezce x^3 + 3.5*x^1 - 10?" << endl;
	cout << (out.str() == "x^3 + 3.5*x^1 - 10") << endl;
	cout << "Test nasobeni skalarem?" << endl;
	a = a * -2;
	//cout << (a.Degree() == 3 && dumpMatch(a, vector<double>{ 20.0, -7.0, -0.0, -2.0 })) << endl;
	out.str("");
	cout << "Test vypisu retezce 2*x^3 - 7*x^1 + 20?" << endl;
	out << a;
	cout << (out.str() == "- 2*x^3 - 7*x^1 + 20") << endl;
	out.str("");
	cout << "Test nuly" << endl;
	out << b;
	cout << (out.str() == "0") << endl;
	cout << "Test vypis 1 prvku?" << endl;
	b[5] = -1;
	out.str("");
	out << b;
	cout << (out.str() == "- x^5") << endl;

	/*-------------------------------------------------------*/
	cout << "Test scitani: 1. test hodnot?" << endl;
	c = a + b;
//	cout <<  (c.Degree() == 5 && dumpMatch(c, vector<double>{ 20.0, -7.0, 0.0, -2.0, 0.0, -1.0 })) << endl;
	out.str("");
	out << c;
	cout << "Test odecitani: 1. test hodnot, 2. test vypis?" << endl;
	c = a - b;
//	cout << (c.Degree() == 5 && dumpMatch(c, vector<double>{ 20.0, -7.0, -0.0, -2.0, -0.0, 1.0 })) << endl;
	out.str("");
	out << c;
	cout <<(out.str() == "x^5 - 2*x^3 - 7*x^1 + 20") << endl;
	cout << "Test nasobeni: 1. test hodnot, 2. test vypis?" << endl;
	c = a * b;
	cout << "vys" << c << endl;
//	cout <<(c.Degree() == 8 && dumpMatch(c, vector<double>{ -0.0, -0.0, 0.0, -0.0, 0.0, -20.0, 7.0, -0.0, 2.0 })) << endl;
	out.str("");
	out << c;
	cout << (out.str() == "2*x^8 + 7*x^6 - 20*x^5") << endl;


	/*-------------------------------------------------------*/
	cout << "Test na operator != ?" << endl;
	cout << (a != b) << endl;
	b[5] = 0;
	cout << "Test na operator == ?" << endl;
	cout << (!(a == b)) << endl;
	
	/*-------------------------------------------------------*/
	cout << "Test vynasobeni polynomu nulou 1. test na vypis 2. test na operator ==  ?" << endl;
	a = a * 0;
//	cout << (a.Degree() == 0	&& dumpMatch(a, vector<double>{ 0.0 })) << endl;
	cout << (a == b) << endl;

	/*-------------------------------------------------------*/
	cout << "Test nahonymi vstpupy?" << endl;
	d[3] = 12; d[7] = 1; d[7] = 0;
	cout << (d.Degree() == 3) << endl;
	a[0] = 5;
	a = a * a;
	//cout << (a.Degree() == 0 && dumpMatch(a, vector<double>{ 25.0 })) << endl;

	a[0] = 0;
	a[325] = 1;
	a = a*a;
	out.str("");
	cout << (out.str() == "x^650") << endl;

	/*-------------------------------------------------------*/
	out << "Test navic -  test na divny mins " << endl;
	CPolynomial g, j, z;
	g[3] = 1;
	j[1] = 1;
	j[0] = 1;
	cout << g << endl;
	cout << j << endl;
	z = g - j;
	cout << z << endl;
	return 0;
}
#endif /* __PROGTEST__ */