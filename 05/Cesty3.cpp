// Cesty.cpp : Defines the entry point for the console application.
//

#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
class NoRouteException
{
};
//=================================================================================================



template <typename _T, typename _E>
class CRoute
{
public:
	// default constructor
	CRoute(){};
	// destructor
	~CRoute(){};
	
	// Add 
	CRoute<_T, _E>& Add(const _T & u1, const _T & u2, const _E & e) {	
		auto itlow = mapa.find(u1);
		
		if ((itlow == mapa.end())) {	//prvek tam neni
			vector<pair<_T, _E>>  pom;
			pom.push_back( pair<_T, _E>(u2, e));
			mapa.insert(itlow, pair<_T, vector< pair< _T, _E> > >(u1, vector< pair< _T, _E> >(pom)));	
		}
		else {							//prvek tam je
			itlow->second.insert(itlow->second.end(), pair<_T, _E>(u2, e));
		}

		// opacna cesta grafu
		auto itlow2 = mapa.find(u2);

		if ((itlow2 == mapa.end())) {	//prvek tam neni
			vector<pair<_T, _E>>  pom2;
			pom2.push_back( pair<_T, _E>(u1, e));
			mapa.insert(itlow2, pair<_T, vector< pair< _T, _E> > >(u2, vector< pair< _T, _E> >(pom2)));

		}
		else {							//prvek tam je
			itlow2->second.insert(itlow2->second.end(), pair<_T, _E>(u1, e));
		}

		return *this;
	}

	void vypis() {
		for (auto it = mapa.begin(); it != mapa.cend(); ++it) {
			cout << "Z: " << it->first <<  endl;
			for (unsigned i = 0; i < it->second.size(); i++) {
				cout << "\t  -> DO: " << it->second[i].first << "       \t" << it->second[i].second << endl;
			}

		}
	} 

	// Find (with  filter)
	list <_T> Find(const _T & u1, const _T & u2, const function<bool(const _E & pom)> f)const {
		queue<_T> fronta;
		set  <_T> open;
		set  <_T> close;
		list <_T> path;
		map  <_T, _T> prev;
		int flag = 0;

		// Implemetace BFS podle algoritmu z prednasek z BI-ZUM
		fronta.push(u1);

		while (!(fronta.empty())) {
			_T current(fronta.front());
			fronta.pop();
			
			if (current == u2) { // rekonstrukce cesty
					_T reconstuct = u2;
					path.push_back(reconstuct);
					while (reconstuct != u1) {
						auto it3 = prev.find(reconstuct);
						reconstuct = it3->second;
						path.push_front(reconstuct);
					}
					flag = 1;		// priznak existujici cety
				break; // konec
			} 

			auto it = mapa.find(current);
			if (it == mapa.end()) throw NoRouteException(); //cesta nenalezena

			for (unsigned i = 0; i < it->second.size(); i++) {  
				_T iteration(it->second[i].first);
				if((close.find(iteration) == close.end()) && (open.find(iteration) == open.end())&&f(it->second[i].second)) { // uzel je FRESH
					fronta.push(iteration);
					open.insert(iteration);
					prev.insert(prev.end(), pair<_T, _T>(iteration, current));
				}

			}
			close.insert(current);
		}

		if(flag == 0 ) throw NoRouteException(); //cesta nenalezena

		return path;
	}	


	// Find (without  filter)
	list<_T> Find(const _T & u1, const _T & u2) const {
		queue<_T> fronta;
		set  <_T> open;
		set  <_T> close;
		list <_T> path;
		map  <_T, _T> prev;
		int flag = 0;

		// Implemetace BFS podle algoritmu z prednasek z BI-ZUM
		fronta.push(u1);

		while (!(fronta.empty())) {
			_T current(fronta.front());
			fronta.pop();
			
			if (current == u2) { // rekonstrukce cesty
					_T reconstuct = u2;
					path.push_back(reconstuct);
					while (reconstuct != u1) {
						auto it3 = prev.find(reconstuct);
						reconstuct = it3->second;
						path.push_front(reconstuct);
					}
					flag = 1;		// priznak existujici cety
				break; // konec
			} 

			auto it = mapa.find(current);
			if (it == mapa.end()) throw NoRouteException(); //cesta nenalezena

			for (unsigned i = 0; i < it->second.size(); i++) {  
				_T iteration(it->second[i].first);
				if((close.find(iteration) == close.end()) && (open.find(iteration) == open.end())) { // uzel je FRESH
					fronta.push(iteration);
					open.insert(iteration);
					prev.insert(prev.end(), pair<_T, _T>(iteration, current));
				}

			}
			close.insert(current);
		}

		if(flag == 0 ) throw NoRouteException(); //cesta nenalezena

		return path;
	}


	map<_T, vector< pair< _T, _E> > > mapa;
};











#ifndef __PROGTEST__
//=================================================================================================
class CTrain
{
public:
	//CTrain() = delete;
	CTrain(const string    & company,
		int               speed)
		: m_Company(company),
		m_Speed(speed)
	{
	}
	friend std::ostream& operator<<(std::ostream& os, const CTrain& x) {
		os << "Vlak " << x.m_Company << " -  " << x.m_Speed << "km/h; ";
		return os;
	}


