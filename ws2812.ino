#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h> //引入相应库

#define LEDS_COUNT  80    //彩灯数目
#define LEDS_PIN  2    //ESP32控制ws2812的引脚
#define CHANNEL   0    //控制通道，最多8路

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);//申请一个彩灯控制对象

u8 m_color[5][3] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}, {0, 0, 0} };//彩灯颜色数组
int delayval = 100;     //逐个彩灯显示间隔时间
int state = 1;

const char* ssid_STA = "小荔枝1";
const char* password_STA = "767701qa";

AsyncWebServer server(80); // 声明WebServer对象
AsyncWebSocket ws("/"); // WebSocket对象，url为/

String indexhtml = String("") +
                   "<!DOCTYPE html>\n" +
                   "<head>\n" +
                   "    <meta charset=\"UTF-8\">\n" +
                   "    <title>s]Smoke Lamp</title>\n" +
                   "    <script>\n" +
                   "        var ws;\n" +
                   "        if (\"WebSocket\" in window) {\n" +
                   "            ws = new WebSocket(\"ws://\" + window.location.host + \"/\"); // 建立WebSocket连接\n" +
                   "            ws.onopen = function () { // 连接建立成功时触发\n" +
                   "                document.getElementById(\"info\").innerHTML += \"WebSocket连接成功！\" + \"<br>\";\n" +
                   "                ws.send(\"connect ok!\"); // 向服务器发送数据\n" +
                   "            };\n" +
                   "            ws.onmessage = function (evt) { // 收到服务器数据时触发\n" +
                   "                document.getElementById(\"info\").innerHTML += evt.data + \"<br>\";\n" +
                   "            };\n" +
                   "            ws.onerror = function () { // 发生错误时触发\n" +
                   "                document.getElementById(\"info\").innerHTML += \"通讯发送错误！\" + \"<br>\";\n" +
                   "            };\n" +
                   "            ws.onclose = function () { // 连接关闭时触发\n" +
                   "                document.getElementById(\"info\").innerHTML += \"WebSocketTest连接已关闭！\" + \"<br>\";\n" +
                   "            };\n" +
                   "        }\n" +
                   "        else {\n" +
                   "            document.getElementById(\"info\").innerHTML = \"浏览器不支持 WebSocket！\";\n" +
                   "        }\n" +
                   "        function send_on() {\n" +
                   "            ws.send(\"ON\"); // 向服务器发送数据\n" +
                   "        }\n" +
                   "        function send_off() {\n" +
                   "            ws.send(\"OFF\"); // 向服务器发送数据\n" +
                   "        }\n" +                 
                   "    </script>\n" +
                   "</head>\n" +
                   "<body>\n" +
                   "    <h1>DuoDuo's Smoke Lamp</h1><p>"+
                   "         <h2>DuoDuo's Smoke Lamp</h2>"+
                   "    <button onmousedown = send_on()>开灯</button>\n" +
                   "    <button onmousedown = send_off()>关灯</button>\n" +
                   "    <h3>实时调试信息:</h3>\n" +
                   "    <h3><div id=\"info\"></div></h3>\n" +
                   "</body>\n" +
                   "</html>\n";
void wifi_init()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);    
    WiFi.begin(ssid_STA,password_STA);//连接到指定路由
    ws.onEvent(onEventHandle); // 绑定回调函数
    server.addHandler(&ws);    // 将WebSocket添加到服务器中
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { //注册链接"/"与对应回调函数（匿名函数形式声明）
      request->send(200, "text/html", indexhtml);                //向客户端发送响应和内容
    });
    server.begin(); //启动websocket服务器
}

// WebSocket事件回调函数
void onEventHandle(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT) // 有客户端建立连接
  {
        client->printf("Hello Client %u !", client->id()); // 向客户端发送数据
        client->ping();                                    // 向客户端发送ping  
  }
  else if (type == WS_EVT_DATA) // 收到来自客户端的数据
  {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        data[len] = 0;
        String datacome=(char *)data;
         
        if(datacome.indexOf("ON")>=0)
        {
           state = 1;
           ws.textAll("Lamp is 开灯");
        }
        else if(datacome.indexOf("OFF")>=0)
        {
           state = 0;
           ws.textAll("Lamp is 关灯");
        }
    
  }
}
void setup() 
{
  wifi_init();
  strip.begin();      //初始化彩灯控制引脚
  strip.setBrightness(4000);//设置彩灯亮度
}
void loop() 
{
  if(state)
  {
    for (int i = 0; i < LEDS_COUNT; i++) 
    {
      strip.setLedColorData(i, 255, 255, 255);//指定彩灯显示的颜色
      strip.show();//显示彩灯，不调用时彩灯不显示
      delay(delayval);
    }
  }
  else
  {
    for (int i = 0; i < LEDS_COUNT; i++) 
    {
      strip.setLedColorData(i, 0, 0, 0);//指定彩灯显示的颜色
      strip.show();//显示彩灯，不调用时彩灯不显示
      delay(delayval);
    }
  }
  delay(500);
}
