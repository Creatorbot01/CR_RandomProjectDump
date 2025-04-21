//Title: Resistor Calculator
//Description: calculate resistor based on color table
//hidin name here fuhhhh also incase my teacher see this hellooooo!!!



//Library section
#include <stdio.h> //base io lib
#include <stdlib.h> //system library
#include <time.h> //time library
#include <string.h> //library managing the string magic
#include <windows.h> //handle windows api cause i wanna be fancy mancy with color text
#include <math.h> //math function guh

// ANSI escape codes (color text for terminal)
#define RESET       "\x1B[0m"
#define BLACK       "\x1B[30m"
#define BROWN       "\x1B[38;2;139;69;19m"   // Custom RGB (saddle brown)
#define RED         "\x1B[31m"
#define ORANGE      "\x1B[38;2;255;165;0m"   // Custom RGB
#define YELLOW      "\x1B[33m"
#define GREEN       "\x1B[32m"
#define BLUE        "\x1B[34m"
#define VIOLET      "\x1B[35m"
#define GRAY        "\x1B[90m"
#define WHITE       "\x1B[97m"
#define GOLD        "\x1B[93m"               // Bright Yellow as Gold
#define SILVER      "\x1B[37m"               // Default white or adjust

#define BG_RED    "\x1B[41m"
#define BG_GREEN  "\x1B[42m"
#define BG_YELLOW "\x1B[43m"

// debug print
int debugConfig = 0;
int enabledClearFunc = 1; //control weither or not we erasing stuff....

//---- you float
#define EPSILON 0.0001f

//Core function (me reusing code from other project be like)
int DebugPrint(int operationType,const char *DialogRequest){
    switch (operationType)
    {
    case 1://something went wrong that barely effected stuff
        printf(YELLOW "[WARNING]: %s\n" RESET,DialogRequest); 
        break;
    case 2://something went wrong that actually bad
        printf(BG_RED WHITE "[ERROR]: %s\n" RESET,DialogRequest);
        //getchar();
        //exit(1);
        break;
    case 3://debug print
        if (debugConfig == 1){
            printf(GREEN "[DEBUG]: %s\n" RESET,DialogRequest);
        }
        break;
    case 4://debug terminal print
        printf(WHITE "/Terminal/%s" RESET,DialogRequest);
        break;
    default: //incase my dumbrain mess up debug print
        printf(YELLOW "[WARNING]: Debug print operationType went out of bound : %d \n" RESET,operationType);
        break;
    }
}

//chatgpt code on handlin window api to allow ansi- tldr: make text color glow wow oohh
// Define ENABLE_VIRTUAL_TERMINAL_PROCESSING manually if missing
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    
    if (hOut == INVALID_HANDLE_VALUE) return;
    
    // Get current console mode
    if (!GetConsoleMode(hOut, &dwMode)) return;
    
    // Enable ANSI escape sequences
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

int generateRandom(int min,int max){ //number randomizer
	return min + rand() %(max - min + 1);
}

int clear(){ //clear the screen(keep player screen clean after mass printing)
    //this thing use command to clear the terminal and each os use different command
        if(enabledClearFunc == 0){//if disabled clear function
            return 0;
        }
        #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        //if os is linux based or mac based
            system("clear");//run clear command
            return 0;
        #endif
        #if defined(_WIN32) || defined(_WIN64)
        //if os is windows based
            system("cls");//run cls command
            return 0;
        #endif
    }

    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)// some twisted library that dont wanna work together
        #include <termios.h>
        #include <unistd.h>
    #elif defined(_WIN32) || defined(_WIN64)
        #include <conio.h>
    #endif

char KeyboardPress(const char *DialogRequest) {//get user keyboard press only (char)
    char temp;

    if (DialogRequest && *DialogRequest != '\0') {
        printf("%s", DialogRequest);
    }

    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        temp = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return temp;
    #elif defined(_WIN32) || defined(_WIN64)
        return getch();
    #else
        return getchar(); // Fallback in case platform not detected
    #endif
}

