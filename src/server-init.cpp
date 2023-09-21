#include <WebServer.h>
#include <Update.h>
#include "import-html-files.h"

extern int output2;
extern String output2State;
extern int digitsNumber;
extern int digitsPoint;
WebServer server(80);

void controlPins(int pinNumber, String &pinState)
{
    String pinsPathOn = String("/pins-") + String(pinNumber) + String("/on");
    String pinsPathOff = String("/pins-") + String(pinNumber) + String("/off");

    server.on(pinsPathOn, HTTP_GET, [pinNumber, &pinState]()
              {
        digitalWrite(pinNumber, HIGH);
        pinState = "on";
        server.sendHeader("Location", "/control-pins");
        server.send(302, "text/plain", ""); });

    server.on(pinsPathOff, HTTP_GET, [pinNumber, &pinState]()
              {
        digitalWrite(pinNumber, LOW);
        pinState = "off";
        server.sendHeader("Location", "/control-pins");
        server.send(302, "text/plain", ""); });
}

void digitDisplayPins()
{
    server.on("/number-UPD", HTTP_GET, []()
              {
        String url = server.uri();
        int digits;
        int point;

        if (server.args() > 0) {
            for (int i = 0; i < server.args(); i++) {
                if (server.argName(i) == "number") {
                    digits = server.arg(i).toInt();
                }
                if (server.argName(i) == "point") {
                    point = server.arg(i).toInt();
                }
            }
            digitsNumber = digits;
            digitsPoint = point;
        }

        server.sendHeader("Location", "/control-pins");
        server.send(302, "text/plain", ""); });
}

void serverInit()
{

    server.on("/", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginHTML()); });

    server.on("/control-pins", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", controlHtml()); });

    server.on("/update-OTA", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", updateOTAHTML()); });

    server.on(
        "/update", HTTP_POST, []()
        {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart(); },
        []()
        {
            HTTPUpload &upload = server.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                {
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });

    server.begin();
}

void serverControl()
{
    controlPins(output2, output2State);
    digitDisplayPins();
}

void serverHandler()
{
    server.handleClient();
    delay(1);
}

