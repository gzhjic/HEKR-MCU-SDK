#include "hekr_protocol.h"

unsigned char Hekr_Send_Buffer[20];
unsigned char Valid_Data[20];
unsigned char Module_Status[20];

static void Hekr_Send_Byte(unsigned char ch)
{
	UART1_SendChar(ch);
}


unsigned char Hekr_RecvData_Handle(unsigned char* data)
{
	//����У��
	if(Sum_Check_Is_Err(data))
	{
		Err_Response(Error_Sum_Check);
		return RecvData_SumCheckErr;
	}
	//ȷ��֡����
	switch(data[2])
	{
	case Device_Upload_Type://MCU�ϴ���Ϣ���� ����Ҫ���� 
													return MCU_Upload_ACK;
													break; 	 
	case Module_Download_Type://WIFI�´���Ϣ
													Hekr_Send_Frame(data);
													Hekr_ValidData_Copy(data);
													return Valid_Data_Update;
													break;  
	case Module_Operation_Type://Hekrģ��״̬
													Hekr_Module_State_Copy(data);
													return Hekr_Module_State_Update;
													break; 
	case Error_Frame_Type://��һ֡���ʹ���	
													return Last_Frame_Send_Err;
													break;			 
	default:Err_Response(Error_No_CMD);break;
	}
	return RecvData_Useless;
}

void Hekr_ValidData_Upload(void)
{
	unsigned char i;
	unsigned char len = Led_Control_Frame_Length-6;
	Hekr_Send_Buffer[0] = Hekr_Frame_Header;
	Hekr_Send_Buffer[1] = Led_Control_Frame_Length;
	Hekr_Send_Buffer[2] = Device_Upload_Type;
	Hekr_Send_Buffer[3] = 0x01;
	for(i = 0; i < len ; i++)
		Hekr_Send_Buffer[i+4] = Valid_Data[i];
	Hekr_Send_Frame(Hekr_Send_Buffer);
}

void Hekr_Module_Control(unsigned char data)
{
	Hekr_Send_Buffer[0] = Hekr_Frame_Header;
	Hekr_Send_Buffer[1] = Module_Query_Frame_Length;
	Hekr_Send_Buffer[2] = Module_Operation_Type;
	Hekr_Send_Buffer[3] = 0x01;
	Hekr_Send_Buffer[4] = data;
	Hekr_Send_Buffer[5] = 0x00;
	Hekr_Send_Frame(Hekr_Send_Buffer);
}



static void Hekr_Send_Frame(unsigned char *data)
{
	unsigned char len = data[1];
	unsigned char i = 0;
	data[len-1] = Sum_Calculate(data);
	for(i = 0 ; i < len ; i++)
	{
		Hekr_Send_Byte(data[i]);
	}
}

static unsigned char Sum_Check_Is_Err(unsigned char* data)
{
	unsigned char temp = Sum_Calculate(data);
	unsigned char len = data[1] - 1;
	if(temp == data[len])
		return 0;
	return 1;
}

static unsigned char Sum_Calculate(unsigned char* data)
{
	unsigned char temp;
	unsigned char i;
	unsigned char len = data[1] - 1;
	temp = 0;
	for(i = 0;i < len; i++)
	{
			temp += data[i];
	}
	return temp;
}

static void Err_Response(unsigned char data)
{
	Hekr_Send_Buffer[0] = Hekr_Frame_Header;
	Hekr_Send_Buffer[1] = Error_Frame_Length;
	Hekr_Send_Buffer[2] = Error_Frame_Type;
	Hekr_Send_Buffer[3] = 0x00;
	Hekr_Send_Buffer[4] = data;
	Hekr_Send_Buffer[5] = 0x00;
	Hekr_Send_Frame(Hekr_Send_Buffer);
}

static void Hekr_ValidData_Copy(unsigned char* data)
{
	unsigned char len,i;
	len = data[1]- 6;
	for(i = 0 ;i < len ; i++)
		Valid_Data[i] = data[i+4];
}

static void Hekr_Module_State_Copy(unsigned char* data)
{
	unsigned char len,i;
	len = data[1]- 6;
	for(i = 0 ;i < len ; i++)
		Module_Status[i] = data[i+4];
}

