#include "OSXUtils.h"
#import <Cocoa/Cocoa.h>

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetMenuBarVisible(bool visible)
{
  if(visible)
  {
    [[NSApplication sharedApplication]
      setPresentationOptions:   NSApplicationPresentationDefault];
  }
  else
  {
    [[NSApplication sharedApplication]
      setPresentationOptions:   NSApplicationPresentationHideMenuBar |
                                NSApplicationPresentationHideDock];
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
QString OSXUtils::ComputerName()
{
  return QString::fromNSString([[NSHost currentHost] localizedName]);
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::HideMainWindow()
{
  [[NSApplication sharedApplication] hide:nil];
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::ShowMainWindow()
{
  [[NSApplication sharedApplication] unhide:nil];
}

/////////////////////////////////////////////////////////////////////////////////////////
bool OSXUtils::IsWindowHidden()
{
  return [NSApplication sharedApplication].hidden;
}
