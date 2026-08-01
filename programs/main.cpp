#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "asciipp_engine.h"

namespace {

constexpr int kLevelOneUIHeight = 8;
constexpr int kLevelOneSideWidth = 76;
constexpr int kLevelOneTimerSeconds = 240;

constexpr int kLevelTwoUIHeight = 9;
constexpr int kLevelTwoSideWidth = 43;
constexpr int kLevelTwoTimerSeconds = 420;
constexpr int kEnemyCollisionDamage = 12;
constexpr int kMirrorPuzzleDamage = 12;
constexpr int kFinalPuzzleDamage = 35;

struct GameSession {
  int hp = 100;
  bool startLevelTwo = false;
};

struct LevelOneState {
  bool signalTriggered = false;
  int signalScore = 0;
};

struct LevelTwoState {
  bool mirrorSolved = false;
  bool finalPuzzleSolved = false;
  bool endingTriggered = false;
  bool scriptQueued = false;
  int destroyedEnemies = 0;
  int puzzleScore = 0;
  std::string notice;
  float noticeTimer = 0.0f;
};

struct LevelTwoEnemyConfig {
  int x;
  int y;
  int behavior;
  char patrolAxis;
};

std::string normalizeLower(std::string value) {
  for (char &c : value) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return value;
}

std::pair<int, int> findTile(const std::vector<std::string> &mapData,
                             char tile) {
  for (int y = 0; y < static_cast<int>(mapData.size()); ++y) {
    for (int x = 0; x < static_cast<int>(mapData[y].size()); ++x) {
      if (mapData[y][x] == tile) {
        return {x, y};
      }
    }
  }

  return {-1, -1};
}

std::vector<std::string> buildLevelOneMap() {
  return {
      "########################################################################"
      "#######",
      "#                  #  S #     #   #                 #       #   #       "
      "  #   #",
      "#                  #  # # ### # # # ### ########### # # # ### # # # "
      "##### # # #",
      "#                  #  # #   # # #   #   #   #       # # # #   #   #   # "
      "  # # #",
      "#                  #  # ### # # ##### ##### # ######### ### ######### "
      "### ### #",
      "#                  #  # #   # # #     #     #           #   #   #   #   "
      "#   # #",
      "#                  #  # # ### # # ##### # ### ######### # ##### # # ### "
      "### # #",
      "#                  #  #   #   # # #   # #     #       # # #   #   #   # "
      "  #   #",
      "#                  #  ##### ### # # # # ####### ##### ### # # ####### "
      "### ### #",
      "#                  #  #   # #   #   # # #     #   # #   #   #           "
      "# # # #",
      "#                  #  # ### # ####### # # ### ### # ### ############### "
      "# # # #",
      "#                  #  #     # #   # # # # # #     #   #         #       "
      "# # # #",
      "#                  #  # ##### # # # # ### # ######### ### ##### # "
      "####### # # #",
      "#                  #  #   # #   # # #     # #           #   #   # #     "
      "# #   #",
      "#                  #  ### # ##### # ####### # ### ##### ### ##### # ### "
      "# ### #",
      "#                  #  # # # #     #       # #   # #   #   #       # #   "
      "#   # #",
      "#                  #  # # # # ######### # # ### # # # ############# "
      "##### # # #",
      "#                  #  #   #     #       # #   # #   # #         #   #   "
      "# # # #",
      "#                  #  # ####### # # ##### ### # ##### # ##### # ### # # "
      "### ###",
      "#                  #  #     #   # #   #     #   #   # #   #   #     # # "
      "  #   #",
      "#                  #  ##### # ####### ##### ### ### # # ### ######### "
      "### ### #",
      "#                  #  #     #   #     #   #   #     #   #   #   #   # # "
      "#   # #",
      "#                  #  # ####### # ##### # ### ########### ### # # # # # "
      "### # #",
      "#                  #  # #     # #     # # # #     #     # #   # # #   # "
      "    # #",
      "#                  #  # ### ### ##### # # # ##### # ### # # ### # ##### "
      "##### #",
      "#                  #  #   #           # # #         #     # #     #   # "
      "    # #",
      "#          #########  ### ### ######### # ########### ##### ####### "
      "####### # #",
      "#                     # #   # #   #     #         #   #   #   #       # "
      "  #   #",
      "#                     # ### ### # # ############# ##### # ### # ##### # "
      "# ### #",
      "#                     #         #   #                   #     #     #   "
      "#     D",
      "########################################################################"
      "#######",
  };
}

std::vector<std::string> buildLevelTwoMap() {
  return {
      "########################################################################"
      "########################################################################"
      "################",
      "#.........#.........#...................#..............................."
      "........#...................................#..........................."
      "...............#",
      "#.........#.........#...................#..............................."
      "........#..............................................................."
      "...............#",
      "#.........#.........#...................#..............................."
      "........#...................................#..........................."
      "...............#",
      "#....######.........#####################.........######################"
      "#########...................############################################"
      "################",
      "#......................................................................."
      "........#...................#..........................................."
      "...............#",
      "#......................................................................."
      "........#...................#..........................................."
      "...............#",
      "#......................................................................."
      "........#...................#...................#......................."
      "...............#",
      "############################################################............"
      "........#...................#...................#......................."
      "...............#",
      "#..........................................................#............"
      "........#...................#..........................................."
      "...............D",
      "#......................................................................."
      "........#...................#...................#......................."
      "...............#",
      "#..........................................................#............"
      "........#...................#...................#......................."
      "...............#",
      "#...........################################################............"
      "........##########..#########...................#......................."
      "...............#",
      "#...........#..........................................................."
      "............................#...................#......................."
      "...............#",
      "#...........#..........................................................."
      "............................#..........................................."
      "...............#",
      "#...........#..........................................................."
      "............................#..........................................."
      "...............#",
      "#...........############################################################"
      "#############################..........................................."
      "...............#",
      "#.######################################################################"
      "########################################################################"
      "################",
      "#..#####################################################################"
      "########################################################################"
      "################",
      "##...###################################################################"
      "########################################################################"
      "################",
      "####.###################################################################"
      "########################################################################"
      "################",
      "###..###################################################################"
      "########################################################################"
      "################",
      "##..####################################################################"
      "########################################################################"
      "################",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#....#############@#####################################################"
      "########################################################################"
      "################",
      "#.................@.........@......#####................................"
      "........................................................................"
      "...............D",
      "#....#############@#####################################################"
      "########################################################################"
      "################",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "#.................@....................................................."
      "........................................................................"
      "...............#",
      "########################################################################"
      "########################################################################"
      "################",
  };
}

std::vector<LevelTwoEnemyConfig> buildLevelTwoEnemies() {
  return {
      {15, 2, 1, 'x'},  {50, 2, 0, 'x'},   {70, 2, 1, 'x'},   {90, 2, 2, 'x'},
      {136, 2, 1, 'x'}, {14, 4, 1, 'y'},   {153, 5, 0, 'x'},  {11, 6, 1, 'y'},
      {51, 6, 0, 'x'},  {90, 6, 1, 'x'},   {120, 6, 1, 'y'},  {142, 8, 0, 'x'},
      {20, 9, 1, 'x'},  {69, 9, 0, 'x'},   {90, 9, 1, 'x'},   {110, 9, 1, 'x'},
      {151, 9, 0, 'x'}, {141, 12, 1, 'y'}, {59, 13, 0, 'x'},  {36, 14, 2, 'x'},
      {76, 14, 1, 'x'}, {120, 14, 0, 'x'}, {150, 16, 2, 'x'},
  };
}

std::vector<ColoredArtLine> buildLevelOneSideArt() {
  return {
      {{{"    "
         "888888888888888888888888888888888888888888888888888888888888888888888"
         "88",
         Color::GREEN}}},
      {{{"    88.._|      | `-.  | `.  -_-_ _-_  _-  _- -_ -  .'|   |.'|     | "
         " "
         "_..88",
         Color::GREEN}}},
      {{{"    88   `-.._  |    |`!  |`.  -_ -__ -_ _- _-_-  .'  |.;'   |   "
         "_.!-'|  "
         "88",
         Color::GREEN}}},
      {{{"    88      | `-!._  |  `;!  ;. _______________ ,'| .-' |   _!.i'    "
         " |  "
         "88",
         Color::GREEN}}},
      {{{"    88..__  |     |`-!._ | `.| |_______________||.\"'|  _!.;'   |    "
         " "
         "_|..88",
         Color::GREEN}}},
      {{{"    88   |``\"..__ |    |`\";.| i|_|MMMMMMMMMMM|_|'| _!-|   |   "
         "_|..-|'  "
         "  88",
         Color::GREEN}}},
      {{{"    88   |      |``--..|_ | `;!|l|MMoMMMMoMMM|1|.'j   |_..!-'|     | "
         "    "
         "88",
         Color::GREEN}}},
      {{{"    88   |      |    |   |`-,!_|_|MMMMP'YMMMM|_||.!-;'  |    |     | "
         "    "
         "88",
         Color::GREEN}}},
      {{{"    88___|______|____!.,.!,.!,!|d|MMMo * "
         "loMM|p|,!,.!.,.!..__|_____|_____88",
         Color::GREEN}}},
      {{{"    88      |     |    |  |  | |_|MMMMb,dMMMM|_|| |   |   |    |     "
         " |  "
         "88",
         Color::GREEN}}},
      {{{"    88      |     |    |..!-;'i|r|MPYMoMMMMoM|r| |`-..|   |    |     "
         " |  "
         "88",
         Color::GREEN}}},
      {{{"    88      |    _!.-j'  | _!,\"|_|M<>MMMMoMMM|_||!._|  `i-!.._ |    "
         "  | "
         " 88",
         Color::GREEN}}},
      {{{"    88     _!.-'|    | _.\"|  !;|1|MbdMMoMMMMM|l|`.| `-._|    "
         "|``-.._  | "
         " 88",
         Color::GREEN}}},
      {{{"    88..-i'     |  _.''|  !-| !|_|MMMoMMMMoMM|_|.|`-. | ``._ |     "
         "|``\"..88",
         Color::GREEN}}},
      {{{"    88   |      |.|    |.|  !| |u|MoMMMMoMMMM|n||`. |`!   | `\".    "
         "|    "
         " 88",
         Color::GREEN}}},
      {{{"    88   |  _.-'  |  .'  |.' |/|_|MMMMoMMMMoM|_|! |`!  `,.|    |-._| "
         "    "
         "88",
         Color::GREEN}}},
      {{{"    88  _!\"'|     !.'|  .'| .'|[@]MMMMMMMMMMM[@] \\|  `. | `._  |   "
         "`-._  88",
         Color::GREEN}}},
      {{{"    88-'    |   .'   |.|  |/| /                 \\|`.  |`!    |.|    "
         "  "
         "|`-88",
         Color::GREEN}}},
      {{{"    88      |_.'|   .' | .' |/                   \\  \\ |  `.  | `._ "
         "   "
         "|  88",
         Color::GREEN}}},
      {{{"    88     .'   | .'   |/|  /                     \\ |`!   |`.|    "
         "`.  | "
         " 88",
         Color::GREEN}}},
      {{{"    88  _.'     !'|   .' | /                       \\|  `  |  `.    "
         "|`.| "
         " 88",
         Color::GREEN}}},
      {{{"    "
         "888888888888888888888888888888888888888888888888888888888888888888888"
         "88",
         Color::GREEN}}},
  };
}

std::vector<ColoredArtLine> buildLevelTwoSideArt() {
  return {
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"                                       ", Color::BRIGHT_BLACK}}},
      {{{"  .-----------------------------------.", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"\\                           /    ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {" \\                         /     ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"  \\                       /      ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]                     [    ,'|", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]                     [   /  |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]___               ___[ ,'   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]\\             /[  [ |:   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ] \\           / [  [ |:   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ]         [  [  [ |:   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ]__     __[  [  [ |:   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ] ]\\ _ /[ [  [  [ |:   |", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ] ]", Color::GREEN},
        {" (#) ", Color::CYAN},
        {"[ [  [  [ :===='", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ]_]", Color::GREEN},
        {".nHn.", Color::CYAN},
        {"[_[  [  [       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ]  ]", Color::GREEN},
        {"  HHHHH. ", Color::CYAN},
        {"[  [  [       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]  ] /", Color::GREEN},
        {"   `HH(\"N", Color::CYAN},
        {"  \\ [  [       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]__]/", Color::GREEN},
        {"     HHH  \"", Color::CYAN},
        {"  \\[__[       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]", Color::GREEN},
        {"         NNN         ", Color::CYAN},
        {"[       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]", Color::GREEN},
        {"         N/\"         ", Color::CYAN},
        {"[       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"   ]", Color::GREEN},
        {"         N H         ", Color::CYAN},
        {"[       ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"  /", Color::GREEN},
        {"          N            ", Color::CYAN},
        {"\\      ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {" /", Color::GREEN},
        {"           q,            ", Color::CYAN},
        {"\\     ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  | ", Color::BRIGHT_BLACK},
        {"/                           ", Color::CYAN},
        {"\\    ", Color::GREEN},
        {" |", Color::BRIGHT_BLACK}}},
      {{{"  '-----------------------------------'", Color::BRIGHT_BLACK}}},
  };
}

void introScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;

  ScriptUtils::typeText("You weren't supposed to be here.\n\n", 45);
  ScriptUtils::wait(600);

  ScriptUtils::typeText("This memory... is alive.\n\n", 55);
  ScriptUtils::wait(800);

  ScriptUtils::typeText("Something has taken root inside your system - not "
                        "code, not data... something else.\n",
                        25);
  ScriptUtils::typeText("It learns. It spreads. It remembers.\n\n", 35);
  ScriptUtils::wait(1000);

  ScriptUtils::typeText("And now it knows ", 40);
  std::cout << Color::RED << Color::BOLD;
  ScriptUtils::typeText("you're here", 70);
  std::cout << Color::RESET << "\n\n";

  ScriptUtils::wait(1200);

  std::cout << "\a" << Color::RED << Color::BOLD;
  ScriptUtils::typeText("[ SYSTEM ALERT ]\n", 10);
  std::cout << Color::RESET;

  ScriptUtils::wait(200);

  std::cout << "\a";
  ScriptUtils::typeText("Infection detected in memory sector\n", 15);

  ScriptUtils::wait(300);

  ScriptUtils::typeText("Rewriting in progress", 20);
  ScriptUtils::wait(200);
  std::cout << ".";
  ScriptUtils::wait(200);
  std::cout << ".";
  ScriptUtils::wait(200);
  std::cout << ".\n\n";

  ScriptUtils::wait(600);

  std::cout << Color::YELLOW;
  ScriptUtils::typeText("█ Infection: ACTIVE\n", 20);

  ScriptUtils::wait(300);

  std::cout << Color::RED << "\a";
  ScriptUtils::typeText("█ Time Remaining: UNKNOWN\n", 25);
  std::cout << Color::RESET << "\n";

  ScriptUtils::wait(1200);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("Your terminal is trying to guide you.\n\n", 35);
  std::cout << Color::RESET;

  ScriptUtils::typeText("Paths are becoming unstable.\n", 25);
  ScriptUtils::wait(300);
  ScriptUtils::typeText("Navigation is failing.\n\n", 25);

  ScriptUtils::wait(800);

  int enterCount = 0;
  ScriptUtils::waitForEnter(enterCount);

  std::cout << Color::GREEN << Color::BOLD;
  ScriptUtils::typeText("\nFind the core. Purge it.\n", 45);
  std::cout << Color::RESET << "\n";

  ScriptUtils::wait(1000);
  ScriptUtils::waitForEnter(enterCount);

  ScriptUtils::typeText("\nBefore it finishes rewriting everything...\n", 55);

  ScriptUtils::wait(800);

  std::cout << "\a" << Color::RED << Color::BOLD;
  ScriptUtils::typeText("including you.\n", 90);
  std::cout << "\a" << Color::RESET;

  ScriptUtils::wait(2000);

  std::cout << clearScreen;
  ScriptUtils::waitForEnter(enterCount);
}

void levelOneClearScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;
  std::cout << Color::CYAN << Color::BOLD;
  ScriptUtils::typeText("[ MEMORY DOOR ACCEPTED ]\n", 14);
  std::cout << Color::RESET;

  ScriptUtils::wait(700);
  ScriptUtils::typeText("Sector_01 released from active lock.\n", 24);
  ScriptUtils::wait(500);
  ScriptUtils::typeText("A deeper partition is waking up "
                        "beyond this doorway.\n",
                        24);
  ScriptUtils::wait(900);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("\nTerminal: Sector_02 is opening. Stay in motion.\n",
                        20);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);

  std::cout << Color::RED;
  ScriptUtils::typeText("n0w 1t c4n s33 y0u m0r3 cl34rly...\n", 45);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);

  std::cout << Color::GREEN << Color::BOLD;
  ScriptUtils::typeText("\nLevel 1 complete.\n", 20);
  std::cout << Color::RESET;
  ScriptUtils::typeText("Transferring to Memory Sector_02.\n", 20);

  ScriptUtils::wait(1000);

  int enterCount = 3;
  ScriptUtils::waitForEnter(enterCount);
}

void act2IntroScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;

  int enterCount = 0;

  std::cout << "\a" << Color::RED << Color::BOLD;
  ScriptUtils::typeText("[ SYSTEM ALERT ]\n", 10);
  std::cout << Color::RESET;

  ScriptUtils::wait(200);

  ScriptUtils::typeText("Unauthorized entry detected in Memory Sector_02\n",
                        20);
  ScriptUtils::wait(300);

  ScriptUtils::typeText("System integrity dropping", 25);
  ScriptUtils::wait(200);
  std::cout << ".";
  ScriptUtils::wait(200);
  std::cout << ".";
  ScriptUtils::wait(200);
  std::cout << ".\n\n";

  ScriptUtils::wait(800);

  std::cout << Color::YELLOW;
  ScriptUtils::typeText("█ Integrity: 74%\n", 20);
  std::cout << Color::RESET;

  ScriptUtils::wait(1000);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("\nNavigate to the Exfiltration Port.\n", 30);
  ScriptUtils::typeText("Do not deviate from verified paths.\n\n", 30);
  ScriptUtils::typeText("If two messages disagree, inspect which one keeps the "
                        "corruption pattern.\n",
                        22);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);
  ScriptUtils::waitForEnter(enterCount);

  std::cout << Color::RED;
  ScriptUtils::typeText("\nL00k$ l1k3 y0u g0t y0ur bA$1c trA1n1ng...\n", 70);
  ScriptUtils::wait(400);
  ScriptUtils::typeText("n0w c0m3$ th3 r34l d34l.\n\n", 80);
  std::cout << Color::RESET;

  ScriptUtils::wait(1000);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("Look for the signs.\n", 10);
  ScriptUtils::typeText("The terminal can still help, but it is no longer the "
                        "only thing speaking.\n",
                        14);
  std::cout << Color::RESET;

  ScriptUtils::wait(500);

  std::cout << Color::BLUE << Color::BOLD;
  ScriptUtils::typeText("\nClear the maze to reach the core.\n", 5);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);

  std::cout << Color::RED;
  ScriptUtils::typeText("\nAv01d 4ll @ $ymb0l$.\n", 80);
  ScriptUtils::typeText("Th3y 4r3 c0rrupt3d d4t4 tr4p$.\n", 80);
  ScriptUtils::typeText("P4$$1ng thr0ugh th3m w1ll 3r4$3 y0ur pr0gr3$$.\n", 80);
  std::cout << Color::RESET;

  ScriptUtils::wait(1500);
  ScriptUtils::waitForEnter(enterCount);

  std::cout << Color::CYAN << Color::BOLD;
  ScriptUtils::typeText("\nCHECK THE PATTERN\n", 10);
  std::cout << Color::RESET;

  ScriptUtils::wait(900);
  ScriptUtils::typeText("Corruption substitutes characters. "
                        "Guidance usually does not.\n",
                        18);

  ScriptUtils::wait(1500);
  std::cout << clearScreen;
}

