////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
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
 
 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __PatchSelectorPatch_hpp__
#define __PatchSelectorPatch_hpp__

#include "StompBox.h"
#include "ApplicationSettings.h"
#include "PatchRegistry.h"
#include "PatchController.h"
#include "owlcontrol.h"

class PatchSelectorPatch : public Patch {
private:
  int count;
  int banksize;
  int current;
  LedPin slot;
  int index;
public:
  PatchSelectorPatch(){
    if(patches.getActiveSlot() == RED){
      slot = GREEN;
      index = settings.patch_red;
    }else{
      slot = RED;
      index = settings.patch_green;
    }
    count = registry.getNumberOfPatches();
    registerParameter(PARAMETER_A, "Bank");
    registerParameter(PARAMETER_B, "Patch");
  }

  void processAudio(AudioBuffer &buffer){
    int banksize = count / 5 + 1;
    int bank = getParameterValue(PARAMETER_A) * 5;
    int patch = getParameterValue(PARAMETER_B) * banksize;
    patch += bank * banksize;
    if(patch == index) // skip selector patch
      patch++;
    if(patch >= count)
      patch = count-1;
    if(slot == RED)
      settings.patch_red = patch;
    else if(slot == GREEN)
      settings.patch_green = patch;
  }
};

#endif // __PatchSelectorPatch_hpp__
