#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>  

//Variables
const int trigPin = 5;
const int echoPin = 18;
int IR = 25;
int IR2 = 33;
int _moisture, sensor_analog;
const int sensor_pin = A0;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;
float distanceFeet;

unsigned long previousMillis = 0;
const long interval = 1000;  

const char* ssid = "name";
const char* password = "password";


const char* serverUrl = "https://smart-walking-stick.onrender.com/predict";


AsyncWebServer server(80);


HardwareSerial mySerial(1); 
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

 
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected!");
    while (true);
  }
  Serial.println("DFPlayer Mini detected.");
  myDFPlayer.setTimeOut(500); 
  myDFPlayer.volume(29); 

  
  pinMode(IR, INPUT);
  pinMode(IR2, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  myDFPlayer.play(24); 
  delay(2500);

  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/image", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (index == 0) {
        Serial.println("Receiving image...");
        File file = SPIFFS.open("/received_image.jpg", FILE_WRITE);
        if (!file) {
          Serial.println("Failed to open file for writing");
          return;
        }
        file.close();
      }

      File file = SPIFFS.open("/received_image.jpg", FILE_APPEND);
      if (!file) {
        Serial.println("Failed to open file for appending");
        return;
      }
      file.write(data, len);
      file.close();

      if (index + len == total) {
        Serial.println("Image received successfully!");
        request->send(200, "text/plain", "Image received");

      
      }
    });

  // Start server
  server.begin();
}

void sendImageToServer(const char* path) {
  HTTPClient http;
  File file = SPIFFS.open(path, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");

    int fileSize = file.size();
    uint8_t* buffer = new uint8_t[fileSize];
    file.read(buffer, fileSize);
    file.close();

    int httpResponseCode = http.POST(buffer, fileSize);
    delete[] buffer;

    if (httpResponseCode > 0) {
      Serial.print("Response code: ");
      Serial.println(httpResponseCode);

      String response = http.getString();
      Serial.print("Server response: ");
      Serial.println(response);
      
      
      handlePrediction(response); 

    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void handlePrediction(const String& response) {
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.f_str());
    myDFPlayer.play(12);  
    return;
  }

  const char* label = doc["label"];
  float confidence = doc["confidence"];

  Serial.print("Label: ");
  Serial.println(label);
  Serial.print("Confidence: ");
  Serial.println(confidence);

  if (strcmp(label, "6 Ogiri") == 0) {
    Serial.println("Detected: 6 Ogiri");
    myDFPlayer.play(18); 
    delay(2500);
  } 
  
  else if (strcmp(label, "0 ọkada") == 0) { 
    Serial.println("Detected: okada");
    myDFPlayer.play(20);  
    delay(2500);
  } 
  
  else if (strcmp(label, "1 òkúta") == 0) {  
    Serial.println("Detected:  òkúta");
    myDFPlayer.play(22); 
    delay(2500);
  }

  else if (strcmp(label, "2 ọkọ ayọkẹlẹ") == 0) {  
    Serial.println("Detected: ọkọ ayọkẹlẹ");
    myDFPlayer.play(21); 
  }

  else if (strcmp(label, "3 Ilẹkùn") == 0) {  
    Serial.println("Detected: Ilẹkùn");
    myDFPlayer.play(15);
    delay(2500);
  }

  else if (strcmp(label, "4 àtẹso") == 0) { 
    Serial.println("Detected: àtẹso");
    myDFPlayer.play(14); 
    delay(2500);
  }

  else if (strcmp(label, "5 àtẹgùn") == 0) {  
    Serial.println("Detected: àtẹgùn");
    myDFPlayer.play(13); 
    delay(2500);
  }

  else if (strcmp(label, "7 Ododo") == 0) {  
    Serial.println("Detected: 7 Ododo");
    myDFPlayer.play(17); 
    delay(2500);
  }

  else if (strcmp(label, "8 eniyan") == 0) { 
    Serial.println("Detected: eniyan");
    myDFPlayer.play(9); 
    delay(2500);
  }

  else if (strcmp(label, "9 opo") == 0) {  
    Serial.println("Detected: opo");
    myDFPlayer.play(11); 
    delay(2500);
  }

  else if (strcmp(label, "10 igi") == 0) {  
    Serial.println("Detected: 1igi");
    myDFPlayer.play(10);  
    delay(2500);
  }

  else if (strcmp(label, "11 Ohun ìdalẹ̀sí") == 0) {  
    Serial.println("Detected: Ohun ìdalẹ̀sí");
    myDFPlayer.play(19);  
    delay(2500);
  }

  else {
    Serial.println("Unknown label received from the server."); 
    delay(2500);
  }
}

void moisture(){
  
  sensor_analog = analogRead(sensor_pin);
    _moisture = (100 - ((sensor_analog / 4095.00) * 100));
    if (_moisture >= 20) {
      Serial.println("There is water in front of you!");
      myDFPlayer.play(1);
      delay(2500);
    }

}

void ir_sensor(){
  int s = digitalRead(IR2);
  int s2 = digitalRead(IR);
  if (s == LOW) {
    Serial.println("TURN LEFT");
    myDFPlayer.play(16); 
    delay(2500);
      
  } else if (s2 == LOW) {
    Serial.println("TURN RIGHT");
    myDFPlayer.play(23);
    delay(2500);
  }
}

void loop() {
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  
    moisture();

    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED / 2;
    distanceFeet = distanceCm * 0.0328;

    if (distanceFeet >= 0.9 && distanceFeet <= 1) {
      Serial.println("Obstacle detected 1 foot ahead of you");
      sendImageToServer("/received_image.jpg"); 
      myDFPlayer.play(2);
      delay(2500);
      ir_sensor();
      moisture();
      

    } else if (distanceFeet > 1 && distanceFeet <= 2) {
      Serial.println("Obstacle detected 2 feet ahead of you");
      sendImageToServer("/received_image.jpg"); 
      myDFPlayer.play(3); 
      delay(2500);
      ir_sensor();
      moisture();

    } else if (distanceFeet > 2 && distanceFeet <= 3) {
      Serial.println("Obstacle detected 3 feet ahead of you");
      sendImageToServer("/received_image.jpg");
      myDFPlayer.play(4);  
      delay(2500);
      ir_sensor();
      moisture();
      

    } else if (distanceFeet > 3 && distanceFeet <= 4) {
      Serial.println("Obstacle detected 4 feet ahead of you");
      sendImageToServer("/received_image.jpg"); 
      myDFPlayer.play(5);  
      delay(2500);
      ir_sensor();
      moisture();
      
    } else if (distanceFeet > 4 && distanceFeet <= 5) {
      Serial.println("Obstacle detected 5 feet ahead of you");
      sendImageToServer("/received_image.jpg");
      myDFPlayer.play(6);  
      delay(2500);
      ir_sensor();
      moisture();
      
      
    } else if (distanceFeet > 5 && distanceFeet <= 6) {
      Serial.println("Obstacle detected 6 feet ahead of you");
      sendImageToServer("/received_image.jpg"); 
      myDFPlayer.play(7);  
      delay(2500);
      ir_sensor();
      moisture();
      

    } else if (distanceFeet > 6 && distanceFeet <= 7) {
      Serial.println("Obstacle detected 7 feet ahead of you");
      sendImageToServer("/received_image.jpg"); 
      myDFPlayer.play(8);  
      delay(2500);
      ir_sensor();
      moisture();
      
    } 
  }
}
