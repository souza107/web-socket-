//To enable Debuging set SP to 1
#define SP 1

#if SP == 1
#define sp(x) Serial.print(x);
#define spl(x) Serial.println(x);
#else
#define sp(x)
#define spl(x)

#endif
//-----------------------------------------------------------------


#include <WiFi.h>
#include <esp_now.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Ticker.h> //https://github.com/sstaub/Ticker
#include <ESPAsyncWebServer.h>

 #include <FastLED.h>
 // How many leds in your strip?
 #define NUM_LEDS 10
 #define DATA_PIN 33

 // Define the array of leds
 CRGB leds[NUM_LEDS];


void send_sensor();
void send_ShowTimeDelay();
void send_AudioTest();
void send_ContTest();
void send_syncMode ();


const int button = 26;
const int button2 = 27;

#define LED1 2
#define LED2 12
//Websocket Varriable Messages
int ShowTimeDelay = 0; // WebServer message of Time delay till show starts
 int SYNC_status = 0;
 int Audio_status = 0;
 int Cont_status = 0;
//ESP NOW SETUP =====================================================
int myID; // Setup Boards ID #
int Sent;

//--------------- ================== ESP NOW SETUP  ================== -------------------------
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
//--------------- ================== ESP NOW SETUP  ================== -------------------------
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0x08, 0x3A, 0xF2, 0x50, 0xA4, 0xF8}; //Web
//uint8_t broadcastAddress2[] = {0x84, 0xCC, 0xA8, 0x7E, 0xBE, 0x58}; //audio
    uint8_t broadcastAddress2[] = {0x8C, 0x4B, 0x14, 0x16, 0x0F, 0x58};

uint8_t broadcastAddress3[] = {0x84, 0xCC, 0xA8, 0x7E, 0xC4, 0x64}; //Board 3 - Receiver
uint8_t broadcastAddress4[] = {0x9C, 0x9C, 0x1F, 0xE3, 0x76, 0x50};//Board 4 - Receiver
  uint8_t broadcastAddress5[] = {0x84, 0xCC, 0xA8, 0x7E, 0xC3, 0x50}; //Board 5 - Receiver
  uint8_t broadcastAddress6[] = {0x9C, 0x9C, 0x1F, 0xC5, 0xA5, 0x80}; //Board 6 - Receiver===============================================================================

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
        int id;
        int x;
        unsigned long int timex;
        long cue_ck_1;
        long cue_ck_2;
        long cue_ck_3;
        long cue_ck_4;
        long cue_ck_5;
        long cue_ck_6;
        //Struct for  Message Board(b1) to Audiopboard to get showtime
        int gettime;
        int showtime;
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
        struct_message board1;
        struct_message board2;
        struct_message board3;
        struct_message board4;
        struct_message board5;
        struct_message board6;

// Create an array with all the structures
struct_message boardsStruct[6] = {board1, board2, board3, board4, board5, board6};
//================================================================================

      //// ----Message received from ESP NOW to confirm that messages have been received (added to verify)
      int board1X;
    
      int board2X;  
      int board3X;
      int board4X;
      int board5X;
      int board6X;
      int b1online;
      int board2time;
      
      //Cont Check MSG
        int board1cc1;  int board1cc2;  int board1cc3;  int board1cc4;  int board1cc5;  int board1cc6;
        int board2cc1;  int board2cc2;  int board2cc3;  int board2cc4;  int board2cc5;  int board2cc6;
        int board3cc1;  int board3cc2;  int board3cc3;  int board3cc4;  int board3cc5;  int board3cc6;
        int board4cc1;  int board4cc2;  int board4cc3;  int board4cc4;  int board4cc5;  int board4cc6;
        int board5cc1;  int board5cc2;  int board5cc3;  int board5cc4;  int board5cc5;  int board5cc6;
        int board6cc1;  int board6cc2;  int board6cc3;  int board6cc4;  int board6cc5;  int board6cc6;
