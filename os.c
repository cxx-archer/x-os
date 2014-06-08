/* 
 * copywright notice
 *
 * the code coming from
 *
 * http://www.amobbs.com/thread-5391072-1-1.html
 *
 * if this offense anyone, please contact 
 *
 * archer.meteor@gmail.com
 *
 *
 *
 * */

#include <regx52.h>

#define MAX_TASKS 5

typedef struct os_task_control_table {
unsigned char os_task_wait_tick;
unsigned char os_task_stack_top;
}TCB;

volatile unsigned char int_count;
volatile unsigned char os_en_cr_count;
#define enter_int() EA=0;int_count++;
#define os_enter_critical() EA=0;os_en_cr_count++;
#define os_exit_critical() if(os_en_cr_count>=1){os_en_cr_count--;if(os_en_cr_count==0)EA=1;}
unsigned char code os_map_tbl[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

volatile unsigned char os_task_int_tbl;
idata volatile TCB os_tcb[MAX_TASKS];
volatile unsigned char os_task_running_id;
volatile unsigned char os_task_rdy_tbl;
unsigned char idata os_task_stack[MAX_TASKS][20];

void os_init(void);
void os_task_create(unsigned char task_id ,unsigned int task_point,unsigned char stack_point);
void os_delay(unsigned char ticks);
void os_start(void);
void os_task_switch(void);
void exit_int(void);

void os_init(void) {
	EA = 0;
	ET2 = 1;
	T2CON = 0X00;
	T2MOD = 0X00;
	RCAP2H = 0x0D8;
	RCAP2L = 0x0F0;
	os_task_rdy_tbl = 0;
	os_task_int_tbl = 0xff;
	int_count = 0;
	os_en_cr_count = 0;
}

void os_task_create(unsigned char task_id ,unsigned int task_point,unsigned char stack_point) {
	os_enter_critical();
	((unsigned char idata *)stack_point)[0] = task_point;
	((unsigned char idata *)stack_point)[1] = task_point>>8;
	os_tcb[task_id].os_task_stack_top = stack_point+14;
	os_task_rdy_tbl |= os_map_tbl[task_id];
	os_tcb[task_id].os_task_wait_tick = 0;
	os_exit_critical();
}

void os_delay(unsigned char ticks) {
	os_enter_critical();
	os_tcb[os_task_running_id].os_task_wait_tick = ticks;
	os_task_rdy_tbl &= ~os_map_tbl[os_task_running_id];
	os_exit_critical();
	os_task_switch();
}

void os_start(void) {
os_task_running_id = 0;
os_tcb[os_task_running_id].os_task_stack_top -= 13;
EA = 1;
SP = os_tcb[os_task_running_id].os_task_stack_top;
TR2 = 1;
}

void os_task_switch(void) {
unsigned char i;
EA = 0;
os_tcb[os_task_running_id].os_task_stack_top = SP;
os_task_int_tbl &= ~os_map_tbl[os_task_running_id];
for(i=0; i<MAX_TASKS; i++) {
if(os_task_rdy_tbl&os_map_tbl[i]) {
break;
}
}
os_task_running_id = i;
SP = os_tcb[os_task_running_id].os_task_stack_top;
if(os_task_int_tbl&os_map_tbl[os_task_running_id]) {
__asm POP 7
__asm POP 6 //»Ö¸´ÈÎÎñ¼Ä´æÆ÷
__asm POP 5
__asm POP 4
__asm POP 3
__asm POP 2
__asm POP 1
__asm POP 0
__asm POP PSW
__asm POP DPL
__asm POP DPH
__asm POP B
__asm POP ACC
}
EA = 1;
__asm RETI
}

void exit_int(void) {
unsigned char i;
SP -= 2;
if(--int_count == 0) {
os_tcb[os_task_running_id].os_task_stack_top = SP;
os_task_int_tbl |= os_map_tbl[os_task_running_id];
for(i=0; i<MAX_TASKS; i++) {
if(os_task_rdy_tbl&os_map_tbl[i]) {
break;
}
}
os_task_running_id = i;
SP = os_tcb[os_task_running_id].os_task_stack_top;
if(os_task_int_tbl&os_map_tbl[os_task_running_id]) {
__asm POP 7
__asm POP 6 //»Ö¸´ÈÎÎñ¼Ä´æÆ÷
__asm POP 5
__asm POP 4
__asm POP 3
__asm POP 2
__asm POP 1
__asm POP 0
__asm POP PSW
__asm POP DPL
__asm POP DPH
__asm POP B
__asm POP ACC
}
EA = 1;
__asm RETI
}
__asm POP 7
__asm POP 6 //»Ö¸´ÈÎÎñ¼Ä´æÆ÷
__asm POP 5
__asm POP 4
__asm POP 3
__asm POP 2
__asm POP 1
__asm POP 0
__asm POP PSW
__asm POP DPL
__asm POP DPH
__asm POP B
__asm POP ACC
EA=1;
__asm RETI
}

void timer2_isr(void) interrupt 5 {
unsigned char i;
TF2=0;
enter_int();
for(i=0; i<MAX_TASKS; i++) {
if(os_tcb[i].os_task_wait_tick) {
os_tcb[i].os_task_wait_tick--;
if(os_tcb[i].os_task_wait_tick == 0) {
os_task_rdy_tbl |= os_map_tbl[i];
}
}
}
exit_int();
}

void task_0(void) {
while(1) {

}
}

sbit seg2 = P2^5;
sbit seg3 = P2^6;
sbit seg4 = P2^7;

void delay_ms(unsigned int xms){
	unsigned int x,y;
	for(x=xms; x>0; x--)
		for(y=248; y>0; y--);
}

unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0};

void task_1(void) {
	unsigned char gw,sw,bw;
	while(1) {
	bw = os_tcb[2].os_task_wait_tick/100;
	sw = os_tcb[2].os_task_wait_tick%100/10;
	gw = os_tcb[2].os_task_wait_tick%10;
	P0 = table[bw];
	seg2=0;
	delay_ms(3);
	seg2=1;
	P0 = table[sw];
	seg3=0;
	delay_ms(3);
	seg3=1;
	P0 = table[gw];
	seg4=0;
	delay_ms(3);
	seg4=1;
	}
}

void task_2(void) {
	unsigned char i;
		while(1) {
		i++;
		P3 = 0x01<<(i%8);
		os_delay(200);
	}
}

void task_3(void) {
	unsigned char i;
	while(1) {
	i++;
	//P2 = 0x01<<(i%8);
	os_delay(7);
	}
}

void task_4(void) {
	unsigned char i;
	while(1) {
		i++;
		P1 = 0x01<<(i%8);
  		os_delay(10);
	}
}

void main(void) {
	os_init();
	os_task_create(4,(unsigned int)&task_0,(unsigned char)os_task_stack[4]);
	os_task_create(3,(unsigned int)&task_1,(unsigned char)os_task_stack[3]);
	os_task_create(2,(unsigned int)&task_2,(unsigned char)os_task_stack[2]);
	os_task_create(1,(unsigned int)&task_3,(unsigned char)os_task_stack[1]);
	os_task_create(0,(unsigned int)&task_4,(unsigned char)os_task_stack[0]);
	os_start();
}
