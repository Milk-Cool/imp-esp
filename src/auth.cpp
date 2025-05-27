#include "auth.h"

bool get_file(fs::FS fs, const char* path, uint8_t* buf, size_t size) {
    File file = fs.open(path);
    if(!file) return false;
    size_t read = file.readBytes((char*)buf, size);
    file.close();
    if(read != size) return false;
    return true;
}

bool get_id(fs::FS fs, uint8_t* id) {
    return get_file(fs, "/id", id, 16);
}
bool get_key(fs::FS fs, uint8_t* key) {
    return get_file(fs, "/key", key, 32);
}