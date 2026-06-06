
#ifndef _GIZWITS_PROTOCOL_H
#define _GIZWITS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
/*

​​SEND_MAX_TIME​：发送最大时间间隔为200毫秒。

​​SEND_MAX_NUM​：重发次数最多为2次。

​​protocol_VERSION​：协议版本号为"00000004"。

​​P0_VERSION​：P0协议版本号为"00000002"。

​​PRODUCT_KEY​：产品密钥为"adc690275a704f109d2f57ac1bf7404e"。

​​PRODUCT_SECRET​：产品秘钥为"36c05a478af54ac2a77f1972ca1f9712"。

​​REPORT_TIME_MAX​：设备状态数据上报间隔最大为6000（即6秒）。

​​CELLNUMMAX​：最大的CELL号为7。

这些宏定义提供了一些常量值，可以在代码中使用。例如，可以用 ​SEND_MAX_TIME​来设置发送数据的最大时间间隔，或者使用 ​PRODUCT_KEY​来设置产品的密钥。
	
	*/
                                                                                                                  
#define SEND_MAX_TIME       200                     ///< 200ms resend
#define SEND_MAX_NUM        2                       ///< resend times
                                                    
#define protocol_VERSION    "00000004"              ///< protocol version
#define P0_VERSION          "00000002"              ///< P0 protocol version

/**@name Product Key  
* @{
*/
#define PRODUCT_KEY "83b64daa06484b0eb5e7c0976e979559"
/**@} */
/**@name Product Secret  
* @{
*/
#define PRODUCT_SECRET "7074edfd1aa343638ad53499a7342eb5"
               
/**@name Device status data reporting interval
* @{
*/
#define REPORT_TIME_MAX 1000 //修改上传时间间隔 ms
/**@} */    

#define CELLNUMMAX 7    


/**@name Whether the device is in the control class, 0 means no, 1 means yes
* @{
*/
#define DEV_IS_GATEWAY   0//中控设备   
#define DEV_IS_GATEWAY2  0//网关设备      
/**@} */

/**@name Binding time
* @{
*/
#define NINABLETIME  0
/**@} */



#define MAX_PACKAGE_LEN    (sizeof(devStatus_t)+sizeof(dataPointFlags_t)+20)///< Data buffer maximum length
#define RB_MAX_LEN          (MAX_PACKAGE_LEN*2)     ///< Maximum length of ring buffer

/**@name Data point related definition
* @{
*/
/*
​​DEV_IS_GATEWAY​：设备是否为控制类设备，0表示不是，1表示是。

​​DEV_IS_GATEWAY2​：设备是否为网关设备，0表示不是，1表示是。

​​NINABLETIME​：绑定时间。

​​MAX_PACKAGE_LEN​：数据缓冲区的最大长度，由设备状态的大小加上数据点标志位的大小再加上20。

​​RB_MAX_LEN​：环形缓冲区的最大长度，等于数据缓冲区最大长度的两倍。

这些宏定义提供了一些开关和常量值，可以根据实际需求进行配置。例如，通过修改 ​DEV_IS_GATEWAY​和 ​DEV_IS_GATEWAY2​的值，
可以设置设备是否为控制类设备或网关设备；通过调整 ​MAX_PACKAGE_LEN​和 ​RB_MAX_LEN​的值，可以配置数据缓冲区和环形缓冲区的最大长度。


*/ //修要修改对应的

#define PH_RATIO                         0.1
#define PH_ADDITION                      0
#define PH_MIN                           0
#define PH_MAX                           140
#define PH_LEN                           1
#define zhuodu_RATIO                         1
#define zhuodu_ADDITION                      0
#define zhuodu_MIN                           0
#define zhuodu_MAX                           10000
#define zhuodu_LEN                           2
#define shuiwen_RATIO                         0.1
#define shuiwen_ADDITION                      0
#define shuiwen_MIN                           0
#define shuiwen_MAX                           1000
#define shuiwen_LEN                           2
#define TDS_RATIO                         1
#define TDS_ADDITION                      0
#define TDS_MIN                           0
#define TDS_MAX                           10000
#define TDS_LEN                           2


/**@} */


/** Writable data points Boolean and enumerated variables occupy byte size */
#define COUNT_BIT 1



/** Read-only data points Boolean and enumerated variables occupy byte size */
#define COUNT_R_BIT 1

#define DATAPOINT_FLAG_LEN sizeof(dataPointFlags_t)		 ///< All data points FLAG occupies the maximum number of bytes


