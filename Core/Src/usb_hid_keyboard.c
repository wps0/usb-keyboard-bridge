/*
 * This code now supports HID and Custom HID library examples;
 * difference is in library included in header file and
 * a few function calls for Sending Reports in "USB_Keyboard_SendChar" function
 *
 *******************************************************************************
 * For HID:
 * -replace Mouse report descriptor for keyboard
 *  report descriptor in "usbd_hid.h" and change report descriptor
 *  size in "usbd_conf.h"
 * -change device descriptor for FS and HS to 0x01 for
 *	keyboard (default 0x02 for mouse)
 *
 *	Note: every time you change and save your CubeMx configuration, library
 *	settings get reset, so Device and Report descriptors and its sizes will
 *	be reset and you will need to change them back -> recommend using Custom HID
 ******************************************************************************
 * For Custom HID:
 * -add keyboard report descriptor provided in repository or from
 *  USB HID documentation (v. 1.11) page 79 and copy contents into
 * 	"uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE]"
 * 	variable in file "usbd_custom_hid.c" This way, report descriptor
 * 	won't be overwritten every time you change something within cube;
 * 	make sure you print it between "USER CODE" comments; 0xC0 is already there
 *
 * -Other settings to change in CubeMx under USB_DEVICE setting:
 * 	-change USBD_CUSTOM_HID_REPORT_DESC_SIZE to "63" using usb keyboard example
 * 	-change CUSTOM_HID_FS_BINTERVAL to 0x0A as in example
 * 	-change USBD_CUSTOMHID_OUTREPORT_BUF_SIZE to 8
 *
 * Now your keyboard / custom HID setting will not be changed when modifying
 * your cube configuration, therefore my recommended choice for your
 * mouse/keyboard/controller or custom device implementation is "custom HID"
 *
 ******************************************************************************
 * Include this code in your project.
 * You can also add this and the header file to your project.
 * Make sure to include header file in main.c
 * Some actions are based on English keyboard layout.
 * Only letters and symbols '.', "space", "enter" '!' and '?' are implemented.
 * Add cases in switch statement at line 38 for additional symbols and actions
 *
 * NOTE:
 * key combinations such as ?,!,@ are created as you would on QWERTY US layout
*/

#include "usb_hid_keyboard.h"

extern USBD_HandleTypeDef hUsbDeviceHS;
static uint8_t HID_buffer[8];

static void USB_charToCode(char ch)
{
	// Check if lower or upper case

	uint8_t index = -1;

	for (uint8_t i = 0; i < sizeof(HID_KEYBRD_Key); i++) {
		if (ch == HID_KEYBRD_Key[i]) {
			// TODO: use only bitwise operators
			if (HID_buffer[0] & 2U) {
				HID_buffer[0] = HID_buffer[0] ^ 2U;
			}
			if (HID_buffer[0] & 16U) {
				HID_buffer[0] = HID_buffer[0] ^ 16U;
			}
			index = i;
			break;
		}
	}

	for (uint8_t i = 0; i < sizeof(HID_KEYBRD_ShiftKey); i++) {
		if (ch == HID_KEYBRD_ShiftKey[i]) {
			HID_buffer[0] |= 2U;
			index = i;
			break;
		}
	}

	for (uint8_t i = 0; i < sizeof(HID_KEYBRD_Codes); i++) {
		if (index == HID_KEYBRD_Codes[i]) {
			HID_buffer[2] = i;
			break;
		}
	}
}

// Send character as a single key press
void USB_Keyboard_SendChar(char ch, uint8_t release_key, HID_KEYBD_Info_TypeDef *kbd_info) {
	uint8_t ret;

	uint8_t modifiers = 0;
	modifiers |= kbd_info->lctrl;
	modifiers |= kbd_info->lshift << 1;
	modifiers |= kbd_info->lalt << 2;
	modifiers |= kbd_info->lgui << 3;
	modifiers |= kbd_info->rctrl << 4;
	modifiers |= kbd_info->rshift << 5;
	modifiers |= kbd_info->ralt << 6;
	modifiers |= kbd_info->rgui << 7;
	printf("Modifiers: %d\n", modifiers);

	HID_buffer[3] = kbd_info->keys[0];
	HID_buffer[4] = kbd_info->keys[1];
	HID_buffer[5] = kbd_info->keys[2];
	HID_buffer[6] = kbd_info->keys[3];
	HID_buffer[7] = kbd_info->keys[4];

	// Convert character to usb hid key code
	USB_charToCode(ch);

	// press keys
	ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, HID_buffer, 8);
	if (ret != HAL_OK)
	{
		Error_Handler();
	}

	// release keys
	if (release_key) {
		HAL_Delay(15);
		HID_buffer[0] = 0;
		HID_buffer[2] = 0;
		ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, HID_buffer, 8);
		if (ret != HAL_OK)
		{
			Error_Handler();
		}
		HAL_Delay(15);
	}
}

// Send string as letters
void USB_Keyboard_SendString(char * s, HID_KEYBD_Info_TypeDef *kbd_info)
{
	uint8_t i = 0;

	while(*(s+i))
	{
		USB_Keyboard_SendChar(*(s+i), 1, kbd_info);
		i++;
	}
}
