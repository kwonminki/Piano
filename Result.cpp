#include "Header_Init.h"

Result::Result(int keynum, clock_t startclock)
{
	this->key_num = keynum;
	this->start = startclock;
}

void Result::End(clock_t endclock)
{
	this->end = endclock;
	this->result = (double)(this->end - this->start);
}