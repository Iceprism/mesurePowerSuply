// https://qiita.com/hurusu1006/items/f493ee4eb9998d5bd740
#include <Wire.h>
#define INA226_adr 0b10000000
#define INA226_REG_POWER_CONSUMPTION 0x03

//指定レジスタにデータを書き込む
void write_INA226_Register(uint8_t reg, uint8_t value) {
  // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
  Wire.beginTransmission(INA226_adr);

  //書き込みレジスタを指定する
  Wire.write((uint8_t)reg);

  //データを書き込む
  Wire.write((uint8_t)(value));

  // 送信完了
  Wire.endTransmission();
}

// MMA8451の指定レジスタからデータを読み出す
uint8_t readINA226_Register(uint8_t reg) {
    // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
    Wire.beginTransmission(INA226_adr);

    // 読み込みたいレジスタを送信
    Wire.write(reg);

    // 送信完了(コネクションは維持する)
    Wire.endTransmission(false);

    // INA226に対して1byteのデータを応答データに要求
    Wire.requestFrom(INA226_adr, 1);

    // readで読み取れるバイト数がなければエラー
    if (! Wire.available()) return -1;

    // INA226から1byteのデータを取得して返却
    return (Wire.read());
}

// INA226から電力消費を取得する
int16_t INA226_read(void) {
  // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
  int16_t power_consumption;
  Wire.beginTransmission(INA226_adr);

  // データ取得開始アドレスはX軸上位バイトのレジスタ
  Wire.write(INA226_REG_POWER_CONSUMPTION);

  // 送信完了(コネクションは維持する)
  Wire.endTransmission(false); // MMA8451 + friends uses repeated start!!

  // MMA8451に対して6byteのデータを応答データに要求
  Wire.requestFrom(INA226_adr, 2);

  // X軸レジスタのアドレスら計6byte取得する(X軸:14bit, Y軸:14bit, Z軸:14bit)
  /* MMA8451_REG_OUT_X_MSB: 0x01  X軸上位バイト 
   *                        0x02  X軸下位バイト
   *                        0x03  Y軸上位バイト
   *                        0x04  Y軸下位バイト
   *                        0x05  Z軸上位バイト
   *                        0x06  Z軸下位バイト
   */
  power_consumption = Wire.read(); power_consumption <<= 8; power_consumption |= Wire.read();


 return power_consumption;
}

long transPower(int16_t power_consumption){
  long trans_power_consumption=0;
  trans_power_consumption = power_consumption * 25;
  return trans_power_consumption;
  }

void setup()
{
  Serial.begin(9600);  // start Serial for output
  Wire.begin();
}


void loop()
{
  int16_t power_consumption;
  long trans_power_consumption;
  
  power_consumption = INA226_read();
  trans_power_consumption = transPower(power_consumption);

  Serial.print(power_consumption);
  Serial.print(", ");
  Serial.print(trans_power_consumption);
  Serial.println(" mW") ;
  delay(500);
}
