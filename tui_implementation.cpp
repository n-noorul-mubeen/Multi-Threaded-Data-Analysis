#include <ncurses.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <thread>
#include <map>
#include <chrono>


using namespace std;



string mainMenu[2] = {"Start", "Exit"};
string subMenu[4] = {"Descriptive Analysis", "Categorical Data Analysis", "Time Based Data Analysis", "Back"};
int choice;
int highlight = 0;
int var = 0;
int subvar = 0;
char file[100];
int subchoice;
vector<pair<string, string>> columnTypes;

int flagD = 0, flagC = 0, flagT = 0, flagSales = 0;


int columnChoice;
int wrtColumnChoice;
int threadChoice;

int columnVar1 = -1;
int columnVar2 = -1;
int numThreads;

vector<string> Column1;
vector<string> Column2;

bool checkCSVFileExists(const string& filename);
void printBanner();
void mainMenuTUI(WINDOW* menuwin);
void subMenuTUI(WINDOW* menuwin);
void setColumns(WINDOW* menuwin);
void choiceOfAnalysisTUI(WINDOW* menuwin);
void analysisTUI(WINDOW* menuwin);


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

// struct SalesChannelResult {
//  unordered_map<string, string> predominantSalesChannels;
// };

AnalysisResult analyzeData(const vector<unordered_map<string, string> >& data, const string& xField, const string& yField) {
 AnalysisResult result;

 double maxIndividualValue = 0.0;
 double minIndividualValue = numeric_limits<double>::max();
 string maxIndividualValueString;
 string minIndividualValueString;
 unordered_map<string, double> totalValues;

 for (const auto& row : data) {
  double yValue = stod(row.at(yField));
  totalValues[row.at(xField)] += yValue;
  if (yValue > maxIndividualValue)
    {
        maxIndividualValue = yValue;
        maxIndividualValueString = row.at(xField);
    }
  if (yValue < minIndividualValue)
    {
        minIndividualValue = yValue;
        minIndividualValueString = row.at(xField);
    }
 }
 result.maxIndividualValue = maxIndividualValue;
 result.maxIndividualValueString = maxIndividualValueString;
 result.minIndividualValue = minIndividualValue;
 result.minIndividualValueString = minIndividualValueString;
 result.totalValues = totalValues;

 return result;
}

YearResult analyzeYearData(const vector<unordered_map<string, string> >& data, const string& xField, const string& yField) {
  YearResult result;

  string maxYear;
  double maxValue = 0.0;
  string minYear;
  double minValue = numeric_limits<double>::max();
 
  unordered_map<string, double> totalValues;

  for (const auto& row : data) {
      string xValue = row.at(xField);
      string year = xValue.substr(xValue.length() - 4);
   
      double yValue = stod(row.at(yField));
      totalValues[year] += yValue;
  }
  result.totalValues = totalValues;

  return result;
}

YearUnitsSoldResult analyzeYearUnitsSoldData(const vector<unordered_map<string, string> >& data, const string& xField, const string& yField) {
  YearUnitsSoldResult result;

  string maxYear;
  int maxValue = 0;
  string minYear;
  int minValue = numeric_limits<int>::max();
 
  unordered_map<string, int> totalValues;

  for (const auto& row : data) {
      string xValue = row.at(xField);
      string year = xValue.substr(xValue.length() - 4);
   
      int yValue = stoi(row.at(yField));
      totalValues[year] += yValue;
  }
  result.totalValues = totalValues;

  return result;
}


void provideInsights(WINDOW* menuwin, const AnalysisResult& result, const string& yField) {
  if (yField == "Total Profit") {
   mvwprintw(menuwin, 7, 1, "Insights: %s is most consistent in terms of profits because of punctual shipping.", result.maxTotalValueString.c_str());
}
if (yField == "Total Cost ") {
   mvwprintw(menuwin, 8, 1, "Insights: In %s the prices of raw materials are very high.", result.maxTotalValueString.c_str());
}
if (yField == "Total Revenue") {
   mvwprintw(menuwin, 9, 1, "Insights: In %s markets sell products at a high rate.", result.maxTotalValueString.c_str());
}
 
      // Add more conditions for other regions
 
  // Add conditions for other fields
}

