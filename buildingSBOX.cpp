#include <iostream>
#include <iomanip>
#define first(p) (p).first
#define second(p) (p).second

using namespace std;

const int MT = 283;
const int MT_BITS = 9;
const long long AFFINE_CONST_VECOTR = 0b1111100001111100001111100001111110001111110001111110001111110001;
const int AFFINE_CONST_SUMMATION = 0b01100011;
int S_BOX[256];

int rijndaelMultiplcation(int num1, int num2);
int rijndaelInverse(int num);
void reduction(int& number);
pair<int, string> bin(int number);
int dotProduct(int num1, int num2);
int affineTransformation(int num);
void buildSBOX(bool print=false);



// This function to find the inverse multiplcation in GF(2^8)
int rijndaelInverse(int num) {
  if(num == 0) return 0;
  for(int i = 1; i < 256; ++i) {
    if(rijndaelMultiplcation(num, i) == 1) return i;
  }
  return -1;
}

// Multiply in GF(2^8)
int rijndaelMultiplcation(int num1, int num2) {
  // Return zero if one of them is zero
  if (num1 == 0 || num2 == 0)
    return 0;
  
  int res = 0;
  while (num1 != 0) {
    // Take the LSB if it's one then go on add the number
    if((num1 & 1) == 1) {
      res ^= num2; // Add the num2
    }
    num2 <<= 1; // Instead of shifting left y by i each time left shift it by one
    num1 >>= 1; // x shifted right to reduce it by one bit
  }
  reduction(res);
  return res;
}

void reduction(int& number) {
  // Degree is the bits length but - 1
  while(true) {
    int numberOfBits = first(bin(number));
    if(numberOfBits < MT_BITS)
      return;
    // Reduce
    number ^= (MT << (numberOfBits - MT_BITS));
  }
}

// Function to return the string representation of the number and it's bits number
pair<int, string> bin(int number) {
  pair<int, string> result(0, "");
  int remainder = 0;
  while (number)
  {
    remainder = number % 2;
    number = (int) number / 2;
    first(result)++;
    second(result).append(to_string(remainder));
  }
  // After taking the binary we wanna reverse it using two pointers
  int left = 0;
  int right = second(result).length() - 1;
  while (left < right)
  {
    char tempBuffer = second(result)[left];
    second(result)[left] = second(result)[right];
    second(result)[right] = tempBuffer;
    right--;
    left++;
  }
  
  return result;
}

// We will deal with bits as vector and each bit is element
// Dot product is Segma product for each element by the another
int dotProduct(int num1, int num2) {
  // The product is defined as AND operation
  int product = num1 & num2;
  if(product == 0) return 0;
  // The sum in GF(2^8) is XOR
  int answer = 0;
  while (product != 0) {
    // Add LSB
    answer ^= (product & 1);
    // Right shift the product to get the next bit
    product >>= 1;
  }
  return answer;
}

// This is the Affine Trasnformation for Rijndael inverse while the affine just change the constants
int affineTransformation(int num) {
  int res = 0;
  for(int i = 7; i >= 0; --i) {
    // Take each 8 bit * the row you want like split the bits and take AND with this 255 to get rid of all bits
    // Except the one you want
    int row = (AFFINE_CONST_VECOTR >> (8 * i)) & 0b11111111;
    // In dot prodcut it's row * column
    int bit = dotProduct(row, num);
    // You want to put the bit in the right place that's why we start from 7
    // then we take left shift by 7 in first place and take XOR (this will combine the ones)
    res ^= (bit << i);
  }
  // Ax + b
  return res ^ AFFINE_CONST_SUMMATION;
}

void buildSBOX(bool print) {
  for(int i = 0; i < 256; ++i) {
    S_BOX[i] = affineTransformation(rijndaelInverse(i));
  }

  if(print) {
    for(int row = 0; row < 16; ++row) {
      for(int col = 0; col < 16; ++col) {
        int a = 16 * row + col;
        cout << hex << "0x" << setw(2) << setfill('0') << affineTransformation(rijndaelInverse(a)) << " ";
      }
      cout << '\n';
    }
  }
}

int main() {
  buildSBOX(true);
  return 0;
}
