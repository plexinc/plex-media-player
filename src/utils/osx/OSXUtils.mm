#include "OSXUtils.h"
#include "QsLog.h"
#include "ui/KonvergoWindow.h"
#include <QTimer>
#import <Cocoa/Cocoa.h>

/////////////////////////////////////////////////////////////////////////////////////////
unsigned long OSXUtils::GetPresentationOptionsForFullscreen(bool hideMenuAndDock)
{
  unsigned long flags = 0;
  if (hideMenuAndDock)
  {
    flags = flags & ~(NSApplicationPresentationAutoHideDock | NSApplicationPresentationAutoHideMenuBar);
    flags |= NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar;
  }
  else
  {
    flags = flags & ~(NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar);
    flags |= NSApplicationPresentationAutoHideDock | NSApplicationPresentationAutoHideMenuBar;
  }

  return flags;
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetPresentationOptions(unsigned long flags)
{
  QLOG_DEBUG() << "Setting presentationOptions =" << flags;
  [[NSApplication sharedApplication] setPresentationOptions:flags];
}

/////////////////////////////////////////////////////////////////////////////////////////
unsigned long OSXUtils::GetPresentationOptions()
{
  unsigned long options = [[NSApplication sharedApplication] presentationOptions];
  QLOG_DEBUG() << "Getting presentationOptions =" << options;
  return options;
}

/////////////////////////////////////////////////////////////////////////////////////////
QString OSXUtils::ComputerName()
{
  return QString::fromNSString([[NSHost currentHost] localizedName]);
}

/////////////////////////////////////////////////////////////////////////////////////////
OSStatus OSXUtils::SendAppleEventToSystemProcess(AEEventID eventToSendID)
{
  AEAddressDesc targetDesc;
  static const  ProcessSerialNumber kPSNOfSystemProcess = {0, kSystemProcess };
  AppleEvent    eventReply  = {typeNull, nullptr};
  AppleEvent    eventToSend = {typeNull, nullptr};

  OSStatus status = AECreateDesc(typeProcessSerialNumber,
    &kPSNOfSystemProcess, sizeof(kPSNOfSystemProcess), &targetDesc);

  if (status != noErr)
    return status;

  status = AECreateAppleEvent(kCoreEventClass, eventToSendID,
    &targetDesc, kAutoGenerateReturnID, kAnyTransactionID, &eventToSend);
  AEDisposeDesc(&targetDesc);

  if (status != noErr)
    return status;

  status = AESendMessage(&eventToSend, &eventReply, kAENormalPriority, kAEDefaultTimeout);
  AEDisposeDesc(&eventToSend);

  if (status != noErr)
    return status;

  AEDisposeDesc(&eventReply);

  return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetCursorVisible(bool visible)
{
  if (visible)
    [NSCursor unhide];
  else
    [NSCursor hide];
}

/////////////////////////////////////////////////////////////////////////////////////////
static NSWindow * qtWindowToCocoaWindow(QWindow *qtWindow)
{
  if (!qtWindow)
    return nil;

  NSView* cocoaView = (NSView*)qtWindow->winId();

  if (!cocoaView)
    return nil;

  return cocoaView.window;
}

/////////////////////////////////////////////////////////////////////////////////////////
static bool isWindowFullScreen(NSWindow *cocoaWindow)
{
  return cocoaWindow.styleMask & NSFullScreenWindowMask;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool OSXUtils::isWindowFullScreen(QWindow *qtWindow)
{
  NSWindow* cocoaWindow = qtWindowToCocoaWindow(qtWindow);

  if (!cocoaWindow)
    return false;

  return isWindowFullScreen(cocoaWindow);
}

/////////////////////////////////////////////////////////////////////////////////////////
static void SetWindowFullScreen(NSWindow *cocoaWindow, bool fullScreen)
{
  if (!cocoaWindow || fullScreen == isWindowFullScreen(cocoaWindow))
    return;

  [cocoaWindow toggleFullScreen:nil];
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetWindowFullScreen(QWindow *qtWindow, bool fullScreen)
{
  SetWindowFullScreen(qtWindowToCocoaWindow(qtWindow), fullScreen);
}

/////////////////////////////////////////////////////////////////////////////////////////
static void MoveWindowWithinScreen(NSWindow *cocoaWindow, QRect screenRect)
{
  if (!cocoaWindow)
    return;

  int padding = 50;
  [cocoaWindow setFrame:NSMakeRect(screenRect.left() + padding, screenRect.top() + padding,
                                   screenRect.width() - padding*2, screenRect.height() - padding*2) display:YES animate:NO];
}

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetWindowFullScreenOnSpecificScreen(QWindow *qtWindow, QRect screenRect)
{
  NSWindow* cocoaWindow = qtWindowToCocoaWindow(qtWindow);

  if (!cocoaWindow)
    return;

  if (isWindowFullScreen(cocoaWindow)) {
    SetWindowFullScreen(cocoaWindow, false);

    // Need to wait for the full screen animation to finish
    QTimer::singleShot(1000, [=] ()
    {
      MoveWindowWithinScreen(cocoaWindow, screenRect);
      SetWindowFullScreen(cocoaWindow, true);
    });
  } else {
    MoveWindowWithinScreen(cocoaWindow, screenRect);
    SetWindowFullScreen(cocoaWindow, true);
  }
}
