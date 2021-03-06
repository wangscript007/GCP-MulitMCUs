#ifndef  __BOOTLOADER_H
#define  __BOOTLOADER_H

#include "DataType/DataType.h"

//BOOTLOADER 错误码定义
#define BOOT_ERR_SUCCESS                        (0)                 //成功
#define BOOT_ERR_FLASH_ADDR                     (1)                 //FLASH地址错误
#define BOOT_ERR_FLASH_OPS                      (2)                 //FLASH操作出错
#define BOOT_ERR_FILE_INFO                      (3)                 //BIN文件信息错误
#define BOOT_ERR_PACK_SIZE                      (4)                 //数据包的长度出错
#define BOOT_ERR_DATA_CHECK                     (5)                 //数据校验出错
#define BOOT_ERR_DATA_LOST                      (6)                 //数据丢失(数据包不连续)


#ifdef __cplusplus
extern "C"
{
#endif
    

/*****************************************************************************
 * BOOTLOADER相关控制接口
 ****************************************************************************/

/**
  * @brief  Bootloader FLASH初始化
  * @param  ulFlashBank 目标FALSH块
  * @param  nBinFileCheckSum BIN文件校验和
  * @param  ulBinFileLen  BIN文件长度
  * @retval 0-成功 非0-失败
  * @note   执行此函数,会清除掉目标FLASH里面的数据,并中止当前的loader(若存在)
  */
uBit32 BOOT_InitFlash(uBit32 ulFlashBank, uBit16 nBinFileCheckSum, uBit32 ulBinFileLen);


/**
  * @brief  升级数据存储
  * @param  pBuff 升级数据
  * @param  ulSize 数据长度 
  * @param  ulFinshFlg 最后一帧数据标志
  * @retval 0-成功 非0-失败
  * @note   若ulFinshFlg 为1, 则会立即将缓冲区的数据写入FLASH之中(不管缓冲区是否
  *         满,未使用的部分用0填充)
  */
uBit32 BOOT_StoreUpdateDataToBuff(uBit8 *pBuff, uBit32 ulSize, uBit32 ulFinshFlg);


/**
  * @brief  FLASH烧录完成校验
  * @param  None
  * @retval 0-成功 非0-失败
  */
uBit32 BOOT_CheckFlashLoaderFinsh(void);


#ifdef __cplusplus
}
#endif

#endif /* __BOOTLOADER_H */
