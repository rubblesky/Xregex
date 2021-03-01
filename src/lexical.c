
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xregex.h"


#define LEXICAL_ERROR_DISPLAY 10

LexicalResult * lexicalAnalyse(IntVector * express){
    int dataLength = getIntVectorDataSize(express);
    LexicalResult * lr = malloc((dataLength+1) * sizeof(struct LexicalResult));
    int isContinuous = 0;
    int k = -1;
    for(int i = 0;i < dataLength;i++){
        int character;
        if((character = getIntVectorData(express,i)) < 0){
            isContinuous = 0;
            k += 1;
            switch (-character) {
                case (int)'(':
                    lr[k].type = LEFT_PARENTHESIS;
                    break;
                case (int)')':
                    lr[k].type = RIGHT_PARENTHESIS;
                    break;
                case (int)'[':
                    lr[k].type = LEFT_BRACKET;
                    break;
                case (int)']':
                    lr[k].type = RIGHT_BRACKET;
                    break;
                case (int)'|':
                    lr[k].type = VERTICAL_BAR;
                    break;
                case (int)'*':
                    lr[k].type = ASTERISK;
                    break;
                default:
                    errorString = "包含未知的转移字符";
                    exit(-1);
            }

        }else{
            if(isContinuous){
                appendIntVector(lr[k].string,character);
            } else{
              k += 1;
              lr[k].type = CHARACTER_STRING;
              lr[k].string = initIntVector(10);
              appendIntVector(lr[k].string,character);
              isContinuous = 1;
            }
        }
    }
    lr[k+1].type = END;
    return lr;
}