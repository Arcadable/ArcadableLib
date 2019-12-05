#include "Arduino.h"
#include "ArcadableLib.h"
Game *Game::_instance = NULL;
Game *Game::getInstance() {
	if (!_instance)
    {
        _instance = new Game;
    }

    return _instance;
}

void Game::setConfiguration(
	SystemConfig *systemConfig,
	CRGB *pixels
) {
	this->systemConfig = systemConfig;
	this->pixels = pixels;

	for (unsigned int i = 0; i < systemConfig->screenWidth * systemConfig->screenHeight; i++) {
		pixels[i] = CRGB(0 + (0 << 8) + (0 << 16));
	}
}

void Game::setGameLogic(
	std::vector<std::vector<int>> *untypedConditions,
	std::vector<std::vector<int>> *untypedCalculations,
	std::vector<std::vector<int>> *untypedValues,
	std::vector<std::vector<int>> *untypedInstructions
) {

	for ( auto &condition : *untypedConditions ) {
		Condition typedCondition(
			condition[0],
			condition[1],
			condition[2],
			condition[3],
			static_cast<RelationalOperator>(condition[4]),
			condition[5],
			condition[6],
			condition[7]
		);
		conditions.insert(std::pair<int, Condition>(condition[0], typedCondition));
	}
	for ( auto &calculation : *untypedCalculations ) {
		Calculation typedCalculation(
			calculation[0],
			calculation[1],
			calculation[2],
			calculation[3],
			static_cast<CalculationOperator>(calculation[4])
		);
		calculations.insert(std::pair<int, Calculation>(calculation[0], typedCalculation)); 
	}
	for ( auto &value : *untypedValues ) {
		Value typedValue(
			value[0],
			static_cast<ValueType>(value[1]),
			value[2],
			value[3],
			value[4]
		);
		values.insert(std::pair<int, Value>(value[0], typedValue));

		if (typedValue.isPartOfList) {
			lists.insert(std::pair<int, Value>(value[4], typedValue));
		}
	}
	for ( auto &instruction : *untypedInstructions ) {
		Instruction typedInstruction(
			instruction[0],
			instruction[1],
			instruction[2]
		);
		instructions.insert(std::pair<int, Instruction>(instruction[0], typedInstruction));
	}
}
void Game::step() {
	for (unsigned int i = 0; i < systemConfig->screenWidth * systemConfig->screenHeight; i++) {
		pixels[i] = CRGB(0 + (0 << 8) + (0 << 16));
	}
	int currentMillis = millis();
	if (currentMillis - _prevMillis < systemConfig->minMillisPerFrame) {
		delay(systemConfig->minMillisPerFrame - (millis() - _prevMillis));
	}
	_doGameStep();
	_prevMillis = millis();
	Serial.print(values.find(8)->second.get());
		Serial.print("-");

		Serial.print(values.find(9)->second.get());
		Serial.print("-");

		Serial.print(calculations.find(14)->second.result());
	Serial.println();
}

void Game::_doGameStep() {
	systemConfig->fetchInputValues();
	
	for (auto &condition : conditions) {
		if (condition.second.rootCondition == true) {
			condition.second.execute();
		}
	}
};