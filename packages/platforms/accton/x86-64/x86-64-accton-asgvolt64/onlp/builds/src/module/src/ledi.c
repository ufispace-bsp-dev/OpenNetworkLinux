/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2013 Accton Technology Corporation.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#include <onlplib/file.h>
#include <onlp/platformi/ledi.h>
#include "platform_lib.h"

#define VALIDATE(_id)                           \
    do {                                        \
        if(!ONLP_OID_IS_LED(_id)) {             \
            return ONLP_STATUS_E_INVALID;       \
        }                                       \
    } while(0)

#define prefix_path "/sys/devices/platform/accton_asgvolt64_led/leds/"
#define filename    "brightness"

enum onlp_led_id
{
    LED_RESERVED = 0,
    LED_LOC = 1,
    LED_STATE,
    LED_ALARM,
    LED_PSU1,
    LED_PSU2,
    LED_FAN,
};

/* LED related data
 */

enum led_light_mode {
    LED_MODE_OFF,
    LED_MODE_RED 				= 10,
    LED_MODE_RED_BLINKING 		= 11,
    LED_MODE_ORANGE 			= 12,
    LED_MODE_ORANGE_BLINKING 	= 13,
    LED_MODE_YELLOW 			= 14,
    LED_MODE_YELLOW_BLINKING 	= 15,
    LED_MODE_GREEN 				= 16,
    LED_MODE_GREEN_BLINKING 	= 17,
    LED_MODE_BLUE 				= 18,
    LED_MODE_BLUE_BLINKING 		= 19,
    LED_MODE_PURPLE 			= 20,
    LED_MODE_PURPLE_BLINKING 	= 21,
    LED_MODE_AUTO 				= 22,
    LED_MODE_AUTO_BLINKING 		= 23,
    LED_MODE_WHITE 				= 24,
    LED_MODE_WHITE_BLINKING 	= 25,
    LED_MODE_CYAN 				= 26,
    LED_MODE_CYAN_BLINKING 		= 27,
    LED_MODE_UNKNOWN			= 99
};

typedef struct led_light_mode_map {
    enum onlp_led_id id;
    enum led_light_mode driver_led_mode;
    enum onlp_led_mode_e onlp_led_mode;
} led_light_mode_map_t;

led_light_mode_map_t led_map[] = {
{LED_LOC,   LED_MODE_OFF,            ONLP_LED_MODE_OFF},
{LED_LOC,   LED_MODE_ORANGE_BLINKING,ONLP_LED_MODE_ORANGE_BLINKING},
{LED_STATE, LED_MODE_RED,            ONLP_LED_MODE_RED},
{LED_STATE, LED_MODE_BLUE,           ONLP_LED_MODE_BLUE},
{LED_STATE, LED_MODE_GREEN,          ONLP_LED_MODE_GREEN},
{LED_ALARM, LED_MODE_RED,            ONLP_LED_MODE_RED},
{LED_ALARM, LED_MODE_BLUE,           ONLP_LED_MODE_BLUE},
{LED_ALARM, LED_MODE_GREEN,          ONLP_LED_MODE_GREEN},
{LED_FAN,  LED_MODE_AUTO,            ONLP_LED_MODE_AUTO},
{LED_PSU1, LED_MODE_AUTO,            ONLP_LED_MODE_AUTO},
{LED_PSU2, LED_MODE_AUTO,            ONLP_LED_MODE_AUTO}
};


static char *leds[] =  /* must map with onlp_led_id */
{
    NULL,
    "loc",
    "state",
    "alarm",
    "psu1",
    "psu2",
    "fan"
};

/*
 * Get the information for the given LED OID.
 */
