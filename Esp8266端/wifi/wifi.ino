/*
 * MusicReminder
 * 2020-12-28 author：陈鹏涛
 */

/*
 * 库函数导入
 */
#include <ESP8266WiFi.h>  
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

/*
 * 基本参数设置
 */
#define upDataTime 2*1000   // 设置上传速率2s（1s<=upDataTime<=60s）
#define MAX_PACKETSIZE 512  // 最大字节数


/*
 * 服务器基础配置
 */
#define TCP_SERVER_ADDR "bemfa.com"     //巴法云服务器地址默认即可
#define TCP_SERVER_PORT "8344"          //服务器端口//TCP创客云端口8344//TCP设备云端口8340
#define DEFAULT_STASSID  "Mi 10"        //WIFI名称，区分大小写，不要写错
#define DEFAULT_STAPSW "*****"      //WIFI密码
String UID = "*************";  //用户私钥，可在控制台获取,修改为自己的UID
String TOPIC = "MusicReminder";         //主题名字，可在控制台新建


/*
 * tcp客户端相关初始化，默认即可
 */
WiFiClient TCPclient;         // Wifi客户端
String TcpClient_Buff = "";   // 客户端缓存
unsigned int TcpClient_BuffIndex = 0;   // 缓存索引
unsigned long TcpClient_preTick = 0;    //
unsigned long preHeartTick = 0;     // 心跳
unsigned long preTCPStartTick = 0;  // 连接
bool preTCPConnected = false;       // TCP连接状态


/*
 * 串口配置
 */
SoftwareSerial TM4C(D5, D6);  // RX=D5,TX=D6
SoftwareSerial FINGER(D2, D3);  // RX=D2,TX=D3
SoftwareSerial MP3(D7, D8);     // RX=D7,TX=D8
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FINGER);  // 指纹部分

/*
 * MP3命令定义
 */
unsigned char PLAY[4] = {0xAA, 0x02, 0x00, 0xAC}; // 播放指令
unsigned char PAUSE[4] = {0xAA, 0x03, 0x00, 0xAD}; // 暂停指令
unsigned char BEFORE[4] = {0xAA, 0x05, 0x00, 0xAF}; // 上一曲指令
unsigned char NEXT[4] = {0xAA, 0x06, 0x00, 0xB0}; // 下一曲指令
unsigned char SOUNDUP[4] = {0xAA, 0x14, 0x00, 0xBE}; // 音量增大指令
unsigned char SOUNDDOWN[4] = {0xAA, 0x15, 0x00, 0xBF}; // 音量降低指令

/*
 * 调试使用参数
 */
String my_mp3_status = "off";     // 当前的MP3状态
int my_finger_status = -1;  // 当前指纹模块的状态

unsigned char MUSIC_ON = 0x01;    // 音乐开始触发
unsigned char MUSIC_OFF = 0x02;   // 音乐暂停触发
unsigned char MUSIC_UP = 0x03;    // 音乐声音增大
unsigned char MUSIC_DOWN = 0x04;  // 音乐声音降低
unsigned char MUSIC_NEXT = 0x05;  // 下一首歌
unsigned char MUSIC_BEFORE = 0x06; //上一首歌
unsigned char FINGER_OPEN = 0x07;  //指纹解锁成功

/**************************************************************************
*                               函数头声明                                  
***************************************************************************/
// TCP初始化部分
void startTCPClient();
void doTCPClientTick();
void sendtoTCPServer(String p); 

// WIFI部分
void startSTA();
void STARTWIFI();

// MP3部分
void turnOnMP3();
void turnOffMp3();
void soundUP();
void soundDOWN();
void beforeMusic();
void nextMusic();
void processCMD(unsigned char num);

// 指纹部分
int getFingerprintIDez();
/**************************************************************************
*                               TCP函数定义
***************************************************************************/

/*
 * 初始化和服务器建立连接
 */
void startTCPClient(){
  if(TCPclient.connect(TCP_SERVER_ADDR, atoi(TCP_SERVER_PORT))){  // 配置服务器地址和端口号
    Serial.print("\nConnected to server:");
    Serial.printf("%s:%d\r\n",TCP_SERVER_ADDR,atoi(TCP_SERVER_PORT));
    String tcpTemp="";
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC+"\r\n";   // 连接服务器格式

    sendtoTCPServer(tcpTemp);   // 发送给服务器数据 
    preTCPConnected = true;     // 连接状态
    preHeartTick = millis();    // 开机后的时间
    TCPclient.setNoDelay(true);
  }
  else{
    Serial.print("Failed connected to server:");
    Serial.println(TCP_SERVER_ADDR);
    TCPclient.stop();       // 关闭连接
    preTCPConnected = false;    // 连接失败
  }
  preTCPStartTick = millis();
}


/*
 * 发送数据到TCP服务器
 */
void sendtoTCPServer(String p){
  
  if (!TCPclient.connected()) 
  {
    Serial.println("Client is not readly");
    return;
  }
  TCPclient.print(p);
  Serial.println("[Send to TCPServer]:String");
  Serial.println(p);
}



/* 
 * 检查数据，发送数据
 */
