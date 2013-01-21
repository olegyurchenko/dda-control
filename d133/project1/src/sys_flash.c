/*----------------------------------------------------------------------------*/
/**
* @pkg sys_flash
*/
/**
* SPI flash AT45DB081D functions.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 21.01.2013  9:34:15<br>
* @pkgdoc sys_flash
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "sys_flash.h"
#include <stm32f10x.h>
#include <sys_timer.h>
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
#include <console.h>

static CONSOLE_CMD flash_console1, flash_console2;
static int flash_cmd1(int argc, char **argv);
static int flash_cmd2(int argc, char **argv);
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_13                 /* PB.13 */
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14                 /* PB.14 */
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15                 /* PB.15 */
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sFLASH_CS_PIN                        GPIO_Pin_2                  /* PD.02 */
#define sFLASH_CS_GPIO_PORT                  GPIOD
#define sFLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOD
/*----------------------------------------------------------------------------*/
#define cs_low()       GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define cs_high()      GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
/*----------------------------------------------------------------------------*/
/* JEDEC Manufacturerбпs ID */
#define MF_ID                (0x1F) /* atmel */
#define DENSITY_CODE_011D    (0x02) /* AT45DB011D Density Code : 00010 = 1-Mbit */
#define DENSITY_CODE_021D    (0x03) /* AT45DB021D Density Code : 00011 = 2-Mbit */
#define DENSITY_CODE_041D    (0x04) /* AT45DB041D Density Code : 00100 = 4-Mbit */
#define DENSITY_CODE_081D    (0x05) /* AT45DB081D Density Code : 00101 = 8-Mbit */
#define DENSITY_CODE_161D    (0x06) /* AT45DB161D Density Code : 00110 = 16-Mbit */
#define DENSITY_CODE_321D    (0x07) /* AT45DB321D Density Code : 00111 = 32-Mbit */
#define DENSITY_CODE_642D    (0x08) /* AT45DB642D Density Code : 01000 = 64-Mbit */
/*----------------------------------------------------------------------------*/
/*
user for AT45DB161.
copy form : http://www.ourdev.cn/bbs/bbs_content.jsp?bbs_sn=737106
thanks to gxlujd.
*/
#define AT45DB_BUFFER_1_WRITE                 0x84
#define AT45DB_BUFFER_2_WRITE                 0x87
#define AT45DB_BUFFER_1_READ                  0xD4
#define AT45DB_BUFFER_2_READ                  0xD6
#define AT45DB_B1_TO_MM_PAGE_PROG_WITH_ERASE  0x83
#define AT45DB_B2_TO_MM_PAGE_PROG_WITH_ERASE  0x86
#define AT45DB_MM_PAGE_TO_B1_XFER             0x53
#define AT45DB_MM_PAGE_TO_B2_XFER             0x55
#define AT45DB_PAGE_ERASE                     0x81
#define AT45DB_SECTOR_ERASE                   0x7C
#define AT45DB_READ_STATE_REGISTER            0xD7
#define AT45DB_MM_PAGE_READ                   0xD2
#define AT45DB_MM_PAGE_PROG_THRU_BUFFER1      0x82
#define AT45DB_CMD_JEDEC_ID                   0x9F
/*----------------------------------------------------------------------------*/
#define sFLASH_DUMMY_BYTE         0xA5
/*----------------------------------------------------------------------------*/
struct JEDEC_ID
{
    uint8_t manufacturer_id;          /* Manufacturer ID */
    uint8_t density_code:5; /* Density Code */
    uint8_t family_code:3;  /* Family Code */
    uint8_t version_code:5; /* Product Version Code */
    uint8_t mlc_code:3;     /* MLC Code */
    uint8_t byte_count;     /* Byte Count */
};
/*----------------------------------------------------------------------------*/
#define SPI_TIMEOUT 100 //5ms
/*----------------------------------------------------------------------------*/
void sflash_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO
       and sFLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK |
                         sFLASH_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< sFLASH_SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);

  /*!< Configure sFLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Deselect the FLASH: Chip Select high */
  cs_high();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;

  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sFLASH_SPI, ENABLE);

