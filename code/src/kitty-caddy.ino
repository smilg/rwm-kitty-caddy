#include <AccelStepper.h>
#include <uRTCLib.h>
#include <U8g2lib.h>
#include <EEPROM.h>

// pin definitions
#define INTERRUPT_PIN 2

#define BTN_UP 8
#define BTN_SEL 7
#define BTN_DOWN 6

#define DEBOUNCE_TIME 100

#define STEP_PIN 3
#define STEPPER_EN_PIN 4
#define DIR_PIN 9
#define MSTEP0_PIN 12
#define MSTEP1_PIN 11
#define MSTEP2_PIN 10

#define SENSOR_ACTIVE_PIN 13
#define SENSOR_IN_PIN A0

// sensor threshold for being in the center of the white mark
#define SENSOR_HIGH_THRESHOLD 1000
#define SENSOR_LOW_THRESHOLD 400

#define SPLASH_SCREEN_WIDTH 128
#define SPLASH_SCREEN_HEIGHT 64
static unsigned char splash_screen_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0E, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 
  0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xA0, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x23, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 
  0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xE0, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xE0, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x00, 0x30, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 
  0x1C, 0x3C, 0x07, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x60, 0x00, 0x00, 
  0x8C, 0x01, 0x00, 0x00, 0x1C, 0x0E, 0x03, 0x03, 0x03, 0x00, 0x00, 0x06, 
  0x00, 0xC0, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x1C, 0x0E, 0x80, 0x83, 
  0x03, 0x00, 0x00, 0x02, 0x00, 0x80, 0x01, 0x00, 0x04, 0x02, 0x00, 0x00, 
  0x1C, 0x07, 0x80, 0x83, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 
  0x04, 0x0C, 0x00, 0x00, 0x9C, 0x03, 0xE7, 0xFF, 0xFF, 0xC0, 0x81, 0x81, 
  0x01, 0x00, 0x03, 0x00, 0x04, 0x18, 0x00, 0x00, 0xDC, 0x01, 0xE7, 0xFF, 
  0xDF, 0xE1, 0xC0, 0x70, 0x00, 0x00, 0x06, 0x00, 0x0C, 0x30, 0x00, 0x00, 
  0xFC, 0x01, 0x87, 0x83, 0xC3, 0xE1, 0x40, 0x0C, 0x00, 0x00, 0x0C, 0x00, 
  0x38, 0x20, 0x00, 0x00, 0xFC, 0x01, 0x87, 0x83, 0x83, 0x61, 0x40, 0x00, 
  0x00, 0x00, 0x18, 0x00, 0x60, 0x60, 0x00, 0x00, 0xFC, 0x03, 0x87, 0x83, 
  0x83, 0x63, 0x40, 0x1C, 0x00, 0x00, 0x10, 0x00, 0xC0, 0x40, 0x00, 0x00, 
  0x3C, 0x07, 0x87, 0x83, 0x83, 0x73, 0x40, 0xE2, 0x01, 0x00, 0x30, 0x00, 
  0x80, 0x41, 0x00, 0x00, 0x1C, 0x07, 0x87, 0x83, 0x03, 0x33, 0x40, 0x00, 
  0x00, 0x00, 0x60, 0x00, 0x00, 0x41, 0x00, 0x00, 0x1C, 0x0E, 0x87, 0x83, 
  0x03, 0x37, 0x80, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x43, 0x00, 0x00, 
  0x1C, 0x0E, 0x87, 0x83, 0x03, 0x3E, 0x80, 0x01, 0x00, 0x00, 0x80, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x1C, 0x1C, 0x87, 0x83, 0x03, 0x1E, 0x00, 0x03, 
  0x00, 0x00, 0x80, 0x01, 0x00, 0x82, 0x00, 0x00, 0x1C, 0x1C, 0x07, 0x1F, 
  0x1F, 0x1E, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x03, 0x00, 0x82, 0x00, 0x00, 
  0x1C, 0x38, 0x07, 0x1E, 0x1E, 0x1C, 0x00, 0x30, 0x00, 0x00, 0x00, 0x06, 
  0x00, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x60, 
  0x00, 0x00, 0x00, 0x0C, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0E, 0x00, 0x80, 0x01, 0x00, 0x00, 0x08, 0x00, 0x04, 0x02, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x00, 0x10, 
  0x00, 0x04, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x07, 0x00, 0x00, 
  0x06, 0x00, 0x00, 0x30, 0x00, 0x08, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xC0, 0x03, 0x00, 0x00, 0x04, 0x00, 0x00, 0x60, 0x00, 0x18, 0x18, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xC0, 
  0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x08, 0x00, 0x00, 0x80, 0x01, 0xC0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x03, 0x80, 0x61, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
  0x06, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x10, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x84, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
  0x70, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x0E, 0x00, 
  0x20, 0x02, 0x00, 0x00, 0xE0, 0x01, 0x18, 0x01, 0xC0, 0x07, 0x00, 0x00, 
  0x38, 0x00, 0x0E, 0x00, 0x20, 0x06, 0x00, 0x00, 0x00, 0x03, 0x10, 0x02, 
  0xF0, 0x1F, 0x00, 0x00, 0x38, 0x00, 0x0E, 0x00, 0x20, 0x04, 0x00, 0x00, 
  0x00, 0x06, 0x10, 0x06, 0x78, 0x0C, 0x00, 0x00, 0x38, 0x00, 0x0E, 0x00, 
  0x20, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x10, 0x04, 0x1C, 0x00, 0x00, 0x00, 
  0x38, 0x00, 0x0E, 0x00, 0x20, 0x04, 0x00, 0x00, 0x00, 0x18, 0x10, 0x04, 
  0x1C, 0x00, 0x7C, 0xC0, 0x3B, 0xF0, 0xEE, 0xC0, 0x21, 0x04, 0x00, 0x00, 
  0x00, 0x10, 0x10, 0x08, 0x0E, 0x00, 0xFF, 0xE0, 0x3F, 0xF8, 0xCF, 0xE1, 
  0x60, 0x04, 0x00, 0x00, 0x00, 0x10, 0x10, 0x08, 0x0E, 0x00, 0xE3, 0x70, 
  0x3C, 0x1C, 0xCF, 0xE1, 0x40, 0x04, 0x01, 0x00, 0x00, 0x30, 0x10, 0x08, 
  0x0E, 0x00, 0xC0, 0x39, 0x38, 0x0E, 0x8E, 0x61, 0x40, 0x04, 0x01, 0x00, 
  0x00, 0x20, 0x10, 0x08, 0x0E, 0x00, 0xF8, 0x39, 0x38, 0x0E, 0x8E, 0x63, 
  0x40, 0x04, 0x03, 0x38, 0x00, 0x20, 0x10, 0x10, 0x0E, 0x00, 0xFE, 0x39, 
  0x38, 0x0E, 0x8E, 0x73, 0x40, 0x04, 0x03, 0x04, 0x00, 0x40, 0x08, 0x10, 
  0x0E, 0x00, 0xCF, 0x39, 0x38, 0x0E, 0x0E, 0x33, 0x40, 0x04, 0x05, 0x02, 
  0x00, 0x40, 0x0C, 0x10, 0x1C, 0x80, 0xC3, 0x39, 0x38, 0x0E, 0x0E, 0x37, 
  0x40, 0x04, 0x05, 0x02, 0x00, 0x40, 0x06, 0x08, 0x1C, 0x88, 0xC3, 0x39, 
  0x38, 0x0E, 0x0E, 0x3E, 0x40, 0x04, 0x0D, 0x01, 0x00, 0xC0, 0x03, 0x08, 
  0x38, 0x9C, 0xE3, 0x71, 0x3C, 0x1C, 0x0F, 0x1E, 0x40, 0x04, 0x09, 0x01, 
  0x00, 0x40, 0x00, 0x0C, 0xF0, 0x1F, 0xFF, 0xF1, 0x3F, 0xFC, 0x0F, 0x1E, 
  0x40, 0x04, 0x09, 0x01, 0x00, 0xC0, 0x00, 0x04, 0xC0, 0x07, 0xDE, 0xC1, 
  0x3B, 0xF0, 0x0E, 0x1C, 0x40, 0x04, 0x11, 0x01, 0x00, 0x80, 0x00, 0x02, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 0x04, 0x71, 0x01, 
  0x00, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 
  0x40, 0x04, 0xE1, 0x01, 0x00, 0x80, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x40, 0x04, 0x11, 0x01, 0x00, 0x80, 0x3E, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x07, 0x40, 0x84, 0x18, 0x01, 
  0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 
  0x60, 0x84, 0x8C, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x10, 0xC2, 0x66, 0x00, 0x00, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x42, 0x13, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x10, 0xC3, 0x09, 0x00, 0xC0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x10, 0xA1, 0x09, 0x00, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1E, 0xFF, 0xFF, 
  0x1F, 0x00, 0x00, 0x00, };

