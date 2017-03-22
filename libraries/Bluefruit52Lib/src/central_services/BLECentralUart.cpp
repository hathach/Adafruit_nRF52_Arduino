/**************************************************************************/
/*!
    @file     BLECentralUart.cpp
    @author   hathach

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2017, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "bluefruit.h"

BLECentralUart::BLECentralUart(uint16_t fifo_depth)
  : BLECentralService(BLEUART_UUID_SERVICE), _txd(), _rxd(), _rxd_fifo(fifo_depth, 1)
{

}

err_t BLECentralUart::begin(void)
{
  // Add UUID128 if needed
  uuid.begin();

  return ERROR_NONE;
}

bool BLECentralUart::discover(uint16_t start_handle)
{
  // Call BLECentralService discover
  VERIFY( BLECentralService::discover(start_handle) );

  // Discover TXD, RXD characteristics
  BLECentralCharacteristic chr_temp;
  while ( Bluefruit.Central.discoverCharacteristic(chr_temp) )
  {
    uint16_t uuid16;
    chr_temp.uuid.get(&uuid16);

    if (uuid16 == 0x0002)
    {
      LOG_LV1(BLECentralUart, "Found RXD");
      _rxd = chr_temp;
    }else if (uuid16 == 0x0003)
    {
      LOG_LV1(BLECentralUart, "Found TXD");
      _txd = chr_temp;

      // TXD has CCCD, discovery it
      ble_gattc_desc_t desc_arr[8];
      Bluefruit.Central.discoverDescriptor(desc_arr, arrcount(desc_arr));
    }
  }

  // true if both RXD & TXD are found
  return ( _rxd.valueHandle() && _txd.valueHandle() );
}