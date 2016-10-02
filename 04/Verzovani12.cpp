// Verzovani2.cpp : Defines the entry point for the console application.
//

#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <stdint.h>
using namespace std;
#endif /* __PROGTEST__ */

class CFile
{
public:
	CFile(void);

	CFile(const CFile & copy);				// copy cons	
	~CFile();								// des
	CFile& operator=(const CFile & copy);	// op=

	bool Seek(uint32_t offset);
	uint32_t Read(uint8_t * dst, uint32_t bytes);
	uint32_t Write(const uint8_t * src, uint32_t bytes);
	void Truncate(void);
	uint32_t FileSize(void) const;
	void AddVersion(void);
	bool UndoVersion(void);

private:
	struct Push {
		int pocetRef;
		uint32_t velikost;  //velikost segmentu = 512
		uint32_t zaplneni;  //ukazuje na prvni prvek
		uint8_t * pole;
		Push() {
			pole = new uint8_t[32];
			pocetRef = 1;
			velikost = 32;
			zaplneni = 0;
		}
		void zvetsit() {
			velikost = (velikost < 100) ? velikost + 10 : (int)(velikost * 1.5);	
			uint8_t * nove = new  uint8_t[(2 * velikost)];
			memcpy(nove, pole, velikost);
			delete[] pole;
			pole = nove;//pocerRef
			
		}
	};

	struct Verze {
		Push *commit;
		Verze * next;
		uint32_t poziceVer;
		int ulozeno;
		Verze() {
			commit = NULL;
			next = NULL;
			ulozeno = 0;
			poziceVer = 0;
		}
	};


	Verze * ver;
	void deepCopy();
	void deleteVezovani();
};
//-------------------------------------------------------------------------------------------------

//OK
CFile::CFile(void)
{
	Verze * verze = new Verze;
	ver = verze;
}



CFile::CFile(const CFile & copy)
{
	if (copy.ver == NULL) {
		Verze * verze = new Verze;
		ver = verze;
		return;
	}
	if (copy.ver->commit == NULL) {//next byt nemuze
		Verze * verze = new Verze;
		ver = verze;
		return;
	}


	Verze * ptr_copy = copy.ver;
	Verze * ptr_verze = ver;
	int flag = 0;

	while (ptr_copy != NULL) {

		Verze * nova = new Verze;
		ptr_copy->ulozeno = 1;
		nova->commit = ptr_copy->commit;
		uint32_t a = ptr_copy->poziceVer;
		nova->poziceVer = a;
		nova->commit->pocetRef += 1;
		nova->ulozeno = 1;

		if (flag == 0) {
			ver = nova;
			flag = 1;
		}
		else {
			ptr_verze->next = nova;
		}

		ptr_verze = nova;
		ptr_copy = ptr_copy->next;
	}
}

CFile::~CFile()
{
	deleteVezovani();
}

CFile & CFile::operator=(const CFile & copy)
{
	if (this == &copy) return *this;

	if (ver->commit == NULL) {
		return *this;
	}


	deleteVezovani();
	Verze * verze = new Verze;
	ver = verze;

	Verze * ptr_copy = copy.ver;
	Verze * ptr_verze = ver;
	int flag = 0;


	if (copy.ver == NULL) {

		return *this;
	}
	if (copy.ver->commit == NULL) {

		return *this;
	}

	while (ptr_copy != NULL) {

		Verze * nova = new Verze;
		nova->commit = ptr_copy->commit;
		nova->poziceVer = ptr_copy->poziceVer;
		nova->commit->pocetRef += 1;
		nova->ulozeno = 1;

		if (flag == 0) {
			delete ver;
			ver = nova;
			flag = 1;
		}
		else {
			ptr_verze->next = nova;
		}

		ptr_verze = nova;
		ptr_copy = ptr_copy->next;
	}


	return *this;
}

bool CFile::Seek(uint32_t offset)
{
	if (offset == 0) {
		if ((ver == NULL) || (ver->commit == NULL)) return true;
		ver->poziceVer = offset;
		return true;
	}

	if (ver == NULL) return false;
	if (ver->commit == NULL) return false;

	if (offset < 0) return false;
	if (offset > ver->commit->zaplneni) return false;

	ver->poziceVer = offset;

	return true;
}

uint32_t CFile::Read(uint8_t * dst, uint32_t bytes)
{
	if (ver == NULL) return 0;
	if (ver->commit == NULL) return 0;

	uint32_t i = 0;

	while ((ver->poziceVer < ver->commit->zaplneni) && (i < bytes)) {

		dst[i] = ver->commit->pole[ver->poziceVer];

		i++;
		ver->poziceVer++;
	}
	return i;
}


