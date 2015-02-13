#include "Actions.h"
#include "Territory.h"
#include <string>
#include <thread> 
#include <chrono>  
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include "Audio.h"

bool returningValue;
Audio actionMusic;

bool Actions::testClearPath(Territory territoryX, Territory territoryY){
	return true;
}

bool Actions::attack(Territory territoryX, Territory territoryY, int owner){
	if (testClearPath(territoryX, territoryY)){
		std::thread attacking(&Actions::attackTerritory, this, territoryX, territoryY, owner);
		attacking.detach();
		return true;
	}
	return false;
}

void Actions::attackTerritory(Territory territoryX, Territory territoryY, int owner){
	float difference = territoryY.getAttackScore() - territoryX.getDefenseScore();
	std::srand(std::time(0)); // use current time as seed for random generator
	int randomFactors = std::rand() % 40 - 20;
	difference = difference + randomFactors;
	actionMusic.playAudio("swords.wav");
	territoryY.setAttackScore(0);

	if (difference > 0){
		//The battle has been won
		std::srand(std::time(0)); // use current time as seed for random generator
		int randomTime = std::rand() % 4000;
		int battleTime = ((60000 / difference) + randomTime);
		//Calculate the battle time in milliseconds with a random factor of up to 3.9 seconds
		std::cout << "Attacking Territory!\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(battleTime));
		std::cout << "Battle Won! - You've claimed a territory\n";
		std::srand(std::time(0)); // use current time as seed for random generator
		int randomValue = std::rand() % 20 - difference;
		int remainingAttack = difference + randomValue;
		territoryX.setDefenseScore(1);
		territoryX.setAttackScore(remainingAttack);
		
		std::cout << "Remaining Troops: ";
		std::cout << remainingAttack;

		actionMusic.playAudio("territoryWon.wav");
		returningValue = true;

	}
	else {
		//The battle has been lost
		if (difference < 0) difference = difference * -1;
		std::srand(std::time(0)); // use current time as seed for random generator
		int randomTime = std::rand() % 4000;
		int battleTime = ((60000 / (difference+1)) + randomTime);
		//Calculate the battle time in milliseconds with a random factor of up to 3.9 seconds
		std::cout << "Attacking Territory!\n";
		std::this_thread::sleep_for(std::chrono::milliseconds((battleTime)));
		std::cout << "Battle Lost!\n";

		int randomValue = std::rand() % 20 - difference;
		int remainingDefense = difference + randomValue;
		territoryX.setDefenseScore(remainingDefense);



		actionMusic.playAudio("territoryLost.wav");
		returningValue = false;
	}

	
}

void Actions::increaseTurn(){
	turnNo++;
}