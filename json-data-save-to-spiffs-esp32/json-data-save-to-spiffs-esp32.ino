// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License
//
// Modified by Syed Razwanul Haque (Nabil), https://www.github.com/Nabilphysics
// for
// json data saving to ESP32 SPIFFS
//https://arduinojson.org/v6/doc/serialization/
#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>


#define FORMAT_SPIFFS_IF_FAILED true

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  while (!Serial) continue;

  //SPIFFS.remove("/hello.txt");
  // Memory pool for JSON object tree.
  //
  // Inside the brackets, 200 is the size of the pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  //StaticJsonBuffer<400> jsonBuffer;

  const size_t bufferSize = JSON_OBJECT_SIZE(3);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  // StaticJsonBuffer allocates memory on the stack, it can be
  // replaced by DynamicJsonBuffer which allocates in the heap.
  //
  // DynamicJsonBuffer  jsonBuffer(200);

  // Create the root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& root = jsonBuffer.createObject();

  // Add values in the object
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root.set<long>("time", 1351824120);
  root["sensor"] = "gps";
  root["time"] = 1351824120;

  // Add a nested array.
  //
  // It's also possible to create the array separately and add it to the
  // JsonObject but it's less efficient.
  JsonArray& d1 = root.createNestedArray("d1");
  for (int k = 0; k < 750; k++) {
    d1.add(4000 + k);
  }

  JsonArray& d2 = root.createNestedArray("d2");
  for (int k = 0; k < 750; k++) {
    d2.add(k);
  }

  //This will save entire json data to SPIFFS
  File file = SPIFFS.open("/test.json", "w+");
  root.printTo(file);
  file.close();

  // List the File
  listDir(SPIFFS, "/", 0);



  Serial.println("Reading JSON from SPIFFS: ");
  readFile(SPIFFS, "/test.json");
  
  //root.printTo(Serial);
  Serial.println();
  Serial.println();
  Serial.print("Array Size: ");
  Serial.print(d1.size() + d2.size());
  Serial.print(" SPIFFS totalBytes: ");
  Serial.print(SPIFFS.totalBytes());
  Serial.print(" SPIFFS usedBytes: ");
  Serial.print(SPIFFS.usedBytes());
  Serial.print("  Max Alloc Heap: ");
  Serial.print(ESP.getMaxAllocHeap());
  Serial.print("  Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.print("  Total Heap: ");
  Serial.println(ESP.getHeapSize());

  //root.prettyPrintTo(Serial);
  // This prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
}

void loop() {
  // not used in this example
}

// See also
// --------
//
// The website arduinojson.org contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// serialization problem.
// Please check it out at: https://arduinojson.org/
//
// The book "Mastering ArduinoJson" contains a tutorial on serialization.
// It begins with a simple example, like the one above, and then adds more
// features like serializing directly to a file or an HTTP request.
// Please check it out at: https://arduinojson.org/book/
