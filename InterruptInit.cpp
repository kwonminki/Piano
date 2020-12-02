#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "Header_Init.h"
#define timer 1000000
#define UNDER 50000

void PinISRRise();
void PinISRFall();

extern clock_t before;

clock_t before_rise = clock();
clock_t before_fall = clock();

int tmp_num = 61;

void InterruptInit()
{
	wiringPiSetupPhys();
	pinMode(11, INPUT);
	pinMode(13, INPUT);
	pinMode(15, INPUT);
	pinMode(19, INPUT);
	pinMode(21, INPUT);
	pinMode(23, INPUT);
	pinMode(33, INPUT);
	pinMode(35, INPUT);
	pinMode(37, INPUT);
	pinMode(12, INPUT);
	pinMode(16, INPUT);
	pinMode(18, INPUT);
	pinMode(22, INPUT);
	pinMode(32, INPUT);
	pinMode(36, INPUT);
	pinMode(38, INPUT);
	pinMode(40, OUTPUT);
 
 

	if (wiringPiISR(37, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(12, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(16, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(18, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(22, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(32, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(36, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}

	if (wiringPiISR(38, INT_EDGE_FALLING, &PinISRFall) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;


		if (wiringPiISR(11, INT_EDGE_RISING, &PinISRRise) < 0) {

			fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

			return;
		}
  

	}
	if (wiringPiISR(13, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(15, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(19, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(21, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(23, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
	if (wiringPiISR(33, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}

	if (wiringPiISR(35, INT_EDGE_RISING, &PinISRRise) < 0) {

		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));

		return;

	}
}

void PinISRFall(void)
{
	
	clock_t now = clock();
	cout << (double)(now - before_fall) << " <<<<<<<< fall" <<endl;
	if ((double)(now - before_fall) < UNDER) {
		return;
	}
	before_fall = clock();
	
	WhenFalling(tmp_num);
	//cout << "<<<<<<<<<<<<<<what?>>>>>>>>>>>>>>" << endl;
EXIT2:
	return;
}

void PinISRRise(void) //눌렸을 때
{ 
	clock_t now = clock();
	cout << (double)(now - before_fall) << " <<<<<<<< rise" << endl;
	if((double)(now - before_rise) < UNDER) {
		return;
	}
	before_rise = clock();

	int arr1[8];
	int arr2[8];
	int key_num;
	
	arr1[0] = digitalRead(40);
	arr1[1] = digitalRead(13);
	arr1[2] = digitalRead(15);
	arr1[3] = digitalRead(19);
	arr1[4] = digitalRead(21); 
	arr1[5] = digitalRead(23); 
	arr1[6] = digitalRead(35); 
	arr1[7] = digitalRead(33);
	arr2[0] = digitalRead(37);
	arr2[1] = digitalRead(12);
	arr2[2] = digitalRead(16);
	arr2[3] = digitalRead(18);
	arr2[4] = digitalRead(22); 
	arr2[5] = digitalRead(32); 
	arr2[6] = digitalRead(36); 
	arr2[7] = digitalRead(38);
	
            
	for (int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if((arr1[i]*arr2[j])*(8*(i)*arr1[i]+j*arr2[j]+1)!=0){
				key_num = i + (j) * 8;
				//if (tmp_num == key_num)return;
				tmp_num = key_num;
				cout << "==============" << key_num << endl;
				WhenRising(key_num);
				goto EXIT;
			}
		}
	}
	cout << "what?" << endl;
EXIT:
	return;	
}

