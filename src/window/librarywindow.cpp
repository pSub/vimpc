/*
   Vimpc
   Copyright (C) 2010 - 2011 Nathan Sweetman

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

   librarywindow.cpp - handling of the mpd music library
   */

#include "librarywindow.hpp"

#include "buffers.hpp"
#include "colour.hpp"
#include "mpdclient.hpp"
#include "screen.hpp"
#include "settings.hpp"
#include "buffers.hpp"
#include "mode/search.hpp"

#include <algorithm>
#include <pcrecpp.h>

using namespace Ui;

LibraryWindow::LibraryWindow(Main::Settings const & settings, Ui::Screen const & screen, Mpc::Client & client, Ui::Search const & search) :
   SelectWindow     (screen),
   settings_        (settings),
   client_          (client),
   search_          (search),
   library_         (Main::Library())
{
}

LibraryWindow::~LibraryWindow()
{
   Clear();
}


void LibraryWindow::Redraw()
{
   Clear();
   client_.ForEachLibrarySong(library_, &Mpc::Library::Add);

   library_.Sort();
}



std::string LibraryWindow::SearchPattern(int32_t id)
{
   //! \todo add a search that searches in collapsed songs and
   //! expands things as necessary
   std::string pattern("");

   Mpc::LibraryEntry const * const entry = library_.Get(id);

   switch (entry->type_)
   {
      case Mpc::ArtistType:
         pattern = entry->artist_;
         break;

      case Mpc::AlbumType:
         pattern = entry->album_;
         break;

      case Mpc::SongType:
         pattern = entry->song_->Title();
         break;

      default:
         ASSERT(false);
         break;
   }

   return pattern;
}


void LibraryWindow::Clear()
{
   library_.Clear();
}

void LibraryWindow::Print(uint32_t line) const
{
   std::string const BlankLine(screen_.MaxColumns(), ' ');

   WINDOW * window = N_WINDOW();

   if ((line + FirstLine()) < BufferSize())
   {
      uint32_t printLine = (line + FirstLine());

      int colour = DetermineSongColour(library_.Get(printLine));

      if (printLine == CurrentLine())
      {
         if (settings_.ColourEnabled() == true)
         {
            wattron(window, COLOR_PAIR(colour));
         }

         wattron(window, A_REVERSE);
      }

      mvwprintw(window, line, 0, BlankLine.c_str());

      if ((library_.Get(printLine)->type_ == Mpc::AlbumType) || (library_.Get(printLine)->type_ == Mpc::ArtistType))
      {
         uint8_t expandCol = 0;

         if (library_.Get(printLine)->type_ == Mpc::AlbumType)
         {
            mvwprintw(window, line, 1, "|--");
            expandCol = 4;
         }

         mvwprintw(window, line, expandCol, "[ ]");

         if ((settings_.ColourEnabled() == true) && (printLine != CurrentLine()))
         {
            wattron(window, COLOR_PAIR(REDONDEFAULT));
         }

         char expand = (library_.Get(printLine)->expanded_ == true) ? '-' : '+';
         mvwprintw(window, line, expandCol + 1, "%c", expand);

         if ((settings_.ColourEnabled() == true) && (printLine != CurrentLine()))
         {
            wattroff(window, COLOR_PAIR(REDONDEFAULT));
         }

         if (settings_.ColourEnabled() == true)
         {
            wattron(window, COLOR_PAIR(colour));
         }

         wmove(window, line, expandCol + 4);

         if (library_.Get(printLine)->type_ == Mpc::ArtistType)
         {
            waddstr(window, library_.Get(printLine)->artist_.c_str());
         }
         else if (library_.Get(printLine)->type_ == Mpc::AlbumType)
         {
            waddstr(window, library_.Get(printLine)->album_.c_str());
         }

         if (settings_.ColourEnabled() == true)
         {
            wattroff(window, COLOR_PAIR(colour));
         }
      }
      else if ((library_.Get(printLine)->type_ == Mpc::SongType) && (library_.Get(printLine)->song_ != NULL))
      {
         mvwprintw(window, line, 1, "|   |--");

         wprintw(window, "[");

         if ((settings_.ColourEnabled() == true) && (printLine != CurrentLine()))
         {
            wattron(window, COLOR_PAIR(REDONDEFAULT));
         }

         wprintw(window, "%2s" , library_.Get(printLine)->song_->Track().c_str());

         if ((settings_.ColourEnabled() == true) && (printLine != CurrentLine()))
         {
            wattroff(window, COLOR_PAIR(REDONDEFAULT));
         }

         wprintw(window, "]");
         waddstr(window, " ");

         if (settings_.ColourEnabled() == true)
         {
            wattron(window, COLOR_PAIR(colour));
         }

         std::string title = library_.Get(printLine)->song_->Title();

         if (title == "Unknown")
         {
            title = library_.Get(printLine)->song_->URI();
         }

         if (title.length() >= 48)
         {
            title  = title.substr(0, 45);
            title += "...";
         }

         waddstr(window, title.c_str());

         mvwprintw(window, line, 61, " [");
         waddstr(window, library_.Get(printLine)->song_->DurationString().c_str());
         waddstr(window, "]");
      }

      if (settings_.ColourEnabled() == true)
      {
         wattroff(window, COLOR_PAIR(colour));
      }

      wattroff(window, A_REVERSE);
   }
}

