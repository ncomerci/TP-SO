#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>
#include <int80.h>
#include <lib.h>
#include <timet.h>

#pragma pack(push)
#pragma pack (1)

typedef struct {
    uint16_t offset_l;
    uint16_t selector;
    uint8_t cero;
    uint8_t access;
    uint16_t offset_m;
    uint32_t offset_h;
    uint32_t other_cero;
} DESCR_INT;

#pragma pack(pop)

DESCR_INT * idt = (DESCR_INT *) 0; //puntero a la idt

void setup_IDT_entry (int index, uint64_t offset) { //recibe el num de interrupción y su rutina de atención
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].selector = 0x08;
  idt[index].cero = 0;
  idt[index].access = ACS_INT;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].other_cero = 0;
}


void load_idt() {
  _cli(); //deshabilito las ints
  
  setup_IDT_entry (0x20, (uint64_t) &_irq00Handler); // Timer tick handler //0x20 timer tick anterior

  // Configuro frecuencia PIC

  int hz = PIT_FREQUENCY;
  int divisor = 1193180 / hz;
  _outportb(PIT_CMDREG, 0x36);
  _outportb(PIT_CHANNEL0, divisor & 0xFF);
  _outportb(PIT_CHANNEL0, divisor >> 8);


  setup_IDT_entry (0x21, (uint64_t) &_irq01Handler); // Keyboard handler

  setup_IDT_entry (0x00, (uint64_t) &_exception0Handler);
  setup_IDT_entry (0x06, (uint64_t) &_exception6Handler);
  
  setup_IDT_entry (0x80, (uint64_t) &_int80Handler); // Soft int handler

  // Interrupción de timer tick y de teclado habilitadas
  picMasterMask(0xFC); //los ultimos estan en 0 ;)
  picSlaveMask(0x00);

  _sti(); //las seteo de nuevo
}