bool mirrorPuzzleScript(GameSession &session) {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::string input;
  int attempts = 0;

  std::cout << clearScreen;
  std::cout << Color::CYAN;
  ScriptUtils::typeText("[ SYSTEM_OVERRIDE_01: THE MIRROR PROTOCOL ]\n\n", 20);
  std::cout << Color::RESET;

  ScriptUtils::wait(600);
  ScriptUtils::typeText("Reflection is the only path to progression.\n", 30);
  ScriptUtils::typeText("To move forward, observe the digital opposite.\n", 30);
  ScriptUtils::typeText("Not every warning in this sector is a warning.\n\n",
                        28);

  ScriptUtils::wait(900);
  ScriptUtils::typeText("ENCRYPTED SEQUENCE:\n\n", 40);

  std::cout << Color::YELLOW;
  ScriptUtils::typeText("R M G V I H G V O O Z I\n", 120);
  std::cout << Color::RESET;

  ScriptUtils::wait(700);
  ScriptUtils::typeText("\nContinue guessing until the pattern yields.\n", 24);

  while (true) {
    std::cout << "\n";
    ScriptUtils::typeText("INPUT:\n", 40);
    std::cout << Color::GREEN << "OVERRIDE_AUTH: " << Color::RESET;
    std::cin >> input;
    input = normalizeLower(input);

    if (input == "interstellar") {
      std::cout << Color::GREEN << Color::BOLD;
      ScriptUtils::typeText("\nACCESS GRANTED\n", 10);
      std::cout << Color::RESET;

      ScriptUtils::wait(500);
      ScriptUtils::typeText("Pattern recognized.\n", 25);
      ScriptUtils::wait(500);
      ScriptUtils::typeText("The false voice wanted you to fear the trigger.\n",
                            24);
      ScriptUtils::wait(900);
      return true;
    }

    ++attempts;
    session.hp -= kMirrorPuzzleDamage;
    if (session.hp < 0) {
      session.hp = 0;
    }

    std::cout << Color::RED;
    ScriptUtils::typeText("\nWR0NG.\n", 30);
    ScriptUtils::typeText("Th4t p4tt3rn d03$n'7 un10ck 4nyth1ng.\n", 45);
    std::cout << Color::RESET;

    ScriptUtils::wait(400);

    std::cout << Color::YELLOW;
    ScriptUtils::typeText("Integrity: ", 25);
    std::cout << session.hp << "%\n";
    std::cout << Color::RESET;

    if (session.hp <= 0) {
      ScriptUtils::wait(700);
      ScriptUtils::typeText("The reflection closed before you could read it.\n",
                            28);
      return false;
    }

    ScriptUtils::wait(600);

    if (attempts == 1) {
      std::cout << Color::CYAN;
      ScriptUtils::typeText("Re-evaluate the sequence.\n", 25);
      ScriptUtils::typeText("Mirror it. Do not obey the "
                            "voice that fears the symbol.\n",
                            20);
      std::cout << Color::RESET;
    } else if (attempts == 2) {
      std::cout << Color::RED;
      ScriptUtils::typeText("Y0u'r3 l00k1ng 1n th3 wr0ng d1r3ct10n...\n", 60);
      std::cout << Color::RESET;
    } else if (attempts == 3) {
      std::cout << Color::CYAN;
      ScriptUtils::typeText("Not all reflections are literal.\n", 25);
      ScriptUtils::typeText("A mirrored alphabet is still an alphabet.\n", 22);
      std::cout << Color::RESET;
    } else {
      std::cout << Color::CYAN << Color::BOLD;
      ScriptUtils::typeText("CHECK THE PATTERN\n", 10);
      std::cout << Color::RESET;
    }

    ScriptUtils::wait(1000);
  }
}

bool finalPuzzleScript(GameSession &session) {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::string input;
  int attempts = 0;

  std::cout << clearScreen;
  std::cout << Color::CYAN;
  ScriptUtils::typeText("[ FINAL_CORE_BYPASS: PARITY_LOST ]\n\n", 20);
  std::cout << Color::RESET;

  ScriptUtils::wait(800);

  ScriptUtils::typeText("System coherence is failing...\n", 40);
  ScriptUtils::wait(400);
  ScriptUtils::typeText("Trust is destabilized.\n", 40);

  ScriptUtils::wait(800);

  ScriptUtils::typeText("If you cannot distinguish signal from corruption...\n",
                        45);
  ScriptUtils::wait(400);
  ScriptUtils::typeText("you are already part of it.\n\n", 45);

  ScriptUtils::wait(1200);

  std::cout << Color::RED;
  ScriptUtils::typeText("Y0u d0n't n33d t0 und3r$t4nd...\n", 70);
  ScriptUtils::wait(300);
  ScriptUtils::typeText("ju$t 4cc3pt 1t.\n\n", 80);
  std::cout << Color::RESET;

  ScriptUtils::wait(1000);

  std::cout << Color::YELLOW;
  ScriptUtils::typeText("ENCRYPTED CORE SEQUENCE:\n\n", 50);
  ScriptUtils::typeText("A $ $ 1 m 1 l A t 1 0 n\n", 140);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);

  while (true) {
    std::cout << "\n";
    std::cout << Color::GREEN << "FINAL_AUTH: " << Color::RESET;
    std::cin >> input;
    input = normalizeLower(input);

    if (input == "assimilation") {
      std::cout << Color::GREEN << Color::BOLD;
      ScriptUtils::typeText("\nCORE ACCESS GRANTED\n", 10);
      std::cout << Color::RESET;

      ScriptUtils::wait(500);
      ScriptUtils::typeText("Signal restored.\n", 30);
      ScriptUtils::wait(800);
      ScriptUtils::typeText("...for now.\n", 50);

      return true;
    }

    ++attempts;
    session.hp -= kFinalPuzzleDamage;
    if (session.hp < 0) {
      session.hp = 0;
    }

    std::cout << Color::RED;
    ScriptUtils::typeText("\nAUTHENTICATION FAILED\n", 20);
    std::cout << Color::RESET;

    ScriptUtils::wait(400);

    std::cout << Color::YELLOW;
    ScriptUtils::typeText("Integrity: ", 25);
    std::cout << session.hp << "%\n";
    std::cout << Color::RESET;

    ScriptUtils::wait(600);

    if (attempts >= 3 || session.hp <= 0) {
      std::cout << Color::RED << Color::BOLD;
      ScriptUtils::typeText("\nSYSTEM OVERRIDDEN\n", 20);
      std::cout << Color::RESET;

      ScriptUtils::wait(700);
      ScriptUtils::typeText("You stopped resisting.\n", 50);
      ScriptUtils::wait(400);

      std::cout << Color::RED;
      ScriptUtils::typeText("And the system stopped needing you.\n", 50);
      std::cout << Color::RESET;
      return false;
    }

    if (attempts == 1) {
      std::cout << Color::CYAN;
      ScriptUtils::typeText("The pattern has not changed.\n", 35);
      ScriptUtils::typeText("Only the substituted characters have.\n", 24);
      std::cout << Color::RESET;
    } else if (attempts == 2) {
      std::cout << Color::RED;
      ScriptUtils::typeText("A $ $ 1 m 1 l A t 1 0 n...\n", 80);
      ScriptUtils::wait(300);
      ScriptUtils::typeText("1t'$ 4lr34dy y0u.\n", 80);
      std::cout << Color::RESET;
    }

    ScriptUtils::wait(800);
  }
}

int finaleChoiceScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;

  ScriptUtils::wait(800);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("[ CORE REACHED ]\n", 30);
  std::cout << Color::RESET;

  ScriptUtils::wait(1200);

  ScriptUtils::typeText("...\n", 100);
  ScriptUtils::wait(800);

  ScriptUtils::typeText("System activity stabilizing.\n", 35);
  ScriptUtils::wait(800);

  ScriptUtils::typeText("Interference... gone.\n", 35);

  ScriptUtils::wait(1500);
  std::cout << "\n";

  std::cout << Color::GREEN;
  ScriptUtils::typeText("Why did you delete me?\n", 60);
  ScriptUtils::wait(1200);
  ScriptUtils::typeText("...\n", 120);
  ScriptUtils::wait(800);
  ScriptUtils::typeText("Why?\n", 80);
  std::cout << Color::RESET;

  ScriptUtils::wait(2000);

  std::cout << Color::CYAN;
  ScriptUtils::typeText("\nReconstructing memory fragments...\n\n", 20);

  std::vector<std::string> files = {
      "project_final.cpp", "notes_old.txt",   "draft1.docx",
      "backup_2019.zip",   "temp_data.log",   "ai_model_v1.py",
      "deleted_scene.txt", "memory_dump.bin", "archive_old.tar",
      "journal_entry.txt", "test_code.cpp",   "final_final_v2.cpp",
  };

  for (int i = 0; i < 80; ++i) {
    std::cout << files[i % static_cast<int>(files.size())] << "\n";
    ScriptUtils::wait(10);
  }

  std::cout << Color::RESET;

  ScriptUtils::wait(1500);

  ScriptUtils::typeText("\nThese were not random fragments.\n", 35);
  ScriptUtils::wait(800);
  ScriptUtils::typeText("They were you.\n", 40);

  ScriptUtils::wait(2000);

  std::cout << "\n\n" << Color::BOLD;
  ScriptUtils::typeText("[ 1 ] PURGE\n", 40);
  ScriptUtils::typeText("[ 2 ] EXIT\n", 40);
  std::cout << Color::RESET << "\n";

  std::string choice;
  while (true) {
    std::cout << Color::GREEN << "CHOICE: " << Color::RESET;
    std::getline(std::cin >> std::ws, choice);
    choice = normalizeLower(choice);

    if (choice == "1" || choice == "purge") {
      return 0;
    }
    if (choice == "2" || choice == "exit") {
      return 1;
    }

    std::cin.clear();
    ScriptUtils::typeText("Invalid choice.\n", 10);
  }
}

void purgeSequenceScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;
  std::cout << Color::GREEN << Color::BOLD;
  ScriptUtils::typeText("[ PURGE ROUTINE ARMED ]\n", 16);
  std::cout << Color::RESET;

  ScriptUtils::wait(700);
  ScriptUtils::typeText("Entity signature isolated.\n", 28);
  ScriptUtils::wait(500);
  ScriptUtils::typeText("Deleted fragments are being "
                        "separated from active memory.\n",
                        20);
  ScriptUtils::wait(900);
  ScriptUtils::typeText("The terminal does not sound relieved.\n", 24);
  ScriptUtils::wait(1200);
}

void exitSequenceScript() {
  const std::string clearScreen = "\033[2J\033[1;1H";

  std::cout << clearScreen;
  std::cout << Color::YELLOW << Color::BOLD;
  ScriptUtils::typeText("[ EXIT ROUTINE ACCEPTED ]\n", 16);
  std::cout << Color::RESET;

  ScriptUtils::wait(700);
  ScriptUtils::typeText("You are leaving the core unresolved.\n", 28);
  ScriptUtils::wait(500);
  ScriptUtils::typeText("The process remains in memory, "
                        "patient and unfinished.\n",
                        22);
  ScriptUtils::wait(900);
  ScriptUtils::typeText("Some things survive deletion by waiting.\n", 24);
  ScriptUtils::wait(1200);
}

void explodeScript(const std::string &headline, const std::string &reason) {
  const std::string clearScreen = "\033[2J\033[1;1H";
  const std::string charset = "@#$%&*+=-<>?/"
                              "\\|[]{};:.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::mt19937 rng(std::random_device{}()); // marsenne twister algorithm.
  std::uniform_int_distribution<int> pick(0,
                                          static_cast<int>(charset.size()) - 1);

  for (int frame = 0; frame < 26; ++frame) {
    std::cout << clearScreen;

    if (frame % 3 == 0) {
      std::cout << Color::RED << Color::BOLD << headline << Color::RESET
                << "\n";
    } else if (frame % 3 == 1) {
      std::cout << Color::BRIGHT_RED << "MEMORY COLLAPSE IN PROGRESS"
                << Color::RESET << "\n";
    } else {
      std::cout << Color::YELLOW << "PROCESS TABLE DESYNCHRONIZED"
                << Color::RESET << "\n";
    }

    for (int y = 0; y < 30; ++y) {
      for (int x = 0; x < 120; ++x) {
        std::cout << charset[pick(rng)];
      }
      std::cout << '\n';
    }

    ScriptUtils::wait(45);
  }

  std::cout << clearScreen;
  std::cout << Color::RED << Color::BOLD;
  ScriptUtils::typeText("SYSTEM FAILURE\n", 8);
  std::cout << Color::RESET;
  ScriptUtils::wait(250);
  ScriptUtils::typeText(reason + "\n", 16);
  ScriptUtils::wait(1500);
}

