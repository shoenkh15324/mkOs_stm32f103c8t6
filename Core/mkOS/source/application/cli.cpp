#include "../../include/hal/hal.h"
#include "../../include/system/system.h"

#if PROJECT_CLI_ENABLE
#include "cli.h"

extern "C" {
     #include "stm32f1xx_hal_uart.h"
     #include "usart.h"
}    
     constexpr size_t Cli_PRINTF_BUF_SIZE = 128;
     #define cliPrintf(fmt, ...) do{ \
          char buf[Cli_PRINTF_BUF_SIZE]; \
          int len = snprintf(buf, sizeof(buf), fmt, ##__VA_ARGS__); \
          HAL_UART_Transmit(&huart1, reinterpret_cast<uint8_t*>(buf), len, HAL_MAX_DELAY); \
     } while(0)

Cli Cli::_instance;

Cli::Cli(){}

int Cli::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          //
          _objState = objStateClosed;
     }
     return 0;
}

int Cli::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     //
     _cli.ch = Uart::CH1;
     _cli.histLineCnt = 0;
     _cli.histLineIdx = 0;
     _cli.histLineLast = 0;
     _cli.histLineNew = false;
     _cli.cmdArgs.getData = _cliArgsGetData;
     _cli.cmdArgs.getFloat = _cliArgsGetFloat;
     _cli.cmdArgs.getStr = _cliArgsGetStr;
     _cli.cmdArgs.isStr = _cliArgsIsStr;
     Uart::get()->open();
     //
     _objState = objStateOpened;
     this->sync(cliSyncLineClean);
     this->cliAdd("help", _cliShowList);
     this->cliAdd("md", _cliMemoryDump);
     return 0;
}

