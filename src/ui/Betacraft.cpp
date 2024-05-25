#include "Betacraft.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "../core/FileSystem.h"
#include "../core/Settings.h"

using json = nlohmann::json;

json fallbackLanguageJson;
json languageJson;

QString bc_translate(const char *key) {

    if (languageJson.contains(key)) {
        return QString::fromStdString(languageJson[key]);
    }

    if (fallbackLanguageJson.contains(key)) {
        return QString::fromStdString(fallbackLanguageJson[key]);
    }

    return QString("");
}

void bc_translate_init() {

    bc_settings *settings = bc_settings_get();

    std::string fallbackLanguagePath = "lang/English.json";

    std::string languagePath = "lang/";
    languagePath += settings->language;
    languagePath += ".json";

    if (!bc_file_exists(languagePath.c_str())) {
        // fallback language - English
        languagePath = fallbackLanguagePath;
    }

    std::ifstream langstream(bc_file_absolute_path(languagePath.c_str()));
    languageJson = json::parse(langstream);

    std::ifstream fallbacklangstream(bc_file_absolute_path(fallbackLanguagePath.c_str()));
    fallbackLanguageJson = json::parse(fallbacklangstream);

    free(settings);
}
