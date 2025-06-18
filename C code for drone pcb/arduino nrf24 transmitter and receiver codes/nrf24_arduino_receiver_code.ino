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
#define RX_DR 6

extern volatile unsigned long timer0_millis; // used to reset timer to zero to complete handshake 


void receiver_mode();
void Send_Telementary();
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

  Serial.print("RX_PW_P0: 0x");
  Serial.println(readRegister(0x11), HEX);  // RX_PW_P0 register
  
  Serial.println("----------------------------");
}

RF24 radio(7, 8); // CE, CSN

const uint8_t address[5] = {'D', 'R', 'O', 'N', 'E'};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  radio.setAddressWidth(5);
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);  // Pipe 0 must match TX_ADDR from STM32
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);
  radio.setCRCLength(RF24_CRC_16);
  radio.startListening();
  Serial.println("Listening started.");
  Serial.println("Radio initialized and listening.");
  printRadioRegisters();
   Sync_with_Ground_station(); // WAITS TO FOR HAND SHAKE TO SYNC CLOCK WITH GROUND STATION
}


unsigned long mode_timer = millis();
void loop() {
 
 unsigned long current_time = millis();
  unsigned long elapsed = current_time - mode_timer;

  if(elapsed<400){
    //Serial.println("mode 1");
    radio.startListening();
    if (radio.available()) {
     char text[33] = {0};
      radio.read(&text, 32);
      text[32] = '\0';
      Serial.println("rec:");
      Serial.println(text);}
      }
  else if (elapsed<500){
  //  Serial.println("mode 2");
    radio.stopListening();
    sending_data("height:25mm, speed: 20 m/s");
  }
  else{
    mode_timer =current_time;
  }

}



  





///////////////////////////////////////////////////////////////////////////////////

bool isSynchronized = false;

void Sync_with_Ground_station() {   // USED TO SYNC WITH GROUND STATION
  while (!isSynchronized) {
    sync_receiver_mode(); }
}

void sync_receiver_mode() {  // USED TO SYNC WITH GGROUND STATION
  if (radio.available()) {
    char text[33] = {0};
    radio.read(&text, 32);
    text[32] = '\0';

    Serial.print("Received: ");
    Serial.println(text);

    if (strcmp(text, "sync") == 0) {
      // 🟢 Reset local clock
      cli();
      timer0_millis = 20;    // OFFSET TO INTERNAL CLOCK TO ALLOW BASE STATION TO SYNC
      sei();

      // 🟢 Send back timestamp
      delay(5);
      radio.stopListening();
      unsigned long now = millis() + 30;
      char buffer[32];
      sprintf(buffer, "%lu", now);
      delay(50);  // mimic communication offset

      bool success = radio.write(&buffer, strlen(buffer));
      if (success) {
        Serial.print("Sent sync time: ");
        Serial.println(buffer);
      } else {
        Serial.println("Send failed");
      }
      radio.startListening();

    } else if (strcmp(text, "confirm") == 0) {
      Serial.println("✅ Received sync confirmation!");
      isSynchronized = true;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////



void receiver_mode(){
   radio.startListening();
    if (radio.available()) {
  char text[33] = {0};
  radio.read(&text, 32);
  Serial.print("Received: ");
  Serial.println(text);

    }}

void sending_data(const char* da){
  radio.stopListening();  
  char text[32];
  strcpy(text, da);  // Copy the input into a local 32-byte buffer
  bool result = radio.write(&text, sizeof(text));

}