#define ARROW_WIDTH 9
#define ARROW_HEIGHT 6

static unsigned char up_arrow_solid_bits[] = {
    0x10, 0x00, 0x38, 0x00, 0x7C, 0x00, 0xFE, 0x00, 0xFF, 0x01, 0xFF, 0x01, 
    };

static unsigned char up_arrow_outline_bits[] = {
    0x10, 0x00, 0x28, 0x00, 0x44, 0x00, 0x82, 0x00, 0x01, 0x01, 0xFF, 0x01, 
    };

static unsigned char down_arrow_solid_bits[] = {
    0xFF, 0x01, 0xFF, 0x01, 0xFE, 0x00, 0x7C, 0x00, 0x38, 0x00, 0x10, 0x00, 
    };

static unsigned char down_arrow_outline_bits[] = {
    0xFF, 0x01, 0x01, 0x01, 0x82, 0x00, 0x44, 0x00, 0x28, 0x00, 0x10, 0x00, 
    };

AccelStepper stepper(1, STEP_PIN, DIR_PIN);
// SpeedyStepper stepper;

uRTCLib rtc;

char time_buffer[16];
char date_buffer[16];
char feedtime_buffer[9];
char next_feed_str[20];
const char NEXT_MEAL_PREFIX[] = "Next meal:";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

