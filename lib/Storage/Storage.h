#ifndef STORAGE_H
#define STORAGE_H

#include <Preferences.h>

class Storage
{
    private:
        Preferences prefs;
    
    public:
        Storage();
        void begin();
        void saveParameter(char* name, String& value);
        String getParameter(char* name, String defaultValue = String());
        bool isEmpty();
        void reset();
};

#endif