	//---------------------------------------------------------------------------------------------
	string                   m_Company;
	int                      m_Speed;
};
//=================================================================================================
class TrainFilterCompany
{
public:
	TrainFilterCompany(const set<string> & companies) : m_Companies(companies) {}
	//---------------------------------------------------------------------------------------------
	bool operator () (const CTrain & train) const {
		return m_Companies.find(train.m_Company) != m_Companies.end();
	}
	//---------------------------------------------------------------------------------------------
private:
	set <string> m_Companies;
};
//=================================================================================================
class TrainFilterSpeed {
public:
	TrainFilterSpeed(int min, int max) : m_Min(min), m_Max(max) {}
	//---------------------------------------------------------------------------------------------
	bool operator ()                   (const CTrain    & train) const
	{
		return train.m_Speed >= m_Min && train.m_Speed <= m_Max;
	}
	//---------------------------------------------------------------------------------------------
private:
	int                      m_Min;
	int                      m_Max;
};
//=================================================================================================
bool NurSchnellzug(const CTrain    & zug)
{
	return (zug.m_Company == "OBB" || zug.m_Company == "DB") && zug.m_Speed > 100;
}
//=================================================================================================

static string      toText(const list<string> & l)
{
	ostringstream oss;

	auto it = l.cbegin();

	oss << *it;
	for (++it; it != l.cend(); ++it){
		oss << " > " << *it;
	}

	return oss.str();
}
//=================================================================================================


int main(void)
{
	CRoute<string, CTrain> lines;

	lines.Add("Berlin", "Prague", CTrain("DB", 120))
		.Add("Berlin", "Prague", CTrain("CD", 80))
		.Add("Berlin", "Dresden", CTrain("DB", 160))
		.Add("Dresden", "Munchen", CTrain("DB", 160))
		.Add("Munchen", "Prague", CTrain("CD", 90))
		.Add("Munchen", "Linz", CTrain("DB", 200))
		.Add("Munchen", "Linz", CTrain("OBB", 90))
		.Add("Linz", "Prague", CTrain("CD", 50))
		.Add("Prague", "Wien", CTrain("CD", 100))
		.Add("Linz", "Wien", CTrain("OBB", 160))
		.Add("Paris", "Marseille", CTrain("SNCF", 300))
		.Add("Paris", "Dresden", CTrain("SNCF", 250));

	lines.vypis();

	list<string> r1 = lines.Find("Berlin", "Linz"); // co kdyz tu bude nezname mesto
	cout << (toText(r1) == "Berlin > Prague > Linz") << endl;

	list<string> r2 = lines.Find("Linz", "Berlin");
	cout << (toText(r2) == "Linz > Prague > Berlin") << endl;

	list<string> r3 = lines.Find("Wien", "Berlin");
	cout << (toText(r3) == "Wien > Prague > Berlin") << endl;

	list<string> r4 = lines.Find("Wien", "Berlin", NurSchnellzug);
	cout << (toText(r4) == "Wien > Linz > Munchen > Dresden > Berlin")<< endl;

	list<string> r5 = lines.Find("Wien", "Munchen", TrainFilterCompany(set<string> { "CD", "DB" }));
	cout << (toText(r5) == "Wien > Prague > Munchen")<< endl;

	list<string> r6 = lines.Find("Wien", "Munchen", TrainFilterSpeed(120, 200));
	cout << (toText(r6) == "Wien > Linz > Munchen")<< endl;

	list<string> r7 = lines.Find("Wien", "Munchen", [](const CTrain & x) { return x.m_Company == "CD"; });
	cout << (toText(r7) == "Wien > Prague > Munchen")<< endl;

	
	list<string> r8 = lines.Find("Munchen", "Munchen");
	cout << (toText(r8) == "Munchen") << endl;

	list<string> r9 = lines.Find("Marseille", "Prague");
	cout << (toText(r9) == "Marseille > Paris > Dresden > Berlin > Prague"
		|| toText(r9) == "Marseille > Paris > Dresden > Munchen > Prague") << endl;

		try
	{
		list<string> r10 = lines.Find("Marseille", "Prague", NurSchnellzug);
		assert("Marseille > Prague connection does not exist!!" == NULL);
	}
	catch (const NoRouteException & e)
	{
		cout << "connection does not exist (with filter)" << endl;
	}
	
	list<string> r11 = lines.Find("Salzburg", "Salzburg");
	cout << (toText(r11) == "Salzburg") << endl;

	list<string> r12 = lines.Find("Salzburg", "Salzburg", [](const CTrain & x) { return x.m_Company == "SNCF"; });
	assert(toText(r12) == "Salzburg");

	
	try
	{
		list<string> r13 = lines.Find("London", "Oxford");
		assert ("London > Oxford connection does not exist!!" == NULL);
	}
	catch (const NoRouteException & e)
	{
		cout << "connection does not exist (without filter)" << endl;
	}

	//--------------------------------------------------------------------
	cout << endl << "----------------------------------------------------" << endl;
	cout << "Vlastni testy: " << endl;
	try
	{
		list<string> r13 = lines.Find("Marseille", "Oxford");
		assert("London > Oxford connection does not exist!!" == NULL);
	}
	catch (const NoRouteException & e)
	{
		cout << "connection does not exist" << endl;
	}


	return 0;
}
#endif  /* __PROGTEST__ */
