/**
  ******************************************************************************
  * @file    HW_Tim.c
  * @author  Duhanfneg
  * @version V1.0
  * @date    2017.11.29
  * @brief   Time output drivers
  ******************************************************************************
  * @attention
  * 
  * 引脚,
  *     CAP0[0] --- P1[13] F4   P8[4]  F7  
  *     CAP0[1] --- P1[12] F4   P8[5]  F7
  *     CAP0[2] --- P1[20] F4   P8[6]  F7
  *     CAP0[3] --- P1[17] F4   P8[7]  F7
  *     CAP1[0] --- P5[0]  F5
  *     CAP1[1] --- P5[1]  F5
  *     CAP1[2] --- P5[2]  F5
  *     CAP1[3] --- P5[3]  F5
  *     CAP2[0] --- P6[1]  F5
  *     CAP2[1] --- P6[2]  F5
  *     CAP2[2] --- P6[3]  F5
  *     CAP2[3] --- P6[6]  F5
  *     CAP3[0] --- P2[0]  F6   PC[1]  F6
  *     CAP3[1] --- P2[1]  F6   PC[4]  F6
  *     CAP3[2] --- P2[2]  F6   PC[5]  F6
  *     CAP3[3] --- P2[6]  F6   PC[6]  F6
  *             --- 
  *     MAT0[0] --- P1[16] F4   P8[0]  F7  
  *     MAT0[1] --- P1[15] F4   P8[1]  F7
  *     MAT0[2] --- P1[14] F4   P8[2]  F7
  *     MAT0[3] --- P1[18] F4   P8[3]  F7
  *     MAT1[0] --- P5[4]  F5
  *     MAT1[1] --- P5[5]  F5
  *     MAT1[2] --- P5[6]  F5
  *     MAT1[3] --- P5[7]  F5
  *     MAT2[0] --- P6[7]  F5
  *     MAT2[1] --- P6[8]  F5
  *     MAT2[2] --- P6[9]  F5
  *     MAT2[3] --- P6[11] F5
  *     MAT3[0] --- P2[3]  F6   PC[7]  F6
  *     MAT3[1] --- P2[4]  F6   PC[8]  F6
  *     MAT3[2] --- P2[5]  F6   PC[9]  F6
  *     MAT3[3] --- P2[7]  F6   PC[10] F6
  * 
  ******************************************************************************
  */
  
/***********************************<INCLUDES>**********************************/
#include "HW_Tim.h"
#include "chip.h"
      

/*****************************************************************************
 * 私有成员定义及实现
 ****************************************************************************/
#define TIM_GET_PR_BY_CNT_FRE(CntFre)            (Chip_TIME_GetClock(TIM[uTimeNode])/(CntFre) - 1)   //通过计数频率计算预分频值
#define TIM_GET_PR_BY_OP_FRE(OutFre, AutoLoad)   (TIM_GET_PR_BY_CNT_FRE((OutFre) * (AutoLoad))) //通过输出频率计算预分频值(计数频率=输出频率*自动重装载值)

//定时器数量定义
#define HW_TIME_COUNT               (4)             //定时器数量

#define HW_TIME_OUT_CH_COUNT        (4)             //输出通道数量
#define HW_TIME_OUT_CH_MASK(n)      ((n)&0xF)       //输出通道掩码

#define HW_TIME_IN_CH_COUNT         (4)             //输入通道数量
#define HW_TIME_IN_CH_MASK(n)       ((n)&0xF)       //输入通道掩码

#define HW_TIME_DEF_COUNT_RANG      (2)             //默认定时器计数范围

#define HW_TIME_DEF_CAPTURE_FRE     (1000000)       //默认捕获频率(1US精度)


//定时器相关变量定义
static LPC_TIMER_T * const TIM[HW_TIME_COUNT] = {LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3};
static const IRQn_Type TIM_IRQn[HW_TIME_COUNT] = {TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn};   //中断向量