/** Event enumeration */
/*
​​WIFI_SOFTAP​：WiFi SOFTAP配置事件。
​​WIFI_AIRLINK​：WiFi模块AIRLINK配置事件。
​​WIFI_STATION​：WiFi模块STATION配置事件。
​​WIFI_OPEN_BINDING​：WiFi模块打开绑定事件。
​​WIFI_CLOSE_BINDING​：WiFi模块关闭绑定事件。
​​WIFI_CON_ROUTER​：WiFi模块连接到路由器事件。
​​WIFI_DISCON_ROUTER​：WiFi模块从路由器断开连接事件。
​​WIFI_CON_M2M​：WiFi模块具有服务器M2M事件。
​​WIFI_DISCON_M2M​：WiFi模块与服务器M2M断开连接事件。
​​WIFI_OPEN_TESTMODE​：WiFi模块打开测试模式事件。
​​WIFI_CLOSE_TESTMODE​：WiFi模块关闭测试模式事件。
​​WIFI_CON_APP​：WiFi模块连接到APP事件。
​​WIFI_DISCON_APP​：WiFi模块断开与APP的连接事件。
​​WIFI_RSSI​：WiFi模块RSSI事件。
​​WIFI_NTP​：网络时间事件。
​​MODULE_INFO​：模块信息事件。
​​TRANSPARENT_DATA​：透传数据事件。
​​EVENT_setyuliang​：用户定义的事件，表示设置预量。
​​EVENT_TYPE_MAX​：事件类型最大值，用于计算成员数量（如果用户意外删除了一些事件类型）。

*/
typedef enum
{
    WIFI_SOFTAP = 0x00,                               ///< WiFi SOFTAP configuration event
    WIFI_AIRLINK,                                     ///< WiFi module AIRLINK configuration event
    WIFI_STATION,                                     ///< WiFi module STATION configuration event
    WIFI_OPEN_BINDING,                                ///< The WiFi module opens the binding event
    WIFI_CLOSE_BINDING,                               ///< The WiFi module closes the binding event
    WIFI_CON_ROUTER,                                  ///< The WiFi module is connected to a routing event
    WIFI_DISCON_ROUTER,                               ///< The WiFi module has been disconnected from the routing event
    WIFI_CON_M2M,                                     ///< The WiFi module has a server M2M event
    WIFI_DISCON_M2M,                                  ///< The WiFi module has been disconnected from the server M2M event
    WIFI_OPEN_TESTMODE,                               ///< The WiFi module turns on the test mode event
    WIFI_CLOSE_TESTMODE,                              ///< The WiFi module turns off the test mode event
    WIFI_CON_APP,                                     ///< The WiFi module connects to the APP event
    WIFI_DISCON_APP,                                  ///< The WiFi module disconnects the APP event
    WIFI_RSSI,                                        ///< WiFi module RSSI event
    WIFI_NTP,                                         ///< Network time event
    MODULE_INFO,                                      ///< Module information event
    TRANSPARENT_DATA,                                 ///< Transparency events
    EVENT_mensuo,
    EVENT_kaideng,
    EVENT_dianqi,
    EVENT_xiaodu,
    EVENT_TYPE_MAX                                   ///< Enumerate the number of members to calculate (user accidentally deleted)
} EVENT_TYPE_T;


/** P0 command command code*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x11,             ///< Protocol 4.10 WiFi Module Control Device WiFi Module Send
    ACTION_READ_DEV_STATUS      = 0x12,             ///< Protocol 4.8 WiFi Module Reads the current status of the device WiFi module sent
    ACTION_READ_DEV_STATUS_ACK  = 0x13,             ///< Protocol 4.8 WiFi Module Read Device Current Status Device MCU Reply
    ACTION_REPORT_DEV_STATUS    = 0X14,             ///< Protocol 4.9 device MCU to the WiFi module to actively report the current status of the device to send the MCU
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi to device MCU transparent
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< Device MCU to WiFi
} actionType_t;

/** Protocol network time structure */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint32_t ntp;
}protocolTime_t;     