//getUserStrInput("Extra dialog", Varname, sizeof(Varname)) example request call - for future me
void getUserStrInput(const char *DialogRequest, char *buffer, int size) { //get an entire string rather than keyboard input
    if (DialogRequest && *DialogRequest != '\0') { // Display extra dialog if needed
        	printf("%s", DialogRequest);
    }

    if (fgets(buffer, size, stdin)) {
        // Remove the newline character if present
        size_t len = 0;
        while (buffer[len] != '\0') len++; // Find the length
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Remove newline
        }
    } else {
        DebugPrint(1,"getUserStrInput: Error reading input.");
        buffer[0] = '\0'; // Ensure buffer is empty on error
    }
}

int getUserIntInput(const char *DialogRequest) { // get user input for integer only
    int num; //hold number
    char term; //buffer backup
    
    while (1) {
    	if (DialogRequest && *DialogRequest != '\0') { // Display extra dialog if needed
        	printf("%s", DialogRequest);
    	}
        if (scanf("%d%c", &num, &term) != 2 || term != '\n') {//get user input and check for non int
            printf("Error input is not integer. Try again.\n"); //if not int print out this
            while (getchar() != '\n');  // Clear input buffer
        } else {
            return num; //return int
        }
    }
}


// actual part responsible for it
// Color for resistor
#define colorAmount 13
typedef struct {
    char colorName[20];
    char Value;
    float Multiplier;
    float Tolerance;
    const char* ANSIColor;  
} colorRES;

colorRES ColorTable[colorAmount] = {
    {"Black",  '0', 1.0f,-1.0f, BLACK},
    {"Brown",  '1', 10.0f,1.0f,  BROWN},
    {"Red",    '2', 100.0f,2.0f,  RED},
    {"Orange", '3', 1000.0f,-1.0f, ORANGE},
    {"Yellow", '4', 10000.0f,-1.0f, YELLOW},
    {"Green",  '5', 100000.0f,0.5f, GREEN},
    {"Blue",   '6', 1000000.0f,0.25f, BLUE},
    {"Violet", '7', 10000000.0f,0.1f, VIOLET},
    {"Gray",   '8', 100000000.0f,0.05f, GRAY},
    {"White",  '9', 1000000000.0f,-1.0f, WHITE},
    {"Gold",   'X', 0.1f,5.0f,  GOLD},
    {"Silver", 'X', 0.01f,10.0f, SILVER},
    {"None",   'X', -1.0f,20.0f, RESET}  
};


//Global Variable
char band123[3];
int band1ID;
int band2ID;
int band3ID;
int multibandID;
int tolerancebandID;



void bar(){//peak laziness
    printf("=============================================\n");
}

int PrintLoopFunction(int OperationType){//function for printing option.
    int i;
    switch(OperationType)
    {
        case 1://first - third band
            for (i=0;i<13;i++){
                if (!(ColorTable[i].Value == 'X')){
                    printf("%d > %s\n",i+1,ColorTable[i].colorName);
                }
            }
            bar();
            break;
        case 2://multiplier band
            for (i = 0; i < 13; i++)
            {
                if (fabs(ColorTable[i].Multiplier - (-1.0f)) >= EPSILON) {
                    printf("%d > %s\n", i + 1, ColorTable[i].colorName);
                }                
            }
            bar();
            break;
        case 3://Tolerance band
        for (i = 0; i < 13; i++)
            {
                if (fabs(ColorTable[i].Tolerance - (-1.0f)) >= EPSILON) {
                    printf("%d > %s\n", i + 1, ColorTable[i].colorName);
                }                
            }
            bar();
            break;
        default:
            DebugPrint(1,"OperationType printloopfunction went out of bound.");
            break;
    }
}

