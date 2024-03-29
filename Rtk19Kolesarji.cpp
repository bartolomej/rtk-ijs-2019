#define _CRT_SECURE_NO_WARNINGS // quit whining, M$VC...

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstring>

#define EVAL

#ifndef EVAL
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

class EAssertion
{
public:
	string desc, file; int line;
	EAssertion(const string &desc_, const string &file_, const int line_) : desc(desc_), file(file_), line(line_) { }
	static inline bool Throw(const string &desc, const string &file, const int line) { throw EAssertion(desc, file, line); return false; }
};

#define Assert(x) ((x) ? true : EAssertion::Throw("Assertion failure (" #x ")", __FILE__, __LINE__))
#define AssertR(x, reason) ((x) ? true : EAssertion::Throw(string(reason).c_str(), __FILE__, __LINE__))
#define AssertU(x) AssertR((x), "Nepricakovana napaka.  Predlagamo, da obvestis organizatorje tekmovanja.")

string TaskName = "Kolesarji";
enum { MaxN = 1000, MaxD = 1000, MaxTock = 1000 };

bool Assert_(bool x, const char *file, const int line, const char *desc)
{
	if (!x)
		fprintf(stderr, "[%s:%d] Assertion failure (%s)\n", file, line, desc);
	return x;
}
//#undef Assert
//#define Assert(x) Assert_((x), __FILE__, __LINE__, #x)

string IntToStr(int x)
{
	char buf[20]; sprintf(buf, "%d", x); return buf;
}

void ParseLine(const string& s, vector<int> &v)
{
	v.clear(); int i = 0, n = s.size();
	while (i < n)
	{
		if (isspace(s[i])) { i++; continue; }
		AssertR(s[i] >= '0' && s[i] <= '9', "namesto stevke se pojavlja znak s kodo " + IntToStr(int((unsigned char) s[i])));
		int x = s[i] - '0'; i++;
		auto M = numeric_limits<decltype(x)>::max();
		while (i < n && s[i] >= '0' && s[i] <= '9')
		{
			int d = s[i] - '0';
			AssertR(x < M / 10 || (x == M / 10 && d <= M % 10), "stevilo " + IntToStr(x) + string(1, s[i]) + " je preveliko");
			x = x * 10 + d; i++;
		}
		v.push_back(x);
	}
}

class TTestCase
{
public:
	string problemName;
	int inputNo, n, k, m, d;
	vector<int> ti;
	vector<vector<int>> dirke;

	void Read(istream& is, int &lineNo, string problemName_)
	{
		problemName = problemName_;
		string s; getline(is, s); lineNo++; AssertR(!is.fail(), "manjka " + IntToStr(lineNo) + ". vrstica");
		AssertR(s.length() == 0, "vrstica " + IntToStr(lineNo) + " bi morala biti prazna, ne pa \"" + s + "\"");
		getline(is, s); lineNo++; AssertR(!is.fail(), "manjka " + IntToStr(lineNo) + ". vrstica");
		int ok = sscanf(s.c_str(), "%d %d %d %d %d", &inputNo, &n, &d, &k, &m);
		AssertR(ok == 5, "v " + IntToStr(lineNo) + ". vrstici bi moralo biti pet celih stevil, ne pa \"" + s + "\"");
		AssertR(n >= 1 && n <= MaxN, "v " + IntToStr(lineNo) + ". vrstici bi moralo biti stevilo kolesarjev n od 1 do " + IntToStr(MaxN) + ", ne pa " + IntToStr(n));
		AssertR(d >= 1 && d <= MaxD, "v " + IntToStr(lineNo) + ". vrstici bi moralo biti stevilo dirk d od 1 do " + IntToStr(MaxD) + ", ne pa " + IntToStr(n));
		AssertR(k >= 1 && k <= n, "v " + IntToStr(lineNo) + ". vrstici bi morala biti velikost ekipe k od 1 do n = " + IntToStr(n) + ", ne pa " + IntToStr(k));
		AssertR(m >= 1 && m <= k && m <= n - k, "v " + IntToStr(lineNo) + ". vrstici bi morala biti velikost menjave m od 1 do min(k, n-k) = " + IntToStr(k < n - k ? k : n - k) + ", ne pa " + IntToStr(m));
		ti.clear(); dirke.clear(); dirke.resize(d);
		getline(is, s); lineNo++; AssertR(!is.fail(), "manjka vrstica s tockami po mestih");
		ParseLine(s, ti);
		AssertR(ti.size() == n, "v " + IntToStr(lineNo) + ". vrstici bi moralo biti " + IntToStr(n) + " stevil s tockami, ne pa " + IntToStr(ti.size()));
		for (int i = 0; i < n; i++)
		{
			AssertR(0 <= ti[i] && ti[i] <= MaxTock, "v " + IntToStr(lineNo) + ". vrstici je t_" + IntToStr(i + 1) + " = " + IntToStr(ti[i]) + ", moralo pa bi biti od 0 do " + IntToStr(MaxTock));
			if (i > 0) AssertR(ti[i - 1] >= ti[i], "v " + IntToStr(lineNo) + ". vrstici je t_" + IntToStr(i) + " = " + IntToStr(ti[i - 1]) + " in  t_" + IntToStr(i + 1) + " = " + IntToStr(ti[i]) + ", zaporedje ni nepadajoce");
		}
		for (int stDirke = 0; stDirke < d; stDirke++)
		{
			getline(is, s); lineNo++; AssertR(!is.fail(), "manjka vrstica za " + IntToStr(stDirke + 1) + ". dirko");
			auto &D = dirke[stDirke];
			ParseLine(s, D);
			AssertR(D.size() == n, "v " + IntToStr(lineNo) + ". vrstici bi morale biti uvrstitve n = " + IntToStr(n) + " kolesarjev, ne pa " + IntToStr(D.size()));
			vector<bool> seen; seen.resize(n); for (int i = 0; i < n; i++) seen[i] = false;
			for (int i = 0; i < D.size(); i++)
			{
				AssertR(1 <= D[i] && D[i] <= n, "v " + IntToStr(lineNo) + ". vrstici se pojavlja neveljavna stevilka kolesarja " + IntToStr(D[i]) + ", morala bi biti od 1 do n = " + IntToStr(n));
				AssertR(! seen[D[i] - 1], "v " + IntToStr(lineNo) + ". vrstici se stevilka kolesarja " + IntToStr(D[i]) + " pojavlja vec kot enkrat");
				seen[D[i] - 1] = true;
			}
		}
	}
};

