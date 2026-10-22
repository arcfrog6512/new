#include <STC15F2K60S2.h>
#define uchar unsigned char
#define uint  unsigned int

/**************** 硬件引脚定义（可根据实际连线修改） ****************/
// 步进电机A/B/C/D四相接P1.0~P1.3
sbit MOTOR_A = P1^0;
sbit MOTOR_B = P1^1;
sbit MOTOR_C = P1^2;
sbit MOTOR_D = P1^3;

/**************** 四相八拍正转节拍表 ****************/
// 通电顺序：A → AB → B → BC → C → CD → D → DA
uchar code step_table[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
uchar step_idx = 0; // 当前节拍下标

/**************** 1ms精准延时 @11.0592MHz ****************/
void delay_ms(uint n)		
{
	while(n){
		uchar i, j;
		i = 11;
		j = 190;
		do { while (--j); } while (--i);
		n--;
	}
}

/**************** 正转1步 ****************/
void motor_forward_step()
{
	// 输出当前节拍电平到四相
	MOTOR_A = step_table[step_idx] & 0x01;
	MOTOR_B = (step_table[step_idx] >> 1) & 0x01;
	MOTOR_C = (step_table[step_idx] >> 2) & 0x01;
	MOTOR_D = (step_table[step_idx] >> 3) & 0x01;
	
	// 节拍下标循环+1
	step_idx++;
	if(step_idx >= 8) step_idx = 0;
}

/**************** 电机停止断电 ****************/
void motor_stop()
{
	MOTOR_A = 0;
	MOTOR_B = 0;
	MOTOR_C = 0;
	MOTOR_D = 0;
}

/**************** 主函数 ****************/
void main()
{
	uint i;
	// IO初始化：P1低四位设为推挽输出
	P1M1 &= 0xf0;
	P1M0 |= 0x0f;
	
	motor_stop(); // 初始电机断电
	
	// 正转40步，10步/秒 → 每步间隔100ms
	for(i = 0; i < 40; i++)
	{
		motor_forward_step(); // 走1步
		delay_ms(100);        // 步间隔100ms，对应10步/秒
	}
	
	motor_stop(); // 走完40步后停止
	while(1);     // 程序结束死循环
}