void CFile::deepCopy()
{
	if (ver->ulozeno == 1) {
		Push * novyP = new Push;

		for (uint32_t i = 0; i < ver->commit->zaplneni; i++) {

			if (i == novyP->velikost) novyP->zvetsit();
			novyP->pole[i] = ver->commit->pole[i];
		}


		ver->commit->pocetRef -= 1;
		if (ver->commit->pocetRef <= 0) {
			delete[] ver->commit->pole;
			delete ver->commit;
		}

		//nastaveni pozice v souboru je hotove 
		novyP->zaplneni = ver->commit->zaplneni;
		ver->commit = novyP;
		ver->ulozeno = 0;

	}
}

//
void CFile::deleteVezovani()
{
	if (ver == NULL) {
		return;
	}

	if ((ver->commit == NULL) && (ver->next == NULL)) {
		delete ver;
		ver = NULL;
	}

	Verze *tmp;

	while (ver != NULL) {
		ver->commit->pocetRef -= 1;
		if (ver->commit->pocetRef <= 0) {
			delete[] ver->commit->pole;
			delete ver->commit;
		}
		tmp = ver;

		ver = ver->next;
		tmp->next = NULL;
		delete tmp;
	}
}


uint32_t CFile::Write(const uint8_t * src, uint32_t bytes)
{
	if (bytes <= 0) return 0;

	if ((ver->commit == NULL)) {
		Push *pool = new Push;
		ver->commit = pool;
	}
	else {
		deepCopy();
	}


	uint32_t j = 0;

	for (uint32_t i = ver->poziceVer; i < (bytes + ver->poziceVer); i++) {

		if (i == ver->commit->velikost) ver->commit->zvetsit();
		ver->commit->pole[i] = src[j++];
	}

	ver->poziceVer += j;

	//je to ok?
	if ((ver->poziceVer) > ver->commit->zaplneni) {
		ver->commit->zaplneni = ver->poziceVer;
	}

	return j;
}


void CFile::Truncate(void)
{
	if (ver == NULL) return;
	if (ver->commit == NULL) return;

	deepCopy();
	ver->commit->zaplneni = ver->poziceVer;
}


uint32_t CFile::FileSize(void) const
{
	if (ver == NULL) return 0;
	if (ver->commit == NULL) return 0;
	return ver->commit->zaplneni;
}

void CFile::AddVersion(void)
{
	if (ver == NULL) return;
	if (ver->commit == NULL) return;

	Verze *n = new Verze;

	n->ulozeno = 1;
	n->commit = ver->commit;
	n->commit->pocetRef += 1;
	n->poziceVer = ver->poziceVer;
	n->next = ver;
	ver = n;
}

bool CFile::UndoVersion(void)
{
	if (ver == NULL) return false;
	if (ver->next == NULL) return false;

	Verze *tmp = ver;
	ver->commit->pocetRef -= 1;

	if (ver->commit->pocetRef <= 0) {
		delete[] ver->commit->pole;
		delete ver->commit;
	}

	ver = ver->next;
	tmp->commit = NULL;
	tmp->next = NULL;
	delete tmp;

	return true;
}


//-------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
bool writeTest(CFile & x, const initializer_list<uint8_t> & data, uint32_t wrLen)
{
	uint8_t  tmp[650];
	uint32_t idx = 0;

	for (auto v : data)
		tmp[idx++] = v;
	return x.Write(tmp, idx) == wrLen;

}

bool readTest(CFile & x, const initializer_list<uint8_t> & data, uint32_t rdLen)
{
	uint8_t  tmp[650];
	uint32_t idx = 0;


	if (x.Read(tmp, rdLen) != data.size()) return false;
	for (auto v : data) {
		if (tmp[idx] != v) {
			return false;
		}
		idx++;
	}
	return true;
}
//-------------------------------------------------------------------------------------------------



