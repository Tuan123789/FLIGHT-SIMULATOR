#include <wiringPi.h> // -lwiringPi 
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdint.h>  //int16_t
#include <math.h>  // gcc ...-lm
#include <mysql.h>
#include <stdlib.h>
#define Sample_rate		25
#define Config 			26
#define Gyro_config		27
#define Acc_config		28
#define Inrerrupt		56
#define PWR_Management	107

#define BT_LG 29      // landing gear
#define BT_FU 22      // flap up
#define BT_FD 16      // flap down
#define BT_STOP 13     // tam dung
#define BT_UG 38      // up gas
#define BT_DG 40      // down gas
#define BT_EXIT 11    // thoat game


int8_t a=0,b=0,c=0,d=0,e=0,f=0,g = 0;

int mpu;

float data;
void ngat1(void){
	while(digitalRead(BT_LG)==1)
    {
        delay(20);
        while(digitalRead(BT_LG)==1)
        {
		a = ~a;
		while(digitalRead(BT_LG) ==1);
        }  
	}
}
void ngat2(void){
	while(digitalRead(BT_FU)==1)
    {
        delay(20);
        while(digitalRead(BT_FU)==1)
        {
		b = ~b;
		while(digitalRead(BT_FU) ==1);
        }  
	}
}
void ngat3(void){
	while(digitalRead(BT_FD)==1)
    {
        delay(20);
        while(digitalRead(BT_FD)==1)
        {
		c= ~c;
		while(digitalRead(BT_FD) ==1);
        }  
	}
}
void ngat4(void){
	while(digitalRead(BT_STOP)==1)
    {
        delay(20);
        while(digitalRead(BT_STOP)==1)
        {
		d = ~d;
        }  
	}
}
void ngat5(void){
	while(digitalRead(BT_UG)==1)
    {
        delay(20);
        while(digitalRead(BT_UG)==1)
        {
		e = ~e;
		while(digitalRead(BT_UG) ==1);
        }  
	}
}
void ngat6(void){
	while(digitalRead(BT_DG) ==1)
	{
		delay(20);
		while(digitalRead(BT_DG) ==1)
		{
			f = ~f;
			while(digitalRead(BT_DG) ==1);
		}  
	}
}
void ngat7(void){
	while(digitalRead(BT_EXIT) ==1)
	{
		delay(20);
		while(digitalRead(BT_EXIT) ==1)
		{
			g = ~g;
			while(digitalRead(BT_EXIT) ==1);
		}  
	}
}
struct mpu_value{
	float x;
	float y;
	float z;
};


void init_6050()
{
	// thanh ghi toi thieu can cau hinh: 25 -> 28, 56, 107
	// wiringPiI2CWriteReg16 cho thanh ghi 16 bit.
	// sample_rate 500Hz
	wiringPiI2CWriteReg8(mpu, Sample_rate, 15);  // 25 la so thap phan, do datasheet de dia chi thanh ghi o 2 dang hex va thap phan.
	// Khong dung nguon xung ngoai, tat DLFP
	wiringPiI2CWriteReg8(mpu, Config, 0);  // ..(thiet bi can giao tiep, dia chi thanh ghi, gia tri gui)
	// gyro FS: +- 500 do/s
	wiringPiI2CWriteReg8(mpu, Gyro_config, 0x08);
	// acc FS: +- 8g
	wiringPiI2CWriteReg8(mpu, Acc_config, 0x10);
	// mo interrupt cua data ready
	wiringPiI2CWriteReg8(mpu, Inrerrupt, 1);
	// chon nguon xung Gyro X
	wiringPiI2CWriteReg8(mpu, PWR_Management, 0x01);
}

int16_t read2registers(uint8_t address){
	int16_t data;
	data=wiringPiI2CReadReg8(mpu,address);
	//(65)=0x32->data=0x0032
	data=data<<8;
	data=data|wiringPiI2CReadReg8(mpu,address+1);
	return data;
}
struct mpu_value read_sensor(uint8_t address, float sensitivity){
	struct mpu_value data;
	data.x=(float)read2registers(address)/sensitivity;
	data.y=(float)read2registers(address+2)/sensitivity;
	data.z=(float)read2registers(address+4)/sensitivity;
	return data;
}

int main()
{
	wiringPiSetupPhys();

	MYSQL *conn;
	
	//pinMode(BT_LG, INPUT);
	//pinMode(BT_FU, INPUT);
	//pinMode(BT_FD, INPUT);
	//pinMode(BT_STOP, INPUT);
	//pinMode(BT_UG, INPUT);
	//pinMode(BT_EXIT, INPUT);
	
	wiringPiISR(BT_LG, INT_EDGE_RISING, &ngat1);
	wiringPiISR(BT_FU, INT_EDGE_RISING, &ngat2);
	wiringPiISR(BT_FD, INT_EDGE_RISING, &ngat3);
	wiringPiISR(BT_STOP, INT_EDGE_RISING, &ngat4);
	wiringPiISR(BT_UG, INT_EDGE_RISING, &ngat5);
	wiringPiISR(BT_DG, INT_EDGE_RISING, &ngat6);
	wiringPiISR(BT_EXIT, INT_EDGE_RISING, &ngat7);
	
	char *server = "192.168.1.6";
	char *user = "root";
	char *password = ""; 
	char *database = "mpu";
	// setup giao tiep I2C
	mpu = wiringPiI2CSetup(0x68);
	// Thiet lap che do do cho MPU6050
	init_6050();
	//doc gia tri gia toc
	struct mpu_value Acc;
	
	while(1){
		// ket noi database
		conn = mysql_init(NULL);
		mysql_real_connect(conn,server,user,password,database,0,NULL,0); 
		
		Acc = read_sensor(59,4096.0);
		delay(50);
		float x = atan2(Acc.x,sqrt(pow(Acc.y,2)+pow(Acc.z,2)))*180/M_PI;
		float y = atan2(Acc.y,sqrt(pow(Acc.x,2)+pow(Acc.z,2)))*180/M_PI;
		//printf("%f %f %f \n",Acc.x,Acc.y,Acc.z);
		char value[200];
		sprintf(value, "update data_mpu set a = %d, b = %d, c = %d, d = %d, e = %d, f = %d, g = %d, x_avg = %0.3f, y_avg = %0.3f where id = 1;",a,b,c,d,e,f,g,x,y);
		mysql_query(conn,value);
		mysql_close(conn);
		delay(10);
	}
}