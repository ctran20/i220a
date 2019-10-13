#include "big-bits.h"
#include "hex-util.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/** Provide concrete definition for struct BigBits to flesh out
 *  abstract BigBits data type.
 */
struct BigBits {
  const char *hexStr;
  int charNum;
};

/** Return a pointer to a representation of a big integer with value
 *  corresponding to the non-empty hexadecimal string hex.  Note that
 *  hex will only contain hexadecimal characters '0' - '9', 'a' - 'f'
 *  and 'A' - 'F' terminated by a NUL '\0' char.
 *
 *  The string hex may not remain valid after this function returns.
 *
 *  Returns NULL on error with errno set "appropriately".
 */
const BigBits *
newBigBits(const char *hex)
{
  assert(CHAR_BIT == 8);
  
  struct BigBits *ret = malloc(sizeof(struct BigBits *));
  char *s = malloc((strlen(hex)+1));
  
  if (ret == NULL || s == NULL) {
    printf("Malloc failure: %s\n", strerror(errno));
    return NULL;
  }
  strcpy(s, hex);
  
  ret->hexStr = s;
  ret->charNum = strlen(hex)+1;
  return ret;
}

/** Frees all resources used by currently valid bigBits.  bigBits
 *  is not valid after this function returns.
 */
void
freeBigBits(BigBits *bigBits)
{
  free(bigBits);
}


/** Return a lower-case hex string representing bigBits. Returned
 *  string should not contain any non-significant leading zeros.
 *  Returns NULL on error with errno set "appropriately".  (Note that
 *  there is no call to free the corresponding string).
 */
const char *
stringBigBits(const BigBits *bigBits)
{
  if (bigBits == NULL) {
    printf("stringBigBits received null: %s\n", strerror(errno));
    return NULL;
  }
  return bigBits->hexStr;
}


/** Return a new BigBits which is the bitwise-& of bigBits1 and bigBits2.
 *  Returns NULL on error with errno set "appropriately".
 */
const BigBits *
andBigBits(const BigBits *bigBits1, const BigBits *bigBits2)
{
  int b1;
  int b2;
	
  if (bigBits1 == NULL || bigBits2 == NULL) {
    printf("stringBigBits received null: %s\n", strerror(errno));
    return NULL;
  }
  
  int len = (bigBits1->charNum > bigBits2->charNum) ? bigBits1->charNum : bigBits2->charNum;
  
  char *hex = malloc(len);
  
  for(int i = 0; i < len-1;i++) {
	  if((bigBits1->charNum - len + i) < 0){
		 hex[i] = bigBits2->hexStr[i];
	  }
	  else if((bigBits2->charNum - len + i) < 0){
		 hex[i] = bigBits1->hexStr[i];
	  }
	  else {
		b1 = charToHexet(bigBits1->hexStr[bigBits1->charNum - len + i]);
		b2 = charToHexet(bigBits2->hexStr[bigBits2->charNum - len + i]);
		hex[i] = hexetToChar(b1 & b2);
	  }
  }
  
  hex[len] = '\0';

  return newBigBits(hex);
}

/** Return a new BigBits which is the bitwise-| of bigBits1 and bigBits2.
 *  Returns NULL on error with errno set "appropriately".
 */
const BigBits *
orBigBits(const BigBits *bigBits1, const BigBits *bigBits2)
{
  int b1;
  int b2;
	
  if (bigBits1 == NULL || bigBits2 == NULL) {
    printf("stringBigBits received null: %s\n", strerror(errno));
    return NULL;
  }
  
  int len = (bigBits1->charNum > bigBits2->charNum) ? bigBits1->charNum : bigBits2->charNum;
  
  char *hex = malloc(len);
  
  for(int i = 0; i < len-1;i++) {
	  if((bigBits1->charNum - len + i) < 0){
		 hex[i] = bigBits2->hexStr[i];
	  }
	  else if((bigBits2->charNum - len + i) < 0){
		 hex[i] = bigBits1->hexStr[i];
	  }
	  else {
		b1 = charToHexet(bigBits1->hexStr[bigBits1->charNum - len + i]);
		b2 = charToHexet(bigBits2->hexStr[bigBits2->charNum - len + i]);
		hex[i] = hexetToChar(b1 | b2);
	  }
  }
  
  hex[len] = '\0';

  return newBigBits(hex);
  
}

/** Return a new BigBits which is the bitwise-^ of bigBits1 and bigBits2.
 *  Returns NULL on error with errno set "appropriately".
 */
const BigBits *
xorBigBits(const BigBits *bigBits1, const BigBits *bigBits2)
{
  int b1;
  int b2;
	
  if (bigBits1 == NULL || bigBits2 == NULL) {
    printf("stringBigBits received null: %s\n", strerror(errno));
    return NULL;
  }
  
  int len = (bigBits1->charNum > bigBits2->charNum) ? bigBits1->charNum : bigBits2->charNum;
  
  char *hex = malloc(len);
  
  for(int i = 0; i < len-1;i++) {
	  if((bigBits1->charNum - len + i) < 0){
		 hex[i] = bigBits2->hexStr[i];
	  }
	  else if((bigBits2->charNum - len + i) < 0){
		 hex[i] = bigBits1->hexStr[i];
	  }
	  else {
		b1 = charToHexet(bigBits1->hexStr[bigBits1->charNum - len + i]);
		b2 = charToHexet(bigBits2->hexStr[bigBits2->charNum - len + i]);
		hex[i] = hexetToChar(b1 ^ b2);
	  }
  }
  
  hex[len] = '\0';

  return newBigBits(hex);
}