/** WiFi Module configuration parameters*/
/*
​WIFI_RESET_MODE​：WiFi模块复位模式。表示WiFi模块被重置。

​​WIFI_SOFTAP_MODE​：WiFi模块的软AP模式。表示WiFi模块以软AP模式工作。

​​WIFI_AIRLINK_MODE​：WiFi模块的AirLink模式。表示WiFi模块以AirLink模式工作。

​​WIFI_PRODUCTION_TEST​：MCU请求将WiFi模块进入生产测试模式。表示MCU请求将WiFi模块切换到生产测试模式。

​​WIFI_NINABLE_MODE​：MCU请求将WiFi模块进入绑定模式。表示MCU请求将WiFi模块切换到可绑定模式。

​​WIFI_REBOOT_MODE​：MCU请求WiFi模块重新启动。表示MCU发出请求，要求WiFi模块重新启动。
*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                           ///< WIFI module reset
  WIFI_SOFTAP_MODE,                                 ///< WIFI module softAP modeF
  WIFI_AIRLINK_MODE,                                ///< WIFI module AirLink mode
  WIFI_PRODUCTION_TEST,                             ///< MCU request WiFi module into production test mode
  WIFI_NINABLE_MODE,                                ///< MCU request module to enter binding mode
  WIFI_REBOOT_MODE,                                 ///< MCU request module reboot  
}WIFI_MODE_TYPE_T;                                

/** The protocol event type*/
/*
​​STATELESS_TYPE​：无状态类型。表示无需保存状态的事件。

​​ACTION_CONTROL_TYPE​：协议4.10：WiFi模块控制设备事件。表示WiFi模块向MCU发送控制设备的事件。

​​WIFI_STATUS_TYPE​：协议4.5：WiFi模块通知设备MCU WiFi模块状态变化的事件。表示WiFi模块向MCU发送WiFi模块状态变化的事件。

​​ACTION_W2D_TRANSPARENT_TYPE​：协议WiFi到设备MCU的透传事件。表示WiFi模块通过透传将数据发送给设备MCU的事件。

​​GET_NTP_TYPE​：协议4.13：MCU请求访问网络时间的事件。表示MCU发出请求，要求获取网络时间的事件。

​​GET_MODULEINFO_TYPE​：协议4.9：MCU获取模块信息的事件。表示MCU发出请求，要求获取与模块相关的信息的事件。

​​PROTOCOL_EVENT_TYPE_MAX​：枚举成员计数（用户不得删除）。用于计算枚举类型的成员数量。

*/

typedef enum
{
  STATELESS_TYPE = 0x00,                            ///< Stateless type
  ACTION_CONTROL_TYPE,                              ///< Protocol 4.10 :WiFi module control device event
  WIFI_STATUS_TYPE,                                 ///< Protocol 4.5 :WiFi module inform the device MCU of the change event of the WiFi module status
  ACTION_W2D_TRANSPARENT_TYPE,                      ///< Protocol WiFi to device MCU transparent event
  GET_NTP_TYPE,                                     ///< Protocol 4.13 :The MCU requests access to the network time event
  GET_MODULEINFO_TYPE,                              ///< Protocol 4.9 :The MCU get module information event
  PROTOCOL_EVENT_TYPE_MAX                           ///< Count enumerated member (User donot delete)
} PROTOCOL_EVENT_TYPE_T;
     
