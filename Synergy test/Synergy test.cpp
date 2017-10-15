// Synergy test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ctime>		//for time checks
#include <vector>		//for storing calls data
#include <iostream>		//cin cout
#include <sstream>		//stringstream
#include <iomanip>		//std::setw
#include <windows.h>	//console events check

#define DIRECTION 0		//define for calls[0]
#define MOVE_TIMEOUT CLOCKS_PER_SEC	//since the cabin needs to move once per second just redefine standart constant
using namespace std;	

//Elevator class
class Elevator {
public:
	Elevator(int _floors);
	~Elevator();
	int move();
	void displayStatus();
	void fancyDisplay();
	int call(int _floor);
	int getFloors();
private:
	//since vectors start from 0 and i suppose that floors start from 1
	//and because i dont want to subtract 1 every time i want to check the flag
	//calls[0] will be used for storing the direction of the elevator
	vector<char> calls;
	int currentFloor;
	int targetFloor;
};

//constructor with the number of floors in the elevator
Elevator::Elevator(int _floors) {
	//cabin at floor 1 and wants to get to floor 1
	currentFloor = 1;
	targetFloor = 1;
	//for storing one extra value in calls[0] resize to _floor+1
	//internal values initialised to 0 which means no calls and not moving
	calls.resize(_floors + 1, 0);
}

//function to move the cabin
int Elevator::move() {
	//add the direction to the current floor
	currentFloor += calls[DIRECTION];
	//check for reaching target
	if (currentFloor == targetFloor) {
		//we reached the target and now we need to check where to go next
		//if we came from below then check everything below, in case a button below was pressed during movement
		if (calls[DIRECTION] == 1) {
			for (int i = currentFloor - 1; i > 0; --i) {
				if (calls[i] == 1) {
					targetFloor = i;
				}
			}
		}
		//same for reaching target from above
		else {
			for (int i = currentFloor + 1; i <calls.size(); ++i) {
				if (calls[i] == 1) {
					targetFloor = i;
				}
			}
		}
		//stop
		calls[DIRECTION] = 0;
	}
	else if (currentFloor > targetFloor) {
		//set the movement towards target
		calls[DIRECTION] = -1;
	}
	else {
		calls[DIRECTION] = 1;
	}
	//if the cabin passes by a floor which it is called to, it will stop for one cycle
	if (calls[currentFloor]) {
		calls[DIRECTION] = 0;
		calls[currentFloor] = 0;
	}
	return 0;
}

//function to call the elevator to floor _floor
int Elevator::call(int _floor) {
	int target = _floor;
	//check for too large and too small floor
	if (target >= calls.size()) {
		target = calls.size() - 1;
	}
	if (target < 1) {
		target = 1;
	}
	cout << "Elevator called to floor: " << target << endl;
	//set the call
	calls[target] = 1;
	//recalculate the target floor
	if (currentFloor <= targetFloor&&targetFloor < target) {
		targetFloor = target;
	}
	else if (currentFloor >= targetFloor&&targetFloor > target) {
		targetFloor = target;
	}
	return 0;
}

//simple get floors
int Elevator::getFloors() {
	return calls.size() - 1;
}

//simple text display of current status
void Elevator::displayStatus() {
	//check the movement direction and display the corresponding message
	if (calls[DIRECTION] == 0) {
		cout << "Elevator is stopped at " << currentFloor << " floor"<<endl;
	}
	else if (calls[DIRECTION] == -1) {
		cout << "Elevator moving down, currently at " << currentFloor << " floor" << endl;
	}
	else {
		cout << "Elevator moving up, currently at " << currentFloor << " floor" << endl;
	}
	//list the calls requested
	cout << "List of requested floors:" << endl;
	for (int i = 1; i < calls.size(); i++) {
		if (calls[i] == 1) {
			cout << i << " ";
		}
	}
	cout << endl;
}

//here i got carried away a little)
void Elevator::fancyDisplay() {
	//clear console output
	system("cls");
	cout << "Elevator:" << endl;
	//draw the elevator shaft from top to the cabin checking if the floor is called
	for (int i = calls.size()-1; i > currentFloor; --i) {
		cout <<setw(3)<<i<< " |  | ";
		if (calls[i] == 1) {
			cout << "called";
		}
		cout << endl;
	}
	//draw the cabin
	cout << setw(3) << currentFloor << " |[]| ";
	//write the direction of movement
	if (calls[DIRECTION] == 1) {
		cout << "going up" << endl;
	}
	else if (calls[DIRECTION] == 0) {
		cout << "stopped" << endl;
	}
	else {
		cout << "going down" << endl;
	}
	//draw the elevator shaft from the cabin to the bottom
	for (int i = currentFloor-1; i > 0; --i) {
		cout << setw(3) << i << " |  | ";
		if (calls[i] == 1) {
			cout << "called";
		}
		cout << endl;
	}
}

Elevator::~Elevator() {
	calls.clear();
}

int main()
{
	int num = 0;		//variable to store input integers
	bool key = true;	//key for cycles
	string input = "";	//string for console input
	clock_t time;		//variable for time
	HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE); // handle to input/output buffer
	DWORD events = 0;			// how many events took place
	
	while (key) {
		cout << "Please enter number of floors: ";
		getline(cin, input);			//get line from console
		stringstream inpStream(input);	//convert to stream
		if (inpStream >> num) {			//try to interpret the input as an integer
			key = false;				//got a number stop the cycle
		}
		else {
			cout << "Invalid number, please try again" << endl;
		}
	}
	//create instance of the object
	Elevator elevator(num);
	key = true;
	//record time
	time = clock(); 
	
	while (key) {
		//check for console input
		GetNumberOfConsoleInputEvents(input_handle, &events);
		if (events>0) {
			cout << "Input next floor or q: ";
			//initerpret integer same as above, but insted call to the desired floor
			getline(cin, input);
			stringstream inpStream(input);
			
			if (inpStream >> num) {
				elevator.call(num);
			}
			else {
				//check for letter q to exit
				if (input[0] == 'q') {
					cout << "Stopping" << endl;
					key = false;
				}
				else {
					cout << "Entered non integer" << endl;
				}
			}
		}
		//using while in case the input took more than a second
		while (clock() - time > MOVE_TIMEOUT) {
			time += MOVE_TIMEOUT;
			elevator.move();
			//decide how to display, dont bother with the fancy display if more than 30 floors
			if (elevator.getFloors() < 30) {
				elevator.fancyDisplay();
			}
			else {
				elevator.displayStatus();
			}
			cout << "Start typing for interrupt" << endl;
		}
	}
	system("pause");
    return 0;
}

