#ifndef __TWILIO__
#define __TWILIO__
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "base64.h"
#include "url_coding.hpp"
#include "digicert.h"
#include <vector>
#include <String>


class TwilioLib {
public:
        TwilioLib(
                const char* account_sid_in, 
                const char* auth_token_in,
                const char* ca_crt_in = DigiCertGlobalRootCA_crt
        )
                : account_sid(account_sid_in)
                , auth_token(auth_token_in)
                , ca_crt(ca_crt_in)
        {}
        //destructor
        ~TwilioLib() = default; 

        bool send_message(
                const String& to_number,
                const String& from_number,
                const String& message_body,
                String& response
        );

        bool get_message(
                const String& to_number, 
                const String& from_number, 
                String& response
        );

        bool get_recentMessage(
                const String& to_number, 
                const String& from_number, 
                String& mostRecentMessage
        );

private:
        String account_sid;
        String auth_token;
        const char* ca_crt;
        static String _get_auth_header(
                const String& user, 
                const String& password
        );
        
};
#endif