/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : ble.c
 *
 *
 */


/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "ble.h"
#include "ble_device_type.h"
#include "lcd.h"
#include "app.h"
#include "gpio.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"
#include <math.h>

/***************************************************** MACROS ********************************************************/
#define BLE_BUFFER_SIZE           5
#define TEMPERATURE_DATA_SIZE     4
#define TEMPERATURE_INT_TO_FLOAT  1000
#define ADVERTISING_MIN           400
#define ADVERTISING_MAX           400
#define CONNECTION_MIN            60
#define CONNECTION_MAX            60
#define CONNECTION_LATENCY        3
#define CONNECTION_TIMEOUT        900
#define CLIENT_PHY_BW             1
#define CLIENT_SCAN_MODE          0
#define CLIENT_SCAN_INTERVAL      80
#define CLIENT_SCAN_RANGE         40
#define CLIENT_MIN_INTERVAL       60
#define CLIENT_MAX_INTERVAL       60
#define CLIENT_LATENCY            3
#define CLIENT_TIMEOUT            750
#define CLIENT_MIN_CE_LENGTH      0
#define CLIENT_MAX_CE_LENGTH      4
#define QUEUE_DEPTH               16
#define USE_ALL_ENTRIES           1
#define SECURITY_REQUIREMENTS     0x0F
#define PASSKEY_ERROR             0x110F
#define THRESHOLD                 30
#define HEART_THRESHOLD           120



/*
 * Global variable to store all the BLE data
 */
ble_data_struct_t ble_data;

/*
 * Global variable to store the return status of all BLE functions
 */
sl_status_t returnStatus;
#if (!DEVICE_IS_BLE_SERVER)
static int32_t FLOAT_TO_INT32(const uint8_t *value_start_little_endian)
{
  uint8_t signByte = 0;
  int32_t mantissa;

  int8_t exponent = (int8_t)value_start_little_endian[4];

  if(value_start_little_endian[3] & 0x80){
      signByte = 0xFF;
  }

  mantissa = (int32_t) (value_start_little_endian[1] << 0) |
                      (value_start_little_endian[2] << 8) |
                      (value_start_little_endian[3] << 16) |
                      (signByte << 24);

  //logic -> value = 10^exponent*mantissa
  return (int32_t) (pow(10,exponent)*mantissa);

}
#endif
ble_data_struct_t* getBleEvent(){
  return (&ble_data);
}
bd_addr server_address = SERVER_BT_ADDRESS;
#if (DEVICE_IS_BLE_SERVER)
void writeIntoAttribute(uint8_t temp_eight){
  uint8_t value = temp_eight;
  returnStatus = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_value, 0, 1, &value );
  if (returnStatus != SL_STATUS_OK){
      LOG_ERROR(" GATT serevr Write Attribute failed = 0x%04x\n\r", (uint32_t)returnStatus);
  }
  else{
      LOG_INFO("TEMP Data Written to Database is %u \n\r", value);
  }
}
void writeHeartRateIntoAttribute(uint8_t heartRate){
  uint8_t value = heartRate;
  returnStatus = sl_bt_gatt_server_write_attribute_value(gattdb_heart_rate_value, 0, 1, &value );
  if (returnStatus != SL_STATUS_OK){
    LOG_ERROR(" GATT serevr HEART RATE Write Attribute failed = 0x%04x\n\r", (uint32_t)returnStatus);
  }
  else{
    LOG_INFO("HEART Data Written to Database is %u \n\r", value);
  }
}
#else
void getTemperatureValue(){
  returnStatus = sl_bt_gatt_read_characteristic_value(ble_data.connectionHandle, gattdb_temperature_value);
  if(returnStatus != SL_STATUS_OK){
    LOG_ERROR("TEMP read characteristic value returned with error code = 0x%04x\n\r", (uint32_t)returnStatus);
  }
}
#endif

