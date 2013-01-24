/**
 * @pkg api_CRC
 */
/**
 * CRC generation functions.
 *
 * CRC generation functions.
 * (C) T&T, Kiev, Ukraine 2004.
 * @pkgdoc api_CRC
 * @author Oleg Yurchenko
 * @version $Revision$
 */
#ifndef CRC8_H
#define CRC8_H

#ifdef __cplusplus
extern "C"
{
#endif  /*__cplusplus*/


  /**Calculate  CRC8 of single entry*/
  unsigned char calc_crc_8( unsigned char crc, unsigned char ch);
  /**Calculate  CRC8 of buffer*/
  unsigned char get_crc_8( unsigned char crc, const void *buf, int size );


#ifdef __cplusplus
};
#endif  /*__cplusplus*/

#endif /*CRC8_H*/
