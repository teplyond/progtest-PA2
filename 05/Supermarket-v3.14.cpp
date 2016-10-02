// Supermarket.cpp : Defines the entry point for the console application.
//

 
#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */
using namespace std;
#endif /* __PROGTEST__ */                         

class CDate
{
public:
	CDate() {};
	CDate(int y, int m, int d);			// CDate ( y, m, d )
	CDate(const CDate & datum);
	CDate& operator=(const CDate & datum);

	bool operator == (const CDate & pol) const;
	bool operator !=(const CDate & datum) const;
	bool operator <=(const CDate & datum) const;
	bool operator < (const CDate & datum) const;
	bool operator >=(const CDate & datum) const;
	bool operator > (const CDate & datum) const;
	friend ostream& operator << (ostream& os, const CDate & datum) {
		os << datum.year << "-" << datum.month << "-" << datum.day;
		return os;
	}
private:
	int year;
	int month;
	int day;
};
//----------------------------------------------------------------------------------------------
class CSupermarket
{

public:
	CSupermarket() {};																		// default constructor
	CSupermarket& Store(const string & name, const CDate& expiryDate, int count);			// Store   ( name, expiryDate, count )
	void Sell(list<pair<string, int> > & shoppingList);										// Sell    ( shoppingList )
	list<pair<string, int> > Expired(const CDate& expiryDate);								// Expired ( date )
	void print();

private:
	struct Vlastnosti {
		CDate expirace;
		int pocetKusu;
		Vlastnosti(const CDate & datum, int pocet) : expirace(datum), pocetKusu(pocet) {};
		bool operator < (const Vlastnosti & datum) const {
			return (this->expirace < datum.expirace) ;
		}
	};
	struct Transakce {
		string nazev;
		int odeber;
		int zbyva;
		Transakce(const string & polozka, int vydej, int naSklade) : nazev(polozka), odeber(vydej), zbyva(naSklade) {};
		bool operator < (const Transakce & tr) const {
			return (this->nazev < tr.nazev);
		}
	};
	map<string, pair<int, set<Vlastnosti> > > sklad;								// <nazev, CelkemKs, set<Expirace, pocetkusu> > unordered_mapu
	multimap<string, string> chybnyNazev;
	multimap<Transakce, map<string, pair<int, set<Vlastnosti> > >::iterator > vyskadneni;
 
	string NajdiSChybou(const string & name, int count);
	void Najdi(const string & name, int count);
	void SmazChyby(const string & name);
};
//------------------------------------------------------------------------------------------
CDate::CDate(int y, int m, int d)
{
	year = y;
	month = m;
	day = d;
}
CDate::CDate(const CDate & datum)
{
	this->day = datum.day;
	this->month = datum.month;
	this->year = datum.year;
}
CDate & CDate::operator=(const CDate & datum)
{
	if (this == &datum) return *this;
	this->day = datum.day;
	this->month = datum.month;
	this->year = datum.year;
	return *this;
}

bool CDate::operator==(const CDate & pol) const
{
	if ((pol.year == this->year) && (pol.month == this->month) && (pol.day == this->day))	return true;
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
	if (this->year < datum.year)
		return true;
	else
		if (this->year > datum.year) return false;


	if (this->month < datum.month)
		return true;
	else
		if (this->month > datum.month) return false;


	if (this->day < datum.day)
		return true;
	else
		if (this->day > datum.day) return false;



	return false;
}
bool CDate::operator>=(const CDate & datum) const
{
	if (operator>(datum) || operator==(datum)) return true;

	return false;
}
bool CDate::operator>(const CDate & datum) const
{
	if (this->year > datum.year)
		return true;
	else
		if (this->year < datum.year) return false;


	if (this->month > datum.month)
		return true;
	else
		if (this->month < datum.month) return false;


	if (this->day > datum.day)
		return true;
	else
		if (this->day < datum.day) return false;



	return false;
}
//------------------------------------------------------------------------------------------

