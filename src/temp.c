		switch (buttonDescriptor->buttonType) {
			case BTN_MENU:
				printf("[%s:%d] Button \"MENU\" pressed (%d);\n"
					,__FUNCTION__, __LINE__, buttonDescriptor->buttonPressCounter);
				break;

			case BTN_UP:
				printf("[%s:%d] Button \"UP\" pressed (%d);\n"
					,__FUNCTION__, __LINE__, buttonDescriptor->buttonPressCounter);
				break;

			case BTN_DOWN:
				printf("[%s:%d] Button \"DOWN\" pressed (%d);\n"
					,__FUNCTION__, __LINE__, buttonDescriptor->buttonPressCounter);

				break;

			case BTN_EXIT:
				printf("[%s:%d] Button \"EXIT\" pressed (%d);\n"
					,__FUNCTION__, __LINE__, buttonDescriptor->buttonPressCounter);
				break;

			default:
				printf("[%s:%d] Button \"UNKNOWN\" pressed (%d);\n"
					,__FUNCTION__, __LINE__, buttonDescriptor->buttonPressCounter);
				break;
		}
		buttonDescriptor->buttonPressCounter++;
		

void getButtonType(char *str, ButtonType btype)
{
	switch (btype) {
	case BTN_MENU:
		sprintf(str, "BTN_MENU");
		break;

	case BTN_UP:
		sprintf(str, "BTN_UP");
		break;

	case BTN_DOWN:
		sprintf(str, "BTN_DOWN");
		break;

	case BTN_EXIT:
		sprintf(str, "BTN_EXIT");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void getGPIO_State(char *str, ButtonType btype)
{
	switch (btype) {
	case GPIO_LOW:
		sprintf(str, "GPIO_LOW");
		break;

	case GPIO_HIGH:
		sprintf(str, "GPIO_HIGH");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void geButtonSignalTpye(char *str, ButtonType btype)
{
	switch (btype) {
	case B_NO_SIGNAL:
		sprintf(str, "B_NO_SIGNAL");
		break;

	case B_PRESSED:
		sprintf(str, "B_PRESSED");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void ButtonDescriptorToStr(char *str, ButtonDescriptor *button)
{
	char btn_type_str[16];
	char btn_state_str[16];
	char btn_signal_str[16];

	getButtonType(btn_type_str, button->buttonType);
	getGPIO_State(btn_state_str, button->buttonsState);
	geButtonSignalTpye(btn_signal_str, button->buttonSignal);

	sprintf(str,
	"    Pin   : %d\n"
	"    Type  : %s\n"
	"    State : %s\n"
	"    Signal: %s\n"
	"    Count : %d\n"
	, button->buttonPin
	, btn_type_str
	, btn_state_str
	, btn_signal_str
	, button->buttonPressCounter
	);

}

void printButtonList(struct ButtonList *btnlist)
{
	char btn_dsc_str[256];

	printf("\nbuttonList [%p]\n", btnlist);

	while (btnlist) {
		printf("  ButtonDescriptor [%p]\n", btnlist->buttonDescriptor);
		ButtonDescriptorToStr(btn_dsc_str, btnlist->buttonDescriptor);
		printf("%s", btn_dsc_str);
		btnlist = btnlist->next;
		printf("\nnext [%p]\n", btnlist);
	}
}

// HANDLER ----------------------------------------------------------
		