vector<unordered_map<string, string> > readCSV(const string& filename) {
  vector<unordered_map<string, string> > data;
  ifstream file(filename);
  string line;

  getline(file, line);
  istringstream iss(line);
  vector<string> headers;
  string header;
  string prefix = "Total Profit";
  while (getline(iss, header, ',')) {
    if(header.substr(0, prefix.length()) == prefix)
        header = prefix;
    headers.push_back(header);
  }

  while (getline(file, line)) {
      istringstream iss(line);
      unordered_map<string, string> row;
      string field;
      for (const string& header : headers) {
          getline(iss, field, ',');
          row[header] = field;
      }
   
      data.push_back(row);
  }

  return data;
}

void analyzeDataMultithreaded(WINDOW* menuwin,const vector<unordered_map<string, string> >& data, const string& xField, const string& yField, int numThreads) {
  vector<thread> threads;
  vector<AnalysisResult> results(numThreads);
  int chunkSize = data.size() / numThreads;
  mutex resultMutex;
  int z = numThreads-1;

  for (int i = 0; i <= z; i++) {
      threads.emplace_back([i, chunkSize, &data, &xField, &yField, &results, &resultMutex, &z]() {
          int startIndex = i * chunkSize;
          int endIndex = (i == z) ? data.size() : (startIndex + chunkSize);
          vector<unordered_map<string, string>> chunkData(data.begin() + startIndex, data.begin() + endIndex);
          AnalysisResult result = analyzeData(chunkData, xField, yField);

          lock_guard<mutex> lock(resultMutex);
          results[i] = result;
      });
  }

  for (thread& t : threads) {
      t.join();
  }

  AnalysisResult finalResult;
  unordered_map<string,double> totalValues;
 
  finalResult.maxIndividualValue = 0.0;
  finalResult.minIndividualValue = numeric_limits<double>::max();
  for (const AnalysisResult& result : results) {
      for (const auto& pair : result.totalValues) {
        totalValues[pair.first] += pair.second;
    }
      if (result.maxIndividualValue > finalResult.maxIndividualValue) {
          finalResult.maxIndividualValue = result.maxIndividualValue;
          finalResult.maxIndividualValueString = result.maxIndividualValueString;
      }
      if (result.minIndividualValue < finalResult.minIndividualValue) {
          finalResult.minIndividualValue = result.minIndividualValue;
          finalResult.minIndividualValueString = result.minIndividualValueString;
      }
  }

  string maxTotalValueString;
  double maxTotalValue = 0.0;
  string minTotalValueString;
  double minTotalValue = numeric_limits<double>::max();
  for (const auto& pair : totalValues) {
    if (pair.second > maxTotalValue)
    {
        maxTotalValue = pair.second;
        maxTotalValueString = pair.first;
    }
    if (pair.second < minTotalValue)
    {
        minTotalValue = pair.second;
        minTotalValueString = pair.first;
    }
  }
  finalResult.maxTotalValue = maxTotalValue;
  finalResult.maxTotalValueString = maxTotalValueString;
  finalResult.minTotalValue = minTotalValue;
  finalResult.minTotalValueString = minTotalValueString;


  
  mvwprintw(menuwin, 3, 1, "%s has highest added %s of %.2lf", maxTotalValueString.c_str(), yField.c_str(), maxTotalValue);
  mvwprintw(menuwin, 4, 1, "%s has lowest added %s of %.2lf", minTotalValueString.c_str(), yField.c_str(), minTotalValue);
  mvwprintw(menuwin, 5, 1, "%s has highest individual %s of %.2lf", finalResult.maxIndividualValueString.c_str(), yField.c_str(), finalResult.maxIndividualValue);
  mvwprintw(menuwin, 6, 1, "%s has lowest individual %s of %.2lf", finalResult.minIndividualValueString.c_str(), yField.c_str(), finalResult.minIndividualValue);

  

  provideInsights(menuwin,finalResult, yField);
}

