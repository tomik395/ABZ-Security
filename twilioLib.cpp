#include "twilioLib.hpp"
#include <ArduinoJson.h>

bool TwilioLib::send_message(
  const String& to_number,
  const String& from_number,
  const String& message_body,
  String& response)
{

  WiFiClientSecure client;
  client.setCACert(ca_crt);
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection Failed!");
    return false;
  }

  String post_data = "To=" + urlencode(to_number) + "&From=" + urlencode(from_number) + \
                     "&Body=" + urlencode(message_body);

  // Construct headers and post body manually
  String auth_header = _get_auth_header(account_sid, auth_token);
  String http_request = "POST /2010-04-01/Accounts/" +
                        String(account_sid) + "/Messages HTTP/1.1\r\n" +
                        auth_header + "\r\n" + "Host: " + host + "\r\n" +
                        "Cache-control: no-cache\r\n" +
                        "User-Agent: ESP8266 Twilio Example\r\n" +
                        "Content-Type: " +
                        "application/x-www-form-urlencoded\r\n" +
                        "Content-Length: " + post_data.length() + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n" + post_data + "\r\n";
  
  //Serial.println("Sending API Call.");
  client.println(http_request);

  client.stop();
  //Serial.println("Closed Connection.");
  return true;
}

bool TwilioLib::get_message(
  const String& to_number, 
  const String& from_number, 
  String& response)
{

  WiFiClientSecure client;
  client.setCACert(ca_crt);
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection Failed!");
    return false;
  }

  // Construct headers and GET request manually
  String auth_header = _get_auth_header(account_sid, auth_token);
  String http_request = "GET /2010-04-01/Accounts/" +
                        String(account_sid) + "/Messages.json" +
                        "?To=" + urlencode(from_number) + 
                        "&From=" + urlencode(to_number) + 
                        " HTTP/1.1\r\n" +
                        auth_header + "\r\n" +
                        "Host: " + host + "\r\n" +
                        "Cache-control: no-cache\r\n" +
                        "User-Agent: ESP8266 Twilio Example\r\n" +
                        "Connection: close\r\n\r\n";

  //Serial.println("Sending API Call.");
  client.println(http_request);

  // Read response from server
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      // Headers finished, break to print the response
      break;
    }
  }

  // Print the response
  while(client.available()){
    String line = client.readStringUntil('\n');
    response += line;
  }
  
  client.stop();
  //Serial.println("Closed Connection.");
  return true;
}

// Comparison function to sort conversations by date
//bool compareDates(const Message &a, const Message &b)
//{
 // return a.epochTime > b.epochTime;
//}

bool TwilioLib::get_recentMessage(
  const String& to_number, 
  const String& from_number,
  String& mostRecentMessage) // Change the return type to Message struct
{
  WiFiClientSecure client;
  client.setCACert(ca_crt);
  const char* host = "api.twilio.com";
  const int httpsPort = 443;
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection Failed!");
    return false;
  }

  // Construct headers
  String auth_header = _get_auth_header(account_sid, auth_token);

  // Construct GET request manually
  String http_request = "GET /2010-04-01/Accounts/" +
                        String(account_sid) + "/Messages.json" +
                        "?To=" + urlencode(from_number) + 
                        "&From=" + urlencode(to_number) +
                        " HTTP/1.1\r\n" +
                        auth_header + "\r\n" +
                        "Host: " + host + "\r\n" +
                        "Cache-control: no-cache\r\n" +
                        "User-Agent: ESP8266 Twilio Example\r\n" +
                        "Connection: close\r\n\r\n";

  //Serial.println("Sending API Call.");
  client.println(http_request);

  // Read response from server
  String response = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      // Headers finished, break to print the response
      break;
    }
  }

  // Print the response
  while(client.available()){
    String line = client.readStringUntil('\n');
    response += line;
  }

  // Parse JSON
  String start = "[{\"body\": \"";
  String end = "\", ";

  int startIndex = response.indexOf(start);
  if (startIndex == -1) { // start string not found
    return "";
  }
  startIndex += start.length();  // start from end of start string
  int endIndex = response.indexOf(end, startIndex);
  if (endIndex == -1) { // end string not found
    return "";
  }
  mostRecentMessage = response.substring(startIndex, endIndex);

  client.stop();
  //Serial.println("Closed Connection.");
  return true;
}


/* Private function to create a Basic Auth field and parameter */
String TwilioLib::_get_auth_header(const String& user, const String& password) {
  size_t toencodeLen = user.length() + password.length() + 2;
  char toencode[toencodeLen];
  memset(toencode, 0, toencodeLen);
  snprintf(
    toencode,
    toencodeLen,
    "%s:%s",
    user.c_str(),
    password.c_str()
  );

  String encoded = base64::encode((uint8_t*)toencode, toencodeLen - 1);
  String encoded_string = String(encoded);
  std::string::size_type i = 0;

  while (i < encoded_string.length()) {
    i = encoded_string.indexOf('\n', i);
    if (i == -1) {
      break;
    }
    encoded_string.remove(i, 1);
  }
  return "Authorization: Basic " + encoded_string;
}