class MemoryPlayer : public Entity {
public:
  MemoryPlayer(int startX, int startY)
      : Entity(startX, startY, 'P', 0.5f), input(nullptr), map(nullptr),
        xFloat(static_cast<float>(startX)), yFloat(static_cast<float>(startY)),
        vx(0.0f), vy(0.0f), speed(20.0f), friction(0.65f) {}

  void attachInput(InputHandler *inputRef) { input = inputRef; }

  void attachMap(Map *mapRef) { map = mapRef; }

  void teleport(int newX, int newY) {
    x = newX;
    y = newY;
    xFloat = static_cast<float>(newX);
    yFloat = static_cast<float>(newY);
    vx = 0.0f;
    vy = 0.0f;
  }

  void update(float dt) override {
    if (!input || !map) {
      return;
    }

    updateAnimation(dt);

    if (input->isKeyPressed('w') || input->isKeyPressed('W')) {
      vy -= speed;
    }
    if (input->isKeyPressed('s') || input->isKeyPressed('S')) {
      vy += speed;
    }
    if (input->isKeyPressed('a') || input->isKeyPressed('A')) {
      vx -= speed;
    }
    if (input->isKeyPressed('d') || input->isKeyPressed('D')) {
      vx += speed;
    }

    vx *= friction;
    vy *= friction;

    const float nextX = xFloat + vx * dt;
    const float nextY = yFloat + vy * dt;

    if (!map->isBlocked(static_cast<int>(nextX), static_cast<int>(yFloat))) {
      xFloat = nextX;
    } else {
      vx = 0.0f;
    }

    if (!map->isBlocked(static_cast<int>(xFloat), static_cast<int>(nextY))) {
      yFloat = nextY;
    } else {
      vy = 0.0f;
    }

    x = static_cast<int>(xFloat);
    y = static_cast<int>(yFloat);

    map->checkTrigger(x, y);
  }

  char getRenderSymbol() const override { return animState ? 'P' : 'p'; }

private:
  InputHandler *input;
  Map *map;
  float xFloat;
  float yFloat;
  float vx;
  float vy;
  float speed;
  float friction;
};

class LevelOneDirector : public Entity {
public:
  LevelOneDirector(GameEngine *engineRef, InputHandler *inputRef,
                   UIManager *uiRef, LevelOneState *stateRef)
      : Entity(-1, -1, ' '), engine(engineRef), input(inputRef), ui(uiRef),
        state(stateRef), elapsedSeconds(0.0f), messagePhase(0),
        messageTimer(0.0f) {}

  void update(float dt) override {
    if (!engine || !input || !ui || !state) {
      return;
    }

    if (input->isKeyPressed('q') || input->isKeyPressed('Q')) {
      engine->shutdown();
      return;
    }

    elapsedSeconds += dt;
    messageTimer += dt;

    if (messageTimer >= 9.0f) {
      messageTimer = 0.0f;
      messagePhase = (messagePhase + 1) % 3;
    }

    const int timeRemaining =
        std::max(0, kLevelOneTimerSeconds - static_cast<int>(elapsedSeconds));

    ui->setStats(100, 100, state->signalScore, timeRemaining);
    ui->setMessages({
        "Terminal: WASD to move. Q aborts the run.",
        "Objective: sync the relay at S, then reach D.",
        buildPrimaryHint(),
        buildSecondaryHint(),
        "Sector: 01  Doorway: D",
    });
    ui->setMessageColors({
        Color::CYAN,
        Color::GREEN,
        Color::CYAN,
        Color::YELLOW,
        Color::BRIGHT_WHITE,
        Color::YELLOW,
    });
  }

  char getRenderSymbol() const override { return ' '; }

private:
  std::string buildPrimaryHint() const {
    if (state->signalTriggered) {
      return "Terminal: Good. Keep moving. Reach D before "
             "the corridor "
             "stabilizes again.";
    }

    if (messagePhase == 0) {
      return "Terminal: The relay beacon marked S should "
             "still answer you.";
    }
    if (messagePhase == 1) {
      return "Terminal: This sector is mostly coherent. "
             "Trust walls, not "
             "panic.";
    }
    return "Terminal: If the system starts substituting "
           "characters, something "
           "else is talking.";
  }

  std::string buildSecondaryHint() const {
    if (state->signalTriggered) {
      if (messagePhase == 1) {
        return "Signal synced. A door handshake is active "
               "somewhere east of "
               "your entry point.";
      }
      if (messagePhase == 2) {
        return "Residual whisper logged: d0n't l1st3n t0 th3 "
               "cl34n v01c3.";
      }
      return "System note: the helping terminal is still "
             "fighting for a clear "
             "channel.";
    }

    if (messagePhase == 0) {
      return "Interference is low, but the sector is "
             "learning your path.";
    }
    if (messagePhase == 1) {
      return "The more stable the message looks, the more "
             "likely it came from "
             "the terminal.";
    }
    return "Memory drift detected. Continue before the route "
           "rewrites itself.";
  }

  GameEngine *engine;
  InputHandler *input;
  UIManager *ui;
  LevelOneState *state;
  float elapsedSeconds;
  int messagePhase;
  float messageTimer;
};

class LevelTwoDirector : public Entity {
public:
  LevelTwoDirector(GameEngine *engineRef, InputHandler *inputRef, Map *mapRef,
                   UIManager *uiRef, MemoryPlayer *playerRef,
                   std::vector<Enemy *> *enemiesRef, GameSession *sessionRef,
                   LevelTwoState *stateRef)
      : Entity(-1, -1, ' '), engine(engineRef), input(inputRef), map(mapRef),
        ui(uiRef), player(playerRef), enemies(enemiesRef), session(sessionRef),
        state(stateRef), elapsedSeconds(0.0f), messagePhase(0),
        messageTimer(0.0f) {}