int Cli::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if (_objState < objStateOpened) {
          LOG("Cli is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     uint8_t rxData =static_cast<unsigned>(reinterpret_cast<uintptr_t>(arg1));
     switch (sync) {
          case cliSyncMain:{
               bool uartAvailable = Uart::get()->sync(Uart::uartAvailableDma);
               if(uartAvailable){
                    uint8_t byte = 0;
                    Uart::get()->sync(Uart::uartReadDma, &byte, (void*)1);
                    this->sync(cliSyncUpdate, (void *)(uintptr_t)byte);
               }
               break;
          }
          case cliSyncUpdate:{
               uint8_t txBuf[8] = {0};
               cliLine *line = &_cli.line;
               if(_objState == objStateOpened){
                    switch(rxData){
                         case CLI_KEY_ENTER:
                              if(line->cmdLen > 0){
                                   this->sync(cliSyncLineAdd);
                                   this->sync(cliSyncRunCmd);
                              }
                              line->cmdLen = 0;
                              line->cursorPos = 0;
                              line->buf[0] = 0;
                              this->sync(cliSyncShowPrompt);
                              break;
                         case CLI_KEY_ESC:
                              _objState = cliStateEscapeWait;
                              break;
                         case CLI_KEY_DEL:
                              if(line->cursorPos < line->cmdLen){
                                   uint8_t cursorShift = line->cmdLen - line->cursorPos;
                                   for (int i = 1; i < cursorShift; i++){
                                        line->buf[line->cursorPos + i - 1] = line->buf[line->cursorPos + i];
                                   }
                                   line->cmdLen--;
                                   line->buf[line->cmdLen] = 0;
                                   cliPrintf("\x1B[1P");
                              }
                              break;
                         case CLI_KEY_BACK:
                              if(line->cmdLen > 0 && line->cursorPos > 0){
                                   if(line->cursorPos == line->cmdLen){
                                        line->cmdLen--;
                                        line->buf[line->cmdLen] = 0;
                                   }
                                   if(line->cursorPos < line->cmdLen){
                                        uint8_t cursorShift = line->cmdLen - line->cursorPos;
                                        for (int i = 1; i < cursorShift; i++){
                                             line->buf[line->cursorPos + i - 1] = line->buf[line->cursorPos + i];
                                        }
                                        line->cmdLen--;
                                        line->buf[line->cmdLen] = 0;
                                   }
                              }
                              if(line->cursorPos > 0){
                                   line->cursorPos--;
                                   cliPrintf("\b \b\x1B[1P");
                              }
                              break;
                         default:
                              if((line->cmdLen + 1) < line->bufLen){
                                   if(line->cursorPos == line->cmdLen){
                                        Uart::get()->sync(Uart::uartSend, (void*)_cli.ch, (void *)&rxData, (void *)1);
                                        line->buf[line->cursorPos] = rxData;
                                        line->cmdLen++;
                                        line->cursorPos++;
                                        line->buf[line->cmdLen] = 0;
                                   }
                                   if(line->cursorPos < line->cmdLen){
                                        uint8_t cursorShift = line->cmdLen - line->cursorPos;
                                        for (int i = 1; i < cursorShift; i++){
                                             line->buf[line->cmdLen - i] = line->buf[line->cmdLen - i - 1];
                                        }
                                        line->buf[line->cursorPos] = rxData;
                                        line->cmdLen++;
                                        line->cursorPos++;
                                        line->buf[line->cmdLen] = 0;
                                        cliPrintf("\x1B[4h%c\x1B[4l", rxData);
                                   }
                              }
                              break;
                    }
               }
               switch(_objState){
                    case cliStateEscapeWait:
                         _objState = cliStateEscapeSequence;
                         break;
                    case cliStateEscapeSequence:
                         _objState = cliStateEscapeKeyWait;
                         break;
                    case cliStateEscapeKeyWait:
                         _objState = objStateOpened;
                         if(rxData == CLI_KEY_LEFT){
                              if(line->cursorPos > 0){
                                   line->cursorPos--;
                                   txBuf[0] = 0x1B;
                                   txBuf[1] = 0x5B;
                                   txBuf[2] = rxData;
                                   Uart::get()->sync(Uart::uartSend, (void*)_cli.ch, txBuf, (void *)3);
                              }
                         }
                         if(rxData == CLI_KEY_RIGHT){
                              if(line->cursorPos < line-> cmdLen){
                                   line->cursorPos++;
                                   txBuf[0] = 0x1B;
                                   txBuf[1] = 0x5B;
                                   txBuf[2] = rxData;
                                   Uart::get()->sync(Uart::uartSend, (void*)_cli.ch, txBuf, (void *)3);
                              }
                         }
                         if(rxData == CLI_KEY_UP){
                              this->sync(cliSyncLineChange, NULL, (void*)true);
                              cliPrintf((char *)_cli.line.buf);
                         }
                         if(rxData == CLI_KEY_DOWN){
                              this->sync(cliSyncLineChange, NULL, (void*)false);
                              cliPrintf((char *)_cli.line.buf);
                         }
                         if(rxData == CLI_KEY_HOME){
                              cliPrintf("\x1B[%dD", line->cursorPos);
                              line->cursorPos = 0;
                              _objState = cliStateEscapeFinalize;
                         }
                         if(rxData == CLI_KEY_END){
                              uint16_t cursorshift = 0;
                              if(line->cursorPos < line->cmdLen){
                                   cursorshift = line->cmdLen - line->cursorPos;
                                   cliPrintf("\x1B[%dC", cursorshift);
                              }
                              if(line->cursorPos > line->cmdLen){
                                   cursorshift = line->cursorPos - line->cmdLen;
                                   cliPrintf("\x1B[%dD", cursorshift);
                              }
                              line->cursorPos = line->cmdLen;
                              _objState = cliStateEscapeFinalize;
                         }
                         break;
                    case cliStateEscapeFinalize:
                         _objState = cliStateEscapeWait;
                         break;
               }
               break;
          }
          case cliSyncLineClean:
               _cli.line.cmdLen = 0;
               _cli.line.cursorPos = 0;
               _cli.line.bufLen = CLI_LINE_BUF_MAX - 1;
               _cli.line.buf[0] = 0;
               break;
          case cliSyncLineAdd:
               _cli.lineBuf[_cli.histLineLast] = _cli.line;
               if (_cli.histLineCnt < CLI_CMD_HIS_MAX){
                    _cli.histLineCnt++;
               }
               _cli.histLineIdx = _cli.histLineLast;
               _cli.histLineLast = (_cli.histLineLast + 1) % CLI_CMD_HIS_MAX;
               _cli.histLineNew = true;
               break;
          case cliSyncLineChange:{
               uint8_t changedIdx = 0;
               bool key = static_cast<bool>(reinterpret_cast<intptr_t>(arg2));
               if(_cli.histLineCnt == 0){
                    return 0;
               }
               if(_cli.line.cursorPos > 0){
                    cliPrintf("\x1B[%dD", _cli.line.cursorPos);
               }
               if(_cli.line.cmdLen > 0){
                    cliPrintf("\x1B[%dP", _cli.line.cmdLen);
               }
               if(key == true){
                    if(_cli.histLineNew == true){
                         _cli.histLineIdx = _cli.histLineLast;
                    }
                    _cli.histLineIdx = (_cli.histLineIdx + _cli.histLineCnt - 1) % _cli.histLineCnt;
                    changedIdx = _cli.histLineIdx;
               }else{
                    _cli.histLineIdx = (_cli.histLineIdx + 1) % _cli.histLineCnt;
                    changedIdx = _cli.histLineIdx;
               }
               _cli.line = _cli.lineBuf[changedIdx];
               _cli.line.cursorPos = _cli.line.cmdLen;
               _cli.histLineNew = false;
               break;
          }
          case cliSyncRunCmd:
               if(this->sync(cliSyncParseArgs) == true){
                    cliPrintf("\r\n");
                    this->sync(cliSyncToUpper, (void*)_cli.argVector[0]);
                    for (int i = 0; i < _cli.cmdCnt; i++){
                         if(strcmp(_cli.argVector[0], _cli.cmdList[i].cmdStr) == 0){
                              _cli.cmdArgs.argCnt = _cli.argCnt - 1;
                              _cli.cmdArgs.argVector = &_cli.argVector[1];
                              _cli.cmdList[i].cmdFunction(this, &_cli.cmdArgs);
                              break;
                         }
                    }
               }
               break;
          case cliSyncParseArgs:{
               char *tok;
               char *nextPtr;
               uint16_t argCnt = 0;
               const char *delim = " \f\n\r\t\v";
               char *cmdLine;
               char **argVector;

               _cli.argCnt = 0;
               cmdLine = (char *)_cli.line.buf;
               argVector = _cli.argVector;
               argVector[argCnt] = NULL;

               tok = strtok_r(cmdLine, delim, &nextPtr);
               while (tok != NULL){
                    argVector[argCnt++] = tok;
                    tok = strtok_r(NULL, delim, &nextPtr);
               }
               _cli.argCnt = argCnt;
               if(argCnt > 0){
                    return 1;
               }
               return 0;
          }
          case cliSyncToUpper:{
               uint16_t i = 0;
               uint8_t strChar;
               char *str = (char*)arg1;
               for (i = 0; i < CLI_CMD_LEN_MAX; i++){
                    strChar = str[i];
                    if(strChar == 0){
                         break;
                    }
                    if((strChar >= 'a') && (strChar <= 'z')){
                         strChar = strChar - 'a' + 'A';
                    }
                    str[i] = strChar;
               }
               if(i == CLI_CMD_LEN_MAX){
                    str[i - 1] = 0;
               }
               break;
          }
          case cliSyncKeepLoop:
               if(Uart::get()->sync(Uart::uartAvailableDma) == 0){
                    return true;
               }else{
                    return false;
               }
               break;
          case cliSyncShowPrompt:
               cliPrintf("\n\r");
               cliPrintf(CLI_PROMPT_STR);
               break;
          default:
               LOG("Unregistered sync");
               break;
     }
     return 0;
}

int32_t Cli::_cliArgsGetData(void* obj, uint8_t index){
     Cli* cli = static_cast<Cli*>(obj);
     if (index >= cli->_cli.cmdArgs.argCnt){
          return 0;
     }
     return static_cast<int32_t>(strtoul(cli->_cli.cmdArgs.argVector[index], nullptr, 0));
}

float Cli::_cliArgsGetFloat(void* obj, uint8_t index){
     Cli* cli = static_cast<Cli*>(obj);
     if (index >= cli->_cli.cmdArgs.argCnt) {
          return 0.0f;
     }
     return strtof(cli->_cli.cmdArgs.argVector[index], nullptr);
}

char* Cli::_cliArgsGetStr(void* obj, uint8_t index){
     Cli* cli = static_cast<Cli*>(obj);
     if (index >= cli->_cli.cmdArgs.argCnt) {
          return nullptr;
     }
     return cli->_cli.cmdArgs.argVector[index];
}

bool Cli::_cliArgsIsStr(void* obj, uint8_t index, char *ptrStr){
     Cli* cli = static_cast<Cli*>(obj);
     if (index >= cli->_cli.cmdArgs.argCnt) {
          return false;
     }
     return strcmp(ptrStr, cli->_cli.cmdArgs.argVector[index]) == 0;
}

bool Cli::cliAdd(const char *cmdStr, void (*cmdFunction)(void*, cliArgs*))
{
     if (_cli.cmdCnt >= CLI_CMD_LIST_MAX) {
          return false;
     }

     uint16_t index = _cli.cmdCnt;
     strncpy(_cli.cmdList[index].cmdStr, cmdStr, CLI_CMD_LEN_MAX - 1);
     _cli.cmdList[index].cmdStr[CLI_CMD_LEN_MAX - 1] = '\0';
     _cli.cmdList[index].cmdFunction = cmdFunction;

     this->sync(cliSyncToUpper, _cli.cmdList[index].cmdStr);
     _cli.cmdCnt++;
     return true;
}

void Cli::_cliShowList(void* obj, cliArgs *args){
     Cli* cli = static_cast<Cli*>(obj);
     cliPrintf("\r\n");
     cliPrintf("---------- cmd list ---------\r\n");
     for (int i = 0; i < cli->_cli.cmdCnt; i++){
          cliPrintf("%s", cli->_cli.cmdList[i].cmdStr);
          cliPrintf("\r\n");
     }
     cliPrintf("-----------------------------\r\n");
}
void Cli::_cliMemoryDump(void* obj, cliArgs *args){
     Cli* cli = static_cast<Cli*>(obj);
     int wordIndex, dumpSize = 16;
     uint32_t *startAddr;
     int asciiGroupIndex, byteIndex;
     uint32_t *asciiPtr;
     uint8_t asciiBytes[4];

     int argc = args->argCnt;
     char **argv = args->argVector;
     if(argc < 1){
          cliPrintf(">> md [addr] [size] \n");
          return;
     }
     if(argc > 1){
          dumpSize = static_cast<uint32_t>(strtoul(args->argVector[1], nullptr, 0));
          if(dumpSize > 128){
               dumpSize = 128;
          }
     }
     startAddr = reinterpret_cast<uint32_t*>(strtoul(args->argVector[0], nullptr, 0));
     asciiPtr = startAddr;
     cliPrintf("\n   ");
     for(wordIndex = 0; wordIndex < dumpSize; wordIndex++){
          if((wordIndex % 4) == 0){
               cliPrintf(" 0x%08X: ", reinterpret_cast<uint32_t>(startAddr));
          }
          cliPrintf(" 0x%08X", *(startAddr));
          if((wordIndex % 4) == 3){
               cliPrintf("  |");
               for(asciiGroupIndex = 0; asciiGroupIndex < 4; asciiGroupIndex++){
                    memcpy(asciiBytes, asciiPtr, 4);
                    for(byteIndex = 0; byteIndex < 4; byteIndex++){
                         if(asciiBytes[byteIndex] > 0x1F && asciiBytes[byteIndex] < 0x7F){
                              cliPrintf("%c", asciiBytes[byteIndex]);
                         }else{
                              cliPrintf(".");
                         }
                    }
                    asciiPtr += 1;
               }
               cliPrintf("|\n   ");
          }
          startAddr++;
     }
}
#endif