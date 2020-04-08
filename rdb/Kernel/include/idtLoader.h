#ifndef IDTLOADER_H_
#define IDTLOADER_H_

void setup_IDT_entry (int index, uint64_t offset);
void load_idt();

#endif