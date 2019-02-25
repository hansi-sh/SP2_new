#include "Sound.h"
short Sound::count = 0;
Sound music::player;

Sound::Sound()
{
	if (count == 1)
	{
		throw std::exception("Sound-> can only create 1 player");
	}
	++count;

}

Sound::~Sound()
{
	--count;
 	engine->drop(); // delete engine
}

bool Sound::init()
{
	engine = createIrrKlangDevice();

	if (!engine)
		return false;
}

void Sound::playSound(const char* music, bool looping)
{
	engine->play2D(music, looping);
}

void Sound::playSound(const char* music)
{
	engine->play2D(music);
}

void Sound::setSoundVol(const float volume)
{
	engine->setSoundVolume(volume);
}
