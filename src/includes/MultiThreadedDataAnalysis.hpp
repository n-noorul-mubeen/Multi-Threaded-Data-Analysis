#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <thread>
#include <map>
#include <ncurses.h>
#include <filesystem>

using namespace std;

struct AnalysisResult {
  double maxTotalValue;
  double minTotalValue;
  double maxIndividualValue;
  double minIndividualValue;
  string maxTotalValueString;
  string minTotalValueString;
  string maxIndividualValueString;
  string minIndividualValueString;
  unordered_map<string, double> totalValues;
};

struct YearResult {
  string maxYear;
  double maxValue;
  string minYear;
  double minValue;
  unordered_map<string, double> totalValues;
};

struct YearUnitsSoldResult {
  string maxYear;
  int maxValue;
  string minYear;
  int minValue;
  unordered_map<string, int> totalValues;
};

struct ItemSoldResult {
 unordered_map<string, int> itemSoldCounts;
 unordered_map<string, string> predominantItemTypes;
};

bool checkCSVFileExists(const string&);

vector<pair<string, string>> getCSVInfo(const string&);

AnalysisResult analyzeData(const vector<unordered_map<string, string> >& , const string&, const string&); 

YearResult analyzeYearData(const vector<unordered_map<string, string> >&, const string& , const string&); 

YearUnitsSoldResult analyzeYearUnitsSoldData(const vector<unordered_map<string, string> >&, const string& , const string&); 


void provideInsights(WINDOW*, const AnalysisResult&, const string&); 

vector<unordered_map<string, string> > readCSV(const string&); 

void analyzeDataMultithreaded(WINDOW*,const vector<unordered_map<string, string> >&, const string& , const string&, int); 

void analyzeYearMultithreaded(WINDOW*, const vector<unordered_map<string, string> >&, const string& , const string&, int); 

void analyzeYearUnitsSoldMultithreaded(WINDOW*, const vector<unordered_map<string, string> >&, const string& , const string&, int);

ItemSoldResult analyzeItemsSold(const vector<unordered_map<string, string> >&, const string& , const string&);;

void analyzeItemsSoldMultithreaded(WINDOW*, const vector<unordered_map<string, string> >&, const string& , const string&, int);;