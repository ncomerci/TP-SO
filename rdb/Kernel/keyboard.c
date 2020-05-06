#include <keyboard.h>
#include <naiveConsole.h>
#include <screen.h>
#include <process.h>
#include <lib.h>

#define BUFFER_SIZE 128

static char buffer[BUFFER_SIZE];
static int buffer_size = 0;
static int buffer_idx = 0;
static int waiting_pid = 0;
static int process_waiting = 0;

#define SHIFT_IN_KEY 

enum SPECIAL_KEYS {SHIFT_IN = -20, SHIFT_OUT, CAPS, BACKS, ENTER, ESC, CTRL_IN, CTRL_OUT, ALT,F1, F2, ARROW_UP, ARROW_LEFT,ARROW_RIGHT, ARROW_DOWN, INS, DEL};

//! US keyboard keymap :: regular keys.
static int regular_f[128] = {
  0x0000,ESC,'1','2','3','4','5','6','7','8','9','0','-','=',BACKS,'\t', // Indexes 0 to 15
  'q','w','e','r','t','y','u','i','o','p','[',']','\n',CTRL_IN,'a','s', // Indexes 16 to 31
  'd','f','g','h','j','k','l',';','\'','`',SHIFT_IN,'\\','z','x','c','v', // Indexes 32 to 47
  'b','n','m',',','.','/',SHIFT_IN,0x372A /*??*/,ALT,' ',CAPS,F1,F2,0x3D00,0x3E00,0x3F00, // Indexes 48 to 63
  0x4000,0x4100,0x4200,0x4300,0x4400,0x4500,0x4600,0x4700,ARROW_UP,0x4900,0x4A2D,ARROW_LEFT,0x4C00,ARROW_RIGHT,0x4E2B,0x4F00, // Indexes 64 to 79
  ARROW_DOWN,0x5100,INS,DEL,0x5400,0x5500,0x5600,0x8500,0x8600,0x0000,0x0000,0x5B00,0x5C00,0x5D00 // Indexes 80 to 93
};

//! US keyboard keymap :: "with SHIFT" keys.
static int with_shift_f[128] = {
  0x0000,0x011B,'!','@','#','$','%','^','&','*','(',')','_','+',0x0E08,0x0F00, // Indexes 0 to 15
  'Q','W','E','R','T','Y','U','I','O','P','{','}',0x1C0D,CTRL_IN,'A','S', // Indexes 16 to 31
  'D','F','G','H','J','K','L',':','"','~',SHIFT_IN,'|','Z','X','C','V', // Indexes 32 to 47
  'B','N','M','<','>','?',0x3600,0x372A,0x3800,0x3920,0x3A00,0x5400,0x5500,0x5600,0x5700,0x5800, // Indexes 48 to 53
  0x5900,0x5A00,0x5B00,0x5C00,0x5D00,0x4500,0x4600,0x4700,0x4800,0x4900,0x4A2D,0x4B00,0x4C00,0x4D00,0x4E2B,0x4F00, // Indexes 54 to 69
  0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x8700,0x8800,0x0000,0x0000,0x5B00,0x5C00,0x5D00 // Indexes 70 to 83
};

static int SHIFTED, CAPS_LOCKED, CTRLED;
static int SPCL_KEYS[] = {14, 29, 42, 54, 56, 58, 59, 60, 72, 75, 77, 80, 157, 170, 182};
static int SPCL_CODES[] = {BACKS, CTRL_IN, SHIFT_IN, SHIFT_IN, ALT, CAPS, F1, F2, ARROW_UP, ARROW_LEFT, ARROW_RIGHT, ARROW_DOWN, CTRL_OUT, SHIFT_OUT, SHIFT_OUT};

static func keyHandler[2][2] = {{normalKey, shiftedKey}, //1ra posicion SHIFT, 2da posicion CAPSLOCK
                                {shiftedKey, normalKey}};

void normalKey(uint8_t aux) { 
  buffer[(buffer_idx + buffer_size) % BUFFER_SIZE] = regular_f[aux];
  buffer_size++;
}
void shiftedKey(uint8_t aux) { 
  buffer[(buffer_idx + buffer_size) % BUFFER_SIZE] = with_shift_f[aux];
  buffer_size++;
}

void keyboard_handler(void) {
  uint8_t aux = kbGet();
  int spec = special_key(aux);

  if (spec < 0) {
    switch (spec) {
      case SHIFT_IN:
        SHIFTED = 1;
        break;
      case SHIFT_OUT:
        SHIFTED = 0;
        break;
      case CAPS:
        CAPS_LOCKED = 1 - CAPS_LOCKED;
        break;
      case CTRL_IN:
        CTRLED = 1;
        break;
      case CTRL_OUT:
        CTRLED = 0;
    }
  }

  if (spec < 0) {
    if (spec != SHIFT_IN && spec != SHIFT_OUT && spec != CAPS && spec != CTRL_IN && spec != CTRL_OUT) {
      buffer[(buffer_idx + buffer_size) % BUFFER_SIZE] = spec;
      buffer_size++;
      if (process_waiting) {
        process_waiting = 0;
        changeState(waiting_pid, READY);
      }
    }
  }
  else if(aux < 128 && aux > 0) {
    /*
    **la idea es que si es una letra, la matriz se encarga de devolver si es minus o mayus
    **pero si no es letra, solo depende de si está o no presionado el SHIFT
    */
    if (CTRLED && regular_f[aux] == 'c') {
      int pid;
      getPid(&pid);
      if (pid > 1 && isCurrentForeground()) // Not shell
        changeState(pid, KILLED);
    }
    else {
      if (regular_f[aux] >= 'a' && regular_f[aux] <= 'z')
        keyHandler[SHIFTED][CAPS_LOCKED](aux);
      else
        keyHandler[SHIFTED][0](aux);
      if (process_waiting) {
        process_waiting = 0;
        changeState(waiting_pid, READY);
      }
    }
  }
}

// Returns 0 if it´s not a special key.
// Returns the special key code.
int special_key(uint8_t key) {
  for (int i = 0; i < sizeof(SPCL_KEYS)/sizeof(int); i++) {
    if (key == SPCL_KEYS[i])
      return SPCL_CODES[i];
  }
  return 0;
}

// Returns 0 if something has been read
int kbRead(char * buff, unsigned int count) {
  if (count == 0)
    return 0;

  if (buffer_size <= 0) {
    int pid;
    getPid(&pid);
    if (isCurrentForeground()) {
      process_waiting = 1;
      waiting_pid = pid;
    }
    changeState(pid, BLOCKED);
  }

  unsigned int size = ((count < buffer_size)?count:buffer_size) * sizeof(char);
  unsigned int i = 0;
  while (i < size) {
      buff[i] = buffer[(buffer_idx + i) % BUFFER_SIZE];
      i++;
  }
  
  buffer_idx = (buffer_idx + i) % BUFFER_SIZE;
  buffer_size -= size;

  return size;
}