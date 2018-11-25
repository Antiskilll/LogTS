/*
 * TeamSpeak 3 G-key plugin
 * Author: Jules Blok (jules@aerix.nl), POQDavid
 *
 * Copyright (c) 2010-2012 Jules Blok
 * Copyright (c) 2017 POQDavid
 * Copyright (c) 2008-2012 TeamSpeak Systems GmbH
 */

#include "ts3_settings.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"
#include "plugin.h"
#include "sqlite3.h"

#include <sstream>
#include <string>

TS3Settings::TS3Settings(void)
{
}

TS3Settings::~TS3Settings(void)
{
	CloseDatabase();
}

bool TS3Settings::CheckAndLog(int returnCode)
{
	if(returnCode != SQLITE_OK)
	{
		ts3Functions.logMessage(sqlite3_errmsg(settings), LogLevel_ERROR, "G-Key Plugin", 0);
		return true;
	}
	return false;
}

bool TS3Settings::OpenDatabase(std::string path)
{
	if(settings != NULL) CloseDatabase();

	if(CheckAndLog(sqlite3_open(path.c_str(), &settings)))
	{
		CloseDatabase();
		return false;
	}

	return true;
}

void TS3Settings::CloseDatabase()
{
	sqlite3_close(settings);
}

bool TS3Settings::GetValueFromQuery(std::string query, std::string& result)
{
	// Prepare the statement
	sqlite3_stmt* sql;
	if(CheckAndLog(sqlite3_prepare_v2(settings, query.c_str(), (int)query.length(), &sql, NULL)))
		return false;

	// Get the value
	if(sqlite3_step(sql) != SQLITE_ROW) return false;
	if(sqlite3_column_count(sql) != 1) return false;
	if(sqlite3_column_type(sql, 0) != SQLITE_TEXT) return false;
	result = std::string(reinterpret_cast<const char*>(
		sqlite3_column_text(sql, 0)
	));

	// Finalize the statement
	if(CheckAndLog(sqlite3_finalize(sql))) return false;

	return true;
}

bool TS3Settings::GetValuesFromQuery(std::string query, std::vector<std::string>& result)
{
	// Prepare the statement
	sqlite3_stmt* sql;
	if(CheckAndLog(sqlite3_prepare_v2(settings, query.c_str(), (int)query.length(), &sql, NULL)))
		return false;

	// Get the values
	if(sqlite3_step(sql) != SQLITE_ROW) return false;
	if(sqlite3_column_count(sql) != 1) return false;
	if(sqlite3_column_type(sql, 0) != SQLITE_TEXT) return false;
	do
	{
		result.push_back(std::string(reinterpret_cast<const char*>(
			sqlite3_column_text(sql, 0)
		)));
	}
	while(sqlite3_step(sql) == SQLITE_ROW);

	// Finalize the statement
	if(CheckAndLog(sqlite3_finalize(sql))) return false;

	return true;
}

std::string TS3Settings::GetValueFromData(std::string data, std::string key)
{
	std::string result;
	std::stringstream ss(data);
	bool found = false;
	while(!ss.eof() && !found)
	{
		getline(ss, result, '=');
		found = result == key;
		getline(ss, result);
	}
	return found?result:std::string();
}

bool TS3Settings::GetIconPack(std::string& result)
{
	return GetValueFromQuery("SELECT value FROM Application WHERE key='IconPack'", result);
}

bool TS3Settings::GetSoundPack(std::string& result)
{
	return GetValueFromQuery("SELECT value FROM Notifications WHERE key='SoundPack'", result);
}

bool TS3Settings::GetPreProcessorData(std::string profile, std::string& result)
{
	std::stringstream ss;
	ss << "SELECT value FROM Profiles WHERE key='Capture/" << profile << "/PreProcessing'";
	return GetValueFromQuery(ss.str(), result);
}

bool TS3Settings::GetEnabledPlugins(std::vector<std::string>& result)
{
	return GetValuesFromQuery("SELECT key FROM Plugins WHERE value='true'", result);
}
