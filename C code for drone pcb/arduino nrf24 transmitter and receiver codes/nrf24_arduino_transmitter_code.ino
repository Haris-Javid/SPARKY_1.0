#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#include <stdint.h>
void sending_data(const char* da);
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
  radio.begin();
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);  // Pipe 0 must match TX_ADDR from STM32
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);
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
  Sync_with_drone();
}

unsigned long mode_timer = millis();
void loop() {
  unsigned long current_time = millis();
  unsigned long elapsed = current_time - mode_timer;

  if(elapsed<400){
   // Serial.println("mode 1");
    sending_data("123,200,50,89");
  
  }
  else if (elapsed<500){
  //Serial.println("mode 2");
  radio.startListening();
  if (radio.available()) {
   char text[33] = {0};
   radio.read(&text, 32);
   text[32] = '\0';
   Serial.println("rec telemerty data: ");
   Serial.println(text);}
   }
  
  else{
    mode_timer =current_time;
  }
}


////////////////////////////////////////////////////////////////////////////////////

bool isSynchronized = false; // sync flag 

void Sync_with_drone() {// THIS FUNCTION WILL SEND ITS RUN TIME CLOCK TIME STAMP TO DRONE AND IF DRONE CAN SEND VALUE BACK WITH +- 0.1 SEC ERROR, CONNECTION GOOD.  
  while (!isSynchronized) {
    // 🟢 Step 1: Reset local clock
    cli();
    timer0_millis = 0;
    sei();

    // 🟢 Step 2: Send "sync" message
    const char buffer[] = "sync";
    radio.stopListening();
    radio.write(&buffer, strlen(buffer));
    radio.startListening();

    // 🟢 Step 3: Wait for drone's reply
    unsigned long start = millis();
    while (millis() - start < 200) {
      if (radio.available()) {
        char text[33] = {0};
        radio.read(&text, sizeof(text) - 1);

        Serial.print("Received: ");
        Serial.println(text);

        unsigned long drone_time = strtoul(text, NULL, 10);
        unsigned long ground_time = millis();
        long diff = (long)drone_time - (long)ground_time;

        Serial.print("Drone Time: ");
        Serial.println(drone_time);
        Serial.print("Ground Time: ");
        Serial.println(ground_time);
        Serial.print("Diff (ms): ");
        Serial.println(diff);

        if (abs(diff) <= 10) {
          Serial.println("✅ Clocks synchronized! Sending confirm...");

          // 🟢 Send "confirm" message
          delay(5);  // small wait for drone to switch to RX
          radio.stopListening();
          const char confirm_msg[] = "confirm";
          bool success = radio.write(&confirm_msg, strlen(confirm_msg));
          radio.startListening();

          if (success) {
            Serial.println("Confirmation sent.");
            isSynchronized = true;
          } else {
            Serial.println("Failed to send confirmation.");
          }
        } else {
          Serial.println("❌ Time difference too high.");
        }
        break;
      }
    }

    if (!isSynchronized) {
      Serial.println("No valid response. Retrying...\n");
      delay(200);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////

void sending_data(const char* da){
  radio.stopListening();  
  char text[32];
  strcpy(text, da);  // Copy the input into a local 32-byte buffer
  bool result = radio.write(&text, sizeof(text));
  

}



// "masterclock[0-7],pwm_1[8-10],pwm_2[11-13],pwm_3[14-16],_pwm_4[17-19]"