void analyzeYearMultithreaded(WINDOW* menuwin, const vector<unordered_map<string, string> >& data, const string& xField, const string& yField, int numThreads) {
  vector<thread> threads;
  vector<YearResult> results(numThreads);
  int chunkSize = data.size() / numThreads;
  mutex resultMutex;
  int z = numThreads-1;

  for (int i = 0; i <= z; i++) {
      threads.emplace_back([i, chunkSize, &data, &xField, &yField, &results, &resultMutex, &z]() {
          int startIndex = i * chunkSize;
          int endIndex = (i == z) ? data.size() : (startIndex + chunkSize);
          vector<unordered_map<string, string>> chunkData(data.begin() + startIndex, data.begin() + endIndex);
          YearResult result = analyzeYearData(chunkData, xField, yField);

          lock_guard<mutex> lock(resultMutex);
          results[i] = result;
      });
  }

  for (thread& t : threads) {
      t.join();
  }

  YearResult finalResult;
  unordered_map<string,double> totalValues;
 
  for (const YearResult& result : results) {
      for (const auto& pair : result.totalValues) {
        totalValues[pair.first] += pair.second;
    }
  }
  double maxVal;
  double minVal;
  string maxKey;
  string minKey;
  auto it = totalValues.begin();
   
    if (it != totalValues.end()) {
        // Initialize max and min values with the first element
        maxVal = it->second;
        minVal = it->second;
        maxKey = it->first;
        minKey = it->first;

        // Iterate through the rest of the map
        ++it;
        for (; it != totalValues.end(); ++it) {
            if (it->second > maxVal) {
                maxVal = it->second;
                maxKey = it->first;
            }
            if (it->second < minVal) {
                minVal = it->second;
                minKey = it->first;
            }
        }
    }


  finalResult.maxValue = maxVal;
  finalResult.maxYear = maxKey;
  finalResult.minValue = minVal;
  finalResult.minYear = minKey;



  mvwprintw(menuwin, 3, 2, "%s has highest total %s of %f", finalResult.maxYear.c_str(), yField.c_str(), finalResult.maxValue);
  mvwprintw(menuwin, 4, 2, "%s has lowest total %s of %f", finalResult.minYear.c_str(), yField.c_str(), finalResult.minValue);

  // provideInsights(finalResult, yField);
}

void analyzeYearUnitsSoldMultithreaded(WINDOW* menuwin, const vector<unordered_map<string, string> >& data, const string& xField, const string& yField, int numThreads) {
  vector<thread> threads;
  vector<YearUnitsSoldResult> results(numThreads);
  int chunkSize = data.size() / numThreads;
  mutex resultMutex;
  int z = numThreads-1;

  for (int i = 0; i <= z; i++) {
      threads.emplace_back([i, chunkSize, &data, &xField, &yField, &results, &resultMutex, &z]() {
          int startIndex = i * chunkSize;
          int endIndex = (i == z) ? data.size() : (startIndex + chunkSize);
          vector<unordered_map<string, string>> chunkData(data.begin() + startIndex, data.begin() + endIndex);
          YearUnitsSoldResult result = analyzeYearUnitsSoldData(chunkData, xField, yField);

          lock_guard<mutex> lock(resultMutex);
          results[i] = result;
      });
  }

  for (thread& t : threads) {
      t.join();
  }

  YearUnitsSoldResult finalResult;
  unordered_map<string,int> totalValues;
 
  for (const YearUnitsSoldResult& result : results) {
      for (const auto& pair : result.totalValues) {
        totalValues[pair.first] += pair.second;
    }
  }
  int maxVal;
  int minVal;
  string maxKey;
  string minKey;
  auto it = totalValues.begin();
   
    if (it != totalValues.end()) {
        // Initialize max and min values with the first element
        maxVal = it->second;
        minVal = it->second;
        maxKey = it->first;
        minKey = it->first;

        // Iterate through the rest of the map
        ++it;
        for (; it != totalValues.end(); ++it) {
            if (it->second > maxVal) {
                maxVal = it->second;
                maxKey = it->first;
            }
            if (it->second < minVal) {
                minVal = it->second;
                minKey = it->first;
            }
        }
    }


  finalResult.maxValue = maxVal;
  finalResult.maxYear = maxKey;
  finalResult.minValue = minVal;
  finalResult.minYear = minKey;



  mvwprintw(menuwin, 3, 2, "%s has highest total %s of %f", finalResult.maxYear.c_str(), yField.c_str(), finalResult.maxValue);
  mvwprintw(menuwin, 4, 2, "%s has lowest total %s of %f", finalResult.minYear.c_str(), yField.c_str(), finalResult.minValue);

}