/** Protocol command code */                                   
typedef enum                               
{                                                   
    CMD_GET_DEVICE_INTO             = 0x01,         ///< Protocol：3.1 
    ACK_GET_DEVICE_INFO             = 0x02,         ///< Protocol：3.1 
            
    CMD_ISSUED_P0                   = 0x03,         ///< Protocol：3.2 3.3 
    ACK_ISSUED_P0                   = 0x04,         ///< Protocol：3.2 3.3 
            
    CMD_REPORT_P0                   = 0x05,         ///< Protocol：3.4 
    ACK_REPORT_P0                   = 0x06,         ///< Protocol：3.4 
            
    CMD_HEARTBEAT                   = 0x07,         ///< Protocol：3.5 
    ACK_HEARTBEAT                   = 0x08,         ///< Protocol：3.5 
            
    CMD_WIFI_CONFIG                 = 0x09,         ///< Protocol：3.6 
    ACK_WIFI_CONFIG                 = 0x0A,         ///< Protocol：3.6 
            
    CMD_SET_DEFAULT                 = 0x0B,         ///< Protocol：3.7 
    ACK_SET_DEFAULT                 = 0x0C,         ///< Protocol：3.7 
            
    CMD_WIFISTATUS                  = 0x0D,         ///< Protocol：3.8 
    ACK_WIFISTATUS                  = 0x0E,         ///< Protocol：3.8 
        
    CMD_MCU_REBOOT                  = 0x0F,         ///< Protocol：4.1 
    ACK_MCU_REBOOT                  = 0x10,         ///< Protocol：4.1 
            
    CMD_ERROR_PACKAGE               = 0x11,         ///< Protocol：3.9 
    ACK_ERROR_PACKAGE               = 0x12,         ///< Protocol：3.9 

    CMD_PRODUCTION_TEST             = 0x13,         ///< Protocol：
    ACK_PRODUCTION_TEST             = 0x14,         ///< Protocol：

    CMD_NINABLE_MODE                = 0x15,         ///< Protocol：3.10
    ACK_NINABLE_MODE                = 0x16,         ///< Protocol：3.10

    CMD_GET_NTP                     = 0x17,         ///< Protocol：4.3 
    ACK_GET_NTP                     = 0x18,         ///< Protocol：4.3 


    CMD_ASK_BIGDATA                 = 0x19,         ///< Protocol：4.4
    ACK_ASK_BIGDATA                 = 0x1A,         ///< Protocol：4.4

    CMD_BIGDATA_READY               = 0x1B,         ///< Protocol：4.5
    ACK_BIGDATA_READY               = 0x1C,         ///< Protocol：4.5

    CMD_BIGDATA_SEND                = 0x1D,         ///< Protocol：4.6
    ACK_BIGDATA_SEND                = 0x1E,         ///< Protocol：4.6

    CMD_S_STOP_BIGDATA_SEND         = 0x1F,         ///< Protocol：4.7
    ACK_S_STOP_BIGDATA_SEND         = 0x20,         ///< Protocol：4.7

    CMD_D_STOP_BIGDATA_SEND         = 0x27,         ///< Protocol：4.8
    ACK_D_STOP_BIGDATA_SEND         = 0x28,         ///< Protocol：4.8

    CMD_ASK_MODULE_INFO             = 0x21,         ///< Protocol：4.9
    ACK_ASK_MODULE_INFO             = 0x22,         ///< Protocol：4.9

    CMD_ASK_AFFAIR_HANDLE           = 0x23,         ///< Protocol：4.10
    ACK_ASK_AFFAIR_HANDLE           = 0x24,         ///< Protocol：4.10

    CMD_AFFAIR_RESULT               = 0x25,         ///< Protocol：4.10
    ACK_AFFAIR_RESULT               = 0x26,         ///< Protocol：4.10

    CMD_REBOOT_MODULE               = 0x29,         ///< Protocol：3.11
    ACK_REBOOT_MODULE               = 0x2A,         ///< Protocol：3.11

    CMD_CONNECT_M2M                 = 0x2D,         ///< Protocol：for Virtualization
    ACK_CONNECT_M2M                 = 0x2E,         ///< Protocol：for Virtualization

    CMD_CONNECT_M2M_BACK            = 0x2F,         ///< Protocol：for Virtualization
    ACK_CONNECT_M2M_BACK            = 0x30,         ///< Protocol：for Virtualization

    CMD_UPLOAD_DATA                 = 0x31,         ///< Protocol：for Virtualization
    ACK_UPLOAD_DATA                 = 0x32,         ///< Protocol：for Virtualization

    CMD_UPLOAD_DATA_BACK            = 0x33,         ///< Protocol：for Virtualization
    ACK_UPLOAD_DATA_BACK            = 0x34,         ///< Protocol：for Virtualization

    CMD_DISCONNECT_M2M              = 0x35,         ///< Protocol：for Virtualization
    ACK_DISCONNECT_M2M              = 0x36,         ///< Protocol：for Virtualization

    CMD_DISCONNECT_M2M_BACK         = 0x37,         ///< Protocol：for Virtualization
    ACK_DISCONNECT_M2M_BACK         = 0x38,         ///< Protocol：for Virtualization

    CMD_RESET_SIMULATOR             = 0x39,         ///< Protocol：for Virtualization
    ACK_RESET_SIMULATOR             = 0x3A,         ///< Protocol：for Virtualization

    CMD_RESET_SIMULATOR_BACK        = 0x3B,         ///< Protocol：for Virtualization
    ACK_RESET_SIMULATOR_BACK        = 0x3C,         ///< Protocol：for Virtualization
} PROTOCOL_CMDTYPE;                                                                                  
                        
