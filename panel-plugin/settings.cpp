/*
 * Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settings.h"

#include <algorithm>

extern "C"
{
#include <libxfce4util/libxfce4util.h>
}

using namespace WhiskerMenu;

//-----------------------------------------------------------------------------

Settings* WhiskerMenu::wm_settings = NULL;

//-----------------------------------------------------------------------------

static void read_vector_entry(XfceRc* rc, const char* key, std::vector<std::string>& desktop_ids)
{
	if (!xfce_rc_has_entry(rc, key))
	{
		return;
	}

	desktop_ids.clear();

	gchar** values = xfce_rc_read_list_entry(rc, key, ",");
	for (size_t i = 0; values[i] != NULL; ++i)
	{
		std::string desktop_id(values[i]);
		if (std::find(desktop_ids.begin(), desktop_ids.end(), desktop_id) == desktop_ids.end())
		{
			desktop_ids.push_back(desktop_id);
		}
	}
	g_strfreev(values);
}

//-----------------------------------------------------------------------------

static void write_vector_entry(XfceRc* rc, const char* key, const std::vector<std::string>& desktop_ids)
{
	const std::vector<std::string>::size_type size = desktop_ids.size();
	gchar** values = g_new0(gchar*, size + 1);
	for (std::vector<std::string>::size_type i = 0; i < size; ++i)
	{
		values[i] = g_strdup(desktop_ids[i].c_str());
	}
	xfce_rc_write_list_entry(rc, key, values, ",");
	g_strfreev(values);
}

//-----------------------------------------------------------------------------

Settings::Settings() :
	button_icon_name("xfce4-whiskermenu"),
	button_title_visible(false),
	button_icon_visible(true),

	launcher_show_name(true),
	launcher_show_description(true),
	launcher_icon_size(IconSize::Small),

	category_hover_activate(false),
	category_icon_size(IconSize::Smaller),

	load_hierarchy(false),
	favorites_in_recent(true),

	display_recent(false),
	position_search_alternate(false),
	position_commands_alternate(false),

	command_settings("xfce4-settings-manager"),
	command_lockscreen("xflock4"),
	command_logout("xfce4-session-logout"),

	menu_width(400),
	menu_height(500)
{
	favorites.push_back("exo-terminal-emulator.desktop");
	favorites.push_back("exo-file-manager.desktop");
	favorites.push_back("exo-mail-reader.desktop");
	favorites.push_back("exo-web-browser.desktop");
}

//-----------------------------------------------------------------------------

Settings::~Settings()
{
}

//-----------------------------------------------------------------------------

void Settings::load(char* file)
{
	if (!file)
	{
		return;
	}

	XfceRc* rc = xfce_rc_simple_open(file, true);
	g_free(file);
	if (!rc)
	{
		return;
	}

	read_vector_entry(rc, "favorites", favorites);
	read_vector_entry(rc, "recent", recent);

	button_title = xfce_rc_read_entry(rc, "button-title", button_title.c_str());
	button_icon_name = xfce_rc_read_entry(rc, "button-icon", button_icon_name.c_str());
	button_title_visible = xfce_rc_read_bool_entry(rc, "show-button-title", button_title_visible);
	button_icon_visible = xfce_rc_read_bool_entry(rc, "show-button-icon", button_icon_visible);

	launcher_show_name = xfce_rc_read_bool_entry(rc, "launcher-show-name", launcher_show_name);
	launcher_show_description = xfce_rc_read_bool_entry(rc, "launcher-show-description", launcher_show_description);
	launcher_icon_size = xfce_rc_read_int_entry(rc, "item-icon-size", launcher_icon_size);

	category_hover_activate = xfce_rc_read_bool_entry(rc, "hover-switch-category", category_hover_activate);
	category_icon_size = xfce_rc_read_int_entry(rc, "category-icon-size", category_icon_size);

	load_hierarchy = xfce_rc_read_bool_entry(rc, "load-hierarchy", load_hierarchy);
	favorites_in_recent = xfce_rc_read_bool_entry(rc, "favorites-in-recent", favorites_in_recent);

	display_recent = xfce_rc_read_bool_entry(rc, "display-recent-default", display_recent);
	position_search_alternate = xfce_rc_read_bool_entry(rc, "position-search-alternate", position_search_alternate);
	position_commands_alternate = xfce_rc_read_bool_entry(rc, "position-commands-alternate", position_commands_alternate) && position_search_alternate;

	command_settings = xfce_rc_read_entry(rc, "command-settings", command_settings.c_str());
	command_lockscreen = xfce_rc_read_entry(rc, "command-lockscreen", command_lockscreen.c_str());
	command_logout = xfce_rc_read_entry(rc, "command-logout", command_logout.c_str());

	menu_width = std::max(300, xfce_rc_read_int_entry(rc, "menu-width", menu_width));
	menu_height = std::max(400, xfce_rc_read_int_entry(rc, "menu-height", menu_height));

	xfce_rc_close(rc);
}

//-----------------------------------------------------------------------------

void Settings::save(char* file)
{
	if (!file)
	{
		return;
	}

	XfceRc* rc = xfce_rc_simple_open(file, false);
	g_free(file);
	if (!rc)
	{
		return;
	}

	write_vector_entry(rc, "favorites", favorites);
	write_vector_entry(rc, "recent", recent);

	xfce_rc_write_entry(rc, "button-title", button_title.c_str());
	xfce_rc_write_entry(rc, "button-icon", button_icon_name.c_str());
	xfce_rc_write_bool_entry(rc, "show-button-title", button_title_visible);
	xfce_rc_write_bool_entry(rc, "show-button-icon", button_icon_visible);

	xfce_rc_write_bool_entry(rc, "launcher-show-name", launcher_show_name);
	xfce_rc_write_bool_entry(rc, "launcher-show-description", launcher_show_description);
	xfce_rc_write_int_entry(rc, "item-icon-size", launcher_icon_size);

	xfce_rc_write_bool_entry(rc, "hover-switch-category", category_hover_activate);
	xfce_rc_write_int_entry(rc, "category-icon-size", category_icon_size);

	xfce_rc_write_bool_entry(rc, "load-hierarchy", load_hierarchy);
	xfce_rc_write_bool_entry(rc, "favorites-in-recent", favorites_in_recent);

	xfce_rc_write_bool_entry(rc, "display-recent-default", display_recent);
	xfce_rc_write_bool_entry(rc, "position-search-alternate", position_search_alternate);
	xfce_rc_write_bool_entry(rc, "position-commands-alternate", position_commands_alternate);

	xfce_rc_write_entry(rc, "command-settings", command_settings.c_str());
	xfce_rc_write_entry(rc, "command-lockscreen", command_lockscreen.c_str());
	xfce_rc_write_entry(rc, "command-logout", command_logout.c_str());

	xfce_rc_write_int_entry(rc, "menu-width", menu_width);
	xfce_rc_write_int_entry(rc, "menu-height", menu_height);

	xfce_rc_close(rc);
}

//-----------------------------------------------------------------------------