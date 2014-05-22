/**
  [x] Core Base   
  [x] Debug Mode   
  [x] Translate   
  [x] Random   
  [x] Timer  
  [x] Debug Mode  
  [x] Buzzer 
  [x] Points  
  [x] Levels (Calculating Time/Points...) 
  [x] Interface ( ncurses.h )    
  [x] Error Implementation  
*/	

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <unistd.h>
    #include <time.h>
    #include <math.h>
    #include <curses.h>
    #include "usbio.h"
     
    /** System Definitions */
    #define DEBUG_MODE 0
     
    /** Hardware Definitions */
     
    /** Amount of Targets */
    #define TARGET_TOTAL 4
     
    /** Amount of Leds */
    #define LED_TOTAL 4
     
    /** Buzzer Pin */
    int BUZZER_PIN = PINO_1;
     
    /** Amount Keys */
    #define KEY_TOTAL 4
     
    /** Keys Codes */
    #define KEY_START 2
    #define KEY_ESCAPE 3
    #define KEY_LEVEL 4
    #define KEY_MODE 5
    #define NO_KEY -1
     
    /** Target Pins */
    int TARGETS[TARGET_TOTAL] = {PINO_6, PINO_7, PINO_8, PINO_9};
    int TARGET_CURRENT = 0;
     
    /** Led Pins */
    int LEDS[LED_TOTAL] = {PINO_4, PINO_5, PINO_6, PINO_7};
     
    /** Keys Pins */
    int KEYS[KEY_TOTAL] = {PINO_2, PINO_3, PINO_4, PINO_5};
    int KEYS_CACHE[KEY_TOTAL] = {0, 0, 0, 0};
     
     
     
    /** Game Definitions */
     
    #define GAME_MODE_TOTAL 2
    #define GAME_MODE_ARCADE 0
    #define GAME_MODE_TIME 1
    int GAME_MODE_CURRENT = 0;
    int GAME_MODE[GAME_MODE_TOTAL] = {GAME_MODE_ARCADE, GAME_MODE_TIME};
    char GAME_MODE_LABEL[GAME_MODE_TOTAL][20] = {"Arcade", "Tempo"};
     
    #define LEVEL_TOTAL 3
     
    #define LEVEL_EASY 0
    #define LEVEL_MEDIUM 1
    #define LEVEL_HIGH 2
    char LEVEL_LABEL[LEVEL_TOTAL][20] = {"Fácil", "Médio", "Difícil"};
     
    int LEVEL_CURRENT = 0;
     
     
     
    /** Translation */
    #define GLOBAL_TRANSLATE_SIZE 100
    const char GLOBAL_TRANSLATE[GLOBAL_TRANSLATE_SIZE][100] = { " Pressione a tecla Start para continuar ", // ID - 0
                                                                " Você fez %.0f pontos ", // ID - 1
                                                                " Alvo %d foi atingido ", // ID - 2
                                                                " Fim de Jogo ", // ID - 3
                                                                " Tempo Total: %lf ", // ID - 4
                                                                " Modo de Jogo ", // ID - 5
                                                                " Nível de Dificuldade ", // ID - 6
                                                                " O Jogo vai começar em %d segundos ", // ID - 7
                                                                " Preparar ", // ID - 8
                                                                " Apontar ", // ID - 9
                                                                " Fogo ", // ID - 10
                                                                " Tempo restante %lf ", // ID - 11
                                                                " Tempo esgotado ", // ID - 12
                                                                " Tecla cancel pressionada ", // ID - 13
								" Faltam %d alvos " // ID - 142
                                                               };
     
    bool ERROR_DETECTED = false;
    bool ERROR_CODE = -1;
     
    /** USB I/O Stuff */
    uPrincipal *uMainState;
     
    /** System procedures */
     
    bool isDebuging() {
      return DEBUG_MODE;
    }
     
    /** Hardware procedures */
     
    void setCurrentTarget(int t) {
      if (t < 0 || t > TARGET_TOTAL) {
        t = 0;
      }
      TARGET_CURRENT = t;
    }
     
    void setCurrentGameMode(int m) {
      if ((m < 0) || (m >= GAME_MODE_TOTAL)) {
        m = 0;
      }
      GAME_MODE_CURRENT = m;
    }
     
    void setCurrentLevel(int l) {
      if ((l < 0) || (l >= LEVEL_TOTAL)) {
        l = 0;
      }
      LEVEL_CURRENT = l;
    }
     
    void turnOnLed(int l) {
      setar_saida(LEDS[l], 0);
    }
     
    void turnOffLed(int l) {
      setar_saida(LEDS[l], 1);
    }
     
    void turnOffAllLeds() {
      int i = 0;
      for (i = 0; i <= LED_TOTAL; i++) {
        turnOffLed(i);
      }
    }
     
    void turnOnAllLeds() {
      int i = 0;
      for (i = 0; i <= LED_TOTAL; i++) {
        turnOnLed(i);
      }
    }
     
    void blinkAllLeds(int count, double _sOn, double _sOff) {
      int i = count;
      for (i = count; i > 0; i--) {
        turnOnAllLeds();
        sleep(_sOn);
        turnOffAllLeds();
        sleep(_sOff);
      }
    }
     
    void turnOnBuzzer(){
      setar_saida(BUZZER_PIN, 1);
    }
     
    void turnOffBuzzer(){
      setar_saida(BUZZER_PIN, 0);
    }
    
    int getRandTarget() {
        srand((unsigned) time(NULL));
	//return 0;
	return rand() % TARGET_TOTAL;
    }
     
    void beepBuzzer(float time) {
      //turnOnBuzzer();
      //sleep(time);
      //turnOffBuzzer();
    }
     
    void beepShort() {
      beepBuzzer(0.5);
    }
     
    void beepLong() {
      beepBuzzer(1);
    }
     
    int getCurrentTarget() {
      return getTarget(TARGET_CURRENT);
    }
     
    int getCurrentTargetNumber() {
      return (TARGET_CURRENT);
    }
     
    int getTarget(int t) {
      return TARGETS[t];
    }
     
    int getCurrentLevel() {
      return (LEVEL_CURRENT);
    }
     
    int getCurrentGameMode() {
      return GAME_MODE[GAME_MODE_CURRENT];
    }
     
    char * getLevelLabel(int i) {
      return LEVEL_LABEL[i];
    }
     
    char * getGameModeLabel(int i) {
      return GAME_MODE_LABEL[i];
    }
     
    const char * translate(int i) {
      return GLOBAL_TRANSLATE[i];
    }
     
    const char * _t(int i) {
      return translate(i);
    }
     
    int getKey(int k) {
      return KEYS[k];
    }
     
    bool readTarget(int t) {
      return ler_entrada(getTarget(t));
    }
     
    bool readCurrentTarget() {
      return readTarget(getCurrentTargetNumber());
    }
     
    void holdIntoStartPressed() {
      bool _pressed = false;
      while (!_pressed) {
        _pressed = (readKeyboard() == KEY_START);
      }
    }
     
    void changeGameMode() {
      int nextGameMode = getCurrentGameMode() + 1;
      setCurrentGameMode(nextGameMode);
    }
     
    void changeLevel() {
      int nextLevel = getCurrentLevel() + 1;
      setCurrentLevel(nextLevel);
    }
     
    int readKeyboard() {
      int a;
      for (a = 0; a < KEY_TOTAL; a++) {
        int key = getKey(a);
        bool _wasPressed = ler_entrada(key);
        if (_wasPressed && !(KEYS_CACHE[a] == 1)) {
          KEYS_CACHE[a] = 1;
          return key;
        }
        if (!_wasPressed) {
          KEYS_CACHE[a] = 0;
        }
      }
      return NO_KEY;
    }
     
    double timeDiff(struct timeval _timeS, struct  timeval _timeF){
      return (double) ((1000.0 * (_timeF.tv_sec - _timeS.tv_sec) + (_timeF.tv_usec - _timeS.tv_usec) / 1000.0) / 1000.0);
    }
     
    void readyGoGoGo(){
      int i = 1;
      for (i = 1; i <= 3; i++ ){
       
        move(5+i, 5);
        printw(_t(7+i));
        refresh();
        turnOnAllLeds();
        turnOnBuzzer();
        sleep(1);
        turnOffAllLeds();
	      turnOffBuzzer();
        refresh();
        sleep(1);
       
      }
      turnOffBuzzer();
    }
     
    void gameModeTime() {
      bool _hit = false;
      bool _cancel = false;
      struct timeval _timeS, _timeF;
      double _timeR;
     
      clear();
      move(5, 1);
      printw(_t(7),3);
      refresh();
      sleep(1);
      readyGoGoGo();
      turnOffBuzzer();
      int _rand = getRandTarget();
      
     
      setCurrentTarget(_rand);
      turnOnLed(getCurrentTargetNumber());
     
      gettimeofday(&_timeS, NULL);
     
      while (!_hit && !_cancel) {
        gettimeofday(&_timeF, NULL);
        if (readCurrentTarget()) {
          _hit = true;
        }
        else if (readKeyboard() == KEY_ESCAPE) {
          _hit = true;
          _cancel = true;
        }
      }
     
     
      _timeR = timeDiff(_timeS, _timeF);
      turnOffAllLeds();
     
      clear();
      move(6, 5);
      printw(_t(4));
      move(7, 5);
      printw(_t(4), _timeR);
      move(9, 5);
      printw(_t(0));
      refresh();
      holdIntoStartPressed();
     
    }
     
    void gameModeArcade() {
      int _hits = 0;
      int _maxHits = 0;
      bool _cancel = false;
      bool _timeCancel = false;
     
     
      struct timeval _timeS, _timeF;
      double _timeR, _timeRTemp, _maxTime, _points;
     
      clear();
      move(5, 1);
      printw(_t(7),3);
      refresh();
      sleep(2);
      readyGoGoGo();
      turnOffBuzzer();
      gettimeofday(&_timeS, NULL);
     
      _maxHits = 10;
      _maxTime = ((_maxHits-5) * ((LEVEL_TOTAL - getCurrentLevel() + 1) * 2));
     
     
     
      while ((_hits < _maxHits) && !_cancel) {
        int _rand = getRandTarget() ;
        
        turnOffAllLeds();
        setCurrentTarget(_rand);
        turnOnLed(getCurrentTargetNumber());
        bool _hit = false;
        while (!_hit) {
          gettimeofday(&_timeF, NULL);
          _timeRTemp = timeDiff(_timeS, _timeF);
          move(10,10);
          printw(_t(11),_maxTime - _timeRTemp);
	        move(11,10);
          printw(_t(14), _maxHits - _hits);
          
          refresh();
          if (readCurrentTarget()) {
	          move(12,10);
            printw(_t(2), getCurrentTargetNumber());
            _hits++;
            _hit = true;
            usleep(250000);
          }
          else if ((readKeyboard() == KEY_ESCAPE)) {
            _hit = true;
            _cancel = true;
          }
          else if (_timeRTemp >= _maxTime) {
            _hit = true;
            _cancel = true;
            _timeCancel = true;
          }
        }
      }
     
      turnOffAllLeds();
     
      gettimeofday(&_timeF, NULL);
      _timeR = timeDiff(_timeS, _timeF);
     
      _points = (((_hits * 7.352) * 1000) / _timeR);
      clear();
      move(7, 1);
      if (_cancel) {
        if (_timeCancel) {
          printw(_t(12));
        }
        else {
          printw(_t(13));
        }
      }
      else {
        printw(_t(3));
        move(8, 1);
        printw(_t(1), _points);
      }
      move(9, 1);
      printw(_t(0));
      refresh();
      holdIntoStartPressed();
    }
     
    void play() {
      switch (getCurrentGameMode()) {
        case GAME_MODE_ARCADE:
          gameModeArcade();
          break;
        case GAME_MODE_TIME:
          gameModeTime();
          break;
        default:
          break;
      }
    }
     
    void keyboardHandler() {
      int key = readKeyboard();
      switch (key) {
        case KEY_MODE:
          changeGameMode();
          break;
        case KEY_LEVEL:
          changeLevel();
          break;
        case KEY_START:
          play();
          break;
        default:
          break;
      }
    }
     
    void setup() {
      setbuf(stdout, NULL);
      // USB I/O Init Setup
      uMainState = (uPrincipal *) malloc(sizeof (sPrincipal));
      iniciar(uMainState);
     
      // Curses Init Setup
      initscr();
      start_color();
     
      init_pair(1, COLOR_RED, COLOR_WHITE);
      init_pair(2, COLOR_BLUE, COLOR_WHITE);
      init_pair(3, COLOR_GREEN, COLOR_WHITE);
      bkgd(COLOR_PAIR(1));
      attron(COLOR_PAIR(1));
      // Simple Led Test
      blinkAllLeds(3,0.5,0.5);
      turnOffBuzzer();
      }
     
      void shutdown() {
      // USB I/O Shutdown
      finalizar();
      free(uMainState);
      // Curses Shutdown
      endwin();
    }
     
    void homeScreen() {
      move(5, 1);
      printw(_t(5));
      printw(" %s ", getGameModeLabel(getCurrentGameMode()));
      move(6, 1);
      printw(_t(6));
      printw(" %s ", getLevelLabel(getCurrentLevel()));
      refresh();
      clear();
    }
     
    int main(void) {
      setup();
     
     
      while (!ERROR_DETECTED) {
        keyboardHandler();
        homeScreen();
      }
      shutdown();
      return EXIT_SUCCESS;
    }