/** Illegal message type*/  
/*
​​errorPacketsType_t​：非法消息类型枚举。用于表示错误的消息类型。

​​ERROR_ACK_SUM​：校验错误类型。表示接收到的消息校验和有误。
​​ERROR_CMD​：命令代码错误类型。表示接收到的消息命令代码有误。
​​ERROR_OTHER​：其他错误类型。
​​execute_result​：执行结果枚举。用于表示执行操作的结果。

​​EXE_SUCESS​：执行成功。表示执行操作成功。
​​EXE_FAILE​：执行失败。表示执行操作失败。
​​dataPoint_t​：用户区设备状态结构。包含湿度和温度的数值。

​​valueshidu​：湿度数值。
​​valuewendu​：温度数值。
​​dataPointFlags_t​：用户区设备状态标志结构。包含湿度和温度的标志位。

​​flagshidu​：湿度标志位。表示湿度是否有效。
​​flagwendu​：温度标志位。表示温度是否有效。
​​devStatus_t​：对应协议中"4.9 设备MCU主动上报当前状态"中设备状态"dev_status（11B）"的结构。

​​valueshidu​：湿度值。
​​valuewendu​：温度值。
*/
typedef enum
{
    ERROR_ACK_SUM = 0x01,                           ///< check error
    ERROR_CMD     = 0x02,                           ///< Command code error
    ERROR_OTHER   = 0x03,                           ///< other
} errorPacketsType_t;

typedef enum
{
    EXE_SUCESS                      = 0x00,
    EXE_FAILE                       = 0x01,
} execute_result;  

#pragma pack(1)
/** User Area Device State Structure */
typedef struct {
    float valuePH;
    uint32_t valuezhuodu;
    float valueshuiwen;
    uint32_t valueTDS;
} dataPoint_t;

/** User Area Device State Structure */
typedef struct {
    uint8_t flagPH:1;
    uint8_t flagzhuodu:1;
    uint8_t flagshuiwen:1;
    uint8_t flagTDS:1;
} dataPointFlags_t;

/** Corresponding to the protocol "4.9 device MCU to WiFi module active reporting of the current state" in the device state "dev_status (11B)" */ 
typedef struct {
    uint8_t valuePH;
    uint16_t valuezhuodu;
    uint16_t valueshuiwen;
    uint16_t valueTDS;
} devStatus_t;



 /*
​​eventInfo_t​ 结构体：

​​num​：队列成员数量。用于表示队列中成员的数量。

​​event​：事件内容。用于存储队列成员的事件内容。

​​moduleStatusInfo_t​ 结构体：

​​rssi​：WIFI信号强度。用于表示WIFI信号的强度。
​​protocolHead_t​ 结构体：

​​head​：协议头部。用于表示协议头部的标识符，固定为0xFFFF。

​​len​：从命令开始到整个数据包结尾所占用的字节数。

​​cmd​：命令类型。用于表示协议中的命令类型。

​​sn​：序列号。用于标识协议中的序列号。

​​flags​：标志位，默认为0。用于根据具体需求在协议中添加标志位信息。

这些结构体在协议处理过程中起到了不同的作用。​eventInfo_t​结构体用于存储事件队列中的事件信息，
包括事件数量和事件内容。​moduleStatusInfo_t​结构体用于表示模块的状态信息，主要包括WIFI信号强度。
​protocolHead_t​结构体用于定义协议的头部信息，包括头部标识符、数据包长度、命令类型、序列号和标志位等。

*/               
/** Event queue structure */                               
typedef struct {                           
    uint8_t num;                                    ///< Number of queue member
    uint8_t event[EVENT_TYPE_MAX];                  ///< Queue member event content
}eventInfo_t;


                            
/** wifiSignal strength structure */                       
typedef struct {                           
    uint8_t rssi;                                   ///< WIFI signal strength
}moduleStatusInfo_t;                                

/** Protocol standard header structure */
typedef struct
{
    uint8_t                 head[2];                ///< The head is 0xFFFF
    uint16_t                len;                    ///< From cmd to the end of the entire packet occupied by the number of bytes
    uint8_t                 cmd;                    ///< command
    uint8_t                 sn;                     ///< 
    uint8_t                 flags[2];               ///< flag,default is 0
} protocolHead_t;