//================================================================================

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
sp("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
 spl(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
        boardsStruct[myData.id-1].x = myData.x;
        //boardsStruct[myData.id-1].y = myData.y;
       // boardsStruct[myData.id-1].z = myData.z;
        boardsStruct[myData.id-1].cue_ck_1 = myData.cue_ck_1;
        boardsStruct[myData.id-1].cue_ck_2 = myData.cue_ck_2;
        boardsStruct[myData.id-1].cue_ck_3 = myData.cue_ck_3;
        boardsStruct[myData.id-1].cue_ck_4 = myData.cue_ck_4;
        boardsStruct[myData.id-1].cue_ck_5 = myData.cue_ck_5;
        boardsStruct[myData.id-1].cue_ck_6 = myData.cue_ck_6;
              boardsStruct[myData.id-1].timex = myData.timex;
              
    Serial.printf("showtime: %d \n", boardsStruct[myData.id-1].timex);

    Serial.printf("cue_ck_1 value: %d \n", boardsStruct[myData.id-1].cue_ck_1);
       Serial.printf("cue_ck_2 value: %d \n", boardsStruct[myData.id-1].cue_ck_2);        
 Serial.printf("cue_ck_3 value: %d \n", boardsStruct[myData.id-1].cue_ck_3);
       Serial.printf("cue_ck_4 value: %d \n", boardsStruct[myData.id-1].cue_ck_4);        
 Serial.printf("cue_ck_5 value: %d \n", boardsStruct[myData.id-1].cue_ck_5);
       Serial.printf("cue_ck_6 value: %d \n", boardsStruct[myData.id-1].cue_ck_6);        

 
    
     board1X =  boardsStruct[0].x; board2X = boardsStruct[1].x; board3X = boardsStruct[2].x; board4X = boardsStruct[3].x; board5X = boardsStruct[4].x; board6X = boardsStruct[5].x;
//     board1Y = boardsStruct[0].y;  board2Y = boardsStruct[1].y;
board2time = boardsStruct[1].timex;


     
    
    //Cont MSG
    
     board1cc1 = boardsStruct[0].cue_ck_1;  board2cc1 = boardsStruct[1].cue_ck_1;  board3cc1 = boardsStruct[2].cue_ck_1; board4cc1 = boardsStruct[3].cue_ck_1;  board5cc1 = boardsStruct[4].cue_ck_1;  board6cc1 = boardsStruct[5].cue_ck_1;
     board1cc2 = boardsStruct[0].cue_ck_2;  board2cc2 = boardsStruct[1].cue_ck_2;  board3cc2 = boardsStruct[2].cue_ck_2; board4cc2 = boardsStruct[3].cue_ck_2;  board5cc2 = boardsStruct[4].cue_ck_2;  board6cc2 = boardsStruct[5].cue_ck_2; 
     board1cc3 = boardsStruct[0].cue_ck_3;  board2cc3 = boardsStruct[1].cue_ck_3;  board3cc3 = boardsStruct[2].cue_ck_3; board4cc3 = boardsStruct[3].cue_ck_3;  board5cc3 = boardsStruct[4].cue_ck_3;  board6cc3 = boardsStruct[5].cue_ck_3;
     board1cc4 = boardsStruct[0].cue_ck_4;  board2cc4 = boardsStruct[1].cue_ck_4;  board3cc4 = boardsStruct[2].cue_ck_4; board4cc4 = boardsStruct[3].cue_ck_4;  board5cc4 = boardsStruct[4].cue_ck_4;  board6cc4 = boardsStruct[5].cue_ck_4;
     board1cc5 = boardsStruct[0].cue_ck_5;  board2cc5 = boardsStruct[1].cue_ck_5;  board3cc5 = boardsStruct[2].cue_ck_5; board4cc5 = boardsStruct[3].cue_ck_5;  board5cc5 = boardsStruct[4].cue_ck_5;  board6cc5 = boardsStruct[5].cue_ck_5;
     board1cc6 = boardsStruct[0].cue_ck_6;  board2cc6 = boardsStruct[1].cue_ck_6;  board3cc6 = boardsStruct[2].cue_ck_6; board4cc6 = boardsStruct[3].cue_ck_6;  board5cc6 = boardsStruct[4].cue_ck_6;  board6cc6 = boardsStruct[5].cue_ck_6;
//
   if(  board2X == 100){ leds[3] = CRGB::Green; FastLED.show();  } //board 2
      if(  board3X == 100){ leds[2] = CRGB::Green; FastLED.show();  } //board 3
         if(  board4X == 100){ leds[1] = CRGB::Green; FastLED.show();  } //board 4
            if(  board5X == 100){ leds[0] = CRGB::Green; FastLED.show();  } //board 5
               if(  board6X == 100){ leds[5] = CRGB::Green; FastLED.show();  } //board 6

               if (board2time > 1){
 
  myData.id = myID; // ID of this board set in the SETUP
  myData.x = 0;
  myData.timex = board2time; //Varriable to send to boards


esp_err_t result1 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 delay(500);

                
               }
} // ================================================================================

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  sp(macStr);
  sp(" send status:\t");
  spl(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Sent = status;
}//================================================================================

//END ESP NOW SETUP ---------------------------------------------------




Ticker timer;

char webpage[] PROGMEM = R"=====(

            <!DOCTYPE html>
           <html>

<script>

var connection = new WebSocket('ws://'+location.hostname+':81/');

var button_sync_status = 0;
var button_audio_status = 0;
var button_cont_status = 0;
var showstart = 0;
var showstartdelay = 0;
var board1= "0"; var board1c1= "0";  var board1c2= "0";  var board1c3= "0";  var board1c4= "0";  var board1c5= "0";  var board1c6= "0";
var board2= "0"; var board2c1= "0";  var board2c2= "0";  var board2c3= "0";  var board2c4= "0";  var board2c5= "0";  var board2c6= "0";
var board3= "0"; var board3c1= "0";  var board3c2= "0";  var board3c3= "0";  var board3c4= "0";  var board3c5= "0";  var board3c6= "0";
var board4= "0"; var board4c1= "0";  var board4c2= "0";  var board4c3= "0";  var board4c4= "0";  var board4c5= "0";  var board4c6= "0";
var board5= "0"; var board5c1= "0";  var board5c2= "0";  var board5c3= "0";  var board5c4= "0";  var board5c5= "0";  var board5c6= "0";
var board6= "0"; var board6c1= "0";  var board6c2= "0";  var board6c3= "0";  var board6c4= "0";  var board6c5= "0";  var board6c6= "0";

var board2Y = 0;





 var b2sync = "No";
 //Var for Board Modes
 var b1_status = "Offline";   var b2_status = "Offline";   var b3_status = "Offline";   var b4_status = "Offline";   var b5_status = "Offline";   var b6_status = "Offline";
    var b1_mode = "STAND BY";   var b2_mode = "STAND BY";   var b3_mode = "STAND BY";   var b4_mode = "STAND BY";   var b5_mode = "STAND BY";   var b6_mode = "STAND BY";
//Var for sync status
var b1sync = "no"; var b2sync = "no"; var b3sync = "no"; var b4sync = "no"; var b5sync = "no"; var b6sync = "no";
 //Var for Audio check only board 6
var audio_check = "Audio check - no";
//Var for Cont check b2-b6
var b2_cont = "no";  var b3_cont = "no";  var b4_cont = "no"; var b5_cont = "no"; var b1_cont = "no";
//var for show start time
var b1_gametime = "no"; var b2_gametime = "no"; var b3_gametime = "no"; var b4_gametime = "no"; var b5_gametime = "no"; var b6_gametime = "no";

 var c_stat;


connection.onmessage = function(event){

  var full_data = event.data;
  console.log(full_data);
  var data = JSON.parse(full_data);
  board1_data = data.board1;
 board2_data = data.board2;
 board2Y_data = data.board2Y;
  board3_data = data.board3;
   board4_data = data.board4;
    board5_data = data.board5;
     board6_data = data.board6;

         board1c1_data = data.board1c1;
         board1c2_data = data.board1c2;
         board1c3_data = data.board1c3;
         board1c4_data = data.board1c4;
         board1c5_data = data.board1c5;
         board1c6_data = data.board1c6;

         board2c1_data = data.board2c1;
         board2c2_data = data.board2c2;
         board2c3_data = data.board2c3;
         board2c4_data = data.board2c4;
         board2c5_data = data.board2c5;
         board2c6_data = data.board2c6;

         board3c1_data = data.board3c1;
         board3c2_data = data.board3c2;
         board3c3_data = data.board3c3;
         board3c4_data = data.board3c4;
         board3c5_data = data.board3c5;
         board3c6_data = data.board3c6;

         board4c1_data = data.board4c1;
         board4c2_data = data.board4c2;
         board4c3_data = data.board4c3;
         board4c4_data = data.board4c4;
         board4c5_data = data.board4c5;
         board4c6_data = data.board4c6;

         board5c1_data = data.board5c1;
         board5c2_data = data.board5c2;
         board5c3_data = data.board5c3;
         board5c4_data = data.board5c4;
         board5c5_data = data.board5c5;
         board5c6_data = data.board5c6;

         board6c1_data = data.board6c1;
         board6c2_data = data.board6c2;
         board6c3_data = data.board6c3;
         board6c4_data = data.board6c4;
         board6c5_data = data.board6c5;
         board6c6_data = data.board6c6;

//convert cont test to string and split
let brd1c1 = board1c1_data.toString(); let b1c1 = brd1c1.split(""); let brd1c2 = board1c2_data.toString(); let b1c2 = brd1c2.split(""); let brd1c3 = board1c3_data.toString(); let b1c3 = brd1c3.split("");  let brd1c4 = board1c4_data.toString(); let b1c4 = brd1c4.split("");   let brd1c5 = board1c5_data.toString(); let b1c5 = brd1c5.split("");let brd1c6 = board1c6_data.toString(); let b1c6 = brd1c6.split("");
let brd2c1 = board2c1_data.toString(); let b2c1 = brd2c1.split(""); let brd2c2 = board2c2_data.toString(); let b2c2 = brd2c2.split(""); let brd2c3 = board2c3_data.toString(); let b2c3 = brd2c3.split("");  let brd2c4 = board2c4_data.toString(); let b2c4 = brd2c4.split("");   let brd2c5 = board2c5_data.toString(); let b2c5 = brd2c5.split("");let brd2c6 = board2c6_data.toString(); let b2c6 = brd2c6.split("");
let brd3c1 = board3c1_data.toString(); let b3c1 = brd3c1.split(""); let brd3c2 = board3c2_data.toString(); let b3c2 = brd3c2.split(""); let brd3c3 = board3c3_data.toString(); let b3c3 = brd3c3.split("");  let brd3c4 = board3c4_data.toString(); let b3c4 = brd3c4.split("");   let brd3c5 = board3c5_data.toString(); let b3c5 = brd3c5.split("");let brd3c6 = board3c6_data.toString(); let b3c6 = brd3c6.split("");
let brd4c1 = board4c1_data.toString(); let b4c1 = brd4c1.split(""); let brd4c2 = board4c2_data.toString(); let b4c2 = brd4c2.split(""); let brd4c3 = board4c3_data.toString(); let b4c3 = brd4c3.split("");  let brd4c4 = board4c4_data.toString(); let b4c4 = brd4c4.split("");   let brd4c5 = board4c5_data.toString(); let b4c5 = brd4c5.split("");let brd4c6 = board4c6_data.toString(); let b4c6 = brd4c6.split("");
let brd5c1 = board5c1_data.toString(); let b5c1 = brd5c1.split(""); let brd5c2 = board5c2_data.toString(); let b5c2 = brd5c2.split(""); let brd5c3 = board5c3_data.toString(); let b5c3 = brd5c3.split("");  let brd5c4 = board5c4_data.toString(); let b5c4 = brd5c4.split("");   let brd5c5 = board5c5_data.toString(); let b5c5 = brd5c5.split("");let brd5c6 = board5c6_data.toString(); let b5c6 = brd5c6.split("");
let brd6c1 = board6c1_data.toString(); let b6c1 = brd6c1.split(""); let brd6c2 = board6c2_data.toString(); let b6c2 = brd6c2.split(""); let brd6c3 = board6c3_data.toString(); let b6c3 = brd6c3.split("");  let brd6c4 = board6c4_data.toString(); let b6c4 = brd6c4.split("");   let brd6c5 = board6c5_data.toString(); let b6c5 = brd6c5.split("");let brd6c6 = board6c6_data.toString(); let b6c6 = brd6c6.split("");

 //Board Online Message
 if (board1_data == 100){b1_status = "Online";}
 if (board2_data == 100){b2_status = "Online";}
 if (board3_data == 100){b3_status = "Online";}
 if (board4_data == 100){b4_status = "Online";}
 if (board5_data == 100){b5_status = "Online";}
 if (board6_data == 100){b6_status = "Online";}


//Board Mode update fro reply messages
      if (board1_data == 1){b1_mode = "Sync Mode";}  else if (board1_data == 12){b1_mode = "Cont Check";}  else if (board1_data == 13){b1_mode = "Audio Check";}  else if (board1_data == 4){b1_mode = "Time";}  else{b1_mode = "Stand By";}
      if (board2_data == 1){b2_mode = "Sync Mode";}  else if (board2_data == 12){b2_mode = "Cont Check";}  else if (board2_data == 13){b2_mode = "Audio Check";}  else if (board2_data == 4){b2_mode = "Time";}  else{b2_mode = "Stand By";}
      if (board3_data == 1){b3_mode = "Sync Mode";}  else if (board3_data == 12){b3_mode = "Cont Check";}  else if (board3_data == 13){b3_mode = "Audio Check";}  else if (board3_data == 4){b3_mode = "Time";}  else{b3_mode = "Stand By";}
      if (board4_data == 1){b4_mode = "Sync Mode";}  else if (board4_data == 12){b4_mode = "Cont Check";}  else if (board4_data == 13){b4_mode = "Audio Check";}  else if (board4_data == 4){b4_mode = "Time";}  else{b4_mode = "Stand By";}
      if (board5_data == 1){b5_mode = "Sync Mode";}  else if (board5_data == 12){b5_mode = "Cont Check";}  else if (board5_data == 13){b5_mode = "Audio Check";}  else if (board5_data == 4){b5_mode = "Time";}  else{b5_mode = "Stand By";}
      if (board6_data == 1){b6_mode = "Sync Mode";}  else if (board6_data == 12){b6_mode = "Cont Check";}  else if (board6_data == 13){b6_mode = "Audio Check";}  else if (board6_data == 4){b6_mode = "Time";}  else{b6_mode = "Stand By";}

//Board clock sync status
      if (board1_data == 2){b1sync = "Yes"; } if (board2_data == 2){b2sync = "Yes";}   if (board3_data == 2){b3sync = "Yes";}   if (board4_data == 2){b4sync = "Yes";}   if (board5_data == 2){b5sync = "Yes";}   if (board6_data == 2){b6sync = "Yes";}

//Cont Check for all boards except board 1 (audio board)
    if (board1_data == 3){b1_cont = "Yes";}  if (board2_data == 3){b2_cont = "Yes";}    if (board3_data == 3){b3_cont = "Yes";}     if (board4_data == 3){b4_cont = "Yes";}     if (board5_data == 3){b5_cont = "Yes";}

//Audio check status for board 1

 if (board6_data == 3){audio_check = "Audio Check - Yes";}

//SHow Start time received
 if (board1_data == 4){b1_gametime = "Game Time";}  if (board2_data == 4){b2_gametime = "Game Time";}   if (board3_data == 4){b3_gametime = "Game Time";}   if (board4_data == 4){b4_gametime = "Game Time";}   if (board5_data == 4){b5_gametime = "Game Time";}  if (board6_data == 4){b6_gametime = "Game Time";}


//Update dash board Mode
 document.getElementById("b1mode").innerHTML = b1_mode; document.getElementById("b2mode").innerHTML = b2_mode;  document.getElementById("b3mode").innerHTML = b3_mode;   document.getElementById("b4mode").innerHTML = b4_mode;  document.getElementById("b5mode").innerHTML = b5_mode;   document.getElementById("b6mode").innerHTML = b6_mode;

//Connection Status
 document.getElementById("b1status").innerHTML = b1_status; document.getElementById("b2status").innerHTML = b2_status;  document.getElementById("b3status").innerHTML = b3_status;   document.getElementById("b4status").innerHTML = b4_status;  document.getElementById("b5status").innerHTML = b5_status;   document.getElementById("b6status").innerHTML = b6_status;

 //Clock sync dash board update
  document.getElementById("b1sync").innerHTML = b1sync;  document.getElementById("b2sync").innerHTML = b2sync;   document.getElementById("b3sync").innerHTML = b3sync;  document.getElementById("b4sync").innerHTML = b4sync;    document.getElementById("b5sync").innerHTML = b5sync;     document.getElementById("b6sync").innerHTML = b6sync;

//Cont Check dash board update
document.getElementById("b1cont").innerHTML = b1_cont; document.getElementById("b2cont").innerHTML = b2_cont;  document.getElementById("b3cont").innerHTML = b3_cont;  document.getElementById("b4cont").innerHTML = b4_cont;  document.getElementById("b5cont").innerHTML = b5_cont;

//Audio check dash board only for board 1
document.getElementById("b6audio").innerHTML = audio_check;

//Show Start time dash board update
document.getElementById("b1time").innerHTML = b1_gametime;     document.getElementById("b2time").innerHTML = b2_gametime;    document.getElementById("b3time").innerHTML = b3_gametime;   document.getElementById("b4time").innerHTML = b4_gametime;   document.getElementById("b5time").innerHTML = b5_gametime;   document.getElementById("b6time").innerHTML = b6_gametime;

//Table 2


document.getElementById("b1c8").innerHTML = b1c1;  document.getElementById("b2c8").innerHTML = b2c1;  document.getElementById("b3c8").innerHTML = b3c1; document.getElementById("b4c8").innerHTML = b4c1; document.getElementById("b5c8").innerHTML = b5c1; document.getElementById("b6c8").innerHTML = b6c1;
document.getElementById("b1c16").innerHTML = b1c2;  document.getElementById("b2c16").innerHTML = b2c2;  document.getElementById("b3c16").innerHTML = b3c2; document.getElementById("b4c16").innerHTML = b4c2; document.getElementById("b5c16").innerHTML = b5c2; document.getElementById("b6c16").innerHTML = b6c2;
document.getElementById("b1c24").innerHTML = b1c3;  document.getElementById("b2c24").innerHTML = b2c3;  document.getElementById("b3c24").innerHTML = b3c3; document.getElementById("b4c24").innerHTML = b4c3; document.getElementById("b5c24").innerHTML = b5c3; document.getElementById("b6c24").innerHTML = b6c3;
document.getElementById("b1c32").innerHTML = b1c4;  document.getElementById("b2c32").innerHTML = b2c4;  document.getElementById("b3c32").innerHTML = b3c4; document.getElementById("b4c32").innerHTML = b4c4; document.getElementById("b5c32").innerHTML = b5c4; document.getElementById("b6c32").innerHTML = b6c4;
document.getElementById("b1c40").innerHTML = b1c5;  document.getElementById("b2c40").innerHTML = b2c5;  document.getElementById("b3c40").innerHTML = b3c5; document.getElementById("b4c40").innerHTML = b4c5; document.getElementById("b5c40").innerHTML = b5c5; document.getElementById("b6c40").innerHTML = b6c5;
document.getElementById("b1c48").innerHTML = b1c6;  document.getElementById("b2c48").innerHTML = b2c6;  document.getElementById("b3c48").innerHTML = b3c6; document.getElementById("b4c48").innerHTML = b4c6; document.getElementById("b5c48").innerHTML = b5c6; document.getElementById("b6c48").innerHTML = b6c6;





}

function button_sync()
{
   button_sync_status = 1;
  console.log("LED 1 is ON");

  send_data();
}

function button_Cont()
{
   button_cont_status = 1;
  console.log("LED 1 is ON");

  send_data();
}



function button_Audio()
{
   button_audio_status = 1;
  console.log("audio");
  send_data();
}

function usermessage(){
var inputdelay = document.getElementById("userinput").value
document.getElementById("showstart").innerHTML = document.getElementById("userinput").value
showstart = inputdelay;

}

function sendusermessage(){
  document.getElementById('timer').innerHTML =
       showstart + ":" + 00;
     startTimer();
showstartdelay = showstart;
console.log("time delay");
send_data();

}

function send_data()
{
  var full_data = '{"SyncMode" :'+button_sync_status+',"AudioTest":'+button_audio_status+',"ContTest":'+button_cont_status+',"showstart" :'+showstartdelay+'}';
  connection.send(full_data);
button_sync_status = 0;
button_audio_status = 0;
showstartdelay = 0;
button_cont_status = 0;

}





</script>
<style>

body {background-color: grey;}


#controls {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  table-layout: fixed;
width: 80%;
}

