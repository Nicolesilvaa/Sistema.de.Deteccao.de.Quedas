/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a minimum setup LoRa test receiver. The program listens for incoming packets
  using the frequency and LoRa settings in the LT.setupLoRa() command. The pins to access the lora device
  need to be defined at the top of the program also.

  This program does not need the DIO1 pin on the LoRa device connected.

  There is a printout on the Arduino IDE serial monitor of the valid packets received, the packet is assumed
  to be in ASCII printable text, if it's not ASCII text characters from 0x20 to 0x7F, expect weird things to
  happen on the Serial Monitor. Sample serial monitor output;

  8s  Hello World 1234567890*,RSSI,-44dBm,SNR,9dB,Length,23,Packets,7,Errors,0,IRQreg,50

  If there is a packet error it might look like this, which is showing a CRC error;

  137s PacketError,RSSI,-89dBm,SNR,-8dB,Length,23,Packets,37,Errors,2,IRQreg,70,IRQ_HEADER_VALID,IRQ_CRC_ERROR,IRQ_RX_DONE

  If there are no packets received in a 10 second period then you should see a message like this;

  112s RXTimeout

  For an example of a more detailed configuration for a receiver, see program 104_LoRa_Receiver.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>                                //the lora device is SPI based so load the SPI library
#include <SX128XLT.h>  
#include <SoftwareSerial.h>
                                                    //include the appropriate library   

SX128XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device 
#define RFBUSY 8                                //busy pin on LoRa device
#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using

#define RXBUFFER_SIZE 255                       //RX buffer size

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
int16_t PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                              //stores signal to noise ratio (SNR) of received packet


void loop()
{
  RXPacketL = LT.receiveIRQ(RXBUFFER, RXBUFFER_SIZE, 60000, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  PacketRSSI = LT.readPacketRSSI();              //read the received packets RSSI value
  PacketSNR = LT.readPacketSNR();                //read the received packets SNR value

  if (RXPacketL == 0)                            //if the LT.receive() function detects an error RXpacketL is 0
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
    Serial.println("Dados recebidos");
  }

  Serial.println();
}


void packet_is_OK()
{
  uint16_t IRQStatus;

  RXpacketCount++;
  IRQStatus = LT.readIrqStatus();                  //read the LoRa device IRQ status register
  printElapsedTime();   
  
      //Acende o led
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);

      //Aguarda intervalo de tempo em milissegundos
      delay(10000);

      //Apaga o led
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
        
      //Aguarda intervalo de tempo em milissegundos
      delay(10000); 
  
  // Serial.print(F("  "));
  // LT.printASCIIPacket(RXBUFFER, RXPacketL);        //print the packet as ASCII characters

  // Serial.print(F(",RSSI,"));
  // Serial.print(PacketRSSI);
  // Serial.print(F("dBm,SNR,"));
  // Serial.print(PacketSNR);
  // Serial.print(F("dB,Length,"));
  // Serial.print(RXPacketL);
  // Serial.print(F(",Packets,"));
  // Serial.print(RXpacketCount);
  // Serial.print(F(",Errors,"));
  // Serial.print(errors);
  // Serial.print(F(",IRQreg,"));
  // Serial.print(IRQStatus, HEX);
}


void packet_is_Error(){
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register

  printElapsedTime();                               //print elapsed time to Serial Monitor

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F(" PacketError"));
    Serial.print(F(",RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,SNR,"));
    Serial.print(PacketSNR);
    Serial.print(F("dB,Length,"));
    Serial.print(LT.readRXPacketL());               //get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(RXpacketCount);
    Serial.print(F(",Errors,"));
    Serial.print(errors);
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();                            //print the names of the IRQ registers set
  }
}


void printElapsedTime(){ 

  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}


void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("Receptor - Lasid UFBA"));
  Serial.println();

  SPI.begin();

  // Serial.println(NSS);
  // Serial.println(NRESET);
  // Serial.println(RFBUSY);
  // Serial.println(LORA_DEVICE);

  bool x = LT.begin(NSS, NRESET, RFBUSY, LORA_DEVICE);

  Serial.println(x);

  
  if (LT.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Serial.println(F("Comunicação estabelecida"));
    delay(1000);
  }
  else
  {
    Serial.println(F("Comunicação não estabelecida"));
    while (1);
  }

  LT.setupLoRa(2445000000, 0, LORA_SF7, LORA_BW_0400, LORA_CR_4_5);      //configure frequency and LoRa settings

  Serial.print(F("Receptor  - Tamanho da mensagem "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}
