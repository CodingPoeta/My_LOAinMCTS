// My_LOAinMCTS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "loaGame.h"


int main()
{
	srand(int(23333));
	loa::loaGame mygame;
	// loa::guiGame mygame;
	// loa::socketGame mygame;

	mygame.start();

    std::cout << "Exiting, thank you!" << std::endl;
	return 0;
}
