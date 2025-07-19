#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#include <stdint.h>
// nRF24L01 SPI commands
#define R_REGISTER    0x00
#define W_REGISTER    0x20

// Register addresses
#define CONFIG       0x00
#define EN_AA        0x01
#define EN_RXADDR    0x02
#define SETUP_AW     0x03
#define SETUP_RETR   0x04
#define RF_CH        0x05
#define RF_SETUP     0x06
#define STATUS       0x07
#define OBSERVE_TX   0x08
#define RX_ADDR_P0   0x0A
#define TX_ADDR      0x10

extern volatile unsigned long timer0_millis; // used to reset timer to zero to complete handshake 

// Manually read a register using SPI
uint8_t readRegister(uint8_t reg) {
  uint8_t result;
  digitalWrite(8, LOW); // CSN low  
  SPI.transfer(R_REGISTER | (reg & 0x1F));
  result = SPI.transfer(0xFF);
  digitalWrite(8, HIGH); // CSN high
  return result;
}
void writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(8, LOW); // CSN low
  SPI.transfer(W_REGISTER | (reg & 0x1F));
  SPI.transfer(value);
  digitalWrite(8, HIGH); // CSN high
}

// Read multiple bytes from register (for addresses)
void readRegisterMulti(uint8_t reg, uint8_t* buf, uint8_t len) {
  digitalWrite(8, LOW);
  SPI.transfer(R_REGISTER | (reg & 0x1F));
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = SPI.transfer(0xFF);
  }
  digitalWrite(8, HIGH);
}

void printRegisterBytes(uint8_t reg, uint8_t len) {
  uint8_t buffer[10];
  readRegisterMulti(reg, buffer, len);
  for (uint8_t i = 0; i < len; i++) {
    if (buffer[i] < 0x10) Serial.print("0");
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void printRadioRegisters() {
  Serial.println("---- nRF24L01 Registers ----");

  Serial.print("CONFIG: 0x");
  Serial.println(readRegister(CONFIG), HEX);

  Serial.print("EN_AA: 0x");
  Serial.println(readRegister(EN_AA), HEX);

  Serial.print("EN_RXADDR: 0x");
  Serial.println(readRegister(EN_RXADDR), HEX);

  Serial.print("SETUP_AW: 0x");
  Serial.println(readRegister(SETUP_AW), HEX);

  Serial.print("SETUP_RETR: 0x");
  Serial.println(readRegister(SETUP_RETR), HEX);

  Serial.print("RF_CH: 0x");
  Serial.println(readRegister(RF_CH), HEX);

  Serial.print("RF_SETUP: 0x");
  Serial.println(readRegister(RF_SETUP), HEX);

  Serial.print("STATUS: 0x");
  Serial.println(readRegister(STATUS), HEX);

  Serial.print("OBSERVE_TX: 0x");
  Serial.println(readRegister(OBSERVE_TX), HEX);

  Serial.print("RX_ADDR_P0: ");
  printRegisterBytes(RX_ADDR_P0, 5);

  Serial.print("TX_ADDR: ");
  printRegisterBytes(TX_ADDR, 5);

  Serial.println("----------------------------");
}




RF24 radio(7,8); // CE, CSN


const uint8_t address[5] = {'D', 'R', 'O', 'N', 'E'};

void setup() {
Serial.begin(9600);
  SPI.begin();
  radio.begin();
  radio.setAddressWidth(5);
  radio.setAutoAck(true); 
  radio.setPayloadSize(4);  // Match STM32's payload size
  radio.setRetries(1, 1);   // 5 retries, 1500µs delay (0x15)
  radio.setAutoAck(0, true);  // Enable AA only on pipe 0
  radio.setAutoAck(1, false); // Disable on other pipes
  radio.setAutoAck(2, false); // Disable on other pipes
  radio.setAutoAck(3, false); // Disable on other pipes
  radio.setAutoAck(4, false); // Disable on other pipes
  radio.setAutoAck(5, false); // Disable on other pipes
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);  // Pipe 0 must match TX_ADDR from STM32
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(78);
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();


if (!radio.isChipConnected()) {
  Serial.println("nRF24L01 not detected!");
} else {
  Serial.println("nRF24L01 is connected.");
}



  Serial.println("Radio initialized and transmiting.");
  printRadioRegisters();
  delay(1000);
}

unsigned long mode_timer = millis();

void loop() {

  if(millis()-mode_timer<=180){
 uint8_t pwm_data1[4] = {0b1100100,0b1100100,0b1100100,0b0};
  sending_data(pwm_data1);
  }
  else if(millis()-mode_timer<=210){
  uint8_t pwm_data2[4] = {0b1100100,0b1100100,0b1100100,0b1};
  sending_data(pwm_data2);}


  else if(millis()-mode_timer<=250){
  receiver_mode();}
  else{
    mode_timer = millis();
  Serial.println("-----------------");
  }

}


////////////////////////////////////////////////////////////////////////////////////

void sending_data(const uint8_t* da){
  radio.stopListening();  
  bool result = radio.write(da,4);}

void sending_data_char(const char* da){
  radio.stopListening();  
  char text[32];
  strcpy(text, da);  // Copy the input into a local 32-byte buffer
  bool result = radio.write(&text, sizeof(text));

}

void receiver_mode(){
   radio.startListening();
    if (radio.available()) {
  uint8_t fromedrone[8];
  radio.read(fromedrone, 4);
  uint32_t number = 0;
  memcpy(&number, fromedrone, 4);
  Serial.println(number);
}}

// "pwm_1[0-3],pwm_2[4-6],pwm_3[7-9],_pwm_4[10-12],1(bit to tell drone to trasnmite)"