struct ItemSoldResult {
 unordered_map<string, int> itemSoldCounts;
 unordered_map<string, string> predominantItemTypes;
};
void printItemSoldInsights(WINDOW* menuwin, const ItemSoldResult& result, const string& xField) {
    int line = 3;
    if(xField != "Item Type"){
    for (const auto& pair : result.predominantItemTypes) {
        mvwprintw(menuwin, line++, 1, "%s predominantly sells %s.", pair.first.c_str(), pair.second.c_str());
    }
    }
    else{
    for (const auto& pair : result.predominantItemTypes) {
        mvwprintw(menuwin, line++, 1, "%s are predominantly sold in %s mode.", pair.first.c_str(), pair.second.c_str());
    }
    }
}

ItemSoldResult analyzeItemsSold(const vector<unordered_map<string, string> >& data, const string& xField, const string& yField) {
 ItemSoldResult result;

 unordered_map<string, int> itemSoldCounts;
 unordered_map<string, string> predominantItemTypes;

 for (const auto& row : data) {
 string xValue = row.at(xField);
 string yValue = row.at(yField);
 itemSoldCounts[xValue + " " + yValue]++;
 if (predominantItemTypes.count(xValue) == 0 || itemSoldCounts[xValue + " " + yValue] > itemSoldCounts[xValue + " " + predominantItemTypes[xValue]]) {
    predominantItemTypes[xValue] = yValue;
 }
 }

 result.itemSoldCounts = itemSoldCounts;
 result.predominantItemTypes = predominantItemTypes;

 return result;
}

void analyzeItemsSoldMultithreaded(WINDOW* menuwin, const vector<unordered_map<string, string> >& data, const string& xField, const string& yField, int numThreads) {
 vector<thread> threads;
 vector<ItemSoldResult> results(numThreads);
 int chunkSize = data.size() / numThreads;
 mutex resultMutex;
 int z = numThreads-1;

 for (int i = 0; i <= z; i++) {
   threads.emplace_back([i, chunkSize, &data, &xField, &yField, &results, &resultMutex, &z]() {
       int startIndex = i * chunkSize;
       int endIndex = (i == z) ? data.size() : (startIndex + chunkSize);
       vector<unordered_map<string, string>> chunkData(data.begin() + startIndex, data.begin() + endIndex);
       ItemSoldResult result = analyzeItemsSold(chunkData, xField, yField);

       lock_guard<mutex> lock(resultMutex);
       results[i] = result;
   });
 }

 for (thread& t : threads) {
   t.join();
 }

 ItemSoldResult finalResult;
 unordered_map<string,int> itemSoldCounts;
 unordered_map<string,string> predominantItemTypes;

 for (const ItemSoldResult& result : results) {
   for (const auto& pair : result.itemSoldCounts) {
       itemSoldCounts[pair.first] += pair.second;
   }
   for (const auto& pair : result.predominantItemTypes) {
       if (predominantItemTypes.count(pair.first) == 0 || itemSoldCounts[pair.first + " " + pair.second] > itemSoldCounts[pair.first + " " + predominantItemTypes[pair.first]]) {
           predominantItemTypes[pair.first] = pair.second;
       }
   }
 }

 finalResult.itemSoldCounts = itemSoldCounts;
 finalResult.predominantItemTypes = predominantItemTypes;

 printItemSoldInsights(menuwin, finalResult, xField);
}