uint8_t current_screen = 0;
uint8_t current_option_selection = 1;

uint8_t minute_choice = 0;
uint8_t hour_choice = 1;
bool pm_chosen = false;
char ampm_choice[3];
char time_disp_buffer[3];

uint8_t selection_field_id = 0;
bool field_selected = false;

char selected_time[9];

const char *OPTION_LIST =
    "Set current time\n"
    "Set feeding time\n"
    "Rotate trays\n"
    "Back";

bool fed_today = false;
uint8_t current_day;

struct FeedTime
{
    uint8_t hour;
    uint8_t minute;
};
FeedTime feedtime;

void SaveFeedTimeEEPROM(FeedTime ft) {
    EEPROM.put(0, ft);
}

void GetFeedTimeEEPROM() {
    EEPROM.get(0, feedtime);
    uint8_t hour = feedtime.hour;

    bool pm = Convert24HrTimeTo12Hr(&hour);

    // set minute to default value of 0 if invalid
    if (feedtime.minute > 59) {
        feedtime.minute = 0;
    }

    char *ampm = "AM";
    if (pm) {
        ampm = "PM";
    }

    sprintf(feedtime_buffer, "%u:%02u %s", hour, feedtime.minute, ampm);
    sprintf(next_feed_str, "%s%s", NEXT_MEAL_PREFIX, feedtime_buffer);
}

void SetFeedTime(uint8_t hour, uint8_t minute, bool pm) {
    feedtime.hour = Convert12HrTimeTo24Hr(hour, pm);
    feedtime.minute = minute;

    SaveFeedTimeEEPROM(feedtime);
    GetFeedTimeEEPROM();
    fed_today = false;
}

/**
 * Convert an hour from 0-23 to an hour from 1-12, and return a boolean representing AM/PM.
 * The hour is modified in place, not returned from the function.
 * 
 * @param hour a pointer to an hour value.
 * @return a boolean set to true if PM, false if AM
 */
bool Convert24HrTimeTo12Hr(uint8_t* hour) {
    // set to default value of noon if time is invalid
    if (*hour > 23) {
        *hour = 12;
        return true;
    }
    // PM
    if (*hour > 11) {
        if (*hour > 12) {
            *hour -= 12;
        }
        return true;
    } else if (*hour == 0) {    // midnight
        *hour = 12;
    }
    return false;
}

