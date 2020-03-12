#include "Condition.h"
#include "Value.h"
#include "Arduino.h"

Instruction::Instruction(
    unsigned short ID,
    unsigned int leftID,
    bool rightIsValue,
    unsigned int rightID,
    InstructionType instructionType
) {
    this->ID = ID;
    this->leftID = leftID;
    this->rightIsValue = rightIsValue;
    this->rightID = rightID;
    this->instructionType = instructionType;

    game = Arcadable::getInstance();
};

void Instruction::execute() {
    switch(instructionType) {
        case MutateValue: {
            Value *valueLeft = &game->values.find(leftID)->second;
            if (
                valueLeft->type == number ||
                valueLeft->type == pixelIndex ||
                valueLeft->type == list ||
                valueLeft->type == text
            ) {
                double right = rightIsValue ? game->values.find(rightID)->second.get() : game->calculations.find(rightID)->second.result();
                valueLeft->set(right);
            }

            break;
        }
        case RunCondition: {
            game->conditions.find(leftID)->second.execute();
            break;
        }
        case DrawPixel: {
            unsigned short int xCalc = ( static_cast<int>(game->values.find(leftID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int yCalc = ( static_cast<int>(game->values.find(leftID)->second.value) & 0x0000ffff );
            
            int x = static_cast<int>(game->calculations.find(xCalc)->second.result());
            int y = static_cast<int>(game->calculations.find(yCalc)->second.result());
            CRGB pixelColor = CRGB(
                static_cast<int>(
                    rightIsValue ?
                    game->values.find(rightID)->second.get() :
                    game->calculations.find(rightID)->second.result()
                )
            );
            game->canvas->drawPixel(x, y, pixelColor);
            break;
        }
        case DrawLine: {
            unsigned short int pos1ID = (leftID & 0xffff0000) >> 16;
            unsigned short int pos2ID = leftID & 0x0000ffff;

            unsigned short int pos1XCalc = ( static_cast<int>(game->values.find(pos1ID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pos1YCalc = ( static_cast<int>(game->values.find(pos1ID)->second.value) & 0x0000ffff );

            unsigned short int pos2XCalc = ( static_cast<int>(game->values.find(pos2ID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pos2YCalc = ( static_cast<int>(game->values.find(pos2ID)->second.value) & 0x0000ffff );

            int pos1X = static_cast<int>(game->calculations.find(pos1XCalc)->second.result());
            int pos1Y = static_cast<int>(game->calculations.find(pos1YCalc)->second.result());

            int pos2X = static_cast<int>(game->calculations.find(pos2XCalc)->second.result());
            int pos2Y = static_cast<int>(game->calculations.find(pos2YCalc)->second.result());

            CRGB lineColor = CRGB(
                static_cast<int>(
                    rightIsValue ?
                    game->values.find(rightID)->second.get() :
                    game->calculations.find(rightID)->second.result()
                )
            );
            game->canvas->drawLine(pos1X, pos1Y, pos2X, pos2Y, lineColor);
            break;
        }
        case DrawRect:
        case FillRect: {
            unsigned short int topLeftDrawID = (leftID & 0xffff0000) >> 16;
            unsigned short int bottomRightDrawID = leftID & 0x0000ffff;

            unsigned short int topLeftDrawXCalc = ( static_cast<int>(game->values.find(topLeftDrawID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int topLeftDrawYCalc = ( static_cast<int>(game->values.find(topLeftDrawID)->second.value) & 0x0000ffff );

            unsigned short int bottomRightDrawXCalc = ( static_cast<int>(game->values.find(bottomRightDrawID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int bottomRightDrawYCalc = ( static_cast<int>(game->values.find(bottomRightDrawID)->second.value) & 0x0000ffff );

            int topLeftDrawX = static_cast<int>(game->calculations.find(topLeftDrawXCalc)->second.result());
            int topLeftDrawY = static_cast<int>(game->calculations.find(topLeftDrawYCalc)->second.result());

            int bottomRightDrawX = static_cast<int>(game->calculations.find(bottomRightDrawXCalc)->second.result());
            int bottomRightDrawY = static_cast<int>(game->calculations.find(bottomRightDrawYCalc)->second.result());

            int width = bottomRightDrawX - topLeftDrawX;
            int height = bottomRightDrawY - topLeftDrawY;

            CRGB drawRectColor = CRGB(
                static_cast<int>(
                    rightIsValue ?
                    game->values.find(rightID)->second.get() :
                    game->calculations.find(rightID)->second.result()
                )
            );

            if (instructionType == InstructionType::DrawRect) {
                game->canvas->drawRect(topLeftDrawX, topLeftDrawY, width, height, drawRectColor);
            } else {
                game->canvas->fillRect(topLeftDrawX, topLeftDrawY, width, height, drawRectColor);
            }
            break;
        }

        case DrawCircle:
        case FillCircle: {
            unsigned short int centerID = (leftID & 0xffff0000) >> 16;
            unsigned short int radiusCalcID = leftID & 0x0000ffff;
            unsigned short int centerXCalcID = ( static_cast<int>(game->values.find(centerID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int centerYCalcID = ( static_cast<int>(game->values.find(centerID)->second.value) & 0x0000ffff );
            
            int radius = static_cast<int>(game->calculations.find(radiusCalcID)->second.result());
            int centerX = static_cast<int>(game->calculations.find(centerXCalcID)->second.result());
            int centerY = static_cast<int>(game->calculations.find(centerYCalcID)->second.result());
            CRGB drawCircleColor = CRGB(
                static_cast<int>(
                    rightIsValue ?
                    game->values.find(rightID)->second.get() :
                    game->calculations.find(rightID)->second.result()
                )
            );

            if (instructionType == InstructionType::DrawCircle) {
                game->canvas->drawCircle(centerX, centerY, radius, drawCircleColor);
            } else {
                game->canvas->fillCircle(centerX, centerY, radius, drawCircleColor);
            }
            break;
        }
        case DrawTriangle:
        case FillTriangle: {
            unsigned short int pixel1DrawID = (leftID & 0xffff0000) >> 16;
            unsigned short int pixel2DrawID = leftID & 0x0000ffff;
            unsigned short int pixel3DrawID = (rightID & 0xffff0000) >> 16;
            CRGB triangleColor = CRGB(rightIsValue ?
                game->values.find(rightID & 0x0000ffff)->second.get() :
                game->calculations.find(rightID & 0x0000ffff)->second.result()
            );
            unsigned short int pixel1XCalc = ( static_cast<int>(game->values.find(pixel1DrawID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pixel1YCalc = ( static_cast<int>(game->values.find(pixel1DrawID)->second.value) & 0x0000ffff );
            unsigned short int pixel2XCalc = ( static_cast<int>(game->values.find(pixel2DrawID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pixel2YCalc = ( static_cast<int>(game->values.find(pixel2DrawID)->second.value) & 0x0000ffff );
            unsigned short int pixel3XCalc = ( static_cast<int>(game->values.find(pixel3DrawID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pixel3YCalc = ( static_cast<int>(game->values.find(pixel3DrawID)->second.value) & 0x0000ffff );
            
            int pixel1X = static_cast<int>(game->calculations.find(pixel1XCalc)->second.result());
            int pixel1Y = static_cast<int>(game->calculations.find(pixel1YCalc)->second.result());
            int pixel2X = static_cast<int>(game->calculations.find(pixel2XCalc)->second.result());
            int pixel2Y = static_cast<int>(game->calculations.find(pixel2YCalc)->second.result());
            int pixel3X = static_cast<int>(game->calculations.find(pixel3XCalc)->second.result());
            int pixel3Y = static_cast<int>(game->calculations.find(pixel3YCalc)->second.result());
            if (instructionType == InstructionType::DrawTriangle) {
                game->canvas->drawTriangle(pixel1X, pixel1Y, pixel2X, pixel2Y, pixel3X, pixel3Y, triangleColor);
            } else {
                game->canvas->fillTriangle(pixel1X, pixel1Y, pixel2X, pixel2Y, pixel3X, pixel3Y, triangleColor);
            }
            break;
        }
        case DrawText: {
            unsigned short int pixelTextID = (leftID & 0xffff0000) >> 16;
            unsigned short int pixelTextXCalc = ( static_cast<int>(game->values.find(pixelTextID)->second.value) & 0xffff0000 ) >> 16;
            unsigned short int pixelTextYCalc = ( static_cast<int>(game->values.find(pixelTextID)->second.value) & 0x0000ffff );
            
            int pixelTextX = static_cast<int>(game->calculations.find(pixelTextXCalc)->second.result());
            int pixelTextY = static_cast<int>(game->calculations.find(pixelTextYCalc)->second.result());
            int scale = static_cast<int>(game->values.find(leftID & 0x0000ffff)->second.get());
            unsigned short int listID = (rightID & 0xffff0000) >> 16;
            CRGB textColor = CRGB(
                static_cast<int>(
                    rightIsValue ?
                    game->values.find(rightID & 0x0000ffff)->second.get() :
                    game->calculations.find(rightID & 0x0000ffff)->second.result()
                )
            );
            std::pair<std::multimap<unsigned short int, Value>::iterator, std::multimap<unsigned short int, Value>::iterator> values;
            values = game->lists.equal_range(listID);
            std::vector<char> text;
            for (std::multimap<unsigned short int, Value>::iterator it = values.first; it != values.second; it++) {
                Value value = game->values.find(it->second.ID)->second;
                if (value.type == ValueType::text) {
                    char c = (static_cast<int>(value.value) & 0xFF000000) >> 24;
                    if (c != 0) {
                        text.push_back(c);
                    }
                    c = (static_cast<int>(value.value) & 0x00FF0000) >> 16;
                    if (c != 0) {
                        text.push_back(c);
                    }
                    c = (static_cast<int>(value.value) & 0x0000FF00) >> 8;
                    if (c != 0) {
                        text.push_back(c);
                    }
                    c = static_cast<int>(value.value) & 0x000000FF;
                    if (c != 0) {
                        text.push_back(c);
                    }
                }
            }
            unsigned short int extraChars = (game->systemConfig->screenWidth - (scale * 6 * text.size())) / (scale * 6);
            for (int i = 0; i <= extraChars; i++ ) {
                text.push_back(' ');
            }
       

            game->canvas->setCursor(pixelTextX, pixelTextY);
            game->canvas->setTextColor(textColor);
            game->canvas->setTextSize(scale);
            game->canvas->setTextWrap(false);
            game->canvas->print(text.data());
            break;
        }
        case Clear: {
            game->canvas->fillScreen(CRGB::Black);
        }
        case SetRotation: {
            game->canvas->setRotation(leftID);
        }
        break;
    }
};
