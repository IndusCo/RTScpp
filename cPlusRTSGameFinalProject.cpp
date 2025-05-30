/*
-------------------------------------------------------------------------------------------------------
Program: RTS Battle Simulator
Description: A text-based simulation of an RTS game where you build your economy
             and army to battle the computer. The army consists of 5 unit types,
             each with a bonus against a specific enemy unit. Economic units (Workers)
             generate cash each turn to fund unit production. This program demonstrates
             variables, control structures, functions, pointers, dynamic memory, and STL containers.
-------------------------------------------------------------------------------------------------------
*/

#include <iostream>   // For input/output operations
#include <string>     // For using std::string
#include <map>        // For STL map container
#include <cstdlib>    // For random number generation
#include <ctime>      // For seeding the random generator
#include <algorithm>  // For std::min
using namespace std;

// Constants for unit costs and income values
const int WORKER_COST = 50;      // Cost to build one Worker (economic unit)
const int WORKER_INCOME = 50;    // Cash generated per Worker each turn
const int INFANTRY_COST = 100;
const int CAVALRY_COST = 150;
const int ARCHER_COST = 125;
const int MAGE_COST = 200;
const int SIEGE_COST = 250;

// Define military unit types in a constant array
const int NUM_UNIT_TYPES = 5;
const string UNIT_TYPES[NUM_UNIT_TYPES] = { "Infantry", "Cavalry", "Archer", "Mage", "Siege" };

// Function prototypes
void displayOpeningScreen();
void displayInstructions();
void gameLoop(int* playerCash, int* computerCash, int& playerWorkers, int& computerWorkers,
    map<string, int>& playerMilitary, map<string, int>& computerMilitary,
    map<string, int>& unitCosts, map<string, string>& counters,
    int& playerBattlePoints, int& computerBattlePoints);
void playerTurn(int* playerCash, int& playerWorkers, map<string, int>& playerMilitary, map<string, int>& unitCosts);
void computerTurn(int* computerCash, int& computerWorkers, map<string, int>& computerMilitary, map<string, int>& unitCosts);
void generateIncome(int* cash, int workers);
void battlePhase(map<string, int>& playerMilitary, map<string, int>& computerMilitary,
    map<string, string>& counters, int& playerBattlePoints, int& computerBattlePoints);
double calculateBattleScore(map<string, int>& military, map<string, int>& enemyMilitary,
    map<string, string>& counters);
void displayCurrentStatus(int playerWorkers, int computerWorkers, int* playerCash, int* computerCash,
    const map<string, int>& playerMilitary, const map<string, int>& computerMilitary,
    int playerBattlePoints, int computerBattlePoints);

//------------------------------------------------------------------------------
// Main function: program entry point.
int main() {
    // Seed the random number generator for varied gameplay.
    srand(static_cast<unsigned int>(time(0)));

    // Dynamically allocate cash for the player and the computer.
    int* playerCash = new int(500);    // Player starts with 500 cash.
    int* computerCash = new int(500);    // Computer starts with 500 cash.

    // Workers represent economic units that generate income.
    int playerWorkers = 0;
    int computerWorkers = 0;

    // Use STL maps to track military unit counts for player and computer.
    map<string, int> playerMilitary;
    map<string, int> computerMilitary;
    for (int i = 0; i < NUM_UNIT_TYPES; i++) {
        playerMilitary[UNIT_TYPES[i]] = 0;
        computerMilitary[UNIT_TYPES[i]] = 0;
    }

    // Map for military unit costs.
    map<string, int> unitCosts;
    unitCosts["Infantry"] = INFANTRY_COST;
    unitCosts["Cavalry"] = CAVALRY_COST;
    unitCosts["Archer"] = ARCHER_COST;
    unitCosts["Mage"] = MAGE_COST;
    unitCosts["Siege"] = SIEGE_COST;

    // Define unit counter relationships: key unit has bonus against the value unit.
    map<string, string> counters;
    counters["Infantry"] = "Cavalry"; // Infantry beats Cavalry.
    counters["Cavalry"] = "Archer";   // Cavalry beats Archer.
    counters["Archer"] = "Mage";      // Archer beats Mage.
    counters["Mage"] = "Siege";       // Mage beats Siege.
    counters["Siege"] = "Infantry";   // Siege beats Infantry.

    // Initialize battle points.
    int playerBattlePoints = 5;
    int computerBattlePoints = 5;

    // Show the opening screen and instructions.
    displayOpeningScreen();
    displayInstructions();

    // Start the main game loop.
    gameLoop(playerCash, computerCash, playerWorkers, computerWorkers,
        playerMilitary, computerMilitary, unitCosts, counters,
        playerBattlePoints, computerBattlePoints);

    // Clean up dynamically allocated memory.
    delete playerCash;
    delete computerCash;

    return 0;  // Program finished successfully.
}