#ifdef USE_CONSOLE
  console_cmd_init(&flash_console1);
  flash_console1.cmd = "fid";
  flash_console1.help = "fid - Read FLASH identification";
  flash_console1.handler = flash_cmd1;
  console_add_cmd(&flash_console1);

  console_cmd_init(&flash_console2);
  flash_console2.cmd = "fs";
  flash_console2.help = "fid - Read FLASH status";
  flash_console2.handler = flash_cmd2;
  console_add_cmd(&flash_console2);
#endif //USE_CONSOLE
}
/*----------------------------------------------------------------------------*/
static int send_byte(uint8_t byte, void *dst)
{
  timeout_t tm;

  SPI_I2S_ReceiveData(sFLASH_SPI); // Clear out any pending RX data

  timeout_set(&tm, SPI_TIMEOUT, sys_tick_count());
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if(timeout_riched(&tm, sys_tick_count()))
      return 0;
  }
  //console_printf("send(%02x)\r\n", byte);
  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sFLASH_SPI, byte);

  timeout_set(&tm, SPI_TIMEOUT, sys_tick_count());
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE)==RESET || SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if(timeout_riched(&tm, sys_tick_count()))
      return 0;
  }

  /*!< Return the byte read from the SPI bus */
  if(dst != 0)
    *((uint8_t *)dst) = SPI_I2S_ReceiveData(sFLASH_SPI);
  //console_printf("rx = %02x\r\n", *((uint8_t *)dst));
  return 1;
}
/*----------------------------------------------------------------------------*/
static int send_then_recv(const void *src, unsigned send_count, void *dst, unsigned recv_count)
{
  const uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  unsigned count;

  tx_buffer = (const uint8_t *)src;
  rx_buffer = (uint8_t *)dst;

  for(count = 0; count < send_count; count++)
  {
    if(!send_byte(tx_buffer[count], 0))
      return 0;
  }

  for(count = 0; count < recv_count; count++)
  {
    if(!send_byte(sFLASH_DUMMY_BYTE, &rx_buffer[count]))
      return 0;
  }
  return 1;
}
/*----------------------------------------------------------------------------*/
int sflash_read_id(int32_t *dst)
{
  uint8_t cmd;
  uint8_t recv[6];
  uint32_t id = 0;
  int i;

  cs_low();
  cmd = AT45DB_CMD_JEDEC_ID;
  if(!send_then_recv(&cmd, 1, recv, 6))
  {
    cs_high();
    return 0;
  }
  cs_high();
  for(i = 0; i < 3; i++)
  {
    id <<= 8;
    id |= recv[i];
  }
  if(dst != 0)
    *dst = id;
  return 1;
}
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static int flash_cmd1(int argc, char **argv)
{
  int32_t flash_id = 0;
  (void)argc;
  (void)argv; //Prevent unused warnings

  if(!sflash_read_id(&flash_id))
    console_printf("\r\n%s\r\n", "SPI timeout error");
  else
    console_printf("\r\nJEDEC Read-ID:%08xh\r\n", flash_id);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int flash_cmd2(int argc, char **argv)
{
  int8_t rcv[16];
  uint8_t cmd;
  int i;

  (void)argc;
  (void)argv; //Prevent unused warnings

  cs_low();
  cmd = AT45DB_READ_STATE_REGISTER;
  if(!send_then_recv(&cmd, 1, rcv, 16))
  {
    console_printf("\r\n%s\r\n", "SPI timeout error");
  }
  else
  {
    for(i = 0; i < 16; i++)
      console_printf("\r\nStatus:%02xh\r\n", (unsigned)rcv[i] & 0xff);
  }

  cs_high();
  return CONSOLE_OK;
}
#endif
/*----------------------------------------------------------------------------*/
