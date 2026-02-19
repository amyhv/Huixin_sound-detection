#include <Huixin_waterDetector_inferencing.h> // Tên thư viện AI của My
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

// --- Khai báo phần cứng ---
Servo myServo;
const int SERVO_PIN = 4;
const int MIC_S3_PIN = 5;

// --- Biến ESP-NOW ---
int volumeC3 = 0;
unsigned long lastUpdateC3 = 0; 

// --- Biến logic thời gian ---
const unsigned long WINDOW_MS = 3000;
unsigned long windowStart = 0;
int beatCount = 0;
bool quayVar = 0;

// Hàm nhận dữ liệu từ C3
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingData, int len) {
  memcpy(&volumeC3, incomingData, sizeof(volumeC3));
  lastUpdateC3 = millis(); 
}

void quayXuoi() {
  myServo.write(0);
  delay(100);
  myServo.write(95);
  delay(200);
  quayVar = 1;
}

void quayNguoc() {
  myServo.write(180);
  delay(100);
  myServo.write(95);  
  delay(100);
  quayVar = 0;
}

void setup() {
  Serial.begin(115200);
  // Đợi Serial kết nối (cho ESP32-S3)
  while (!Serial && millis() < 5000); 
  
  myServo.attach(SERVO_PIN);
  myServo.write(95);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    esp_now_register_recv_cb(OnDataRecv);
  }

  windowStart = millis();
  Serial.println("System Ready - AI Skip Active");
}

void loop() {
  // --- PHẦN 1: CHẠY AI (My copy phần lấy mẫu Mic từ file ví dụ vào đây nhé) ---
  // Để code này chạy được, My cần copy đoạn 'inference' từ ví dụ esp32_s3_microphone sang.
  // Ở đây mình viết logic Skip dựa trên kết quả AI:
  
  ei_impulse_result_t result = { 0 }; 
  // run_classifier(&signal, &result, false); // Hàm này lấy từ ví dụ mẫu

  // LOGIC SKIP: Nếu AI thấy là NOISE (1) hoặc WATER (2) với độ tin cậy > 70%
  if (result.classification[1].value > 0.7 || result.classification[2].value > 0.7) {
    return; // THOÁT LOOP NGAY LẬP TỨC - KHÔNG CHẠY CODE DƯỚI
  }

  // --- PHẦN 2: LOGIC CŨ CỦA MY (Chỉ chạy khi AI không Skip) ---
  unsigned long now = millis();
  int volumeS3 = analogRead(MIC_S3_PIN);

  // Nếu mất kết nối với C3 quá 2 giây thì không làm gì
  if (now - lastUpdateC3 > 2000) return;

  // Reset cửa sổ 3 giây
  if (now - windowStart >= WINDOW_MS) {
    if (quayVar == 1 && beatCount < 3) {
      quayNguoc();
    }
    windowStart = now;
    beatCount = 0;
  }

  if (volumeC3 > 0) {
    float ratio = (float)volumeS3 / (float)volumeC3;
    // Điều kiện kích hoạt: Tỉ lệ âm thanh và độ to tối thiểu
    bool isBeat = (ratio >= 1.5f && volumeS3 > 1800);

    if (isBeat) {
      beatCount++;
      if (quayVar == 0) {
        quayXuoi();
      }
    }
  }

  delay(50);
}