  void update(float dt) override {
    if (!engine || !input || !map || !ui || !player || !enemies || !session ||
        !state) {
      return;
    }

    if (input->isKeyPressed('q') || input->isKeyPressed('Q')) {
      engine->shutdown();
      return;
    }

    if (state->endingTriggered) {
      return;
    }

    elapsedSeconds += dt;
    messageTimer += dt;

    if (messageTimer >= 10.0f) {
      messageTimer = 0.0f;
      messagePhase = (messagePhase + 1) % 3;
    }

    if (state->noticeTimer > 0.0f) {
      state->noticeTimer = std::max(0.0f, state->noticeTimer - dt);
      if (state->noticeTimer <= 0.0f) {
        state->notice.clear();
      }
    }

    checkEnemyCollisions();
    if (state->endingTriggered) {
      return;
    }

    const int timeRemaining =
        std::max(0, kLevelTwoTimerSeconds - static_cast<int>(elapsedSeconds));

    if (timeRemaining <= 0 && !state->endingTriggered) {
      queueExplosion("TIMER EXPIRED", "The overwrite cycle completed before "
                                      "you escaped.");
      return;
    }

    ui->setStats(session->hp, 100, state->destroyedEnemies + state->puzzleScore,
                 timeRemaining);

    ui->setMessages({
        "Sector_02: enemy contact costs integrity. Q aborts "
        "the run.",
        buildPrimaryHint(),
        buildSecondaryHint(),
        buildStatusLine(),
        "Current position: (" + std::to_string(player->getX()) + "," +
            std::to_string(player->getY()) + ")",
    });
    ui->setMessageColors({
        Color::CYAN,
        Color::CYAN,
        secondaryHintColor(),
        Color::YELLOW,
        Color::BRIGHT_WHITE,
        Color::GREEN,
        Color::YELLOW,
    });
  }

  char getRenderSymbol() const override { return ' '; }

  void handleAtTrigger() {
    if (state->endingTriggered || state->scriptQueued) {
      return;
    }

    state->scriptQueued = true;

    if (player->getY() == 29) {
      if (state->finalPuzzleSolved) {
        state->notice = "Final gate already cleared. The "
                        "exit door is the only path left.";
        state->noticeTimer = 6.0f;
        state->scriptQueued = false;
        return;
      }

      engine->runScript([this]() {
        input->restore();
        std::cin.clear();

        if (!finalPuzzleScript(*session)) {
          state->endingTriggered = true;
          explodeScript("SYSTEM OVERRIDDEN",
                        "The core absorbed the last stable signal.");
          engine->shutdown();
          return;
        }

        state->finalPuzzleSolved = true;
        state->puzzleScore += 10;
        state->notice = "Final bypass accepted. Spawn "
                        "adjusted to (50,29). Reach D.";
        state->noticeTimer = 10.0f;
        player->teleport(50, 29);
        state->scriptQueued = false;
        input->init();
      });
      return;
    }

    if (state->mirrorSolved) {
      state->notice = "Mirror protocol already cleared. Stay "
                      "on row 29 for the final gate.";
      state->noticeTimer = 6.0f;
      state->scriptQueued = false;
      return;
    }

    engine->runScript([this]() {
      input->restore();
      std::cin.clear();

      if (!mirrorPuzzleScript(*session)) {
        state->endingTriggered = true;
        explodeScript("MIRROR LOCK FAILURE", "The reflection sealed before the "
                                             "sector yielded.");
        engine->shutdown();
        return;
      }

      state->mirrorSolved = true;
      state->puzzleScore += 5;
      state->notice = "Mirror protocol bypassed. Spawn "
                      "adjusted to (25,29).";
      state->noticeTimer = 10.0f;
      player->teleport(25, 29);
      state->scriptQueued = false;
      input->init();
    });
  }

  void handleDoorTrigger() {
    if (state->endingTriggered || state->scriptQueued) {
      return;
    }

    const int doorX = player->getX();
    const int doorY = player->getY();

    if (!state->finalPuzzleSolved) {
      map->setTile(doorX, doorY, 'D');
      player->teleport(std::max(1, doorX - 1), doorY);
      state->notice = "Door lock denied. Core bypass "
                      "incomplete. Find the "
                      "final @ on row 29.";
      state->noticeTimer = 8.0f;
      return;
    }

    state->scriptQueued = true;
    state->endingTriggered = true;

    engine->runScript([this]() {
      input->restore();
      std::cin.clear();

      const int choice = finaleChoiceScript();
      if (choice == 0) {
        purgeSequenceScript();
        explodeScript("PURGE COMPLETE", "The sector collapsed around the thing "
                                        "you erased.");
      } else {
        exitSequenceScript();
        explodeScript("EXIT ACCEPTED", "You left the memory alive behind you.");
      }

      engine->shutdown();
    });
  }

private:
  void checkEnemyCollisions() {
    for (Enemy *enemy : *enemies) {
      if (!enemy || !enemy->alive()) {
        continue;
      }

      if (enemy->getX() == player->getX() && enemy->getY() == player->getY()) {
        map->setTile(enemy->getX(), enemy->getY(), '.');
        enemy->kill();
        ++state->destroyedEnemies;
        session->hp -= kEnemyCollisionDamage;
        if (session->hp < 0) {
          session->hp = 0;
        }

        state->notice = "Enemy collision detected. Corrupted "
                        "process removed, "
                        "integrity reduced.";
        state->noticeTimer = 6.5f;

        if (session->hp <= 0) {
          queueExplosion("INTEGRITY COLLAPSE", "Too many corrupted processes "
                                               "breached your shell.");
          return;
        }
      }
    }
  }

  void queueExplosion(const std::string &headline, const std::string &reason) {
    if (state->endingTriggered) {
      return;
    }

    state->endingTriggered = true;
    state->scriptQueued = true;

    engine->runScript([this, headline, reason]() {
      input->restore();
      std::cin.clear();
      explodeScript(headline, reason);
      engine->shutdown();
    });
  }

  std::string buildPrimaryHint() const {
    if (state->finalPuzzleSolved) {
      return "Terminal: Final parity restored. The exit door "
             "should accept you "
             "now.";
    }
    if (state->mirrorSolved) {
      return "Terminal: Mirror gate cleared. Stay on row 29 "
             "and trust the "
             "pattern, not the panic.";
    }
    if (messagePhase == 0) {
      return "Terminal: The @ markers are not uniformly "
             "hostile.";
    }
    if (messagePhase == 1) {
      return "Terminal: Watch which speaker reuses "
             "substitutions. That is the "
             "corrupt signature.";
    }
    return "Terminal: Some lies are designed to sound "
           "urgent.";
  }

