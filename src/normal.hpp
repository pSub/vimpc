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

   normal.hpp - normal mode input handling 
   */

#ifndef __UI__NORMAL
#define __UI__NORMAL

#include <map>

#include "handler.hpp"
#include "modewindow.hpp"
#include "mpdclient.hpp"
#include "player.hpp"
#include "playlist.hpp"
#include "search.hpp"
#include "screen.hpp"

namespace Main
{
   class Vimpc;
   class Settings;
}

namespace Ui
{
   // Handles all input received whilst in normal mode
   class Normal : public Handler, public Player
   {
   public:
      Normal(Ui::Screen & screen, Mpc::Client & client, Main::Settings & settings, Ui::Search & search);
      ~Normal();

   public: // Ui::Handler
      void InitialiseMode(int input);
      void FinaliseMode(int input);
      bool Handle(int input);
      bool CausesModeToStart(int input) const;

   private: //Player wrapper functions
      bool ClearScreen(uint32_t count) { return Player::ClearScreen(); }
      bool Pause(uint32_t count)       { return Player::Pause(); }
      bool Random(uint32_t count)      { return Player::Random(true); }
      bool Stop(uint32_t count)        { return Player::Stop(); }

   private:
      bool Confirm(uint32_t count);
      bool RepeatLastAction(uint32_t count);

   private: //Selecting
      template <Window::Position POSITION>
      bool Select(uint32_t count); 

   private: //Searching
      template <Search::Skip SKIP>
      bool SearchResult(uint32_t count); 

   private: //Skipping
      template <Player::Skip SKIP>
      bool SkipSong(uint32_t count); 

      template <Player::Skip SKIP>
      bool SkipArtist(uint32_t count); 

   private: //Scrolling
      template <Screen::Size SIZE, Screen::Direction DIRECTION>
      bool Scroll(uint32_t count);
      
      template <Screen::Location LOCATION>
      bool ScrollTo(uint32_t line);

   private:
      ModeWindow *  window_;
      uint32_t      actionCount_;
      int32_t       lastAction_;
      uint32_t      lastActionCount_;
      bool          wasSpecificCount_;

      typedef bool (Ui::Normal::*ptrToMember)(uint32_t);
      typedef std::map<int, ptrToMember> ActionTable;
      ActionTable   actionTable_;

      Ui::Search        & search_;
      Mpc::Client       & client_;
      Ui::Screen        & screen_;
      Main::Settings    & settings_;

   };

   // \todo this should be implemented using the window
   // somehow
   //Implementation of selecting functions
   template <Window::Position POSITION>
   bool Normal::Select(uint32_t count)
   {
      screen_.Select(POSITION, count);
      return true;
   }


   //Implementation of skipping functions
   template <Ui::Player::Skip SKIP>
   bool Normal::SkipSong(uint32_t count)
   {
      return Player::SkipSong(SKIP, count);
   }

   template <Ui::Player::Skip SKIP>
   bool Normal::SkipArtist(uint32_t count)
   {
      return Player::SkipArtist(SKIP, count);
   }


   //Implementation of searching functions
   template <Ui::Search::Skip SKIP>
   bool Normal::SearchResult(uint32_t count)
   {
      return search_.SearchResult(SKIP, count);
   }


   // Implementation of scrolling functions
   template <Screen::Size SIZE, Screen::Direction DIRECTION>
   bool Normal::Scroll(uint32_t count)
   {
      screen_.Scroll(SIZE, DIRECTION, count);
      return true;
   }

   template <Screen::Location LOCATION>
   bool Normal::ScrollTo(uint32_t line)
   {
      if ((LOCATION == Screen::Specific) && (wasSpecificCount_ == false))
      {
         screen_.ScrollTo(Screen::Bottom);
      }
      else
      {
         screen_.ScrollTo(LOCATION, line);
      }
      return true;
   }

}

#endif
