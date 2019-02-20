/*
 * Barcode Scanner Arduino
 * Original code by: RadhiFadhillah (github) https://github.com/felis/USB_Host_Shield_2.0/issues/323
 * Modified by: Dirga Brajamusti (dirgabrajamusti::github)
 */
#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <Servo.h>

String hasil;
bool proses;
int timeout = 2000;
int timeout2 = 3000;
int flag = 1;

Servo servo1;
Servo servo2;
Servo servo3;


class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);
  hasil = hasil + (char)ascii;
}

void MyParser::OnScanFinished() {
  Serial.println(hasil);
  proses = true;
}

USB          Usb;
//USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay(200);

  Hid.SetReportParser(0, &Parser);
  servo1.attach(2);
  servo2.attach(4);
  servo3.attach(8);
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
}

void loop() {
  Usb.Task();
  //Cek variable 
  if(hasil.substring(0,1) == "1" && proses == true){
    Serial.println("Hasil: 1");
    Serial.println("Membuka gerbang");
    servo1.write(90);
    delay(1500);
    servo1.write(0);
    Serial.println("Selesai");
    proses = false;
    hasil = "";
  }
  if(hasil.substring(0,1) == "2" && proses == true){
    Serial.println("Hasil: 2");
    Serial.println("Membuka gerbang");
    servo1.write(90);
    servo2.write(90);
    if(timeout && flag){
      timeout--;
    }else{
      servo1.write(0);
      servo2.write(0);
      Serial.println("Selesai");
      hasil = "";
      proses = false;
      timeout = 2000; 
      flag = 1;
    }
  }
  if(hasil.substring(0,1) == "3" && proses == true){
    Serial.println("Hasil: 3");

    Serial.println("Membuka gerbang");
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    if(timeout2 && flag){
      timeout2--;
    }else{
      servo1.write(0);
      servo2.write(0);
      servo3.write(0);
      Serial.println("Selesai");
      hasil = "";
      proses = false;
      timeout2 = 3000;
      flag = 1; 
    }
  }
}