void handle_ble_event(sl_bt_msg_t * evt){
#if (DEVICE_IS_BLE_SERVER)
  switch (SL_BT_MSG_ID(evt->header)){
    case sl_bt_evt_system_boot_id:
      {
        ble_data.bme280IndicationEnable = false;                                   // Disable HTM Indication //
        ble_data.connectionOpened = false;                                      // Disable Connection open //
        ble_data.inFlight = false;                                              // Disable InFlight //
        ble_data.isBonded = false;
        ble_data.heartRateIndicationEnabled = false;
        ble_data.gattProcedureComplete = false;

        // Get ID //
        returnStatus = sl_bt_system_get_identity_address(&ble_data.serverAddress,&ble_data.serverAddressType);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("System Get Identity Address failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        // Create advertiser //
        returnStatus = sl_bt_advertiser_create_set(&ble_data.advertisingSetHandle);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Advertiser Create Set failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        // Set advertising timing //
        returnStatus = sl_bt_advertiser_set_timing(ble_data.advertisingSetHandle, ADVERTISING_MIN, ADVERTISING_MAX, 0, 0);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Set Timing failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        // Start advertising //
        returnStatus = sl_bt_advertiser_start(ble_data.advertisingSetHandle,sl_bt_advertiser_general_discoverable,sl_bt_advertiser_connectable_scannable);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Advertiser Start failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        returnStatus = sl_bt_sm_delete_bondings();
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Delete Bonding failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        returnStatus = sl_bt_sm_configure(SECURITY_REQUIREMENTS, sm_io_capability_displayyesno);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Bonding Configuration failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        displayInit();
        displayPrintf(DISPLAY_ROW_NAME, BLE_DEVICE_TYPE_STRING);
        displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x"
                                                    ,ble_data.serverAddress.addr[0]
                                                    ,ble_data.serverAddress.addr[1]
                                                    ,ble_data.serverAddress.addr[2]
                                                    ,ble_data.serverAddress.addr[3]
                                                    ,ble_data.serverAddress.addr[4]
                                                    ,ble_data.serverAddress.addr[5]);
        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        displayPrintf(DISPLAY_ROW_ASSIGNMENT, "Final Project");
      break;
      }

    case sl_bt_evt_connection_opened_id:
      {
        ble_data.connectionOpened = true;                                       // Enable Connection open //
        ble_data.connectionHandle = evt->data.evt_connection_opened.connection; // Get connection Handle //
        ble_data.bondingHandle = evt->data.evt_connection_opened.bonding;

        // Stop advertising //
        returnStatus = sl_bt_advertiser_stop(ble_data.advertisingSetHandle);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Advertiser Stop failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        // Do range check //
        // Set Connection parameters //
        returnStatus = sl_bt_connection_set_parameters(ble_data.connectionHandle, CONNECTION_MIN, CONNECTION_MAX, CONNECTION_LATENCY, CONNECTION_TIMEOUT, 0, 0xFFFF);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Connection Set Parameters failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }

        returnStatus = sl_bt_system_set_soft_timer(8192, 1, false);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("System Set soft timer failed with error = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        returnStatus = sl_bt_sm_delete_bondings();
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Delete Bonding failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
        displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      break;
      }
    case sl_bt_evt_connection_closed_id:
      {
        gpioLed0SetOff();
        gpioLed1SetOff();
        ble_data.connectionOpened = false;                                      // Disable Connection open //
        ble_data.bme280IndicationEnable = false;                                   // Disable Indication //
        ble_data.inFlight = false;
        ble_data.isBonded = false;
        ble_data.heartRateIndicationEnabled = false;
        // Start advertising again //
        returnStatus = sl_bt_advertiser_start(ble_data.advertisingSetHandle,sl_bt_advertiser_general_discoverable,sl_bt_advertiser_connectable_scannable);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Advertiser Start failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        returnStatus = sl_bt_sm_delete_bondings();
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("Delete Bonding failed = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
        displayPrintf(DISPLAY_ROW_PASSKEY, "");
        displayPrintf(DISPLAY_ROW_ACTION, "");
        displayPrintf(DISPLAY_ROW_9, "");
        displayPrintf(DISPLAY_ROW_BTADDR2, "");
      break;
      }
    case sl_bt_evt_connection_parameters_id:
      {
        // Print all the connection parameters to the terminal //
        LOG_INFO("CONNECTION PARAMETERS BY MASTER Connection:%d,\t Interval:%d,\t Latency:%d,\t Timeout:%d\n\r",
                     (int) (evt->data.evt_connection_parameters.connection),
                     (int) (evt->data.evt_connection_parameters.interval),
                     (int) (evt->data.evt_connection_parameters.latency),
                     (int) (evt->data.evt_connection_parameters.timeout));

      break;
      }
    case sl_bt_evt_sm_confirm_passkey_id:
      {
        ble_data.connectionHandle = evt->data.evt_sm_confirm_passkey.connection;
        ble_data.passKey = evt->data.evt_sm_confirm_passkey.passkey;
        displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
        displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey:%d", ble_data.passKey);
        displayPrintf(DISPLAY_ROW_9, "Confirm Key");
        break;
      }
    case sl_bt_evt_sm_confirm_bonding_id:
      {
        ble_data.connectionHandle = evt->data.evt_sm_confirm_bonding.connection;
        ble_data.bondingHandle = evt->data.evt_sm_confirm_bonding.bonding_handle;
        returnStatus = sl_bt_sm_bonding_confirm(ble_data.connectionHandle, 1);
        if (returnStatus != SL_STATUS_OK){
            LOG_ERROR("sl_bt_bonding_confirm failed with error = 0x%04x\n\r", (uint32_t)returnStatus);
        }
        break;
      }
    case sl_bt_evt_sm_bonded_id:
      {
        ble_data.connectionHandle = evt->data.evt_sm_bonded.connection;
        ble_data.bondingHandle = evt->data.evt_sm_bonded.bonding;
        ble_data.isBonded = true;
        displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
        displayPrintf(DISPLAY_ROW_PASSKEY, "");
        displayPrintf(DISPLAY_ROW_9, "");
        displayPrintf(DISPLAY_ROW_ACTION, "");

        break;
      }
    case sl_bt_evt_sm_bonding_failed_id:
      {
        LOG_ERROR("Bonding Closed\n\r");
        break;
      }
    case sl_bt_evt_gatt_server_characteristic_status_id:
      {
        if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_value){   // Temperature Measurement //
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config){   // Change in CCCD //
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication){        // Indication enabled //
                ble_data.bme280IndicationEnable = true;                                                             // Set indication //
                gpioLed0SetOn();
              }

              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable){           // Indication disabled //
                ble_data.bme280IndicationEnable = false;                                                            // Unset indication //
                gpioLed0SetOff();
                displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
              }
          }
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)     // Confirmation received //
            ble_data.inFlight = false;                                                                           // Disable inFLight //
        }
        if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_heart_rate_value){   // Temperature Measurement //
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config){   // Change in CCCD //
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication){        // Indication enabled //
                ble_data.heartRateIndicationEnabled = true;                                                             // Set indication //
                gpioLed1SetOn();
              }

              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable){           // Indication disabled //
                ble_data.heartRateIndicationEnabled = false;                                                            // Unset indication //
                gpioLed1SetOff();
                displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
              }
          }
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)     // Confirmation received //
            ble_data.inFlight = false;                                                                           // Disable inFLight //
        }
        break;
      }
    case sl_bt_evt_system_soft_timer_id:
      {
        if(evt->data.evt_system_soft_timer.handle==2)
          {
            displayUpdate();
          }
        break;
      }
    case sl_bt_evt_system_external_signal_id:
      {
        if(evt->data.evt_system_external_signal.extsignals & PB0_PRESS){
          if((!ble_data.isBonded)){
            returnStatus = sl_bt_sm_passkey_confirm(ble_data.connectionHandle, 1);
            if(returnStatus!=0){
              LOG_ERROR("sl_bt_sm_passkey_confirm returned error = 0x%04x\n", (uint32_t)returnStatus);
            }
          }
        }
      break;
      }
    case sl_bt_evt_gatt_server_indication_timeout_id:
      {
        //ble_data.inFlight = false;                                              // Disable inFlight //
        ble_data.bme280IndicationEnable = false;
        ble_data.heartRateIndicationEnabled = false;
        LOG_ERROR("Received Timeout from client, previous data transfer failed \n\r");
      break;
      }
  }
