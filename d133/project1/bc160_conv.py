#!/usr/bin/env python
# -*- coding: utf-8 -*-
## Convertor form utf-8 cyrrilic dda_text file
## to BC160 LCD cyrillic encoding

import re, sys

escape_dict = {'"' : '\\"', "'" : "\\'", '\\':'\\\\', '\r' : '\\r', '\n' : '\\n' }
escape_re = re.compile(r'[\"\'\\\r\n]')
#----------------------------------------------------------------------------------------------------#
def escape(src):
  return escape_re.sub(lambda mo: escape_dict[mo.string[mo.start():mo.end()]], src)
#----------------------------------------------------------------------------------------------------#
#----------------------------------------------------------------------------------------
string_re = re.compile(r'(["]([^"])*["])')
#----------------------------------------------------------------------------------------
def extract(filename):
  src = open(filename, 'rt')
  print("/*\nDO NOT EDIT MANUALLY THIS FILE.\nThis file was generated form {0}\n*/".format(filename))
  src_str = src.readline()
  while src_str:
    pos = 0
    r = string_re.search(src_str, pos)
    sharp = src_str.find('#')
    if sharp >= 0:
      if r and sharp < r.start():
         r = None
    #print str, r
    result = ''
    while r:
      #print(r.string[r.start():r.end()])
      f = r.string[r.start():r.end()]
      result = unicode(f[1:-1], 'utf8', 'ignored')
      encoded = bc160_encode(result)
      if encoded != result:
        src_str = src_str[0:r.start()] + '"{0}"/*{1}*/'.format(encoded.encode('utf8', 'ignored'), result.encode('utf8', 'ignored')) + src_str[r.end():]

      pos = r.end()
      r = string_re.search(src_str, pos)


    print src_str,
    src_str = src.readline()
  src.close()
#----------------------------------------------------------------------------------------
def u(src):
  return unicode(src, 'utf8', 'ignored')
#----------------------------------------------------------------------------------------
table = {
u("А") : ord('A'),
u("Б") : 0xa0,
u("В") : ord('B'),
u("Г") : 0xa1,
u("Д") : 0xe0,
u("Е") : ord('E'),
u("Ж") : 0xa3,
u("З") : 0xa4,
u("И") : 0xa5,
u("Й") : 0xa6,
u("К") : ord('K'),
u("Л") : 0xa7,
u("М") : ord('M'),
u("Н") : ord('H'),
u("О") : ord('O'),
u("П") : 0xa8,
u("Р") : ord('P'),
u("С") : ord('C'),
u("Т") : ord('T'),
u("У") : 0xa9,
u("Ф") : 0xaa,
u("Х") : ord('X'),
u("Ц") : 0xe1,
u("Ч") : 0xab,
u("Ш") : 0xac,
u("Щ") : 0xe2,
u("Ъ") : 0xad,
u("Ы") : 0xae,
u("Ь") : 0xc4, ##
u("Э") : 0xaf,
u("Ю") : 0xb0,
u("Я") : 0xb1,
u("а") : ord('a'),
u("б") : 0xb2,
u("в") : 0xb3,
u("г") : 0xb4,
u("д") : 0xe3,
u("е") : ord('e'),
u("ж") : 0xb6,
u("з") : 0xb7,
u("и") : 0xb8,
u("й") : 0xb9,
u("к") : 0xba,
u("л") : 0xbb,
u("м") : 0xbc,
u("н") : 0xbd,
u("о") : ord('o'),
u("п") : 0xbe,
u("р") : ord('p'),
u("с") : ord('c'),
u("т") : 0xbf,
u("у") : ord('y'),
u("ф") : 0xe4,
u("х") : ord('x'),
u("ц") : 0xe5,
u("ч") : 0xc0,
u("ш") : 0xc1,
u("щ") : 0xe6,
u("ъ") : 0xc2,
u("ы") : 0xc3,
u("ь") : 0xc4,
u("э") : 0xc5,
u("ю") : 0xc6,
u("я") : 0xc7,
u("І") : ord('I'),
u("і") : ord('i')
}
#----------------------------------------------------------------------------------------
def bc160_encode(src):
  dst = ''
#  for ch in src.decode('utf-8'):
  for ch in src:
    c = table.get(ch, ord(ch))
    if c >= 0x20 and c < 0x80:
      dst += chr(c)
    else:
      dst += "\\x{0:02x}".format(c)
  return dst
#----------------------------------------------------------------------------------------

if __name__ == '__main__':
  #print table
  extract(sys.argv[1])
