#pragma once

/** Header declaring all the thread types in the game.
*	The interface of these must be in such a way that the messages can do their work
*	by calling functions in that interface. */
/*
class GameStateThread;
class SoundThread;
class NetworkThread;
class ResourceThread;
class LoggerThread;
*/
#include <string>

namespace ThreadNames {
	extern const std::string GameState; // "GSTN"
	extern const std::string Sound; // "SNDTN"
	extern const std::string Network; // "NTTN"
	extern const std::string Resource; // "RSTN"
	extern const std::string Logger; // "LOG"
}

/*
#define GAME_STATE_THREAD_NAME "Game State Thread"
#define SOUND_THREAD_NAME "Sound thread"
#define NETWORK_THREAD_NAME "Network thread"
#define RESOURCE_THREAD_NAME "Resource thread"
#define LOGGER_THREAD_NAME "Logger Thread"
*/