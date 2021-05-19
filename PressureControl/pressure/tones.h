#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112


void alarm(){
 // M5.Speaker.tone(NOTE_DL1, 1); //frequency 3000, with a duration of 200ms
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL2, 1);
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL3, 1);
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL4, 1);
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL5, 1);
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL6, 1);
//  sleep(1);
//  M5.Speaker.tone(NOTE_DL7, 1);
//  sleep(1);

  M5.Speaker.tone(NOTE_DL5, 1);
  sleep(1);
  M5.Speaker.tone(NOTE_DL3, 1);
}
