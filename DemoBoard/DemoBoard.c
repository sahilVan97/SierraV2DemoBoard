/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stm32h7xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>
#include "../lv_demos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId LEDThread1Handle, lvglUIHandle;

/* Private function prototypes -----------------------------------------------*/
static void LED_Thread1(void const *argument);
static void LGVL_Thread(void const *argument);

static void selectors_create(lv_obj_t * parent);
static void text_input_create(lv_obj_t * parent);
static void msgbox_create(void);
static void msgbox_event_cb(lv_event_t * e);
static void ta_event_cb(lv_event_t * e);

/* Private functions ---------------------------------------------------------*/


/* Static Variables ---------------------------------------------------------*/
static lv_group_t * g;
static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) {
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
	HAL_Init();  
	
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_13;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Thread 1 definition */
	osThreadDef(LED1, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	 /*  Thread 2 definition */
    osThreadDef(lgvlUI, LGVL_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	/* Start thread 1 */
	LEDThread1Handle = osThreadCreate(osThread(LED1), NULL);
  
	/* Start thread 2 */
    lvglUIHandle = osThreadCreate(osThread(lgvlUI), NULL);
  
	/* Start scheduler */
	osKernelStart();

	  /* We should never get here as control is now taken by the scheduler */
	for (;;)
		;
}

void SysTick_Handler(void) {
	HAL_IncTick();
	osSystickHandler();
}

/**
  * @brief  Toggle LED1
  * @param  thread not used
  * @retval None
  */
static void LED_Thread1(void const *argument) {
	(void) argument;
  
	for (;;) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		osDelay(2000);
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		osDelay(2000);
		
	}
}

/**
  * @brief  Toggle LED2 thread
  * @param  argument not used
  * @retval None
  */
static void LGVL_Thread(void const *argument) {
    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t * cur_drv = NULL;
    
	for (;;) {
    	cur_drv = lv_indev_get_next(cur_drv);
    	if (!cur_drv) {
        	break;
    	}

    	if (cur_drv->driver->type == LV_INDEV_TYPE_KEYPAD) {
        	lv_indev_set_group(cur_drv, g);
    	}

    	if (cur_drv->driver->type == LV_INDEV_TYPE_ENCODER) {
        	lv_indev_set_group(cur_drv, g);
    	}
	}
    
    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, LV_DPI_DEF / 3);

    t1 = lv_tabview_add_tab(tv, "Selectors");
    t2 = lv_tabview_add_tab(tv, "Text input");

    selectors_create(t1);
    text_input_create(t2);

    msgbox_create();
}

static void selectors_create(lv_obj_t * parent) {
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * obj;

    obj = lv_table_create(parent);
    lv_table_set_cell_value(obj, 0, 0, "00");
    lv_table_set_cell_value(obj, 0, 1, "01");
    lv_table_set_cell_value(obj, 1, 0, "10");
    lv_table_set_cell_value(obj, 1, 1, "11");
    lv_table_set_cell_value(obj, 2, 0, "20");
    lv_table_set_cell_value(obj, 2, 1, "21");
    lv_table_set_cell_value(obj, 3, 0, "30");
    lv_table_set_cell_value(obj, 3, 1, "31");
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_calendar_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_btnmatrix_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_checkbox_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_slider_create(parent);
    lv_slider_set_range(obj, 0, 10);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_switch_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_spinbox_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_dropdown_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_roller_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t * list = lv_list_create(parent);
    lv_obj_update_layout(list);
    if (lv_obj_get_height(list) > lv_obj_get_content_height(parent)) {
        lv_obj_set_height(list, lv_obj_get_content_height(parent));
    }

    lv_list_add_btn(list, LV_SYMBOL_OK, "Apply");
    lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Close");
    lv_list_add_btn(list, LV_SYMBOL_EYE_OPEN, "Show");
    lv_list_add_btn(list, LV_SYMBOL_EYE_CLOSE, "Hide");
    lv_list_add_btn(list, LV_SYMBOL_TRASH, "Delete");
    lv_list_add_btn(list, LV_SYMBOL_COPY, "Copy");
    lv_list_add_btn(list, LV_SYMBOL_PASTE, "Paste");
}

static void text_input_create(lv_obj_t * parent) {
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * ta1 = lv_textarea_create(parent);
    lv_obj_set_width(ta1, LV_PCT(100));
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_placeholder_text(ta1, "Click with an encoder to show a keyboard");

    lv_obj_t * ta2 = lv_textarea_create(parent);
    lv_obj_set_width(ta2, LV_PCT(100));
    lv_textarea_set_one_line(ta2, true);
    lv_textarea_set_placeholder_text(ta2, "Type something");

    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(ta2, ta_event_cb, LV_EVENT_ALL, kb);
}

static void msgbox_create(void) {
    static const char * btns[] = { "Ok", "Cancel", "" };
    lv_obj_t * mbox = lv_msgbox_create(NULL, "Hi", "Welcome to the keyboard and encoder demo", btns, false);
    lv_obj_add_event_cb(mbox, msgbox_event_cb, LV_EVENT_ALL, NULL);
    lv_group_focus_obj(lv_msgbox_get_btns(mbox));
    lv_obj_add_state(lv_msgbox_get_btns(mbox), LV_STATE_FOCUS_KEY);
#if LV_EX_MOUSEWHEEL
    lv_group_set_editing(g, true);
#endif
    lv_group_focus_freeze(g, true);

    lv_obj_align(mbox, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * bg = lv_obj_get_parent(mbox);
    lv_obj_set_style_bg_opa(bg, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(bg, lv_palette_main(LV_PALETTE_GREY), 0);
}

static void msgbox_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * msgbox = lv_event_get_current_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        const char * txt = lv_msgbox_get_active_btn_text(msgbox);
        if (txt) {
            lv_msgbox_close(msgbox);
            lv_group_focus_freeze(g, false);
            lv_group_focus_obj(lv_obj_get_child(t1, 0));
            lv_obj_scroll_to(t1, 0, 0, LV_ANIM_OFF);

        }
    }
}

static void ta_event_cb(lv_event_t * e) {
    lv_indev_t * indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_indev_type_t indev_type = lv_indev_get_type(indev);

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED && indev_type == LV_INDEV_TYPE_ENCODER) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(kb);
        lv_group_set_editing(lv_obj_get_group(kb), kb);
        lv_obj_set_height(tv, LV_VER_RES / 2);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_height(tv, LV_VER_RES);
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
