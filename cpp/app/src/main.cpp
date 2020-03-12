/*
 * main.cpp
 *
 *  Created on: 28.02.2020
 *      Author: jarko
 */

#include "altair/MainBoard.hpp"
#include "altair/Card/Cpu.hpp"


#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	altair::MainBoard board;

	board.addCard(new altair::card::Cpu());
}
