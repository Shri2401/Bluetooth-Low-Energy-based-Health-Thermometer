/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : ble.h
 *
 *
 */


#ifndef __SRC_BLE_H_
#define __SRC_BLE_H_

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>
#include <stdbool.h>

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "sl_bt_api.h"
#include "sl_bgapi.h"
#include "sl_component_catalog.h"
#include "sl_status.h"
#include "gatt_db.h"

/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "autogen/gatt_db.h"

/***************************************************** MACROS ********************************************************/
#define UINT8_TO_BITSTREAM(p, n)  { *(p)++ = (uint8_t)(n); }

#define UINT32_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                    *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }

#define UINT32_TO_FLOAT(m, e)   (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24));

/*
 * Data Structure to hold all the BlueTooth related events
 *
 */
typedef struct{
  bd_addr serverAddress;
  bd_addr clientAddress;

  bool bme280IndicationEnable;

  bool connectionOpened;
  bool isReadyToBond;
  uint8_t serverAddressType;
  uint8_t clientAddressType;

  uint8_t advertisingSetHandle;
  uint8_t connectionHandle;
  int32_t temp_value;
  uint8_t discoveredCharacteristicHandle;
  uint32_t discoveredServiceHandle;
  bool inFlight;
  bool gattProcedureComplete;
  uint8_t bondingHandle;
  bool isBonded;
  uint32_t passKey;
  uint32_t extSignal;
  bool heartRateIndicationEnabled;
  uint8_t clientButtonState;
  bool clientButtonChar;
  bool readHeartValue;
} ble_data_struct_t;

/*
 * Function to return the pointer to BlueTooth data structure which holds all the events
 * Parameters  : void
 * Return Type : ble_data_struct_t*  => Pointer to local data structure
 *
 */
ble_data_struct_t* getBleEvent();

/*
 * Function to send the temperature measurement to the BlueTooth mobile application
 * Parameter   : uint32_t => 32 bit temperature value in integer
 * Return type : void  => Function returns nothing
 *
 */
void indicate_temperature(uint32_t temp_in);

/*
 * Function to handle all the BlueTooth related events like => connection_open, connection_close, system boot id, connection_parameters, external_signal, characteristics status, timeout
 * Parameter   : sl_bt_msg_t* => Pointer to a Data Structure that holds the current events from the BlueTooth stack
 * Return type : void
 *
 */
void handle_ble_event(sl_bt_msg_t * evt);

void writeIntoAttribute(uint8_t temp_eight);
void getTemperatureValue();

#endif // __SRC_BLE_H_