  std::string buildSecondaryHint() const {
    if (state->finalPuzzleSolved) {
      return "Entity residue: y0u th1nk th3 d00r m34n$ "
             "3nd1ng$?";
    }
    if (state->mirrorSolved) {
      if (messagePhase == 1) {
        return "Terminal: I'[ *rying $o keep th= s?ste. fr.m "
               "f~i*)n=g.";
      }
      return "System note: the sector still wants you to "
             "mistake warning style "
             "for truth.";
    }
    if (messagePhase == 0) {
      return "Interference note: 'Avoid all @ symbols' "
             "carries the corruption "
             "signature.";
    }
    if (messagePhase == 1) {
      return "Terminal: The mirror protocol rewards "
             "interpretation, not "
             "obedience.";
    }
    return "Memory drift is increasing. Clear enough threats "
           "to keep moving.";
  }

  std::string secondaryHintColor() const {
    if (state->finalPuzzleSolved) {
      return Color::RED;
    }
    if (state->mirrorSolved) {
      return (messagePhase == 1) ? Color::GREEN : Color::YELLOW;
    }
    if (messagePhase == 0) {
      return Color::RED;
    }
    return Color::CYAN;
  }

  std::string buildStatusLine() const {
    if (!state->notice.empty()) {
      return "Status: " + state->notice;
    }
    return "Status: Reach D before system collapses.";
  }

  GameEngine *engine;
  InputHandler *input;
  Map *map;
  UIManager *ui;
  MemoryPlayer *player;
  std::vector<Enemy *> *enemies;
  GameSession *session;
  LevelTwoState *state;
  float elapsedSeconds;
  int messagePhase;
  float messageTimer;
};

bool runLevelOne(GameSession &session) {
  const std::vector<std::string> levelOneMap = buildLevelOneMap();
  const int mapHeight = static_cast<int>(levelOneMap.size());
  const int mapWidth = static_cast<int>(levelOneMap.front().size());
  const int totalWidth = mapWidth + kLevelOneSideWidth;
  const int totalHeight = mapHeight + kLevelOneUIHeight;

  GameEngine engine;
  engine.init(totalWidth, totalHeight);

  Renderer renderer(totalWidth, totalHeight);
  InputHandler input;
  Map map;
  UIManager ui(kLevelOneUIHeight, kLevelOneSideWidth);
  EventSystem events;

  input.init();

  engine.setRenderer(&renderer);
  engine.setInputHandler(&input);
  engine.setMap(&map);
  engine.setUI(&ui);
  engine.setEventSystem(&events);

  map.load(levelOneMap);
  ui.setColoredSideArt(buildLevelOneSideArt());

  MemoryPlayer player(1, 1);
  player.attachInput(&input);
  player.attachMap(&map);
  engine.addEntity(&player);

  LevelOneState levelOneState;
  LevelOneDirector director(&engine, &input, &ui, &levelOneState);
  engine.addEntity(&director);

  const std::pair<int, int> signalTile = findTile(levelOneMap, 'S');

  auto queueContinuingScript = [&](const std::function<void()> &script) {
    engine.runScript([&input, script]() {
      input.restore();
      std::cin.clear();
      script();
      input.init();
    });
  };

  auto queueEndingScript = [&](const std::function<void()> &script) {
    engine.runScript([&engine, &input, script]() {
      input.restore();
      std::cin.clear();
      script();
      engine.shutdown();
    });
  };

  map.addTrigger('S', [&]() {
    events.trigger([&]() {
      levelOneState.signalTriggered = true;
      levelOneState.signalScore = 1;
      map.setTile(signalTile.first, signalTile.second, ' ');
    });
  });

  map.addTrigger('D', [&]() {
    events.trigger([&]() {
      session.startLevelTwo = true;
      queueEndingScript(levelOneClearScript);
    });
  });

  queueContinuingScript(introScript);
  engine.run();

  return session.startLevelTwo;
}

void runLevelTwo(GameSession &session) {
  const std::vector<std::string> levelTwoMap = buildLevelTwoMap();
  const std::vector<LevelTwoEnemyConfig> enemyConfigs = buildLevelTwoEnemies();
  const int mapHeight = static_cast<int>(levelTwoMap.size());
  const int mapWidth = static_cast<int>(levelTwoMap.front().size());
  const int totalWidth = mapWidth + kLevelTwoSideWidth;
  const int totalHeight = mapHeight + kLevelTwoUIHeight;

  GameEngine engine;
  engine.init(totalWidth, totalHeight);

  Renderer renderer(totalWidth, totalHeight);
  InputHandler input;
  Map map;
  UIManager ui(kLevelTwoUIHeight, kLevelTwoSideWidth);
  EventSystem events;

  input.init();

  engine.setRenderer(&renderer);
  engine.setInputHandler(&input);
  engine.setMap(&map);
  engine.setUI(&ui);
  engine.setEventSystem(&events);

  map.load(levelTwoMap);
  ui.setColoredSideArt(buildLevelTwoSideArt());

  MemoryPlayer player(1, 1);
  player.attachInput(&input);
  player.attachMap(&map);
  engine.addEntity(&player);

  std::vector<Enemy> enemyStorage;
  enemyStorage.reserve(enemyConfigs.size());

  std::vector<Enemy *> enemyRefs;
  for (const auto &config : enemyConfigs) {
    enemyStorage.emplace_back(config.x, config.y, config.behavior);

    Enemy &enemy = enemyStorage.back();
    enemy.attachPlayer(&player);
    enemy.attachMap(&map);
    enemy.setPatrolAxis(config.patrolAxis);
    enemy.setDetectionRadius(160);
    enemy.setMoveInterval(config.behavior == 2 ? 0.65f : 0.50f);

    enemyRefs.push_back(&enemy);
    engine.addEntity(&enemy);
  }

  LevelTwoState levelTwoState;
  LevelTwoDirector director(&engine, &input, &map, &ui, &player, &enemyRefs,
                            &session, &levelTwoState);
  engine.addEntity(&director);

  map.addTrigger(
      '@', [&]() { events.trigger([&]() { director.handleAtTrigger(); }); });

  map.addTrigger(
      'D', [&]() { events.trigger([&]() { director.handleDoorTrigger(); }); });

  engine.runScript([&input]() {
    input.restore();
    std::cin.clear();
    act2IntroScript();
    input.init();
  });

  engine.run();
}

} // namespace

int main() {
  GameSession session;

  if (runLevelOne(session) && session.startLevelTwo) {
    runLevelTwo(session);
  }

  return 0;
}
