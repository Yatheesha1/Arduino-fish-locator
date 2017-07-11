#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include<String.h>
#include <SPI.h>
#include <Ethernet.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//MAC address

IPAddress ip(192,168,137,20);//
EthernetServer server(80);
//byte serverName[] = { 192, 168, 1, 5 };

static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
int soundpin=5;
int flag=0;
String st;
int sound=0;


// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  gpsSerial.begin(GPSBaud);
  pinMode(soundpin,INPUT);
}

void loop()
{
  ether();
}

void ether()
{
  EthernetClient client;
  String readString;
  client = server.available();
  if (client) 
  {
    while (client.connected()) 
    { 
      sound=digitalRead(soundpin);  
      gpslocate();
      if (client.available()) 
      {
         char c = client.read(); 
         //read char by char HTTP request
         if (readString.length() < 100) 
         {
          //store characters to string
          readString += c;
         }   
         //if HTTP request has ended
         if (c == '\n') 
         {              
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();  
              
           client.println("<HTML>");
           
           client.println("<HEAD>");
           client.println("<TITLE>Arduino Project</TITLE>");
           client.println("</HEAD>");
           
           client.println("<BODY>");  
                  
           client.println("<br/>");  
           client.println("<H2>FISH LOCATER</H2>"); 
         
           client.println("Location ");
           client.println(st);
           client.println("<br/>");
           if(sound)
             client.println("Fish found "); 
           else  
             client.println("No Fish found "); 
           client.println("</BODY>");
           client.println("<head>");
           client.println("<meta http-equiv='refresh' content='1'>");
           client.println("</head>");
           
           client.println("</HTML>");
     
           //delay(1);
           //stopping client
           client.stop();      
         }  
      } 
    }     
    readString="";            
  }
}

void gpslocate()
{
  int k=0;
  String lati,longi;
  while (gpsSerial.available() > 0)
    {
      k++;
      flag=0;
      if (gps.encode(gpsSerial.read()))
      {
        Serial.print(F("Location: ")); 
        if (gps.location.isValid())
        {
          lati=String(gps.location.lat(),6);
          longi=String(gps.location.lng(),6);
          Serial.print(lati);
          Serial.print(F(","));
          Serial.print(longi+"\t");
          st="http://maps.google.com/?q="+lati+","+longi;
          Serial.println(st);
          return;
        }
        else
        {
          Serial.println(F("INVALID"));
          if(k>5)
          {  
             st="Invalid";
             return;
          }
          delay(50);
        }
      }
    }
    if (millis() > 5000 && gps.charsProcessed() < 10 && flag==0)
    {
      Serial.println(F("No GPS detected: check wiring."));
      st="No GPS detected: check wiring.";
      flag=1;
    }
}
