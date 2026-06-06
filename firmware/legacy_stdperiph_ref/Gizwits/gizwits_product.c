
#include <stdio.h>
#include <string.h>
//#include "hal_key.h"
#include "gizwits_product.h"
#include "common.h"
#include "main.h"

static uint32_t timerMsCount;
uint8_t aRxBuffer;

/** User area the current device state structure*/
dataPoint_t currentDataPoint;
//extern keysTypedef_t keys;

//extern TIM_HandleTypeDef htim2;
//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;

/**@} */
/**@name Gizwits User Interface
* @{
根据注释和代码推测，该函数可能用于处理与机智云平台相关的事件。以下是对每个变量的解释：

​​info​：该参数是一个指向 ​eventInfo_t​类型结构体的指针。它可能包含了与事件相关的信息，如事件类型、设备标识等。

​​gizdata​：该参数是一个指向 ​uint8_t​类型数据的指针，它可能作为事件数据的缓冲区。

​​len​：该参数是 ​gizdata​ 缓冲区中数据的长度，用来确保不超出缓冲区大小。

在代码中，变量 ​i​被声明为 ​uint8_t​类型，并初始化为0。接下来，几个指针变量被声明，通过将 ​gizdata​强制转换为不同的结构体类型来解析数据。

随后，通过条件编译的方式，检查 ​MODULE_TYPE​的值，如果满足条件，则定义并声明一个指向 ​gprsInfo_t​类型的指针变量 ​gprsInfoData​；否则，定义并声明一个指向 ​moduleInfo_t​类型的指针变量 ​ptModuleInfo​。
*/


//int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
//{
//    uint8_t i = 0;
//    dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
//    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
//    protocolTime_t *ptime = (protocolTime_t *)gizdata;
//    
//#if MODULE_TYPE
//    gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
//#else
//    moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
//#endif

//    if((NULL == info) || (NULL == gizdata))
//    {
//        return -1;
//    }

//    for(i=0; i<info->num; i++)
//    {
//        switch(info->event[i])
//        {

//        case WIFI_SOFTAP://对于Wi-Fi处于软AP模式时的操作。
//            break;
//        case WIFI_AIRLINK://对于Wi-Fi处于Airlink配置模式时的操作。
//            break;
//        case WIFI_STATION://对于Wi-Fi处于Station模式（连接到路由器）时的操作。
//            break;
//        case WIFI_CON_ROUTER://连接上WiFi  自动进入页面

//						smart_config = 0;
// 
//            break;
//        case WIFI_DISCON_ROUTER://对于与路由器断开连接时的操作。
// 
//            break;
//        case WIFI_CON_M2M://对于成功连接到M2M服务器时的操作。
// 
//            break;
//        case WIFI_DISCON_M2M://对于与M2M服务器断开连接时的操作。
//            break;
//        case WIFI_RSSI:
//            GIZWITS_LOG("RSSI %d\n", wifiData->rssi);//获取当前Wi-Fi信号强度值（RSSI）并进行操作。
//            break;
//        case TRANSPARENT_DATA://处理透传数据。
//            GIZWITS_LOG("TRANSPARENT_DATA \n");
//            //user handle , Fetch data from [data] , size is [len]
//            break;
//        case WIFI_NTP://获取NTP时间并进行操作。
//            GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
//            break;
//        case MODULE_INFO://处理模块信息。
//            GIZWITS_LOG("MODULE INFO ...\n");
//#if MODULE_TYPE
//            GIZWITS_LOG("GPRS MODULE ...\n");
//            //Format By gprsInfo_t 如果 ​MODULE_TYPE​的值为真（非零），则表示设备使用的是GPRS模块。
//            GIZWITS_LOG("moduleType : [%d] \n",gprsInfoData->Type);
//#else
//            GIZWITS_LOG("WIF MODULE ...\n");
//            //Format By moduleInfo_t 如果 ​MODULE_TYPE​的值为假（0），则表示设备使用的是WiFi模块。
//            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
//#endif
//        break;
//        default:
//            break;
//        }
//    }

//    return 0;
//}
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
    protocolTime_t *ptime = (protocolTime_t *)gizdata;
    
#if MODULE_TYPE
    gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
    moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif
//修要修改对应的
    if((NULL == info) || (NULL == gizdata))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {

        case WIFI_SOFTAP://对于Wi-Fi处于软AP模式时的操作。
            break;
        case WIFI_AIRLINK://对于Wi-Fi处于Airlink配置模式时的操作。
            break;
        case WIFI_STATION://对于Wi-Fi处于Station模式（连接到路由器）时的操作。
            break;
        case WIFI_CON_ROUTER://连接上WiFi  自动进入页面
						smart_config = 0;
            break;
        case WIFI_DISCON_ROUTER://对于与路由器断开连接时的操作。
 
            break;
        case WIFI_CON_M2M://对于成功连接到M2M服务器时的操作。
 
            break;
        case WIFI_DISCON_M2M://对于与M2M服务器断开连接时的操作。
            break;
        case WIFI_RSSI://获取当前Wi-Fi信号强度值（RSSI）并进行操作。
            GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA://处理透传数据。
            GIZWITS_LOG("TRANSPARENT_DATA \n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case WIFI_NTP://获取NTP时间并进行操作。
            GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
            break;
        case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
#if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",gprsInfoData->Type);
#else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
#endif
        break;
        default:
            break;
        }
    }

    return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

