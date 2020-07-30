#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "web.h"
#include "config.h"


extern struct ConfigSettingsStruct ConfigSettings;
extern struct ConfigPanel cfgPanel;

extern String text ;
extern int sizeText;
extern int scrollText;
extern int xText;
extern int yText;
extern int light;

ESP8266WebServer serverWeb(80);

void webServerHandleClient()
{
   serverWeb.handleClient();
}




const char HTTP_HEADER[] PROGMEM = 
  "<head>"
  "<script type='text/javascript' src='web/js/jquery-min.js'></script>"
  "<script type='text/javascript' src='web/js/bootstrap.min.js'></script>"
  "<script type='text/javascript' src='web/js/functions.js'></script>"
  "<link href='web/css/bootstrap.min.css' rel='stylesheet' type='text/css' />"
  "<link href='web/css/style.css' rel='stylesheet' type='text/css' />"
  " </head>"
    "<body>"
  "<nav class='navbar navbar-expand-lg navbar-light bg-light rounded'><a class='navbar-brand' href='/'><img src='web/img/logo.png'/> <strong>Ikea Frekvens Config </strong>"
   VERSION
  "</a>"
  "<button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarNavDropdown' aria-controls='navbarNavDropdown' aria-expanded='false' aria-label='Toggle navigation'>"
  "<span class='navbar-toggler-icon'></span>"
  "</button>"
  "<div id='navbarNavDropdown' class='collapse navbar-collapse justify-content-md-center'>"
  "<ul class='navbar-nav'>"
    "<li class='nav-item'>"
      "<a class='nav-link' href='/'>WiFi</a>"
    "</li>"
    "<li class='nav-item'>"
      "<a class='nav-link' href='/apiweb'>WEB API</a>"
    "</li>"
     "<li class='nav-item'>"
      "<a class='nav-link' href='/tools'>Tools</a>"
    "</li>"
    "<li class='nav-item'>"
      "<a class='nav-link' href='#'>Help</a>"
    "</li>"
  "</ul></div>"
  "</nav>";


const char HTTP_ROOT[] PROGMEM = 
 "<h1>Config WiFi</h1>"
  "<div class='row justify-content-md-center' >"
    "<div class='col-sm-6'><form method='POST' action='save'>"
  "<div class='form-group'>"
    "<label for='ssid'>SSID</label>"
    "<input class='form-control' id='ssid' type='text' name='WIFISSID' value='{{ssid}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='pass'>Password</label>"
    "<input class='form-control' id='pass' type='password' name='WIFIpassword' value=''>"
  "</div>"
   "<div class='form-group'>"
    "<label for='ip'>@IP</label>"
  "<input class='form-control' id='ip' type='text' name='ipAddress' value='{{ip}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='mask'>@Mask</label>"
  "<input class='form-control' id='mask' type='text' name='ipMask' value='{{mask}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='gateway'>@Gateway</label>"
  "<input type='text' class='form-control' id='gateway' name='ipGW' value='{{gw}}'>"
  "</div>"
   "Server Port : <br>{{port}}<br><br>"
  "<button type='submit' class='btn btn-primary mb-2'name='save'>Save</button>"
  "</form>";


  const char HTTP_APIWEB[] PROGMEM = 
 "<h1>API Web</h1>"
  "<div class='row justify-content-md-center' >"
    "<div class='col-sm-6'><form method='post' action='savepanel'>"
  "<div class='form-group'>"
    "<label for='text'>TEXT</label>"
    "<input class='form-control' id='text' type='text' name='text' value='{{text}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='size'>Size</label>"
    "<select class='form-control' id='size' name='size'>"
      "<option value='1' {{selMin}}>Min</option>"
      "<option value='2' {{selMax}}>Max</option>"
    "</select>"
   
  "</div>"
  "<div class='form-group'>"
    "<label for='light'>Light</label>"
    "<input class='form-control' id='light' maxlength='4' inputmode='numeric' name='light' value='{{light}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='scroll'>Scroll</label>"
    "<select class='form-control' id='scroll' name='scroll'>"
      "<option value='1' {{scrollOui}}>Oui</option>"
      "<option value='0' {{scrollNon}}>Non</option>"
    "</select>"
  
  "</div>"
   "<div class='form-group'>"
    "<label for='posx'>X</label>"
  "<input class='form-control' id='posx' type='text' inputmode='numeric' name='x' value='{{xText}}'>"
  "</div>"
   "<div class='form-group'>"
    "<label for='posy'>Y</label>"
  "<input type='text' class='form-control' id='posy' inputmode='numeric' name='y' value='{{yText}}'>"
  "</div>"
  "<button type='submit' class='btn btn-primary mb-2'name='save'>Refresh</button>"
  "</form>";