/*
​​protocolDeviceInfo_t​ 结构体：

​​head​：协议头部。用于表示协议头部的标识符等信息。

​​protocolVer​：协议版本号。用于表示协议的版本信息。

​​p0Ver​：P0协议版本号。用于表示P0协议的版本信息。

​​hardVer​：硬件版本号。用于表示设备的硬件版本信息。

​​softVer​：软件版本号。用于表示设备的软件版本信息。

​​productKey​：产品密钥。用于表示设备所属产品的密钥信息。

​​ninableTime​：绑定时间（秒）。用于表示设备的绑定时间。

​​devAttr​：设备属性。用于表示设备的属性信息。

​​productSecret​：产品密钥。用于表示设备所属产品的密钥信息。

​​sum​：校验和。用于校验数据包的完整性。

​​protocolCommon_t​ 结构体：

​​head​：协议头部。用于表示协议头部的标识符等信息。

​​sum​：校验和。用于校验数据包的完整性。

​​protocolCfgMode_t​ 结构体：

​​head​：协议头部。用于表示协议头部的标识符等信息。

​​cfgMode​：配置模式。用于指示设备所处的配置模式。

​​sum​：校验和。用于校验数据包的完整性。

​​protocolUTT_t​ 结构体：

​​head​：协议头部。用于表示协议头部的标识符等信息。

​​time​：硬件时间。用于表示设备的硬件时间。

​​ntp_time​：网络时间。用于表示设备的网络时间。

​​sum​：校验和。用于校验数据包的完整性。

​​wifiStatus_t​ 联合体：

​​value​：16位无符号整数，用于存储WiFi模块的工作状态。

​​types​：结构体类型，用于将 ​value​进行分组表示不同状态的标志位。

​​softap​：1位，表示是否处于SoftAP模式。

​​station​：1位，表示是否处于Station（STA）模式。

​​onboarding​：1位，表示是否处于配网模式。

​​binding​：1位，表示是否处于绑定模式。

​​con_route​：1位，表示是否连接到路由器。

​​con_m2m​：1位，表示是否连接到M2M服务器。

​​reserve1​：2位保留字段。

​​rssi​：3位，表示WIFI信号强度。

​​app​：1位，表示是否通过APP控制。

​​test​：1位，表示是否处于测试模式。

​​reserve2​：3位保留字段。

这些结构体和联合体用于在WiFi模块和设备之间进行数据传输和状态变更的通信。不同的结构体表示不同的协议格式，
包括设备信息、配置模式、网络时间等。联合体用于存储WiFi模块的工作状态，并将其分组表示为不同的标志位。

*/


/** 4.1 WiFi module requests the device information protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 protocolVer[8];         ///< Protocol version
    uint8_t                 p0Ver[8];               ///< p0 Protocol version
    uint8_t                 hardVer[8];             ///< Hardware version
    uint8_t                 softVer[8];             ///< Software version
    uint8_t                 productKey[32];         ///< Product key
    uint16_t                ninableTime;            ///< Binding time(second)
    uint8_t                 devAttr[8];             ///< Device attribute
    uint8_t                 productSecret[32];      ///< Product secret
    uint8_t                 sum;                    ///< checksum
} protocolDeviceInfo_t;

/** Protocol common data frame(4.2、4.4、4.6、4.9、4.10) protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 sum;                    ///< checksum
} protocolCommon_t;

/** 4.3 The device MCU informs the WiFi module of the configuration mode  protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 cfgMode;                ///< Configuration parameters
    uint8_t                 sum;                    ///< checksum
} protocolCfgMode_t;

/** 4.13 The MCU requests the network time  protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 time[7];                ///< Hardware version
    uint8_t                 ntp_time[4];            ///< Software version
    uint8_t                 sum;                    ///< checksum
} protocolUTT_t;

/** WiFi module working status*/
typedef union
{
    uint16_t                value;
    struct
    {
        uint16_t            softap:1;     
        uint16_t            station:1;    
        uint16_t            onboarding:1; 
        uint16_t            binding:1;    
        uint16_t            con_route:1;  
        uint16_t            con_m2m:1;    
        uint16_t            reserve1:2;   
        uint16_t            rssi:3;       
        uint16_t            app:1;        
        uint16_t            test:1;       
        uint16_t            reserve2:3;   
    }types; 
    
} wifiStatus_t;

/** WiFi status type :protocol structure */
/*
​​protocolWifiStatus_t​ 结构体：

​​head​：协议标准头部结构。用于存储协议的标准头部信息。

​​status​：WIFI状态。用于表示设备的WIFI状态。

​​sum​：校验和。用于校验协议数据的完整性。

​​protocolGetModuleInfo_t​ 结构体：

​​head​：协议标准头部结构。用于存储协议的标准头部信息。

​​type​：信息类型。用于标识传输的信息类型。

​​sum​：校验和。用于校验协议数据的完整性。

​​moduleInfo_t​ 结构体：

​​moduleType​：模块类型。用于表示模块的类型信息。

​​serialVer​：串口协议版本。用于存储串口协议的版本号。

​​hardVer​：硬件版本。用于存储硬件版本号。

​​softVer​：软件版本。用于存储软件版本号。

​​mac​：MAC地址。用于存储设备的MAC地址。

​​ip​：IP地址。用于存储设备的IP地址。

​​devAttr​：设备属性。用于存储设备的属性信息。
*/
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    wifiStatus_t            ststus;                 ///< WIFI status
    uint8_t                 sum;                    ///< checksum
} protocolWifiStatus_t;

