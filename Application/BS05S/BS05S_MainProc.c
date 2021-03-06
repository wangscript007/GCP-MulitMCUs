/**
  ******************************************************************************
  * @file    GCMachine.c
  * @author  Duhanfneg
  * @version V1.0
  * @date    2017.10.23
  * @brief   
  ******************************************************************************
  * @attention
  * 
  * 
  * 
  * 
  ******************************************************************************
  */
   
   
/***********************************<INCLUDES>**********************************/
#include "BS05S_MainProc.h"
#include "BS05S_HwCtrl.h"
#include "BS05S_HardwareDef.h"
#include "BS05S_ComMan.h"
#include "DataType/DataType.h"
#include "SysPeripheral/SysTimer/SysTimer.h"
#include "SysPeripheral/CoreCtrl/CoreCtrl.h"



/**
  * @brief  相关资源初始化
  * @param  None
  * @retval None
  */
void BS05S_Init(void)
{
    //上电延时
    SysTime_DelayMs(200);
    
    //配置JTAG引脚
    CoreCtrl_JTAGConfig(1);
    
    //硬件初始化
    BS05S_HwInit();
    
}


/**
  * @brief  主任务处理
  * @param  None
  * @retval None
  */
void BS05S_MainProc(void)
{
    //LED显示
    BS05S_MainWorkLedShow();
    
    //按键检测
    BS05S_KeyProc();
    
    
}
