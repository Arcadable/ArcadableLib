/*
	ArcadableLib.h - Library for running games on LED strips.
	Created by Niek de Wit, January, 2020.
	Released into the public domain.
*/
#ifndef ArcadableLib_h
#define ArcadableLib_h

#include "Arduino.h"
#include <map>
#include <vector>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <Wire.h>

class Value;
class Calculation;
class Condition;
class Instruction;
class SystemConfig;

class Arcadable {
	public:
		SystemConfig *systemConfig;

		std::map<int, Value> values;
		std::map<int, Calculation> calculations;
		std::map<int, Condition> conditions;
		std::multimap<int, Instruction> instructions;
		std::multimap<int, Value> lists;
		CRGB* pixels;
		static Arcadable *getInstance();
		void setup(
			SystemConfig *systemConfig,
			CRGB *pixels
		);

		void step();

	private:
		static Arcadable *_instance;
		unsigned int _prevGameMillis;
		unsigned int _prevWireMillis;
		bool _gameLoaded = false;
		bool _readyToLoad = true;
		bool _pollImmediately = false;
		void _doGameStep();
		void _unloadGameLogic();
		void _readAndLoadGameLogic();
		void _readEEPROM(
			unsigned int startAddress,
			unsigned int dataLength,
			unsigned char *dataReceiver
		);

};
#include "SystemConfig.h"
#include "Value.h"
#include "Calculation.h"
#include "Instruction.h"
#include "Condition.h"


#endif