#controls td, #controls th {
  border: 4px solid #ddd;
  padding: 8px;
}

#controls tr:nth-child(even){background-color: #f2f2f2;}
#controls tr:nth-child(odd){background-color: white;}


#controls th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: center;
  background-color: black;
  color: white;
}
td {
  text-align: center;
}
.button {
  background-color: #4CAF50; /* Green */
  border: none;
  color: white;
  padding: 10px 10px;
  border-radius: 6px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}
table, th, td {
  border: 1px solid black;
}

.button2 {background-color: #008CBA;} /* Blue */
.button3 {background-color: #f44336;} /* Red */
.button4 {background-color: #e7e7e7; color: black;} /* Gray */
.button5 {background-color: #555555;} /* Black */
</style>

<body>

<center>
<h2 style="color: white;">Show Controls</h2>

<table id="controls">
  <tr>
    <th>Sync Time</th>
    <th>Continuity Check</th>
      <th>Audio Check</th>
    <th>Input Start Delay</th>
      <th>Confirm Delay</th>
        <th>Send Start</th>
        <th>Starts/Min</th>

  </tr>
  <tr>
    <style>

div  {
  color: black;
  font-family: courier;
  font-size: 160%;
}
</style>
  <div>Time left = <span id="timer"></span></div>
    <td><button class="button" onclick= "button_sync()" >Sync</button></td>
    <td><button class="button button2" onclick="button_Cont()" >Cont</button></td>
    <td><button class="button button2" onclick="button_Audio()">Audio</button></td>

      <td><input id ="userinput" size = "10"></td>
    <td><button class="button button5" onclick="usermessage()"> Select </button></td>

      <td>  <button class="button button3" onclick="sendusermessage()"> Send </button> </td>
<td>  <h3 id ="showstart">0</h3></td>
  </tr>
</table>
<br><br><br>


<h2 style="color: white;">Board Status</h2> <button id="button">Status</button>

<table id="controls" >

      </style>
  <tr>
    <th>Boards</th>
    <th>Status</th>
    <th>Mode </th>
      <th>Synced </th>
    <th>Continuity  </th>
      <th> Time Received </th>


  </tr>
  <tr>


<td>Board 1</td>
<td><p id="b1status">Offline</p></td>
<td><p id="b1mode">Mode</p></td>
<td><p id="b1sync">No</p></td>
<td><p id="b1cont">Not checked</p></td>
<td><p id="b1time">No</p></td>
</tr>
<tr>
  <td>Board 2</td>
  <td><p id="b2status">Offline</p></td>
  <td><p id="b2mode">Mode</p></td>
  <td><p id="b2sync">No</p></td>
  <td><p id="b2cont">Not checked</p></td>
  <td><p id="b2time">No</p></td>
</tr>
<tr>
  <td>Board 3</td>
  <td><p id="b3status">Offline</p></td>
  <td><p id="b3mode">Mode</p></td>
  <td><p id="b3sync">No</p></td>
  <td><p id="b3cont">Not checked</p></td>
  <td><p id="b3time">No</p></td>
</tr>
<tr>
  <td>Board 4</td>
  <td><p id="b4status">Offline</p></td>
  <td><p id="b4mode">Mode</p></td>
  <td><p id="b4sync">No</p></td>
  <td><p id="b4cont">Not checked</p></td>
  <td><p id="b4time">No</p></td>
</tr>
<tr>
  <td>Board 5</td>
  <td><p id="b5status">Offline</p></td>
  <td><p id="b5mode">Mode</p></td>
  <td><p id="b5sync">No</p></td>
  <td><p id="b5cont">Not checked</p></td>
  <td><p id="b5time">No</p</td>
</tr>
<tr>
  <td>Board 6</td>
  <td><p id="b6status">Offline</p></td>
  <td><p id="b6mode">Mode</p></td>
  <td><p id="b6sync">No</p></td>
  <td><p id="b6audio">Not checked</p></td>
  <td><p id="b6time">No</p></td>
</tr>

</table>

<br><br><br>
<table id="controls" >

      </style>
  <tr>
    <th  >Boards</th>
    <th>Cues 1 - 8 </th>
      <th>Cues 9 - 16 </th>
    <th>Cues 17 - 24  </th>
      <th> Cues 25 - 32 </th>
      <th> Cues 33 - 40 </th>
      <th> Cues 41 - 48 </th>


  </tr>
  <tr>


<td > Board 1  </td>
<td> <p id="b1c8">Not Checked</p> </td>
<td> <p id="b1c16">Not Checked</p> </td>
<td> <p id="b1c24">Not Checked</p> </td>
<td> <p id="b1c32">Not Checked</p> </td>
<td> <p id="b1c40">Not Checked</p> </td>
<td> <p id="b1c48">Not Checked</p> </td>
</tr>
<tr>
  <td>  Board 2 </td>
  <td> <p id="b2c8">Not Checked</p> </td>
  <td> <p id="b2c16">Not Checked</p> </td>
  <td> <p id="b2c24">Not Checked</p> </td>
  <td> <p id="b2c32">Not Checked</p> </td>
  <td> <p id="b2c40">Not Checked</p> </td>
  <td> <p id="b2c48">Not Checked</p> </td>
</tr>
<tr>
  <td>  Board 3 </td>
  <td> <p id="b3c8">Not Checked</p> </td>
  <td> <p id="b3c16">Not Checked</p> </td>
  <td> <p id="b3c24">Not Checked</p> </td>
  <td> <p id="b3c32">Not Checked</p> </td>
  <td> <p id="b3c40">Not Checked</p> </td>
  <td> <p id="b3c48">Not Checked</p> </td>
</tr>
<tr>
  <td>Board 4   </td>
  <td> <p id="b4c8">Not Checked</p> </td>
  <td> <p id="b4c16">Not Checked</p> </td>
  <td> <p id="b4c24">Not Checked</p> </td>
  <td> <p id="b4c32">Not Checked</p> </td>
  <td> <p id="b4c40">Not Checked</p> </td>
  <td> <p id="b4c48">Not Checked</p> </td>
</tr>
<tr>
  <td> Board 5  </td>
  <td> <p id="b5c8">Not Checked</p> </td>
  <td> <p id="b5c16">Not Checked</p> </td>
  <td> <p id="b5c24">Not Checked</p> </td>
  <td> <p id="b5c32">Not Checked</p> </td>
  <td> <p id="b5c40">Not Checked</p> </td>
  <td> <p id="b5c48">Not Checked</p> </td>
</tr>
<tr>
  <td> Board 6  </td>
  <td> <p id="b6c8">Not Checked</p> </td>
  <td> <p id="b6c16">Not Checked</p> </td>
  <td> <p id="b6c24">Not Checked</p> </td>
  <td> <p id="b6c32">Not Checked</p> </td>
  <td> <p id="b6c40">Not Checked</p> </td>
  <td> <p id="b6c48">Not Checked</p> </td>
</tr>

</table>
</center>
<script>
document.getElementById("button").addEventListener("click", function(){
td_array = document.getElementsByTagName("td");
check_value = "Offline";
check_value1 = "Game Time";
check_value2 = "Yes";
check_value3 = "Online";


for (i = 0; i < td_array.length; i++){
if (td_array[i].textContent == check_value){
td_array[i].style.backgroundColor = "#FE3535"; // Red
};
if (td_array[i].textContent == check_value1){
td_array[i-5].style.backgroundColor = "#3BF969"; //Green
};
if (td_array[i].textContent == check_value2){
td_array[i].style.backgroundColor = "#3BF969"; //Green
};
if (td_array[i].textContent == check_value3){
td_array[i].style.backgroundColor = "#A8BFF9"; //Blue
};
};
});
  setTimeout("CallButton()",3000)
  function CallButton(){
  document.getElementById("button").click();
  }
  function startTimer() {
         var presentTime = document.getElementById('timer').innerHTML;
         var timeArray = presentTime.split(/[:]+/);
         var m = timeArray[0];
         var s = checkSecond((timeArray[1] - 1));
         if(s==59){m=m-1}
         if(m<0){
           return
         }

         document.getElementById('timer').innerHTML =
           m + ":" + s;
         console.log(m)
         setTimeout(startTimer, 1000);

       }

       function checkSecond(sec) {
         if (sec < 10 && sec >= 0) {sec = "0" + sec}; // add zero in front of numbers < 10
         if (sec < 0) {sec = "59"};
         return sec;
       }
</script>



<p id="b2c"></p>
<p id="b3"></p>
<p id="b4"></p>
<p id="b5"></p>
<p id="b6"></p>
</body>
</html>




)=====";



// ipaddress/led1/on
//ipaddress/led1/off

// ipaddress/led2/on
//ipaddress/led2/off


AsyncWebServer server(80); // server port 80
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      
     DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

   SYNC_status = doc["SyncMode"];
  sp("syncmodestatus = ")
   spl(SYNC_status)
  Cont_status = doc["ContTest"];
  Audio_status = doc["AudioTest"];
  sp("audiotest = ")
   spl(Audio_status)
   ShowTimeDelay =  doc["showstart"];
  Serial.print("Show start -   ");
  Serial.println(ShowTimeDelay*60000);
  //digitalWrite(LED1,LED1_status);
 // di//gitalWrite(LED2,LED2_status);

  if(ShowTimeDelay > 0){
    send_ShowTimeDelay();
  }

    if( SYNC_status == 1){ spl("shouldnt be here")
    send_syncMode ();
    
  }

     if( Audio_status == 1){ spl("shouldnt be here")
    send_AudioTest ();
    
  }

       if( Cont_status == 1){ spl("shouldnt be here")
    send_ContTest ();
    
  }

  }
}

void setup(void)
{
  
  Serial.begin(115200);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(button, INPUT_PULLUP);
   pinMode(button2, INPUT_PULLUP);

//  
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
leds[0] = CRGB::Red; leds[1] = CRGB::Red; leds[2] = CRGB::Red; leds[3] = CRGB::Red; leds[4] = CRGB::Red;
leds[5] = CRGB::Red; leds[6] = CRGB::Red; leds[7] = CRGB::Red; leds[8] = CRGB::Red; leds[9] = CRGB::Red;
FastLED.show();
  
 // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  WiFi.softAP("MrBoom", "fire12345");
  Serial.println("softap");
  Serial.println("");
  Serial.println(WiFi.softAPIP());


  if (MDNS.begin("ESP")) { //esp.local/
    Serial.println("MDNS responder started");
  }



  server.on("/", [](AsyncWebServerRequest * request)
  { 
   
  request->send_P(200, "text/html", webpage);
  });

   server.on("/led1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(LED1,HIGH);
  request->send_P(200, "text/html", webpage);
  });

  server.onNotFound(notFound);

  server.begin();  // it will start webserver
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer.attach(2,send_sensor);
   //timer.attach(10,send_sensor);  might need this

      //-----------ESP NOW BOARD ID setup -------------------


                                  if(WiFi.macAddress() == "08:3A:F2:50:A4:F8") {
                                          spl() spl() spl("==========I am Board 1 - WEB ==========")spl(WiFi.macAddress())
                                          spl() spl() myID = 1; }

//                                  if(WiFi.macAddress() == "84:CC:A8:7E:BE:58") {
//                                          spl() spl() spl("===========I am Board 2 - AUDIO ===========")spl(WiFi.macAddress())
//                                          spl() spl() myID = 2; }

    if(WiFi.macAddress() == "8C:4B:14:16:0F:58"){
       spl()spl()
   Serial.println("===========I am Board 2===========");
   spl()spl()
   myID = 2;
   }

                                  if(WiFi.macAddress() == "9C:9C:1F:C5:A5:80") {
                                          spl() spl() spl("========== I am Board 4 - RECEIVER 1 ==========")spl(WiFi.macAddress())
                                            spl() spl() myID = 4;  }

                                  if(WiFi.macAddress() == "84:CC:A8:7E:C4:64") {
                                          spl() spl() spl("========== I am Board 3 - RECEIVER 2 ==========")spl(WiFi.macAddress())
                                            spl() spl() myID = 3;  }

                                  if(WiFi.macAddress() == "84:CC:A8:7E:C3:50") {
                                          spl() spl() spl("========== I am Board 5 - RECEIVER 3 ==========")spl(WiFi.macAddress())
                                            spl() spl() myID = 5;  }

                                  if(WiFi.macAddress() == "9C:9C:1F:E3:76:50") {
                                          spl() spl() spl("===========I am Board 6  - RECEIVER 4 ==========")spl(WiFi.macAddress())
                                          spl() spl() myID = 6; }

                          //----------- ESP NOW SETUP INFO  -------------------

    //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

    esp_now_register_send_cb(OnDataSent);

      // register 0 peer
                          esp_now_peer_info_t peerInfo;
                          peerInfo.channel = 0;
                          peerInfo.encrypt = false;



                  //   register 1st peer
                          memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
                          if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                  spl("Failed to add peer");
                                  return; }

                  //  register 2nd pair
                          memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
                          if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                  spl("Failed to add peer");
                                  return; }
                  //
                  //   register 3rd peer
                          memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
                          if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                  spl("Failed to add peer");
                                  return; }

                   //   register 4th peer
                          memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
                          if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                  spl("Failed to add peer");
                                  return; }

                   //   register 5th peer
                          memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
                          if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                  spl("Failed to add peer");
                                  return; }

                                  //   register 6th peer
                                         memcpy(peerInfo.peer_addr, broadcastAddress6, 6);
                                         if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                                                 spl("Failed to add peer");
                                                 return; }


  spl("Finished Setup - delaying for 3 seconds")
  delay(3000);
    spl("Main Loop has Started");

}