//TIM输出IO引脚定义
static const PINMUX_GRP_T m_TimeOutputPinMux[HW_TIME_COUNT][HW_TIME_OUT_CH_COUNT] = 
{
#if 1
    //TIM0
    {
        {0x01, 16, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 15, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 14, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 18, SCU_MODE_INACT | SCU_MODE_FUNC4},
    },
#else 
    //TIM0
    {
        {0x08,  0, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  1, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  2, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  3, SCU_MODE_INACT | SCU_MODE_FUNC7},
    },
#endif
    
    //TIM1
    {
        {0x05,  4, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  5, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  6, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  7, SCU_MODE_INACT | SCU_MODE_FUNC5},
    },
    
    //TIM2
    {
        {0x06,  7, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06,  8, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06,  9, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06, 11, SCU_MODE_INACT | SCU_MODE_FUNC5},
    },
    
#if 1
    //TIM3
    {
        {0x02,  3, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  4, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  5, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  7, SCU_MODE_INACT | SCU_MODE_FUNC6},
    },
#else 
    //TIM3
    {
        {0x0C,  7, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C,  8, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C,  9, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C, 10, SCU_MODE_INACT | SCU_MODE_FUNC6},
    },
#endif

};


//TIM输入IO引脚定义
static const PINMUX_GRP_T m_TimeInputPinMux[HW_TIME_COUNT][HW_TIME_IN_CH_COUNT] = 
{
#if 1
    //TIM0
    {
        {0x01, 13, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 12, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 20, SCU_MODE_INACT | SCU_MODE_FUNC4},
        {0x01, 17, SCU_MODE_INACT | SCU_MODE_FUNC4},
    },
#else 
    //TIM0
    {
        {0x08,  4, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  5, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  6, SCU_MODE_INACT | SCU_MODE_FUNC7},
        {0x08,  7, SCU_MODE_INACT | SCU_MODE_FUNC7},
    },
#endif
    
    //TIM1
    {
        {0x05,  0, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  1, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  2, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x05,  3, SCU_MODE_INACT | SCU_MODE_FUNC5},
    },
    
    //TIM2
    {
        {0x06,  1, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06,  2, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06,  3, SCU_MODE_INACT | SCU_MODE_FUNC5},
        {0x06,  6, SCU_MODE_INACT | SCU_MODE_FUNC5},
    },
    
#if 1
    //TIM3
    {
        {0x02,  0, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  1, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  2, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x02,  6, SCU_MODE_INACT | SCU_MODE_FUNC6},
    },
#else 
    //TIM3
    {
        {0x0C,  1, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C,  4, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C,  5, SCU_MODE_INACT | SCU_MODE_FUNC6},
        {0x0C,  6, SCU_MODE_INACT | SCU_MODE_FUNC6},
    },
#endif

};


/**
  * @brief  定时器输出IO配置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码
  * @retval None
  */
static void HW_TIM_ConfigOutputIO(uint8_t uTimeNode, uint8_t uChannelMask)
{
    for (int i = 0; i < HW_TIME_OUT_CH_COUNT; i++)
    {
        if (HW_TIME_OUT_CH_MASK(uChannelMask) & (0x1<<i))
        {
            //初始化引脚
            Chip_SCU_SetPinMuxing(&m_TimeOutputPinMux[uTimeNode][i], 1);
        }
    }

}


/**
  * @brief  定时器输入IO配置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码
  * @retval None
  */
static void HW_TIM_ConfigInputIO(uint8_t uTimeNode, uint8_t uChannelMask)
{
    for (int i = 0; i < HW_TIME_IN_CH_COUNT; i++)
    {
        if (HW_TIME_IN_CH_MASK(uChannelMask) & (0x1<<i))
        {
            //初始化引脚
            Chip_SCU_SetPinMuxing(&m_TimeInputPinMux[uTimeNode][i], 1);
        }
    }

}


/**
  * @brief  定时器外设时钟获取
  * @param  uTimeNode 定时器节点
  * @retval 定时器时钟
  */
static uint32_t HW_TIM_GetPeripheralClock(uint8_t uTimeNode)
{
    uint32_t ulTimeClock = 0;
    
    switch (uTimeNode)
    {
    case HW_TIME_NODE_0: ulTimeClock = Chip_Clock_GetRate(CLK_MX_TIMER0); break;
    case HW_TIME_NODE_1: ulTimeClock = Chip_Clock_GetRate(CLK_MX_TIMER1); break;
    case HW_TIME_NODE_2: ulTimeClock = Chip_Clock_GetRate(CLK_MX_TIMER2); break;
    case HW_TIME_NODE_3: ulTimeClock = Chip_Clock_GetRate(CLK_MX_TIMER3); break;
    default: break; //不应该到这里
    }
    
    return ulTimeClock;
}