uint8_t Convert12HrTimeTo24Hr(uint8_t hour, bool pm) {
    if (hour == 12) {
        if (!pm) {
            hour = 0;
        }
    } else if (pm) {
        hour += 12;
    }
    return hour;
}

void UpdateTime() {
    rtc.refresh();
    uint8_t hour = rtc.hour();
    char *ampm = "AM";
    bool pm = Convert24HrTimeTo12Hr(&hour);
    if (pm) {
        ampm = "PM";
    }

    sprintf(time_buffer, "%u:%02u:%02u %s", hour, rtc.minute(), rtc.second(), ampm);
    sprintf(date_buffer, "%u/%u/%u",  rtc.month(), rtc.day(), rtc.year());
}

void SetRTCTime(uint8_t hour, uint8_t minute, bool pm) {
    rtc.set(0, minute, Convert12HrTimeTo24Hr(hour, pm), rtc.dayOfWeek(), rtc.day(), rtc.month(), rtc.year());
    UpdateTime();
    fed_today = false;
}

void HomeTrays() {
    digitalWrite(SENSOR_ACTIVE_PIN, HIGH);
    delay(15);
    
    stepper.setMaxSpeed(2000);
    stepper.setAcceleration(5000);
    // roughly move to mark
    stepper.move(9000);
    while(analogRead(SENSOR_IN_PIN) < SENSOR_LOW_THRESHOLD) {
        stepper.run();
    }
    stepper.stop();
    stepper.runToPosition();
    stepper.setMaxSpeed(400);
    if (analogRead(SENSOR_IN_PIN) > SENSOR_LOW_THRESHOLD) {
        // if the sensor is on the mark, move forward slowly to find the edge of the mark
        stepper.move(4000);
        while(analogRead(SENSOR_IN_PIN) > SENSOR_LOW_THRESHOLD) {
            stepper.run();
        }
        stepper.stop();
        stepper.runToPosition();
    } else {
        // if we overshot the mark, move backward slowly to find the edge of the mark
        stepper.move(-4000);
        while(analogRead(SENSOR_IN_PIN) < SENSOR_LOW_THRESHOLD) {
            stepper.run();
        }
        stepper.stop();
        stepper.runToPosition();
    }
    // move from the edge of the mark to the center of the mark
    stepper.move(-200);
    stepper.runToPosition();

    digitalWrite(SENSOR_ACTIVE_PIN, LOW);
    current_screen = 0;
}

void RotateTrays() {
    stepper.setMaxSpeed(4000);
    stepper.setAcceleration(4000);
    stepper.move(8400);
    stepper.runToPosition();
}

void MainMenu() {
    u8g2.setFont(u8g2_font_8x13_tr);
    current_option_selection = u8g2.userInterfaceSelectionList(
        "Settings",
        current_option_selection, 
        OPTION_LIST);
    switch (current_option_selection) {
        case 1: // set current time
            // reset selections
            field_selected = false;
            selection_field_id = 0;
            
            // set starting time to current time
            hour_choice = rtc.hour();
            minute_choice = rtc.minute();
            pm_chosen = Convert24HrTimeTo12Hr(&hour_choice);
            
            // change screen
            current_screen = 2;
            break;
        case 2: // set feeding time
            // reset selections
            field_selected = false;
            selection_field_id = 0;

            // set starting time to current feed time
            hour_choice = feedtime.hour;
            minute_choice = feedtime.minute;
            pm_chosen = Convert24HrTimeTo12Hr(&hour_choice);
            
            // change screen
            current_screen = 3;
            break;
        case 3: // rotate trays
            RotateTrays();
        case 4: // exit
            current_screen = 0;
        default:
            break;
    }
}

