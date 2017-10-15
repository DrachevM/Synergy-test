// Synergy test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"		//WARNING! comment this include before compiling with GCC
#include <ctime>		//for time checks
#include <vector>		//for storing calls data
#include <iostream>		//cin cout
#include <sstream>		//stringstream
#include <iomanip>		//std::setw
#include <windows.h>	//console events check

#define DISPLAY_CAP 30	//define for display mode 
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
	vector<bool> calls; //flags for calls
	char direction;
	int currentFloor;
	int targetFloor;	//the last floor in the calls
};

//constructor with the number of floors in the elevator
Elevator::Elevator(int _floors) {
	//cabin at floor 1 and wants to get to floor 1 no calls
	currentFloor = 0;
	direction = 0;
	targetFloor = 0;
	calls.resize(_floors, 0);
}

//function to move the cabin
int Elevator::move() {
	//add the direction to the current floor
	currentFloor += direction;
	//check for reaching target
	if (currentFloor == targetFloor) {
		//we reached the target and now we need to check where to go next
		//if we came from below then check everything below, in case a button below the cabin was pressed during movement
		if (direction == 1) {
			for (int i = currentFloor - 1; i >= 0; --i) {
				if (calls[i]) {
					targetFloor = i;
				}
			}
		}
		//same for reaching target from above
		else {
			for (int i = currentFloor + 1; i <calls.size(); ++i) {
				if (calls[i]) {
					targetFloor = i;
				}
			}
		}
		//stop
		direction = 0;
	}
	else if (currentFloor > targetFloor) {
		//set the movement towards target
		direction = -1;
	}
	else {
		direction = 1;
	}
	//if the cabin passes by a floor which it is called to, it will stop for one cycle
	if (calls[currentFloor]) {
		direction = 0;
		calls[currentFloor] = false;
	}
	return 0;
}

//function to call the elevator to floor _floor
int Elevator::call(int _floor) {
	int target = _floor;
	//check for too large and too small floor
	if (target > calls.size()) {
		target = this->getFloors();
	}
	if (target < 1) {
		target = 1;
	}
	cout << "Elevator called to floor: " << target << endl;
	//decrease by one to match with vector indices
	--target;
	//set the call
	calls[target] = true;
	//recalculate the target floor
	if (currentFloor <= targetFloor&&targetFloor < target) {
		targetFloor = target;
	}
	else if (currentFloor >= targetFloor&&targetFloor > target) {
		targetFloor = target;
	}
	return 0;
}

//simple get floor count
int Elevator::getFloors() {
	return calls.size();
}

//simple text display of current status
void Elevator::displayStatus() {
	//check the movement direction and display the corresponding message
	if (direction == 0) {
		cout << "Stopped     at ";
	}
	else if (direction == -1) {
		cout << "Moving down at ";
	}
	else {
		cout << "Moving up   at ";
	}
	cout << "floor " << setw(3) << currentFloor + 1 << ", out of " << setw(3) << this->getFloors();
	//list the calls requested
	cout << ". Will stop at: ";
	for (int i = 1; i < calls.size(); i++) {
		if (calls[i]) {
			cout << i + 1 << " ";
		}
	}
}

//here i got carried away a little)
void Elevator::fancyDisplay() {
	//clear console output
	system("cls");
	cout << "Elevator:" << endl;
	//draw the elevator shaft from top to the cabin checking if the floor is called
	for (int i = calls.size()-1; i > currentFloor; --i) {
		cout <<setw(3)<<i + 1<< " |  | ";
		if (calls[i]) {
			cout << "called";
		}
		cout << endl;
	}
	//draw the cabin
	cout << setw(3) << currentFloor + 1 << " |[]| ";
	//write the direction of movement
	if (direction == 1) {
		cout << "going up" << endl;
	}
	else if (direction == 0) {
		cout << "stopped" << endl;
	}
	else {
		cout << "going down" << endl;
	}
	//draw the elevator shaft from the cabin to the bottom
	for (int i = currentFloor - 1; i >= 0; --i) {
		cout << setw(3) << i + 1 << " |  | ";
		if (calls[i]) {
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
		//using while in case the input took more than a second
		while (clock() - time > MOVE_TIMEOUT) {
			//add timeout instead of setting to current time to account for the input delay
			time += MOVE_TIMEOUT;
			elevator.move();
			//decide how to display, dont bother with the fancy display if more than 30 floors
			if (elevator.getFloors() < DISPLAY_CAP) {
				elevator.fancyDisplay();
			}
			else {
				elevator.displayStatus();
			}
			cout << " Start typing for interrupt" << endl;
		}
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
	}
	system("pause");
    return 0;
}