//------------------------------------------------------------------------------
// Function: displayOpeningScreen
// Purpose: Displays a welcome message for the RTS Battle Simulator.
void displayOpeningScreen() {
    cout << "==============================================" << endl;
    cout << "      Welcome to RTS Battle Simulator         " << endl;
    cout << "==============================================" << endl;
    cout << "Build your economy and army to defeat the computer!" << endl;
}

//------------------------------------------------------------------------------
// Function: displayInstructions
// Purpose: Explains game rules and instructions to the player.
void displayInstructions() {
    cout << "\nInstructions:" << endl;
    cout << "1. Build Workers (cost " << WORKER_COST << ") to generate income (+" << WORKER_INCOME << " cash per Worker)." << endl;
    cout << "2. Recruit military units (Infantry, Cavalry, Archer, Mage, Siege) using cash." << endl;
    cout << "3. Each military unit type has a bonus against one enemy type:" << endl;
    cout << "   - Infantry beats Cavalry" << endl;
    cout << "   - Cavalry beats Archer" << endl;
    cout << "   - Archer beats Mage" << endl;
    cout << "   - Mage beats Siege" << endl;
    cout << "   - Siege beats Infantry" << endl;
    cout << "4. Both you and the computer start with 5 battle points." << endl;
    cout << "   After each battle, the losing side loses one battle point." << endl;
    cout << "   If your battle points reach 0, you lose the game (and vice versa)." << endl;
    cout << "Press Enter to start your adventure...";
    cin.ignore();
    cin.get();
}

//------------------------------------------------------------------------------
// Function: gameLoop
// Purpose: The main loop where each turn the player and computer act, income is generated,
//          battles can be initiated, and battle points are updated. The computer's full status
//          (including battle points) is displayed only after a battle.
void gameLoop(int* playerCash, int* computerCash, int& playerWorkers, int& computerWorkers,
    map<string, int>& playerMilitary, map<string, int>& computerMilitary,
    map<string, int>& unitCosts, map<string, string>& counters,
    int& playerBattlePoints, int& computerBattlePoints) {
    bool gameRunning = true;
    while (gameRunning) {
        // Generate income for both sides.
        generateIncome(playerCash, playerWorkers);
        generateIncome(computerCash, computerWorkers);

        // Display player's status (without computer's hidden info).
        cout << "\n--- Player Status ---" << endl;
        cout << "Cash: " << *playerCash << " | Workers: " << playerWorkers << endl;
        cout << "Army:" << endl;
        for (auto& unit : playerMilitary) {
            cout << unit.first << ": " << unit.second << endl;
        }
        cout << "Battle Points: " << playerBattlePoints << endl;

        // Player takes a turn.
        playerTurn(playerCash, playerWorkers, playerMilitary, unitCosts);

        // Computer takes its turn.
        computerTurn(computerCash, computerWorkers, computerMilitary, unitCosts);

        // Option to engage in battle.
        char battleChoice;
        cout << "\nDo you want to engage in battle with the computer? (Y/N): ";
        cin >> battleChoice;
        if (battleChoice == 'Y' || battleChoice == 'y') {
            battlePhase(playerMilitary, computerMilitary, counters, playerBattlePoints, computerBattlePoints);
            // After battle, display full status for both sides.
            displayCurrentStatus(playerWorkers, computerWorkers, playerCash, computerCash,
                playerMilitary, computerMilitary, playerBattlePoints, computerBattlePoints);
            // Check if either side has reached 0 battle points.
            if (playerBattlePoints == 0) {
                cout << "\nYou have lost all your battle points. You lose the game!" << endl;
                break;
            }
            else if (computerBattlePoints == 0) {
                cout << "\nThe computer has lost all its battle points. You win the game!" << endl;
                break;
            }
        }

        // Ask if the player wants to continue to the next turn.
        char continueChoice;
        cout << "\nDo you want to continue to the next turn? (Y/N): ";
        cin >> continueChoice;
        if (continueChoice == 'N' || continueChoice == 'n') {
            gameRunning = false;
        }
    }
}