void initWebServer()
{
  serverWeb.serveStatic("/web/js/jquery-min.js", SPIFFS, "/web/js/jquery-min.js");
  serverWeb.serveStatic("/web/js/functions.js", SPIFFS, "/web/js/functions.js");
  serverWeb.serveStatic("/web/js/bootstrap.min.js", SPIFFS, "/web/js/bootstrap.min.js");
  serverWeb.serveStatic("/web/js/bootstrap.min.js.map", SPIFFS, "/web/js/bootstrap.min.js.map");
  serverWeb.serveStatic("/web/css/bootstrap.min.css", SPIFFS, "/web/css/bootstrap.min.css");
  serverWeb.serveStatic("/web/css/style.css", SPIFFS, "/web/css/style.css");
  serverWeb.serveStatic("/web/img/logo.png", SPIFFS, "/web/img/logo.png");
  serverWeb.serveStatic("/web/img/wait.gif", SPIFFS, "/web/img/wait.gif");
  serverWeb.serveStatic("/web/img/", SPIFFS, "/web/img/");
  serverWeb.on("/", handleRoot);
  serverWeb.on("/save", HTTP_POST, handleSaveConfig);
  serverWeb.on("/savepanel", HTTP_POST, handleSavePanel);
  serverWeb.on("/api", HTTP_GET, handleAPI);
  serverWeb.on("/apiweb", handleAPIWeb);
  serverWeb.on("/tools", handleTools);
  serverWeb.on("/reboot", handleReboot);
  serverWeb.on("/fsbrowser", handleFSbrowser);
  serverWeb.on("/readFile", handleReadfile);
  serverWeb.onNotFound(handleNotFound);
  serverWeb.begin();
}

void handleNotFound() {

  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += serverWeb.uri();
  message += F("\nMethod: ");
  message += (serverWeb.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += serverWeb.args();
  message += F("\n");

  for (uint8_t i = 0; i < serverWeb.args(); i++) {
    message += " " + serverWeb.argName(i) + ": " + serverWeb.arg(i) + "\n";
  }

  serverWeb.send(404, F("text/plain"), message);

}

void handleAPI() {

   if(!serverWeb.hasArg("text")) {serverWeb.send(500, "text/plain", "BAD ARGS"); return;}

   text = serverWeb.arg("text");
   sizeText = serverWeb.arg("size").toInt();
   scrollText = serverWeb.arg("scroll").toInt();
   xText = serverWeb.arg("x").toInt();
   yText = serverWeb.arg("y").toInt();
   light = serverWeb.arg("light").toInt();

   String configPanel;
   configPanel = "{\"text\":\""+text+"\",\"sizeText\":\""+sizeText+"\",\"scrollText\":\""+scrollText+"\",\"xText\":\""+xText+"\",\"yText\":\""+yText+"\",\"light\":\""+light+"\"}";    
   StaticJsonDocument<512> jsonBuffer;
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, configPanel);
 
   File panelFile = SPIFFS.open("/config/panel.json", "w");
   if (!panelFile) {
   }else{
     serializeJson(doc, panelFile);
   }
   
   serverWeb.send(200,F("text/html"), "OK");

}