/*****************************************************************************
 * 定时器输出相关控制接口
 ****************************************************************************/

/**
  * @brief  定时器输出模式配置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码字
  * @param  uOutputCfgCode 工作模式
  * @retval None
  */
static void HW_TIM_ConfigOutputMode(uint8_t uTimeNode, uint8_t uChannelMask)
{
    //初始化模块
    Chip_TIMER_Init(TIM[uTimeNode]);
    
    //复位计数器
    Chip_TIMER_Reset(TIM[uTimeNode]);
    
    //设置PWM模式(计时)
    Chip_TIMER_TIMER_SetCountClockSrc(TIM[uTimeNode], TIMER_CAPSRC_RISING_PCLK, 0);
    
    //设置PWM触发操作(复位计数)
    Chip_TIMER_ResetOnMatchEnable(TIM[uTimeNode], 3);
    
    //设置PWM时序
    Chip_TIMER_SetMatch(TIM[uTimeNode], 0, HW_TIME_DEF_COUNT_RANG-1);
    Chip_TIMER_SetMatch(TIM[uTimeNode], 1, HW_TIME_DEF_COUNT_RANG-1);
    Chip_TIMER_SetMatch(TIM[uTimeNode], 3, HW_TIME_DEF_COUNT_RANG-1);
    
    //外部引脚状态设置(翻转)
    Chip_TIMER_ExtMatchControlSet(TIM[uTimeNode], 0, TIMER_EXTMATCH_TOGGLE, 0);
    Chip_TIMER_ExtMatchControlSet(TIM[uTimeNode], 0, TIMER_EXTMATCH_TOGGLE, 1);
    
}


/**
  * @brief  定时器输出模式初始化
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码字
  * @param  uOutputCfgCode 工作模式
  * @retval None
  * @note   初始化后,默认已经打开定时器通道,但是尚未启动定时器
  */
void HW_TIM_InitOutput(uint8_t uTimeNode, uint8_t uChannelMask)
{
    //引脚配置
    HW_TIM_ConfigOutputIO(uTimeNode, uChannelMask);
    
    //模式配置
    HW_TIM_ConfigOutputMode(uTimeNode, uChannelMask);
    
    //时序配置
    HW_TIM_SetOutputPwmDutyRatio(uTimeNode, uChannelMask, 50);
    HW_TIM_SetOutputPwmFrq(uTimeNode, 1000);
    
    //开启定时器
    //HW_TIM_EnableOutput(uTimeNode, 1);
    
}


/**
  * @brief  定时器通道使能设置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码字
  * @param  bIsEnble true-使能 flase-关闭
  * @retval None
  * @note   选择的通道,必须是已初始化的通道
  */
void HW_TIM_EnableChannel(uint8_t uTimeNode, uint8_t uChannelMask, bool bIsEnble)
{
    if (bIsEnble)
    {
        //使能定时器通道
        HW_TIM_SetOutputPwmDutyRatio(uTimeNode, uChannelMask, 50);
    }
    else 
    {
        //禁止定时器通道
        HW_TIM_SetOutputPwmDutyRatio(uTimeNode, uChannelMask, 0);
    }
    
}


/**
  * @brief  定时器输出中断使能
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码字
  * @param  bIsEnable 使能位
  * @retval None
  */
void HW_TIM_EnableOutputIRQ(uint8_t uTimeNode, uint8_t uChannelMask, bool bIsEnable)
{
    if (bIsEnable)
    {
        NVIC_DisableIRQ(TIM_IRQn[uTimeNode]);
        NVIC_SetPriority(TIM_IRQn[uTimeNode], ((0x01 << 3) | 0x01));
        NVIC_EnableIRQ(TIM_IRQn[uTimeNode]);
    }
    else 
    {
        NVIC_DisableIRQ(TIM_IRQn[uTimeNode]);
    }
    
}


/**
  * @brief  定时器输出PWM占空比设置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 定时器通道掩码,位值为1时开启对应的通道
  * @param  fDutyRatio PWM波的占空比,范围为[0,100]
  * @retval None
  */
