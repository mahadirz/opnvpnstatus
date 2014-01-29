# OpnVPNStatus BETA v29012014

##Introduction

This simple c program would output the OpenVPN 
connected users to http web page as JSON format. 

This is Beta version, currently I have no idea how to
make this program as auto start service. Need to read
more about bash scripting.

## Install


1. git clone https://github.com/mahadirz/opnvpnstatus
2. cd opnvpnstatus

3. Edit in header.h const char path[] = "/etc/openvpn/"; 
   and const int port = 8081; to installed openvpn
   and with unused port number (you may leave this as 
   default).
   
4. make
5. chmod +x opnvpnstatus
6. cp opnvpnstatus /usr/local/bin/
7. To run in daemon mode use: opnvpnstatus
   debug mode: opnvpnstatus -debug


## License

The MIT License (MIT)

Copyright 2014 Mahadir Lab

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