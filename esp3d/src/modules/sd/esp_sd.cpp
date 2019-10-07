/*
  esp_sd.cpp - ESP3D SD support class

  Copyright (c) 2014 Luc Lebosse. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "../../include/esp3d_config.h"
#ifdef SD_FEATURE
#include "esp_sd.h"
#include "../../core/genLinkedList.h"
#ifdef SD_TIMESTAMP_FEATURE
#include <time.h>
#endif //SD_TIMESTAMP_FEATURE
#include <FS.h>


#define ESP_MAX_SD_OPENHANDLE 4
File tSDFile_handle[ESP_MAX_SD_OPENHANDLE];

bool ESP_SD::_started = false;

//constructor
ESP_SD::ESP_SD()
{
}

//destructor
ESP_SD::~ESP_SD()
{
}

//helper to format size to readable string
String & ESP_SD::formatBytes (uint64_t bytes)
{
    static String res;
    if (bytes < 1024) {
        res = String ((uint16_t)bytes) + " B";
    } else if (bytes < (1024 * 1024) ) {
        res = String ((float)(bytes / 1024.0),2) + " KB";
    } else if (bytes < (1024 * 1024 * 1024) ) {
        res = String ((float)(bytes / 1024.0 / 1024.0),2) + " MB";
    } else {
        res = String ((float)(bytes / 1024.0 / 1024.0 / 1024.0),2) + " GB";
    }
    return res;
}

ESP_SDFile::ESP_SDFile(const char * name, const char * filename, bool isdir, size_t size)
{
    _isdir = isdir;
    _dirlist = "";
    _isfakedir = isdir;
    _index = -1;
    _filename = filename;
    _name = name;
#ifdef SD_TIMESTAMP_FEATURE
    memset (&_lastwrite,0,sizeof(time_t));
#endif //SD_TIMESTAMP_FEATURE 
    _iswritemode = false;
    _size = size;
}

ESP_SDFile::~ESP_SDFile()
{
    //log_esp3d("Destructor %s index %d",(_isdir)?"Dir":"File", _index);
}

ESP_SDFile::operator bool() const
{
    if ((_index != -1) || (_filename.length() > 0)) {
        //log_esp3d("Bool yes %d %d",_index,  _filename.length());
        return true;
    } else {
        return false;
    }
}

bool ESP_SDFile::isOpen()
{
    return !(_index == -1);
}

const char* ESP_SDFile::name() const
{
    return _name.c_str();
}

const char* ESP_SDFile::filename() const
{
    return _filename.c_str();
}

bool ESP_SDFile::isDirectory()
{
    return _isdir;
}

size_t ESP_SDFile::size()
{
    return _size;
}

#ifdef SD_TIMESTAMP_FEATURE
time_t ESP_SDFile::getLastWrite()
{
    return _lastwrite;
}
#endif //SD_TIMESTAMP_FEATURE

int ESP_SDFile::available()
{
    if (_index == -1 || _isdir) {
        return 0;
    }
    return tSDFile_handle[_index].available();
}

size_t ESP_SDFile::write(uint8_t i)
{
    if ((_index == -1) || _isdir) {
        return 0;
    }
    return tSDFile_handle[_index].write (i);
}

size_t ESP_SDFile::write(const uint8_t *buf, size_t size)
{
    if ((_index == -1) || _isdir) {
        return 0;
    }
    return tSDFile_handle[_index].write (buf, size);
}

int ESP_SDFile::read()
{
    if ((_index == -1) || _isdir) {
        return -1;
    }
    return tSDFile_handle[_index].read();
}

size_t ESP_SDFile::read(uint8_t* buf, size_t size)
{
    if ((_index == -1) || _isdir) {
        return -1;
    }
    return tSDFile_handle[_index].read(buf, size);
}

void ESP_SDFile::flush()
{
    if ((_index == -1) || _isdir) {
        return;
    }
    tSDFile_handle[_index].flush();
}

ESP_SDFile& ESP_SDFile::operator=(const ESP_SDFile & other)
{
    //log_esp3d("Copy %s", other._filename.c_str());
    _isdir = other._isdir;
    _isfakedir = other._isfakedir;
    _index = other._index;
    _filename = other._filename;
    _name = other._name;
    _size = other._size;
    _iswritemode = other._iswritemode;
    _dirlist = other._dirlist;
#ifdef SD_TIMESTAMP_FEATURE
    memcpy(&_lastwrite, &(other._lastwrite), sizeof (time_t));
#endif //SD_TIMESTAMP_FEATURE
    return *this;
}

#endif //SD_FEATURE
