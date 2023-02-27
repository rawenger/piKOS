#pragma once

void uart0_init();
void uart0_send(char c);
char uart0_recv();
void uart0_send_str(const char *str);