void doTCPClientTick(){
 //检查是否断开，断开后重连
  if(WiFi.status() != WL_CONNECTED) return;

  if (!TCPclient.connected()) { //断开重连

      if(preTCPConnected == true){

        preTCPConnected = false;
        preTCPStartTick = millis();
        Serial.println();
        Serial.println("TCP Client disconnected.");
        TCPclient.stop();
      }
    else if(millis() - preTCPStartTick > 1*1000)//重新连接
      startTCPClient();
  }
  // 连接成功
  else
  {
    if (TCPclient.available()) {//收数据
      char c =TCPclient.read();   // 读取服务器发送的字符
      TcpClient_Buff +=c;         // 存入缓存区
      TcpClient_BuffIndex++;      // 当前存入的索引
      TcpClient_preTick = millis(); // 更新当前时间点
      
      // 超过最大缓存的处理
      if(TcpClient_BuffIndex>=MAX_PACKETSIZE - 1){  
        TcpClient_BuffIndex = MAX_PACKETSIZE-2;
        TcpClient_preTick = TcpClient_preTick - 200;
      }
      // 更新当前时间点
      preHeartTick = millis();
    }

    if(millis() - preHeartTick >= upDataTime){//上传数据
      preHeartTick = millis();    // 更新当前时间点

      // 有数据上传时在此修改
      /*********************数据上传*******************/
      /*
        数据用#号包裹，以便app分割出来数据，&msg=#23#80#on#\r\n，即#温度#湿度#按钮状态#，app端会根据#号分割字符串进行取值，以便显示
        如果上传的数据不止温湿度，可在#号后面继续添加&msg=#23#80#data1#data2#data3#data4#\r\n,app字符串分割的时候，要根据上传的数据进行分割
      */
      String upstr = "";
      upstr = "cmd=2&uid="+UID+"&topic="+TOPIC+"&msg=#"+"#\r\n";   // 上传服务器的数据形式
      sendtoTCPServer(upstr);   // 上传命令
      upstr = "";
    }
  }
  if((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick>=200))   // 缓冲区里有数据
  {//data ready
    TCPclient.flush();
    Serial.println("Buff");
    Serial.println(TcpClient_Buff);
    //////字符串匹配，检测发了的字符串TcpClient_Buff里面是否包含&msg=on，如果有，则打开开关
    if((TcpClient_Buff.indexOf("&msg=on") > 0)) {
      turnOnMP3();
    //////字符串匹配，检测发了的字符串TcpClient_Buff里面是否包含&msg=off，如果有，则关闭开关
    }else if((TcpClient_Buff.indexOf("&msg=off") > 0)) {
      turnOffMP3();
    }
   TcpClient_Buff=""; //清空字符串，以便下次接收
   TcpClient_BuffIndex = 0;
  }
}


/**************************************************************************
                                WIFI函数定义
***************************************************************************/
/*
 * STA模式设置
 */
void startSTA(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(DEFAULT_STASSID, DEFAULT_STAPSW);
}

/*
  STARTWIFI
  检查是否需要初始化WiFi
  检查WiFi是否连接上，若连接成功启动TCP Client
  控制指示灯
*/
void STARTWIFI(){
  static bool startSTAFlag = false;   // 连接的标志
  static bool taskStarted = false;    // 连接是否成功标志
  static uint32_t lastWiFiCheckTick = 0;    // 上一次系统时间

  // 若没有连接则接入
  if (!startSTAFlag) {      
    startSTAFlag = true;
    startSTA();
    Serial.printf("Heap size:%d\r\n", ESP.getFreeHeap());
  }

  // 未连接1s重连，millis()为当前系统时间
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 1000) {
      lastWiFiCheckTick = millis();
    }
  }
  
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
      Serial.print("\r\nGet IP Address: ");
      Serial.println(WiFi.localIP());
      startTCPClient();   // Wifi连接成功后，开始TCP客户端
    }
 }
}

/**************************************************************************
                                MP3函数定义
***************************************************************************/
/*
 * 播放音乐
 */
void turnOnMP3(){
  Serial.println("MP3 Turn ON");
  MP3.write(PLAY, 4);
  my_mp3_status = "on";
}

/*
 * 暂停音乐
 */
void turnOffMP3(){
  Serial.println("MP3 Turn OFF");
  MP3.write(PAUSE, 4);
  my_mp3_status = "off";
}

/*
 * 增大音量
 */
void soundUP(){
  MP3.write(SOUNDUP, 4);
}

/*
 * 降低音量
 */
void soundDOWN(){
   MP3.write(SOUNDDOWN, 4);
}

/*
 * 下一首歌
 */
void nextMusic(){
  MP3.write(NEXT, 4);
}

/*
 * 上一首歌
 */
 void beforeMusic(){
  MP3.write(BEFORE, 4);
 }

/*
 * MP3命令处理
 */
void processCMD(unsigned char num){
  if(num == MUSIC_ON) turnOnMP3();
  if(num == MUSIC_OFF) turnOffMP3();
  if(num == MUSIC_UP) soundUP();
  if(num == MUSIC_DOWN) soundDOWN();
  if(num == MUSIC_BEFORE) beforeMusic();
  if(num == MUSIC_NEXT) nextMusic();
}

/**************************************************************************
                                指纹函数定义
***************************************************************************/
/*
 * 检验指纹函数
 */
int getFingerprintIDez() {
  // 若当前指纹不在库中，则返回-1
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // 若为当前指纹为库中指纹，则返回1
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return 1; 
}


/**************************************************************************
                                主函数定义
***************************************************************************/
// 初始化，相当于main 函数
void setup() {
  // 串口初始化
  Serial.begin(9600);
  TM4C.begin(9600);
  MP3.begin(9600);
  FINGER.begin(57600);
}

//循环
void loop() {
  STARTWIFI(); // 初始化Wifi
  doTCPClientTick();   // 初始化TCP客户端

  // 指纹的第一次识别
  if(my_finger_status!=1){
    my_finger_status = getFingerprintIDez();
    if(my_finger_status==1){
      TM4C.write(1);
    }
  }

  // TM4C单片机传输上来的指令
  if (TM4C.available()){
    processCMD(char(TM4C.read()));
  }
}
