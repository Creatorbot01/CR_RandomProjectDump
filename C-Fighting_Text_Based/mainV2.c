//Final project: Text-Based Fighting Game

//List to do
/*
    - makeshift core function
        - debug terminal[x]
        - main menu[x]
        - basic terminal interaction(ie. clear terminal/play sound)[X]
        - randomizer function(lets go gambling)[X]

    - rpg function
        - damage calculation[x]
        - hp and mp function for player and enemies[x]
            - hp and mp variable need to be global[X]
        - level and exp [x]
        - player stats
            - dodge chance 
                - top stats {dodgestats}/100 * 50 = % dodge chance[x]
                - if random(1-100) < % then take no damage[x]
                - if random(1-100) = % then take minor damage (fun mechanic)(if fatal damage set hp = 1) [x]

    - combat mechanic
        - enemies behavior
            - modular stats[x]
            - make em fight[x]
            - make em heal[x]
            - make em steal[x]
        - fight[x]
        - heal[x]
        - recover[x]

Broken thing need to improve
-player one shotting enemies(somehow base damage is 800)[x][fixed]
-must use the additional damage increase[x][fixed]
-rework the damage calculation[x][fixed]
-find out why the health dip to negetive for enemies sometime[?][somehow fixed]
-find out how the fuck it goes out of bound on enemies selection[x][fixed]

Future me please finish writing the comment on each function in this c file.
*/

// Library

#include <stdio.h> //base io lib
#include <stdlib.h> //system library
#include <time.h> //time library
//#include <stdbool.h> //boolean library
#include <string.h> //library managing the string magic
#include <windows.h> //handle windows api cause i wanna be fancy mancy with color text
#include <math.h> //math function guh

// ANSI escape codes (color text for terminal)
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

#define BG_RED    "\x1B[41m"
#define BG_GREEN  "\x1B[42m"
#define BG_YELLOW "\x1B[43m"

// debug config(enabled,defaultHP,defaultMP,plrbaseattackmin,plrbaseattackmax,disableenemyautoselect)
int debugConfig[6] = {0,100,100,20,30,0};