//------------------------------------------------------------------------------
// Function: playerTurn
// Purpose: Lets the player decide to build an economic unit (Worker) or a military unit.
void playerTurn(int* playerCash, int& playerWorkers, map<string, int>& playerMilitary, map<string, int>& unitCosts) {
    int choice;
    cout << "\nChoose an action:" << endl;
    cout << "1. Build a Worker (Economic Unit) - Cost: " << WORKER_COST << endl;
    cout << "2. Build a Military Unit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        if (*playerCash >= WORKER_COST) {
            *playerCash -= WORKER_COST;
            playerWorkers++;
            cout << "Worker built! Total Workers: " << playerWorkers << endl;
        }
        else {
            cout << "Not enough cash to build a Worker." << endl;
        }
    }
    else if (choice == 2) {
        cout << "Select a military unit to build:" << endl;
        for (int i = 0; i < NUM_UNIT_TYPES; i++) {
            cout << i + 1 << ". " << UNIT_TYPES[i] << " - Cost: " << unitCosts[UNIT_TYPES[i]] << endl;
        }
        int unitChoice;
        cout << "Enter your choice: ";
        cin >> unitChoice;
        if (unitChoice < 1 || unitChoice > NUM_UNIT_TYPES) {
            cout << "Invalid unit selection." << endl;
        }
        else {
            string chosenUnit = UNIT_TYPES[unitChoice - 1];
            if (*playerCash >= unitCosts[chosenUnit]) {
                *playerCash -= unitCosts[chosenUnit];
                playerMilitary[chosenUnit]++;
                cout << chosenUnit << " recruited!" << endl;
            }
            else {
                cout << "Not enough cash to build " << chosenUnit << "." << endl;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Function: computerTurn
// Purpose: Implements a basic AI for the computer to invest in Workers or military units.
void computerTurn(int* computerCash, int& computerWorkers, map<string, int>& computerMilitary, map<string, int>& unitCosts) {
    if (*computerCash < 100) {
        if (*computerCash >= WORKER_COST) {
            *computerCash -= WORKER_COST;
            computerWorkers++;
            cout << "Computer builds a Worker." << endl;
        }
    }
    else {
        int unitIndex = rand() % NUM_UNIT_TYPES;
        string chosenUnit = UNIT_TYPES[unitIndex];
        if (*computerCash >= unitCosts[chosenUnit]) {
            *computerCash -= unitCosts[chosenUnit];
            computerMilitary[chosenUnit]++;
            cout << "Computer recruits " << chosenUnit << "." << endl;
        }
    }
}

//------------------------------------------------------------------------------
// Function: generateIncome
// Purpose: Increases cash based on the number of Workers.
void generateIncome(int* cash, int workers) {
    *cash += workers * WORKER_INCOME;
}

//------------------------------------------------------------------------------
// Function: battlePhase
// Purpose: Simulates a battle between the player's and the computer's military forces,
//          calculates battle scores based on unit counts and bonus advantages, and updates
//          battle points (the loser loses one battle point).
void battlePhase(map<string, int>& playerMilitary, map<string, int>& computerMilitary,
    map<string, string>& counters, int& playerBattlePoints, int& computerBattlePoints) {
    cout << "\n--- Battle Phase ---" << endl;
    double playerScore = calculateBattleScore(playerMilitary, computerMilitary, counters);
    double computerScore = calculateBattleScore(computerMilitary, playerMilitary, counters);

    cout << "Player Battle Score: " << playerScore << endl;
    cout << "Computer Battle Score: " << computerScore << endl;

    if (playerScore > computerScore) {
        computerBattlePoints--;
        cout << "You win the battle! The computer loses 1 battle point." << endl;
    }
    else if (computerScore > playerScore) {
        playerBattlePoints--;
        cout << "The computer wins the battle! You lose 1 battle point." << endl;
    }
    else {
        cout << "The battle is a tie! No battle points lost." << endl;
    }

    // Reset military forces after the battle.
    for (auto& unit : playerMilitary) {
        unit.second = 0;
    }
    for (auto& unit : computerMilitary) {
        unit.second = 0;
    }
}

//------------------------------------------------------------------------------
// Function: calculateBattleScore
// Purpose: Computes a battle score based on the total count of military units and a bonus
//          for units that counter enemy forces.
double calculateBattleScore(map<string, int>& military, map<string, int>& enemyMilitary,
    map<string, string>& counters) {
    double score = 0.0;
    // Base score: total count of military units.
    for (auto& unit : military) {
        score += unit.second;
    }
    // Add bonus for counter advantages.
    for (auto& unit : military) {
        string unitType = unit.first;
        string counteredType = counters[unitType];  // Bonus for this unit type.
        int bonusUnits = min(unit.second, enemyMilitary[counteredType]);
        score += bonusUnits * 0.5;
    }
    return score;
}

//------------------------------------------------------------------------------
// Function: displayCurrentStatus
// Purpose: Displays the current cash, worker count, military unit counts, and battle points
//          for both the player and the computer.
void displayCurrentStatus(int playerWorkers, int computerWorkers, int* playerCash, int* computerCash,
    const map<string, int>& playerMilitary, const map<string, int>& computerMilitary,
    int playerBattlePoints, int computerBattlePoints) {
    cout << "\n=== Current Status After Battle ===" << endl;

    cout << "Player:" << endl;
    cout << "  Cash: " << *playerCash << " | Workers: " << playerWorkers << endl;
    cout << "  Army:" << endl;
    for (auto& unit : playerMilitary) {
        cout << "    " << unit.first << ": " << unit.second << endl;
    }
    cout << "  Battle Points: " << playerBattlePoints << endl;

    cout << "\nComputer:" << endl;
    cout << "  Cash: " << *computerCash << " | Workers: " << computerWorkers << endl;
    cout << "  Army:" << endl;
    for (auto& unit : computerMilitary) {
        cout << "    " << unit.first << ": " << unit.second << endl;
    }
    cout << "  Battle Points: " << computerBattlePoints << endl;
    cout << "===============================" << endl;
}
