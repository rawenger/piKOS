#pragma once

void muart_init (void );
char muart_recv (void );
void muart_send (char c );
void muart_send_str(char* str);