void loop(void)
{
    if (digitalRead(button) == LOW) {
   leds[4] = CRGB::Green; FastLED.show();
  myData.id = myID; // ID of this board set in the SETUP
  //Varriable to send to boards
myData.x = 100;

esp_err_t result1 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 delay(500);

}  if (digitalRead(button2) == LOW) {
   leds[4] = CRGB::Blue; FastLED.show();
  myData.id = myID; // ID of this board set in the SETUP
  //Varriable to send to boards
myData.x = 2;

esp_err_t result1 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 delay(500);}

else{  leds[4] = CRGB::Black; FastLED.show(); websockets.loop();}

}

void send_sensor()
{
int b1 = board1X; int b1c1 = board1cc1;  int b1c2 = board1cc2;  int b1c3 = board1cc3;  int b1c4 = board1cc4;  int b1c5 = board1cc5;  int b1c6 = board1cc6;
int b2 = board2X; int b2c1 = board2cc1;  int b2c2 = board2cc2;  int b2c3 = board2cc3;  int b2c4 = board2cc4;  int b2c5 = board2cc5;  int b2c6 = board2cc6;
int b3 = board3X; int b3c1 = board3cc1;  int b3c2 = board3cc2;  int b3c3 = board3cc3;  int b3c4 = board3cc4;  int b3c5 = board3cc5;  int b3c6 = board3cc6;
int b4 = board4X; int b4c1 = board4cc1;  int b4c2 = board4cc2;  int b4c3 = board4cc3;  int b4c4 = board4cc4;  int b4c5 = board4cc5;  int b4c6 = board4cc6;
int b5 = board5X; int b5c1 = board5cc1;  int b5c2 = board5cc2;  int b5c3 = board5cc3;  int b5c4 = board5cc4;  int b5c5 = board5cc5;  int b5c6 = board5cc6;
int b6 = board6X; int b6c1 = board6cc1;  int b6c2 = board6cc2;  int b6c3 = board6cc3;  int b6c4 = board6cc4;  int b6c5 = board6cc5;  int b6c6 = board6cc6;

   sp("board2x = ")
  spl(board2X)

  // JSON_Data = {"temp":t,"hum":h}
  String JSON_Data = "{\"board1\":";
         JSON_Data += b1;
         JSON_Data += ",\"board2\":";
         JSON_Data += b2;
         JSON_Data += ",\"board2\":";
         JSON_Data += b2;
   JSON_Data += ",\"board1c1\":";
   JSON_Data += b1c1;
   JSON_Data += ",\"board1c2\":";
   JSON_Data += b1c2;
   JSON_Data += ",\"board1c3\":";
   JSON_Data += b1c3;
   JSON_Data += ",\"board1c4\":";
   JSON_Data += b1c4;
   JSON_Data += ",\"board1c5\":";
   JSON_Data += b1c5;
   JSON_Data += ",\"board1c6\":";
   JSON_Data += b1c6;
   JSON_Data += ",\"board2c1\":";
   JSON_Data += b2c1;
   JSON_Data += ",\"board2c2\":";
   JSON_Data += b2c2;
   JSON_Data += ",\"board2c3\":";
   JSON_Data += b2c3;
   JSON_Data += ",\"board2c4\":";
   JSON_Data += b2c4;
   JSON_Data += ",\"board2c5\":";
   JSON_Data += b2c5;
   JSON_Data += ",\"board2c6\":";
   JSON_Data += b2c6;
   JSON_Data += ",\"board3c1\":";
   JSON_Data += b3c1;
   JSON_Data += ",\"board3c2\":";
   JSON_Data += b3c2;
   JSON_Data += ",\"board3c3\":";
   JSON_Data += b3c3;
   JSON_Data += ",\"board3c4\":";
   JSON_Data += b3c4;
   JSON_Data += ",\"board3c5\":";
   JSON_Data += b3c5;
   JSON_Data += ",\"board3c6\":";
   JSON_Data += b3c6;
   JSON_Data += ",\"board4c1\":";
   JSON_Data += b4c1;
   JSON_Data += ",\"board4c2\":";
   JSON_Data += b4c2;
   JSON_Data += ",\"board4c3\":";
   JSON_Data += b4c3;
   JSON_Data += ",\"board4c4\":";
   JSON_Data += b4c4;
   JSON_Data += ",\"board4c5\":";
   JSON_Data += b4c5;
   JSON_Data += ",\"board4c6\":";
   JSON_Data += b4c6;
   JSON_Data += ",\"board5c1\":";
   JSON_Data += b5c1;
   JSON_Data += ",\"board5c2\":";
   JSON_Data += b5c2;
   JSON_Data += ",\"board5c3\":";
   JSON_Data += b5c3;
   JSON_Data += ",\"board5c4\":";
   JSON_Data += b5c4;
   JSON_Data += ",\"board5c5\":";
   JSON_Data += b5c5;
   JSON_Data += ",\"board5c6\":";
   JSON_Data += b5c6;
   JSON_Data += ",\"board6c1\":";
   JSON_Data += b6c1;
   JSON_Data += ",\"board6c2\":";
   JSON_Data += b6c2;
   JSON_Data += ",\"board6c3\":";
   JSON_Data += b6c3;
   JSON_Data += ",\"board6c4\":";
   JSON_Data += b6c4;
   JSON_Data += ",\"board6c5\":";
   JSON_Data += b6c5;
   JSON_Data += ",\"board6c6\":";
   JSON_Data += b6c6;
          JSON_Data += ",\"board3\":";
         JSON_Data += b3;
          JSON_Data += ",\"board4\":";
         JSON_Data += b4;
          JSON_Data += ",\"board5\":";
         JSON_Data += b5;
          JSON_Data += ",\"board6\":";
         JSON_Data += b6;
         JSON_Data += "}";
   Serial.println(JSON_Data);        
  websockets.broadcastTXT(JSON_Data);
}

