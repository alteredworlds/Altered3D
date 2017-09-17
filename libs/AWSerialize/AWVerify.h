#ifndef __AWVERIFY_H
#define __AWVERIFY_H

#include "md5.h"

#define USER_INFO_FIELD_LEN 60
#define SERIAL_FIELD_LEN    20
#define ASCII_SERIAL_LEN    33 //32 chars plus the terminator
#define AWSN_DELIMIT_CHAR   ':'


bool checkDeliveryID(const char* serial);
bool validate(const char* name, const char* email, const char* del, const char* serial);
void unspace(char* str);

#endif //sentinel __AWVERIFY_H