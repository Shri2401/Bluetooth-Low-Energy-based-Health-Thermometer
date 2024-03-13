/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (temperature sensor)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart rate sensor)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : scheduler.c
 *
 */

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>
#include <stdbool.h>

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "sl_power_manager.h"
#include "em_i2c.h"
#include "em_core.h"


/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "scheduler.h"
#include "ble_device_type.h"
#include "app.h"
#include "gpio.h"
#include "i2c.h"
#include "timers.h"
#include "ble.h"
#include "lcd.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

/****************************************************** MACROS ********************************************************/
#define I2C_STARTUP_WAIT 80000
#define I2C_READY_WAIT   10800

uint32_t event_store = 0x00000000;                               // Data Structure to store the events //
#if (!DEVICE_IS_BLE_SERVER)
//static const uint8_t tempBME280Service[16] = {0xa2, 0x85, 0x1a, 0x54, 0x46, 0x78, 0x8d, 0x9b, 0xfc, 0x4e, 0x2a, 0x7b, 0x8e, 0x0c, 0xfb, 0x96};
//static const uint8_t tempBME280Char[16] = {0x55, 0x1e, 0x70, 0x9a, 0x95, 0x9a, 0x7b, 0x85, 0xb4, 0x4f, 0x96, 0xeb, 0x93, 0xd1, 0x78, 0xb2};
static const uint8_t tempBME280Service[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x08, 0x00, 0x00, 0x00 };
static const uint8_t tempBME280Char[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x09, 0x00, 0x00, 0x00};
static const uint8_t heartRateService[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x01, 0x00, 0x00, 0x00 };
static const uint8_t heartRateChar[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x02, 0x00, 0x00, 0x00};

#endif

void schedulerSetEventUF(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= UNDER_FLOW_INTERRUPT;                         // Set the event //
  sl_bt_external_signal(UNDER_FLOW_INTERRUPT);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}

void schedulerSetEventComp1(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= COMP1_INTERRUPT;                              // Set the event //
  sl_bt_external_signal(COMP1_INTERRUPT);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}

void schedulerSetEventButtonPress(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= I2C_TRANSFER_COMPLETE;                        // Set the event //
  sl_bt_external_signal(PB0_PRESS);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}

void schedulerSetEventButtonRelease(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= I2C_TRANSFER_COMPLETE;                        // Set the event //
  sl_bt_external_signal(PB0_RELEASE);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}

void schedulerSetEventButtonReleasePB1(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= I2C_TRANSFER_COMPLETE;                        // Set the event //
  sl_bt_external_signal(PB1_RELEASE);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}

void schedulerSetEventButtonPressPB1(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                         // Entering critical section to set the event //

  //event_store |= I2C_TRANSFER_COMPLETE;                        // Set the event //
  sl_bt_external_signal(PB1_PRESS);
  CORE_EXIT_CRITICAL();                                          // Exit the critical section //
}



#if (DEVICE_IS_BLE_SERVER)
void myEventHandler(sl_bt_msg_t* nextEvent){

  // Check for the external signal //
  if(SL_BT_MSG_ID(nextEvent->header) != sl_bt_evt_system_external_signal_id)
    return;
  ble_data_struct_t* myBleEvent = getBleEvent();

  if ((myBleEvent->connectionOpened) && (myBleEvent->bme280IndicationEnable) && (myBleEvent->isBonded)){
    if (nextEvent->data.evt_system_external_signal.extsignals & UNDER_FLOW_INTERRUPT){
        measureTemperature();
    }
  }

  if ((myBleEvent->connectionOpened) && (myBleEvent->heartRateIndicationEnabled) && (myBleEvent->isBonded)){
    if (nextEvent->data.evt_system_external_signal.extsignals & UNDER_FLOW_INTERRUPT){
        measureHeartRate();
    }
  }
}
#else
typedef enum{
   idle_client,
   discover_temp_characteristic,
   set_temp_notifications,
   discover_heart_rate_service,
   discover_heart_rate_characteristic,
   set_heart_rate_notifications,
   procedure_complete,
   close_event
}discover_state_t;

