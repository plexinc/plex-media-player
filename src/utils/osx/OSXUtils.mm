#include "OSXUtils.h"
#include "QsLog.h"
#include "ui/KonvergoWindow.h"
#include "core/Globals.h"
#import <Cocoa/Cocoa.h>

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
@interface PMPWindowRestoration : NSObject
+ (void)restoreWindowWithIdentifier:(NSUserInterfaceItemIdentifier)identifier
                              state:(NSCoder *)state
                  completionHandler:(void (^)(NSWindow *, NSError *))completionHandler;
@end

@implementation PMPWindowRestoration

+ (void)restoreWindowWithIdentifier:(NSString *)identifier state:(NSCoder *)state completionHandler:(void (^)(NSWindow *, NSError *))completionHandler
{
  KonvergoWindow* window = Globals::MainWindow();
  NSView* view = (NSView*)window->winId();
  completionHandler(view.window, nil);
}

@end

/////////////////////////////////////////////////////////////////////////////////////////
void OSXUtils::SetWindowRestoration(KonvergoWindow* window)
{
  NSView* view = (NSView*)window->winId();
  NSWindow* win = view.window;
  win.restorable = YES;
  win.restorationClass = [PMPWindowRestoration class];
}
