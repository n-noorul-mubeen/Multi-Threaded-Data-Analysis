#include <ncurses.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

bool checkCSVFileExists(const string& filename) {
   std::filesystem::path filePath(filename);
   if (std::filesystem::exists(filePath)) {
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
   for (auto& column : columns) {
       if (getline(lineStream, data, ',')) {
           // Check if the data can be converted to an integer or a float
           if (data.find_first_not_of("0123456789.") == string::npos) {
               column.second = "numeric";
           } 
           // Check if the data can be converted to a date
           else if (data.find_first_not_of("0123456789-:/ ") == string::npos) {
               column.second = "date";
           } 
           // Check if the data is a string
           else {
               column.second = "string";
           }
       }
   }
   break;
 }

 return columns;
}




int main(){
    
    initscr();
    noecho();
    cbreak();

    mvprintw(1,1,R"(  __  __       _ _   _     _______ _                        _          _   _____        _                               _           _     )");
    mvprintw(2,1,R"( |  \/  |     | | | (_)   |__   __| |                      | |        | | |  __ \      | |            /\               | |         (_)    )");
    mvprintw(3,1,R"( | \  / |_   _| | |_ _ ______| |  | |__  _ __ ___  __ _  __| | ___  __| | | |  | | __ _| |_ __ _     /  \   _ __   __ _| |_   _ ___ _ ___ )");
    mvprintw(4,1,R"( | |\/| | | | | | __| |______| |  | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | | |  | |/ _` | __/ _` |   / /\ \ | '_ \ / _` | | | | / __| / __|)");
    mvprintw(5,1,R"( | |  | | |_| | | |_| |      | |  | | | | | |  __| (_| | (_| |  __| (_| | | |__| | (_| | || (_| |  / ____ \| | | | (_| | | |_| \__ | \__ \)");
    mvprintw(6,1,R"( |_|  |_|\__,_|_|\__|_|      |_|  |_| |_|_|  \___|\__,_|\__,_|\___|\__,_| |_____/ \__,_|\__\__,_| /_/    \_|_| |_|\__,_|_|\__, |___|_|___/)");
    mvprintw(7,1,R"(                                                                                                                           __/ |          )");
    mvprintw(8,1,R"(                                                                                                                          |___/           )");

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW* menuwin = newwin(yMax - 14, xMax - 14, 10, 10);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);

    keypad(menuwin, true);
    

    string mainMenu[2] = {"Start", "Exit"};
    string subMenu[3] = {"Descriptive Analysis", "Categorical Data Analysis", "Time Based Data Analysis"};
    int choice;
    int highlight = 0;
    int var = 0;
    int subvar = 0;
    char file[100];
    int subchoice;
    vector<pair<string, string>> columnTypes;

    int columnChoice;
    int wrtColumnChoice;

    int columnVar1 = -1;
    int columnVar2 = -1;

    vector<string> Column1;
    vector<string> Column2;
    

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


    highlight = 0;
    while(var == 1){
        wclear(menuwin);
        box(menuwin, 0, 0);
        mvwprintw(menuwin, 1, 1, "Choose the type of Data Analysis you want to perform: ");
        for(int i = 0; i < 3; ++i){
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
            if(highlight == 3)
                highlight = 2;
            break;
        
        default:
            break;
        }

        if(subchoice == 10){
            subvar = highlight + 1;
            var++;
        }
    }
    highlight = 0;

    if(var == 2){
        if(subvar == 1 || subvar == 2 ){
            for (size_t i = 0; i < 2; ++i){
                Column2.push_back(columnTypes[i].first);
            
            }
            if(subvar == 1){
                for (size_t i = 0; i < columnTypes.size(); ++i){
                    if(columnTypes[i].second == "numeric")
                        Column1.push_back(columnTypes[i].first);
                    
                }
            }
            else{
                for (size_t i = 0; i < columnTypes.size(); ++i){
                    if(columnTypes[i].second == "string" && i>=2)
                        Column1.push_back(columnTypes[i].first);
                
            }
            }
        }
        
        else if(subvar == 3){
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if((columnTypes[i].second == "string" || columnTypes[i].second == "numeric")&& i>=2 )
                    Column1.push_back(columnTypes[i].first);
            
            }
            for (size_t i = 0; i < columnTypes.size(); ++i){
                if(columnTypes[i].second == "date" )
                    Column2.push_back(columnTypes[i].first);
            }
            
        }

        
    }

    while(var == 2 && subvar ==1){

        wclear(menuwin);
        box(menuwin, 0, 0); 
        

        mvwprintw(menuwin, 1, 1, "Select the column that you want to perform Descriptive Analysis:" );

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
            columnVar1 = highlight;
            var++;
        }

    }
    while(var == 2 && subvar ==2){
        wclear(menuwin);
        box(menuwin, 0, 0); 
        
        mvwprintw(menuwin, 1, 1, "Select the column that you want to perform Categorical Data Analysis:" );

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
            columnVar1 = highlight;
            var++;
        }
    }

   

    while(var == 2 && subvar == 3){
        wclear(menuwin);
        box(menuwin, 0, 0);
        

        mvwprintw(menuwin, 1, 1, "Select the column that you want to perform Time Related Data Analysis:" );

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
            columnVar1 = highlight;
            var++;
        }
    }

     highlight = 0;

    while(var == 3 && (subvar == 1 || subvar == 2)){
        wclear(menuwin);
        box(menuwin, 0, 0);

        

        
        mvwprintw(menuwin, 1, 1, "Select the column with respect to which you want to perform the Analysis:" );
        for (int i = 0; i < 2; ++i){
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
                if(highlight == 2)
                    highlight = 1;
                break;
            default:
                break;
        }

        if(wrtColumnChoice == 10){
            columnVar2 = highlight;
            var++;
        }
    }

    while(var == 3 && subvar == 3){
        wclear(menuwin);
        box(menuwin, 0, 0); 
        

        
        for (int i = 0; i < (int)Column2.size(); ++i){
                if( i == highlight)
                    wattron(menuwin, A_REVERSE);
                mvwprintw(menuwin, i+2, 1, Column2[i].c_str());
                wattroff(menuwin, A_REVERSE);
        }
                    

        mvwprintw(menuwin, 1, 1, "Select the column with respect to which you want to perform the Analysis:" );

        
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
            columnVar2 = highlight;
            var++;
        }
    }

    
    endwin();
    return 0;
}

