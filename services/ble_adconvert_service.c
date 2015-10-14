/* This file was generated by plugin 'Nordic Semiconductor nRF5x v.1.1.6' (BDS version 1.0.1531.0) */

#include "ble_adconvert_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "app_util_bds.h"

#define OPCODE_LENGTH 1 /**< Length of opcode inside ADconvert service packet. */
#define HANDLE_LENGTH 2 /**< Length of handle inside ADconvert service packet. */

/* TODO Consider changing the max values if encoded data for characteristic/descriptor is fixed length */ 
#define MAX_ADVALUE_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted ADvalue. */ 

/**@brief Function for encoding ADvalue.
 *
 * @param[in]   p_advalue              ADvalue characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t advalue_encode(ble_adconvert_service_advalue_t * p_advalue, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    len += bds_uint16_encode(&p_advalue->advalue, &encoded_buffer[len]); 
    return len;
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_adconvert_service       ADconvert service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_adconvert_service_t * p_adconvert_service, ble_evt_t * p_ble_evt)
{
    p_adconvert_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_adconvert_service       ADconvert service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_adconvert_service_t * p_adconvert_service, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_adconvert_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_adconvert_service       ADconvert service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_adconvert_service_t * p_adconvert_service, ble_gatts_evt_write_t * p_ble_evt)
{
    
    if(p_ble_evt->handle == p_adconvert_service->advalue_handles.cccd_handle)
    {
        if(p_adconvert_service->evt_handler != NULL)
        {
            ble_adconvert_service_evt_t evt;
            evt.evt_type = BLE_ADCONVERT_SERVICE_ADVALUE_EVT_CCCD_WRITE;
            bds_uint16_decode(p_ble_evt->len, p_ble_evt->data, &evt.params.cccd_value);
            p_adconvert_service->evt_handler(p_adconvert_service, &evt);
        }
    } 
}

/**@brief Authorize WRITE request event handler.
 *
 * @details Handles WRITE events from the BLE stack.
 *
 * @param[in]   p_sc_ctrlpt  SC Ctrlpt structure.
 * @param[in]   p_gatts_evt  GATTS Event received from the BLE stack.
 *
 */
static void on_rw_authorize_request(ble_adconvert_service_t * p_adconvert_service, ble_gatts_evt_t * p_gatts_evt)
{
    ble_gatts_evt_rw_authorize_request_t * p_auth_req = &p_gatts_evt->params.authorize_request;
    if (p_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        if (   (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_PREP_WRITE_REQ)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)
           )
        {
        
        }
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_adconvert_service       ADconvert service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_adconvert_service_on_ble_evt(ble_adconvert_service_t * p_adconvert_service, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_adconvert_service, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_adconvert_service, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_adconvert_service, &p_ble_evt->evt.gatts_evt.params.write);
            break;
         case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_request(p_adconvert_service, &p_ble_evt->evt.gatts_evt);
            break;
        default:
            //No implementation needed.
            break;
    }
}

/**@brief Function for initializing the ADconvert service. */
uint32_t ble_adconvert_service_init(ble_adconvert_service_t * p_adconvert_service, const ble_adconvert_service_init_t * p_adconvert_service_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_adconvert_service->evt_handler = p_adconvert_service_init->evt_handler;
    p_adconvert_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // Add a custom base UUID.
    ble_uuid128_t bds_base_uuid = {{0xEF, 0xE8, 0xC7, 0x87, 0xF7, 0x70, 0x43, 0x92, 0x1E, 0x47, 0xD1, 0x7C, 0x00, 0x00, 0x40, 0x40}};
    uint8_t       uuid_type;
    err_code = sd_ble_uuid_vs_add(&bds_base_uuid, &uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    ble_uuid.type = uuid_type;
    ble_uuid.uuid = 0xE9EB;
        
    // Add service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_adconvert_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    // Add ADvalue characteristic
    ble_adconvert_service_advalue_t advalue_initial_value = p_adconvert_service_init->ble_adconvert_service_advalue_initial_value; 

    uint8_t advalue_encoded_value[MAX_ADVALUE_LEN];
    ble_add_char_params_t add_advalue_params;
    memset(&add_advalue_params, 0, sizeof(add_advalue_params));
    
    add_advalue_params.uuid                = 0x2E72; 
    add_advalue_params.max_len             = MAX_ADVALUE_LEN;
    add_advalue_params.init_len            = advalue_encode(&advalue_initial_value, advalue_encoded_value);
    add_advalue_params.p_init_value        = advalue_encoded_value; 
    add_advalue_params.char_props.notify   = 1; 
    add_advalue_params.char_props.read     = 1; 
    add_advalue_params.read_access         = SEC_OPEN; 
    add_advalue_params.cccd_write_access   = SEC_OPEN;
    // 1 for variable length and 0 for fixed length.
    add_advalue_params.is_var_len          = 1; 

    err_code = characteristic_add(p_adconvert_service->service_handle, &add_advalue_params, &(p_adconvert_service->advalue_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    return NRF_SUCCESS;
}

/**@brief Function for setting the ADvalue. */
uint32_t ble_adconvert_service_advalue_set(ble_adconvert_service_t * p_adconvert_service, ble_adconvert_service_advalue_t * p_advalue)
{
    ble_gatts_value_t gatts_value;
    uint8_t encoded_value[MAX_ADVALUE_LEN];

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = advalue_encode(p_advalue, encoded_value);
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_value;

    return sd_ble_gatts_value_set(p_adconvert_service->conn_handle, p_adconvert_service->advalue_handles.value_handle, &gatts_value);
}

/**@brief Function for sending the ADvalue. */
uint32_t ble_adconvert_service_advalue_send(ble_adconvert_service_t * p_adconvert_service, ble_adconvert_service_advalue_t * p_advalue)
{
    uint32_t err_code = NRF_SUCCESS;

    if (p_adconvert_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        ble_gatts_hvx_params_t hvx_params;
        uint8_t encoded_value[MAX_ADVALUE_LEN];
        uint16_t hvx_len;

        // Initialize value struct.
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_len           = advalue_encode(p_advalue, encoded_value);
        hvx_params.handle = p_adconvert_service->advalue_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.p_len  = &hvx_len;
        hvx_params.offset = 0;
        hvx_params.p_data = encoded_value;

        err_code = sd_ble_gatts_hvx(p_adconvert_service->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}
