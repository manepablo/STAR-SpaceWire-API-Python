#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jan  5 17:16:11 2022

This programm has the purpose of wrapping some star dundee funtion into simple python code
For This STAR-System > V5 needs to be installed

@author: manea
von Hoerner&Sulger GmbH
"""
from cffi import FFI
import subprocess
import os
import sys
ffi = FFI()



class starapi():
    def __init__(self, libpath, verb = False):
        """
        Constructor of star_api class
        
        Parameters
        ----------
        libpath : str
            path to star api folder where one can find the libstar-api.so.
            example: /usr/local/STAR-Dundee/STAR-System/lib/x86-64
        verb : bool, optional
            if True some messages are printed, which may slow down operation.
            The default is False.

        """
        if sys.platform == "linux":
            self.api = ffi.dlopen(libpath + "/libstar-api.so")
        elif sys.platform == "windows":
            #TODO test this 
            self.api = ffi.dlopen(libpath + "/libstar-api.dll")  
        else:
            raise Exception("platform '" + sys.platform + "' not supported")
        self._cdefines()
        self.opened_channels = {}
        self.apilib = libpath
        self.verb = verb
        out =  self.api.STAR_getApiVersion()
        print("using " + (ffi.string(out.name).decode("utf-8")) + " Version: " + str(out.major) + "." +str(out.minor))
        
        #self._build_star_system()
    def open_channel(self, channel, device=1):
        """
        This function opens a channel to a selected device and port. If only
        one device is plugged in, it will be found as device 1 by default.
        It is possible to open multiple channels on one deice.
        
        When opening a channel the new channel is stored in self.opened_channels
        
        NOTE: it hasnt been tested with multiple devices

        Parameters
        ----------
        channel : int
            Channel Number.
        device : int, optional
            DESCRIPTION. The default is 1.

        Raises
        ------
        Exception
            DESCRIPTION.
        ValueError
            DESCRIPTION.

        Returns
        -------
        selectedChannel : TYPE
            DESCRIPTION.

        """
        self.list_devices()
        if self.devices == ffi.NULL:
            if self.verb: print("No devices found")
            if self.verb: print("trying to build STAR system again")
            #self._build_star_system()
            self.list_devices()
            if self.devices == ffi.NULL:
                raise Exception("No devices found")            
        if channel > self.api.STAR_getDeviceChannels(self.devices[device-1]):
            raise ValueError(ffi.string((self.api.STAR_getDeviceName(self.devices[device-1]))).decode("utf-8") + " has only " + str(self.api.STAR_getDeviceChannels(self.devices[device-1])) + " channels")
        if str(channel) in self.opened_channels.keys():
            raise Exception("channel " + str(channel) + " already opened")
        selectedChannel = self.api.STAR_openChannelToLocalDevice(self.devices[device-1], self.api.STAR_CHANNEL_DIRECTION_INOUT, channel, 1)
        self.opened_channels[str(channel)]=selectedChannel
        return selectedChannel
    def close_channel(self, channel,device=1):
        """
        This function closes a channel to a device. Thus it will delete the entry from 
        self.opened_channels. 

        Parameters
        ----------
        channel : int
            indicator of which channel is to be closed.
        device : int, optional
            indicator of which device is to be selected. The default is 1.

        Raises
        ------
        Exception
            If the channel which is to be closed cannot be found in self.opened_channels.

        Returns
        -------
        None.

        """
        
        if str(channel) not in self.opened_channels.keys():
            raise Exception("channel " + str(channel) + " not opened")
        self.api.STAR_closeChannel(self.opened_channels[str(channel)])  
        del self.opened_channels[str(channel)]
    def send_msg(self,byte_msg, channel):
        """
        sand any message ofer over SpaceWire (SpW)

        Parameters
        ----------
        byte_msg : bytes
            message, which should be send via SpW. 
        channel : int
            channel over which the message should be send.

        Raises
        ------
        Exception
            channel not opened.

        Returns
        -------
        status : int 
            0: STAR_TRANSFER_STATUS_NOT_STARTED
            1: STAR_TRANSFER_STATUS_STARTED
            2: STAR_TRANSFER_STATUS_COMPLETE 
            3. STAR_TRANSFER_STATUS_CANCELLED
            4: STAR_TRANSFER_STATUS_ERROR

        """
        if str(channel) not in self.opened_channels.keys():
            raise Exception("channel " + str(channel) + " not opened")
        #create a message object 
        packet = self.api.STAR_createPacket(ffi.new("STAR_SPACEWIRE_ADDRESS*"), byte_msg, len(byte_msg), self.api.STAR_EOP_TYPE_EOP);
        # now add the packet to a pointer array 
        all_packets = ffi.new("STAR_STREAM_ITEM **")
        all_packets[0] = packet
        #create transfer operation object
        transferOperation = self.api.STAR_createTxOperation(all_packets, 1);
        out = self.api.STAR_submitTransferOperation(self.opened_channels[str(channel)],transferOperation)
        status = self.api.STAR_waitOnTransferOperationCompletion(transferOperation, 100)
        self.api.STAR_disposeTransferOperation(transferOperation)
        return status
    
    def rec_msg(self, channel):
        """
        This funtion receves a Message. If there is no new message to be received it will
        reutturn None

        Parameters
        ----------
        channel : in
            channel over which the message should be received.

        Raises
        ------
        Exception
            channel not opened..

        Returns
        -------
        bytes if there is a new message. Else None.

        """
        if str(channel) not in self.opened_channels.keys():
            raise Exception("channel " + str(channel) + " not opened")
        #create a receive operation
        receiveOperation = self.api.STAR_createRxOperation(1, self.api.STAR_RECEIVE_PACKETS)
        self.api.STAR_submitTransferOperation(self.opened_channels[str(channel)],receiveOperation)
        status = self.api.STAR_waitOnTransferOperationCompletion(receiveOperation, 100)     
        if status == self.api.STAR_TRANSFER_STATUS_COMPLETE:
            num_packets = self.api.STAR_getTransferItemCount(receiveOperation)
            rec_packets = self.api.STAR_getTransferItem(receiveOperation, 0)
            
            datalenght = ffi.new("unsigned int *")
            rec_msg_char = self.api.STAR_getPacketData(rec_packets.item, datalenght)
            ret_val = []
            for i in range(0,datalenght[0]):
                ret_val.append(rec_msg_char[i])
                
            del [rec_packets, rec_msg_char]
        elif status == self.api.STAR_TRANSFER_STATUS_STARTED:
            if self.verb: print("RECEIVER: no new message")
            ret_val = None
        else:
            if self.verb: print("RECEIVER: an error has occured")
            ret_val = None
        self.api.STAR_disposeTransferOperation(receiveOperation)
        del [receiveOperation]
        try: return bytes(ret_val)
        except: return None
    def list_devices(self):
        """
        returns a list of devices. and stores the device 

        Returns
        -------
        TYPE
            DESCRIPTION.
        TYPE
            DESCRIPTION.

        """
        #get list of connected devices
        num_prt =  ffi.new("U32 *")
        self.devices = self.api.STAR_getDeviceList(num_prt)
        self.found_devices_serialnumber = []
        for i in range(0,num_prt[0]):
            self.found_devices_serialnumber.append(ffi.string((self.api.STAR_getDeviceName(self.devices[i]))).decode("utf-8"))
        return self.found_devices_serialnumber
    def _cdefines(self):
        """
        calls the custom define file.
        TODO:   find a way to load the provided header files automaticalle ccfi does not seem to support this.
                baybe parse the files and extract the deffinitions. Problm order of deffinitions

        Returns
        -------
        None.

        """
        #constants
        ffi.cdef(open(os.path.dirname(os.path.abspath(__file__))+"/defines.h","r").read(), override=True)
    def build_star_system(self):
        """
        This function calls the build script of STAR system (usb-drivers ...)
        Returns
        -------
        None.

        """        
        subprocess.call("sudo "+ os.path.abspath(os.path.split(self.apilib)[0]) + "/../build_star-system.sh")
        
        


