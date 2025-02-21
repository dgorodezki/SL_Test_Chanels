#include <libusb.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VENDOR_ID 0x1D6B   // ID вашего устройства
#define PRODUCT_ID 0x0104  // ID вашего устройства
#define ENDPOINT_IN 0x81   // Эндпоинт для получения данных

typedef struct {
  float channel1;
  float channel2;
  float channel3;
  float channel4;
  float channel5;
} AudioSample;

void receive_audio_samples(libusb_device_handle *handle, AudioSample *samples,
                           int num_samples) {
  int transferred;
  int result =
      libusb_bulk_transfer(handle, ENDPOINT_IN, (unsigned char *)samples,
                           num_samples * sizeof(AudioSample), &transferred, 0);

  if (result != 0) {
    fprintf(stderr, "Ошибка получения данных: %d\n", result);
  } else {
    printf("Получено %d байт\n", transferred);
  }
}

void list_usb_devices(libusb_context *ctx) {
  libusb_device **devs;
  libusb_device_handle *handle = NULL;
  ssize_t cnt;
  int i;

  // Получаем список всех устройств
  cnt = libusb_get_device_list(ctx, &devs);
  if (cnt < 0) {
    perror("Error getting device list");
    return;
  }

  printf("Number of USB devices found: %d\n", cnt);

  // Перебираем все устройства
  for (i = 0; i < cnt; i++) {
    libusb_device *dev = devs[i];
    struct libusb_device_descriptor desc;

    // Получаем дескриптор устройства
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      perror("Error getting device descriptor");
      continue;
    }

    // Выводим информацию о каждом устройстве
    printf("Device %d:\n", i + 1);
    printf("  Vendor ID: 0x%04X\n", desc.idVendor);
    printf("  Product ID: 0x%04X\n", desc.idProduct);
    printf("  Device Class: 0x%02X\n", desc.bDeviceClass);
    printf("  Device Subclass: 0x%02X\n", desc.bDeviceSubClass);
    printf("  Device Protocol: 0x%02X\n", desc.bDeviceProtocol);
    printf("  USB version: %d.%d\n", desc.bcdUSB >> 8, desc.bcdUSB & 0xFF);
    printf("  Manufacturer Index: %d\n", desc.iManufacturer);
    printf("  Product Index: %d\n", desc.iProduct);
    printf("  Serial Number Index: %d\n", desc.iSerialNumber);

    printf("Open device...\n");
    if (int res = libusb_open(dev, &handle)) {
      printf("#Open Error# code: %d\n\tname: %s\n\tstr: %s\n", res,
             libusb_error_name(res), libusb_strerror(res));
      printf("\n");
      continue;
    }

    char buffer[256];
    if (desc.iManufacturer) {
      int ret = libusb_get_string_descriptor_ascii(
          handle, desc.iManufacturer, (unsigned char *)buffer, sizeof(buffer));
      if (ret > 0) {
        printf("  Manufacturer: %s\n", buffer);
      }
    }

    if (desc.iProduct) {
      int ret = libusb_get_string_descriptor_ascii(
          handle, desc.iProduct, (unsigned char *)buffer, sizeof(buffer));
      if (ret > 0) {
        printf("  Product: %s\n", buffer);
      }
    }

    if (desc.iSerialNumber) {
      int ret = libusb_get_string_descriptor_ascii(
          handle, desc.iSerialNumber, (unsigned char *)buffer, sizeof(buffer));
      if (ret > 0) {
        printf("  Serial Number: %s\n", buffer);
      }
    }
    libusb_close(handle);
    printf("\n");
  }

  // Освобождаем список устройств
  libusb_free_device_list(devs, 1);
  printf("\n ------------------- \n\n");
}

int main() {
  setlocale(LC_ALL, "Rus");
  libusb_context *context = NULL;
  libusb_device_handle *handle = NULL;
  libusb_device **devs = NULL;
  ssize_t cnt;
  int i;
  int result;

  // Инициализация libusb
  result = libusb_init(&context);
  if (result < 0) {
    fprintf(stderr, "Ошибка инициализации libusb: %d\n", result);
    return 1;
  }

  list_usb_devices(context);

  cnt = libusb_get_device_list(context, &devs);
  if (cnt < 0) {
    perror("Error getting device list");
    return 1;
  }

  for (i = 0; i < cnt; i++) {
    libusb_device *dev = devs[i];
    struct libusb_device_descriptor desc;

    // Получаем дескриптор устройства
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      perror("Error getting device descriptor");
      continue;
    }

    if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
      printf("Device %d:\n", i + 1);
      printf("  Vendor ID: 0x%04X\n", desc.idVendor);
      printf("  Product ID: 0x%04X\n", desc.idProduct);
      if (int res = libusb_open(dev, &handle)) {
        printf("Error code: %d\n\tname - %s\n\tstr - %s\n", res,
               libusb_error_name(res), libusb_strerror(res));
        return 1;
      }
      break;
    }
  }

  //// Получение аудиосигналов
  // AudioSample samples[10];  // Пример: получаем 10 сэмплов
  // receive_audio_samples(handle, samples, 10);

  //// Обработка полученных данных (например, вывод в консоль)
  // for (int i = 0; i < 10; i++) {
  //   printf("Сэмпл %d: %f %f %f %f %f\n", i, samples[i].channel1,
  //          samples[i].channel2, samples[i].channel3, samples[i].channel4,
  //          samples[i].channel5);
  // }

  // Закрытие устройства и освобождение ресурсов
  libusb_close(handle);
  libusb_exit(context);

  return 0;
}