1.数据收集除数据点之外的其他数据：这部分功能可能是为了采集一些设备状态信息、传感器数据或其他非标准数据。具体采集什么样的数据以及如何实现，将取决于应用需求和设备设计。

2.自定义采集频率：允许用户根据需要来定制数据采集的频率。这样可以进行灵活的控制，以满足不同应用场景的需求。用户可以设置数据采集的时间间隔、触发条件或其他参数，使采集频率适应特定的应用案例。

3.设计数据过滤算法：该部分涉及到对采集到的数据进行处理和筛选。用户可以根据自己的需求设计算法，进行数据过滤、去噪、平滑或其他操作。这有助于提高数据质量和准确性，并根据实际需求提取有用的信息。
* @param none
* @return none
*/
//修要修改对应的
void userHandle(void)//数据处理
{


    currentDataPoint.valuezhuodu = turbidity_temp;//Add Sensor Data Collection
    currentDataPoint.valuePH = PH_temp;//Add Sensor Data Collection
    currentDataPoint.valueshuiwen = temperature_temp;//Add Sensor Data Collection
		currentDataPoint.valueTDS = TDS_DAT;


}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
数据点的初始化：在该函数中，开发者可以完成对数据点的初始化。数据点通常是指用于表示设备状态、属性或功能的变量或结构体。
开发者可以为每个数据点设置初始值或状态，以确保设备在启动时具有正确的默认值。

用户相关数据的初始化：该函数可能还涉及其他与用户相关的数据的初始化，这些数据可能与应用需求或设备配置有关。
例如，用户界面的设置、用户偏好设置或其他用户相关的配置项。
*/
//修要修改对应的
void userInit(void)
{
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 

		currentDataPoint.valuezhuodu = turbidity_temp;
    currentDataPoint.valuePH = PH_temp;
    currentDataPoint.valueshuiwen = temperature_temp;
		currentDataPoint.valueTDS = TDS_DAT;
}




/**
* @brief Millisecond timing maintenance function, milliseconds increment, overflow to zero

* @param none
* @return none
毫秒计时的维护：该函数用于对毫秒计时器进行更新和维护。这可以通过递增一个全局变量或者一个静态变量来实现，以确保每次调用该函数时，毫秒计时器被增加。

毫秒计时器的递增：该函数会增加毫秒计时器的值。这通常是通过增加一个固定的值（例如1）来完成的，以确保每次调用函数时，毫秒计时器的值会增加相应的量。

溢出处理：当毫秒计时器累积到一定值后，可能会发生溢出。为了避免溢出导致的错误，该函数会在溢出时将毫秒计时器的值归零，重新开始计数。

需要注意的是，该代码只是提供了
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief Read millisecond count

* @param none
* @return millisecond count
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief MCU reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

/**@} */

/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
UART中断处理：该函数是一个UART的中断处理函数，用于处理串口接收数据的中断事件。当接收到数据时，该函数会被触发执行。

模块通信：该函数用于实现模块之间的通信，特别是WiFi模块之间的串口通信。它负责接收通过UART串口传输的数据，并进行相应的处理和解析。

串口协议数据接收：函数在中断触发时，会执行串口协议数据的接收操作。这可能包括读取串口接收缓冲区中的数据，并进行解码、处理、存储等操作，以便进一步处理。
*/
void UART_IRQ_FUN(void)
{
  uint8_t value = 0;
  value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}

/**
* @brief Serial port write operation, send data to WiFi module
*
* @param buf      : buf address
* @param len      : buf length
*
* @return : Return effective data length;-1，return failure
串口写操作：该函数用于向串口发送数据，将数据发送给WiFi模块。函数参数中的 ​buf​指向待发送数据的缓冲区地址，​len​表示待发送数据的长度。

WiFi模块通信：通过串口写操作，该函数实现了与WiFi模块之间的通信。它负责将数据发送给WiFi模块，以实现设备的控制、数据传输或其他功能。

返回值处理：该函数的返回值表示串口写操作的执行结果。如果串口写入成功，则返回有效数据的长度；如果写入失败，则返回-1。
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
	//		uint8_t crc[1] = {0x55};
    uint32_t i = 0;
	
    if(NULL == buf)
    {
        return -1;
    }

    for(i=0; i<len; i++)
    {
        USART_SendData(USART2,buf[i]);
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕


        if(i >=2 && buf[i] == 0xFF)
        {
						 USART_SendData(USART2,0x55);
          while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        }
    }

#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
        GIZWITS_LOG("%02x ", buf[i]);

        if(i >=2 && buf[i] == 0xFF)
        {
            GIZWITS_LOG("%02x ", 0x55);
        }
    }
    GIZWITS_LOG("\n");
#endif
		
		return len;
}  
 