void HW_TIM_SetOutputPwmDutyRatio(uint8_t uTimeNode, uint8_t uChannelMask, float fDutyRatio)
{
    uint32_t ulCmpVal = TIM[uTimeNode]->MR[3];
    uint16_t nRegValue = (uint16_t)((fDutyRatio * ulCmpVal) / 100.0);
    
    for (int i = 0; i < HW_TIME_OUT_CH_COUNT; i++)
    {
        if (HW_TIME_OUT_CH_MASK(uChannelMask) & (0x01<<i))
        {
            Chip_TIMER_SetMatch(TIM[uTimeNode], i, nRegValue);
        }
    }
    
}


/**
  * @brief  定时器输出比较值设置
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码
  * @param  nCompareVal 比较值
  * @retval None
  */
void HW_TIM_SetOutputCmpVal(uint8_t uTimeNode, uint8_t uChannelMask, uint16_t nCompareVal)
{
    for (int i = 0; i < HW_TIME_OUT_CH_COUNT; i++)
    {
        if (HW_TIME_OUT_CH_MASK(uChannelMask) & (0x01<<i))
        {
            Chip_TIMER_SetMatch(TIM[uTimeNode], i, nCompareVal);
        }
    }
    
}


/**
  * @brief  定时器输出PWM频率设置
  * @param  uTimeNode 定时器节点
  * @param  ulFrequency  PWM波的频率
  * @retval None
  */
void HW_TIM_SetOutputPwmFrq(uint8_t uTimeNode, uint32_t ulFrequency)
{
    uint32_t ulTimeClock = HW_TIM_GetPeripheralClock(uTimeNode);
    uint32_t ulPrescale = ((ulTimeClock / (ulFrequency*HW_TIME_DEF_COUNT_RANG*2)))-1;
    Chip_TIMER_PrescaleSet(TIM[uTimeNode], ulPrescale);
        
}


/**
  * @brief  定时器输出自动重加载值设置
  * @param  nReloadVal 重加载值
  * @retval None
  */
void HW_TIM_SetOutputAutoReloadValue(uint8_t uTimeNode, uint16_t nReloadVal)
{
    Chip_TIMER_SetMatch(TIM[uTimeNode], 3, nReloadVal);
    
}


/**
  * @brief  定时器输出自动重加载值获取
  * @param  uTimeNode 定时器节点
  * @retval 定时器中的重加载值
  */
uint16_t HW_TIM_GetOutputAutoReloadValue(uint8_t uTimeNode)
{
    
    return TIM[uTimeNode]->MR[3];
}


/**
  * @brief  定时器输出使能
  * @param  uTimeNode 定时器节点
  * @param  bIsEnable 定时器使能位
  * @retval None
  */
void HW_TIM_EnableOutput(uint8_t uTimeNode, bool bIsEnable)
{
    if (bIsEnable)
    {
        Chip_TIMER_Enable(TIM[uTimeNode]);
    }
    else 
    {
        Chip_TIMER_Disable(TIM[uTimeNode]);
    }
    
}


/*****************************************************************************
 * 定时器输入计数相关控制接口
 ****************************************************************************/

/**
  * @brief  定时器输入模式初始化
  * @param  uTimeNode 定时器节点
  * @param  uChannelNum 通道编号(从0算起)
  * @retval None
  */
static void HW_TIM_ConfigInputCountMode(uint8_t uTimeNode, uint8_t uChannelNum)
{
    //初始化模块
    Chip_TIMER_Init(TIM[uTimeNode]);
    
    //复位计数器
    Chip_TIMER_Reset(TIM[uTimeNode]);
    
    //设置PWM模式(计时)
    Chip_TIMER_TIMER_SetCountClockSrc(TIM[uTimeNode], TIMER_CAPSRC_FALLING_CAPN, uChannelNum);
    
    //复位PWM触发操作
    TIM[uTimeNode]->MCR = 0;
    
}


/**
  * @brief  定时器输入计数初始化
  * @param  uTimeNode 定时器节点
  * @param  uChannelMask 通道掩码字
  * @retval None
  */
void HW_TIM_InitInputCount(uint8_t uTimeNode, uint8_t uChannelMask)
{
    //引脚配置
    HW_TIM_ConfigInputIO(uTimeNode, uChannelMask);
    
    //模式配置
    uint8_t uChannelNum = (uChannelMask & 0x1) ? 0 : 1;
    HW_TIM_ConfigInputCountMode(uTimeNode, uChannelNum);
    
}


