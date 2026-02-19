#include <esp_mac.h> // Thư viện để làm việc với địa chỉ MAC

void setup() {
  Serial.begin(115200); // Thiết lập tốc độ truyền dữ liệu
  while(!Serial); // Chờ cho đến khi cổng Serial sẵn sàng
  
  uint8_t mac_addr[6]; // Tạo mảng để lưu 6 byte của địa chỉ MAC
  esp_err_t ret = esp_base_mac_addr_get(mac_addr); // Gọi hàm lấy địa chỉ
  
  if(ret == ESP_OK) {
    Serial.print("Dia chi MAC cua ban la: ");
    // In địa chỉ ra màn hình theo định dạng chuẩn
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", 
                  mac_addr[0], mac_addr[1], mac_addr[2], 
                  mac_addr[3], mac_addr[4], mac_addr[5]);
  } else {
    Serial.println("Khong the doc duoc dia chi MAC");
  }
}

void loop() {}