CSupermarket & CSupermarket::Store(const string & name, const CDate & expiryDate, int count)
{
	Vlastnosti v1(expiryDate, count);
 
	map<string, pair<int, set<Vlastnosti> > >::iterator pozice = sklad.find(name);
	
	if (pozice == sklad.end()) {
		set<Vlastnosti> vlozit;
		vlozit.insert(v1);
		sklad.insert(make_pair(name, pair<int, set<Vlastnosti> >(count, vlozit)));

		for (unsigned int i = 0; i < name.length(); ++i) {
			string novy(name);
			novy[i] = '#';
			chybnyNazev.insert(make_pair(novy, name));
		}

	}
	else {
		pozice->second.first += count;

		set<Vlastnosti>::iterator itVlozit = pozice->second.second.find(v1);

		if (itVlozit == pozice->second.second.end()) {
			pozice->second.second.insert(v1);
		}
		else {
			Vlastnosti v2(itVlozit->expirace, itVlozit->pocetKusu + count);
			pozice->second.second.erase(v1);
			pozice->second.second.insert(v2);
			
		}
	}

	return *this;
}

void CSupermarket::Sell(list<pair<string, int>>& shoppingList)
{
	vyskadneni.clear();
	vector<string> tmp;

	for (auto it = shoppingList.begin(); it != shoppingList.end(); ) {
		if (it->second == 0) {
			it = shoppingList.erase(it);
			break;
		}

		tmp.push_back(it->first);
		if (sklad.count(it->first) == 1) {
		//najdi
			Najdi(it->first, it->second );
		} else{
		//najdi s chybou
			it->first = NajdiSChybou(it->first, it->second);
		}
		++it;
	}


	int i = 0;
	for (auto it = shoppingList.begin(); it != shoppingList.end(); ) {
		Transakce t(it->first, 0, 0);
		
		auto itVyskl = vyskadneni.find(t);
		if (itVyskl != vyskadneni.end()) {
			auto itSklad = itVyskl->second;
			int pocetVyskadneni;
			
			if (it->second >= itVyskl->first.odeber) {
				itSklad->second.first -= itVyskl->first.odeber;	// prepsani celkovych poctu kusu

				 pocetVyskadneni = itVyskl->first.odeber;
				 it->second -= pocetVyskadneni;
			}
			else {
				itSklad->second.first -= it->second;
				pocetVyskadneni = it->second;
				it->second = 0;
			}
			int pocet = pocetVyskadneni;

			//odebrani ze skaldu
			for (auto itDatum = itSklad->second.second.begin() ; pocetVyskadneni != 0 ; ) {  
				if (itDatum->pocetKusu <= pocetVyskadneni) {
					pocetVyskadneni -= itDatum->pocetKusu;
					itDatum = itSklad->second.second.erase(itDatum);
					if (itSklad->second.second.size() == 0) {
						SmazChyby(itSklad->first);
						sklad.erase(itSklad);
						break;
					}
										
				}
				else {
					Vlastnosti v(itDatum->expirace, itDatum->pocetKusu - pocetVyskadneni);
					itDatum = itSklad->second.second.erase(itDatum);
					itSklad->second.second.insert(v);
					pocetVyskadneni = 0;
				}
			}
			
			//odebrani z vyskladnujicich polozek
			Transakce t(itVyskl->first.nazev, itVyskl->first.odeber - pocet, itVyskl->first.zbyva);
			vyskadneni.erase(itVyskl);
			if (t.odeber != 0) {
				vyskadneni.insert(make_pair(t, itSklad));
			}
			
			
		}

		//obnoveni nespravneho slova
		it->first = tmp[i];
		i++;

		//smazani z shoppingListu
		if (it->second == 0) {
			it = shoppingList.erase(it);
		}
		else {
			++it;
		}
	}
}