void handleAPIWeb() {

  String result;
  result += F("<html>");
  result += FPSTR(HTTP_HEADER);
  result += FPSTR(HTTP_APIWEB);
  result += F("</html>");
  loadConfigPanel();
  
   result.replace("{{text}}",cfgPanel.text);
   if (cfgPanel.sizeText=="1")
   {
      result.replace("{{selMin}}","Selected");
      result.replace("{{selMax}}","");
   }else if (cfgPanel.sizeText=="2")
   {
      result.replace("{{selMin}}","");
      result.replace("{{selMax}}","Selected");
   }
   if (cfgPanel.scrollText=="1")
   {
      result.replace("{{scrollOui}}","Selected");
      result.replace("{{scrollNon}}",""); 
   }else  if (cfgPanel.scrollText=="0")
   {
      result.replace("{{scrollOui}}","");
      result.replace("{{scrollNon}}","Selected");
   }
   result.replace("{{scrollText}}",cfgPanel.scrollText);
   result.replace("{{xText}}",cfgPanel.xText);
   result.replace("{{yText}}",cfgPanel.yText);
   result.replace("{{light}}",cfgPanel.light);
      
   serverWeb.send(200,F("text/html"), result);

}


void handleRoot() {
  String result;
  result += F("<html>");
  result += FPSTR(HTTP_HEADER);
  result += FPSTR(HTTP_ROOT);
  result += F("</html>");

  result.replace("{{version}}",VERSION);
  result.replace("{{ssid}}",ConfigSettings.ssid);
  result.replace("{{ip}}",ConfigSettings.ipAddress);
  result.replace("{{mask}}",ConfigSettings.ipMask);
  result.replace("{{gw}}",ConfigSettings.ipGW);
  

  serverWeb.send(200,F("text/html"), result);
  
}

void handleSavePanel()
{
  if(!serverWeb.hasArg("text")) {serverWeb.send(500, "text/plain", "BAD ARGS"); return;}

   String configPanel;
   text = serverWeb.arg("text");
   sizeText = serverWeb.arg("size").toInt();
   scrollText = serverWeb.arg("scroll").toInt();
   xText = serverWeb.arg("x").toInt();
   yText = serverWeb.arg("y").toInt();
   light = serverWeb.arg("light").toInt();

   configPanel = "{\"text\":\""+text+"\",\"sizeText\":\""+sizeText+"\",\"scrollText\":\""+scrollText+"\",\"xText\":\""+xText+"\",\"yText\":\""+yText+"\",\"light\":\""+light+"\"}";   
   Serial.println(configPanel); 
   StaticJsonDocument<512> jsonBuffer;
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, configPanel);
 
   File panelFile = SPIFFS.open("/config/panel.json", "w");
   if (!panelFile) {
   }else{
     serializeJson(doc, panelFile);
   }
  serverWeb.sendHeader(F("Location"),F("/apiweb"));
  serverWeb.send(303); 
 
}


void handleSaveConfig()
{
  if(!serverWeb.hasArg("WIFISSID")) {serverWeb.send(500, "text/plain", "BAD ARGS"); return;}

   String StringConfig;
    String ssid = serverWeb.arg("WIFISSID");
    String pass = serverWeb.arg("WIFIpassword");
    String ipAddress = serverWeb.arg("ipAddress");
    String ipMask = serverWeb.arg("ipMask");
    String ipGW = serverWeb.arg("ipGW");

   StringConfig = "{\"ssid\":\""+ssid+"\",\"pass\":\""+pass+"\",\"ip\":\""+ipAddress+"\",\"mask\":\""+ipMask+"\",\"gw\":\""+ipGW+"\"}";    
   StaticJsonDocument<512> jsonBuffer;
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, StringConfig);
 
   File configFile = SPIFFS.open("/config/config.json", "w");
   if (!configFile) {
   }else{
    // json.printTo(configFile);
     serializeJson(doc, configFile);
   }
  serverWeb.send(200, "text/html", "Save config OK ! <br><form method='GET' action='reboot'><input type='submit' name='reboot' value='Reboot'></form>");
  
 
}

