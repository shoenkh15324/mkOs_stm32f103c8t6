#pragma once
#if PROJECT_CLI_ENABLE

#define CLI_KEY_BACK          0x08
#define CLI_KEY_DEL           0x7F
#define CLI_KEY_ENTER         0x0D
#define CLI_KEY_ESC           0x1B
#define CLI_KEY_LEFT          0x44
#define CLI_KEY_RIGHT         0x43
#define CLI_KEY_UP            0x41
#define CLI_KEY_DOWN          0x42
#define CLI_KEY_HOME          0x31
#define CLI_KEY_END           0x34

#define CLI_PROMPT_STR        "cli: " // CLI 프롬프트(prompt)에 출력될 문자열을 정의

#define CLI_ARGS_MAX          32  // CLI 명령어에서 사용할 수 있는 최대 인자(argument)의 수
#define CLI_PRINT_BUF_MAX     256 // CLI에서 출력할 수 있는 최대 버퍼 크기
#define CLI_CMD_LIST_MAX      16 // 최대 CLI 명령어의 수
#define CLI_CMD_LEN_MAX       16 // CLI 명령어의 최대 길이
#define CLI_CMD_HIS_MAX       5  // 명령어 히스토리의 최대 크기
#define CLI_LINE_BUF_MAX      64 // 명령어 입력 버퍼의 최대 크기

class Cli : public Object{
public:
     enum{// Cli State
          cliStateEscapeWait = objStateBegin,
          cliStateEscapeSequence,
          cliStateEscapeKeyWait,
          cliStateEscapeFinalize,
     };
     enum{ // Cli Sync
          cliSyncMain = Object::syncBegin,
          cliSyncUpdate,
          cliSyncLineClean,
          cliSyncLineAdd,
          cliSyncLineChange,
          cliSyncRunCmd,
          cliSyncParseArgs,
          cliSyncToUpper,
          cliSyncKeepLoop,
          cliSyncShowPrompt,
          cliSyncShowList,
     };

private:
     typedef struct{
          uint8_t buf[CLI_LINE_BUF_MAX];
          uint8_t bufLen;
          uint8_t cursorPos;
          uint8_t cmdLen;
     } cliLine;
     typedef struct{
          uint16_t argCnt;
          char **argVector;
          int32_t (*getData)(void*, uint8_t index);
          float (*getFloat)(void*, uint8_t index);
	     char *(*getStr)(void*, uint8_t index);
	     bool (*isStr)(void*, uint8_t index, char *str);
     } cliArgs;
     typedef struct{
          char cmdStr[CLI_CMD_LEN_MAX];
          void (*cmdFunction)(void*, cliArgs *);
     } cliCmd;
     typedef struct{
          uint8_t ch;
          uint16_t argCnt;
          char *argVector[CLI_ARGS_MAX];
          bool histLineNew;
          int8_t histLineIdx;
          uint8_t histLineLast;
          uint8_t histLineCnt;
          cliLine lineBuf[CLI_CMD_HIS_MAX];
          cliLine line;
          uint16_t cmdCnt;
          cliCmd cmdList[CLI_CMD_LIST_MAX];
          cliArgs cmdArgs;
     } cliInfo;
private:
     static Cli _instance;
     Cli();
     Cli(const Cli &) = delete;
     Cli& operator=(const Cli &) = delete;
public:
     static inline Cli *get() { return &_instance; }
private:
     cliInfo _cli;
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
     bool cliAdd(const char *, void (*cmdFunction)(void*, cliArgs*));
protected:
     static int32_t _cliArgsGetData(void*, uint8_t);
     static float _cliArgsGetFloat(void*, uint8_t);
     static char *_cliArgsGetStr(void*, uint8_t);
     static bool _cliArgsIsStr(void*, uint8_t index, char *);
     static void _cliShowList(void*, cliArgs *);
     static void _cliMemoryDump(void*, cliArgs *);
};
#endif