list<pair<string, int>> CSupermarket::Expired(const CDate & expiryDate)
{
	list<pair<string, int>> result;
	map<string, int> tmp;

	for (auto it = sklad.begin(); it != sklad.end(); ++it) {
		for (auto i = it->second.second.begin(); i != it->second.second.end(); ++i) {
			if (i->expirace < expiryDate) {
				auto itTmp = tmp.find(it->first);
				if (itTmp == tmp.end()) {
					tmp.insert(pair<string, int>(it->first, i->pocetKusu));
				}
				else {
					itTmp->second += i->pocetKusu;
				}
			}
			else {
				break;
			}

		}
		
	}

	for (auto it = tmp.begin(); it != tmp.end(); ++it) {
		result.push_back(pair<string, int>(it->first, it->second));
	}

	result.sort([](const pair<string, int> & a, const pair<string, int> & b) -> bool { return  (a.second > b.second); });

	return result;
}

void CSupermarket::print()
{
	for (auto it = sklad.begin(); it != sklad.end(); ++it) {
		cout << "Zbozi: " << it->first << ", Celkem: " << it->second.first <<  endl;
		for (auto i = it->second.second.begin(); i != it->second.second.end(); ++i) {
			cout << "\t\t( " << i->expirace << ", " << i->pocetKusu << " )" << endl;
		}
	
	}
}

string CSupermarket::NajdiSChybou(const string & name, int count)
{
	int shody = 0;
	string jmeno = "";
	for (unsigned int i = 0; i < name.length(); ++i) {
		string novy(name);
		novy[i] = '#';


		if (chybnyNazev.count(novy) == 1) {
			jmeno = novy;
			shody++;
		}
	}
	if (shody == 1) {
		auto iter = chybnyNazev.find(jmeno);

		Najdi(iter->second, count);
		return iter->second;
	}
	return name;
}

void CSupermarket::Najdi(const string & name, int count)
{
	auto itPrvek = sklad.find(name);

	auto itVyskaldniDuplicita = vyskadneni.find(Transakce(name,0, 0));
	if (itVyskaldniDuplicita != vyskadneni.end()) {
		if (itVyskaldniDuplicita->first.zbyva == 0) return;

		if (itVyskaldniDuplicita->first.zbyva <= count) {
			Transakce t(name, itVyskaldniDuplicita->first.odeber + itVyskaldniDuplicita->first.zbyva, 0);


			vyskadneni.erase(itVyskaldniDuplicita);
			vyskadneni.insert(make_pair(t, itPrvek));
		}
		else {
			Transakce t(name, itVyskaldniDuplicita->first.odeber + count, itVyskaldniDuplicita->first.zbyva - count);


			vyskadneni.erase(itVyskaldniDuplicita);
			vyskadneni.insert(make_pair(t, itPrvek));
		}

	}
	else {
		if (itPrvek->second.first <= count) {
			vyskadneni.insert(make_pair(Transakce(name, itPrvek->second.first, 0), itPrvek));

		}
		else {
			vyskadneni.insert(make_pair(Transakce(name, count, itPrvek->second.first - count), itPrvek));
		}
	}

}

void CSupermarket::SmazChyby(const string & name)
{
	for (unsigned int i = 0; i < name.length(); ++i) {
		string novy(name);
		novy[i] = '#';
		auto itChyba = chybnyNazev.find(novy);
		if ((itChyba != chybnyNazev.end()) && (itChyba->second == name)) {
			chybnyNazev.erase(itChyba);
		}
	}


}