#else
       switch(SL_BT_MSG_ID(evt->header))
         {
           case sl_bt_evt_system_boot_id:
             {
               ble_data.bme280IndicationEnable = false;                                   // Disable HTM Indication //
               ble_data.connectionOpened = false;                                      // Disable Connection open //
               ble_data.inFlight = false;                                              // Disable InFlight //
               ble_data.isBonded = false;                                              // Disable Bonding flag //
               ble_data.heartRateIndicationEnabled = false;
               ble_data.clientButtonState = 0x00;
               ble_data.clientButtonChar = true;
               ble_data.isReadyToBond = false;
               ble_data.gattProcedureComplete = false;
               ble_data.readHeartValue = false;

             // Get the Client address //
             returnStatus= sl_bt_system_get_identity_address(&ble_data.clientAddress, &ble_data.clientAddressType);

             // Data structure Init //
             ble_data.serverAddress = server_address;

             // Scanner config //
             returnStatus = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, CLIENT_SCAN_MODE);
             if(returnStatus != SL_STATUS_OK){
                 LOG_ERROR("scanner set mode call returned error = 0x%04x\n\r", (uint32_t)returnStatus);
             }

             // Scanner timing config //
             returnStatus = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, CLIENT_SCAN_INTERVAL, CLIENT_SCAN_RANGE);
             if(returnStatus != SL_STATUS_OK){
                 LOG_ERROR("sl_bt_scanner_set_timing call returned error = 0x%04x\n\r", (uint32_t)returnStatus);
             }
             // Set default Parameters //
             returnStatus = sl_bt_connection_set_default_parameters(CLIENT_MIN_INTERVAL, CLIENT_MAX_INTERVAL, CLIENT_LATENCY, CLIENT_TIMEOUT, CLIENT_MIN_CE_LENGTH, CLIENT_MAX_CE_LENGTH);
             if(returnStatus != SL_STATUS_OK){
                 LOG_ERROR("sl_bt_connection_set_default_parameters returned error = 0x%04x\n\r", (uint32_t)returnStatus);
             }


             // Start Scanning //
             returnStatus = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
             if(returnStatus != SL_STATUS_OK){
                 LOG_ERROR("sl_bt_scanner_start returned 7878 error = 0x%04x\n\r", (uint32_t)returnStatus);
             }
             returnStatus = sl_bt_sm_delete_bondings();
             if (returnStatus != SL_STATUS_OK){
                 LOG_ERROR("Delete Bonding failed = 0x%04x\n\r", (uint32_t)returnStatus);
             }
             returnStatus = sl_bt_sm_configure(SECURITY_REQUIREMENTS, sm_io_capability_displayyesno);
             if (returnStatus != SL_STATUS_OK){
                 LOG_ERROR("Bonding Configuration failed = 0x%04x\n\r", (uint32_t)returnStatus);
             }
            // Initialize Display //
            displayInit();
            displayPrintf(DISPLAY_ROW_NAME, "Client");                          // Display Client //
            // Display Client address //
            displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x",
                          ble_data.clientAddress.addr[0],
                          ble_data.clientAddress.addr[1],
                          ble_data.clientAddress.addr[2],
                          ble_data.clientAddress.addr[3],
                          ble_data.clientAddress.addr[4],
                          ble_data.clientAddress.addr[5]
                          );
            displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
            displayPrintf(DISPLAY_ROW_ASSIGNMENT, "Final Project");

             break;
             }



           case sl_bt_evt_scanner_scan_report_id:
             {
             // Check server address for pairing //
             if(evt->data.evt_scanner_scan_report.address.addr[0] == ble_data.serverAddress.addr[0] &&
                 evt->data.evt_scanner_scan_report.address.addr[1] == ble_data.serverAddress.addr[1] &&
                 evt->data.evt_scanner_scan_report.address.addr[2] == ble_data.serverAddress.addr[2] &&
                 evt->data.evt_scanner_scan_report.address.addr[3] == ble_data.serverAddress.addr[3] &&
                 evt->data.evt_scanner_scan_report.address.addr[4] == ble_data.serverAddress.addr[4] &&
                 evt->data.evt_scanner_scan_report.address.addr[5] == ble_data.serverAddress.addr[5]){


                displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");             // Successful connection //
                // Display Server address //
                displayPrintf(DISPLAY_ROW_BTADDR2, "%x:%x:%x:%x:%x:%x",
                              ble_data.serverAddress.addr[0],
                              ble_data.serverAddress.addr[1],
                              ble_data.serverAddress.addr[2],
                              ble_data.serverAddress.addr[3],
                              ble_data.serverAddress.addr[4],
                              ble_data.serverAddress.addr[5]
                              );

                // Address matches //
                returnStatus = sl_bt_scanner_stop(); //stop scanning
                 if(returnStatus != SL_STATUS_OK){
                     LOG_ERROR("sl_bt_scanner_stop returned error = 0x%04x\n\r", (uint32_t)returnStatus);
                 }

                 ble_data.connectionHandle = 1;
                 // Open connection //
                 returnStatus = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                                      evt->data.evt_scanner_scan_report.address_type,
                                                     sl_bt_gap_phy_1m,
                                                     NULL);
                 if(returnStatus != SL_STATUS_OK){
                     LOG_ERROR("sl_bt_connection open returned error = 0x%04x\n\r", (uint32_t)returnStatus);
                 }

             }
             break;
             }

           case sl_bt_evt_connection_opened_id:
             {
               ble_data.connectionHandle = evt->data.evt_connection_opened.connection;
               ble_data.connectionOpened = true;
               displayPrintf(DISPLAY_ROW_CONNECTION, "Handling Indications");
               break;
             }

           case sl_bt_evt_gatt_procedure_completed_id:
             {
               LOG_INFO("PROCEDURE_COMPLETE\n\r");
               if(evt->data.evt_gatt_procedure_completed.result == PASSKEY_ERROR){
                 returnStatus = sl_bt_sm_increase_security(ble_data.connectionHandle);
                 if(returnStatus != 0){
                     LOG_ERROR("sl_bt_sm_increase_security returned error = 0x%04x\n\r", (uint32_t)returnStatus);
                 }

                 LOG_INFO("INSIDE_PASSKEY_ERROR\n\r");
               }
               else if (evt->data.evt_gatt_procedure_completed.result != 0){
                 LOG_ERROR("sl_bt_evt_gatt_procedure_completed_id returned error = 0x%04x\n\r", (uint32_t)returnStatus);
               }
             }
             break;

           case sl_bt_evt_gatt_service_id:
             {
             // Store service handle //
             ble_data.discoveredServiceHandle = evt->data.evt_gatt_service.service;
             LOG_INFO("Got service Handle %u \n\r", ble_data.discoveredServiceHandle);
             break;
             }

           case sl_bt_evt_gatt_characteristic_id:
             {
             // Store characteristics handle //
             ble_data.discoveredCharacteristicHandle = evt->data.evt_gatt_characteristic.characteristic;
             LOG_INFO("Got Charac Handle %u \n\r", ble_data.discoveredCharacteristicHandle);
             break;
             }

           case sl_bt_evt_gatt_characteristic_value_id:
             {
             // Send characteristic confirmation //
             if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_read_response){
                 uint8_t tempValue = evt->data.evt_gatt_characteristic_value.value.data[0];
                 if (tempValue >= THRESHOLD && (!ble_data.readHeartValue)){
                   displayPrintf(DISPLAY_ROW_8, "Temp Value %u", tempValue);
                   gpioLed0SetOn();
                   ble_data.readHeartValue = true;
                   returnStatus = sl_bt_gatt_read_characteristic_value(ble_data.connectionHandle, gattdb_heart_rate_value);
                   if(returnStatus != SL_STATUS_OK){
                     LOG_ERROR("HEART read characteristic value returned with error code = 0x%04x\n\r", (uint32_t)returnStatus);
                   }
                 }
                 else if(ble_data.readHeartValue){
                     displayPrintf(DISPLAY_ROW_9, "Heart Value %u", tempValue);
                     if (tempValue >= HEART_THRESHOLD){
                       gpioLed1SetOn();
                     }
                     else{
                       gpioLed1SetOff();
                     }
                     ble_data.readHeartValue = false;

                 }
                 else{
                     displayPrintf(DISPLAY_ROW_9, "");
                     displayPrintf(DISPLAY_ROW_8, "");
                     gpioLed1SetOff();
                     gpioLed0SetOff();
                 }
             }
             else{
               returnStatus = sl_bt_gatt_send_characteristic_confirmation(ble_data.connectionHandle);
               if(returnStatus != 0){
                 LOG_ERROR("sl_bt_send_confirmation returned error = 0x%04x\n\r", (uint32_t)returnStatus);
               }
               if (evt->data.evt_gatt_characteristic_value.characteristic == gattdb_heart_rate_value){
                 uint8_t buttonStatus = evt->data.evt_gatt_characteristic_value.value.data[1];
                 if (buttonStatus == 0){
                   displayPrintf(DISPLAY_ROW_9, "Button Released");
                 }
                 else if (buttonStatus == 1){
                   displayPrintf(DISPLAY_ROW_9, "Button Pressed");
                 }
               }
               else if (evt->data.evt_gatt_characteristic_value.characteristic == gattdb_heart_rate_value){
                 // Temperature conversion //
                 ble_data.temp_value = FLOAT_TO_INT32(&evt->data.evt_gatt_characteristic_value.value.data[0]);
                 // Display it in the LCD //
                 displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp : %d C", ble_data.temp_value);
               }
             }

             break;
             }
           case sl_bt_evt_system_soft_timer_id:
             {
               if(evt->data.evt_system_soft_timer.handle==2)
                 {
                   displayUpdate();
                 }
               break;
             }
           case sl_bt_evt_connection_closed_id:
             {
             ble_data.bme280IndicationEnable = false;                                   // Disable HTM Indication //
             ble_data.connectionOpened = false;                                      // Disable Connection open //
             ble_data.inFlight = false;
             ble_data.isBonded = false;
             ble_data.heartRateIndicationEnabled = false;
             ble_data.clientButtonState = false;
             ble_data.clientButtonState = 0x00;

             // Update LCD //
             displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
             displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
             displayPrintf(DISPLAY_ROW_BTADDR2,"");
             displayPrintf(DISPLAY_ROW_ACTION, "");
             displayPrintf(DISPLAY_ROW_PASSKEY, "");
             displayPrintf(DISPLAY_ROW_9, "");

             // Start Scanning again //
             returnStatus = sl_bt_scanner_start(CLIENT_PHY_BW, sl_bt_scanner_discover_generic);
             if(returnStatus != 0){
                 LOG_ERROR("sl_bt_scanner_start 6868 returned error = 0x%04x\n\r", (uint32_t)returnStatus);
             }
             returnStatus = sl_bt_sm_delete_bondings();
             if(returnStatus != 0){
               LOG_ERROR("sl_bt_sm_delete_bondings returned error = 0x%04x\n\r", (uint32_t)returnStatus);
             }
             break;
             }
           case sl_bt_evt_sm_bonded_id:
             {
               ble_data.connectionHandle = evt->data.evt_sm_bonded.connection;
               ble_data.bondingHandle = evt->data.evt_sm_bonded.bonding;
               ble_data.isBonded = true;
               displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
               displayPrintf(DISPLAY_ROW_PASSKEY, "");
               displayPrintf(DISPLAY_ROW_ACTION, "");
               displayPrintf(DISPLAY_ROW_9, "");

               break;
             }
           case sl_bt_evt_sm_confirm_passkey_id:
             {
               ble_data.connectionHandle = evt->data.evt_sm_confirm_passkey.connection;
               ble_data.passKey = evt->data.evt_sm_confirm_passkey.passkey;
               displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
               displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey:%d", ble_data.passKey);
               break;
             }
           case sl_bt_evt_system_external_signal_id:
             {
             if(evt->data.evt_system_external_signal.extsignals & PB1_PRESS){
               if ((!ble_data.isBonded)){
                 returnStatus = sl_bt_gatt_read_characteristic_value(ble_data.connectionHandle, gattdb_heart_rate_value);
                 if(returnStatus != SL_STATUS_OK){
                     LOG_ERROR("read characteristic value returned with error code = 0x%04x\n\r", (uint32_t)returnStatus);
                 }
                 else{
                     ble_data.isReadyToBond = true;
                 }
               }
             }

             if(evt->data.evt_system_external_signal.extsignals & PB0_PRESS){
               if((!ble_data.isBonded) && (ble_data.isReadyToBond)){
                   returnStatus = sl_bt_sm_passkey_confirm(ble_data.connectionHandle, 1);
                 if(returnStatus!=0){
                   LOG_ERROR("sl_bt_sm_passkey_confirm returned error = 0x%04x\n", (uint32_t)returnStatus);
                 }
               }
             }
             break;
             }
         }

