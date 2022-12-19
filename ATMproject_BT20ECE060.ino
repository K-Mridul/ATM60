//required libraries for my code
#include <WiFi.h>                          
#include <WiFiClientSecure.h>              //libraries to access WiFi-connection based functions
#include <HTTPClient.h>                    //library to access HTTP functions
#include <UniversalTelegramBot.h>          //library to access Telegram BOT for User Interaction 
#include <ArduinoJson.h>                   //to implement json functions 
#include <bits/stdc++.h>                   //to access to CPP functions and data structures

//personal wifi credentials
//const char* ssid = "";
//const char* password ="";

//VNIT Library wifi credentials
const char* ssid = "VeNky" ;                           //"Reading Room";
const char* password = "76543210";                     //"Password@4321";

//Google script id & credentials
String Google_Script_ID = "AKfycbwdRvDjl5ysBbjskI-4h8b1ClBOFNlnB6DvQwsHpCMR7b7WSuffB1kDo-x7VfbTGs02zw";

//telegram Bot credentials--> BOTtoken and Chat_id
#define BOTtoken "5902213155:AAEOGY6LV8s7nUU2DVIuV8RH5WEXyNFmgDk"
#define Chat_ID "1484391810"

//Creating a new Wi-Fi client with WiFiClientSecure
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

String readUser(int row,char column){

    if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + Google_Script_ID + "/exec?row=" + row + "&column=" + column + "&request=read";   //url to access data of the cell
    http.begin(url.c_str());          
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    String data = http.getString();
    if(httpCode == 200 ){
      return data;
    }        
    else{
      return "Something's Wrong!";
    }     
    http.end();        
  }
}

String writeUser(String username, String password){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;        
    String url = "https://script.google.com/macros/s/" + Google_Script_ID + "/exec?username=" + String(username) + "&password=" + String(password) + "&request=write" ;
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String data = http.getString();
    if(httpCode == 200){
      return data;
    }        
    else{
      return "Something's Wrong!";          //tO-DO 
    }     
    http.end();           
  }       
}

void updateBalance(int row, char column, String value){
  
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http; 
    String url = "https://script.google.com/macros/s/" + Google_Script_ID + "/exec?row="+ String(row) + "&column=" + column + "&value=" + value + "&request=update";
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String data = http.getString();
    if(httpCode == 200){
      return;
    }        
    else{
      return;          //tO-DO 
    }     
    http.end();
  }
}  

//function to fetch data entered at the Telegram bot UI
String getInput()
{
  int newMsg = bot.getUpdates(bot.last_message_received+1);
  while(newMsg <1){
    newMsg = bot.getUpdates(bot.last_message_received+1);
  } 
  String userInput = bot.messages[0].text;
  return userInput;
}  

//In the setup, we can put the code for WiFi connection & display of the execution over Serial Monitor
void setup(){
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  Serial.flush();      
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);        // Add root certificate for api.telegram.org
}

void thankyouMsg(){
  bot.sendMessage(Chat_ID, "Thankyou! Have a nice day! :)","");
  }

void errorMsg(){
  bot.sendMessage(Chat_ID, "Oops! Your userID or password entered is wrong! :(","");
}
  
void loop()
{
    bot.sendMessage(Chat_ID, " WELCOME to the ATM60_bot! \n For new user registration Enter -> 0 \n For existing users : \n \t \t To Withdraw, Enter -> 1 \n \t \t To Credit, Enter -> 2 \n \t \t To Check Balance, Enter -> 3 ", "");
    String userInput = getInput();
    //CREATING NEW USER ACCOUNT and returning userID to the newly registered user
    if(userInput=="0"){                                      
      bot.sendMessage(Chat_ID, "Enter a new username:","");      
      String newUsername = getInput();
      //Serial.println(newUsername); 
      
      bot.sendMessage(Chat_ID, "Set a password for your account:","");
      String newPassword = getInput();
      //Serial.println(newPassword); 

      
      String userID = writeUser(newUsername,newPassword);
      //Serial.println(userID);      
      bot.sendMessage(Chat_ID, " Dear " + newUsername + " your account has been successfully created! \n Your userID is " + userID + "\n Safe Transactions!", "");
      thankyouMsg();                                          //thankyou msg post user registration
    }

    //WITHDRAWAL of amount entered in multiple of 100Rs and updating the result  
    else if(userInput=="1"){                                 
      bot.sendMessage(Chat_ID, "Enter your userID to continue...","");
      int userID = getInput().toInt();
      String savedPassword = readUser(userID+1,'C');
      bot.sendMessage(Chat_ID, "Enter your password:","");
      String enteredPassword = getInput();
      if(savedPassword == enteredPassword){
        bot.sendMessage(Chat_ID, "Enter the number of 100Rs notes you would like to withdraw...","");
        int notes = getInput().toInt();
        int balance = readUser(userID+1,'D').toInt() - 100*notes;
        Serial.println(balance);
        String balance_str = String(balance);
        updateBalance(userID+1,'D',balance_str); 
        thankyouMsg();             
      }
      else{
        errorMsg();                                             //sending error msg to T-bot     
      }
    }
    
    //CREDIT of amount deposited by the registered user post verification
    else if(userInput=="2"){                                   
      bot.sendMessage(Chat_ID, "Enter your userID to continue...","");
      int userID = getInput().toInt();
      String savedPassword = readUser(userID+1,'C');
      bot.sendMessage(Chat_ID, "Enter your password:","");
      String enteredPassword = getInput();
      if(savedPassword == enteredPassword){
        bot.sendMessage(Chat_ID, "Enter the amount you want to deposit...","");
        int creditAmount = getInput().toInt();
        int balance = readUser(userID+1,'D').toInt() + creditAmount;
        Serial.println(balance);
        String balance_str = String(balance);
        updateBalance(userID+1,'D',balance_str);
        thankyouMsg();                                          //thankyou msg at the end of transaction
      }
      else{
        errorMsg();                                             //sending error msg to T-bot
      }
    }
    
  //BALANCE DISPLAY of registered user    
    else if(userInput=="3"){                                     
      bot.sendMessage(Chat_ID, "Enter your userID to continue...","");
      int userID = getInput().toInt();
      String savedPassword = readUser(userID+1,'C');
      bot.sendMessage(Chat_ID, "Enter your password:","");
      String enteredPassword = getInput();
      if(savedPassword == enteredPassword){
        String balance = readUser(userID+1,'D');
        bot.sendMessage(Chat_ID, "Your current balance is -> " + balance + "rupees.","");
        thankyouMsg(); 
      }
      else{
        errorMsg();                                                //sending error msg to T-bot
      }                    
    }       
}  