/** Protocol common data frame(4.9) :protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 type;                   ///< Information Type
    uint8_t                 sum;                    ///< checksum
} protocolGetModuleInfo_t;

typedef struct
{
    uint8_t                 moduleType;             ///< Information Type
    uint8_t                 serialVer[8];           ///< Serial port protocol version
    uint8_t                 hardVer[8];             ///< Hardware version
    uint8_t                 softVer[8];             ///< Software version
    uint8_t                 mac[16];                ///< mac 
    uint8_t                 ip[16];                 ///< ip
    uint8_t                 devAttr[8];             ///< Device attribute
} moduleInfo_t;

/** Protocol common data frame(4.9) :protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    moduleInfo_t            wifiModuleInfo;         ///< WIFI module information
    uint8_t                 sum;                    ///< checksum
} protocolModuleInfo_t;


/** GPRS information of base station */
typedef struct
{
    uint16_t                    LAC_ID;             ///<LAC area ID
    uint16_t                    CellID;             ///<Base station ID
    uint8_t                     RSSI;               ///<Signal strength of base station
} gprsCellInfo_t;


/** 3.19 The basic information of the GPRS communication module  */
typedef struct
{
    uint8_t                 Type;//2G/3g/4g
    uint8_t                 Pro_ver[8];//Universal serial port protocol version
    uint8_t                 Hard_ver[8];//Hardware version
    uint8_t                 Soft_ver[8];//Software version
    uint8_t                 Device_attribute[8];//Device attribute
    uint8_t                 IMEI[16];//string
    uint8_t                 IMSI[16];//string
    uint8_t                 MCC[8];//Mobile country code
    uint8_t                 MNC[8];//Mobile network code
    uint8_t                 CellNum;//Number of base station
    uint8_t                 CellInfoLen;//Information length of base station 
    gprsCellInfo_t          GPRS_CellINFO[CELLNUMMAX];
}gprsInfo_t;

/** 4.7 Illegal message notification :protocol structure*/
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 error;                  ///< error value
    uint8_t                 sum;                    ///< checksum
} protocolErrorType_t;


/** P0 message header */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 action;                 ///< p0 command 
} protocolP0Head_t;


/** protocol “4.9 The device MCU reports the current status to the WiFi module” device status "dev_status(11B)"  */ 
typedef struct {
                     
    devStatus_t devStatus;                          ///< Stores the device status data
}gizwitsReport_t;

/** resend strategy structure */
/*
​​protocolWaitAck_t​ 结构体：

​​num​：重发次数。用于表示需要重发的次数。

​​flag​：等待ACK标志位。当为1时表示需要等待ACK，为0时表示不需要等待ACK。

​​buf​：重发数据缓冲区。用于存储需要重发的数据。

​​dataLen​：重发数据长度。重发数据的长度。

​​sendTime​：重发时间。用于记录重发的时间。

​​gizwitsElongateP0Form_t​ 结构体：

​​devDatapointFlag​：数据点标志位。用于存储所有数据点的标志位。

​​devStatus​：所有数据点数据。用于存储所有数据点的数据。

​​gizwitsP0Max_t​ 结构体：

​​action​：P0动作。用于表示P0动作的类型。

​​gizdata​：最大缓冲区，可以容纳所有数据点的值。用于存储所有数据点的值。

这些结构体在协议处理过程中起到了不同的作用。​protocolWaitAck_t​结构体用于处理等待ACK的相关信息，​gizwitsElongateP0Form_t​结构体用于存储设备的所有数据点信息，而 ​gizwitsP0Max_t​结构体用于定义P0动作和保存数据点值的缓冲区。
*/
typedef struct {
    uint8_t                 num;                    ///< resend times
    uint8_t                 flag;                   ///< 1,Indicates that there is a need to wait for the ACK;0,Indicates that there is no need to wait for the ACK
    uint8_t                 buf[MAX_PACKAGE_LEN];   ///< resend data buffer
    uint16_t                dataLen;                ///< resend data length
    uint32_t                sendTime;               ///< resend time
} protocolWaitAck_t;
                                                                                