//------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
int main(void)
{
	CSupermarket s;

	CDate a(30, 5, 1993);
	
	 
	
	s.Store("bread", CDate(2016, 4, 30), 100) .
		Store("butter", CDate(2016, 5, 10), 10) .
		Store("beer", CDate(2016, 8, 10), 50) .
		Store("bread", CDate(2016, 4, 25), 100) .
		Store("okey", CDate(2016, 7, 18), 5);
	
	list<pair<string, int> > l0 = s.Expired(CDate(2018, 4, 30));
	cout << (l0.size() == 4) << endl;;
	cout << ((l0 == list<pair<string, int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } })) << endl;
	
	list<pair<string, int> > l1{ { "bread", 2 },{ "Coke", 5 },{ "butter", 20 } };
	s.Sell(l1);
	cout << (l1.size() == 2) << endl;
	cout << ((l1 == list<pair<string, int> > { { "Coke", 5 }, { "butter", 10 } })) << endl;
	

	list<pair<string, int> > l2 = s.Expired(CDate(2016, 4, 30));
	cout << (l2.size() == 1) << endl;
	cout << ((l2 == list<pair<string, int> > { { "bread", 98 } })) << endl;

	list<pair<string, int> > l3 = s.Expired(CDate(2016, 5, 20));
	cout << (l3.size() == 1) << endl;
	cout << ((l3 == list<pair<string, int> > { { "bread", 198 } })) << endl;

	list<pair<string, int> > l4{ { "brewd", 105 } };
	s.Sell(l4);
	cout << (l4.size() == 0) << endl;
	cout << ((l4 == list<pair<string, int> > {  })) << endl;
	
	list<pair<string, int> > l5 = s.Expired(CDate(2017, 1, 1));
	cout << (l5.size() == 3) << endl;
 	cout << ((l5 == list<pair<string, int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } })) << endl;
	
	s.Store("Coke", CDate(2016, 12, 31), 10);

 
	list<pair<string, int> > l6{ { "Cake", 1 },{ "Coke", 1 },{ "cake", 1 },{ "coke", 1 },{ "cuke", 1 },{ "Cokes", 1 } };

	s.Sell(l6);
	cout << (l6.size() == 3) << endl;
	cout << ((l6 == list<pair<string, int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } })) << endl;

	
	list<pair<string, int> > l7 = s.Expired(CDate(2017, 1, 1));
	cout << (l7.size() == 4) << endl;
	cout << (l7 == list<pair<string, int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } }) << endl;

	s.Store("cake", CDate(2016, 11, 1), 5);



	cout << "Dalsi ukazkove testy: " << endl;
	list<pair<string, int> > l8{ { "Cake", 1 },{ "Coke", 1 },{ "cake", 1 },{ "coke", 1 },{ "cuke", 1 } };
	s.Sell(l8);
	cout << (l8.size() == 2) << endl;
	cout << ((l8 == list<pair<string, int> > { { "Cake", 1 }, { "coke", 1 } })) << endl;

	list<pair<string, int> > l9 = s.Expired(CDate(2017, 1, 1));
	cout << (l9.size() == 5) << endl;
	cout << ((l9 == list<pair<string, int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } })) << endl;

	list<pair<string, int> > l10{ { "cake", 15 },{ "Cake", 2 } };
	s.Sell(l10);
	cout << (l10.size() == 2) << endl;
	cout << ((l10 == list<pair<string, int> > { { "cake", 12 }, { "Cake", 2 } })) << endl;

	list<pair<string, int> > l11 = s.Expired(CDate(2017, 1, 1));
	cout << (l11.size() == 4) << endl;
	cout << ((l11 == list<pair<string, int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } })) << endl;

	


	list<pair<string, int> > l12{ { "Cake", 4 } };
	s.Sell(l12);
	cout << (l12.size() == 0) << endl;
	cout << ((l12 == list<pair<string, int> > {  })) << endl;

	list<pair<string, int> > l13 = s.Expired(CDate(2017, 1, 1));
	cout << (l13.size() == 4) << endl;
	cout << ((l13 == list<pair<string, int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } })) << endl;
	

	list<pair<string, int> > l14{ { "Beer", 20 },{ "Coke", 1 },{ "bear", 25 },{ "beer", 10 } };
	s.Sell(l14);
	cout << (l14.size() == 1) << endl;

	cout << ((l14 == list<pair<string, int> > { { "beer", 5 } })) << endl;


	//-----------VL-TEST---------- odeberu vse | odeberu min nez mam | odeberu vic nez mam
	CSupermarket albert;
	// otestovat jetli to funguje pridavnu polozky se stejnym datumem  
	albert.Store("bread", CDate(2016, 4, 30), 100) .
		Store("butter", CDate(2016, 5, 10), 10) .
		Store("beer", CDate(2016, 8, 10), 50) .
		Store("bread", CDate(2016, 4, 25), 100) .
		Store("okey", CDate(2016, 7, 18), 5);

	cout << "Vlastni test : " << endl;
	list<pair<string, int> > nl1{ { "okey", 5 },{ "beer", 10 },{ "butter", 20 } };
	albert.Sell(nl1);
	cout << (nl1.size() == 1) << endl;

	cout << ((nl1 == list<pair<string, int> > { { "butter", 10 } })) << endl;

	list<pair<string, int> > nl2 = albert.Expired(CDate(2018, 4, 30));
	cout << (nl2.size() == 2) << endl;
	cout << ((nl2 == list<pair<string, int> > { { "bread", 200 }, { "beer", 40 }  })) << endl;
	//-----------------------------

	CSupermarket kaufland;
	list<pair<string, int> > nl3 = kaufland.Expired(CDate(2018, 4, 30));
	cout << (nl3.size() == 0) << endl;
	cout << ((nl3 == list<pair<string, int> > {  })) << endl;

	list<pair<string, int> > nla5{ { "buttEr", 20 } };
	kaufland.Sell(nla5);
	cout << (nla5.size() == 1) << endl;
	cout << ((nla5 == list<pair<string, int> > { { "buttEr", 20 } })) << endl;



	kaufland.Store("butter", CDate(2016, 5, 10), 10);
	list<pair<string, int> > nl4 = kaufland.Expired(CDate(2017, 4, 30));
	cout << (nl4.size() == 1) << endl;
	cout << ((nl4 == list<pair<string, int> > { { "butter", 10 } })) << endl;

	cout << "test na nespravna slova" << endl;
	list<pair<string, int> > nl5{ { "buttEr", 20 } };
	kaufland.Sell(nl5);
	cout << (nl5.size() == 1) << endl;
	cout << ((nl5 == list<pair<string, int> > { { "buttEr", 10 } })) << endl;

	list<pair<string, int> > nl6 = kaufland.Expired(CDate(2018, 4, 30));
	cout << (nl6.size() == 0) << endl;
	cout << ((nl6 == list<pair<string, int> > {  })) << endl;

	list<pair<string, int> > nl7{ { "buttEr", 20 } };
	kaufland.Sell(nl7);
	cout << (nl7.size() == 1) << endl;
	cout << ((nl7 == list<pair<string, int> > { { "buttEr", 20 } })) << endl;

	list<pair<string, int> > nl8{ {} };
	kaufland.Sell(nl8);
	cout << (nl8.size() == 0) << endl;
	cout << ((nl8 == list<pair<string, int> > {  })) << endl;
	//----------------------------

	CSupermarket obi;
	obi.Store("bread", CDate(2016, 4, 30), 100) .
		Store("bread", CDate(2016, 4, 25), 100);
	
	list<pair<string, int> > nl9{ { "bread", 105 },{ "Bread", 95 } };
	obi.Sell(nl9);
	cout << (nl9.size() == 0) << endl;
	cout << ((nl9 == list<pair<string, int> > {  })) << endl;

	//----------------------------

	CSupermarket billa;
	billa.Store("bread", CDate(2016, 4, 25), 100) .
		Store("bread", CDate(2016, 4, 25), 100);

	list<pair<string, int> > nl10{ { "bread", 250 },{ "bread", 50 } };
	billa.Sell(nl10);
	cout << (nl10.size() == 2) << endl;
	cout << ((nl10 == list<pair<string, int> > { { "bread", 50 }, { "bread", 50 } })) << endl;
 

	list<pair<string, int> > nl11{ { "bread", 0 } };
	billa.Sell(nl11);
	cout << (nl11.size() == 0) << endl;
	cout << ((nl11 == list<pair<string, int> > {  })) << endl;

	//----------------------------








	return 0;
}
#endif /* __PROGTEST__ */