void ReadTestCases(const char *fnTestCase, vector<TTestCase>& testCases)
{
	ifstream is(fnTestCase);
	//
	string s; getline(is, s);
	AssertR(!is.fail(), "manjka prva vrstica z imenom naloge");
	AssertR(s == TaskName, "v prvi vrstici bi moral biti niz \"" + TaskName + "\", ne pa \"" + s + "\"");
	string problemName = s;
	//
	getline(is, s); AssertR(!is.fail(), "manjka druga vrstica s stevilom testnih primerov");
	int nTestCases;
	int ok = sscanf(s.c_str(), "%d", &nTestCases);
	AssertR(ok == 1 && nTestCases > 0, "v drugi vrstici bi moralo biti stevilo testnih primerov, ne pa \"" + s + "\"");
	//
	testCases.clear();
	for (int testCaseNo = 1, lineNo = 2; testCaseNo <= nTestCases; testCaseNo++)
	{
		int oldLineNo = lineNo;
		TTestCase testCase; testCase.Read(is, lineNo, problemName);
		AssertR(testCase.inputNo == testCaseNo, "v vrsticah " + IntToStr(oldLineNo) + ".." + IntToStr(lineNo) + " bi moral biti testni primer stevilka " + IntToStr(testCaseNo) + ", ne pa " + IntToStr(testCase.inputNo));
		testCases.push_back(testCase);
	}
}

class TResitev
{
public:
	int userId; // 6 digits
	int caseNo;
	const TTestCase *testCase;
	vector<vector<int>> ekipe;
	int tocke;