typedef struct {                           
    dataPointFlags_t  devDatapointFlag;             ///< Datapoint Flag , All datapoints 
    devStatus_t devStatus;                          ///< All datapoints data
}gizwitsElongateP0Form_t;  

typedef struct {                           
    uint8_t action;                                 ///< P0 action 
    uint8_t gizdata[sizeof(gizwitsElongateP0Form_t)];  ///< Max buffer ,  Can hold all datapoints value 
}gizwitsP0Max_t;        

/** Protocol main and very important struct */
/*
​​issuedFlag​：P0动作类型。用于标识P0动作的类型。

​protocolBuf​：协议数据处理缓冲区。用于处理协议数据的缓冲区。

​​transparentBuff​：透传数据存储区域。用于存储透传数据的区域。

​​transparentLen​：传输数据长度。透传数据的长度。

​​sn​：消息SN（序列号）。用于标识消息的序列号。

​​timerMsCount​：定时器计数。用于计算定时器的时间。

​​waitAck​：等待ACK的协议数据结构。用于存储等待ACK的协议数据信息。

​​issuedProcessEvent​：控制事件。用于存储控制事件的信息。

​​wifiStatusEvent​：WIFI状态事件。用于存储WIFI状态变化的事件信息。

​​NTPEvent​：网络时间协议（NTP）事件。用于存储NTP事件的信息。

​​moduleInfoEvent​：模块信息事件。用于存储模块信息变化的事件信息。

​​waitReportDatapointFlag​：待上报数据点标志位。用于存储待上报数据点的标志位。

​​reportData​：上报实际数据。用于存储上报的实际数据，可以容纳所有数据点的数值。

​​reportDataLen​：上报实际数据长度。上报实际数据的长度。

​​gizCurrentDataPoint​：当前设备数据点状态。当前设备数据点的状态信息。

​​gizLastDataPoint​：上一次设备数据点状态。上一次设备数据点的状态信息。

​​wifiStatusData​：WIFI信号强度。用于存储WIFI信号强度的信息。

​​TimeNTP​：网络时间信息。用于存储网络时间的信息。

​​gprsInfoNews​：GPRS模块信息。用于存储GPRS模块的信息。

​​wifiModuleNews​：WIFI模块信息。用于存储WIFI模块的信息。


*/
typedef struct
{
    uint8_t issuedFlag;                             ///< P0 action type
    uint8_t protocolBuf[MAX_PACKAGE_LEN];           ///< Protocol data handle buffer
    uint8_t transparentBuff[MAX_PACKAGE_LEN];       ///< Transparent data storage area
    uint32_t transparentLen;                        ///< Transmission data length
    
    uint32_t sn;                                    ///< Message SN
    uint32_t timerMsCount;                          ///< Timer Count 
    protocolWaitAck_t waitAck;                      ///< Protocol wait ACK data structure
    
    eventInfo_t issuedProcessEvent;                 ///< Control events
    eventInfo_t wifiStatusEvent;                    ///< WIFI Status events
    eventInfo_t NTPEvent;                           ///< NTP events
    eventInfo_t moduleInfoEvent;                    ///< Module Info events

    dataPointFlags_t  waitReportDatapointFlag;      ///< Store the data points to be reported flag
    uint8_t reportData[sizeof(gizwitsElongateP0Form_t)];    ///< Reporting actual data , Max , Can hold all datapoints value
    uint32_t reportDataLen;                         ///< Reporting actual data length

    dataPoint_t gizCurrentDataPoint;                ///< Current device datapoints status
    dataPoint_t gizLastDataPoint;                   ///< Last device datapoints status
    moduleStatusInfo_t wifiStatusData;              ///< WIFI signal intensity
    protocolTime_t TimeNTP;                         ///< Network time information
#if MODULE_TYPE
    gprsInfo_t   gprsInfoNews;
#else  
    moduleInfo_t  wifiModuleNews;                   ///< WIFI module Info
#endif
    
}gizwitsProtocol_t;

#pragma pack()

/**@name Gizwits user API interface
* @{
*/

extern uint32_t gizGetTimerCount(void);

void gizwitsInit(void);
int32_t gizwitsSetMode(uint8_t mode);
void gizwitsGetNTP(void);
int32_t gizwitsHandle(dataPoint_t *currentData);
int32_t gizwitsPassthroughData(uint8_t * gizdata, uint32_t len);
void gizwitsGetModuleInfo(void);
int32_t gizPutData(uint8_t *buf, uint32_t len);

/**@} */
#ifdef __cplusplus
}
#endif 

#endif