int main(void)
{

	//------------------------------
	cout << "Zakladni testy  - ukazka: " << endl;
	CFile f0;
	cout << (writeTest(f0, { 10, 20, 30 }, 3)) << endl;
	cout << (f0.FileSize() == 3) << endl;
	cout << (writeTest(f0, { 60, 70, 80 }, 3)) << endl;
	cout << (f0.FileSize() == 6) << endl;
	cout << (f0.Seek(2)) << endl;
	cout << (writeTest(f0, { 5, 4 }, 2)) << endl;
	cout << (f0.FileSize() == 6) << endl;
	cout << (f0.Seek(1)) << endl;
	cout << (readTest(f0, { 20, 5, 4, 70, 80 }, 7)) << endl;
	cout << (f0.Seek(3)) << endl;
	f0.AddVersion();
	cout << (f0.Seek(6)) << endl;
	cout << (writeTest(f0, { 100, 101, 102, 103 }, 4)) << endl;
	f0.AddVersion();
	cout << (f0.Seek(5)) << endl;

	CFile f1(f0);
	f0.Truncate();
	cout << (f0.Seek(0)) << endl;
	cout << (readTest(f0, { 10, 20, 5, 4, 70 }, 20)) << endl;
	cout << (f0.UndoVersion()) << endl;
	cout << (f0.Seek(0)) << endl;
	cout << (readTest(f0, { 10, 20, 5, 4, 70, 80, 100, 101, 102, 103 }, 20)) << endl;
	cout << (f0.UndoVersion()) << endl;
	cout << (f0.Seek(0)) << endl;
	cout << (readTest(f0, { 10, 20, 5, 4, 70, 80 }, 20)) << endl;
	cout << (!f0.Seek(100)) << endl;
	cout << (writeTest(f1, { 200, 210, 220 }, 3)) << endl;
	cout << (f1.Seek(0)) << endl;

	cout << (readTest(f1, { 10, 20, 5, 4, 70, 200, 210, 220, 102, 103 }, 20)) << endl;
	cout << (f1.UndoVersion()) << endl;
	cout << (f1.UndoVersion()) << endl;
	cout << (readTest(f1, { 4, 70, 80 }, 20)) << endl;
	cout << (!f1.UndoVersion()) << endl;

	//------------------------------
	cout << "Vlastni testy  - ukazka: " << endl;
	CFile f2;
	cout << (writeTest(f2, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (f2.FileSize() == 6) << endl;
	cout << (f2.Seek(3)) << endl;
	cout << (readTest(f2, { 60 }, 1)) << endl;
	f2.AddVersion();
	cout << (writeTest(f2, { 70, 70 }, 2)) << endl;
	cout << (f2.Seek(0)) << endl;
	CFile f3(f2);
	cout << " Dalsi testy I: " << endl;

	cout << (readTest(f2, { 10, 20, 30, 60, 70, 70 }, 20)) << endl;
	cout << (f2.Seek(0)) << endl;
	cout << (readTest(f3, { 10, 20, 30, 60, 70, 70 }, 20)) << endl;
	cout << (writeTest(f2, { 70, 70, 70, 70 }, 4)) << endl;
	cout << (f3.Seek(2)) << endl;
	cout << (readTest(f3, { 30, 60, 70, 70 }, 20)) << endl;

	cout << " Dalsi testy II: " << endl;
	cout << (f3.Seek(2)) << endl;
	CFile f4 = f1 = f3;
	cout << (readTest(f3, { 30, 60, 70, 70 }, 20)) << endl;
	cout << (readTest(f1, { 30, 60, 70, 70 }, 20)) << endl;
	cout << (readTest(f4, { 30, 60, 70, 70 }, 20)) << endl;
	cout << " Dalsi testy III: " << endl;
	f1.UndoVersion();
	f4.UndoVersion();
	f2.UndoVersion();
	cout << (f2.Seek(0)) << endl;
	cout << (f4.Seek(0)) << endl;
	cout << (f1.Seek(0)) << endl;
	cout << (readTest(f1, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (readTest(f4, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (readTest(f2, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << " Dalsi testy IV: " << endl;

	CFile f5 = f3;
	CFile f6 = f2;

	CFile f8;
	cout << (f8.Seek(0)) << endl;

	CFile f9(f8);
	CFile f7;
	f7.UndoVersion();
	f7 = f7;
	f7 = f2;

	f7 = f7;
	f7 = f8;

	CFile f12;
	f2 = f12;
	f12 = f6;

	cout << " Dalsi testy V: " << endl;
	cout << (writeTest(f7, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (f7.FileSize() == 6) << endl;
	cout << (f7.Seek(3)) << endl;
	cout << (readTest(f7, { 60 }, 1)) << endl;
	f7.AddVersion();
	cout << (writeTest(f7, { 70, 70 }, 2)) << endl;
	cout << (f7.Seek(0)) << endl;


	cout << !(readTest(f2, { NULL, NULL }, 2)) << endl;


	cout << " Dalsi testy VI: " << endl;

	CFile f13;
	cout << (writeTest(f13, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (writeTest(f13, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (writeTest(f13, { 10, 20, 30, 60, 70, 80 }, 6)) << endl;
	cout << (f13.FileSize() == 18) << endl;
	cout << (f13.Seek(0)) << endl;
	cout << (writeTest(f13, { 10, 10, 10 }, 3)) << endl;
	cout << (writeTest(f13, { 10, 10, 10 }, 3)) << endl;
	cout << (readTest(f13, { 10, 20, 30 }, 3)) << endl;
	cout << (readTest(f13, { 60, 70, 80 }, 3)) << endl;
	cout << (f13.Seek(12)) << endl;
	cout << (readTest(f13, { 10 }, 1)) << endl;
	cout << " Dalsi testy VII: " << endl;
	cout << (f13.Seek(12)) << endl;


	CFile f14;




	return 0;
}
#endif /* __PROGTEST__ */

