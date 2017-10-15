// Synergy test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>	//_kbhit() не придумал другого решения
#include <ctime>	//время
#include <vector>
#include <iostream>
#include <sstream>	//stringstream
#include <iomanip>	//std::setw
#include <windows.h>

#define DIRECTION 0
#define MOVE_TIMEOUT CLOCKS_PER_SEC
using namespace std;


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
	vector<char> calls;
	int currentFloor;
	int targetFloor;
};

Elevator::Elevator(int _floors) {
	currentFloor = 1;
	targetFloor = 1;
	calls.resize(_floors + 1, 0);
}
	
int Elevator::move() {
	currentFloor += calls[DIRECTION];
	if (currentFloor == targetFloor) {
		if (calls[DIRECTION] == 1) {
			for (int i = currentFloor - 1; i > 0; --i) {
				if (calls[i] == 1) {
					targetFloor = i;
				}
			}
		}
		else {
			for (int i = currentFloor + 1; i <calls.size(); ++i) {
				if (calls[i] == 1) {
					targetFloor = i;
				}
			}
		}
		calls[DIRECTION] = 0;
	}
	else if (currentFloor > targetFloor) {
		calls[DIRECTION] = -1;
	}
	else {
		calls[DIRECTION] = 1;
	}
	if (calls[currentFloor]) {
		calls[DIRECTION] = 0;
		calls[currentFloor] = 0;
	}
	return 0;
}

int Elevator::call(int _floor) {
	int target = _floor;
	if (target >= calls.size()) {
		target = calls.size() - 1;
	}
	if (target < 1) {
		target = 1;
	}
	cout << "Elevator called to floor: " << target << endl;
	calls[target] = 1;
	if (currentFloor <= targetFloor&&targetFloor < target) {
		targetFloor = target;
	}
	else if (currentFloor >= targetFloor&&targetFloor > target) {
		targetFloor = target;
	}
	return 0;
}

int Elevator::getFloors() {
	return calls.size() - 1;
}

void Elevator::displayStatus() {
	if (calls[DIRECTION] == 0) {
		cout << "Elevator is stopped at " << currentFloor << " floor"<<endl;
	}
	else if (calls[DIRECTION] == -1) {
		cout << "Elevator moving down, currently at " << currentFloor << " floor" << endl;
	}
	else {
		cout << "Elevator moving up, currently at " << currentFloor << " floor" << endl;
	}
	cout << "List of requested floors:" << endl;
	for (int i = 1; i < calls.size(); i++) {
		if (calls[i] == 1) {
			cout << i << " ";
		}
	}
	cout << endl;
}

void Elevator::fancyDisplay() {
	system("cls");
	cout << "Elevator:" << endl;
	for (int i = calls.size()-1; i > currentFloor; --i) {
		cout <<setw(3)<<i<< " |  | ";
		if (calls[i] == 1) {
			cout << "called";
		}
		cout << endl;
	}
	cout << setw(3) << currentFloor << " |[]| ";
	if (calls[DIRECTION] == 1) {
		cout << "going up" << endl;
	}
	else if (calls[DIRECTION] == 0) {
		cout << "stopped" << endl;
	}
	else {
		cout << "going down" << endl;
	}
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

//Elevator elevator;



int main()
{
	int num = 0;
	bool key = true;
	string input = "";
	clock_t time;
	while (key==true) {
		cout << "Please enter number of floors: ";
		getline(cin, input);
		stringstream inpStream(input);
		if (inpStream >> num) {
			key = false;
		}
		else {
			cout << "Invalid number, please try again" << endl;
		}
	}
	Elevator elevator(num);
	key = true;
	time = clock(); 
	
	
	HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE); // handle to input/output buffer
	DWORD events = 0;			// how many events took place
	
	
	while (key==true) {
		GetNumberOfConsoleInputEvents(input_handle, &events);
		if (events>0) {
			cout << "Input next floor or q: ";
			getline(cin, input);
			stringstream inpStream(input);
			
			if (inpStream >> num) {
				elevator.call(num);
			}
			else {
				if (input[0] == 'q') {
					cout << "Stopping" << endl;
					key = false;
				}
				else {
					cout << "Entered non integer" << endl;
				}
			}
		}
		while (clock() - time > MOVE_TIMEOUT) {
			time += MOVE_TIMEOUT;
			elevator.move();
			if (elevator.getFloors() < 60) {
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

