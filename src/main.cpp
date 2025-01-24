#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include "FireTimer.h"

// #include <ezButton.h>

#include <UI/ui.h>
#include <UI/vars.h>
#include <UI/actions.h>

/*=========================
   USEFUL RESOURCES
 *=========================*/

//https://github.com/ffodGit/esp32-8048s043-getting-started-00
//https://www.instructables.com/Value-Your-Project-Use-Graphic-Display/

// put function declarations here:
int myFunction(int, int);
void LoopArc();
void IncrementArcWithEncoder();


//Encoder
#define Encoder_CLK   14
#define Encoder_DT 12
#define Encoder_Switch 27

int Previous_Output;
int Encoder_Count;

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

//Display - 1.8 inch TFT LCD module SPI
#define DISPLAY_HORZ_RES 128
#define DISPLAY_VERT_RES 160

// #define TFT_MOSI 23
// #define TFT_SCLK 18
// #define TFT_CS   5
// #define TFT_DC   2
// #define TFT_RST  4

//scherm gebruikt misschien stiekem de ST7789 driver ipv 7735

// int counter = 0;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;

// ezButton encoder_button(Encoder_Switch);

FireTimer ArcIncrementTimer;
bool IncrementArcPositiveFlag = true;

static const uint16_t screenWidth  = 128;
static const uint16_t screenHeight = 160;

// static lv_disp_draw_buf_t draw_buf;
/* LVGL draws into this buffer, 1/10 screen size usually works well. The size is in bytes. */
#define DRAW_BUF_SIZE (screenWidth * screenHeight / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);



/* Display flushing */
void my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    // if (LV_COLOR_16_SWAP) {
    //     size_t len = lv_area_get_size( area );
    //     lv_draw_sw_rgb565_swap( pixelmap, len );
    // }

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( (uint16_t*) pixelmap, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}


void setup() {

  //Begin serial communication
  String LVGL_Arduino = "Hello Arduino ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);
  
  //pin Mode declaration
  pinMode (Encoder_DT, INPUT_PULLDOWN);
  pinMode (Encoder_CLK, INPUT_PULLDOWN);
  pinMode (Encoder_Switch, INPUT_PULLUP);

  Previous_Output = digitalRead(!Encoder_DT); //Read the inital value of Output A
  // encoder_button.setDebounceTime(50);  // set debounce time to 50 milliseconds


  //Initialize SPI connection with TFT screen
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,4);
  tft.setTextColor(TFT_WHITE);
  // tft.println ("Hello World!");

  tft.println("test");
  tft.println("test");
  tft.println("test");
  tft.println("test");
  tft.println("test");
  tft.println("test");
  tft.println("test");
  delay(1000);
  Serial.println("Several test messages should have been printed!");
  tft.fillScreen(TFT_BLACK);


  lv_init();  
  
  /* Set a tick source so that LVGL will know how much time elapsed. */
  lv_tick_set_cb((lv_tick_get_cb_t)millis);
  
  // Create display
  lv_display_t * display1 = lv_display_create(screenWidth, screenHeight);
  lv_display_set_flush_cb(display1, my_disp_flush);
  lv_display_set_buffers(display1, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  
  Serial.println("Setup done");


  ui_init();

  ArcIncrementTimer.begin(10);


}

void loop() {
  // encoder_button.loop();  // MUST call the loop() function first
  lv_timer_handler();
  // Update EEZ-Studio UI
  ui_tick();
  // delay(1000);

  // if (ArcIncrementTimer.fire()){
      // LoopArc();
  // }

  // IncrementArcWithEncoder();

   if (digitalRead(Encoder_DT) != Previous_Output)
  {
    if (digitalRead(Encoder_CLK) != Previous_Output)
    {
      Encoder_Count ++;
      set_var_arc_value(get_var_arc_value() + 1);
      Serial.println(Encoder_Count);
    }
    else
    {
      Encoder_Count--;
      set_var_arc_value(get_var_arc_value() - 1);
      Serial.println(Encoder_Count);
    }
  }

  Previous_Output = digitalRead(Encoder_DT);

  if (digitalRead(Encoder_Switch) == 0)
  {
    delay(5);
    if (digitalRead(Encoder_Switch) == 0) {
      Serial.println("Switch pressed");
      Serial.print("Current state of arc_value: ");
      Serial.println(get_var_arc_value());
      while (digitalRead(Encoder_Switch) == 0);
    }
  }

}



/*=========================
   FUNCTION DECLARATIONS
 *=========================*/
int myFunction(int x, int y) {
  return x + y;
}

// Test function to automatically increment and decrement the value for the arc on a timer
void LoopArc(){

  if (IncrementArcPositiveFlag){
    if (get_var_arc_value() < 100){
      set_var_arc_value(get_var_arc_value() +1);
    }
    
    if (get_var_arc_value() == 100){
      IncrementArcPositiveFlag = false;
    }
  }else {
    if (get_var_arc_value() > 0){
      set_var_arc_value(get_var_arc_value() - 1);
    }
    if (get_var_arc_value() == 0){
      IncrementArcPositiveFlag = true;
    }
    
  }
}

// // Test function to increment and decrement the value of the arc with a rotary encoder
// void IncrementArcWithEncoder(){
//     // read the current state of the rotary encoder's CLK pin
//   CLK_state = digitalRead(Encoder_CLK  );

//   // If the state of CLK is changed, then pulse occurred
//   // React to only the rising edge (from LOW to HIGH) to avoid double count
//   if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
//     // if the DT state is HIGH
//     // the encoder is rotating in counter-clockwise direction => decrease the counter
//     if (digitalRead(Encoder_DT) == HIGH) {
//       direction = DIRECTION_CCW;
//       set_var_arc_value(get_var_arc_value() - 1);
//     } else {
//       // the encoder is rotating in clockwise direction => increase the counter
//       direction = DIRECTION_CW;
//       set_var_arc_value(get_var_arc_value() + 1);
//     }

//     prev_CLK_state = CLK_state;

//     if (encoder_button.isPressed()){
//       Serial.println("Encoder Button was pressed!");
//     }
//   }
// }