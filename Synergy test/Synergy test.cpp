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
#define HARD_CAP	500	//define for maximum possible floors
#define MOVE_TIMEOUT CLOCKS_PER_SEC	//since the cabin needs to move once per second just redefine standart constant
using namespace std;	

//Elevator class
class Elevator {
public:
	Elevator(int _floors, char _pickUpDir);
	~Elevator();
	int move();
	void displayStatus();
	void fancyDisplay();
	int callFromFloor(int _floor);
	int callFromCabin(int _floor);
	int getFloors();
private:
	vector<char> calls; //flags for calls
	char direction;
	int currentFloor;
	int targetFloor;	//the last floor in the calls
	char pickUpDir;		//the elevator will stop at a passing floor called from floor only if moving a certain direction
};

//constructor with the number of floors in the elevator
Elevator::Elevator(int _floors, char _pickUpDir) {
	//cabin at floor 1 and wants to get to floor 1 no calls
	pickUpDir = _pickUpDir;
	currentFloor = 0;
	direction = 0;
	targetFloor = 0;
	if (_floors > HARD_CAP) {
		cout << "Too many floors. Setting to " << HARD_CAP << endl;
		_floors = HARD_CAP;
	}
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
		calls[currentFloor] = 0;
	}
	else if (currentFloor > targetFloor) {
		//set the movement towards target
		direction = -1;
	}
	else {
		direction = 1;
	}
	//if the cabin passes by a floor which it is called to, it will stop for one cycle
	//it will stop in floors called from floor only if the direction is matching pickup direction or pickup direction is not set
	if (direction == pickUpDir && pickUpDir!=0) {
		if (calls[currentFloor] > 0) {
			direction = 0;
			calls[currentFloor] = 0;
		}
	}
	else {
		if (calls[currentFloor] > 1) {
			direction = 0;
			calls[currentFloor] = 0;
		}
	}
	return 0;
}

//function to call the elevator to floor _floor
int Elevator::callFromFloor(int _floor) {
	int target = _floor;
	//check for too large and too small floor
	if (target > this->getFloors()) {
		target = this->getFloors();
	}
	if (target < 1) {
		target = 1;
	}
	cout << "Elevator called to floor: " << target << endl;
	//decrease by one to match with vector indices
	--target;
	//set the call if the floor is not called yet
	if (calls[target] == 0) {
		calls[target] = 1;
	}
	//recalculate the target floor
	if (currentFloor <= targetFloor&&targetFloor < target) {
		targetFloor = target;
	}
	else if (currentFloor >= targetFloor&&targetFloor > target) {
		targetFloor = target;
	}
	return 0;
}

//function to call the elevator to floor _floor
int Elevator::callFromCabin(int _floor) {
	int target = _floor;
	//check for too large and too small floor
	if (target > this->getFloors()) {
		target = this->getFloors();
	}
	if (target < 1) {
		target = 1;
	}
	cout << "Elevator called to floor: " << target << endl;
	//decrease by one to match with vector indices
	--target;
	//set the call the cabin call is more important
	calls[target] = 2;
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
	switch (direction) {
	case 1:
		cout << "Moving up   at ";
		break;
	case 0:
		cout << "Stopped     at ";
		break;
	case -1:
		cout << "Moving down at ";
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
	cout << "Elevator pickup direction is: ";
	switch (pickUpDir) {
	case 1:
		cout << "UP" << endl;
		break;
	case 0:
		cout << "not set" << endl;
		break;
	case -1:
		cout << "DOWN" << endl;
		break;
	}
	//draw the elevator shaft from top to the cabin checking if the floor is called
	for (int i = calls.size()-1; i > currentFloor; --i) {
		cout <<setw(3)<<i + 1<< " |  | ";
		if (calls[i] == 1) {
			cout << "called from floor";
		}
		else if (calls[i] == 2) {
			cout << "called from cabin";
		}
		cout << endl;
	}
	//draw the cabin
	cout << setw(3) << currentFloor + 1 << " |[]| ";
	//write the direction of movement
	switch (direction) {
	case 1:
		cout << "going up" << endl;
		break;
	case 0:
		cout << "stopped" << endl;
		break;
	case -1:
		cout << "going down" << endl;
	}
	//draw the elevator shaft from the cabin to the bottom
	for (int i = currentFloor - 1; i >= 0; --i) {
		cout << setw(3) << i + 1 << " |  | ";
		if (calls[i] == 1) {
			cout << "called from floor";
		}
		else if (calls[i] == 2) {
			cout << "called from cabin";
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
	char ltr = 0;
	
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
	key = true;
	while (key) {
		cout << "Enter the pickup direction u - up, d - down, b - both"<<endl;
		cout << "The elevator will stop at passing by floors not called from the cabin only when moving in the pickup direction ";
		getline(cin, input);			//get line from console
		stringstream inpStream(input);	//convert to stream
		inpStream >> ltr;
		//check the recieved flag reuse the same variable
		switch (ltr) {
		case'u':
			ltr = 1;
			key = false;
			break;
		case'd':
			ltr = -1;
			key = false;
			break;
		case'b':
			ltr = 0;
			key = false;
			break;
		default:
			cout << "unknown command " << endl;
		}
	}
	//create instance of the object
	Elevator elevator(num, ltr);
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
			cout << "Input f<number> or c<number> to call or q to quit: " << endl;
		}
		//check for console input
		GetNumberOfConsoleInputEvents(input_handle, &events);
		if (events>0) {
			//initerpret integer same as above, but insted call to the desired floor
			getline(cin, input);
			stringstream inpStream(input);
			//get the first letter as a command
			inpStream >> ltr;
			switch (ltr) {
			case'q':
				cout << "Stopping" << endl;
				key = false;
				break;
			case'c':
				if (inpStream >> num) {
					elevator.callFromCabin(num);
				}
				else {
					cout << "Entered non integer" << endl;
				}
				break;
			case'f':
				if (inpStream >> num) {
					elevator.callFromFloor(num);
				}
				else {
					cout << "Entered non integer" << endl;
				}
				break;
			default:
				cout << "unknown command" << endl;
			}
		}
	}
	system("pause");
    return 0;
}