int calculateResistor(){ //where we show result
    clear();
    char *end;
    long num = strtol(band123, &end, 10);//convert string to int(i miss python)
    bar();
    //a whole fancy ui that show the resistor
    printf("<<Resistor>>\n");
    printf("\n-==%s(%s)%s=",ColorTable[band1ID].ANSIColor,ColorTable[band1ID].colorName,RESET);
    if (band2ID != -1){
        printf("=%s(%s)%s=",ColorTable[band2ID].ANSIColor,ColorTable[band2ID].colorName,RESET);
    }if (band3ID != -1){
        printf("=%s(%s)%s=",ColorTable[band3ID].ANSIColor,ColorTable[band3ID].colorName,RESET);
    }
    printf("=%s(%s)%s=",ColorTable[multibandID].ANSIColor,ColorTable[multibandID].colorName,RESET);
    printf("=%s(%s)%s==-\n",ColorTable[tolerancebandID].ANSIColor,ColorTable[tolerancebandID].colorName,RESET);
    bar();
    //calculate resistance of the resistor
    float result = num * ColorTable[multibandID].Multiplier;
    printf("%d * %.2f = %.2f\n",num,ColorTable[multibandID].Multiplier,result);//show calculation
    printf(BG_GREEN WHITE "\nResult are %.0f +/- %.2f\n\n" RESET,result,ColorTable[tolerancebandID].Tolerance);//show result
    bar();
    return 0;
}

int validationOptionCheck(P_Value,OpsValue){ //check if user input is a valid option
    //printf("%d | %d\n",P_Value,OpsValue);
    //printf("%d | %d\n",P_Value >= 0,P_Value <= colorAmount);
    if (!(P_Value >= 0 && P_Value <= colorAmount)){//check if it within range like legit not 2000
        DebugPrint(2,"Out of bound option.");
        DebugPrint(2,"Press any key to continue...");
        KeyboardPress(0);
        return 1;
    }
    if (ColorTable[P_Value].Value == 'X' && OpsValue == 1){//first to third band check
        DebugPrint(2,"Option Invalid try again.");
        DebugPrint(2,"Press any key to continue...");
        KeyboardPress(0);
        return 1;
    }
    if (ColorTable[P_Value].Multiplier < 0 && OpsValue == 2){//multiplier check
        DebugPrint(2,"Multiplier Option Invalid try again.");
        DebugPrint(2,"Press any key to continue...");
        KeyboardPress(0);
        return 1;
    }
    if (ColorTable[P_Value].Tolerance < 0 && OpsValue == 3){//tolerance check
        DebugPrint(2,"Tolerance Option Invalid try again.");
        DebugPrint(2,"Press any key to continue...");
        KeyboardPress(0);
        return 1;
    }
    return 0;
}

int Picker(Loop){//the thing that pick the user input
    //reset to default
    memset(band123, '\0', sizeof(band123));
    band1ID = -1;
    band2ID = -1;
    band3ID = -1;
    multibandID = -1;
    tolerancebandID = -1; 

    //declare var
    int i;
    int picker;
    int validCheck = 0;
    int C_Band = 1;
    //band123 check
    while (!validCheck)
    {
        for (i=1;i<=Loop;i++){//band 123
            clear();
            //printf("%d",Loop);
            PrintLoopFunction(1);//print our option
            printf(BG_GREEN WHITE "Please pick your option for Band %d(Value band) :" RESET,C_Band);
            bar();
            picker = getUserIntInput(0)- 1;
            if (validationOptionCheck(picker,1) == 0){
                // Convert the char to a string
                char valueStr[2];  // One extra space for null terminator
                valueStr[0] = ColorTable[picker].Value;
                valueStr[1] = '\0';  // Null-terminate the string
                strcat(band123,valueStr);
                if (i==1){
                    C_Band = 2;
                    band1ID = picker;//pass the first time
                }else if (i==2){
                    C_Band = 3;
                    band2ID = picker;//pass the second time
                }else if (i==3){
                    C_Band = 4;
                    band3ID = picker;//pass the third time
                }
                if (i==Loop)
                {
                    validCheck = 1;//all pass and we can go to another section
                }
            }else {
                i = 1;
                C_Band = 1;
            }
        }
    }
    validCheck = 0;//reset check (reduce,reuse,recycle)

    //Multiplier check
    while (!validCheck)
    {
        clear();//MultiplierBand
        PrintLoopFunction(2);//print our option
        printf(BG_GREEN WHITE "Please pick your option for Band %d(Multiplier band) :" RESET,C_Band);
        picker = getUserIntInput(0)- 1;
        bar();
        //printf("%d",picker);
        if (validationOptionCheck(picker,2) == 0){//check success
            C_Band++;
            multibandID = picker;
            validCheck = 1;
        }
    }
    validCheck = 0;

    while (!validCheck)
    {
        clear();//ToleranceBand
        PrintLoopFunction(3);//print our option
        printf(BG_GREEN WHITE "Please pick your option for Band %d(Tolerance band) :" RESET,C_Band);
        picker = getUserIntInput(0)- 1;
        bar();
        if (validationOptionCheck(picker,3) == 0){//check success
            C_Band++;
            tolerancebandID = picker;
            validCheck = 1;
        }
    }
    calculateResistor();//goto calculate result shit.
    return 0;
}