void LibraryWindow::Left(Ui::Player & player, uint32_t count)
{
   if (CurrentLine() < library_.Size())
   {
      Mpc::LibraryEntry * const parent = library_.Get(CurrentLine())->parent_;

      bool scrolled = false;

      if ((library_.Get(CurrentLine())->expanded_ == true) && (library_.Get(CurrentLine())->type_ != Mpc::SongType))
      {
         scrolled = true;
      }

      library_.Collapse(CurrentLine());

      if ((parent == NULL) && (scrolled == false))
      {
         Scroll(-1);
      }
      else if (scrolled == false)
      {
         uint32_t parentLine = 0;
         for (; (parentLine < library_.Size()) && (library_.Get(parentLine) != parent); ++parentLine);

         ScrollTo(parentLine);
      }
   }
}

void LibraryWindow::Right(Ui::Player & player, uint32_t count)
{
   if (CurrentLine() < library_.Size())
   {
      library_.Expand(CurrentLine());
      Scroll(1);
   }
}

void LibraryWindow::Confirm()
{
   client_.Clear();
   Main::Playlist().Clear();

   library_.AddToPlaylist(Mpc::Song::Single, client_, CurrentLine());
   client_.Play(0);
}

int32_t LibraryWindow::DetermineSongColour(Mpc::LibraryEntry const * const entry) const
{
   int32_t colour = Colour::Song;

   if ((entry->song_ != NULL) && (entry->song_->URI() == client_.GetCurrentSongURI()))
   {
      colour = Colour::CurrentSong;
   }
   else if ((search_.LastSearchString() != "") && (settings_.HightlightSearch() == true))
   {
      pcrecpp::RE expression(".*" + search_.LastSearchString() + ".*");

      if (((entry->type_ == Mpc::ArtistType) && (expression.FullMatch(entry->artist_) == true)) ||
          ((entry->type_ == Mpc::AlbumType)  && (expression.FullMatch(entry->album_) == true)) ||
          ((entry->type_ == Mpc::SongType)   && (expression.FullMatch(entry->song_->Title()) == true)))
      {
         colour = Colour::SongMatch;
      }
   }
   else if ((entry->type_ == Mpc::SongType) && (entry->song_ != NULL) && (entry->song_->Reference() > 0))
   {
      colour = Colour::FullAdd;
   }
   else if ((entry->children_.size() >= 1) && (entry->childrenInPlaylist_ == entry->children_.size()))
   {
      colour = Colour::FullAdd;
   }
   else if ((entry->children_.size() >= 1) && (entry->partial_ > 0))
   {
      colour = Colour::PartialAdd;
   }

   return colour;
}
