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
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
#include <console.h>
#include <dda_uart.h>
#define FLASH_DEBUG

static CONSOLE_CMD flash_console1, flash_console2, flash_console3, flash_console4;
static int flash_cmd1(int argc, char **argv);
static int flash_cmd2(int argc, char **argv);
static int flash_cmd3(int argc, char **argv);
static int flash_cmd4(int argc, char **argv);
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
#ifdef FLASH_DEBUG
#define FLASH_TRACE         console_printf
#else
#define FLASH_TRACE(...)
#endif /**< #ifdef FLASH_DEBUG */
/*----------------------------------------------------------------------------*/
static int32_t bytes_per_page = 256; /* Page Erase (256 Bytes) */
static int32_t page_count = 512*8;   /* 8-Mbit / 8 / 256 = 4096 */
static int32_t block_size = 1024*2;    /* Block Erase (2-Kbytes) */
/*----------------------------------------------------------------------------*/
//flash_mode flags
#define FLASH_READ 1
#define FLASH_WRITE 2
#define FLASH_MODIFIED 4
/*----------------------------------------------------------------------------*/
static int flash_mode = 0;
static uint8_t sflash_buffer[1024];
static off_t sflash_offset = 0;
/*----------------------------------------------------------------------------*/
static int send_byte(uint8_t byte, void *dst);
static int send_then_recv(const void *src, size_t send_count, void *dst, size_t recv_count);
static int send_then_send(const void *src, size_t send_count, const void *data, size_t data_count);
int sflash_status(uint8_t *dst);
static int wait_busy(void);
int sflash_page_write(unsigned page, const void *src);
int sflash_page_read(unsigned page, void *dst);
/*----------------------------------------------------------------------------*/
static void sflash_setup(void);
static size_t AT45DB_flash_read_page_256(off_t pos, void* buffer, size_t size);
static size_t AT45DB_flash_read_page_512(off_t pos, void* buffer, size_t size);
static size_t AT45DB_flash_read_page_1024(off_t pos, void* buffer, size_t size);
static size_t AT45DB_flash_write_page_256(off_t pos, const void* buffer, size_t size);
static size_t AT45DB_flash_write_page_512(off_t pos, const void* buffer, size_t size);
static size_t AT45DB_flash_write_page_1024(off_t pos, const void* buffer, size_t size);
/*----------------------------------------------------------------------------*/
typedef size_t (*read_page_t)(off_t, void*, size_t);
typedef size_t (*write_page_t)(off_t, const void*, size_t);
/*----------------------------------------------------------------------------*/
static read_page_t sflash_page_reader = AT45DB_flash_read_page_256;
static write_page_t sflash_page_writer = AT45DB_flash_write_page_256;
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
#define SPI_TIMEOUT 5 //5ms
#define FLASH_TIMEOUT 100
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
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sFLASH_SPI, ENABLE);
  sflash_setup();

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

  console_cmd_init(&flash_console3);
  flash_console3.cmd = "fd";
  flash_console3.help = "fd [<start addr> [<size>]] - Dump flash memory";
  flash_console3.handler = flash_cmd3;
  console_add_cmd(&flash_console3);

  console_cmd_init(&flash_console4);
  flash_console4.cmd = "ff";
  flash_console4.help = "ff <start addr> <value> [<size>] - Fill <value> into flash memory";
  flash_console4.handler = flash_cmd4;
  console_add_cmd(&flash_console4);


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
static int send_then_recv(const void *src, size_t send_count, void *dst, size_t recv_count)
{
  const uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  size_t count;

  tx_buffer = (const uint8_t *)src;
  rx_buffer = (uint8_t *)dst;

  cs_low();
  for(count = 0; count < send_count; count++)
  {
    if(!send_byte(tx_buffer[count], 0))
    {
      cs_high();
      return 0;
    }
  }

  for(count = 0; count < recv_count; count++)
  {
    if(!send_byte(sFLASH_DUMMY_BYTE, &rx_buffer[count]))
    {
      cs_high();
      return 0;
    }
  }
  cs_high();
  return 1;
}
/*----------------------------------------------------------------------------*/
static int send_then_send(const void *src, size_t send_count, const void *data, size_t data_count)
{
  const uint8_t *tx_buffer;
  const uint8_t *data_buffer;
  size_t count;

  tx_buffer = (const uint8_t *)src;
  data_buffer = (const uint8_t *)data;

  cs_low();
  for(count = 0; count < send_count; count++)
  {
    if(!send_byte(tx_buffer[count], 0))
    {
      cs_high();
      return 0;
    }
  }

  for(count = 0; count < data_count; count++)
  {
    if(!send_byte(data_buffer[count], 0))
    {
      cs_high();
      return 0;
    }
  }
  cs_high();
  return 1;
}
/*----------------------------------------------------------------------------*/
static int wait_busy(void)
{
  uint8_t flashstatus = 0;
  timeout_t tm;

  timeout_set(&tm, FLASH_TIMEOUT, sys_tick_count());

  cs_low();

  /*!< Send "Read Status Register" instruction */
  send_byte(AT45DB_READ_STATE_REGISTER, 0);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    if(timeout_riched(&tm, sys_tick_count()))
    {
      FLASH_TRACE("FLASH timeout\r\n");
      cs_high();
      return 0;
    }
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    send_byte(sFLASH_DUMMY_BYTE, &flashstatus);
  }
  while (!(flashstatus & 0x80)); /* Write in progress */

  cs_high();
  return 1;
}
/*----------------------------------------------------------------------------*/
static void sflash_setup(void)
{
  uint8_t cmd;
  uint8_t id_recv[6];
  struct JEDEC_ID* JEDEC_ID;

  JEDEC_ID = (struct JEDEC_ID *)id_recv;

  cmd = AT45DB_CMD_JEDEC_ID;
  if(!send_then_recv(&cmd, 1, id_recv, 6))
  {
    FLASH_TRACE("SPI timeout\r\n");
    return;
  }

  /**< 1FH = Atmel */
  /**< 001 = Atmel DataFlash */
  if(JEDEC_ID->manufacturer_id != 0x1F || JEDEC_ID->family_code != 0x01)
  {
    FLASH_TRACE("Manufacturers ID or Memory Type error!\r\n");
    FLASH_TRACE("JEDEC Read-ID Data : %02X %02X %02X\r\n", id_recv[0], id_recv[1], id_recv[2]);
    return;
  }

  if(JEDEC_ID->density_code == DENSITY_CODE_011D)
  {
    /**< AT45DB011D Density Code : 00010 = 1-Mbit */
    FLASH_TRACE("AT45DB011D detection\r\n");
    bytes_per_page = 256; /* Page Erase (256 Bytes) */
    page_count = 512;     /* 1-Mbit / 8 / 256 = 512 */
    block_size = 1024*2;    /* Block Erase (2-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_021D)
  {
    /**< AT45DB021D Density Code : 00011 = 2-Mbit */
    FLASH_TRACE("AT45DB021D detection\r\n");
    bytes_per_page = 256; /* Page Erase (256 Bytes) */
    page_count = 512*2;   /* 2-Mbit / 8 / 256 = 1024 */
    block_size = 1024*2;    /* Block Erase (2-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_041D)
  {
    /**< AT45DB041D Density Code : 00100 = 4-Mbit */
    FLASH_TRACE("AT45DB041D detection\r\n");
    bytes_per_page = 256; /* Page Erase (256 Bytes) */
    page_count = 512*4;   /* 4-Mbit / 8 / 256 = 2048 */
    block_size = 1024*2;    /* Block Erase (2-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_081D)
  {
    /**< AT45DB081D Density Code : 00101 = 8-Mbit */
    FLASH_TRACE("AT45DB081D detection\r\n");
    bytes_per_page = 256; /* Page Erase (256 Bytes) */
    page_count = 512*8;   /* 8-Mbit / 8 / 256 = 4096 */
    block_size = 1024*2;    /* Block Erase (2-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_161D)
  {
    /**< AT45DB161D Density Code : 00110 = 16-Mbit */
    FLASH_TRACE("AT45DB161D detection\r\n");
    bytes_per_page = 512; /* Page Erase (512 Bytes) */
    page_count = 256*16;  /* 16-Mbit / 8 / 512 = 4096 */
    block_size = 1024*4;    /* Block Erase (4-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_321D)
  {
    /**< AT45DB321D Density Code : 00111 = 32-Mbit */
    FLASH_TRACE("AT45DB321D detection\r\n");
    bytes_per_page = 512; /* Page Erase (512 Bytes) */
    page_count = 256*32;  /* 32-Mbit / 8 / 512 = 8192 */
    block_size = 1024*4;    /* Block Erase (4-Kbytes) */
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_642D)
  {
    /**< AT45DB642D Density Code : 01000 = 64-Mbit */
    FLASH_TRACE("AT45DB642D detection\r\n");
    bytes_per_page = 1024; /* Page Erase (1 Kbyte) */
    page_count = 128*64;   /* 64-Mbit / 8 / 1024 = 8192 */
    block_size = 1024*8;     /* Block Erase (8 Kbytes) */
  }
  else
  {
    FLASH_TRACE("Memory Capacity error!\r\n");
  }

  if(JEDEC_ID->density_code == DENSITY_CODE_642D)
  {
    sflash_page_reader = AT45DB_flash_read_page_1024;
    sflash_page_writer = AT45DB_flash_write_page_1024;
  }
  else if(JEDEC_ID->density_code == DENSITY_CODE_161D
          || JEDEC_ID->density_code == DENSITY_CODE_321D )
  {
    sflash_page_reader = AT45DB_flash_read_page_512;
    sflash_page_writer = AT45DB_flash_write_page_512;
  }
  else
  {
    sflash_page_reader = AT45DB_flash_read_page_256;
    sflash_page_writer = AT45DB_flash_write_page_256;
  }
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_read_page_256(off_t pos, void* buffer, size_t size)
{
    uint32_t index, nr;
    uint8_t *read_buffer = buffer;
    uint32_t page = pos;

    //FLASH_TRACE("AT45DB_flash_read_page_256(%04x)\r\n", page);
    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[8];
      uint32_t i;

      for(i=0; i<sizeof(send_buffer); i++)
      {
        send_buffer[i] = 0;
      }

      send_buffer[0] = AT45DB_MM_PAGE_READ;
      send_buffer[1] = (uint8_t)(page >> 7);
      send_buffer[2] = (uint8_t)(page << 1);

      if(!send_then_recv(send_buffer, 8, read_buffer, 256))
        return 0;

      read_buffer += 256;
      page++;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_read_page_512(off_t pos, void* buffer, size_t size)
{
    uint32_t index, nr;
    uint8_t * read_buffer = buffer;
    uint32_t  page = pos;

    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[8];
      uint32_t i;

      for(i=0; i<sizeof(send_buffer); i++)
      {
        send_buffer[i] = 0;
      }

      send_buffer[0] = AT45DB_MM_PAGE_READ;
      send_buffer[1] = (uint8_t)(page >> 6);
      send_buffer[2] = (uint8_t)(page << 2);

      if(!send_then_recv(send_buffer, 8, read_buffer, 512))
        return 0;

      read_buffer += 512;
      page++;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_read_page_1024(off_t pos, void* buffer, size_t size)
{
    uint32_t index, nr;
    uint8_t * read_buffer = buffer;
    uint32_t  page = pos;

    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[8];
      uint32_t i;

      for(i=0; i<sizeof(send_buffer); i++)
      {
        send_buffer[i] = 0;
      }

      send_buffer[0] = AT45DB_MM_PAGE_READ;
      send_buffer[1] = (uint8_t)(page >> 5);
      send_buffer[2] = (uint8_t)(page << 3);

      if(!send_then_recv(send_buffer, 8, read_buffer, 1024))
        return 0;

      read_buffer += 1024;
      page++;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_write_page_256(off_t pos, const void* buffer, size_t size)
{
    uint32_t index, nr;
    const uint8_t * write_buffer = buffer;
    uint32_t  page = pos;

    //FLASH_TRACE("AT45DB_flash_write_page_256(%04x)\r\n", page);
    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[4];

      send_buffer[0] = AT45DB_MM_PAGE_PROG_THRU_BUFFER1;
      send_buffer[1] = (uint8_t) (page >> 7);
      send_buffer[2] = (uint8_t) (page << 1);
      send_buffer[3] = 0;

      if(!send_then_send(send_buffer, 4, write_buffer, 256))
        return 0;

      write_buffer += 256;
      page++;

      if(!wait_busy())
        return 0;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_write_page_512(off_t pos, const void* buffer, size_t size)
{
    uint32_t index, nr;
    const uint8_t * write_buffer = buffer;
    uint32_t  page = pos;

    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[4];

      send_buffer[0] = AT45DB_MM_PAGE_PROG_THRU_BUFFER1;
      send_buffer[1] = (uint8_t) (page >> 6);
      send_buffer[2] = (uint8_t) (page << 2);
      send_buffer[3] = 0;

      if(!send_then_send(send_buffer, 4, write_buffer, 512))
        return 0;

      write_buffer += 512;
      page++;

      if(!wait_busy())
        return 0;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
static size_t AT45DB_flash_write_page_1024(off_t pos, const void* buffer, size_t size)
{
    uint32_t index, nr;
    const uint8_t * write_buffer = buffer;
    uint32_t  page = pos;

    nr = size;

    for (index = 0; index < nr; index++)
    {
      uint8_t send_buffer[4];

      send_buffer[0] = AT45DB_MM_PAGE_PROG_THRU_BUFFER1;
      send_buffer[1] = (uint8_t) (page >> 5);
      send_buffer[2] = (uint8_t) (page << 3);
      send_buffer[3] = 0;

      if(!send_then_send(send_buffer, 4, write_buffer, 1024))
        return 0;

      write_buffer += 1024;
      page++;

      if(!wait_busy())
        return 0;
    }

    return size;
}
/*----------------------------------------------------------------------------*/
int sflash_read_id(int32_t *dst)
{
  uint8_t cmd;
  uint8_t recv[6];
  uint32_t id = 0;
  int i;

  cmd = AT45DB_CMD_JEDEC_ID;
  if(!send_then_recv(&cmd, 1, recv, 6))
    return 0;

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
/*****************************************************************************/
/*Status Register Format:                                   */
/* ------------------------------------------------------------------------- */
/* | bit7   | bit6   | bit5   | bit4   | bit3   | bit2   | bit1   | bit0   | */
/* |--------|--------|--------|--------|--------|--------|--------|--------| */
/* |RDY/BUSY| COMP   |         device density            |   X    |   X    | */
/* ------------------------------------------------------------------------- */
/* 0:busy   |        |        AT45DB041:0111             | protect|page size */
/* 1:ready  |        |        AT45DB161:1011             |                   */
/* --------------------------------------------------------------------------*/
/*****************************************************************************/
int sflash_status(uint8_t *dst)
{
  uint8_t cmd;

  cmd = AT45DB_READ_STATE_REGISTER;
  if(!send_then_recv(&cmd, 1, dst, 1))
    return 0;
  return 1;
}
/*----------------------------------------------------------------------------*/
/**Write 256 to flash. Parameter Page - flash page 0...4096*/
int sflash_page_write(unsigned page, const void *src)
{
  return sflash_page_writer(page, src, 1);
}
/*----------------------------------------------------------------------------*/
/**Read 256 bytes from flash. Parameter Page - flash page 0...4096*/
int sflash_page_read(unsigned page, void *dst)
{
  return sflash_page_reader(page, dst, 1);
}
/*----------------------------------------------------------------------------*/
/**Open flash for read or write*/
int sflash_open(int mode)
{
  flash_mode = mode & (FLASH_READ | FLASH_WRITE);
  sflash_offset = 0xffffffff;
  sflash_seek(0, SEEK_SET);
  return 1;
}
/*----------------------------------------------------------------------------*/
static unsigned page_mask()
{
  return bytes_per_page == 1024 ? 0x3ff : (bytes_per_page == 512 ? 0x1ff : 0xff);
}
/*----------------------------------------------------------------------------*/
static unsigned addr_page(unsigned addr)
{
  switch(bytes_per_page)
  {
  case 1024:
    return addr >> 10;
  case 512:
    return addr >> 9;
  }
  return addr >> 8;
}
/*----------------------------------------------------------------------------*/
#define valid_page(p) ((p) >= 0 && (p) < page_count)
/*----------------------------------------------------------------------------*/
/**Close flash*/
void sflash_close()
{
  if((flash_mode & FLASH_WRITE) && (flash_mode & FLASH_MODIFIED))
  {
    sflash_page_writer(addr_page(sflash_offset), sflash_buffer, 1);
  }
  flash_mode = 0;
}
/*----------------------------------------------------------------------------*/
static int change_page(off_t old_page, off_t new_page)
{
  //FLASH_TRACE("Change_page(%04x, %04x)\r\n", old_page, new_page);
  if(old_page == new_page)
    return 1;

  if(valid_page(old_page) && (flash_mode & FLASH_WRITE) && (flash_mode & FLASH_MODIFIED) && !sflash_page_writer(old_page, sflash_buffer, 1))
    return 0;
  flash_mode &= ~FLASH_MODIFIED;
  if(valid_page(new_page) && (flash_mode & FLASH_READ) && !sflash_page_reader(new_page, sflash_buffer, 1))
    return 0;
  if(!(flash_mode & FLASH_READ))
    memset(sflash_buffer, 0xff, bytes_per_page);
  return 1;
}
/*----------------------------------------------------------------------------*/
off_t sflash_seek(off_t offset, int whence)
{
  off_t addr;
  off_t flash_size;

  flash_size = page_count * bytes_per_page;

  addr = offset;
  switch(whence)
  {
  case SEEK_CUR:
    addr = sflash_offset + offset;
    break;
  case SEEK_END:
    addr = flash_size + offset;
    break;
  }

  if(addr >= 0 && addr < flash_size)
  {
    off_t page = addr_page(addr);
    if(!change_page(addr_page(sflash_offset), page))
      return -1;
    sflash_offset = addr;
  }
  return sflash_tell();
}
/*----------------------------------------------------------------------------*/
off_t sflash_tell()
{
  return sflash_offset;
}
/*----------------------------------------------------------------------------*/
/**Write flash*/
int sflash_write(const void *src, unsigned size)
{
  unsigned i;
  const uint8_t *p;
  off_t flash_size;

  flash_size = page_count * bytes_per_page;
  if(!(flash_mode & FLASH_WRITE))
    return -1;
  p = (const uint8_t *) src;
  for(i = 0; i < size && sflash_offset < flash_size; i++)
  {
    off_t offset;
    offset = sflash_offset & page_mask();
    if(sflash_buffer[offset] != *p || !(flash_mode & FLASH_READ))
    {
      sflash_buffer[offset] = *p;
      flash_mode |= FLASH_MODIFIED;
    }

    offset ++;
    p ++;

    if(offset >= bytes_per_page)
    {
      off_t page;
      page = addr_page(sflash_offset);
      if(!change_page(page, page+1))
        return i;
    }
    sflash_offset ++;
  }
  return i;
}
/*----------------------------------------------------------------------------*/
/**Read from flash*/
int sflash_read(void *dst, unsigned size)
{
  unsigned i;
  uint8_t *p;
  off_t flash_size;

  flash_size = page_count * bytes_per_page;

  if(!(flash_mode & FLASH_READ))
    return -1;

  p = (uint8_t *) dst;
  for(i = 0; i < size && sflash_offset < flash_size; i++)
  {
    off_t offset;
    offset = sflash_offset & page_mask();
    *p = sflash_buffer[offset];
    offset ++;
    p ++;

    if(offset >= bytes_per_page)
    {
      off_t page;
      page = addr_page(sflash_offset);
      if(!change_page(page, page+1))
        return i;
    }
    sflash_offset ++;
  }
  return i;
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
  uint8_t status;

  (void)argc;
  (void)argv; //Prevent unused warnings

  if(!sflash_status(&status))
  {
    console_printf("\r\n%s\r\n", "SPI timeout error");
  }
  else
  {
    console_printf("\r\nStatus:%02xh\r\n", (unsigned)status & 0xff);
  }

  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int flash_cmd3(int argc, char **argv)
{
  off_t start = 0;
  size_t size = 256, i;

  if(argc > 1)
    start = str2int(argv[1]);
  if(argc > 2)
    size = str2int(argv[2]);

  sflash_open(O_RDONLY);
  sflash_seek(start, SEEK_SET);

  for(i = 0; i < size; i += 16)
  {
    uint8_t buf[16];
    unsigned tm;
    int j;

    while(uart_tx_size())
    {
    }

    tm = sys_tick_count();
    if(sflash_read(buf, 16) != 16)
    {
      console_printf("\r\nError read flash");
      break;
    }
    tm = sys_tick_count() - tm;
    if(!i)
      console_printf("\r\n");

    console_printf("%06x ", start + i);
    for(j = 0; j < 16; j++)
      console_printf("%02x ", (unsigned)buf[j] & 0xff);

    for(j = 0; j < 16; j++)
      console_printf("%c", (buf[j] >= 0x20 && buf[j] < 0x80) ? buf[j] : '.');
    console_printf(" (%ums)\r\n", tm);
  }
  sflash_close();

  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int flash_cmd4(int argc, char **argv)
{
  off_t start = 0;
  size_t size = 256;
  uint8_t value = 0;
  uint8_t buf[16];
  unsigned tm;

  if(argc > 1)
    start = str2int(argv[1]);
  if(argc > 2)
    value = str2int(argv[2]);
  if(argc > 3)
    size = str2int(argv[3]);

  memset(buf, value, 16);
  sflash_open(O_WRONLY);
  tm = sys_tick_count();
  sflash_seek(start, SEEK_SET);
  while(size)
  {
    unsigned sz = 16;
    if(sz > size)
      sz = size;
    if(sflash_write(buf, sz) != (int)sz)
    {
      console_printf("\r\nError write flash");
      break;
    }
    size -= sz;
  }
  sflash_close();
  tm = sys_tick_count() - tm;
  console_printf("\r\nTime elapsed:%ums", tm);

  return CONSOLE_OK;
}
#endif
/*----------------------------------------------------------------------------*/
