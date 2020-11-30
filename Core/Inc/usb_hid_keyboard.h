#ifndef INC_USB_HID_KEYBOARD_H_
#define INC_USB_HID_KEYBOARD_H_

#include "usbd_def.h"

void USB_Keyboard_SendChar(char ch);
void USB_Keyboard_SendString(char *s);

#endif /* INC_USB_HID_KEYBOARD_H_ */
