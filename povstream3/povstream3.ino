


#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include <RF24.h>
#include <SPI.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(50, 5, NEO_GRB + NEO_KHZ800);

RF24 radio(9, 10);  //teensy 3,0


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


int l;

unsigned char buffer[32];
byte red[80][80];
byte green[80][80];
byte blue[80][80];

byte pixel_x[91][41];
byte pixel_y[91][41];

unsigned long t;
unsigned long c,e,n255;



int deg;

int sinus[361];
int cosinus[361];

volatile unsigned long disk_speed, disk_timer;
volatile boolean start;

unsigned long loop_speed, loop_timer;

void setup() {
  strip.begin(); strip.show(); 
  strip.setPixelColor(1, 255);
  strip.setPixelColor(2, 255<<8);
  strip.setPixelColor(3, 255<<16);

  pinMode(3,INPUT);

  //delay(2000);
  Serial.begin(115200);
  //delay(1000);
    
  t=millis();  
  for (int i=0; i<361; i++ ) {
      sinus[i]=(int)(sin(i*6.28318531/360)*1000);
      cosinus[i]=(int)(cos(i*6.28318531/360)*1000);
  }

  for (int d=0; d<=90; d++) {
    for (int a=0; a<40; a++) {
        int x= (a*cosinus[d])/1000;
        int y= (a*sinus[d])/1000;    
        pixel_x[d][a]= x;
        pixel_y[d][a]= y;
   /*   Serial.print("d= ");
        Serial.print(d);
        Serial.print(" a= ");
        Serial.print(a);
        Serial.print(" x= ");
        Serial.print(x);
        Serial.print(" y= ");
        Serial.println(y);
        delay(10);
    */    
        
    }    
  }  
  
  for (int x=0; x<24; x++) {
    for (int y=0; y<80; y++) {
         red[x][y]=255;     
    }
  }
  
  for (int x=24; x<48; x++) {
    for (int y=0; y<80; y++) {
         green[x][y]=255;     
    }
  }
 
 for (int x=48; x<80; x++) {
    for (int y=0; y<80; y++) {
         blue[x][y]=255;     
    }
  }
 
  
 /* 
  for (int x=0; x<80; x++) {
    for (int y=0; y<80; y++) {
         red[x][y]=(byte)int(x*255/80);
         green[x][y]=(byte)int(y*255/80);
         blue[x][y]=(byte)int(((x+y)%80)*255/80);        
     
    }
  }
   */ 
Serial.print(" 10 degrees led 4: ");
Serial.print( pixel_x[10][4]);
Serial.print(" ");
Serial.println(pixel_y[10][4]);


Serial.print(" 25 degrees led 30: ");
Serial.print( pixel_x[25][30]);
Serial.print(" ");
Serial.println(pixel_y[25][30]);


Serial.print(" 45 degrees led 20: ");
Serial.print( pixel_x[45][20]);
Serial.print(" ");
Serial.println(pixel_y[45][20]);


Serial.print(" 75 degrees led 30: ");
Serial.print( pixel_x[75][30]);
Serial.print(" ");
Serial.println(pixel_y[75][30]);

Serial.println(millis()-t);   

  strip.show(); // Initialize all pixels to 'off'

  //printf_begin();
  Serial.println("RF24/examples/pongtest/\n");
 
  radio.begin();
  radio.setRetries(15,15);

Serial.println("Setting up channel");
  // optionally, reduce the payload size.  seems to
  // improve reliability
	radio.setPayloadSize(32);
	radio.setChannel(0x4c);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);

  
  Serial.println("Setting up pipes");
  
    // radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);


  //
  // Start listening
  //
  Serial.println("Start listening");
  radio.startListening();
  
  Serial.println("End of setup process. Starting loop");
  
  
  // getframe();
  
  attachInterrupt(3, interrupt, FALLING);
  
  /// END SETUP
  start=false;
}

void loop(){
  
  loop_speed=micros()-loop_timer;
  
  
  Serial.print("Loop was: ");
  Serial.println(loop_speed);
 
  while(!start) {}
  start=false;
  Serial.println("START");
  
  loop_timer=micros();
  
  for (int d=0; d < 360; d=d+3) {
      for (int a=0; a < 40; a++) {     
          int k=(a%4)*10+int(a/4);
          int r=red[get_x(degr(d,a),a)][get_y(degr(d,a),a)];
          int g=green[get_x(degr(d,a),a)][get_y(degr(d,a),a)];
          int b=blue[get_x(degr(d,a),a)][get_y(degr(d,a),a)];
          if (degr(d,a)<90) { 
              strip.setPixelColor(k,256*256*255); }
          else {
            if (degr(d,a)<180) { 
              strip.setPixelColor(k,256*255); }
            else {
               if (degr(d,a)<270) { 
                  strip.setPixelColor(k,255); 
               } else {
                 strip.setPixelColor(k,256*256*255+255*256+255); 
               } 
            }
          }
          //strip.setPixelColor(k,red[get_x(d,a)][get_y(d,a)]); // <<16 + green[get_x(d,a)][get_y(d,a)]<<8 + blue[get_x(d,a)][get_y(d,a)]);
                    
          //strip.setPixelColor(k,int(a*255/40));
          
          
          /* if (start) { start = false; break; } */
      }
      strip.show();
      delay(5);
  }
 
  //delay(100);
   for (int a=0; a < 40; a++) {  
      strip.setPixelColor(a, 0);
   }
   strip.show();
  
  
  
  

} // forever loop


void getframe() {
  
  while (c < 5120) {
    
    if ( radio.available() )  {
        // Dump the payloads until we've gotten everything
       	unsigned long got_time;
	bool done = false;
			
	while (!done){			
	     done = radio.read( &buffer, 32 );	
             if (!done) e++;
            
             for (int i=0; i< 30; i++) {
                    //strip.setPixelColor(i, buffer[i] );
             }             
             c++;                      
             t = micros();                             
          }			
     }
  }
         Serial.print(c);
         Serial.print(" ");
         Serial.print(e);
         Serial.print(" ");
         Serial.print(n255);
         Serial.print(" ");
         Serial.println();
         //delay(10);
}

/*
int get_y(int d, int a) {
  if (d < 90) { return 40-pixel_y[d][a]; }
  if (d < 180) { return 40-pixel_y[180-d][a]; }
  if (d < 270) { return 40+pixel_y[d-180][a]; }
  return 40+pixel_y[360-d][a];   
}

int get_x(int d, int a) {
  if (d < 90) { return 40-pixel_x[d][a]; }
  if (d < 180) { return 40-pixel_x[180-d][a]; }
  if (d < 270) { return 40+pixel_x[d-180][a]; }
  return 40+pixel_x[360-d][a];    
}
*/

//  Toon
int get_y(int d, int a){
    if (d < 90) { return 40-pixel_y[d][a]; }
    if (d < 180) { return 40+pixel_y[180-d][a]; }
    if (d < 270) { return 40+pixel_y[d-180][a]; }
    return 40-pixel_y[360-d][a];   
};
int get_x(int d, int a){
  if (d < 90) { return 40-pixel_x[d][a]; }
  if (d < 180) { return 40-pixel_x[180-d][a]; }
  if (d < 270) { return 40+pixel_x[d-180][a]; }
  return 40+pixel_x[360-d][a];   
};


int degr(int d, int pos) {
 d = d + (pos%4)*90;
 if (d <0) d=d+360;
 if (d >= 360) d=d-360;
 return d;
  
}

void interrupt() {

  Serial.println("IRQ");
  
  disk_speed=micros()-disk_timer;
  disk_timer=micros();
  start=true;
}
