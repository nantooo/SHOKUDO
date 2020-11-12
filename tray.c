#include <WiFi.h>
#include <HTTPClient.h>
#include <stdio.h>
#include <string.h>
int PIN_NUM = 32;
int PIN_NUM2 = 33;
int k = 0;
int c =0;
int data[10]={0,0,0,0,0,0,0,0,0,0};
int Threshold=150;

const char SSID[] = "";// need to modify for prodction 
const char PASSWORD[] = "";

void setup() {
  Serial.begin(115200);
  while (!Serial);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" connected");
  
}


void send_data(int d){
  HTTPClient http;  //Declare an object of class HTTPClient
  String url = "http://54.199.32.89/";
  int input= 8881 ;
  int output = d ;
  String serverPath = url + "?input=" + input + "&output=" + output ;

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("NumberOftray: ");
        Serial.println(output);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
  http.end();
}

void cal(int v){
  if(v){//if the pressure of tray is not 0
    k+=1;
  }else{// if 0
    if(k>=4){
      int result=k/4;
      send_data(result);// call data-sending function
    }
    k=0;
  }
  
  delay(625);// delay for 1/4 of a tray passing through
  
}

int compare_int(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

int belt_condition(int b){// if it's running or not
  data[c]=b;// keep the last 10 pressure value 
  c+=1; //shift
  if(c==10){
    c=0;
  }
  
  int sort[10];
  memcpy(sort, data, sizeof(data));// copy data to sort
  qsort(sort, 10, sizeof(int), compare_int);
  Serial.print("MIN: ");
  Serial.println(sort[0]);
  Serial.print("MAX: ");
  Serial.println(sort[9]);
  
  if((sort[9]-sort[0])<Threshold){
    return 0;//not running
  }else{
    return 1;//running
  }
}

void loop() {
  int tray = 4095-analogRead(PIN_NUM);
  Serial.print("tray: ");
  Serial.println(tray);
  int belt = analogRead(PIN_NUM2);
  Serial.print("belt: ");
  Serial.println(belt);
  if(belt_condition(belt)){
    // if running
    cal(tray);//cal is a function that calculate the number of trays
  }else{
    //if not running
    Serial.print("Not running");
    Serial.print("");
  }
}