int main(){
    
    initscr();
    noecho();
    cbreak();

    
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    printBanner();
    WINDOW* menuwin = newwin(yMax - 14, xMax - 14, 10, 10);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);

    keypad(menuwin, true);
    
    

    mainMenuTUI(menuwin);
    
    endwin();
    return 0;
}

bool checkCSVFileExists(const string& filename) {
   filesystem::path filePath(filename);
   if (filesystem::exists(filePath)) {
       return true;
   } else {
       return false;
   }
}


vector<pair<string, string>> getCSVInfo(const string& filename) {
 ifstream file(filename);
 if (!file.is_open()) {
   cerr << "Could not open the file - '" << filename << "'" << endl;
   return {};
 }

 string line;
 vector<pair<string, string>> columns;

 // Get the column names
 if (getline(file, line)) {
   istringstream lineStream(line);
   string column;
   while (getline(lineStream, column, ',')) {
       columns.push_back({column, ""});
   }
 }

 // Get the data types
 while (getline(file, line)) {
   istringstream lineStream(line);
   string data;
   int len = 0;
   for (auto& column : columns) {
        if (getline(lineStream, data, ',')) {
           // Check if the data can be converted to an integer or a float
           if ((data.find_first_not_of("0123456789.") == string::npos || len == 13)&&column.first != "Order ID") {
               column.second = "numeric";
           } 
           // Check if the data can be converted to a date
           else if (data.find_first_not_of("0123456789-:/ ") == string::npos) {
               column.second = "date";
           } 
           // Check if the data is a string
           else if(column.first != "Order Priority"){
               column.second = "string";
           }
        
       }
       len++;
   }
   break;
 }

 return columns;
}

void printBanner(){
    int num_cols = COLS;
    string str = " __  __       _ _   _     _______ _                        _          _   _____        _                               _           _     ";
    int padding_left = (num_cols / 2) - (str.size() / 2);

    mvprintw(1,padding_left,R"(  __  __       _ _   _     _______ _                        _          _   _____        _                               _           _     )");
    mvprintw(2,padding_left,R"( |  \/  |     | | | (_)   |__   __| |                      | |        | | |  __ \      | |            /\               | |         (_)    )");
    mvprintw(3,padding_left,R"( | \  / |_   _| | |_ _ ______| |  | |__  _ __ ___  __ _  __| | ___  __| | | |  | | __ _| |_ __ _     /  \   _ __   __ _| |_   _ ___ _ ___ )");
    mvprintw(4,padding_left,R"( | |\/| | | | | | __| |______| |  | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | | |  | |/ _` | __/ _` |   / /\ \ | '_ \ / _` | | | | / __| / __|)");
    mvprintw(5,padding_left,R"( | |  | | |_| | | |_| |      | |  | | | | | |  __| (_| | (_| |  __| (_| | | |__| | (_| | || (_| |  / ____ \| | | | (_| | | |_| \__ | \__ \)");
    mvprintw(6,padding_left,R"( |_|  |_|\__,_|_|\__|_|      |_|  |_| |_|_|  \___|\__,_|\__,_|\___|\__,_| |_____/ \__,_|\__\__,_| /_/    \_|_| |_|\__,_|_|\__, |___|_|___/)");
    mvprintw(7,padding_left,R"(                                                                                                                           __/ |          )");
    mvprintw(8,padding_left,R"(                                                                                                                          |___/           )");

}

