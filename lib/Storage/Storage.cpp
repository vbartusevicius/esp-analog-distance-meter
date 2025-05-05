#include "Storage.h"
#include "Parameter.h"

Storage::Storage()
{
}

void Storage::begin()
{
    prefs.begin("esp_distance_meter");
}

void Storage::saveParameter(char* name, String& value)
{
    prefs.putString(name, value);
}

String Storage::getParameter(char* name, String defaultValue)
{
    return prefs.getString(name, defaultValue);
}

bool Storage::isEmpty()
{
    for (auto &param : Parameter::REQUIRED_PARAMS) {
        if (!prefs.isKey(param)) {
            return true;
        }
    }

    return false;
}

void Storage::reset()
{
    prefs.clear();
}