#endif
}

#if (DEVICE_IS_BLE_SERVER)
void indicate_temperature(uint32_t temp_in){

  uint8_t htm_temperature_buffer[BLE_BUFFER_SIZE];                              // Buffer to store temperature reading //

  uint8_t flags = 0x00;                                                         // Flags //
  uint8_t *p = htm_temperature_buffer;
  uint32_t htm_temperatrue_flt;

  UINT8_TO_BITSTREAM(p, flags);                                                 // Encode flag data //
  htm_temperatrue_flt = UINT32_TO_FLOAT(temp_in*TEMPERATURE_INT_TO_FLOAT, -3);  // Floating point conversion //
  UINT32_TO_BITSTREAM(p, htm_temperatrue_flt);                                  // Encode temperature data //

  uint8_t temp_eight_bit = (uint8_t)temp_in;

  // Write to GATT server write attribute //
  returnStatus = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_value, 0, TEMPERATURE_DATA_SIZE, &temp_eight_bit);

  if(returnStatus != SL_STATUS_OK){
      LOG_ERROR("gatt db server write attributes call returned with error code = 0x%04x\n\r", (uint32_t)returnStatus);
  }

  // Check for conditions //
  if(ble_data.bme280IndicationEnable && (ble_data.connectionOpened)){
    if(!ble_data.inFlight){
      // Send data over BLE //
      returnStatus = sl_bt_gatt_server_send_indication(ble_data.connectionHandle, gattdb_temperature_value, BLE_BUFFER_SIZE, &htm_temperature_buffer[0]);
      if(returnStatus != SL_STATUS_OK){
        LOG_ERROR("server send indication call returned with error code  = 0x%04x\n\r", (uint32_t)returnStatus);
      }
      else{
        ble_data.inFlight = true;                                             // Enable inFlight //
      }
    }
  }
}
#endif