void send_ShowTimeDelay (){ //update to receive time instead of 1
  ShowTimeDelay = ShowTimeDelay * 60000;
 
  myData.id = myID; // ID of this board set in the SETUP
  myData.timex = ShowTimeDelay; //Varriable to send to boards
myData.x = 0;

esp_err_t result1 = esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
 delay(500);
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
  delay(500);
  ShowTimeDelay = 0;
}
// board3
void send_syncMode (){ // clock sync
  
  myData.id = myID; // ID of this board set in the SETUP
 
myData.x = 2;

esp_err_t result1 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 delay(500);
  esp_err_t result2 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  delay(500);
  ShowTimeDelay = 0;
    SYNC_status = 0;
    spl(SYNC_status)
}

void send_AudioTest (){ // clock sync
  
  myData.id = myID; // ID of this board set in the SETUP
 //Varriable to send to boards
myData.x = 4;

spl("sending audio check")
esp_err_t result1 = esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
 delay(500);
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
  delay(500);
  ShowTimeDelay = 0;
    SYNC_status = 0;
    spl(SYNC_status)
}

void send_ContTest (){ // clock sync
  
  myData.id = myID; // ID of this board set in the SETUP
  //Varriable to send to boards
myData.x = 3;

esp_err_t result1 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 delay(500);
  esp_err_t result2 = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  delay(500);
  ShowTimeDelay = 0;
    SYNC_status = 0;
    spl(SYNC_status)
}//