void handleTools() {
  String result;
  
  result += F("<html>");
  result += FPSTR(HTTP_HEADER);
   result +=F("<h1>Tools</h1>");
   result += F("<div class='btn-group-vertical'>");
   result += F("<a href='/fsbrowser' class='btn btn-primary mb-2'>FSbrowser</button>");
   result += F("<a href='/update' class='btn btn-primary mb-2'>Update</button>");
   result += F("<a href='/reboot' class='btn btn-primary mb-2'>Reboot</button>");
  result += F("</div></body></html>");

  serverWeb.send(200, F("text/html"), result);
}

void handleReboot() {
  String result;
  
  result += F("<html>");
  result += FPSTR(HTTP_HEADER);
   result +=F("<h1>Reboot ...</h1>");
  result = result + F("</body></html>");
  serverWeb.sendHeader(F("Location"),F("/"));
  serverWeb.send(303); 

  ESP.restart();
}

void handleUpdate() {
  String result;
  
  result += F("<html>");
  result += FPSTR(HTTP_HEADER);
   result +=F("<h1>Update ...</h1>");
   result +=F("<div class='row justify-content-md-center' >");
  result +=F("<div class='col-sm-6'><form method='POST' action='StartOTA' enctype='multipart/form-data'>");
  result += F("<div class='form-group'>");
      result +=F(" <label for='ip'>File : </label>");
      result +=F(" <input type='file' name='file' class='form-control' id='file' />");
    result += F("</div>");
   result += F("<button type='submit' class='btn btn-primary mb-2'>Update</button>");
  result = result + F("</form></body></html>");

  serverWeb.send(200, F("text/html"), result);
}

void handleFSbrowser()
{
  String result;
  result += F("<html>");
  result += FPSTR(HTTP_HEADER); 
   result +=F("<h1>FSBrowser</h1>");
   result += F("<nav id='navbar-custom' class='navbar navbar-default navbar-fixed-left'>");
      result += F("      <div class='navbar-header'>");
        result += F("        <!--<a class='navbar-brand' href='#'>Brand</a>-->");
      result += F("      </div>");
   result +=F("<ul class='nav navbar-nav'>");
        
    String str = "";
    Dir dir = SPIFFS.openDir("/config/");
    while (dir.next()) {
      String tmp =  dir.fileName();
        tmp = tmp.substring(8);    
        result += F("<li><a href='#' onClick=\"readfile('");
        result +=tmp;
        result+=F("');\">");
        result +=  tmp;
        result +=F(  " ( ");
        result +=  dir.fileSize();
        result +=  F(" o)</a></li>");
        
    }
    result += F("</ul></nav>");
    result +=F("<div class='container-fluid' >");
    result +=F("  <div class='app-main-content'>");
    result+= F("<form method='POST' action='saveFile'>");
    result += F("<div class='form-group'>");
      result +=F(" <label for='file'>File : <span id='title'></span></label>");
       result +=F("<input type='hidden' name='filename' id='filename' value=''>");
      result +=F(" <textarea class='form-control' id='file' name='file' rows='10'>");     
      result +=F("</textarea>");
    result += F("</div>");
    result += F("<button type='submit' class='btn btn-primary mb-2'>Save</button>");
     result +=F("</Form>");            
    result +=F("</div>");
    result +=F("</div>");
  result +=  F("</body></html>");

  serverWeb.send(200, F("text/html"), result);
}

void handleReadfile()
{
  String result;
  String filename = "/config/"+serverWeb.arg(0);
  File file = SPIFFS.open(filename, "r");
 
  if (!file) {
    return;
  }
 
  while (file.available()) {
    result += (char) file.read();
  }
  file.close();
  serverWeb.send(200, F("text/html"), result);
}
