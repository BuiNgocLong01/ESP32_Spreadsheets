#include <WiFi.h>
#include <HTTPClient.h>

//ENTER_GOOGLE_DEPLOYMENT_ID
const char * ssid = "TOTOLINK_N350RT";
const char * password = "15031972";
String GOOGLE_SCRIPT_ID = "AKfycby_HOx6XfpQzeAWzjfzo61rbNYDn0_C_dOIw10PjDnGUqEd2La0g-Sc6fTH5BaTnnws";

// ----------Khai bao cam bien------------------------------------------------------------------------------------
const int trig = 16;
const int echo = 17;
long duration;
int distanceCm, distanceInch;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

unsigned long Time;

void readSensor(int &distanceCm, int &distanceInch)
{
  // Clears the trigPin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);

  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;
}

void setup()
{
  Serial.begin(115200);
  Time = millis();
  pinMode(trig, OUTPUT); //Chân trig xuất tín hiệu
  pinMode(echo, INPUT); //Chân echo nhận tín hiệu
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("CONNECTED !!!");
}


// ======================================================
void loop()
{

  if (millis() - Time >= 1000)
  {
    Time = millis();

    print_speed();

  }
}


void print_speed()
{
  readSensor(distanceCm, distanceInch);
  if ( distanceCm != 0)
  {
    Serial.println("distanceCm=" + String(distanceCm) + "&distanceInch=" + String(distanceInch));

    String param;
    param  = "distanceCm=" + String(distanceCm) + "&" + "distanceInch=" + String(distanceInch);

    Serial.println(param);
    write_to_google_sheet(param);
  }
  else
  {
    Serial.println("No data !!!");
  }

}
/******/
void write_to_google_sheet(String params)
{
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  //Serial.print(url);
  Serial.println("Data sent to Google Sheets");
  //---------------------------------------------------------------------
  //starts posting data to google sheet
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
  //---------------------------------------------------------------------
  //getting response from google sheet
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload: " + payload);
  }
  //---------------------------------------------------------------------
  http.end();
}