void SetTimeMenu(char* title, void (*func)(uint8_t, uint8_t, bool)) {
    if (pm_chosen) {
        strcpy(ampm_choice, "PM");
    } else {
        strcpy(ampm_choice, "AM");
    }
    uint8_t max_field = 4;
    switch (u8g2.getMenuEvent()) {
        case U8X8_MSG_GPIO_MENU_SELECT:
            if (selection_field_id < 3) { // button not hovered
                // toggle selection state i.e. select or deselect the current input field
                field_selected = !field_selected;
            } else if (selection_field_id == 3) {   // save button
                func(hour_choice, minute_choice, pm_chosen); // save time
                current_screen = 0; // exit to menu
            } else if (selection_field_id == 4) {   // cancel button
                // exit to menu
                current_screen = 0;
            }
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            if (field_selected) {
                switch (selection_field_id) {
                    case 0: // decrement hour
                        if (hour_choice > 1) {
                            hour_choice--;
                        } else {
                            hour_choice = 12;
                        }
                        break;
                    case 1: // decrement minute
                        if (minute_choice > 0) {
                            minute_choice--;
                        } else {
                            minute_choice = 59;
                        }
                        break;
                    case 2: // am/pm toggle
                        pm_chosen = !pm_chosen;
                        break;
                    default:
                        break;
                }
            } else {
                if (selection_field_id > 3) { // overflow to select first field
                    selection_field_id = 0;
                } else {
                    selection_field_id++;   // or increment to next field
                }
            }
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            if (field_selected) {
                switch (selection_field_id) {
                    case 0: // increment hour
                        if (hour_choice < 12) {
                            hour_choice++;
                        } else {
                            hour_choice = 1;
                        }
                        break;
                    case 1: // increment minute
                        if (minute_choice < 59) {
                            minute_choice++;
                        } else {
                            minute_choice = 0;
                        }
                        break;
                    case 2: // am/pm toggle
                        pm_chosen = !pm_chosen;
                        break;
                    default:
                        break;
                }
            } else {
                if (selection_field_id < 1) { // overflow to select last field
                    selection_field_id = 4;
                } else {
                    selection_field_id--;   // or decrement to previous field
                }
            }
            break;
        default:
            break;
    }
    sprintf(selected_time, "%02u:%02u %s", hour_choice, minute_choice, ampm_choice);
    
    u8g2.setFont(u8g2_font_6x13_tr);
    u8g2.drawStr((SCREEN_WIDTH-u8g2.getStrWidth(title))/2, 10, title);

    uint8_t save_button_flags = U8G2_BTN_HCENTER|U8G2_BTN_BW1;
    uint8_t cancel_button_flags = U8G2_BTN_HCENTER|U8G2_BTN_BW1;

    // invert button if hovered 
    if (selection_field_id == 3) {
        save_button_flags |= U8G2_BTN_INV;
    } else if (selection_field_id == 4) {
        cancel_button_flags |= U8G2_BTN_INV;
    }

    u8g2.drawButtonUTF8(40,58,save_button_flags, 0, 2, 1, "Save");  // id 3
    u8g2.drawButtonUTF8(88,58,cancel_button_flags, 0, 2, 1, "Cancel");    // id 4

    u8g2.setFont(u8g2_font_9x15_mr);
    u8g2.drawStr((SCREEN_WIDTH-72)/2, 34, selected_time);

    u8g2.drawXBM(32,15,ARROW_WIDTH, ARROW_HEIGHT, up_arrow_outline_bits);
    u8g2.drawXBM(32,37,ARROW_WIDTH, ARROW_HEIGHT, down_arrow_outline_bits);
    u8g2.drawXBM(60,15,ARROW_WIDTH, ARROW_HEIGHT, up_arrow_outline_bits);
    u8g2.drawXBM(60,37,ARROW_WIDTH, ARROW_HEIGHT, down_arrow_outline_bits);

    if (field_selected) {
        u8g2.setDrawColor(2);
        switch (selection_field_id) {
            case 0:
                u8g2.drawBox(27, 22, 20, 14);
                u8g2.drawXBM(32,15,ARROW_WIDTH, ARROW_HEIGHT, up_arrow_solid_bits);
                u8g2.drawXBM(32,37,ARROW_WIDTH, ARROW_HEIGHT, down_arrow_solid_bits);
                break;
            case 1:
                u8g2.drawBox(54, 22, 20, 14);
                u8g2.drawXBM(60,15,ARROW_WIDTH, ARROW_HEIGHT, up_arrow_solid_bits);
                u8g2.drawXBM(60,37,ARROW_WIDTH, ARROW_HEIGHT, down_arrow_solid_bits);
                break;
            case 2:
                u8g2.drawBox(81, 22, 20, 14);
                break;
            default:
                break;
        }
        u8g2.setDrawColor(1);
    } else {
        switch (selection_field_id) {
            case 0:
                u8g2.drawFrame(27, 22, 20, 14);
                break;
            case 1:
                u8g2.drawFrame(54, 22, 20, 14);
                break;
            case 2:
                u8g2.drawFrame(81, 22, 20, 14);
                break;
            default:
                break;
        }
    }
}

