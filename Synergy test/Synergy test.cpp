// Synergy test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <ctime>
#include <vector>
#include <iostream>
#include <sstream>

#define DIRECTION 0
#define MOVE_TIMEOUT CLOCKS_PER_SEC
using namespace std;

#include <windows.h>

class Elevator {
public:
	Elevator(int _floors);
	~Elevator();
	int move();
	void displayStatus();
	int call(int _floor);
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
	while (key==true) {
		if (_kbhit()) {
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
			system("cls");
			elevator.displayStatus();
		}
	}
	system("pause");
    return 0;
}

