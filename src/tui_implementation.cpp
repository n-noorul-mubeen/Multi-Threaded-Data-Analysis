#include "includes/MultiThreadedDataAnalysis.hpp"


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

void printBanner();
void mainMenuTUI(WINDOW* menuwin);
void subMenuTUI(WINDOW* menuwin);
void setColumns(WINDOW* menuwin);
void choiceOfAnalysisTUI(WINDOW* menuwin);
void analysisTUI(WINDOW* menuwin);


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
    highlight = 0;
    wclear(menuwin);
    box(menuwin, 0, 0);
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
            Column1.clear();
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "numeric")
                    Column1.push_back(columnTypes[i].first);
                
            }

            Column2.clear();
            for (size_t i = 0; i < 3; ++i){
                Column2.push_back(columnTypes[i].first);
        
            }
            flagD = 1;
            Column1.push_back("Back");
            Column2.push_back("Back");
        }

        else if(subvar == 2 && !flagC){
            Column1.clear();
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "string" && i>=2)
                    Column1.push_back(columnTypes[i].first);
            
            }

            Column2.clear();
            for (size_t i = 0; i < 2; ++i){
                Column2.push_back(columnTypes[i].first);
        
            }

            flagC = 1;
            Column1.push_back("Back");
            
        }
        else if(subvar == 3  && !flagT){
            Column1.clear();
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if((columnTypes[i].second == "numeric")&& i>=2 )
                    Column1.push_back(columnTypes[i].first);
            
            }

            Column2.clear();
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
    
    if( subvar == 2 && columnVar1 == 1){
        Column2.push_back("Item Type");
        Column2.push_back("Back");
        flagSales = 1;
    }
        
    
    
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
    
    highlight = 0;
    
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
                analyzeDataMultithreaded(menuwin, data, Column2[columnVar2], Column1[columnVar1], numThreads);
            }
            else if(subvar == 2){
                analyzeItemsSoldMultithreaded(menuwin, data ,Column2[columnVar2], Column1[columnVar1],numThreads);
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

