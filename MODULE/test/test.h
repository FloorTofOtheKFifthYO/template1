#ifndef _TEST_H_
#define _TEST_H_

#include "sys.h"
#include "configuration.h"

#define TEST_RX_ID 0x40
#define TEST_TX_ID 0x41

void test_init();

void test_left_target(u8 number);

void test_right_target(u8 number);

#endif
