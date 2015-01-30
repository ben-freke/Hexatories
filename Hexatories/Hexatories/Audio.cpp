#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include "Audio.h"

sf::SoundBuffer buffer;
sf::Sound sound;


int Audio::playAudio(char* filename){

	buffer.loadFromFile(filename);
	sound.setBuffer(buffer);
	sound.play();

	return 0;

}