//Core function
int DebugPrint(int operationType,const char *DialogRequest){
    switch (operationType)
    {
    case 1://something went wrong that barely effected stuff
        printf(YELLOW "[WARNING]: %s\n" RESET,DialogRequest); 
        break;
    case 2://something went wrong that actually bad
        printf(BG_RED WHITE "[ERROR]: %s\n" RESET,DialogRequest);
        getchar();
        exit(1);
        break;
    case 3://debug print
        if (debugConfig[0] == 1){
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

void clear(){ //clear the screen(keep player screen clean after mass printing)
    //this thing use command to clear the terminal and each os use different command
        #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        //if os is linux based or mac based
            system("clear");//run clear command
        #endif
        #if defined(_WIN32) || defined(_WIN64)
        //if os is windows based
            system("cls");//run cls command
        #endif
    }

char KeyboardPress(const char *DialogRequest){//get keyboard input
	char temp;
	if (DialogRequest && *DialogRequest != '\0') { //display extra dialog if needed
        printf("%s", DialogRequest);
    }
	#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    //if os is linux based or mac based
        #include <termios.h> //linux & mac library for system interface
    	#include <unistd.h>
    	
    	tcgetattr(STDIN_FILENO, &oldt);  // Get terminal settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);  // Disable buffering & echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        temp = getchar();  // Read one character
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore terminal settings
        return temp;
    #endif
    #if defined(_WIN32) || defined(_WIN64)
    //if os is windows based
        #include <conio.h> //window api library
        return getch();  // Works without Enter
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

//global variable

// Player Management Array
//HPMPArray = {HP,maxHP,MP,maxMP}
int HPMPPlayerArray[4] = {100,100,100,100};
//expLevel = {exp,MaxExp,Level}
int expLevel[3] = {0,10,1};
//kill = kill
int kill;
//PlayerStats = {VIT,INT,DEX,STR,BONUS} 
// VIT - increase hp | INT - increase mp | DEX - increase dodge change | STR - increase damage chance | BONUS - reallocation stats to 4 main stats
int PlayerStats[5] = {1,1,1,1,40};

// Enemies Management Array
//HPMPArray = {HP,maxHP,MP,maxMP}
int HPMPEnemiesArray[4] = {100,100,100,100};
// {Enemies base stats, increase in difficulty} basestats + extradiffstat = dmgMultiplier & dmgReduce
float EnemiesDamageMultiplier[2] = {0.0,0.0};
float EnemiesDamageReduction[2] = {0.0,0.0};
// basedamage array
int EnemiesBaseDamage[2] = {0,0};
//Enemiesbehaviour
int BehaviourATKHEAL[3] = {0,0,0};
int BehaviourManaSteal[3] = {0,0,0};
//EnemiesNameAndDescription
char Enemyname[40];
char Enemydesc[100];

//Enemy Dictionary
#define ENEMY 5
typedef struct {
    char name[40];  // Class name
    char desc[100]; // enemies description
    int mindamage;     // minimum Damage value
    int maxdamage;       // maximum damage value
    double BaseDMGReduc;   // control basic damage reduction
    double BaseDMGMultiplier; // control basic damage multiplier
    int Health; //enemies stats
    int Mana; //enemies mana
    int BehaviourModuleManagement[3]; //guhhh 100 - 50 - 25 (action between attack and heal)
    int ThresholdForManaSteal[3]; //steal player to replenish our own 100 - 50 - 25
} enemy;

enemy EnemyStore[ENEMY] = {
	{"Slime","regular slime.kinda weak.",10,20,0.0,0.0,50,250,{0,25,50},{0,30,90}},
	{"Big Slime","slime comeback but much more bigger.",10,20,0.0,0.0,400,100,{0,25,50},{0,30,90}},
    {"powerfull Slime","this one has muscle.",20,30,0.0,0.5,150,300,{0,25,50},{0,30,90}},
    {"Scary monster","he very scary.",30,40,0.2,0.0,500,60,{0,30,40},{0,20,80}},
    {"Metalic Coackroach","he got that medieval armor.",10,15,0.4,0.0,1000,100,{0,10,30},{0,10,60}},
};

//RPG Core Function
int gameover(){
    clear();
    printf(BG_GREEN WHITE "!!! YOU DIED !!! \n" RESET);
    printf("Your kill is %d \n",kill);
    printf("Final Stat:\n");
    printf("VIT = %d     INT = %d \nDEX = %d     STR = %d\n",PlayerStats[0],PlayerStats[1],PlayerStats[2],PlayerStats[3]);
    printf("Exprience = %d/%d Level = %d",expLevel[0],expLevel[1],expLevel[3]);
    char UIS = KeyboardPress("/nPress Y to restart the game or any key to exit programs\n");
    if (UIS == 'y'||UIS == 'Y'){
        clear();
        printf(GREEN "Game restart. Make sure to allocate stat before fighting\n" RESET);
        main();
    }else{
        exit(0);
    }
}

int explevelfunc(exp){
    int predictExp = expLevel[0] + exp;
    if (predictExp >= expLevel[1]){
        predictExp = predictExp - expLevel[1];
        expLevel[2] = expLevel[2] + 1;
        PlayerStats[4] = PlayerStats[4] + 10;
        expLevel[0] = 0;
        expLevel[1] = expLevel[1] + (int)ceil(expLevel[1]/2);
    }else{
        expLevel[0] = predictExp;
    }
    return 0;
}

int success(){
    clear();
    printf("%s successfully killed",Enemyname);
    getchar();
    kill = kill + 1;
    explevelfunc(20);
    int randDifChance = generateRandom(0,9);
    if (randDifChance < 3){
        if (EnemiesDamageMultiplier[1] < 0.5){
            DebugPrint(3,"Diff Increase Multiplier");
            EnemiesDamageMultiplier[1] = EnemiesDamageMultiplier[1] + 0.05;
            return 0;
        }
    }else if (randDifChance < 6){
            DebugPrint(3,"Diff Increase reduction");
        if (EnemiesDamageReduction[1] < 0.5){
            EnemiesDamageReduction[1] = EnemiesDamageReduction[1] + 0.05;
            return 0; 
        }
    }
    DebugPrint(3,"Diff skip");
    return 0;
}

int HPMPManagement(int operationType, int Value1, int Value2) {
    char buffer[50];

    switch (operationType) {
        case 1: { // Update maxHP
            printf("Increase maxHP\n");
            HPMPPlayerArray[1] = debugConfig[1] + PlayerStats[0]; 
            break;
        }
        case 2: { // Update maxMP
            printf("Increase maxMP\n");
            HPMPPlayerArray[3] = debugConfig[2] + PlayerStats[1];
            break;
        }
        case 3: { // Predict HP/MP reduction
            if (Value1 == 1) {
                DebugPrint(3, "predict hp");
                return (HPMPPlayerArray[0] >= Value2) ? 0 : 1;
            } else {
                DebugPrint(3, "predict MP");
                return (HPMPPlayerArray[2] >= Value2) ? 0 : 1;
            }
            break;
        }
        case 4: { // Reduce HP/MP
            if (Value1 == 1) {
                DebugPrint(3, "lowering hp");
                HPMPPlayerArray[0] -= Value2;
                if (HPMPPlayerArray[0] <= 0) {
                    DebugPrint(3, "player died");
                    gameover();
                    return 0;
                }
            } else {
                DebugPrint(3, "lowering MP");
                HPMPPlayerArray[2] -= Value2;
                if (HPMPPlayerArray[2] <= 0) {
                    DebugPrint(3, "player mana = 0");
                    gameover();
                    return 0;
                }
            }
            break;
        }
        case 5: { // Heal HP
            int healtotal = HPMPPlayerArray[0] + Value1;
            HPMPPlayerArray[0] = (healtotal >= HPMPPlayerArray[1]) ? HPMPPlayerArray[1] : healtotal;
            break;
        }
        case 6: { // Heal MP
            int healtotal = HPMPPlayerArray[2] + Value1;
            HPMPPlayerArray[2] = (healtotal >= HPMPPlayerArray[3]) ? HPMPPlayerArray[3] : healtotal;
            break;
        }
        default: { // Out of bound operation type
            snprintf(buffer, sizeof(buffer), "HPMPManagement operation type went out of bound: %d", operationType);
            DebugPrint(1, buffer);
            break;
        }
    }
    return 0;
}

int EnemiesManagement(int operationType,int Value1,int Value2){
    char buffer[50];
    int i;
    switch (operationType)
    {
    case 1:{//applies enemies to global variable(Value1 == enemiesID)
        if (EnemyStore[Value1].Health == 0){//check if enemies id exist
            return 0;//if not exist return 0
        }
        //applied health and mana to global var
        DebugPrint(3,"HPMP");
        HPMPEnemiesArray[0] = EnemyStore[Value1].Health;
        HPMPEnemiesArray[1] = EnemyStore[Value1].Health;
        HPMPEnemiesArray[2] = EnemyStore[Value1].Mana;
        HPMPEnemiesArray[3] = EnemyStore[Value1].Mana;
        Sleep(0.5);

        //aplies behaviour to global var
        DebugPrint(3,"Behaviour");
        memcpy(BehaviourATKHEAL, EnemyStore[Value1].BehaviourModuleManagement, sizeof(EnemyStore[Value1].BehaviourModuleManagement));
        memcpy(BehaviourManaSteal, EnemyStore[Value1].ThresholdForManaSteal, sizeof(EnemyStore[Value1].BehaviourModuleManagement));
        Sleep(0.5);

        //applies enemies stats to global var
        DebugPrint(3,"stats");
        EnemiesBaseDamage[0] = EnemyStore[Value1].mindamage;
        EnemiesBaseDamage[1] = EnemyStore[Value1].maxdamage;
        EnemiesDamageMultiplier[0] = EnemyStore[Value1].BaseDMGMultiplier;
        EnemiesDamageReduction[0] = EnemyStore[Value1].BaseDMGReduc;
        Sleep(0.5);

        //aplies enemies identity to global var
        DebugPrint(3,"identity");
        strcpy(Enemyname, EnemyStore[Value1].name);
        strcpy(Enemydesc, EnemyStore[Value1].desc);

        Sleep(0.5);

        DebugPrint(3,"Complete");
        return 1;
        break;
    }
    case 2:{
        DebugPrint(3,Enemyname);
        DebugPrint(3,Enemydesc);
        for (i = 0; i < 4; i++){
            snprintf(buffer, sizeof(buffer), "HPMPDebug/> %d", HPMPEnemiesArray[i]);
            DebugPrint(3,buffer);
        }
        for (i = 0; i < 3; i++)
        {
            snprintf(buffer, sizeof(buffer), "BehaviorBasic/> %d", BehaviourATKHEAL[i]);
            DebugPrint(3,buffer);
        }
        for (i = 0; i < 3; i++)
        {
            snprintf(buffer, sizeof(buffer), "BehaviorMana/> %d", BehaviourManaSteal[i]);
            DebugPrint(3,buffer);
        }
        for (i = 0; i < 2; i++)
        {
            snprintf(buffer, sizeof(buffer), "BaseDamage/> %d", EnemiesBaseDamage[i]);
            DebugPrint(3,buffer);
        }
        for (i = 0; i < 2; i++)
        {
            snprintf(buffer, sizeof(buffer), "Multiplier/> %d", EnemiesDamageMultiplier[i]);
            DebugPrint(3,buffer);
        }
        for (i = 0; i < 2; i++)
        {
            snprintf(buffer, sizeof(buffer), "Reduction/> %d", EnemiesDamageReduction[i]);
            DebugPrint(3,buffer);
        }
        break;
    }
    case 3:{// Calculate HP or MP reduce(Value1 = Hp/Mp , Value2 = howmuch)
        if (Value1 == 1){
            DebugPrint(3,"predict hp");
            if (HPMPEnemiesArray[0]>= Value2){
                DebugPrint(3,"enemies will survive");
                return 0;
            }else {
                DebugPrint(3,"enemies will died");
                return 1;
            }
        }else{
            DebugPrint(3,"predict MP");
            if (HPMPEnemiesArray[2]>= Value2){
                DebugPrint(3,"mana passed");
                return 0;
            }else{
                DebugPrint(3,"mana fail");
                return 1;
            }
        }
    }
    case 4:{//reduce HPMP (Value1 = HP/MP, value2 = howmuch)
        if (Value1 == 1){
            DebugPrint(3,"lowering hp");
            HPMPEnemiesArray[0] = HPMPEnemiesArray[0] - Value2;
            if (HPMPEnemiesArray[0]<= 0){
                DebugPrint(3,"enemies died");
            }
        }else{
            DebugPrint(3,"predict MP");
            HPMPEnemiesArray[2] = HPMPEnemiesArray[2] - Value2;
            if (HPMPEnemiesArray[2]<= 0){
                DebugPrint(3,"enemies mana = 0");
            }
        }
    }
    case 5: { // Heal HP
        int healtotal = HPMPEnemiesArray[0] + Value1;
        HPMPEnemiesArray[0] = (healtotal >= HPMPEnemiesArray[1]) ? HPMPEnemiesArray[1] : healtotal;
        break;
    }
    case 6: { // Heal MP
        int healtotal = HPMPEnemiesArray[2] + Value1;
        HPMPEnemiesArray[2] = (healtotal >= HPMPEnemiesArray[3]) ? HPMPEnemiesArray[3] : healtotal;
        break;
    }
    default:{// incase operation type went out of bound
        snprintf(buffer, sizeof(buffer), "HPMPManagement operation type went out of bound: %d", operationType);
        DebugPrint(1, buffer);
        break;
    }
    }
}

int playerstatsmanagement(int operationType,int Value1,int Value2){
    char buffer[40];
    switch (operationType)
    {
    case 1:{//allocation from free to actual stats(Value1 = stat,Value2 = amount of stats)
        PlayerStats[4] = PlayerStats[4] - Value2;
        PlayerStats[Value1] = PlayerStats[Value1] + Value2;
        HPMPManagement(1,0,0);
        HPMPManagement(2,0,0);
        break;
    }
    case 2:{
        if (PlayerStats[4] >= 1){
            DebugPrint(3,"player can upgrade stats");
            return 0;
        }else{
            DebugPrint(3,"player cant upgrade stats");
            return 1;
        }
    }
    case 3:{
        PlayerStats[4] = PlayerStats[4] + Value1;
        break;
    }
    default:{// incase operation type went out of bound
        snprintf(buffer, sizeof(buffer), "HPMPManagement operation type went out of bound: %d", operationType);
        DebugPrint(1, buffer);
        break;
    }
    }
    return 0;
}

//RPG Combat Mechanic

int DodgeChanceRandom(int dmg){
    int DexChance = PlayerStats[3]/100 * 50;
    int randomNumbergenerated = generateRandom(1,100);
    if (DexChance == randomNumbergenerated){// if dex equal to generated chance then cut damage to half
        DebugPrint(3,"small hit");
        int dmghalf = dmg / 2;
        int CheckForDeathTrigger = HPMPManagement(3,1,dmghalf);
        if (CheckForDeathTrigger == 1){
            DebugPrint(3,"Alert player died on this save em");
            return 3;
        }else{
            DebugPrint(3,"it fineeeee so half damage is good");
            return 2;
        }
    }else if (DexChance > randomNumbergenerated){
        DebugPrint(3,"success dodge chance");
        return 0;
    }else{
        DebugPrint(3,"dodge fail laugh at em");
        return 1;
    } 
}

int playerTakeDamage(float damage){
    float Truedamage = damage + (damage * (EnemiesDamageMultiplier[0] + EnemiesDamageMultiplier[1]));
    int TrueDamageINT = (int)ceil(Truedamage);

    int dodge = DodgeChanceRandom(TrueDamageINT);

    char buffer[30];
    snprintf(buffer, sizeof(buffer), "Damage output calculation: %d", TrueDamageINT);
    DebugPrint(3, buffer);
    if (dodge == 1){
        printf(RED "%s deal %d damage to you\n" RESET,Enemyname,TrueDamageINT);
        HPMPManagement(4,1,TrueDamageINT);
    }else if (dodge == 2){
        int dmghalf = TrueDamageINT/2;
        printf(YELLOW "%s deal %d damage to you but you barely dodge and only got %d amount of damage\n" RESET,Enemyname,TrueDamageINT,dmghalf);
        HPMPManagement(4,1,dmghalf);
    }else if (dodge == 3){
        int dmgtobarelycrit = HPMPPlayerArray[0] - 1;
        printf(YELLOW "%s deal %d damage to you. it almost kill you but you survive with 1 hp\n" RESET,Enemyname,TrueDamageINT);
        HPMPManagement(4,1,dmgtobarelycrit);
    }else{
        printf(GREEN "you dodge the %s attack \n" RESET,Enemyname);
        DebugPrint(3,"dodge crit damage reduc is yes");
    }
    return 0;
}

int enemiesTakeDamage(float damage){
    float scalingFactor = ((float)PlayerStats[3] / 20.0f) * 10.0f; // Ensure floating-point division
    float baseDamage = damage * scalingFactor;
    float totalDamageReduction = EnemiesDamageReduction[0] + EnemiesDamageReduction[1];
    totalDamageReduction = fminf(totalDamageReduction, 1.0f); // Ensure reduction doesn't exceed 100%
    float trueDamage = baseDamage * (1.0f - totalDamageReduction);
    int TrueDamageINT = (int)ceil(trueDamage);
    char buffer[30];
    printf(BLUE "You deal %d damage to %s\n" RESET,TrueDamageINT,Enemyname);
    EnemiesManagement(4,1,TrueDamageINT);
    return 0;
}

//debug function
int debugterminal(){
	char command[20];
	getUserStrInput("/Terminal> ", command, sizeof(command));
	if (strcasecmp(command, "test")==0){
		DebugPrint(4,">DebugTerminalHelloWorld\n");
	}else if(strcasecmp(command, "clr")==0){
		clear();
	}else if(strcasecmp(command, "exit")==0){
		DebugPrint(4,">Exiting terminal...\n");
        Sleep(2);
        clear();
		return 0;
	}else if(strcasecmp(command, "fe")==0){
        DebugPrint(4,"EnemiesMNG/> Insert Enemy Id: ");
		int eid = getUserIntInput(0);
		int result = EnemiesManagement(1,eid,0);
		if (result==1){ //make the function to appllied to global level future me
			EnemiesManagement(2,0,0);
		}else {
			DebugPrint(4,"EnemiesMNG/> [ERROR] ID Invalid\n");
		}
	}else if(strcasecmp(command, "crash")==0){
		DebugPrint(2,"Manual Crash Triggered Throught Terminal");
		return 0;
    }else if(strcasecmp(command, "dmgcalcplr")==0){
        float damage = 20.0;
        float Truedamage = damage + (damage * (EnemiesDamageMultiplier[0] + EnemiesDamageMultiplier[1]));
        int TrueDamageINT = (int)ceil(Truedamage);
        printf("TrueDamageINT: %d\n", TrueDamageINT);
    }else if(strcasecmp(command, "dmgcalcenemies")==0){
        float damage = 20.0;
        float scalingFactor = (PlayerStats[3] / 20.0f) * 10.0f; // Ensure floating-point division
        float baseDamage = damage * scalingFactor;
        float totalDamageReduction = EnemiesDamageReduction[0] + EnemiesDamageReduction[1];
        totalDamageReduction = fminf(totalDamageReduction, 1.0f); // Ensure reduction doesn't exceed 100%

        float trueDamage = baseDamage * (1.0f - totalDamageReduction);
        int TrueDamageINT = (int)ceil(trueDamage);

        printf("TrueDamageINT: %d\n", TrueDamageINT);
    }else if(strcasecmp(command, "printenemies")==0){
		EnemiesManagement(2,0,0);
    }else if(strcasecmp(command, "printplr")==0){
        int i;
        for (i = 0; i < 4 ; i++){
            printf("[HPMP] %d\n",HPMPPlayerArray[i]);
        }
    }else if(strcasecmp(command, "setdiffvar")==0){
		DebugPrint(4,"DiffVar/> set variable multiplier float: ");
        float cat;
        scanf("%.2f",&cat);
        EnemiesDamageMultiplier[1] = cat;
        DebugPrint(4,"DiffVar/> set variable reduction float: ");
        scanf("%.2f",&cat);
        EnemiesDamageMultiplier[1] = cat;
    }else if(strcasecmp(command, "godmode")==0){
        debugConfig[1] = 9999999;
        debugConfig[2] = 9999999;
        HPMPManagement(1,0,0);
        HPMPManagement(2,0,0);
        DebugPrint(4,"DiffVar/> Health set to 999999\n");
        DebugPrint(4,"DiffVar/> Mana set to 999999\n");
    }else if(strcasecmp(command, "se")==0){
        DebugPrint(4,"EnemiesMNG/> Insert Enemy Id: ");
		int eid = getUserIntInput(0);
		int result = EnemiesManagement(1,eid,0);
		if (result==1){
            debugConfig[5] = 1; 
			EnemiesManagement(2,0,0);
		}else {
			DebugPrint(4,"EnemiesMNG/> [ERROR] ID Invalid\n");
		}
	}
	debugterminal();
    return 0;
}


//game menu shit
void bar(){//peak lazziness be like
    printf("===========================================\n");
}

int fightEnemiesLogic(){
    int GenerateBehavior = generateRandom(1,100);
    int MPBehavior, HPBehavior;
    int getHpPercentage[3];
    int getMpPercentage[3];

    getHpPercentage[0] = (int)ceil(HPMPEnemiesArray[1] * 75/100);
    getHpPercentage[1] = (int)ceil(HPMPEnemiesArray[1] * 50/100);
    getHpPercentage[2] = (int)ceil(HPMPEnemiesArray[1] * 25/100);
    getMpPercentage[0] = (int)ceil(HPMPEnemiesArray[3] * 75/100);
    getMpPercentage[1] = (int)ceil(HPMPEnemiesArray[3] * 50/100);
    getMpPercentage[2] = (int)ceil(HPMPEnemiesArray[3] * 25/100);

    if (HPMPEnemiesArray[2]>=getMpPercentage[0]){
        MPBehavior = BehaviourManaSteal[0];
    }else if (HPMPEnemiesArray[2]>=getMpPercentage[1]){
        MPBehavior = BehaviourManaSteal[1];
    }else if (HPMPEnemiesArray[2]>=getMpPercentage[2]){
        MPBehavior = BehaviourManaSteal[2];
    }
    if (HPMPEnemiesArray[0]>=getHpPercentage[0]){
        HPBehavior = BehaviourATKHEAL[0];
    }else if (HPMPEnemiesArray[0]>=getHpPercentage[1]){
        HPBehavior = BehaviourATKHEAL[1];
    }else if (HPMPEnemiesArray[0]>=getHpPercentage[2]){
        HPBehavior = BehaviourATKHEAL[2];
    }

    if (MPBehavior > GenerateBehavior){
        DebugPrint(3,"enemies behavior : steal mp");
        HPMPManagement(4,2,20);
        EnemiesManagement(6,20,0);
    }else if (HPBehavior > GenerateBehavior){
        DebugPrint(3,"enemies behavior : heal hp");
        EnemiesManagement(4,2,20);
        EnemiesManagement(5,20,0);
    }else {
        DebugPrint(3,"enemies behavior : attack player");
        int damage = generateRandom(EnemiesBaseDamage[0],EnemiesBaseDamage[1]);
        playerTakeDamage((float)damage);
    }
    
    return 0;
}

int fightInterface(){
    //clear();
    DebugPrint(3,"fightloopstart");
    int loop = 1;
    while (loop == 1)
    {
        bar();
        printf(" > %s \n %s \n\n HP: %d   MP: %d \n",Enemyname,Enemydesc,HPMPEnemiesArray[0],HPMPEnemiesArray[2]);
        bar();
        printf("Your Stats\n");
        printf("HP: %d/%d    MP: %d/%d\n",HPMPPlayerArray[0],HPMPPlayerArray[1],HPMPPlayerArray[2],HPMPPlayerArray[3]);
        printf(RED" 1> Fight   "GREEN" 2> Heal HP   "CYAN" 3> Recover MP\n" RESET);
        char UIS;
        UIS = KeyboardPress(0);
        clear();
        if (UIS == '1'){
            DebugPrint(3,"fightfightfight");
            int damage = generateRandom(debugConfig[3],debugConfig[4]);
            enemiesTakeDamage((float)damage);
        }else if (UIS == '2'){
            DebugPrint(3,"Heal HP");
            HPMPManagement(4,2,20);
            HPMPManagement(5,20,0);
        }else if (UIS == '3'){
            DebugPrint(3,"Heal MP");
            HPMPManagement(6,20,0);
        }else{
            continue;
        }
        int HPEnemiesCheck = EnemiesManagement(3,1,0);
        int MPEnemiesCheck = EnemiesManagement(3,2,0);
        if ((HPEnemiesCheck == 1)||(MPEnemiesCheck == 1)){
            DebugPrint(3,"win condition triggered");
            success();
            return 0;
        }
        fightEnemiesLogic();
    }
    return 0;
}

int gamefightbegin(){
    DebugPrint(3,"begin fight function");
    if (debugConfig[5] == 0){
        int randomEnemyIDGen = generateRandom(0,(ENEMY-1));
        EnemiesManagement(1,randomEnemyIDGen,0);
        EnemiesManagement(2,0,0);
    }
    debugConfig[5] = 0;
    HPMPManagement(5,99999,0);
    HPMPManagement(6,99999,0);
    fightInterface();
    clear();
    return 0;
}


int statsmenu(){
    clear();
    bar();
    printf("<<<Player Status>>>\n");
    printf("1>VIT = %d    2>INT = %d \n",PlayerStats[0],PlayerStats[1]);
    printf("3>DEX = %d    4>STR = %d \n",PlayerStats[2],PlayerStats[3]);
    printf("Bonus Stats = %d\n\nto increase a stat select number of the stat to increase it. Each increase consume bonus stat. \nPress i for info of each stat and e to return to main menu.\n",PlayerStats[4]);
    bar();
    char UIS;
    UIS = KeyboardPress("<Insert Input>");
    int checkForValidUpgrade = playerstatsmanagement(2,0,0);
    if ((UIS == '1') && (checkForValidUpgrade == 0)){
        DebugPrint(3,"upgrade vit");
        playerstatsmanagement(1,0,1);
    }else if ((UIS == '2') && (checkForValidUpgrade == 0)){
        DebugPrint(3,"upgrade int");
        playerstatsmanagement(1,1,1);
    }else if ((UIS == '3') && (checkForValidUpgrade == 0)){
        DebugPrint(3,"upgrade dex");
        playerstatsmanagement(1,2,1);
    }else if ((UIS == '4') && (checkForValidUpgrade == 0)){
        DebugPrint(3,"upgrade str");
        playerstatsmanagement(1,3,1);
    }else if (((UIS == 'd')||(UIS == 'D')) && (debugConfig[0] == 1)){
        DebugPrint(3,"debug increase bonus stat");
        playerstatsmanagement(3,1,0);
    }else if ((UIS == 'i') || (UIS == 'I')){
        printf("1>VIT = increase hp    2>INT = increase mp \n");
        printf("3>DEX = increase dodge chance    4>STR = increase damage \n");
        printf("Press any key to return...");
        getchar();
    }else if ((UIS == 'e') || (UIS == 'E')){
        clear();
        DebugPrint(3,"return to main menu");
        return 0;
    }else if (((UIS == '1')||(UIS == '2')||(UIS == '3')||(UIS == '4'))&&checkForValidUpgrade == 1){
        printf(BG_YELLOW RED "You dont have enough bonus stat to increase your main stats" RESET);
        getchar();
    }else{
        DebugPrint(3,"non use input receive");
    }
    statsmenu();
}

int mainMenu(){
    bar();
    printf("<<Current Level and Experience>>\n");
    printf("Level: %d    Exp: %d/%d\n",expLevel[2],expLevel[0],expLevel[1]);
    bar();
    printf("Pick the action on what you would do...\n");
    printf(GREEN"1> Fight   "CYAN"2> Upgrade stats   " RED"3> Exit Game\n"RESET);
    bar();
    char UIS;
    UIS = KeyboardPress("<Insert Input>");
    int loopInput = 1;
    while (loopInput == 1)
    {
        if(UIS == '1'){
            clear();
            DebugPrint(3,"start combat scenario\n");
            loopInput = 0;
            gamefightbegin();
        }else if(UIS == '2'){
            DebugPrint(3,"show stats menu\n");
            loopInput = 0;
            statsmenu();
        }else if(UIS == '3'){
            clear();
            DebugPrint(3,"exit func trigger\n");
            printf(MAGENTA"Goodbye user...\n"RESET);
            loopInput = 0;
            getchar();
            exit(0);
        }else if (UIS == 'd'||UIS == 'D'){
            debugConfig[0] = 1;//enabled debug mode
            DebugPrint(1,"Debug Function Enabled. May result unstable situation.");
            DebugPrint(3,"Entering DebugTerminal.exe");
            Sleep(3);
            clear();
            debugterminal();
        }
        
    }
    mainMenu();
}

int main(){
    srand(time(0)); // Seed random number generator 
    enableANSI();  // Enable ANSI support
    printf(CYAN "[Message] Im sayin this but go and increase your stats before fight cause you will not last long without it.\n"RESET);
    mainMenu();
    getchar();
    return 0;
}