static onlp_led_info_t linfo[] =
{
    { }, /* Not used */
    {
        { ONLP_LED_ID_CREATE(LED_LOC), "Chassis LED 1 (LOC LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
        ONLP_LED_CAPS_ON_OFF | ONLP_LED_CAPS_ORANGE,
    },
    {
        { ONLP_LED_ID_CREATE(LED_STATE), "Chassis LED 2 (STATE LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
        ONLP_LED_CAPS_RED | ONLP_LED_CAPS_BLUE | ONLP_LED_CAPS_GREEN,
    },
    {
        { ONLP_LED_ID_CREATE(LED_ALARM), "Chassis LED 3 (ALARM LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
        ONLP_LED_CAPS_RED | ONLP_LED_CAPS_BLUE | ONLP_LED_CAPS_GREEN,
    },
    {
        { ONLP_LED_ID_CREATE(LED_PSU1), "Chassis LED 4 (PSU1 LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
        ONLP_LED_CAPS_AUTO,
    },
    {
        { ONLP_LED_ID_CREATE(LED_PSU2), "Chassis LED 5 (PSU2 LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
       ONLP_LED_CAPS_AUTO,
    },
    {
        { ONLP_LED_ID_CREATE(LED_FAN), "Chassis LED 6 (FAN LED)", 0 },
        ONLP_LED_STATUS_PRESENT,
        ONLP_LED_CAPS_AUTO,
    },
};

static int driver_to_onlp_led_mode(enum onlp_led_id id, enum led_light_mode driver_led_mode)
{
    int i, nsize = sizeof(led_map)/sizeof(led_map[0]);
    
    for (i = 0; i < nsize; i++)
    {
        if (id == led_map[i].id && driver_led_mode == led_map[i].driver_led_mode)
        {
            return led_map[i].onlp_led_mode;
        }
    }
    
    return 0;
}

static int onlp_to_driver_led_mode(enum onlp_led_id id, onlp_led_mode_t onlp_led_mode)
{
    int i, nsize = sizeof(led_map)/sizeof(led_map[0]);
    
    for(i = 0; i < nsize; i++)
    {
        if (id == led_map[i].id && onlp_led_mode == led_map[i].onlp_led_mode)
        {
            return led_map[i].driver_led_mode;
        }
    }
    
    return 0;
}

/*
 * This function will be called prior to any other onlp_ledi_* functions.
 */
int
onlp_ledi_init(void)
{
    return ONLP_STATUS_OK;
}

int
onlp_ledi_info_get(onlp_oid_t id, onlp_led_info_t* info)
{
    int  local_id;
	char data[2] = {0};
    char fullpath[50] = {0};
		
    VALIDATE(id);
	
    local_id = ONLP_OID_ID_GET(id);
    
    /* get fullpath */
    sprintf(fullpath, "%s%s/%s", prefix_path, leds[local_id], filename);
		
	/* Set the onlp_oid_hdr_t and capabilities */
    *info = linfo[ONLP_OID_ID_GET(id)];

    /* Set LED mode */
    if (onlp_file_read_string(fullpath, data, sizeof(data), 0) != 0) {
        DEBUG_PRINT("%s(%d)\r\n", __FUNCTION__, __LINE__);
        return ONLP_STATUS_E_INTERNAL;
    }

    info->mode = driver_to_onlp_led_mode(local_id, atoi(data));

    /* Set the on/off status */
    if (info->mode != ONLP_LED_MODE_OFF) {
        info->status |= ONLP_LED_STATUS_ON;
    }

    return ONLP_STATUS_OK;
}

/*
 * Turn an LED on or off.
 *
 * This function will only be called if the LED OID supports the ONOFF
 * capability.
 *
 * What 'on' means in terms of colors or modes for multimode LEDs is
 * up to the platform to decide. This is intended as baseline toggle mechanism.
 */
int
onlp_ledi_set(onlp_oid_t id, int on_or_off)
{
    VALIDATE(id);

    if (!on_or_off) {
        return onlp_ledi_mode_set(id, ONLP_LED_MODE_OFF);
    }

    return ONLP_STATUS_E_UNSUPPORTED;
    
}

/*
 * This function puts the LED into the given mode. It is a more functional
 * interface for multimode LEDs.
 *
 * Only modes reported in the LED's capabilities will be attempted.
 */
int
onlp_ledi_mode_set(onlp_oid_t id, onlp_led_mode_t mode)
{
    int  local_id;
    char fullpath[50] = {0};		

    VALIDATE(id);
	
    local_id = ONLP_OID_ID_GET(id);
    sprintf(fullpath, "%s%s/%s", prefix_path, leds[local_id], filename);	
    
    if (onlp_file_write_integer(fullpath, onlp_to_driver_led_mode(local_id, mode)) != 0)
    {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}

/*
 * Generic LED ioctl interface.
 */
int
onlp_ledi_ioctl(onlp_oid_t id, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}
