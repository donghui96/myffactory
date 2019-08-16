// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ�á���"espconn.h"��ͷ#include"ip_addr.h"��#include"ip_addr.h"����"espconn.h"֮ǰ
#include "espconn.h"			// TCP/UDP�ӿ�
//#include "espnow.h"
#include "ets_sys.h"			// �ص�����
//#include "gpio.h"
#include "mem.h"				// �ڴ�����Ⱥ���
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX��������ʱ��
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro
//==================================================================================


// �궨��
//==================================================================================
#define		ProjectName			"STA_UDP_Client"		// �������궨��

#define		ESP8266_STA_SSID	"GXDS2019"			// �����WIFI��
#define		ESP8266_STA_PASS	"xnsq2019"		// �����WIFI����

#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED��
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED��
//==================================================================================


// ȫ�ֱ���
//==================================================================================
os_timer_t OS_Timer_1;			// ����������ʱ��

struct espconn ST_NetCon;		// �������ӽṹ��
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ESP8266_STA��ʼ��
//==============================================================================
void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX()
{
	struct station_config STA_Config;	// STA�����ṹ��

	struct ip_info ST_ESP8266_IP;		// STA��Ϣ�ṹ��

	// ����ESP8266�Ĺ���ģʽ
	//------------------------------------------------------------------------
	wifi_set_opmode(0x01);				// ����ΪSTAģʽ�������浽Flash

	/*
	// ����STAģʽ�µ�IP��ַ��ESP8266Ĭ�Ͽ���DHCP Client������WIFIʱ���Զ�����IP��ַ��
	//--------------------------------------------------------------------------------
	wifi_station_dhcpc_stop();						// �ر� DHCP Client
	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,8,88);		// ����IP��ַ
	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,8,1);		// �������ص�ַ
	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// ������������
	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����STAģʽ�µ�IP��ַ
	*/

	// �ṹ�帳ֵ������STAģʽ����
	//-------------------------------------------------------------------------------
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA�����ṹ�� = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// ����WIFI��
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// ����WIFI����

	wifi_station_set_config(&STA_Config);	// ����STA�����������浽Flash

	// wifi_station_connect();		// ESP8266����WIFI
}
//=========================================================================================


// �ɹ������������ݵĻص�����
//==========================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb_JX(void *arg)
{
	//os_printf("\nESP8266_WIFI_Send_OK\n");
}
//==========================================================


// �ɹ������������ݵĻص�����������1�����紫��ṹ��espconnָ�롢����2�����紫������ָ�롢����3�����ݳ��ȡ�
//=========================================================================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Recv_Cb_JX(void * arg, char * pdata, unsigned short len)
{
	struct espconn * T_arg = arg;		// �����������ӽṹ��ָ��

	remot_info * P_port_info = NULL;	// ����Զ��������Ϣָ��


	// ������������LED����/��
	//-------------------------------------------------------------------------------
	os_printf("ESP8266_Receive_Data = %s\r\n",pdata);		// ���ڴ�ӡ���յ�������
	OLED_ShowString(32,6,pdata);
	// ��ȡԶ����Ϣ��UDPͨ���������ӵģ���Զ��������Ӧʱ���ȡ�Է���IP/�˿���Ϣ��
	//------------------------------------------------------------------------------------
	if(espconn_get_connection_info(T_arg, &P_port_info, 0)==ESPCONN_OK)	// ��ȡԶ����Ϣ
	{
		T_arg->proto.udp->remote_port  = P_port_info->remote_port;		// ��ȡ�Է��˿ں�
		T_arg->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// ��ȡ�Է�IP��ַ
		T_arg->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		T_arg->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		T_arg->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];
		//os_memcpy(T_arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// �ڴ濽��
	}

	//--------------------------------------------------------------------
	OLED_ShowIP(16,4,T_arg->proto.udp->remote_ip);	// ��ʾԶ������IP��ַ
	//--------------------------------------------------------------------

	//espconn_send(T_arg,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// ��Է�����Ӧ��
}
//=========================================================================================================


// ����espconn�ͽṹ��
//-----------------------------------------------
//struct espconn ST_NetCon;	// �������ӽṹ��

