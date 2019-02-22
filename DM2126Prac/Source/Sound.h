#pragma once
#include <memory>
#include <iostream>
#include "irrklang.h"
#include <stdexcept>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

class Sound
{
public:
	Sound();
	~Sound();
	bool init();
	void playSound(const char* music, bool looping);
	void playSound(const char* music);
	void setSoundVol(const float volume);

private:
	static short count;// count how many sound playing
	ISoundEngine* engine;
};

namespace music
{
	extern Sound player;
}
