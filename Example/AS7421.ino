     #define I2C_SCL 13 
     #define I2C_SDA 17

    #include <Wire.h>
     
     uint8_t address, error, nDevices, add;


#define CFG_MISC 0x38
#define CFG_LED_MULT 0x39
#define TEMP_COMPDAC 0x3C
#define CFG_LED_WAIT 0x3D
#define CFG_PINMAP 0x3E

#define ENABLE 0x60
#define LTF_ITIME_L 0x61
#define LTF_ITIME_M 0x62
#define LTF_ITIME_H 0x63
#define LTF_WTIME_L 0x64
#define LTF_WTIME_M 0x65
#define LTF_WTIME_H 0x66

#define CFG_LTF 0x67
#define CFG_LED 0x68
#define LTF_ICOUNT 0x69
#define CFG_RAM 0x6A
#define CFG_GPIO 0x6B
#define INT_ENABLE 0x6C
#define CFG_AZ 0x6D

#define STATUS_0 0x70
#define STATUS_1 0x71
#define STATUS_2 0x72
#define STATUS_3 0x73
#define STATUS_4 0x74
#define STATUS_5 0x75
#define STATUS_6 0x76
#define STATUS_7 0x77

#define CH0_DATA  0x80

int i;
uint8_t data_buf[2];

uint16_t data[64];

void setup(){


    
    Wire.begin(I2C_SDA, I2C_SCL, 400000);
     
    Serial.begin(115200);
    while (!Serial);             // Leonardo: wait for serial monitor
    //Serial.println("Scanning");

    add=0x64;



    Wire.beginTransmission(add);
    Wire.write(LTF_ITIME_H); 
    Wire.write(0x01); 
    Wire.endTransmission();

    Wire.beginTransmission(add);
    Wire.write(LTF_WTIME_H); 
    Wire.write(0x01); 
    Wire.endTransmission();

    Wire.beginTransmission(add);
    Wire.write(CFG_LTF); 
    Wire.write(0b00011100); 
    Wire.endTransmission();

    Wire.beginTransmission(add);
    Wire.write(LTF_ICOUNT); 
    Wire.write(0x01); 
    Wire.endTransmission();


  
  i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_0); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_0 ");
    Serial.println(data_buf[0], HEX);

     i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_1); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_1 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_2); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_2 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_3); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_3 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_4); 
  Wire.endTransmission();
  Wire.requestFrom(add, 4);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_1 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_5); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_5 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_6); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_6 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_7); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("STATUS_7 ");
    Serial.println(data_buf[0], HEX);

         i=0;

  Wire.beginTransmission(add);
  Wire.write(CH0_DATA); 
  Wire.endTransmission();
  Wire.requestFrom(add, 2);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    Serial.print("CH0_DATA ");    
    Serial.print(data_buf[1], HEX);
    Serial.print(" ");
    Serial.println(data_buf[0], HEX);



    Wire.beginTransmission(add);
    Wire.write(ENABLE); 
    Wire.write(0b00000011); 
    Wire.endTransmission();

}



void loop(){

         i=0;

  Wire.beginTransmission(add);
  Wire.write(STATUS_7); 
  Wire.endTransmission();
  Wire.requestFrom(add, 1);
  while(Wire.available()){ 
    data_buf[i]=Wire.read();
    i++;
  }
    Wire.endTransmission();

    //Serial.print("STATUS_7 ");
    //Serial.println(data_buf[0], HEX);

  i=0;

  Wire.beginTransmission(add);
  Wire.write(CH0_DATA); 
  Wire.endTransmission();
  Wire.requestFrom(add, 128);
  while(Wire.available()){ 
    data_buf[0]=Wire.read();
    data_buf[1]=Wire.read();

    data[i]= ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];
    i++;
  }
    Wire.endTransmission();

    for(i=0; i<64; i++){
      Serial.print(data[i]);
      Serial.print(",");
    }
    Serial.println();

    Wire.beginTransmission(add);
    Wire.write(ENABLE); 
    Wire.write(0b00000011); 
    Wire.endTransmission();

    delay(500);


    /*

   uint16_t vis =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];


    i=0;

  Wire.beginTransmission(add);
    Wire.write(0x97); 
    Wire.endTransmission();
    Wire.requestFrom(add, 2);
    while(Wire.available()){ 
      data_buf[i]=Wire.read();
      i++;
    }
    Wire.endTransmission();

   uint16_t red =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];

    i=0;

  Wire.beginTransmission(add);
    Wire.write(0x99); 
    Wire.endTransmission();
    Wire.requestFrom(add, 2);
    while(Wire.available()){ 
      data_buf[i]=Wire.read();
      i++;
    }
    Wire.endTransmission();

   uint16_t green =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];

    i=0;

  Wire.beginTransmission(add);
    Wire.write(0x9B); 
    Wire.endTransmission();
    Wire.requestFrom(add, 2);
    while(Wire.available()){ 
      data_buf[i]=Wire.read();
      i++;
    }
    Wire.endTransmission();

   uint16_t blue =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];

       i=0;

  Wire.beginTransmission(add);
    Wire.write(0x9D); 
    Wire.endTransmission();
    Wire.requestFrom(add, 2);
    while(Wire.available()){ 
      data_buf[i]=Wire.read();
      i++;
    }
    Wire.endTransmission();

   uint16_t clear =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];

       i=0;

  Wire.beginTransmission(add);
    Wire.write(0x9F); 
    Wire.endTransmission();
    Wire.requestFrom(add, 2);
    while(Wire.available()){ 
      data_buf[i]=Wire.read();
      i++;
    }
    Wire.endTransmission();

   uint16_t ir =  ( ( uint16_t ) data_buf[ 1 ] << 8 ) | data_buf[ 0 ];

  Serial.print(ir);   Serial.print(",");
  Serial.print(red);  Serial.print(",");
  Serial.print(green); Serial.print(",");
  Serial.print(blue); Serial.print(",");
  Serial.print(vis);  Serial.print(",");
  Serial.print(clear); Serial.print(",");
  Serial.println();



    delay(15000);*/
    
   // delay(5000);
 
}



