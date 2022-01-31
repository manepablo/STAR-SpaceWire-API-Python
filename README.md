# STAR-SpW-API-Python
This is a C wrapper class for the Space Wire API C functions provided by STAR-Dundee written in Python. It allows to send and receive bytearrays via Space Wire Protocoll. Using a MK4 Brick. At this time the code is only tested for linux systems.

## Prerequisites
For Operating the Software the Star Spacewire Software is required. [STAR-System](https://www.star-dundee.com/wp-content/star_uploads/2013/03/products-STAR-System.pdf) is the API and driver system provided with STAR-Dundee. This is because the STAR Space Wire Brick needs certains USB drivers. Also the written Python software relies on the STAR API which is written in C. The software was Build and tested with the newest version (version 5)

## Setup
Connect the Space Wire Brick to the computer via USB. start the STAR "receive" application and check if there is a available device. If the Brick is propperly connected an no device can be found you must run the `build-star-system.sh` manually. retry step one afterwards. Now Import The Package with and load the object. Make sure to provide the correct link to the STAR-System:  

```Python
from starapi import starapi
path = "/usr/local/STAR-Dundee/STAR-System/lib/x86-64"
star = starapi(path)
```

Check how many devicec can be found:
```Python
print(star.list_devices())
```

Open a channel and send a message: 
```Python
star.open_channel(1)
star.send_msg(byte_msg= b'\xbe\xef\x12\x34', channel = 1)
```

Receive a message on channel two in case you connected ch1 with ch2.:
```Python
star.open_channel(2)
print(star.rec_msg(channel = 2))
```

Close the channels:
```Python
star.close_channel(1)
star.close_channel(2))
```

