#include <thread> 
#include <chrono>  
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>

#ifndef __AUDIO_H
#define __AUDIO_H

class Audio {
public:
	sf::SoundBuffer buffer;
	sf::Sound sound;

	int playAudio(char* filename);
	void fadeInAudio(int time);
	void fadeOutAudio(int time);
	void audioFadeThread(int time, int type);
};

#endif