// ��ʼ����������(UDPͨ��)
//==================================================================================================
void ICACHE_FLASH_ATTR ESP8266_NetCon_Init_JX()
{
	// �ṹ�帳ֵ
	//--------------------------------------------------------------------------
	ST_NetCon.type = ESPCONN_UDP;		// ����ͨ��Э��ΪUDP

	ST_NetCon.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));	// �����ڴ�

	// �˴���Ҫ����Ŀ��IP/�˿�(ESP8266��ΪClient����ҪԤ��֪��Server��IP/�˿�)
	//--------------------------------------------------------------------------
	ST_NetCon.proto.udp->local_port  = 9050 ;	// ���ñ��ض˿�
	ST_NetCon.proto.udp->remote_port = 9050;	// ����Ŀ��˿�
	ST_NetCon.proto.udp->remote_ip[0] = 192;	// ����Ŀ��IP��ַ
	ST_NetCon.proto.udp->remote_ip[1] = 168;
	ST_NetCon.proto.udp->remote_ip[2] = 31;
	ST_NetCon.proto.udp->remote_ip[3] = 1;
	//u8 remote_ip[4] = {192,168,8,47};		// Ŀ��ip��ַ
	//os_memcpy(ST_NetCon.proto.udp->remote_ip,remote_ip,4);	// �����ڴ�

	// ע��ص�����
	//-------------------------------------------------------------------------------------------
	espconn_regist_sentcb(&ST_NetCon,ESP8266_WIFI_Send_Cb_JX);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(&ST_NetCon,ESP8266_WIFI_Recv_Cb_JX);	// ע���������ݽ��ճɹ��Ļص�����

	// ����UDP��ʼ��API
	//-----------------------------------------------
	espconn_create(&ST_NetCon);		// ��ʼ��DUPͨ��


	// ������Server����ͨ��
	//----------------------------------------------------------------------------
	//espconn_send(&ST_NetCon,"Hello,I am ESP8266",os_strlen("Hello,I am ESP8266"));
}
//==================================================================================================


// ������ʱ�Ļص�����
//=========================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	u8 C_LED_Flash = 0;				// LED��˸�ƴ�

	struct ip_info ST_ESP8266_IP;	// ESP8266��IP��Ϣ
	u8 ESP8266_IP[4];				// ESP8266��IP��ַ


	// �ɹ�����WIFI��STAģʽ�£��������DHCP(Ĭ��)����ESO8266��IP��ַ��WIFI·�����Զ����䡿
	//-------------------------------------------------------------------------------------
	if( wifi_station_get_connect_status() == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
	{
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ��ȡSTA��IP��Ϣ
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// IP��ַ�߰�λ == addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// IP��ַ�θ߰�λ == addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// IP��ַ�εͰ�λ == addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// IP��ַ�Ͱ�λ == addr�߰�λ

		// ��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------
		//os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(16,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------

		// ����WIFI�ɹ���LED����3��
		//----------------------------------------------------
		for(; C_LED_Flash<=5; C_LED_Flash++)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
			delay_ms(100);
		}


		os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��

		ESP8266_NetCon_Init_JX();		// ��ʼ����������(UDPͨ��)
	}
}
//=========================================================================================================



// ������ʱ����ʼ��(ms����)
//=====================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//=====================================================================================

// LED��ʼ��
//=============================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}
//=============================================================================


// user_init��entry of user application, init user function here
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	//os_printf("\r\n=================================================\r\n");
	//os_printf("\t Project:\t%s\r\n", ProjectName);
	//os_printf("\t SDK version:\t%s", system_get_sdk_version());
	//os_printf("\r\n=================================================\r\n");

	// OLED��ʾ��ʼ��
	//--------------------------------------------------------
	OLED_Init();							// OLED��ʼ��
	//OLED_ShowString(0,0,"ESP8266");	// ESP8266ģʽ
	OLED_ShowString(0,2,"I:");				// ESP8266_IP��ַ
	OLED_ShowString(0,4,"P:");	// Զ������ģʽ
	OLED_ShowString(0,6,"Rec:");				// Զ������IP��ַ
	//--------------------------------------------------------

	LED_Init_JX();		// LED��ʼ��


	ESP8266_STA_Init_JX();			// ESP8266_STA��ʼ��

	OS_Timer_1_Init_JX(1000,1);		// 1�붨ʱ
}
//==============================================================================



/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void){}