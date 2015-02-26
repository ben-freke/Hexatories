#include <thread> 
#include <chrono> 
#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include "Audio.h"

int Audio::playAudio(char* filename, bool loop) {

	buffer.loadFromFile(filename);
	sound.setBuffer(buffer);
	
	sound.play();
	sound.setLoop(loop);

	return 0;
}

void Audio::fadeInAudio(int time) {
	sound.setVolume(0);
	std::thread audioFade(&Audio::audioFadeThread, this, time, 1);
	audioFade.detach();
}

void Audio::fadeOutAudio(int time) {
	sound.setVolume(0);
	std::thread audioFade(&Audio::audioFadeThread, this, time, 0);
	audioFade.detach();
}

void Audio::audioFadeThread(int time, int type) {
	time = time * 1000;
	int interval = time / 100;

	if (type == 1) {
		for (int x = 0; x <= 100; x++) {
			sound.setVolume((float) x);
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
	}

	if (type == 0) {
		for (int x = 100; x >= 0; x--) {
			sound.setVolume((float) x);
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
		sound.stop();
	}
}

int Audio::getVolume() {
	return (int) sound.getVolume();
}

void Audio::setVolume(int newVol) {
	sound.setVolume((float) newVol);
}