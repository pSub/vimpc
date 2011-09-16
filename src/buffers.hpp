/*
   Vimpc
   Copyright (C) 2010 Nathan Sweetman

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   buffers.hpp - global access to important buffers (playlist, library, console)
   */

#ifndef __MAIN__BUFFERS
#define __MAIN__BUFFERS

#include <iostream>

#include "window/console.hpp"
#include "buffer/browse.hpp"
#include "buffer/library.hpp"
#include "buffer/list.hpp"
#include "buffer/playlist.hpp"

namespace Main
{
   Mpc::Playlist & Playlist();
   Mpc::Playlist & PlaylistPasteBuffer();
   Mpc::Browse   & Browse();
   Mpc::Library  & Library();
   Mpc::Lists    & Lists();
   Ui::Console   & Console();
}

#endif