/**
  * @brief  定时器计数使能
  * @param  uTimeNode 定时器节点
  * @param  bIsEnable 使能位
  * @retval None
  */
void HW_TIM_EnableInputCount(uint8_t uTimeNode, bool bIsEnable)
{
    if (bIsEnable)
    {
        //启动定时器
        Chip_TIMER_Enable(TIM[uTimeNode]);
    }
    else 
    {
        //关闭定时器
        Chip_TIMER_Disable(TIM[uTimeNode]);
    }
    
}


/**
  * @brief  定时器最大输入计数设置
  * @param  uTimeNode 定时器节点
  * @param  ulMaxCount 最大计数值
  * @retval None
  * @note   到达最大计数值后,计数寄存器会复位;若设置多个通道的"最大计数值",以最小的计数值为准;
  */
void HW_TIM_SetMaxInputCount(uint8_t uTimeNode, uint8_t uChannelNum, uint32_t ulMaxCount)
{
    //设置最大计数值
    Chip_TIMER_ResetOnMatchEnable(TIM[uTimeNode], uChannelNum);     //到达最大值时复位
    Chip_TIMER_SetMatch(TIM[uTimeNode], uChannelNum, ulMaxCount);   //设置最大计数值
    
}


/**
  * @brief  定时器计数值获取
  * @param  uTimeNode 定时器节点
  * @retval 当前计数值
  */
uint32_t HW_TIM_GetCurInputCount(uint8_t uTimeNode)
{
    
    return Chip_TIMER_ReadCount(TIM[uTimeNode]);
}


/*****************************************************************************
 * 定时器输入捕获相关控制接口
 ****************************************************************************/

/**
  * @brief  捕获模式初始化
  * @param  uTimeNode 定时器节点
  * @param  uChannelNum 通道编号(从0算起)
  * @param  uTrgSource 触发源(1-上升沿捕获 2-下降沿捕获 3-双边沿捕获)
  * @retval 0-成功 非0-失败
  */
void HW_TIM_InitCapture(uint8_t uTimeNode, uint8_t uChannelNum, uint8_t uTrgSource)
{
    //引脚配置
    HW_TIM_ConfigInputIO(uTimeNode, 0x1<<uChannelNum);
    
    //初始化模块
    Chip_TIMER_Init(TIM[uTimeNode]);
    
    //设置时钟源(使用内部PCLK时钟)
    Chip_TIMER_TIMER_SetCountClockSrc(TIM[uTimeNode], TIMER_CAPSRC_RISING_PCLK, 0);
    
    //设置计数频率
    uint32_t ulTimeClock = HW_TIM_GetPeripheralClock(uTimeNode);
    uint32_t ulPrescale = ((ulTimeClock / HW_TIME_DEF_CAPTURE_FRE))-1;
    Chip_TIMER_PrescaleSet(TIM[uTimeNode], ulPrescale);
    
    //复位计数器
    Chip_TIMER_Reset(TIM[uTimeNode]);
    
    //设置触发边沿
    if (uTrgSource & HW_TIM_CAP_EDGE_RISING)
    {
        Chip_TIMER_CaptureRisingEdgeEnable(TIM[uTimeNode], uChannelNum);
    }
    if (uTrgSource & HW_TIM_CAP_EDGE_FALLING)
    {
        Chip_TIMER_CaptureFallingEdgeEnable(TIM[uTimeNode], uChannelNum);
    }
    
}


/**
  * @brief  捕获捕获边缘设置
  * @param  uTimeNode 定时器节点
  * @param  uChannelNum 通道编号(从0算起)
  * @param  uTrgSource 触发源(1-上升沿捕获 2-下降沿捕获 3-双边沿捕获)
  * @retval None
  */
void HW_TIM_SetCaptureEdge(uint8_t uTimeNode, uint8_t uChannelNum, uint8_t uTrgSource)
{
    //设置触发边沿
    if (uTrgSource & HW_TIM_CAP_EDGE_RISING)
    {
        Chip_TIMER_CaptureRisingEdgeEnable(TIM[uTimeNode], uChannelNum);
    }
    if (uTrgSource & HW_TIM_CAP_EDGE_FALLING)
    {
        Chip_TIMER_CaptureFallingEdgeEnable(TIM[uTimeNode], uChannelNum);
    }
    
}