	void Read(istream& is, int& lineNo, int caseNo_, const TTestCase& testCase_)
	{
		caseNo = caseNo_;
		testCase = &testCase_;
		const int n = testCase->n, d = testCase->d, k = testCase->k, m = testCase->m;
		ekipe.clear(); ekipe.resize(d);
		vector<int> seen; seen.resize(n); for (int i = 0; i < n; i++) seen[i] = -1;
		tocke = 0;
		for (int di = 0; di < d; di++)
		{
			auto &E = ekipe[di]; 
			string s; getline(is, s); AssertR(!is.fail(), "manjka vrstica z ekipo za " + IntToStr(di + 1) + ". dirko");
			ParseLine(s, E);
			AssertR(E.size() == k, "ekipa za " + IntToStr(di + 1) + ". dirko bi morala imeti k = " + IntToStr(k) + " kolesarjev, ne pa " + IntToStr(E.size()));
			int stMenjav = 0;
			vector<int> tockeKolesarjev; tockeKolesarjev.resize(n);
			const auto &izid = testCase->dirke[di];
			for (int i = 0; i < n; i++) tockeKolesarjev[izid[i] - 1] = testCase->ti[i];
			for (int i = 0; i < E.size(); i++)
			{
				AssertR(E[i] >= 1 && E[i] <= n, "v ekipi za " + IntToStr(di + 1) + ". dirko je kolesar " + IntToStr(E[i]) + ", moral bi biti od 1 do n = " + IntToStr(n));
				int kolesar = E[i] - 1;
				AssertR(seen[kolesar] != di, "v ekipi za " + IntToStr(di + 1) + ". dirko se kolesar " + IntToStr(E[i]) + " pojavlja vec kot enkrat");
				if (seen[kolesar] < di - 1) stMenjav++;
				seen[kolesar] = di;
				tocke += tockeKolesarjev[kolesar];
			}
			if (di > 0) AssertR(0 <= stMenjav && stMenjav <= m, "med " + IntToStr(di) + ". in " + IntToStr(di + 1) + ". dirko je " + IntToStr(stMenjav) + " menjav, moralo bi jih biti od 1 do m = " + IntToStr(m));
		}
	}

	int Score() { return tocke; }

};

int PrintHelp()
{
	printf("Uporaba: Rtk19Kolesarji eval input.txt submission.txt report.txt\n");
	return 1;
}

int Evaluate(int argc, char **argv)
{
	if (argc < 5) return PrintHelp();
	const char *fnTestCase = argv[2], *fnSubmission = argv[3], *fnReport = argv[4];
	FILE *fReport = fopen(fnReport, "wt");
	try
	{
		vector<TTestCase> testCases;
		ReadTestCases(fnTestCase, testCases);
		int nTestCases = (int)testCases.size();
		vector<pair<int, int> > results; // (caseNo, score) pairs
		{
			ifstream is(fnSubmission);
			string s; getline(is, s);
			AssertR(!is.fail(), "manjka stevilka tekmovalca");
			AssertR(s.size() == 6, "stevilka tekmovalca bi morala imeti 6 stevk");
			int userId; int ok = sscanf(s.c_str(), "%d", &userId);
			AssertR(ok == 1, "v prvi vrstici bi morala biti stevilka tekmovalca, ne pa \"" + s + "\"");
			getline(is, s); AssertR(!is.fail(), "manjka druga vrstica");
			AssertR(s == TaskName, "v drugi vrstici bi moral biti niz \"" + TaskName + "\", ne pa \"" + s + "\"");
			//
			int nSubmissions = 0, lineNo = 3;
			while (true)
			{
				getline(is, s); if (is.fail()) break;
				if (s.length() <= 0) { lineNo++; continue; }
				int caseNo; ok = sscanf(s.c_str(), "%d", &caseNo);
				AssertR(ok == 1, "v " + IntToStr(lineNo) + ". vrstici bi morala biti stevilka testnega primera, ne pa \"" + s + "\"");
				int iCase = -1; for (int i = 0; i < (int)testCases.size(); i++) if (testCases[i].inputNo == caseNo) { iCase = i; break; }
				AssertR(iCase >= 0, "v " + IntToStr(lineNo) + ". vrstici se pojavlja neveljavna stevilka testnega primera: " + IntToStr(caseNo));
				TResitev solution;
				TTestCase &testCase = testCases[iCase];
				lineNo++; solution.Read(is, lineNo, caseNo, testCase);
				//
				int score = solution.Score();
				results.push_back(pair<int, int>(caseNo, score));
				//
				nSubmissions++;
			}
		}
		fprintf(fReport, "OK\n%d\n", int(results.size()));
		for (int i = 0; i < int(results.size()); i++) fprintf(fReport, "%d %d\n", results[i].first, results[i].second);
	}
	catch (EAssertion e)
	{
		fprintf(fReport, "Napaka [%s:%d]: %s\n-1\n", /*e.file.c_str()*/ "Rtk19Kolesarji.cpp", e.line, e.desc.c_str());
	}
	catch (...)
	{
		fprintf(fReport, "Nepricakovana napaka.  Predlagamo, da obvestis organizatorje tekmovanja.\n-1\n");
	}
	fclose(fReport);
	return 0;
}

//#include "Rtk19KolesarjiSolver.cpp"

int main(int argc, char** argv)
{
	//return TestSolver(argc, argv);
	if (argc > 1 && strcmp(argv[1], "eval") == 0) return Evaluate(argc, argv);
	PrintHelp();
	return 0;
}

