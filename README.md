# cps2png

This is a tool to convert CPS files found in games published by KID and 5pb. (now MAGES. GAME) into PNG format. It supports over 10% more CPS files than name-brand open-source alternatives.

## CPS versions
### 100 (`64 00`)
| Game                                     | Platform    |
| ---------------------------------------- | ----------- |
| Memories Off (2000, KID)                 | Windows     |

This version of the format has a 12-byte header, while later versions have a 16-byte header. cps2png works with the CPS files found in *Memories Off for Windows*. There may be trailing transparency or black pixels at the end of some images (this won't occur for images with no transparency, or if you use the "no-alpha" builds), or odd colours in areas of images that don't appear in-game.
### 102 (`66 00`)
| Game                                     | Platform    |
| ---------------------------------------- | ----------- |
| Never7 -the end of infinity- (2001, KID) | Windows     |
| Tentama 1st Sunny Side (2002, KID)       | Windows     |
| Memories Off 2nd (2001/2002, KID)        | Windows     |
| Ever17 -the out of infinity- (2003, KID) | Windows     |

cps2png works with all CPS files found in these releases, and the uncompressed files produced by arysm's [Ever17 CPS Converter](https://github.com/arsym-dev/Ever17-CPS-Converter).
### 104 (`68 00`)
| Game                                     | Platform    |
| ---------------------------------------- | ----------- |
| Kanokon: Esuii (2002, 5pb.)              | PS2         |

These don't work yet.
## Usage

```./cps2png <file.cps> <file.png>```

Not safe with untrusted input. I'll make it endian-safe if you can show me a single big-endian device you actually want to run it on :pichuisAJ4:

## Build instructions

Adapt these steps as needed:

Download libspng from https://libspng.org/download/ and extract spng.c and spng.h into the cps2png folder. Download miniz from https://github.com/richgel999/miniz/releases and extract miniz.c and miniz.h into the cps2png folder. Run `gcc -O3 -I./ -D SPNG_USE_MINIZ -D SPNG_STATIC -std=c99 cps2png.c spng.c miniz.c -lm -o cps2png`.

## License notices
cps2png is based in part on code from [eternal-legacy](https://github.com/malucard/eternal-legacy) by malucart.
```zlib License

© 2022 malucart

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
```
cps2png uses the [libspng](https://libspng.org/) library.
```BSD 2-Clause License

Copyright (c) 2018-2023, Randy <randy408@protonmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

Provided binaries build libspng with [miniz](https://github.com/richgel999/miniz).
```Copyright 2013-2014 RAD Game Tools and Valve Software
Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC

All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```

Provided Linux binaries may be statically linked with [musl libc](https://musl.libc.org/).
 ```Copyright © 2005-2020 Rich Felker, et al.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