void mainMenuTUI(WINDOW* menuwin){
    while(var == 0){
        
        for(int i = 0; i < 2; i++ ){
            if( i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+1, 1, mainMenu[i].c_str());
            wattroff(menuwin, A_REVERSE);
        }
        choice =  wgetch(menuwin);

        switch(choice){
            case KEY_UP:
                highlight--;
                if(highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight == 2)
                    highlight = 1;
                break;
            default:
                break;
        }
        if(choice == 10 && highlight == 0){
            
            wclear(menuwin);
            box(menuwin, 0, 0);
            echo();
            mvwprintw(menuwin,3 ,1, "Enter the file name to analyse data: ");
            wgetstr(menuwin, file);
            noecho();
            if(checkCSVFileExists(file)){
                columnTypes = getCSVInfo(file);
                var++;
            }
            else{
                mvwprintw(menuwin,5 ,3, "Entered file does not exists!! (Enter any key to restart)");
                wgetch(menuwin);
                wclear(menuwin);
                box(menuwin, 0, 0);
                continue;
            }

        }
        if(choice == 10 && highlight == 1)
            break;
    
    }
    if(choice == 10 && highlight == 0){
        
        subMenuTUI(menuwin);
    }

}

void subMenuTUI(WINDOW* menuwin){
    highlight = 0;
    wclear(menuwin);
    box(menuwin, 0, 0);
    while(var == 1){
        
        mvwprintw(menuwin, 1, 1, "Choose the type of Data Analysis you want to perform: ");
        for(int i = 0; i < 4; ++i){
            if(i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+2, 1, subMenu[i].c_str());
            wattroff(menuwin, A_REVERSE);

        }
        subchoice = wgetch(menuwin);

        switch (subchoice)
        {
        case KEY_UP:
            highlight--;
            if(highlight == -1)
                highlight = 0;
            break;
        case KEY_DOWN:
            highlight++;
            if(highlight == 4)
                highlight = 3;
            break;
        
        default:
            break;
        }

        if(subchoice == 10){
            if(highlight == 3){
                var--;
                mainMenuTUI(menuwin);
            }
            else{
                subvar = highlight + 1;
                var++;
            }
        }
    }
    if( subchoice == 10 && highlight != 3){
        highlight = 0;
        setColumns(menuwin);
    }
}


void setColumns(WINDOW* menuwin){
    if(var == 2){ 
        if(subvar == 1 && !flagD){
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "numeric")
                    Column1.push_back(columnTypes[i].first);
                
            }
            for (size_t i = 0; i < 3; ++i){
                Column2.push_back(columnTypes[i].first);
        
            }
            flagD = 1;
            Column1.push_back("Back");
            Column2.push_back("Back");
        }

        else if(subvar == 2 && !flagC){
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "string" && i>=2)
                    Column1.push_back(columnTypes[i].first);
            
            }


            for (size_t i = 0; i < 2; ++i){
            Column2.push_back(columnTypes[i].first);
        
            }

            flagC = 1;
            Column1.push_back("Back");
            
        }
        else if(subvar == 3  && !flagT){
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if((columnTypes[i].second == "numeric")&& i>=2 )
                    Column1.push_back(columnTypes[i].first);
            
            }

            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "date" )
                    Column2.push_back(columnTypes[i].first);
            }
            flagT = 1;
            Column1.push_back("Back");
            Column2.push_back("Back");
            
        }
        
    }
    choiceOfAnalysisTUI(menuwin);
}


