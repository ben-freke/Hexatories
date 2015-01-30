#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>

#ifndef __AUDIO_H
#define __AUDIO_H

class Audio{
public:
	sf::Sound sound;
	sf::SoundBuffer buffer;

	int playAudio(char* filename);
	
};

#endif