/*
 * aoadefs.h
 *
 *  Created on: May 23, 2016
 *      Author: liqi
 */

#ifndef AOADEFS_H_
#define AOADEFS_H_

typedef enum {
	ANDROID_DEVICE_AOA_STATE_UNKNOWN,		/* Unknown Android-powered device AOA state */
	ANDROID_DEVICE_AOA_STATE_ACCESSORY,		/* Supports Android accessory mode and is already in accessory mode */
	ANDROID_DEVICE_AOA_STATE_NEED_SWITCH,	/* Supports Android accessory mode but it is not in accessory mode */
	ANDROID_DEVICE_AOA_STATE_NOT_SUPPORT	/* Does not support Android accessory mode */
} android_device_aoa_state_t ;

#define AOADEV_VID_GOOGLE				0x18D1		/* Google's ID for Accessory */
#define AOADEV_PID_ACCESSORY			0x2D00		/* accessory, Provides two bulk endpoints for communicating with an Android application. */
#define AOADEV_PID_ACCESSORY_ADB		0x2D01		/* accessory + adb,	For debugging purposes during accessory development. Available only if the user has enabled USB Debugging in the Android device settings. */
#define AOADEV_PID_AUDIO				0x2D02		/* AOAv2, For streaming audio from an Android device to an accessory. */
#define AOADEV_PID_AUDIO_ADB			0x2D03		/* AOAv2 */
#define AOADEV_PID_ACCESSORY_AUDIO		0x2D04		/* AOAv2 */
#define AOADEV_PID_ACCESSORY_AUDIO_ADB	0x2D05		/* AOAv2 */

/*
 * Attempt to start Android-powered device in accessory mode
 * If the vendor and product IDs do not correspond to an Android-powered device in accessory mode,
 * the accessory cannot discern whether the device supports (but is not in) accessory mode
 * or if the device does not support accessory mode.
 * This can occur because devices that support accessory mode
 * (but are not in accessory mode) initially report the device manufacturer vendor and product IDs
 * instead of the AOA vendor and product IDs.
 *
 * The accessory should try to start the device in accessory mode to determine if the device supports that mode:
 */

/*
 * Send a 51 control request ("Get Protocol") to determine
 * if the device supports the Android accessory protocol.
 * If the device supports the protocol, it returns a non-zero number that
 * represents the supported protocol version.
 *
 * The control request is on endpoint 0 with the following characteristics:
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        51
 *  value:          0
 *  index:          0
 *  data            protocol version number (16 bits little endian sent from the
                device to the accessory)
 */
#define USB_CNTLREQ_GET_PROTOCOL			51


/*
 * If the device returns a supported protocol version,
 * send a control request with identifying string information to the device.
 * This information allows the device to determine an appropriate application for the accessory
 * (or present a URL to the user if an appropriate application does not exist).
 *
 * The control request is on endpoint 0 (for each string ID) with the following characteristics:
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        52
 *  value:          0
 *  index:          string ID
 *  data            zero terminated UTF8 string sent from accessory to device
 */
#define USB_CNTLREQ_SEND_STRING				52

#define SEND_STRING_ID_MANUFACTURER			0
#define SEND_STRING_ID_MODEL				1
#define SEND_STRING_ID_DESCRIPTION			2
#define SEND_STRING_ID_VERSION				3
#define SEND_STRING_ID_URI					4
#define SEND_STRING_ID_SERIAL				5

/*
 * Send a control request to ask the device to start in accessory mode.
 *
 * The control request is on endpoint 0 with the following characteristics:
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        53
 *  value:          0
 *  index:          0
 *  data            none
 */
#define USB_CNTLREQ_START_ACCESSORY_MODE	53

/* Audio Support in AOAv2 */

/*
 * Control request for set audio mode.
 * AOAv2 includes support for audio output from an Android device to an accessory via
 * a standard USB audio class interface capable of 2 channel, 16-bit PCM audio
 * with a bit rate of 44100 Khz (additional audio modes may be added in the future).
 *
 * To enable audio support, the accessory must send a new USB control request
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        58
 *  value:          Accessory assigned ID for the HID device
 *  index:          total length of the HID report descriptor
 *  data            none
 */
#define USB_CNTLREQ_ENABLE_AUDIO			58

/* HID Support in AOAv2 */

/*
 * Control request for registering a HID device.
 * Upon registering, a unique ID is sent by the accessory in the
 * value parameter. This ID will be used for future commands for
 * the device
 *
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        ACCESSORY_REGISTER_HID_DEVICE
 *  value:          0 for no audio (default),
 *  				1 for 2 channel, 16-bit PCM at 44100 KHz
 *  index:          0
 *  data            none
 */
#define USB_CNTLREQ_ACCESSORY_REGISTER_HID         54

/*
 * Control request for unregistering a HID device.
 *
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        ACCESSORY_REGISTER_HID
 *  value:          Accessory assigned ID for the HID device
 *  index:          0
 *  data            none
 */
#define USB_CNTLREQ_ACCESSORY_UNREGISTER_HID         55

/*
 * Control request for sending the HID report descriptor.
 * If the HID descriptor is longer than the endpoint zero max packet size,
 * the descriptor will be sent in multiple ACCESSORY_SET_HID_REPORT_DESC
 * commands. The data for the descriptor must be sent sequentially
 * if multiple packets are needed.
 *
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        ACCESSORY_SET_HID_REPORT_DESC
 *  value:          Accessory assigned ID for the HID device
 *  index:          offset of data in descriptor
 *                      (needed when HID descriptor is too big for one packet)
 *  data            the HID report descriptor
 */
#define USB_CNTLREQ_ACCESSORY_SET_HID_REPORT_DESC         56

/*
 * Control request for sending HID events.
 *
 *  requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
 *  request:        ACCESSORY_SEND_HID_EVENT
 *  value:          Accessory assigned ID for the HID device
 *  index:          0
 *  data            the HID report for the event
 */
#define USB_CNTLREQ_ACCESSORY_SEND_HID_EVENT         57


/*
 * Accessory Identifying String Information to android device
 */
typedef struct accessory_identify_info {
	char manufacturer[256];
	char model[256];
	char description[256];
	char version[256];
	char uri[256];
	char serial[256];
} accessory_identify_info_t;




#endif /* AOADEFS_H_ */
