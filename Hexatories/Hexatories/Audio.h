#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <thread> 
#include <chrono>  

#ifndef __AUDIO_H
#define __AUDIO_H

class Audio{
public:
	sf::Sound sound;
	sf::SoundBuffer buffer;

	int playAudio(char* filename);
	void fadeInAudio(int time);
	void audioFadeThread(int time);

	
};

#endif