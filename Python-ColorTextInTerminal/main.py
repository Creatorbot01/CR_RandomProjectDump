import sys, os, random, time, winsound

#ansi enabler
has_colors = sys.stdout.isatty()
if __name__ == "__main__":
    # Enable ANSI colors on Windows
    if sys.platform == "win32" and not has_colors:
        os.system("")

#ColorList
# FOREGROUND (TEXT) COLORS
BLACK     = "\033[30m"
RED       = "\033[31m"
GREEN     = "\033[32m"
YELLOW    = "\033[33m"
BLUE      = "\033[34m"
MAGENTA   = "\033[35m"
CYAN      = "\033[36m"
WHITE     = "\033[37m"
GRAY      = "\033[90m"      # Bright Black
BR_RED    = "\033[91m"      # Bright Red
BR_GREEN  = "\033[92m"      # Bright Green
BR_YELLOW = "\033[93m"      # Bright Yellow

# BACKGROUND COLORS
B_BLACK     = "\033[40m"
B_RED       = "\033[41m"
B_GREEN     = "\033[42m"
B_YELLOW    = "\033[43m"
B_BLUE      = "\033[44m"
B_MAGENTA   = "\033[45m"
B_CYAN      = "\033[46m"
B_WHITE     = "\033[47m"
B_GRAY      = "\033[100m"     # Bright Black Background
B_BR_RED    = "\033[101m"     # Bright Red Background
B_BR_GREEN  = "\033[102m"     # Bright Green Background
B_BR_YELLOW = "\033[103m"     # Bright Yellow Background

# TEXT FORMATTING
RESET       = "\033[0m"
BOLD        = "\033[1m"
DIM         = "\033[2m"       # Not widely supported(No window support)
ITALIC      = "\033[3m"       # Not widely supported(No window support)
UNDERLINE   = "\033[4m"
BLINK       = "\033[5m"       # Annoying, avoid (Mostly prob broken)
INVERSE     = "\033[7m"       # Swap FG/BG colors
HIDDEN      = "\033[8m"       # For passwords
STRIKE      = "\033[9m"

#Core Function
#Core - System Function
def Clear(): #clear the terminal
    if sys.platform == "win32":
        os.system("clear")
    else:
        os.system("cls")

def DebugPrint(OperationType, Text): #Debug print
    if OperationType == 1:
        print(f"{WHITE}{B_RED}[ERROR]:{Text}{RESET}")
    elif OperationType == 2:
        print(f"{GREEN}[DEBUG]:{Text}{RESET}")
    elif OperationType == 3:
        print(f"{YELLOW}[WARN]:{Text}{RESET}")
    else:
        print(f"{BLACK}{B_BR_RED}[ERROR]:DebugPrint OperationType went out of bound!{RESET}")

#sound function
def BEEP():
    winsound.Beep(1000,1000)
def CustomBeep(Hz,Time):
    MiliTime = Time * 1000
    winsound.Beep(Hz,int(MiliTime))

#Core - Variable check
def is_int(value): # check if value is int
    try:
        return float(value).is_integer()
    except (ValueError, TypeError):
        return False

def is_float(value):# check if value is float or int(since int can be float)
    if isinstance(value, (float, int)):
        return True
    try:
        float(value)
        return True
    except (ValueError, TypeError):
        return False

#Core - User Input Service
def UserINTInput(Text):#get INT value only tell user to repeat if user insert non int value
    lock = True
    while lock:
        if Text != 0:
            print(f"{Text}", end="")
        temp = input()
        if is_int(temp):
            lock = False
            return temp
        else:
            DebugPrint(3,"Input invalid. Input Not Int. Try again.")
            print("")
    
def UserFloatInput(Text):# same as above but for float
    lock = True
    while lock:
        if Text != 0:
            print(f"{Text}", end="")
        temp = input()
        if is_float(temp):
            lock = False
            return temp
        else:
            DebugPrint(3,"Input invalid. Input Not Float. Try again.")
            print("")

def Dirty_get_single_key():#the logic for keyboard press input fetch(yes you can get input from here but no)
    """Capture exactly one key press and return it"""
    if sys.platform == "win32":
        import msvcrt
        return msvcrt.getch().decode('utf-8')
    else:
        import termios, tty
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            return sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

def UserKeyboardInput(Text):# get user keyboard input
    if Text != 0:
        print(f"{Text}")
    temp = Dirty_get_single_key()
    if temp == '\x1b':  # ESC
        return "ESC"
    elif temp == '\x03':  # CTRL+C
        return "CTRL_C"
    elif temp == '\t':
        return "TAB"
    return temp


#Main code area(test code below)

DebugPrint(1,"This will be red error print with bg")
DebugPrint(2,"This will be green debug print")
DebugPrint(3,"This will be yellow warning print")

uis = UserINTInput(f"{GREEN}insert int: {RESET}")
print("user input =",uis)
uis = UserFloatInput(f"{GREEN}insert float: {RESET}")
print("user input =",uis)
uis = UserKeyboardInput(f"{GREEN}press any key {RESET}")
print("user input =",uis)
BEEP()
BEEP()
BEEP()
CustomBeep(2000,0.5)