void choiceOfAnalysisTUI(WINDOW* menuwin){
    highlight = 0;

    while(var == 2){
        wclear(menuwin);
        box(menuwin, 0, 0);
        string analysisColumn = "Select the column that you want to perform " + subMenu[subvar-1];
        mvwprintw(menuwin, 1, 1, analysisColumn.c_str() );

        for (int i = 0; i < (int)Column1.size(); ++i){
            if( i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+2, 1, Column1[i].c_str());
            wattroff(menuwin, A_REVERSE);
                
        }
        columnChoice = wgetch(menuwin);
        switch(columnChoice){
            case KEY_UP:
                highlight--;
                if(highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight == Column1.size())
                    highlight = Column1.size()-1;
                break;
            default:
                break;
        }
        if(columnChoice == 10){
            if(Column1[highlight] == "Back"){
                var--;
                subMenuTUI(menuwin);
            }
            else{
                columnVar1 = highlight;
                var++;
            }
        }
    }


    
    
    highlight = 0;
    if( subvar == 2 && columnVar1 == 1 && flagSales == 0){
            Column2.push_back("Item Type");
            flagSales = 1;
        }
    Column2.push_back("Back");
    while(var == 3){

        wclear(menuwin);
        box(menuwin, 0, 0);
        mvwprintw(menuwin, 1, 1, "Select the column with respect to which you want to perform the Analysis:" );
        for (int i = 0; i < Column2.size(); ++i){
            if( i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+2, 1, Column2[i].c_str());
            wattroff(menuwin, A_REVERSE);
                
        }
        wrtColumnChoice = wgetch(menuwin);
        switch(wrtColumnChoice){
            case KEY_UP:
                highlight--;
                if(highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight == Column2.size())
                    highlight = Column2.size()-1;
                break;
            default:
                break;
        }

        if(wrtColumnChoice == 10){
            if(Column2[highlight] == "Back"){
                var--;
                choiceOfAnalysisTUI(menuwin);
            }
            else{
                columnVar2 = highlight;
                var++;
            }
            
        }
    }

    if(wrtColumnChoice == 10 && Column2[highlight] != "Back")
        analysisTUI(menuwin);
}

void analysisTUI(WINDOW* menuwin){
    //if(var == 4 ){
    highlight = 0;
    /*wclear(menuwin);
    box(menuwin, 0, 0);
    vector<unordered_map<string, string> > data = readCSV(file);
    const auto& firstMap = data[0];
    const auto& firstPair = *firstMap.begin();*/
    while(var==4){
        wclear(menuwin);
        box(menuwin, 0, 0);
        mvwprintw(menuwin, 2,2, "Choose the number of threads: ");
        for(int i = 0; i < 4; ++i){
            if(i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+3, 1, "%d", i+1);
            wattroff(menuwin, A_REVERSE);
        }
        threadChoice =  wgetch(menuwin);

        switch(threadChoice){
            case KEY_UP:
                highlight--;
                if(highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight == 4)
                    highlight = 3;
                break;
            default:
                break;
        

        }
        if(threadChoice == 10){
            wclear(menuwin);
            box(menuwin, 0, 0);
            vector<unordered_map<string, string> > data = readCSV(file);
            const auto& firstMap = data[0];
            const auto& firstPair = *firstMap.begin();
            numThreads = highlight + 1;
            auto start = chrono::high_resolution_clock::now();
            if(subvar == 1){
                if(Column1[columnVar1] == R"(Total Profit
)"){
                    analyzeDataMultithreaded(menuwin, data, Column2[columnVar2], "Total Profits", numThreads);    
                }
                else{
                    analyzeDataMultithreaded(menuwin, data, Column2[columnVar2], Column1[columnVar1], numThreads);
                }
            }else if(subvar == 2){
                analyzeItemsSoldMultithreaded(menuwin, data,Column2[columnVar2], Column1[columnVar1],numThreads);
            }
            else if(subvar == 3){
                if(Column1[columnVar1] == "Unit Sold"){
                    analyzeYearUnitsSoldMultithreaded(menuwin, data, Column2[columnVar2], Column1[columnVar1], numThreads);
                }
                else{
                    analyzeYearMultithreaded(menuwin, data, Column2[columnVar2], Column1[columnVar1], numThreads);
                }

            }
            
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            mvwprintw(menuwin, 25,1,"Execution Time: %f seconds", elapsed.count());
            mvwprintw(menuwin, 30, 1,"{enter any key to exit}");
            wrefresh(menuwin);
            getch();
            var++;
        }

    }
}

