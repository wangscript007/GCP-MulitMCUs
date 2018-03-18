/**
  ******************************************************************************
  * @file    GCIOCtrl.c
  * @author  Duhanfneg
  * @version V1.0
  * @date    2017.10.20
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
#include "BS05M_IOTable.h"
#include "SysPeripheral/GPIO/GPIO_Man.h"



//输入IO表
static GPIO_DATA m_InputIOTable[] = 
{
    //按键
    {GPIO_PORT_C, 14},
};


//输出IO表
static GPIO_DATA m_OutputIOTable[] = 
{
    //LED灯
    {GPIO_PORT_C, 13},

    //工作指示灯
    {GPIO_PORT_B, 5},
    {GPIO_PORT_B, 7},
    {GPIO_PORT_B, 9},
    
};


//控制IO表
GPIO_CTRL_TABLE g_BS05M_IOTable = 
{
    m_OutputIOTable,
    m_InputIOTable,
    sizeof(m_OutputIOTable)/sizeof(GPIO_DATA),
    sizeof(m_InputIOTable)/sizeof(GPIO_DATA),
    
};