int main(){
    enableANSI();//enable the fancy graphic
    //First Check How much Band
    int LoopForce = 1;//the first input prompt loop
    int LoopBandValue; //how many band we got
    int MAINEVILLOOPP = 1;//Main loop to reset back to the start
    printf(GREEN"Do you wish to have history preview?\n"RESET);//give option to disabled clear terminal function
    printf("enabling history preview allow you to scroll up to see previous input.\n");
    printf("if your screen is small it best for for you to disabled history preview.\n");
    printf(BG_RED WHITE"This only prompt once so please think before picking!!!"RESET);
    int BUI = getUserIntInput("[1 = YES, 2 = NO] : ");
    if (BUI == 1){
        enabledClearFunc = 0;//disabled clear
    }else{
        enabledClearFunc = 1;//enabled clear
    }
    while (MAINEVILLOOPP)
    {
        while (LoopForce){
            
            clear();//check how many band
            printf("1> 3 Band\n");
            printf("2> 4 Band\n");
            printf("3> 5 Band\n");
            bar();
            char UIS = 'a';
            int FUIS = 0; //emergency patches a hacky workaround...
            if (enabledClearFunc == 1){
                UIS = KeyboardPress("<Please Pick Total Band Amount>");//clear enabled
            }else{
                FUIS = getUserIntInput("<Please Pick Total Band Amount> : ");//clear disabled
            }
            if (UIS=='1'||FUIS == 1){
                LoopForce = 0;
                LoopBandValue = 1;
            }else if (UIS=='2'||FUIS == 2){
                LoopForce = 0;
                LoopBandValue = 2;
            }else if (UIS=='3'||FUIS == 3){
                LoopForce = 0;
                LoopBandValue = 3;
            }else{
                DebugPrint(1,"Out of bound option. Try Again");
                KeyboardPress(0);
            }
        }
        Picker(LoopBandValue);
        int complicatinglifechoice = 1;
        while (complicatinglifechoice)//when all done ask user to either quit or continue
        {
            char UIS = 'a';
            int FUIS = 0; //still not proud of this workaround but it work
            if (enabledClearFunc == 1){
                UIS = KeyboardPress("Exit Programs[Y/N]: ");//clear enabled
            }else{
                FUIS = getUserIntInput("Exit Programs[1 = YES/2 = NO]: ");//clear disabled
            }
            if (UIS == 'y'||UIS == 'Y'||FUIS == 1){//exit programs
                complicatinglifechoice = 0;
                MAINEVILLOOPP = 0;
                clear();
                printf(YELLOW"Exiting Program...\n"RESET);
                Sleep(1);
                printf(RED"\nGoodbye user.."RESET);
                printf(RED"\n[Press any key to close the program]"RESET);
                KeyboardPress(0);
                exit(0);//THE RED BAD SWITCH
            }else if (UIS == 'n'||UIS == 'N'||FUIS == 2){//reset programs
                complicatinglifechoice = 0;
                LoopForce = 1;
                clear();
                printf(GREEN "\nRestarting...\n" RESET);
                Sleep(3);
            }  
        }
    }
    return 0;
}