void myDiscoveryStateMachine(sl_bt_msg_t* evt)
{
  ble_data_struct_t *ble_data = getBleEvent();                                  // Pointer to a local data structure //
  static discover_state_t currentState = idle_client;                           // States //
  sl_status_t returnStatus;
  if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id){             // Go to idle state if Close event happened //
      currentState = idle_client;
  }

  switch(currentState)
  {
    case idle_client:
      {
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_opened_id){          // Check for OpenID event //
          returnStatus = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connectionHandle, sizeof(tempBME280Service), tempBME280Service);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_discover_primary_services_by_uuid TEMP returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          LOG_INFO("INSIDE_SERVICE - %u\n\r", ble_data->connectionHandle);
          currentState = discover_temp_characteristic;
      }
      break;
      }

    case discover_temp_characteristic:
      {
      if((SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)){          // Check for GATT completed event //
          returnStatus = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connectionHandle, ble_data->discoveredServiceHandle, sizeof(tempBME280Char), tempBME280Char);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid TEMP returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          currentState = set_temp_notifications;
          LOG_INFO("INSIDE_CHAR - %u\n\r", ble_data->connectionHandle);
          LOG_INFO("INSIDE_CHAR SER - %u\n\r", ble_data->discoveredServiceHandle);
      }

      break;
      }

    case set_temp_notifications:
      {
      if((SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)){          // Check for GATT completed event //
          returnStatus = sl_bt_gatt_set_characteristic_notification(ble_data->connectionHandle, ble_data->discoveredCharacteristicHandle, sl_bt_gatt_indication);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_set_characteristic_notification TEMP returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          currentState = discover_heart_rate_service;
          LOG_INFO("INSIDE_NOTIFY - %u\n\r", ble_data->connectionHandle);
      }
      break;
      }

    case discover_heart_rate_service:
      {
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id){          // Check for GATT completed event //
          returnStatus = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connectionHandle, sizeof(heartRateService), heartRateService);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_discover_primary_services_by_uuid HEART RATE returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          LOG_INFO("INSIDE_SERVICE - %u\n\r", ble_data->connectionHandle);
          currentState = discover_heart_rate_characteristic;
      }
      break;
      }

    case discover_heart_rate_characteristic:
      {
      if((SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)){          // Check for GATT completed event //
          returnStatus = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connectionHandle, ble_data->discoveredServiceHandle, sizeof(heartRateChar), heartRateChar);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid HEART RATE returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          currentState = set_heart_rate_notifications;
          LOG_INFO("INSIDE_CHAR - %u\n\r", ble_data->connectionHandle);
          LOG_INFO("INSIDE_CHAR SER - %u\n\r", ble_data->discoveredServiceHandle);
      }

      break;
      }

    case set_heart_rate_notifications:
      {
      if((SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)){          // Check for GATT completed event //
          returnStatus = sl_bt_gatt_set_characteristic_notification(ble_data->connectionHandle, ble_data->discoveredCharacteristicHandle, sl_bt_gatt_indication);
          if(returnStatus != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_set_characteristic_notification HEART RATE returned error = 0x%04x\n", (uint32_t)returnStatus);
          }
          currentState = procedure_complete;
          LOG_INFO("INSIDE_NOTIFY - %u\n\r", ble_data->connectionHandle);
      }
      break;
      }
    case procedure_complete:
      {
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id){          // Check for GATT completed event //
          currentState = close_event;
      }
      break;
      }

    case close_event:
      {
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id){                 // Check for close connection event //
          currentState = idle_client;
      }
      break;
      }
  }

}

void readTemperatureValue(sl_bt_msg_t* evt){

  // Check for the external signal //
  if(SL_BT_MSG_ID(evt->header) != sl_bt_evt_system_external_signal_id)
    return;
  ble_data_struct_t* myBleEvent = getBleEvent();

  if ((myBleEvent->connectionOpened) && (myBleEvent->isBonded)){
    if (evt->data.evt_system_external_signal.extsignals & UNDER_FLOW_INTERRUPT){
        getTemperatureValue();
    }
  }

}
#endif
