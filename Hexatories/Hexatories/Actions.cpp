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


bool Actions::attack(Territory &territoryX, Territory &territoryY){
	if (!attackFlag){ 
		//Check that an attack hasn't already been made this turn.
		if (!(territoryX.getOwner() == territoryY.getOwner())){
			//Check the user doesn't already own this territory
			if (territoryY.getAttackScore() > 0){
				//Check the user actually has someone to attack with.
				attackFlag = true;
				float difference = territoryY.getAttackScore() - territoryX.getDefenseScore();
				std::srand(std::time(0)); // use current time as seed for random generator
				int randomFactors = std::rand() % 40 - 20;
				difference = difference + randomFactors;
				actionMusic.playAudio("swords.wav");
				territoryY.setAttackScore(0);

				if (difference > 0){
					//The battle has been won
					int randomTime = std::rand() % 4000;
					int battleTime = ((60000 / difference) + randomTime);
					std::cout << "Attacking Territory!\n";
					std::cout << "Battle Won! - You've claimed a territory\n";
					int randomValue = std::rand() % 20 - difference;
					int remainingAttack = difference + randomValue;
					territoryX.setOwner(2);
					territoryY.resetAttack();
					territoryX.resetDefense();
					territoryX.setAttackScore(remainingAttack);

					std::cout << "Remaining Troops: ";
					std::cout << remainingAttack;
					std::cout << "\n";
					actionMusic.playAudio("territoryWon.wav");
					return true;

				}
				else {
					//The battle has been lost
					if (difference < 0) difference = difference * -1;
					int randomTime = std::rand() % 4000;
					int battleTime = ((60000 / (difference + 1)) + randomTime);
					std::cout << "Attacking Territory!\n";
					std::cout << "Battle Lost!\n";

					int randomValue = std::rand() % 20 - difference;
					int remainingDefense = difference + randomValue;
					territoryX.resetAttack();



					actionMusic.playAudio("territoryLost.wav");
					return false;
				}
			}
			else{
				std::cout << "You need some attackers to attack with.\n";
			}
		}
		else{
			std::cout << "You can't attack your own territory.\n";

		}
	}
	else{
		std::cout << "You have already attacked a territory in this turn.\n";
	}

	
}

void Actions::increaseTurn(){
	turnNo++;
	attackFlag = false;
	std::cout << "Turn Number: " << turnNo << "\n";

}