void IdleScreen() {
    if (!u8g2.getMenuEvent()) {
        u8g2.setFont(u8g2_font_8x13_tr);
        uint8_t line_height = u8g2.getAscent()+2;
        line_height -= u8g2.getDescent();
        u8g2.drawStr((SCREEN_WIDTH-u8g2.getStrWidth("Kitty Caddy"))/2, u8g2.getAscent(), "Kitty Caddy");
        u8g2.drawLine(0, 12, 128, 12);
        u8g2.drawStr((SCREEN_WIDTH-u8g2.getStrWidth(time_buffer))/2, line_height+14, time_buffer);

        u8g2.drawStr((SCREEN_WIDTH-u8g2.getStrWidth(NEXT_MEAL_PREFIX))/2, line_height*2+14, NEXT_MEAL_PREFIX);
        u8g2.drawStr((SCREEN_WIDTH-u8g2.getStrWidth(feedtime_buffer))/2, line_height*3+14, feedtime_buffer);
    } else {
        current_screen = 1;
    }
}

void CheckFeedTime() {
    if (current_day != rtc.day()) {
        fed_today = false;
        current_day = rtc.day();
    }
    if (!fed_today && feedtime.hour == rtc.hour() && feedtime.minute == rtc.minute()) {
        RotateTrays();
        fed_today = true;
    }
}

void setup() {
    // display init
    u8g2.begin(BTN_SEL, BTN_DOWN, BTN_UP);
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.setFontMode(1);

    // splash screen
    u8g2.firstPage();
    do {
        u8g2.drawXBM(0,0,SPLASH_SCREEN_WIDTH,SPLASH_SCREEN_HEIGHT,splash_screen_bits);
    } while(u8g2.nextPage());

    // stepper motor pins
    pinMode(STEPPER_EN_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(MSTEP0_PIN, OUTPUT);
    pinMode(MSTEP1_PIN, OUTPUT);
    pinMode(MSTEP2_PIN, OUTPUT);

    // button pins
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_SEL, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);

    // homing sensor pin
    pinMode(SENSOR_ACTIVE_PIN, OUTPUT);
    digitalWrite(SENSOR_ACTIVE_PIN, LOW);

    /**
     * microstepping setup - 1/8 step
     * M0   |M1 |M2 |Resolution
     * 0    |0  |0  |1 step
     * 1    |0  |0  |1/2 step
     * 0	|1  |0  |1/4 step
     * 1	|1	|0	|1/8 step
     * 0	|0	|1  |1/16 step
     * 1	|0  |1  |1/32 step
     * 0	|1	|1  |1/32 step
     * 1	|1	|1  |1/32 step
     **/
    digitalWrite(MSTEP0_PIN, HIGH);
    digitalWrite(MSTEP1_PIN, HIGH);
    digitalWrite(MSTEP2_PIN, LOW);
    digitalWrite(STEPPER_EN_PIN, LOW);

    // rtc init
    URTCLIB_WIRE.begin();
    rtc.set_rtc_address(0x68);
	rtc.set_model(URTCLIB_MODEL_DS3231);
    rtc.enableBattery();

    UpdateTime();
    GetFeedTimeEEPROM();

    current_day = rtc.day();
    HomeTrays();
}

void loop() {
    UpdateTime();
    CheckFeedTime();
    u8g2.firstPage();
    do {
        switch (current_screen) {
        case 1:
            MainMenu();
            break;
        case 2:
            SetTimeMenu("Set Current Time", &SetRTCTime);
            break;
        case 3:
            SetTimeMenu("Set Feed Time", &SetFeedTime);
            break;
        default:
            IdleScreen();
            break;
        }
    } while (u8g2.nextPage());
}