/**
  * @brief  定时器输入使能
  * @param  uTimeNode 定时器节点
  * @param  bIsEnable 定时器使能位
  * @retval None
  */
void HW_TIM_EnableCapture(uint8_t uTimeNode, bool bIsEnable)
{
    Chip_TIMER_Enable(TIM[uTimeNode]);
    
}


/**
  * @brief  计数器复位
  * @param  uTimeNode 定时器节点
  * @retval None
  */
void HW_TIM_ResetCounter(uint8_t uTimeNode)
{
    //复位计数器
    Chip_TIMER_Reset(TIM[uTimeNode]);
    
}


/**
  * @brief  定时器输入捕获值获取
  * @param  uTimeNode 定时器节点
  * @param  uChannelNum 通道编号(从0算起)
  * @retval 定时器通道捕获寄存器值
  */
uint32_t HW_TIM_GetCaptureValue(uint8_t uTimeNode, uint8_t uChannelNum)
{
    
    return Chip_TIMER_ReadCapture(TIM[uTimeNode], uChannelNum);
}


/*****************************************************************************
 * 定时器输入中断相关控制接口
 ****************************************************************************/

/**
  * @brief  MCPWM计数使能
  * @param  uTimeNode 定时器节点
  * @param  uChannelNum 通道编号(从0算起)
  * @param  uIntModeMask 中断模式掩码(可以多种模式'与'后入参)
  * @param  bIsEnable 使能位
  * @retval None
  */
void HW_TIM_EnableIRQ(uint8_t uTimeNode, uint8_t uChannelNum, uint8_t uIntModeMask, bool bIsEnable)
{
    if (bIsEnable)
    {
        //开外设中断
        if (uIntModeMask & HW_TIM_INT_MATCH)
        {
            Chip_TIMER_MatchEnableInt(TIM[uTimeNode], uChannelNum);
        }
        if (uIntModeMask & HW_TIM_INT_CAPTURE)
        {
            Chip_TIMER_CaptureEnableInt(TIM[uTimeNode], uChannelNum);
        }
        
        //开内核中断
        NVIC_EnableIRQ(TIM_IRQn[uTimeNode]);
    }
    else 
    {
        //关外设中断
        if (uIntModeMask & HW_TIM_INT_MATCH)
        {
            Chip_TIMER_MatchDisableInt(TIM[uTimeNode], uChannelNum);
        }
        if (uIntModeMask & HW_TIM_INT_CAPTURE)
        {
            Chip_TIMER_CaptureDisableInt(TIM[uTimeNode], uChannelNum);
        }
    }
    
}


/**
  * @brief  MCPWM中断状态获取
  * @param  uTimeNode 定时器节点
  * @param  uIntMode 中断模式(仅能获取一种中断的标志)
  * @retval None
  */
bool HW_TIM_GetITStatus(uint8_t uTimeNode, uint8_t uChannelNum, uint8_t uIntMode)
{
    bool bITStatus = false;
    
    if (uIntMode & HW_TIM_INT_MATCH)
    {
        bITStatus = Chip_TIMER_MatchPending(TIM[uTimeNode], uChannelNum);
    }
    else if (uIntMode & HW_TIM_INT_CAPTURE)
    {
        bITStatus = Chip_TIMER_CapturePending(TIM[uTimeNode], uChannelNum);
    }
    
    return bITStatus;
}


/**
  * @brief  MCPWM中断状态清除
  * @param  uTimeNode 定时器节点
  * @param  uIntModeMask 中断模式掩码(可以多种模式'与'后入参)
  * @retval None
  */
void HW_TIM_ClearITStatus(uint8_t uTimeNode, uint8_t uChannelNum, uint8_t uIntModeMask)
{
    if (uIntModeMask & HW_TIM_INT_MATCH)
    {
        Chip_TIMER_ClearMatch(TIM[uTimeNode], uChannelNum);
    }
    if (uIntModeMask & HW_TIM_INT_CAPTURE)
    {
        Chip_TIMER_ClearCapture(TIM[uTimeNode], uChannelNum);
    }
}

